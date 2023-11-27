#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

typedef struct {
    int rows;
    int cols;
    unsigned char *cells;
} Map;

Map* initMap(const char* filename) {
    FILE* file = fopen(filename, "r");
    if (file == NULL) {
        fprintf(stderr, "Error opening file: %s\n", filename);
        exit(EXIT_FAILURE);
    }

    Map* map = (Map*)malloc(sizeof(Map));
    if (map == NULL) {
        fprintf(stderr, "Memory allocation failed\n");
        fclose(file);
        exit(EXIT_FAILURE);
    }

    // Read rows and cols from the file
    fscanf(file, "%d %d", &map->rows, &map->cols);

    // Allocate memory for cells
    map->cells = (unsigned char*)malloc(map->rows * map->cols * sizeof(unsigned char));
    if (map->cells == NULL) {
        fprintf(stderr, "Memory allocation failed\n");
        fclose(file);
        free(map);
        exit(EXIT_FAILURE);
    }

    // Read cell values from the file
    for (int i = 0; i < map->rows; ++i) {
        for (int j = 0; j < map->cols; ++j) {
            fscanf(file, "%hhu", &map->cells[i * map->cols + j]);
        }
    }

    fclose(file);

    return map;
}

void releaseMap(Map* map) {
    free(map->cells);
    free(map);
}

bool isBorder(Map* map, int r, int c, int border) {
    if (r < 0 || r >= map->rows || c < 0 || c >= map->cols) {
        // Coordinates are out of bounds
        return false;
    }

    unsigned char cellValue = map->cells[r * map->cols + c];

    switch (border) {
        // Left diagonal border
        case 0:
            return (cellValue & 1) != 0;
        // Right diagonal border
        case 1:
            return (cellValue & 2) != 0;
        // Top or bottom border
        case 2:
            return (cellValue & 4) != 0;
        // Invalid border value
        default:
            return false;
    }
}

// Print location and adjust it to 1-based indexing
void printLocation(int r, int c) {
    printf("%d,%d\n", r + 1, c + 1);
}

// Determine if the triangle is at the top or bottom based on its position in the grid
int triangleType(int row, int col) {
    if ((row + col) % 2 == 0) {
      	// Top sided
        return -1;
    } else {
    	// Bottom sided
        return 1;
    }
}

int turn(int r, int c, int border) {
	if (border == 0) {
		if (triangleType(r, c) == -1) {
			// move top
			return 2;
		} else {
			// move right
			return 1;
		}
	} else if (border == 1) {
		if (triangleType(r, c) == -1) {
			// move left
			return 2;
		} else {
			// move bottom
			return 2;
		}
	}

	if (border == 2) {
		if (triangleType(r, c) == -1) {
			// move right
			return 1;
		}
	}

	return 0;
}

void findPath(Map* map, int r, int c, int rule) {
    // int currentBorder = startBorder(map, currentR, currentC, rule);
    int border = 1;

    // @todo remove from prod.
    printf("rule %d\n", rule);

    // Print the starting coordinate and initial direction
    printLocation(r, c);

	// @todo remove from prod
	int oldBorder;

    // @todo use checkBorder
    while (1) {
    	printf("isBorder %d: %d\n", border, isBorder(map, r, c, border));

    	if (isBorder(map, r, c, border) == true) {
    		oldBorder = border;

			border = turn(r, c, border);

    		printf("border hit, turn from %d to %d\n", oldBorder, border);
    	} else {
    		// Step
			if (border == 0) {
				// Step left
				c--;
				printf("Step left\n");
			} else if (border == 1) {
				// Step right
				c++;
				printf("Step right\n");
			} else if (border == 2) {
				int type = triangleType(r, c);
				r+= type;
				printf("Step top or bottom %d + turn\n", type);

				border = turn(r, c, border);
			}
    	}

        // Print the current coordinate
        printLocation(r, c);

        // Check if we have reached the exit
        // @todo extract to isOut function
        if (r < 0 || r >= map->rows || c < 0 || c >= map->cols) {
            break;
        }

        // Determine the next border based on the rule
        // currentBorder = startBorder(map, r, c, rule);
    }
}

int main(int argc, char* argv[]) {
    // Validate input arguments
    if (argc != 5) {
        fprintf(stderr, "Usage: %s [--lpath|--rpath] <row> <col> <filename>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    // Initialize input variables
    int rule;

    if (strcmp(argv[1], "--rpath") == 0) {
        rule = 0;
    } else if (strcmp(argv[1], "--lpath") == 0) {
        rule = 1;
    } else {
        fprintf(stderr, "Unsupported rule %s\n", argv[1]);
        exit(EXIT_FAILURE);
    }

    int row = atoi(argv[2]) - 1;  // Adjust to 0-based indexing
    int col = atoi(argv[3]) - 1;  // Adjust to 0-based indexing

    const char* filename = argv[4];

    // Initialize the map from the filename
    Map* map = initMap(filename);

    // Find and print the path
    findPath(map, row, col, rule);

    // Release memory
    releaseMap(map);

    return 0;
}

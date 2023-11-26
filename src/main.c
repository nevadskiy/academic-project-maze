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

int startBorder(Map* map, int r, int c, int rule) {
    if (r < 0 || r >= map->rows || c < 0 || c >= map->cols) {
        // Coordinates are out of bounds
        return -1;
    }

    unsigned char cellValue = map->cells[r * map->cols + c];

	printf("Cell %d,%d has value %d\n", r + 1, c + 1, cellValue);

	// Left-hand rule
	if (rule == -1) {
		if (cellValue & 1) {
			// Left diagonal border is missing
			return 0;
		}

		if ((cellValue >> 1) & 1) {
			// Right diagonal border is missing
			return 1;
		}

		if ((cellValue >> 2) & 1) {
			// Top / bottom border is missing
			return 0;
		}
	}

	// Right-hand rule
    if (rule == 1) {
        if ((c % 2 == 1 && r % 2 == 1) || (c % 2 == 0 && r % 2 == 0)) {
            if ((cellValue & 1) == 0) {
                return 0;  // Follow the left diagonal border
            }
        }
        if ((cellValue & 2) == 0) {
            return 1;  // Follow the right edge of the skew
        }
        return 2;  // Follow the upper or lower limit
    } else {  // Left-hand rule
        if ((c % 2 == 1 && r % 2 == 1) || (c % 2 == 0 && r % 2 == 0)) {
            if ((cellValue & 1) == 0) {
                return 0;  // Follow the left diagonal border
            }
        }
        if ((cellValue & 4) == 0) {
            return 2;  // Follow the upper or lower limit
        }
        return 1;  // Follow the right edge of the skew
    }
}

void printCoordinate(int r, int c) {
	// Print coordinates and adjust to 1-based indexing
    printf("%d,%d\n", r + 1, c + 1);
}

void findPath(Map* map, int r, int c, int rule) {
    int currentR = r;
    int currentC = c;
    int currentBorder = startBorder(map, currentR, currentC, rule);

    // Print the starting coordinate and initial direction
    printCoordinate(currentR, currentC);

    while (1) {
        // Move to the next cell based on the current border
        switch (currentBorder) {
            case 0:  // Left diagonal border
                if (rule == 0) {
                    currentC--;
                } else {
                    currentR--;
                }
                break;
            case 1:  // Right edge of the skew
                if (rule == 0) {
                    currentC++;
                } else {
                    currentR--;
                }
                break;
            case 2:  // Upper or lower limit
                if (rule == 0) {
                    currentR++;
                } else {
                    currentC++;
                }
                break;
            default:
                break;
        }

        // Print the current coordinate
        printCoordinate(currentR, currentC);

        // Check if we have reached the exit
        if (currentR < 0 || currentR >= map->rows || currentC < 0 || currentC >= map->cols) {
            break;
        }

        // Determine the next border based on the rule
        currentBorder = startBorder(map, currentR, currentC, rule);
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
        rule = -1;
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

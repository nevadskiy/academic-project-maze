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

// Enum to represent the possible directions
enum Direction {
    TOP_LEFT,
    TOP,
    TOP_RIGHT,
    BOTTOM_LEFT,
    BOTTOM,
    BOTTOM_RIGHT
};

int borderByDirection(enum Direction direction) {
	// TOP_LEFT
	if (direction == 0) {
		// Left border
		return 0;
	}

	// TOP
	if (direction == 1) {
		// Left border
		return 0;
	}

	// TOP_RIGHT
	if (direction == 2) {
		// Top/Bottom border
		return 2;
	}

	// BOTTOM_LEFT
	if (direction == 3) {
		// Top/Bottom border
		return 2;
	}

	// BOTTOM
	if (direction == 4) {
		// Right border
		return 1;
	}

	// BOTTOM_RIGHT
	// Right border
	return 1;
}

int turnUsingLeftHandRule(int row, int col, int border) {
	// inspecting left border
	if (border == 0) {
		if (triangleType(row, col) == -1) {
			// from left to top
			return 2;
		}

		// from left to right
		return 1;
	}

	// inspecting right border
	if (border == 1) {
		if (triangleType(row, col) == -1) {
			// from right to left
			return 0;
		}

		// from right to bottom
		return 2;
	}

	// inspecting top/bottom border
	if (border == 2) {
		if (triangleType(row, col) == -1) {
			// from top to right
			return 1;
		}
	}

	// from bottom to left
	return 0;
}

void escape(Map* map, int row, int col, int rule) {
	enum Direction direction = BOTTOM_RIGHT;

	printf("rule %d", rule);

	// @todo use DO while loop...
	// Moving loop
    while (1) {
    	int border;
		// @todo use DO while loop instead...
    	// Turning loop (consider using single loop)
		border = borderByDirection(direction);

    	while (1) {
			printf("Inspecting border %d at %dx%d: %d\n", border, row + 1, col + 1, isBorder(map, row, col, border));

			if (isBorder(map, row, col, border) == false) {
				break;
			}

			printf("Turning border from %d\n", border);

			border = turnUsingLeftHandRule(row, col, border);

			printf("Turning border to %d\n", border);
    	}

		// Step
		if (border == 0) {
			// Step left
			col -= 1;

			// Update direction
			if (triangleType(row, col) == -1) {
				printf("Step bottom left\n");
				direction = BOTTOM_LEFT;
			} else {
				printf("Step top left\n");
				direction = TOP_LEFT;
			}

		} else if (border == 1) {
			// Step right
			col += 1;

			// Update direction
			if (triangleType(row, col) == -1) {
				printf("Step bottom right\n");
				direction = BOTTOM_RIGHT;
			} else {
				printf("Step top right\n");
				direction = TOP_RIGHT;
			}
		} else if (border == 2) {
			int type = triangleType(row, col);
			row += type;

			// Update direction
			if (type == -1) {
				printf("Step top\n");
				direction = TOP;
			} else {
				printf("Step bottom\n");
				direction = BOTTOM;
			}
		}

        // Print the current coordinate
        printLocation(row, col);

        // Check if we have reached the exit
        // @todo extract to isOut function
        if (row < 0 || row >= map->rows || col < 0 || col >= map->cols) {
            break;
        }
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

    // Find the way out from the maze
    escape(map, row, col, rule);

    // Release memory
    releaseMap(map);

    return 0;
}

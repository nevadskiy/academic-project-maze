#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

// Define map structure
typedef struct {
    int rows;
    int cols;
    unsigned char *cells;
} Map;

// Build map from filename
Map* buildMap(const char* filename) {
	// Open map file
    FILE* file = fopen(filename, "r");
    if (file == NULL) {
        fprintf(stderr, "Error opening file: %s\n", filename);
        exit(EXIT_FAILURE);
    }

	// Allocate memory for map
    Map* map = (Map*)malloc(sizeof(Map));
    if (map == NULL) {
        fprintf(stderr, "Memory allocation failed\n");
        fclose(file);
        exit(EXIT_FAILURE);
    }

    // Initialize map size from file
    fscanf(file, "%d %d", &map->rows, &map->cols);

    // Allocate memory for cells
    map->cells = (unsigned char*)malloc(map->rows * map->cols * sizeof(unsigned char));
    if (map->cells == NULL) {
        fprintf(stderr, "Memory allocation failed\n");
        fclose(file);
        free(map);
        exit(EXIT_FAILURE);
    }

    // Initialize cells from map file
    for (int i = 0; i < map->rows; ++i) {
        for (int j = 0; j < map->cols; ++j) {
            fscanf(file, "%hhu", &map->cells[i * map->cols + j]);
        }
    }

	// Close map file
    fclose(file);

    return map;
}

void releaseMap(Map* map) {
    free(map->cells);
    free(map);
}

// Determine if location is outside map
bool isOutside(Map *map, int row, int col) {
	// Compare location with map size
	return row < 0 || row >= map->rows || col < 0 || col >= map->cols;
}

// Determine if border exists
bool isBorder(Map* map, int row, int col, int border) {
	// Get cell value from map
    unsigned char value = map->cells[row * map->cols + col];

    switch (border) {
        // Left diagonal border
        case 0:
        	// Inspect 1st LSB (least significant bit)
            return ((value >> 0) & 1) != 0;
        // Right diagonal border
        case 1:
        	// Inspect 2nd LSB (least significant bit)
            return ((value >> 1) & 1) != 0;
        // Top or bottom border
        case 2:
        	// Inspect 3rd LSB (least significant bit)
            return ((value >> 2) & 1) != 0;
        // Invalid border value
        default:
            return false;
    }
}

// Print location and adjust it to 1-based indexing
void printLocation(int row, int col) {
    printf("%d,%d\n", row + 1, col + 1);
}

// Determine if triangle is at top or bottom based on its position in grid
int triangleType(int row, int col) {
    if ((row + col) % 2 == 0) {
      	// Top sided
        return -1;
    } else {
    	// Bottom sided
        return 1;
    }
}

// Enum to represent possible directions
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

int turnByLeftHandRule(int row, int col, int border) {
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

void escapeMap(Map* map, int row, int col, int rule) {
	enum Direction direction = BOTTOM_RIGHT;

	printf("rule %d\n", rule);

	// @todo use DO while loop...
	// Moving loop
    while (1) {
		// Check if we have reached exit
		if (isOutside(map, row, col)) {
			break;
		}

		// Print current coordinate
		printLocation(row, col);

		int border = borderByDirection(direction);
		// printf("Setting starting border %d direction %d\n", border, direction);

    	// Turning loop (consider using single loop)
		// @todo use DO while loop instead...
    	while (1) {
			// printf("Inspecting border %d at %dx%d: %d\n", border, row + 1, col + 1, isBorder(map, row, col, border));

			if (isBorder(map, row, col, border) == false) {
				break;
			}

			// printf("Turning border from %d\n", border);

			border = turnByLeftHandRule(row, col, border);

			// printf("Turning border to %d\n", border);
    	}

		// Step
		if (border == 0) {
			// Update direction
			if (triangleType(row, col) == -1) {
				// printf("Step bottom left\n");
				direction = BOTTOM_LEFT;
			} else {
				// printf("Step top left\n");
				direction = TOP_LEFT;
			}

			// Step left
			col -= 1;
		} else if (border == 1) {
			// Update direction
			if (triangleType(row, col) == -1) {
				// printf("Step bottom right\n");
				direction = BOTTOM_RIGHT;
			} else {
				// printf("Step top right\n");
				direction = TOP_RIGHT;
			}

			// Step right
			col += 1;
		} else if (border == 2) {
			int type = triangleType(row, col);

			// Update direction
			if (type == -1) {
				// printf("Step top\n");
				direction = TOP;
			} else {
				// printf("Step bottom\n");
				direction = BOTTOM;
			}

			row += type;
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

    // Build map using filename
    Map* map = buildMap(filename);

    // Find way out from maze
    escapeMap(map, row, col, rule);

    // Release memory
    releaseMap(map);

    return 0;
}

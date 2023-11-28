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

    // Init map size from file
    fscanf(file, "%d %d", &map->rows, &map->cols);

    // Allocate memory for cells
    map->cells = (unsigned char*)malloc(map->rows * map->cols * sizeof(unsigned char));
    if (map->cells == NULL) {
        fprintf(stderr, "Memory allocation failed\n");
        fclose(file);
        free(map);
        exit(EXIT_FAILURE);
    }

    // Init cells from map file
    for (int i = 0; i < map->rows; ++i) {
        for (int j = 0; j < map->cols; ++j) {
            fscanf(file, "%hhu", &map->cells[i * map->cols + j]);
        }
    }

	// Close map file
    fclose(file);

    return map;
}

// Release map and free up memory
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
int getTriangle(int row, int col) {
    if ((row + col) % 2 == 0) {
      	// Top sided
        return -1;
    } else {
    	// Bottom sided
        return 1;
    }
}

// Enum with possible directions
enum Direction {
    TOP_LEFT,
    TOP,
    TOP_RIGHT,
    BOTTOM_LEFT,
    BOTTOM,
    BOTTOM_RIGHT
};

// Get border by moving direction with left hand rule
// @note can be simplified using math formula
int getBorderByDirectionWithLeftHandRule(enum Direction direction) {
	switch (direction) {
        case TOP_LEFT:
        case TOP:
            // Left border
            return 0;
        case TOP_RIGHT:
        case BOTTOM_LEFT:
            // Top / Bottom border
            return 2;
        case BOTTOM:
        case BOTTOM_RIGHT:
            // Right border
            return 1;
        default:
            // Unsupported direction
            return -1;
    }
}

// Get border by moving direction with right hand rule
// @note can be simplified using math formula
int getBorderByDirectionWithRightHandRule(enum Direction direction) {
	switch (direction) {
		case BOTTOM:
		case BOTTOM_LEFT:
			// Left border
			return 0;
        case TOP:
        case TOP_RIGHT:
            // Right border
            return 1;
        case TOP_LEFT:
        case BOTTOM_RIGHT:
            // Top / Bottom border
            return 2;
        default:
            // Unsupported direction
            return -1;
    }
}

/*
// Turn to next border by left hand rule (clockwise direction)
int turnByLeftHandRule(int row, int col, int border) {
	// Get triangle type (top sided or bottom sided) by location
    int triangle = triangle(row, col);

	 if (border == 0) {
	     // from left to top or left to right
	     return (triangle == -1) ? 2 : 1;
	 } else if (border == 1) {
	     // from right to left or right to bottom
	     return (triangle == -1) ? 0 : 2;
	 } else if (border == 2) {
	     // from top to right or bottom to left
	     return (triangle == -1) ? 1 : 0;
	 }
}

// Turn to next border by right hand rule (counter-clockwise direction)
int turnByRightHandRule(int row, int col, int border) {
	// Get triangle type (top sided or bottom sided) by location
    int triangle = triangle(row, col);

	 if (border == 0) {
	     // from left to top or left to right
	     return (triangle == -1) ? 1 : 2;
	 } else if (border == 1) {
	     // from right to left or right to bottom
	     return (triangle == -1) ? 2 : 0;
	 } else if (border == 2) {
	     // from top to right or bottom to left
	     return (triangle == -1) ? 0 : 1;
	 }
}
*/

// Use math to determine next border by location and hand rule
int turn(int row, int col, int border, int rule) {
	int triangle = getTriangle(row, col);

	return (3 + border + (rule * triangle)) % 3;
}

void escapeMap(Map* map, int row, int col, int rule) {
	// @todo calculate starting direction (subtask #3)
	enum Direction direction = BOTTOM_RIGHT;

	// Move while inside map
    while (isOutside(map, row, col) == false) {
		// Print current coordinate
		printLocation(row, col);

		int border;

		if (rule == -1) {
			// Get current border by moving direction and right hand rule
			border = getBorderByDirectionWithRightHandRule(direction);
		} else if (rule == 1) {
			// Get current border by moving direction and left hand rule
			border = getBorderByDirectionWithLeftHandRule(direction);
		}

    	// Turn until no border
    	while (isBorder(map, row, col, border)) {
    		/*
    		if (rule == -1) {
				// Turn to next border using left hand rule
				border = turnByRightHandRule(row, col, border);
    		} else if (rule == 1) {
				// Turn to next border using left hand rule
				border = turnByLeftHandRule(row, col, border);
    		}
    		*/
    		border = turn(row, col, border, rule);
    	}

		// Move
		if (border == 0) {
			// Update direction
			if (getTriangle(row, col) == -1) {
				direction = BOTTOM_LEFT;
			} else {
				direction = TOP_LEFT;
			}

			// Update location and step 1 cell left
			col -= 1;
		} else if (border == 1) {
			// Update direction
			if (getTriangle(row, col) == -1) {
				direction = BOTTOM_RIGHT;
			} else {
				direction = TOP_RIGHT;
			}

			// Update location and step 1 cell right
			col += 1;
		} else if (border == 2) {
			// Get triangle type (top border or bottom border)
			int triangle = getTriangle(row, col);

			// Update direction
			if (triangle == -1) {
				direction = TOP;
			} else {
				direction = BOTTOM;
			}

			// Update location and step 1 cell top or bottom
			row += triangle;
		}
    }
}

// @todo map validation with --test argument
// @todo help message with --help argument
// @todo (bonus) shortest maze path using --shortest argument
int main(int argc, char* argv[]) {
    // Validate input arguments
    if (argc != 5) {
        fprintf(stderr, "Usage: %s [--lpath|--rpath] <row> <col> <filename>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    // Init input variables
    int rule;

	if (strcmp(argv[1], "--rpath") == 0) {
        rule = -1;
    } else if (strcmp(argv[1], "--lpath") == 0) {
        rule = 1;
    } else  {
        fprintf(stderr, "Unsupported hand rule %s\n", argv[1]);
        exit(EXIT_FAILURE);
    }

	// Init starting location
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

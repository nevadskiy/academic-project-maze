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
bool isBorder(Map* map, int row, int col, int edge) {
	// Get cell value from map
    unsigned char value = map->cells[row * map->cols + col];

    switch (edge) {
        // Left diagonal edge
        case 0:
        	// Inspect 1st LSB (least significant bit)
            return ((value >> 0) & 1) != 0;
        // Right diagonal edge
        case 1:
        	// Inspect 2nd LSB (least significant bit)
            return ((value >> 1) & 1) != 0;
        // Top or bottom edge
        case 2:
        	// Inspect 3rd LSB (least significant bit)
            return ((value >> 2) & 1) != 0;
        // Invalid edge value
        default:
            return false;
    }
}

// Print location and adjust it to 1-based indexing
void printLocation(int row, int col) {
    printf("%d,%d\n", row + 1, col + 1);
}

// Determine if triangle has top or bottom edge on its position in grid
int getTriangle(int row, int col) {
    if ((row + col) % 2 == 0) {
      	// Top edge
        return -1;
    } else {
    	// Bottom edge
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

// Get edge by moving direction with left hand rule
// @note can be simplified using math formula
int getEdgeByDirectionWithLeftHandRule(enum Direction direction) {
	switch (direction) {
        case TOP_LEFT:
        case TOP:
            // Left edge
            return 0;
        case TOP_RIGHT:
        case BOTTOM_LEFT:
            // Top / Bottom edge
            return 2;
        case BOTTOM:
        case BOTTOM_RIGHT:
            // Right edge
            return 1;
        default:
            // Unsupported direction
            return -1;
    }
}

// Get edge by moving direction with right hand rule
// @note can be simplified using math formula
int getEdgeByDirectionWithRightHandRule(enum Direction direction) {
	switch (direction) {
		case BOTTOM:
		case BOTTOM_LEFT:
			// Left edge
			return 0;
        case TOP:
        case TOP_RIGHT:
            // Right edge
            return 1;
        case TOP_LEFT:
        case BOTTOM_RIGHT:
            // Top / Bottom edge
            return 2;
        default:
            // Unsupported direction
            return -1;
    }
}

/*
// Turn to next edge by left hand rule (clockwise direction)
int turnByLeftHandRule(int row, int col, int edge) {
	// Get triangle type (top sided or bottom sided) by location
    int triangle = getTriangle(row, col);

	 if (edge == 0) {
	     // from left to top or left to right
	     return (triangle == -1) ? 2 : 1;
	 } else if (edge == 1) {
	     // from right to left or right to bottom
	     return (triangle == -1) ? 0 : 2;
	 }

	 // from top to right or bottom to left
	 return (triangle == -1) ? 1 : 0;
}

// Turn to next edge by right hand rule (counter-clockwise direction)
int turnByRightHandRule(int row, int col, int edge) {
	// Get triangle type (top sided or bottom sided) by location
    int triangle = getTriangle(row, col);

	 if (edge == 0) {
	     // from left to top or left to right
	     return (triangle == -1) ? 1 : 2;
	 } else if (edge == 1) {
	     // from right to left or right to bottom
	     return (triangle == -1) ? 2 : 0;
	 }

	 // from top to right or bottom to left
	 return (triangle == -1) ? 0 : 1;
}
*/

// Use math to determine next edge by location and hand rule
int turn(int row, int col, int edge, int rule) {
	int triangle = getTriangle(row, col);

	return (3 + edge + (rule * triangle)) % 3;
}

void escapeMap(Map* map, int row, int col, int rule) {
	// @todo calculate starting direction (subtask #3) startDirection()
	// enum Direction direction = startDirection(map, row, col);
	enum Direction direction = BOTTOM_RIGHT;

	// Move while inside map
    while (isOutside(map, row, col) == false) {
		// Print current coordinate
		printLocation(row, col);

		int edge;

		if (rule == -1) {
			// Get current edge by moving direction and right hand rule
			edge = getEdgeByDirectionWithRightHandRule(direction);
		} else if (rule == 1) {
			// Get current edge by moving direction and left hand rule
			edge = getEdgeByDirectionWithLeftHandRule(direction);
		}

    	// Turn until no border
    	while (isBorder(map, row, col, edge)) {
    		/*
    		if (rule == -1) {
				// Turn to next border using left hand rule
				edge = turnByRightHandRule(row, col, edge);
    		} else if (rule == 1) {
				// Turn to next edge using left hand rule
				edge = turnByLeftHandRule(row, col, edge);
    		}
    		*/
    		edge = turn(row, col, edge, rule);
    	}

		// Move
		if (edge == 0) {
			// Update direction
			if (getTriangle(row, col) == -1) {
				direction = BOTTOM_LEFT;
			} else {
				direction = TOP_LEFT;
			}

			// Update location and step 1 cell left
			col -= 1;
		} else if (edge == 1) {
			// Update direction
			if (getTriangle(row, col) == -1) {
				direction = BOTTOM_RIGHT;
			} else {
				direction = TOP_RIGHT;
			}

			// Update location and step 1 cell right
			col += 1;
		} else if (edge == 2) {
			// Get triangle type (top edge or bottom edge)
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

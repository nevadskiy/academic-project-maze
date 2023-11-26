#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

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
        case 0:  // Left diagonal border
            return (cellValue & 1) != 0;
        case 1:  // Right diagonal border
            return (cellValue & 2) != 0;
        case 2:  // Top or bottom border
            return (cellValue & 4) != 0;
        default:
            return false;  // Invalid border value
    }
}

int main(int argc, char* argv[]) {
    // Validate input arguments
    if (argc != 5) {
        fprintf(stderr, "Usage: %s <row> <col> <border> <filename>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    // Initialize input variables
    int row = atoi(argv[1]) - 1;  // Adjust to 0-based indexing
    int col = atoi(argv[2]) - 1;  // Adjust to 0-based indexing
    int border = atoi(argv[3]);
    const char* filename = argv[4];

    // Initialize the map from the filename
    Map* map = initMap(filename);

    // Subtask #1: Print loaded map
    //  for (int i = 0; i < map->rows; ++i) {
    //      for (int j = 0; j < map->cols; ++j) {
    //          printf("%hhu ", map->cells[i * map->cols + j]);
    //      }
    //      printf("\n");
    //  }

    // Subtask #2: Check if there is a wall on the left diagonal border of cell (2, 3)
    // Check if there is a wall on the specified border of the given cell
    bool hasWall = isBorder(map, row, col, border);

    if (hasWall) {
        printf("There is a wall on the specified border of cell (%d, %d)\n", row + 1, col + 1);
    } else {
        printf("There is no wall on the specified border of cell (%d, %d)\n", row + 1, col + 1);
    }

    // Release memory
    releaseMap(map);

    return 0;
}
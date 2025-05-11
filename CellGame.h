//
// Created by nick on 4/29/25.
//

#ifndef CELLGAME_H
#define CELLGAME_H

#include <SDL3/SDL.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct {
    SDL_FRect square;
    int posX;
    int posY;
} Cell;

typedef struct {
    Cell* data;
    size_t size;     // number of shapes currently stored
    size_t capacity; // how many shapes can fit before realloc
} CellArray;

// Initialize an empty array
void initCellArray(CellArray* array) {
    array->data = NULL;
    array->size = 0;
    array->capacity = 0;
}

void freeCellArray(CellArray* array) {
    free(array->data);
    array->data = NULL;
    array->size = 0;
    array->capacity = 0;
}

void pushCell(CellArray* array, Cell cell) {
    if (array->size >= array->capacity) {
        // Grow the array (double size, or start with 4)
        size_t new_capacity = array->capacity == 0 ? 4 : array->capacity * 2;
        Cell* new_data = realloc(array->data, new_capacity * sizeof(Cell));
        if (!new_data) {
            fprintf(stderr, "Failed to allocate memory!\n");
            exit(1);
        }
        array->data = new_data;
        array->capacity = new_capacity;
    }
    array->data[array->size++] = cell;
}

#endif //CELLGAME_H

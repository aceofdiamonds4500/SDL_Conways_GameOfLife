#define SDL_MAIN_USE_CALLBACKS 1  /* use the callbacks instead of main() */
#define WINDOW_WIDTH 1300
#define WINDOW_HEIGHT 760
#define ARR_WIDTH (WINDOW_WIDTH / 10)
#define ARR_HEIGHT (WINDOW_HEIGHT / 10)

#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <math.h>
#include "CellGame.h"
#include <stdio.h>

/* We will use this renderer to draw into this window every frame. */
static SDL_Window *window = NULL;
static SDL_Renderer *renderer = NULL;
CellArray cells;
double gridPosX = 0, gridPosY = 0;

int gameDelayVal = 5;

int gameArray[ARR_WIDTH][ARR_HEIGHT];
int nextGen[ARR_WIDTH][ARR_HEIGHT];
int update = 0;

void newCell(int x, int y);
int checkNeighbors(int cellX, int cellY);
void gameDelay(int gameDelay);
void gameUpdate(void);

void newCell(int x, int y) {
    Cell newCell;
    newCell.posX = x;
    newCell.posY = y;

    //if (gameArray[newCell.posX][newCell.posY] == 1) {return;}
    newCell.square.x = (float) x * 10;
    newCell.square.y = (float) y * 10;
    newCell.square.w = 10;
    newCell.square.h = 10;

    gameArray[newCell.posX][newCell.posY] = 1;

    pushCell(&cells, newCell);
    // printf("%f\n", gridPosX);
    // printf("%d\n", gameArray[newCell.posX][newCell.posY]);
    // printf("%f\n", gridPosY);
}

int checkNeighbors(int cellX, int cellY) {
    int count = 0;
    for (int i = cellY-1; i <= cellY+1; i++) {
        for (int j = cellX-1; j <= cellX+1; j++) {
            if (cellX < 0 || cellX >= ARR_WIDTH || cellY < 0 || cellY >= ARR_HEIGHT || (cellY == i && cellX == j)) {
                count += 0;
            }
            else if (gameArray[j][i] == 1) { count++; }
        }
    }
    return count;
}

void gameUpdate(void) {
    /*displays the first generation
     for (int i = 0; i < ARR_HEIGHT; i++) {
         for (int j = 0; j < ARR_WIDTH; j++) {
             printf("%d ", gameArray[j][i]);
         }
         printf("\n");
     }
     */

    /* keeps track of all the neighbors and kills/births new cells */
    int neighbors = 0;
    for (int i = 0; i < ARR_HEIGHT; i++) {
        for (int j = 0; j < ARR_WIDTH; j++) {
            neighbors = checkNeighbors(j, i);
            if (gameArray[j][i] == 0 && neighbors != 3) {
                nextGen[j][i] = 0;
            }
            else if (gameArray[j][i] == 1 && (neighbors <= 1 || neighbors > 3)) {
                nextGen[j][i] = 0;
            }
            else {
                nextGen[j][i] = 1;
            }
        }
    }

    //last thing to do is clear the array
    for (int i = 0; i < ARR_HEIGHT; i++) {
        for (int j = 0; j < ARR_WIDTH; j++) {
            gameArray[j][i] = 0;
        }
    }

    /* display nextGen */
    /*for (int i = 0; i < ARR_HEIGHT; i++) {
        for (int j = 0; j < ARR_WIDTH; j++) {
            printf("%d ", nextGen[j][i]);
        }
        printf("\n");
    }*/

    /* gets rid of the other cells */
    freeCellArray(&cells);
    initCellArray(&cells);

    /* updates from new array to old array */
    for (int i = 0; i < ARR_HEIGHT; i++) {
        for (int j = 0; j < ARR_WIDTH; j++) {
            if (nextGen[j][i] == 1) {
                newCell(j, i);
            }
        }
    }
}

void gameDelay(int gameDelay) {
    SDL_Delay(gameDelay * 25);
}

/* This function runs once at startup. */
SDL_AppResult SDL_AppInit(void **appstate, int argc, char *argv[])
{
    initCellArray(&cells);

    SDL_SetAppMetadata("Conway\'s Game of Life", "1.0", "com.gameoflife");

    if (!SDL_Init(SDL_INIT_VIDEO)) {
        SDL_Log("Couldn't initialize SDL: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    if (!SDL_CreateWindowAndRenderer("examples/renderer/clear", WINDOW_WIDTH, WINDOW_HEIGHT, 0, &window, &renderer)) {
        SDL_Log("Couldn't create window/renderer: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    return SDL_APP_CONTINUE;  /* carry on with the program! */
}

/* This function runs when a new event (mouse input, keypresses, etc) occurs. */
SDL_AppResult SDL_AppEvent(void *appstate, SDL_Event *event)
{

    if (event->type == SDL_EVENT_QUIT) {
        return SDL_APP_SUCCESS;  /* end the program, reporting success to the OS. */
    }
    if (event->type == SDL_EVENT_MOUSE_BUTTON_DOWN) {
        gridPosX = event->button.x;
        int x = (int) gridPosX / 10;
        gridPosY = event->button.y;
        int y = (int)gridPosY / 10;

        if (fmod(event->button.x, 10) > 0) {
            gridPosX -= fmod(event->button.x, 10);
        }
        if (fmod(event->button.y, 10) > 0) {
            gridPosY -= fmod(event->button.y, 10);
        }
        newCell(x, y);
    }

    if (event->type == SDL_EVENT_KEY_DOWN) {
        if (event->key.key == SDLK_ESCAPE) {
            if (update == 1) {
                update = 0;
            }
            else if (update == 0) {
                update = 1;
            }
        }
        if (event->key.key == SDLK_UP) {
            if (gameDelayVal > 0) {
                gameDelayVal--;
            }
        }
        if (event->key.key == SDLK_DOWN) {
            if (gameDelayVal <= 15) {
                gameDelayVal++;
            }
        }
        if (event->key.key == SDLK_N) {
            update = 0;
            /* gets rid of the other cells */
            freeCellArray(&cells);
            initCellArray(&cells);
            for (int i = 0; i < ARR_HEIGHT; i++) {
                for (int j = 0; j < ARR_WIDTH; j++) {
                    gameArray[j][i] = 0;
                }
            }
        }
    }

    return SDL_APP_CONTINUE;  /* carry on with the program! */
}

/* This function runs once per frame, and is the heart of the program. */
SDL_AppResult SDL_AppIterate(void *appstate)
{
    Uint32 startTime = SDL_GetTicks();
    if (update) {
        gameDelay(gameDelayVal);
        gameUpdate();
    }
    Uint32 endTime = SDL_GetTicks();

    // Calculate elapsed time
    Uint32 elapsedTime = endTime - startTime;

    // Convert milliseconds to seconds
    float secondsElapsed = (float)elapsedTime / 1000.0f;

    printf("%f seconds\n", secondsElapsed);
    SDL_SetRenderDrawColorFloat(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE_FLOAT);  /* new color, full alpha. */

    /* clear the window to the draw color. */
    SDL_RenderClear(renderer);
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    for (size_t i = 0; i < cells.size; ++i) {
        SDL_RenderFillRect(renderer, &cells.data[i].square);
    }
    /* put the newly-cleared rendering on the screen. */
    SDL_RenderPresent(renderer);

    return SDL_APP_CONTINUE;  /* carry on with the program! */
}

/* This function runs once at shutdown. */
void SDL_AppQuit(void *appstate, SDL_AppResult result)
{
    /* SDL will clean up the window/renderer for us. */
    freeCellArray(&cells);
}
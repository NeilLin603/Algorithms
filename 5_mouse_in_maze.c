#include <stdio.h>

#define MAZE_SIZE 10

typedef struct {
    int x, y;
} Point_t;

int travel(int maze[][MAZE_SIZE], Point_t start, Point_t end) {
    if (maze[start.y][start.x]) {
        return 0;
    }
    maze[start.y][start.x] = 1;
    if (maze[end.y][end.x]) {
        return 1;
    }
    Point_t up = {.x = start.x, .y = start.y - 1}, down = {.x = start.x, .y = start.y + 1},
            left = {.x = start.x - 1, .y = start.y}, right = {.x = start.x + 1, .y = start.y};
    return travel(maze, up, end) || travel(maze, down, end) || travel(maze, left, end) || travel(maze, right, end);
}

void printMaze(const int maze[][MAZE_SIZE]) {
    for (int i = 0; i < MAZE_SIZE; i++) {
        for (int j = 0; j < MAZE_SIZE; j++) {
            if (maze[i][j] == 2) {
                printf("███"); // Walls
            } else if (maze[i][j] == 1) {
                printf(" ⋅ "); // Paths where mouse has traveled
            } else {
                printf("   "); // Paths where mouse hasn't traveled
            }
        }
        printf("\n");
    }
}

int main() {
    int maze[][MAZE_SIZE] = {
        {2, 2, 2, 2, 2, 2, 2, 2, 2, 2},
        {2, 0, 0, 0, 0, 0, 2, 0, 2, 2},
        {2, 2, 0, 2, 0, 2, 0, 0, 0, 2},
        {2, 2, 0, 2, 2, 0, 0, 2, 0, 2},
        {2, 2, 0, 0, 0, 0, 2, 2, 0, 2},
        {2, 2, 0, 2, 2, 2, 0, 0, 0, 2},
        {2, 0, 0, 2, 0, 0, 0, 2, 0, 2},
        {2, 2, 0, 2, 0, 2, 2, 2, 2, 2},
        {2, 2, 0, 2, 0, 0, 0, 0, 0, 2},
        {2, 2, 2, 2, 2, 2, 2, 2, 2, 2}};
    Point_t start = {.x = 1, .y = 1}, end = {.x = MAZE_SIZE - 2, .y = MAZE_SIZE - 2};

    if (travel(maze, start, end)) {
        printf("The mouse found the way to the end.\n");
    } else {
        printf("The mouse didn't find the way to the end.\n");
    }
    printMaze(maze);

    return 0;
}

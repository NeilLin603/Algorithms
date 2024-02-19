#include <stdio.h>

void swap(char *a, char *b) {
    printf("Swap %c & %c.\n", *a, *b);
    *a ^= *b;
    *b ^= *a;
    *a ^= *b;
}

void dutchFlag(char *a, char *b, char *c) {
    if (*a != 'R') {
        if (*b == 'R') {
            swap(a, b);
        } else {
            swap(a, c);
        }
    }
    if (*b != 'W') {
        swap(b, c);
    }
}

int main() {
    char colors[][3] = {
        {'R', 'W', 'B'},  // Case 1
        {'W', 'B', 'R'},  // Case 2
        {'B', 'R', 'W'},  // Case 3
        {'R', 'B', 'W'},  // Case 4
        {'B', 'W', 'R'},  // Case 5
        {'W', 'R', 'B'}}; // Case 6

    for (int i = 0; i < sizeof(colors) / sizeof(colors[0]); i++) {
        printf("Case %d:\n", i + 1);
        dutchFlag(&colors[i][0], &colors[i][1], &colors[i][2]);
        printf("\n");
    }

    return 0;
}

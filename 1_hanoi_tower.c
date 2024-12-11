#include <stdio.h>
#include <stdlib.h>

void hanoi(unsigned int n, char a, char b, char c) {
    if (n == 1) {
        printf("Move from %c to %c.\n", a, c);
        return;
    }
    if (n) {
        hanoi(n - 1, a, c, b);
        hanoi(1, a, b, c);
        hanoi(n - 1, b, a, c);
    }
}

int main() {
    unsigned int n;
    printf("Enter the number of plates: ");
    scanf("%d", &n);

    hanoi(n, 'A', 'B', 'C');

    system("pause");
    return 0;
}

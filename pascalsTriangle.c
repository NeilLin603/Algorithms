#include <stdio.h>
#include <stdlib.h>

int pascal(unsigned int m, unsigned int n) {
    if (m < n) {
        return -1;
    }
    if (!n || m == n) {
        return 1;
    }
    return pascal(m - 1, n - 1) + pascal(m - 1, n);
}

int main() {
    unsigned int n;
    printf("Enter the number of rows of Pascal's triangle: ");
    scanf("%d", &n);

    // Print the n-row Pascal's triangle
    for (int i = 0; i < n; i++) {
        printf("%*.s", (n - i - 1) * 3);
        for (int j = 0; j <= i; j++) {
            printf("%6d", pascal(i, j));
        }
        printf("\n");
    }

    system("pause");
    return 0;
}

#include <stdio.h>
#include <stdlib.h>

int fibonacci(unsigned int n) {
    int a = 0, b = 1;
    while (n--) {
        b += a;
        a = b - a;
    }
    return a;
}

int main() {
    unsigned int n;
    printf("Enter the term of Fibonacci sequence: ");
    scanf("%d", &n);

    printf("fibonacci(%d) = %d\n", n, fibonacci(n));

    system("pause");
    return 0;
}

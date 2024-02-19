#include <stdio.h>
#include <stdlib.h>

typedef struct {
    char name[16];
    int weight;
    int price;
} Fruit_t;

int bestPrice(Fruit_t *fruits, int fruitsSize, int weightLimit) {
    int *prices = (int *)calloc(weightLimit + 1, sizeof(int));
    for (int i = 0; i < fruitsSize; i++) {
        for (int j = fruits[i].weight; j <= weightLimit; j++)
        if (prices[j] < prices[j - fruits[i].weight] + fruits[i].price) {
            prices[j] = prices[j - fruits[i].weight] + fruits[i].price;
        }
    }
    int price = prices[weightLimit];
    free(prices);
    return price;
}

int main() {
    int weightLimit;
    printf("Enter the weight limit of backpack: ");
    scanf("%d", &weightLimit);
    Fruit_t fruits[] = {
        {.name = "apple", .weight = 5, .price = 5200},
        {.name = "banana", .weight = 3, .price = 3000},
        {.name = "lemon", .weight = 2, .price = 2000},
        {.name = "mango", .weight = 4, .price = 4200},
        {.name = "strawberry", .weight = 1, .price = 900}};
    int fruitsSize = sizeof(fruits) / sizeof(fruits[0]);

    printf("The max price of backpack = %d\n", bestPrice(fruits, fruitsSize, weightLimit));

    system("pause");
    return 0;
}

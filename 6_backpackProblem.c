#include <stdio.h>
#include <stdlib.h>

typedef struct {
    char name[16];
    int weight;
    int price;
} Fruit_t;

typedef struct ListNode {
    Fruit_t fruit;
    struct ListNode *next;
} Node_t;

void clearList(Node_t **head) {
    Node_t *ret;
    while (*head) {
        ret = *head;
        *head = ret->next;
        free(ret);
    }
}

Node_t *copyList(Node_t *head) {
    if (!head) {
        return (Node_t *)NULL;
    }
    Node_t *copy = (Node_t *)malloc(sizeof(Node_t));
    Node_t *tail = copy, *new;
    copy->fruit = head->fruit;
    while (head->next) {
        head = head->next;
        new = (Node_t *)malloc(sizeof(Node_t));
        new->fruit = head->fruit;
        tail->next = new;
        tail = new;
    }
    tail->next = (Node_t *)NULL;
    return copy;
}

void addNode(Node_t **head, Fruit_t fruit) {
    Node_t *new = (Node_t *)malloc(sizeof(Node_t));
    new->fruit = fruit;
    new->next = *head;
    *head = new;
}

int popNode(Node_t **head, Fruit_t *fruit) {
    if (*head) {
        Node_t *ret = *head;
        *fruit = ret->fruit;
        *head = ret->next;
        free(ret);
        return 1;
    }
    return 0;
}

int maxPrice(Fruit_t *fruits, int fruitsSize, int weightLimit, Node_t **fruitList) {
    int *prices = (int *)calloc(weightLimit + 1, sizeof(int));
    Node_t **lists = (Node_t **)calloc(weightLimit + 1, sizeof(Node_t *));
    for (int i = 0; i < fruitsSize; i++) {
        for (int j = fruits[i].weight; j <= weightLimit; j++)
        if (prices[j] < prices[j - fruits[i].weight] + fruits[i].price) {
            prices[j] = prices[j - fruits[i].weight] + fruits[i].price;
            clearList(&lists[j]);
            lists[j] = copyList(lists[j - fruits[i].weight]);
            addNode(&lists[j], fruits[i]);
        }
    }
    int price = prices[weightLimit];
    free(prices);
    *fruitList = lists[weightLimit];
    for (int j = 0; j < weightLimit; j++) {
        clearList(&lists[j]);
    }
    free(lists);
    return price;
}

int main() {
    int weightLimit;
    printf("Enter the weight limit of backpack: ");
    scanf("%d", &weightLimit);
    Fruit_t fruits[] = {
        {.name = "apple", .weight = 5, .price = 5900},
        {.name = "banana", .weight = 3, .price = 3500},
        {.name = "lemon", .weight = 2, .price = 2300},
        {.name = "mango", .weight = 4, .price = 4750},
        {.name = "strawberry", .weight = 1, .price = 1100}};
    Node_t *fruitList;
    Fruit_t fruit;

    printf("The max price of backpack = %d\n",
           maxPrice(fruits, sizeof(fruits) / sizeof(fruits[0]), weightLimit, &fruitList));
    printf("%-*.*sWeight\tPrice\n", 12, 10, "Name");
    while (popNode(&fruitList, &fruit)) {
        printf("%-*.*s%-12d%-12d\n", 12, 10, fruit.name, fruit.weight, fruit.price);
    }

    system("pause");
    return 0;
}

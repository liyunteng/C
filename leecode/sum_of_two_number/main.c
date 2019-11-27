/*
 * Description: main
 *
 * Copyright (C) 2018 liyunteng
 * Last-Updated: <2019/09/16 23:40:31>
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#if 0
int *twoSum(int *nums, int numsSize, int target) {
    int *result = NULL;
    int a, b;
    for (a = 0; a < numsSize; a++) {
        for (b = a+1; b < numsSize; b++) {
            if (nums[a] + nums[b] == target) {
                result = malloc(sizeof(int) * 2);
                result[0] = a;
                result[1] = b;
                return result;
            }
        }
    }
    return result;
}
#endif

struct hash_data {
    int index;
};

struct hash_table {
    struct hash_data *element;
    const int *nums;
    int count;
};

int
hash_init(struct hash_table *table, int count)
{
    if (count <= 0) {
        return -1;
    }

    table->element =
        (struct hash_data *)malloc(sizeof(struct hash_data) * count);
    if (table->element == NULL) {
        return -1;
    }

    for (int i = 0; i < count; i++) {
        table->element[i].index = -1;
    }
    table->count = count;
    return 0;
}

void
hash_free(struct hash_table *table)
{
    if (table->element) {
        free(table->element);
        table->element = NULL;
    }

    table->count = 0;
}

int
hash_addr(int data, int table_count)
{
    int addr = data % table_count;
    return (addr >= 0) ? addr : (addr + table_count);
}

void
hash_insert(struct hash_table *table, int data, int index)
{
    int addr = hash_addr(data, table->count);
    while (table->element[addr].index >= 0) {
        addr = (addr + 1) % table->count;
    }
    table->element[addr].index = index;
}

struct hash_data *
hash_find(struct hash_table *table, int data)
{
    int primary;
    int addr = primary = hash_addr(data, table->count);
    do {
        if (table->element[addr].index < 0) {
            return NULL;
        }
        if (table->nums[table->element[addr].index] == data) {
            return &table->element[addr];
        }
        addr = (addr + 1) % table->count;
    } while (addr != primary);
    return NULL;
}

int *
twoSum(int *nums, int numsSize, int target, int *returnSize)
{
    int *result = NULL;
    *returnSize = 0;
    struct hash_table table;
    struct hash_data *p_data;

    if (hash_init(&table, numsSize * 2) < 0) {
        return result;
    }
    table.nums = nums;
    for (int i = 0; i < numsSize; i++) {
        if ((p_data = hash_find(&table, target - nums[i])) != NULL) {
            result      = malloc(sizeof(int) * 2);
            result[0]   = p_data->index;
            result[1]   = i;
            *returnSize = 2;
            hash_free(&table);
            return result;
        }
        hash_insert(&table, nums[i], i);
    }
    hash_free(&table);
    return result;
}

int
main(void)
{
    int *test = malloc(sizeof(int) * 100);
    for (int i = 0; i < 100; i++) {
        test[i] = i;
    }
    int size = 0;
    int *r   = twoSum(test, 100, 98 + 99, &size);
    if (r != NULL) {
        printf("%d %d", r[0], r[1]);
    }
    return 0;
}

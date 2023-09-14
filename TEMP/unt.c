// Sort Untyped Array


void sortNotTypedArray(void *a, int size, int (*compare)(void *, void *), int typeSize)
{
    int l, r, end = size * typeSize;
    for (l = 0; l < end - typeSize; l += typeSize)
    {
        for (r = l + typeSize; r < end; r += typeSize)
        {
            if (compare(a + l, a + r) > 0)
            {
                int *temp = malloc(typeSize); // memory creation
                if (!temp)
                    return;
                memcpy(temp, a + l, typeSize); // source, destination, size
                memcpy(a + l, a + r, typeSize);
                memcpy(a + r, temp, typeSize);
                free(temp);
            }
        }
    }
}

#include <stdio.h>
#include <stdlib.h> // For malloc(...)
#include "int_array.h"

// Create a function to create a untyped list of integers
void *createList(int size, int typeSize)
{
    void *list = malloc(size * typeSize);
    // populate random values
    for (int i = 0; i < size; i++)
        *(int *)(list + i * typeSize) = rand() % 100;
    return list;
}

// display the list
void displayList(void *list, int size, int typeSize)
{
    for (int i = 0; i < size; i++)
        printf("%d ", *(int *)(list + i * typeSize));
    printf("\n");
}
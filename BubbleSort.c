#include <stdio.h>
#include <stdlib.h>

void bubbleSort(int numbers[], int array_size)
{
    int i, j, temp;
    for (i = (array_size - 1); i > 0; i--) 
    {
        for (j = 1; j <= i; j++) // changed to <=i in task 0
        {
            if (numbers[j - 1] > numbers[j])
            {
                //temp = (int *)malloc(sizeof(int *));
                temp = numbers[j - 1];
                numbers[j - 1] = numbers[j];
                numbers[j] = temp;
            }
        }
    }
}

int main(int argc, char **argv)
{
    char **arr = argv + 1; // pointer to the array of numbers ?
    int i, n = argc - 1; // number of elements in the array without the program name
    
    int *numbers = (int *)calloc(n, sizeof(int));

    printf("Original array:");
    for (i = 0; i < n; ++i) // arr points to the element in place 1 (not 0 - not to include the program name)
    // we changed <= to < in task 0
    {
        printf(" %s", arr[i]);
        numbers[i] = atoi(arr[i]);
    }
    printf("\n");

    bubbleSort(numbers, n);

    printf("Sorted array:");
    for (i = 0; i < n; ++i)
        printf(" %d", numbers[i]);
    printf("\n");
    free(numbers); //we added- task 0 
    return 0;
}
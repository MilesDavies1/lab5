#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <time.h>

#define SIZE 100

int array[SIZE];

void fillArrayWithRandomNumbers(int arr[SIZE]) {
    for (int i = 0; i < SIZE; i++)
        array[i] = rand() % 100;
}

void printArray(int arr[SIZE]) {
    for (int i = 0; i < SIZE; i++)
        printf("%5d", arr[i]);
    printf("\n");
}

typedef struct StartEndIndexes {
    int start;
    int end;
} StartEndIndexes;

// Function to merge two halves of the array
void merge(int arr[], int start, int mid, int end) {
    int leftSize = mid - start + 1;
    int rightSize = end - mid;

    int left[leftSize], right[rightSize];

    // Copy data to temporary arrays left[] and right[]
    for (int i = 0; i < leftSize; i++)
        left[i] = arr[start + i];
    for (int j = 0; j < rightSize; j++)
        right[j] = arr[mid + 1 + j];

    // Merge the temporary arrays back into arr[start..end]
    int i = 0, j = 0, k = start;
    while (i < leftSize && j < rightSize) {
        if (left[i] <= right[j]) {
            arr[k] = left[i];
            i++;
        } else {
            arr[k] = right[j];
            j++;
        }
        k++;
    }

    // Copy the remaining elements of left[], if there are any
    while (i < leftSize) {
        arr[k] = left[i];
        i++;
        k++;
    }

    // Copy the remaining elements of right[], if there are any
    while (j < rightSize) {
        arr[k] = right[j];
        j++;
        k++;
    }
}

// Recursive function for parallel merge sort
void* mergeSort(void* args) {
    StartEndIndexes* sei = (StartEndIndexes*)args;
    int start = sei->start;
    int end = sei->end;

    if (start < end) {
        // Find mid point
        int mid = start + (end - start) / 2;

        // Create two threads for sorting the two halves
        pthread_t tid1, tid2;
        StartEndIndexes leftArgs, rightArgs;
        leftArgs.start = start;
        leftArgs.end = mid;
        rightArgs.start = mid + 1;
        rightArgs.end = end;
        pthread_create(&tid1, NULL, mergeSort, &leftArgs);
        pthread_create(&tid2, NULL, mergeSort, &rightArgs);

        // Wait for the threads to finish
        pthread_join(tid1, NULL);
        pthread_join(tid2, NULL);

        // Merge the sorted halves
        merge(array, start, mid, end);
    }

    return NULL;
}

// Function to perform parallel merge sort
void parallelMergeSort() {
    StartEndIndexes sei;
    sei.start = 0;
    sei.end = SIZE - 1;

    // Create a thread for merge sort
    pthread_t tid;
    pthread_create(&tid, NULL, mergeSort, &sei);

    // Wait for the thread to finish
    pthread_join(tid, NULL);
}

int main() {
    srand(time(0));

    // 1. Fill array with random numbers.
    fillArrayWithRandomNumbers(array);

    // 2. Print the array.
    printf("Unsorted array: ");
    printArray(array);

    // 3. Perform parallel merge sort.
    parallelMergeSort();

    // 4. Print the sorted array.
    printf("Sorted array:   ");
    printArray(array);

    return 0;
}

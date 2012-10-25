#include <stdio.h>
#include <stdlib.h>

void show_array(int *);
int myinit(int *, int);
int* mymalloc(int *, int);
int mydispose(int *);
int myfree(int *, int *);

int* malloc_test(int *array, int size, int id)
{
    int *newArray = mymalloc(array, size);
    
    for (int i = 0; i < size; i++)
    {
        newArray[i] = id;
    }
    
    return newArray;
}

int free_test(int *array, int *block, int size)
{
    for (int i = 0; i < size; i++)
    {
        block[i] = 0;
    }
    
    return myfree(array, block);
}

int main()
{
	int *array = malloc(500 * sizeof(int));

	printf("Myinit: %d \n", myinit(array, 500));

	printf("Is the array initialised? %d\n", mydispose(array));

	//Test with another random array to make sure we don't have false
	//positives
	int *otherArray = malloc(10 * sizeof(int));
	printf("Is the other array wrong? %d\n", mydispose(otherArray));

	int *pointer = malloc_test(array, 1, 101);
	int *pointer2 = malloc_test(array, 1, 102);

	show_array(array);

	printf("\nFREEING MEMORY\n");
	printf("Free first pointer: %d\n\n", free_test(array, pointer, 1));

	show_array(array);

	printf("\nASKING FOR NEW SPACE\n");
	int *pointer3 = malloc_test(array, 1, 103);
	int *pointer4 = malloc_test(array, 2, 104);
	int *pointer5 = malloc_test(array, 2, 105);

	free_test(array, pointer2, 1);
	free_test(array, pointer4, 2);

	int *pointer6 = malloc_test(array, 3, 106);
	
	show_array(array);
	
	//Use the pointers for nothing, so the compiler doesn't complain
	//it's unused
	if (*pointer3 + *pointer6 + *pointer5) {}

	return 0;
}

void show_array(int *array)
{
	for (int i = 0; i < 100; i++)
	{
		printf("%d: %d\n", i, array[i]);
	}
}

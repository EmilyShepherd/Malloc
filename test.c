#include <stdio.h>
#include <stdlib.h>

void show_array(int *);
int myinit(int *, int);
int* mymalloc(int *, int);
int mydispose(int *);
int myfree(int *);

int main()
{
	int *array = malloc(500 * sizeof(int));

	printf("Myinit: %d \n", myinit(array, 500));

	printf("Is the array initialised? %d\n", mydispose(array));

	//Test with another random array to make sure we don't have false
	//positives
	int *otherArray = malloc(10 * sizeof(int));
	printf("Is the other array wrong? %d\n", mydispose(otherArray));

	show_array(array);

	int *pointer = mymalloc(array, 1);
	*pointer = 5;
	int *pointer2 = mymalloc(array, 1);
	*pointer2 = 10;

	printf("My pointer points to: %p \n", (void *)pointer);
	printf("My pointer points to: %d \n", *pointer);

	show_array(array);

	printf("My pointer points to: %p \n", (void *)pointer2);
	printf("My pointer points to: %d \n", *pointer2);

	//printf("My pointer points to: %d \n", mymalloc(array, 1));
	printf("My array points to: %p \n", (void *)array);

	show_array(array);

	return 0;
}

void show_array(int *array)
{
	for (int i = 0; i < 100; i++)
	{
		printf("%d: %d\n", i, array[i]);
	}
}
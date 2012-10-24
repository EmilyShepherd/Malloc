#include <stdio.h>
#include <stdlib.h>

int main()
{
	int *array = malloc(500 * sizeof(int));
	char *arr  = (char *)array;

	printf("Myinit: %d \n", myinit(array, 500));

	printf("Is the array initialised? %d\n", mydispose(array));

	//Test with another random array to make sure we don't have false
	//positives
	int *otherArray = malloc(10 * sizeof(int));
	printf("Is the other array wrong? %d\n", mydispose(otherArray));

	int p = mymalloc(array, 2);

	//printf("%d \n %d \n\n", (int)array, p);

	//Show what's in the array:
	for (int i = 0; i < 100; i++)
	{
		printf("%d: %d\n", i, array[i]);
	}
}
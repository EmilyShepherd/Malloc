#include <stdio.h>
#include <stdlib.h>

int main()
{
	int *array = malloc(500 * sizeof(int));

	printf("Myinit: %d", myinit(array, 500));
}
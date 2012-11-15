#include <stdio.h>

//Steal the inner function used by our mymalloc and myfree functions
int inc_header(int *, int *);

void gui_show_array(int *array)
{
    int curheader = 1;
    
    printf("+-------------------------------------+\n");
    printf("| Total memory space: %d\n", array[0]);
    printf("+-------------------------------------+\n");
    
    while (1)
    {
        int size;
        
        if (array[curheader] > 0)
        {
            size = array[curheader];
            printf("| Free block, size: %d\n", size);
        }
        else
        {
            size = -1 * array[curheader];
            printf("| Used block, size: %d\n", size);
        }
        
        for (int i = curheader + 1; i < curheader + size + 1; i++)
        {
            if (array[curheader] > 0)
            {
                if (array[i] == 0)
                {
                    printf("|\n");
                }
                else
                {
                    printf("|   Dead header (size: %d)\n", array[i]);
                }
            }
            else if (array[i] == 999)
            {
                printf("|   Data\n");
            }
            else if (array[i] == 0)
            {
                printf("+ - - - - - - - - - - - - - - - - - - +\n");
                printf("| Wasted int\n");
            }
            else
            {
                printf("|   Data (%d)\n", array[i]);
            }
        }
        
        printf("+-------------------------------------+\n");

        if (!inc_header(array, &curheader)) return;
    }
}


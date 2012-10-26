int inc_header(int *array, int *curheader)
{
	if (array[*curheader] < 0)
	{
		*curheader += -1*array[*curheader] + 1;
	}
	else
	{
		*curheader += array[*curheader] + 1;
	}

	if (*curheader >= array[0])
	{
		return 0;
	}
	else
	{
		return 1;
	}
}

int decode_header(int *header, int *size)
{
    unsigned char *byte = (unsigned char *)header;
    unsigned usize = 0;
    int free = 0;
    
    //The 7th bit has in the first byte is the free flag,
    //so check that. If it's 1, set it to 0 so it doesn't
    //get used as part of the value by the code below.
    if ((*byte & 0x40) == 0x40)
    {
        free   = 1;
        *byte &= 0xBF; //1011 1111
    }
    
    while (1)
    {
        //Shift the value up 7 bits and put the next 7 bits
        //in after it.
        usize = (usize << 7) + (*byte & 0x7F); //0111 1111
        
        //If the 8th bit is 1, there is another byte to follow
        if (*byte >> 7 == 1)
        {
            byte++;
        }
        else
        {
            *size = (signed int)usize;
            return free;
        }
    }
}

int encode_header(int *iarray, int free, int size)
{
    unsigned char *barray = (unsigned char *)iarray;
    int pos = 0;
    unsigned int usize = (unsigned int)size;
    
    //Split the number into 5 bytes, each containing
    //7 bits of the value.
    unsigned char byte[5];
    byte[0] = usize >> 28 & 0x0F;
    byte[1] = usize >> 21 & 0x7F;
    byte[2] = usize >> 14 & 0x7F;
    byte[3] = usize >> 7  & 0x7F;
    byte[4] = usize       & 0x7F;
    
    //Loop through the bytes, looking for the first
    //one that's used - so we need not save the higher
    //unused bytes.
    for (pos = 0; byte[pos] == 0; pos++);
    
    //Check if this byte uses its 7th bit. If it does
    //we'll need to create a new byte, because we need
    //the 7th bit of the first byte for the sign.
    if ((byte[pos] & 0x40) == 0x40)
    {
        pos--;
    }
    
    //If we want to mark this header's block as free, save
    //1 into the 7th bit of the first byte
    if (free)
    {
        byte[pos] = byte[pos] | 0x40; //0100 0000
    }
    
    //Save all except the last byte, with the MSB bit
    //set to 1 to show there are more bytes to follow
    for (int i = pos; i < 4; i++)
    {
        *barray = byte[i] | 0x80; //1000 0000
        barray++;
    }
    
    //Save the last byte
    //The 8th bit must be 0, but we know it is, because of
    //the 0x7F mask we used above.
    *barray = byte[4];
    
    return 5 - pos;
}

int myinit(int *array, int size)
{
	if (size < 14) return 0;

	array[0] = size;

	array[1] = size - 2;

	return 1;
}

int* mymalloc(int *array, int size)
{
	int curheader = 1;

	while (1)
	{
		// positive header int, implies free space of this size
		if (array[curheader] >= size)
		{
			
			if (size + 1 < array[curheader])
			{
				// Allocate space, then put header after data for new section of free data
				array[curheader + size + 1] = array[curheader] - size - 1;
				array[curheader] = -1*size;

			} 
			else
			{
				// No size for a new header, the entire array is being used
				array[curheader] *= -1; 	
			}

			return &array[curheader+1];
		}

		if (!inc_header(array, &curheader)) return (int *) 0;
	} 



}

int myfree(int *array, int *block)
{
	int curheader  = 1;
	int lastheader = 0;
	
	while (1)
	{
		if (&array[curheader + 1] == block)
		{
			//This block exists, but it is not being used
			//ignore this free request
			if (array[curheader] > 0)
			{
				return 0;
			}

			//Mark it as free
			array[curheader] *= -1;

			int nextheader = curheader + 1 + array[curheader];

			//Next block is free - combine
			if (nextheader < array[0] && array[nextheader] > 0)
			{
				array[curheader] += array[nextheader] + 1;
			}

			//Last block is free - combine
			if (lastheader != 0 && array[lastheader] > 0)
			{
				array[lastheader] += array[curheader] + 1;
			}

			return 1;
		}

		//Save this header and move unto the next one
		lastheader = curheader;
		if (!inc_header(array, &curheader)) return 0;
	}
}

int mydispose(int *array)
{

	// Return one if free space pointer correctly pointers to the rest of the array
	if (array[1] == array[0] - 2) return 1;
	return 0; 
}

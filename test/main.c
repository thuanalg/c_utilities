#include <stdio.h>
#include <stdlib.h>
#include <ext_c_ultilities.h>


int main(int argc, char *argv[])
{
	int err = 0;
	if(argc < 2)
	{
		fprintf(stdout, "\nPlease enter test directory.\n");
		return EXIT_FAILURE;
	}
	c_ultilities_remove_dir(argv[1], &err);
	if(err)
	{
		fprintf(stdout, "\nPlease enter test directory. err = %d\n", err);
	}
	return EXIT_SUCCESS;
}

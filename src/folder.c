#include <ext_c_ultilities.h>
#include "int_c_ultilities.h"
#include <string.h>
#include <sys/types.h>
#include <dirent.h>
#include <unistd.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>


CU(remove_dir) (char *path, int *e)
{
	int err = 0;
	DIR* srcdir = 0;
	struct dirent* dent = 0;
	char tmp[1024];
	struct stat st;
	size_t zn = 0;
	char *fname = 0;
	do
	{
		srcdir = opendir(path);
		if (srcdir == 0)
		{
			err = __LINE__;
			break;
		}
		while( (dent = readdir(srcdir)) != 0)
		{
			if(strcmp(dent->d_name, ".") == 0 || strcmp(dent->d_name, "..") == 0)
			{
				continue;
			}
			sprintf(tmp, "%s/%s", path, dent->d_name);
			if (stat(tmp, &st) < 0)
			{
				perror(dent->d_name);
				err = __LINE__;
				break;
			}
			zn = strlen(path) + strlen(dent->d_name) + 1;
			fname = calloc(1, zn);
			sprintf(fname, "%s/%s", path, dent->d_name);

			fprintf(stdout, "fname: %s\n\n", fname);

			if (S_ISDIR(st.st_mode) && fname) 
			{
				c_ultilities_remove_dir(fname, e);
			}
			else if(S_ISREG(st.st_mode) || S_ISLNK(st.st_mode)) {
				if(fname) {
					err = remove(fname) ? __LINE__ : 0;
				}
			}
			if(fname) {
				free(fname);
			}
			if(err) {
				break;
			}
		}
		closedir(srcdir);
		if(err)
		{
			break;
		}
		fprintf(stdout, "path: %s\n\n", path);
		err = rmdir(path);
		if(err) {
			err = __LINE__;
		}
	}
	while(0);
	if(e)
	{
		*e = err;
	}
} 

CU(create_dir) (char *path, int *e)
{
	
}

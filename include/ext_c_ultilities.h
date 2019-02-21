#ifndef __EXT_C_ULTILITES__H__
#define __EXT_C_ULTILITES__H__

#define CONCAT(prefix,name)  prefix##name

#define CU(name) void CONCAT(c_ultilities_,name)

#ifdef __cplusplus
extern "C" {
#endif

CU(remove_dir) (char *path, int *);

CU(create_dir) (char *path, int *);


#ifdef __cplusplus
}
#endif



#endif

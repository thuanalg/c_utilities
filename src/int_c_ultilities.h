#ifndef __INT_C_ULTILITES__H__
#define __INT_C_ULTILITES__H__

#define CONCAT(prefix, name)  prefix##name

#define CU(name) void CONCAT(c_ultilities_, name)

#ifdef __cplusplus
extern "C" {
#endif

CU(remove_dir) (char *path);

CU(create_dir) (char *path);


#ifdef __cplusplus
#endif

#endif

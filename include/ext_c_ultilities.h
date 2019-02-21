#ifndef __EXT_C_ULTILITES__H__
#define __EXT_C_ULTILITES__H__

#ifndef __CONCAT__
#define __CONCAT__

#define CONCAT(prefix,name)  prefix##name

#define CU(name) CONCAT(c_ultilities_,name)

#endif

#ifdef __cplusplus
extern "C" {
#endif

void CU(remove_dir) (char *path, int *);

void CU(create_dir) (char *path, int *);


#ifdef __cplusplus
}
#endif



#endif

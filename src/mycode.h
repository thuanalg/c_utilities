#include <sqlite3.h>
#ifndef __SQLITE_EXT_PROCESS__
#define __SQLITE_EXT_PROCESS__

#ifdef __cplusplus
extern "C" {
#endif

#define _X(u) AAA_##u

int _X(xyz_open)(char *path);
int _X(xyz_close)(int session);

#ifdef __cplusplus
}
#endif
#endif


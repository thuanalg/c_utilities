#include <sqlite3.h>
#ifndef __SQLITE_EXT_PROCESS__
#define __SQLITE_EXT_PROCESS__

#ifdef __cplusplus
extern "C" {
#endif

#define _X(u) AAA_##u

int _X(sqlite3_open)(char *path);

#ifdef __cplusplus
}
#endif
#endif


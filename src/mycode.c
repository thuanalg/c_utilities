#include "mycode.h"
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h> /* For mode constants */
#include <fcntl.h> /* For O_* constants */
#include <pthread.h>

static int _X(init_service)(char *path);

int _X(sqlite3_open)(char *path)
{
  int err;
  do
  {

  }
  while(0);

  return err;
}

#define PRIVATE_NAME_STA  "/5112052a-02a6-4818-ac42-2de914ef5700_"
#define STA_SHM_MODE      (S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH)
#define STA_SHM_SZ        (20 << 1)
static void * sta_shm_sqlite3 = 0;
static char sta_shm_key[1024];

int _X(init_service)(char *path)
{
  char create = 0;
  char *key = sta_shm_key;
  int err = 0;
  int shm = 0;
  void *data = 0;
  sprintf(key, "%s%s", PRIVATE_NAME_STA, key);
  do{
    shm = shm_open(key, O_CREAT|O_RDWR | O_EXCL, STA_SHM_MODE);  
    if(shm >= 0)
    {
      create = 1;
    }
    else
    {
      shm = shm_open(key, O_RDWR, STA_SHM_MODE);  
      if(shm < 0)
      {
        //err = CANNOT_INIT_SHM;
        err = __LINE__;
        break;
      }
    }
    err = ftruncate(shm, STA_SHM_SZ);
    if(err)
    {
      break;
    }
    err = close(shm); 
    if(err)
    {
      break;
    }
    sta_shm_sqlite3 = mmap(0, STA_SHM_SZ, 
        PROT_READ | PROT_WRITE | PROT_EXEC , MAP_SHARED, shm, 0);
    if(!sta_shm_sqlite3)
    {
      err = __LINE__;
      break;
    }
    if(!create)
    {
      break;
    }
    memset(sta_shm_sqlite3, 0, STA_SHM_SZ);
  }
  while(0);

  return err;
}

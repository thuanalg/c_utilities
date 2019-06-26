#include "mycode.h"
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h> /* For mode constants */
#include <fcntl.h> /* For O_* constants */
#include <pthread.h>

static int _X(init_service)(char *path, void **, int);
#define PRIVATE_NAME_UTV  "/5112052a-02a6-4818-ac42-2de914ef5700_"
#define UTV_KHL_MODE      (S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH)
#define UTV_KHL_SZ        (20 << 1)
static void * data_shm_xyz = 0;
static void * comm_shm_xyz  = 0;
static char sta_shm_key[1024];
/************************************************************************
@return: session
************************************************************************/
int _X(xyz_close)(int session)
{
  int err = 0;
  do
  {
    if(data_shm_xyz){
      err = unmap(data_shm_xyz, UTV_KHL_SZ);
    }
    if(comm_shm_xyz){
      err = unmap(comm_shm_xyz, UTV_KHL_SZ);
    }
  }
  while(0);
  return err;
}
int _X(xyz_open)(char *path)
{
  int err;
  do
  {
    err = _X(init_service)(path, &data_shm_xyz, UTV_KHL_SZ);
    err = _X(init_service)(path, &comm_shm_xyz, UTV_KHL_SZ);
  }
  while(0);

  return err;
}


int _X(init_service)(char *path, void **out, int sz)
{
  char create = 0;
  char *key = sta_shm_key;
  int err = 0;
  int shm = 0;
  void *data = 0;
  sprintf(key, "%s%s", PRIVATE_NAME_UTV, key);
  do{
    shm = shm_open(key, O_CREAT|O_RDWR | O_EXCL, UTV_KHL_MODE);  
    if(shm >= 0)
    {
      create = 1;
    }
    else
    {
      shm = shm_open(key, O_RDWR, UTV_KHL_MODE);  
      if(shm < 0)
      {
        err = __LINE__;
        break;
      }
    }
//    err = ftruncate(shm, UTV_KHL_SZ);
//    if(err)
//    {
//      break;
//    }
    (*out) = mmap(0, sz, 
        PROT_READ | PROT_WRITE | PROT_EXEC , MAP_SHARED, shm, 0);
    if(!(*out))
    {
      err = __LINE__;
      break;
    }
    if(!create)
    {
      break;
    }
    memset((*out), 0, UTV_KHL_SZ);
  }
  while(0);
  if(shm > -1)
  {
    err = close(shm); 
  }
  return err;
}
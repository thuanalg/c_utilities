#include "mycode.h"
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h> /* For mode constants */
#include <fcntl.h> /* For O_* constants */
#include <pthread.h>


typedef enum {
  CM_REGISTER,
  CM_QUERY,
  
  CM_EXIT,
} XYZ_COMM_;

typedef struct {
  int nbyte;
  int sess;
  unsigned int pid;
  char text;
} XYZ_COMM_FMT;

typedef struct
{
  pthread_mutex_t mtx;
  unsigned int lpid;
  int sz_cmds;
  char cmds;
}XYZ_COMMAND;

#define PRIVATE_NAME_UTV  "/5112052a-02a6-4818-ac42-2de914ef5700_"
#define UTV_KHL_MODE      (S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH)
#define UTV_KHL_SZ        (20 << 1)
static void * data_shm_xyz = 0;
static char my_key[1024];


static int _X(init_service)(char *path, void **, int);
static int _X(rem_sess)(int sess);
static int _X(xyz_register)(int *sess);
static int _X(xyz_cmd_fmt)(int sess, int cmd, 
  char *cmdtext, char **out, int *);
/************************************************************************
* Format of memory segment
*      @COMMANFD|@DATARESULT
*      sizeof(@COMMAND) = 50 * 1024 (50 KB)
*      sizeof(@DATARESULT) = (1024 - 50) * 1024 == (1024 - 50) KB.
* @COMMAND detail:
*    mutex|lockingId|totalcmds|list_commands
*    @mutex: sizeof(mutex), mandatory. Fixed size
     @lockingId: process Id owns this segment memory. Fixed size =
                 sizeof(int)
*    @totalZcommands: sizeof(int), the size of total current command(s)  
*                    ,mandatory. Fixed size.
*    @list_commands: a group command(s), dynamic size. 
*                      UTF-8 referrence
************************************************************************/


/************************************************************************
@return: session
************************************************************************/
int _X(xyz_cmd_fmt)(int sess, int cmd, 
char *cmdtext, char **out, int *nbyte)
{
  int err = 0;
  do
  {
    if(!nbyte)
    {
      err = __LINE__;
      break;
    }
    if(!out)
    {
      err = __LINE__;
      break;
    }
    *nbyte = (int) (4 * sizeof(int) + strlen(cmdtext) + 1);
    *out = realloc(*out, *nbyte);
    if(!(*out))
    {
      err = __LINE__;
      break;
    }
    memset(*out, 0, *nbyte);
    {
      //Format: totalbytes|session|process id of sender|cmdID|cmdText
      //@totalbytes: 4 bytes (sizeof(int)), mandatory
      //@session:    4 bytes(sizeof(int)), mandatory
      //@processID:  4 bytes, mandatory
      //@cmdID:      4 bytes, mandatory
      //@cmdText:    unpredictable, optional
      int s = 0;
      unsigned int pid = (unsigned int)getpid();
      memcpy(*out + s * sizeof(int), nbyte, sizeof(int));
      s++;
      memcpy(*out + s * sizeof(int), &sess, sizeof(int));
      s++;
      memcpy(*out + s * sizeof(int), &pid, sizeof(int));
      s++;
      memcpy(*out + s * sizeof(int), &cmd, sizeof(int));
      s++;
      memcpy(*out + s * sizeof(int), cmdtext, 1 + strlen(cmdtext));
    }
  }
  while(0);
  return err;
}

int _X(xyz_register)(int *sess)
{
  int err = 0;
  do
  {

  }
  while(0);
  return err;
}

int _X(rem_sess)(int sess)
{
  int err = 0;
  do
  {

  }
  while(0);
  return err;
}

int _X(xyz_close)(int session)
{
  int err = 0;
  do
  {
    err = _X(rem_sess)(session);
    if(data_shm_xyz)
    {
      err = munmap(data_shm_xyz, UTV_KHL_SZ);
    }
    if(my_key[0])
    {
      err = shm_unlink(my_key);
    }
  }
  while(0);
  return err;
}

int _X(xyz_open)(char *path)
{
  int err;
  int sess = 0;
  do
  {
    err = _X(init_service)(path, &data_shm_xyz, UTV_KHL_SZ);
    err = _X(xyz_register)(&sess);
  }
  while(0);

  return err;
}


int _X(init_service)(char *path, void **out, int sz)
{
  char create = 0;
  char *key = my_key;
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

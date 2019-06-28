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
  int cmd_id;
  char cmd;
} XYZ_COMMAND;

      //@totalbytes: 4 bytes (sizeof(int)), mandatory
      //@session:    4 bytes(sizeof(int)), mandatory
      //@processID:  4 bytes, mandatory
      //@cmdID:      4 bytes, mandatory
      //@cmdText:    unpredictable, optional
typedef struct
{
  pthread_mutex_t mtx;
  unsigned int lpid;
  int sz_cmds;
  char cmds;
}
XYZ_GROUP_COMMAND;

#define PRIVATE_NAME_UTV  "/5112052a-02a6-4818-ac42-2de914ef5700_"
#define UTV_KHL_MODE      (S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH)
#define UTV_KHL_SZ        ( 1024 * 1024)
#define UTV_CMD_SZ        ( 50 * 1024)
#define UTV_RES_SZ        ( UTV_KHL_SZ - UTV_CMD_SZ)
static void * data_shm_xyz = 0;
static char my_key[1024];


static int _X(init_service)(char *path, void **, int);
static int _X(init_shm_mtx)(char*);
static int _X(rem_sess)(int sess);
static int _X(xyz_register)(int *sess);
static int _X(xyz_cmd_fmt)(int sess, int cmd, 
  char *cmdtext, XYZ_COMMAND **out);
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
char *cmdtext, XYZ_COMMAND **out)
{
  int err = 0;
  int n = 0;
  int sz  = 0;
  do
  {
    XYZ_COMMAND *p = 0;
    if(!out)
    {
      err = __LINE__;
      break;
    }
    sz = cmdtext ? strlen(cmdtext) : 0;
    n = (int) (sizeof(XYZ_COMMAND) + sz + 1);
    p = realloc(p, n);
    if(!(p))
    {
      err = __LINE__;
      break;
    }
    memset(p, 0, n);
    //@totalbytes: 4 bytes (sizeof(int)), mandatory
    //@session:    4 bytes(sizeof(int)), mandatory
    //@processID:  4 bytes, mandatory
    //@cmdID:      4 bytes, mandatory
    //@cmdText:    unpredictable, optional
    p->nbyte = n;
    p->sess = sess;
    p->pid = (unsigned int) getpid();
    p->cmd_id = cmd;
    if(cmdtext)
    {
      memcpy(&(p->cmd), cmdtext, sz + 1);
    }
    *out = p;
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
    {
      char *cmd_mtx = (char*)(*out); 
      char *res_mtx = cmd_mtx + UTV_CMD_SZ;
      err  = _X(init_shm_mtx)(cmd_mtx);
      err  = _X(init_shm_mtx)(res_mtx);
    }
  }
  while(0);
  if(shm > -1)
  {
    err = close(shm); 
  }
  return err;
}

int _X(init_shm_mtx)(char *obj)
{
  int err = 0;
  pthread_mutex_t *shm_mtx = (pthread_mutex_t *) obj;
  do
  {
    if(!shm_mtx)
    {
      err = __LINE__;
      break;
    }
    pthread_mutex_t *shm_mtx = 0;

    pthread_mutexattr_t psharedm;
    pthread_mutexattr_init(&psharedm);
    pthread_mutexattr_setpshared(&psharedm,
        PTHREAD_PROCESS_SHARED);
    err = pthread_mutex_init(shm_mtx, &psharedm);                                                           
    if(err)
    {
      break;
    }
  }
  while(0);
  return err;
}

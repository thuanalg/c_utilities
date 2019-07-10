/*******************************************************************************
 * We apply while loop, because consumer can NOT wait for signal
 * And the shm should be have 2 parts, 
*******************************************************************************/
#include "mycode.h"
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h> /* For mode constants */
#include <fcntl.h> /* For O_* constants */
#include <pthread.h>
#include <sqlite3.h>


typedef int (*xyz_callback)(void *data, int argc, char **argv, char **azColName);
typedef enum {
  CM_REGISTER,
  CM_UNREGISTER,
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

static pthread_mutex_t *xyz_mtx_cmd = 0;

#define PRIVATE_NAME_UTV  "/5112052a-02a6-4818-ac42-2de914ef5700_"
#define UTV_KHL_MODE      (S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH)
#define UTV_TOTAL_SZ        ( 1024 * 1024)
#define UTV_CMD_SZ        	( 50   * 1024)

#define UTV_CMD_SEG   ( 0 )
#define UTV_RES_SEG   ( UTV_CMD_SZ )

static void * data_shm_xyz = 0;
static char my_key[1024];


static int _X(init_service)(char *path, void **, int);
static int _X(init_shm_mtx)(char*);
static int _X(rem_sess)(int sess);
static int _X(xyz_register)(int *sess);
static int _X(xyz_unregister)(int sess);
static int _X(xyz_cmd_fmt)(int sess, int cmdid, 
  char *cmdtext, XYZ_COMMAND **out);
static int _X(xyz_send_cmd)(XYZ_COMMAND *cmd);

int _X(xyz_exec)(int sess, const char *sql,  
  xyz_callback cb, void *data, char **errmsg);

/************************************************************************
* Format of memory segment
*      @COMMANFD|@DATARESULT
*      sizeof(@COMMAND) = 50 * 1024 (50 KB)
*      sizeof(@DATARESULT) = (1024 - 50) * 1024 == (1024 - 50) KB.
* @COMMAND detail:
*    mutex|processlockingId|totalcmdsize|list_commands
*    @mutex: sizeof(mutex), mandatory. Fixed size
     @processlockingId: process Id owns this segment memory. Fixed size =
                 sizeof(int)
*    @totalcmdsize: sizeof(int), the size of total current command(s)  
*                    ,mandatory. Fixed size.
*    @list_commands: a group command(s), dynamic size. 
*                      UTF-8 referrence
* @DATARESULT detail
    mutex|cmdid|processid|totalsize|the_UTF-8_chain
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

int _X(xyz_send_cmd)(XYZ_COMMAND *cmd)
{
  int err = 0;
  pthread_mutex_t *p = xyz_mtx_cmd;
  do
  {
    if(p)
    {
      err = __LINE__;
      break;
    }
    err = pthread_mutex_lock(p);
    if(err)
    {
      break;
    }
    {

    }
    err = pthread_mutex_unlock(p);
    if(err)
    {
      break;
    }
  }
  while(0);
  return err;
}

int _X(xyz_register)(int *sess)
{
  int err = 0;
  XYZ_COMMAND *cmdpre = 0;
  do
  {
    int pid = 0;
    if(!sess)
    {
      err = __LINE__;
      break;
    }
    pid = (int)getpid();
    //Send a registering command_. 
    //ntthuan
    //Session, command id, cmd text, output
    err = _X(xyz_cmd_fmt)(pid, CM_REGISTER, "", &cmdpre);
    if(err || !cmdpre)
    {
      break;
    }
    _X(xyz_send_cmd)(cmdpre);
    if(err)
    {
      break;
    }
    *sess = pid;
  }
  while(0);
  if(cmdpre)
  {
    free(cmdpre);
  }
  return err;
}

int _X(xyz_unregister)(int sess)
{
  int err = 0;
  do
  {
    //Send unregistering command_
		//Prepare command
		//
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
      err = munmap(data_shm_xyz, UTV_TOTAL_SZ);
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
    err = _X(init_service)(path, &data_shm_xyz, UTV_TOTAL_SZ);
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
//    err = ftruncate(shm, UTV_TOTAL_SZ);
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
    memset((*out), 0, UTV_TOTAL_SZ);
    {
      char *cmd_mtx = (char*)(*out); 
      char *res_mtx = cmd_mtx + UTV_RES_SEG;;
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

int _X(xyz_exec)(int sess, const char *sql, xyz_callback cb, void *data, char **errmsg)
{
  int err = 0;
  XYZ_COMMAND *out = 0;
  do
  {
    err = _X(xyz_cmd_fmt)(sess, CM_QUERY, (char*)sql, &out);
    if(err)
    {
      break;
    }
  }
  while(0);
  return err;
}

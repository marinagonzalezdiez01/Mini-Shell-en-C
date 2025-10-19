/*
 * --- PŔACTICA SISTEMAS OPERATIVOS ---
 * - Raquel Varela Romani -- Marina Gonzalez Diez
 */
#ifndef P2_COMMANDS_H
#define P2_COMMANDS_H

#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <sys/utsname.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <dirent.h>
#include <pwd.h>
#include <grp.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include <sys/errno.h>
#include <sys/mman.h>
#include <sys/resource.h>
#include <ctype.h>
#include <sys/ipc.h>
#include <sys/shm.h>

#include "list.h"
#include "memList.h"
#include "procList.h"

//#define PATH_MAX 4096
#define MAX_SIZE 256
#define MAX_BUFFER 2048
#define MAXVAR 2048
#define TAMANO 2048
#define MAX_DIRS 100
#define MAX_PATH_LEN 1024

typedef struct {
    char *dirs[MAX_DIRS];
    int count;
} SearchList;


void command_authors(char option[MAX_SIZE],int trozos);
void command_pid();
void command_ppid();
void command_cdDir(char option[MAX_SIZE], int trozos);
void command_date(char option[MAX_SIZE], int trozos);
void command_historic(char option[MAX_SIZE], int trozos, tList historic);
void command_listOpen(tList fileList);
void command_open(char file[MAX_SIZE], char option[MAX_SIZE], int trozos, tList *fileList);
void command_close(char option[MAX_SIZE], int trozos, tList *fileList);
void command_dup(char option[MAX_SIZE], int trozos, tList *fileList);
void command_infosys();
void command_help(char option[MAX_SIZE], int trozos);
void cwd();
void command_makeFile(int trozos,char param1[MAX_SIZE]);
void command_makeDir(int trozos,char param1[MAX_SIZE]);
void getName(char *ruta, char *nombre);
void longMode(char *directory, bool lnk, bool acc);
void shortMode(char *directory);
void date(time_t timeDir);
void directoryContent(char* param, bool lng, bool acc, bool lnk, bool hid, bool rec, bool rev);
void longList(char *param, bool lng, bool acc, bool lnk, bool hid, bool rec, bool rev);
void command_list(int trozos, char *param1[]);
void command_erase(int trozos, char *param1[]);
void command_delrec(int trozos, char *param1[]);
void revlist(char *param, bool hid, bool lng, bool acc, bool lnk, int depth);
void command_revlist(int trozos, char *param1[]);
void reclist(char *param, bool hid, bool lng, bool acc, bool lnk, int depth);
void command_reclist(int trozos, char *param1[]);
void command_allocate(char *param1[],int trozos,tListM *M);
void command_deallocate(char *param1[],int trozos,tListM *M);
void command_memfill(void *addr, size_t cont, unsigned char byte);
void command_memdump(void *addr, int nbytes);
void mostrarMemListCompleta(tListM M);
void command_memory(char *param[], int trozos, tListM *M);
void command_readfile(char *args[], int trozos);
void command_writefile(char *args[], int trozos);
void command_read(char *args[], int trozos);
void command_write(char *args[], int trozos);
void command_recurse(char *param[], int trozos);
void command_getuid();
void command_setuid(char *param[], int trozos);
void command_showvar(char *param[], int trozos, char * arg3[]);
void command_changevar (char *param[], int trozos, char *arg3[], char *env[]);
void command_subsvar(char *param[], int trozos, char *arg3[], char *env[]);
void command_environ(char *param[], int trozos, char *arg3[], char *env[]);
void command_fork(tListP *procList);
void command_listjobs(tListP *procList);
void command_deljobs(char *param[], int trozos, tListP *procList);


#endif
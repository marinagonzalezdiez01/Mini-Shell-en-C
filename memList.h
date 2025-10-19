/*
 * --- PŔACTICA SISTEMAS OPERATIVOS ---
 * - Raquel Varela Romani -- Marina Gonzalez Diez
 */
#ifndef MEMLIST_H
#define MEMLIST_H

#include <stdbool.h>
#include <stdlib.h>

#endif

#define LNULLM -1
#define MAX_SIZE_LISTM 1024

typedef struct file{
    int fd;
    char filename[256];
}tFile;

typedef enum cmd{
    MALLOC, SHARED, MMAP
}cmd;

typedef struct tItemM{
    cmd cmdType;
    void *dir;
    size_t size;
    char date[128];
    union{
        int key;
        tFile file;
        int shmid;
    }Union;
}tItemM;

typedef int tPosM;

typedef struct tListM{
    tItemM itemM[MAX_SIZE_LISTM];
    int lastPos;
}tListM;

void createEmptyListM(tListM* L);
bool insertItemM(tItemM d, tListM* L);
void deleteItemM(tPosM pos, tListM *L);
void deleteListM(tListM* L);
bool isEmptyListM(tListM L);
tItemM getItemM(tPosM p, tListM L);
tPosM firstM(tListM L) ;
tPosM lastM(tListM L);
tPosM nextM(tPosM p, tListM L);
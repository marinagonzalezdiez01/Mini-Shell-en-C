/*
 * --- PŔACTICA SISTEMAS OPERATIVOS ---
 * - Raquel Varela Romani -- Marina Gonzalez Diez
 */
#ifndef LIST_H
#define LIST_H
#define LNULL NULL
#define MAX_SIZE_LIST 1024

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>


typedef struct{
    int pos;
    char com[MAX_SIZE_LIST];
}tItem;

typedef struct tNode *tPos;

struct tNode {
    tItem item;
    tPos next;
};

typedef tPos tList;

void createEmptyList(tList* L);
bool isEmptyList(tList L );
tPos first(tList L);
tPos last(tList L);
tPos next(tPos p, tList L);
tPos previous(tPos p, tList L);
bool insertItem(tItem d, tList* L);
tList deleteAtPosition(tPos p, tList* L);
tItem getItem(tPos p, tList L );
tList updateItem(tItem d, tPos p, tList* L);
tPos findItem(tItem d, tList L);
void deleteList(tList* L);
int listSize(tList L);

#endif //LIST_H
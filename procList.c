/*
 * --- PŔACTICA SISTEMAS OPERATIVOS ---
 * - Raquel Varela Romani -- Marina Gonzalez Diez
 */
#include "procList.h"


void createEmptyListP(tListP* L) {
    L->lastPos = -1;
}

bool isEmptyListP(tListP L) {
    return L.lastPos == -1;
}

tPosLP firstP(tListP L){
    return 0;
}
tPosLP lastP(tListP L){
    return L.lastPos;
}

tItemLP getItemP(tPosLP p, tListP L) {
    return L.itemP[p];
}

void deleteItemP(tPosLP pos, tListP *L) {
    for (int i = 0; i < lastP(*L); i++) {
        if (i >= pos) {
            L->itemP[i] = L->itemP[i + 1];
        }
    }
    L->lastPos--;
}

void deleteListP(tListP* L){
    for (int i = 0; i <= L->lastPos; i++) {
        deleteItemP(i, L);
        L->lastPos--;
    }
}

bool insertItemP(tItemLP i, tListP *L) {
    bool fin = false;
    if (L->lastPos < MAX_SIZE_LISTP) {
        L->lastPos++;
        L->itemP[L->lastPos] = i;
        fin = true;
    } else {
        fin = false;
    }
    return fin;
}
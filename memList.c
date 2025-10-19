/*
 * --- PŔACTICA SISTEMAS OPERATIVOS ---
 * - Raquel Varela Romani -- Marina Gonzalez Diez
 */
#include <stdlib.h>
#include <stdbool.h>
#include "memList.h"

void createEmptyListM(tListM *L) {
    L->lastPos = -1;
}

bool isEmptyListM(tListM L) {
    return L.lastPos == -1;
}

tPosM firstM(tListM L) {
    return 0;
}

tPosM lastM(tListM L) {
    return L.lastPos;
}

tItemM getItemM(tPosM pos, tListM L) {
    return L.itemM[pos];
}

void deleteItemM(tPosM pos, tListM *L) {
    for (int i = 0; i < lastM(*L); i++) {
        if (i >= pos) {
            L->itemM[i] = L->itemM[i + 1];
        }
    }
    L->lastPos--;
}

void deleteListM(tListM *L) {
    for (int i = 0; i <= L->lastPos; i++) {
        deleteItemM(i, L);
        L->lastPos--;
    }
}

bool insertItemM(tItemM i, tListM *L) {
    bool end = false;
    if (L->lastPos < MAX_SIZE_LISTM) {
        L->lastPos++;
        L->itemM[L->lastPos] = i;
        end = true;
    } else {
        end = false;
    }
    return end;
}
tPosM nextM(tPosM p, tListM M) {
    // Aquí va la lógica para devolver el siguiente elemento de la lista
    if (p == lastM(M)) { // Si estamos en el último elemento
        return LNULLM; // No hay más elementos
    }
    return p + 1; // Asumiendo que tPosM es un índice entero
}

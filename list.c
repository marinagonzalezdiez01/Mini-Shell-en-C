/*
 * --- PŔACTICA SISTEMAS OPERATIVOS ---
 * - Raquel Varela Romani -- Marina Gonzalez Diez
 */
#include "list.h"

void createEmptyList(tList* L){
    *L = LNULL;
}
bool isEmptyList (tList L){
    return (L==LNULL);
}
tPos first (tList L){
    return L;
}
tPos last (tList L){
    tPos p = L;
    while (p != LNULL && p->next != LNULL) {
        p = p->next;
    }
    return p;
}
tPos next (tPos p, tList L){
    return p->next;
}
tPos previous (tPos p, tList L){
    tPos q = L;
    while (q != LNULL && q->next != p) {
        q = q->next;
    }
    return q;
}
bool createNode (tPos* p){
    *p = malloc(sizeof(struct tNode));
    return *p!=LNULL;
}

bool insertItem(tItem d, tList* L) {
    tPos q,r;
    tPos p;
    if(!createNode(&q))
        return false;
    else {
        q->item = d;
        q->next = LNULL;
        if (*L == LNULL){
            *L = q;
        }else if(d.pos < (*L)->item.pos){
            q->next = *L;
            *L = q;
        }else{
            p = findItem(d,*L);
            if(p==LNULL){
                for (r=*L; r->next != LNULL; r=r->next);
                r->next = q;
            }else{
                q->item = p->item;
                p->item = d;
                q->next = p->next;
                p->next = q;
            }
        }
        return true;
    }
}

tList deleteAtPosition (tPos p, tList *L){
    tPos q;
    if (p == *L){
        *L= (*L)->next;
    }else if (p->next ==LNULL){
        for (q = *L; q->next!=p; q = q->next);
        q->next = LNULL;
    }else{
        q = p->next;
        p->item = q->item;
        p->next = q->next;
        p = q;
    }
    free(p);
    return *L;
}
tItem getItem (tPos p, tList L){
    return p->item;
}
tList updateItem (tItem d, tPos p, tList *L){
    p->item = d;
    return *L;
}

tPos findItem (tItem d, tList L) {
    tPos p;
    if (L == LNULL)
        return LNULL;
    else {
        for (p = L; (p != LNULL) && (p->item.pos != d.pos); p = p->next);
        if (p == LNULL) {
            return LNULL;
        }
        return p;
    }
}

void deleteList(tList *L) {
    tPos p = *L;
    tPos next;
    while (p!=NULL) {
        next=p->next;
        free(p);
        p=next;
    }
    *L=NULL;
}

int listSize(tList L){
    int size=0;
    tPos pos= first(L);
    if(isEmptyList(L)){
        return 0;
    }
    while(pos!=LNULL){
        size++;
        pos= next(pos,L);
    }
    return size;
}
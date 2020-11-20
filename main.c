#include "list.h"
#include "io.h"
#include <stdlib.h>

/*Вершина - функция, которую надо вызвать следующей*/
typedef void * (*vertex)();

void * start();
void * word();
void * specsym(char cprev);
void * specsym2();
void * newline();
void * stop();

int main() {
    vertex V=start;
    c=getchar();
    null_list();
    while(1)
        V=V();
}
void* start() {
    if(c==' '||c=='\t') { c=getchar(); return start;}
    else if (c==EOF) {
        termlist();
        printlist();
        clearlist();
        return stop;
    }
    else if (c=='\n') {
        termlist();
        printsize();
        printlist();
        bubblesort();
        printlist();
        c=getchar();
        return newline;
    }
    else { char cprev=c;
        nullbuf();
        addsym();
        c = getchar();
        if (symset(cprev))
            return word;
        else
            return specsym(cprev);  
    }
}
void* word(){
    if(symset(c)) {
        addsym();
        c=getchar();
        return word;
    }
    else {
        addword();
        return start;
    }
}

void* specsym(char cprev) {
    switch (c){
        case '>': 
        case '|':
        case '&':
            if (c == cprev){
                addsym();
                c = getchar();
                return specsym2;
            }
            /* FALLTHROUGH */
        default:
            addword();
            return start;
    }
}
void* specsym2() {
    addword();
    return start;
}

void* newline(){
    clearlist();
    return start;
}
void* stop(){exit(0);}

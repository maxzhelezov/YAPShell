#include "list.h"

#include <stdlib.h>
#include <string.h>
#include "io.h"
#include <errno.h>
#define SIZE 16

int c; /*текущий символ */
char ** lst; /* список слов (в виде массива)*/
char * buf; /* буфер для накопления текущего слова*/
int sizebuf; /* размер буфера текущего слова*/
int sizelist; /* размер списка слов*/
int curbuf; /* индекс текущего символа в буфере*/
int curlist; /* индекс текущего слова в списке*/

void clearlist(){
    int i;
    sizelist=0;
    curlist=0;
    if (lst==NULL) return;
    for (i=0; lst[i]!=NULL; i++)
        free(lst[i]);
    free(lst);
    lst=NULL;
}
void null_list(){
    sizelist=0;
    curlist=0;
    lst=NULL;
}
void termlist(){
    if (lst==NULL) return;
    if (curlist>sizelist-1){
        errno = 0;
        lst=realloc(lst,(sizelist+1)*sizeof(*lst));
        if (lst == NULL)
            perr(strerror(errno));
    }
    lst[curlist]=NULL;
    /*выравниваем используемую под список память точно по размеру списка*/
    errno = 0;
    lst=realloc(lst,(sizelist=curlist+1)*sizeof(*lst));
    if (lst == NULL)
       perr(strerror(errno)); 
}
void nullbuf(){
    buf=NULL;
    sizebuf=0;
    curbuf=0;
}
void addsym(){
    if (curbuf>sizebuf-1){
        errno = 0;
        buf=realloc(buf, sizebuf+=SIZE);
        if (buf == NULL)
            perr(strerror(errno));
    }
    buf[curbuf++]=c;
}
void addword(){
    if (curbuf>sizebuf-1)
        buf=realloc(buf, sizebuf+=1); /* для записи ’\0’ увеличиваем буфер
                                         при необходимости */
    buf[curbuf++]='\0';
    /*выравниваем используемую память точно по размеру слова*/
    errno = 0;
    buf=realloc(buf,sizebuf=curbuf);
    if (buf == NULL)
        perr(strerror(errno));
    if (curlist>sizelist-1){
        errno = 0;
        lst=realloc(lst, (sizelist+=SIZE)*sizeof(*lst)); 
        if (lst == NULL)
            perr(strerror(errno));
    }
    lst[curlist++]=buf;
}

void printsize(){
    printstr("Длина списка: ");
    if (sizelist == 0)
        printint(0);
    else
        printint(sizelist - 1);
    printstr("\n");
}

void printlist(){
    int i;
    if (lst==NULL) return;
        for (i=0; i<sizelist-1; i++){
        printstr(lst[i]);
        printstr(" ");
    }
    printstr("\n");
}
int symset(int c){
    return c!='\n' &&
        c!=' ' &&
        c!='\t' &&
        c!='>' &&
        c!='|' &&
        c!='&' &&
        c!='<' &&
        c!='(' &&
        c!=')' &&
        c!=';' &&
        c!= EOF ;
}

int strcmpl(char * a, char * b){
    for( ; *a == *b; ++a, ++b ){
        if ( *a == '\0' )
            return 1;
        if ( *b == '\0' )
            return 0;
    }
    return ((unsigned char) *a > (unsigned char) *b);
}

void bubblesort(){
    int i,j;
    char * temp;
    for(i = 0; i < sizelist - 2; i++)
        for(j = 0; j < sizelist - i - 2; j++)
            if (strcmpl(lst[j], lst[j+1])){
                temp = lst[j];
                lst[j] = lst[j+1];
                lst[j+1] = temp;
            }
}

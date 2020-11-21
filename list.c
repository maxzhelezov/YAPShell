#include "list.h"

#include <stdlib.h>
#include <string.h>
#include "io.h"
#include <errno.h>
#define SIZE 16

static int c; /*текущий символ */
list lst; /* список слов (в виде массива)*/
static char * buf; /* буфер для накопления текущего слова*/
static int sizebuf; /* размер буфера текущего слова*/
static int sizelist; /* размер списка слов*/
static int curbuf; /* индекс текущего символа в буфере*/
static int curlist; /* индекс текущего слова в списке*/

void clearlist()
{
    int i;
    sizelist=0;
    curlist=0;
    if (lst==NULL) return;
    for (i=0; lst[i]!=NULL; i++)
        free(lst[i]);
    free(lst);
    lst=NULL;
}
static void null_list()
{
    sizelist=0;
    curlist=0;
    lst=NULL;
}
static void termlist()
{
    if (lst==NULL) return;
    if (curlist>sizelist-1)
    {
        errno = 0;
        lst=realloc(lst,(sizelist+1)*sizeof(*lst));
        if (lst == NULL)
            perror(strerror(errno));
    }
    lst[curlist]=NULL;
    /*выравниваем используемую под список память точно по размеру списка*/
    errno = 0;
    lst=realloc(lst,(sizelist=curlist+1)*sizeof(*lst));
    if (lst == NULL)
        perror(strerror(errno));
}
static void nullbuf()
{
    buf=NULL;
    sizebuf=0;
    curbuf=0;
}
static void addsym()
{
    if (curbuf>sizebuf-1)
    {
        errno = 0;
        buf=realloc(buf, sizebuf+=SIZE);
        if (buf == NULL)
            perror(strerror(errno));
    }
    buf[curbuf++]=c;
}
static void addword()
{
    if (curbuf>sizebuf-1)
        buf=realloc(buf, sizebuf+=1); /* для записи ’\0’ увеличиваем буфер
                                         при необходимости */
    buf[curbuf++]='\0';
    /*выравниваем используемую память точно по размеру слова*/
    errno = 0;
    buf=realloc(buf,sizebuf=curbuf);
    if (buf == NULL)
        perror(strerror(errno));
    if (curlist>sizelist-1)
    {
        errno = 0;
        lst=realloc(lst, (sizelist+=SIZE)*sizeof(*lst));
        if (lst == NULL)
            perror(strerror(errno));
    }
    lst[curlist++]=buf;
}

void printlist()
{
    int i;
    if (lst==NULL) return;
    for (i=0; i<sizelist-1; i++)
    {
        printstr(lst[i]);
        printstr(" ");
    }
    printstr("\n");
}
static int symset(int c)
{
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
           c!='"' &&
           c!= EOF ;
}
/*Построение списка*/

/*Вершина - функция, которую надо вызвать следующей*/
typedef void * (*vertex)();

static void * start();
static void * word();
static void * specsym(char cprev);
static void * specsym2();
static void * stop();
static int stop_flag = 0;

void buildlist()
{
    vertex V=start;
    c=getchar();
    null_list();
    while(!stop_flag)
        V=V();
}
static void* start()
{
    if(c==' '||c=='\t')
    {
        c=getchar();
        return start;
    }
    if (c == EOF){
        termlist();
        return stop;

    }
    else if (c=='\n')
    {
        termlist();
        return stop;
    }
    else
    {
        char cprev=c;
        nullbuf();
        addsym();
        c = getchar();
        if (symset(cprev))
            return word;
        else
            return specsym(cprev);
    }
}

static void* word()
{
    if(symset(c))
    {
        addsym();
        c=getchar();
        return word;
    }
    else
    {
        addword();
        return start;
    }
}

static void* specsym(char cprev)
{
    switch (c)
    {
    case '>':
    case '|':
    case '&':
        if (c == cprev)
        {
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
static void* specsym2()
{
    addword();
    return start;
}

static void* stop()
{
    stop_flag = 1;
    return NULL;
}

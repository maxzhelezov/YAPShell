#include "list.h"

#include <stdlib.h>
#include <string.h>
#include "io.h"
#include <errno.h>
#define SIZE 16

/* Переменные используемые совместно функциями */
static int c; /* Текущий символ */
static list lst; /* Список слов (в виде массива) */
static char * buf; /* Буфер для накопления текущего слова */
static int size_buf; /* Размер буфера текущего слова */
static int size_list; /* Размер списка слов */
static int cur_buf; /* Индекс текущего символа в буфере */
static int cur_list; /* Индекс текущего слова в списке */

/* Вспомогательные функции необходимые при работе вершин L-графа*/
static void null_list(); /* Инициализирует lst */
static void term_list(); /* Завершает lst */
static void null_buf(); /* Инициализирует buf */
static void add_sym(); /* Добавляет символ c в buf */
static void add_word(); /* Добавляет слово из buf в lst */
static int sym_set(int c); /* Набор специальных символов */

/* Вершина - функция, которую надо вызвать следующей */
typedef void * (*vertex)();

/* Описание вершин L-графа */
static void * start(); /* Начало L-графа */
static void * word(); /* Вершина для рапознования слова */
static void * spec_sym(char cprev); /* Вершина для определения одного 
                                       специального символа */
static void * spec_sym2(); /* Вершина для определения двойных специальных 
                              символов */
static void * stop(); /* Вершина останова анализа */
static int stop_flag = 0; /* Флаг остановки */


/* Процедура построения списка lst */
list build_list()
{
    vertex V = start;
    c = get_char();
    null_list();
    stop_flag = 0;
    while(!stop_flag)
        V = V();
    return lst; 
}

/* Процедура печати списка lst */
void print_list(list lst_loc)
{
    int i;
    lst = lst_loc;
    if(lst == NULL) return;
    for(i = 0; lst[i] != NULL; i++)
    {
        print_str(lst[i]);
        print_str(" ");
    }
    print_str("\n");
}

/* Процедура овобождения памяти из-под списка lst */
void clear_list(list lst_loc)
{
    int i;
    lst = lst_loc;
    size_list = 0;
    cur_list = 0;
    if(lst == NULL) return;
    for(i = 0; lst[i] != NULL; i++)
        free(lst[i]);
    free(lst);
    lst = NULL;
}

/* Процедура иницаиализация списка lst и подготовка к его заполнению */
static void null_list()
{
    size_list = 0;
    cur_list = 0;
    lst = NULL;
}

/* Завершает создание списка слов lst */
static void term_list()
{
    if(lst == NULL) return;
    if(cur_list > size_list - 1)
    {
        errno = 0;
        lst = realloc(lst, (size_list + 1) * sizeof(*lst));
        if(lst == NULL)
            perr(strerror(errno));
    }
    lst[cur_list] = NULL;
    /* Выравниваем используемую под список память точно по размеру списка */
    errno = 0;
    lst = realloc(lst, (size_list = cur_list + 1) * sizeof(*lst));
    if(lst == NULL)
        perr(strerror(errno));
}

/* Инициализация буфера buf текущего слова и подготовка к его заполнению */
static void null_buf()
{
    buf = NULL;
    size_buf = 0;
    cur_buf = 0;
}

/* Процедура добавления символа в буфер текущего слова buf */
static void add_sym()
{
    if(cur_buf > size_buf - 1)
    {
        errno = 0;
        buf = realloc(buf, size_buf += SIZE);
        if(buf == NULL)
            perr(strerror(errno));
    }
    buf[cur_buf++] = c;
}

/* Процедура добавления слова из buf в список слов lst */
static void add_word()
{
    if(cur_buf > size_buf - 1)
        buf = realloc(buf, size_buf += 1); /* Для записи ’\0’ увеличиваем буфер
                                              при необходимости */
    buf[cur_buf++] = '\0';
    /* Выравниваем используемую память точно по размеру слова */
    errno = 0;
    buf = realloc(buf, size_buf = cur_buf);
    if(buf == NULL)
        perr(strerror(errno));
    if(cur_list > size_list - 1)
    {
        errno = 0;
        lst = realloc(lst, (size_list += SIZE) * sizeof(*lst));
        if(lst == NULL)
            perr(strerror(errno));
    }
    lst[cur_list++] = buf;
}

/* Функция проверки символа на специальный */
static int sym_set(int c)
{
    return c != '\n' &&
           c != ' ' &&
           c != '\t' &&
           c != '>' &&
           c != '|' &&
           c != '&' &&
           c != '<' &&
           c != '(' &&
           c != ')' &&
           c != ';' &&
           c != '"' &&
           c != EOF ;
}

static void* start()
{
    if(c == ' ' || c == '\t')
    {
        c = get_char();
        return start;
    }
    else if(c == EOF || c == '\n')
    {
        term_list();
        return stop;
    }
    else
    {
        char cprev = c;
        null_buf();
        add_sym();
        c = get_char();
        if(sym_set(cprev))
            return word;
        else
            return spec_sym(cprev);
    }
}

static void* word()
{
    if(sym_set(c))
    {
        add_sym();
        c = get_char();
        return word;
    }
    else
    {
        add_word();
        return start;
    }
}

static void* spec_sym(char cprev)
{
    switch (c)
    {
    case '>':
    case '|':
    case '&':
        if(c == cprev)
        {
            add_sym();
            c = get_char();
            return spec_sym2;
        }
    /* FALLTHROUGH */
    default:
        add_word();
        return start;
    }
}
static void* spec_sym2()
{
    add_word();
    return start;
}

static void* stop()
{
    stop_flag = 1;
    return NULL;
}

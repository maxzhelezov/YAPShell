#include "tree.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#define SIZE 16

/* Общие переменные внутри модуля */
static tree beg_cmd, cur_cmd, prev_cmd; /* Начальная команда, текущая и
                                           предыдущая */
static int end_flag; /* Флаг останова */
static list lst; /* Список слов */
static int cur_list; /* Текущая позиция в нем */
static int argv_cur_size, argv_max_size; /* Текущие размеры argv в cur_cmd */

/* Тип - следующая вершина графа*/
typedef void *(*vertex)();

/* Вершины L-графа */
static void * begin();
static void * conv();
static void * conv2();
static void * subin();
static void * subout();
static void * in(int apnd);
static void * out();
static void * backgrnd();
static void * next(enum type_of_next nxt);
static void * end();
static void * error();

static tree make_cmd(); /* Создает дерево из одного элемента с пустыми полями */
static void make_bgrnd(tree t); /* Устанаваливает поле конвеера background = 1 
                                  во всех команда конвеера t */
static char * add_argv(); /* Добавляет очередной элемент в массив argv текущей
                             команды, а также возвращает добавленный элемент */
static void init(); /* Инициализация */
static int check_spec(char* s); /* Проверяет является ли строка спец символом*/
static void term_argv(); /* Завершает запись в argv cur_cmd */

tree build_tree(list lst_loc){
    vertex V = begin;
    init();
    lst = lst_loc;
    while (!end_flag){
        V = V();
    }
    return beg_cmd;
}

static void init(){
    beg_cmd = NULL;
    cur_cmd = NULL;
    prev_cmd = NULL;
    end_flag = 0;
    cur_list = 0;
    argv_cur_size = 0;
    argv_max_size = 0;
}

static tree make_cmd(){
    tree temp;
    temp = malloc(sizeof(node));
    temp -> argv = NULL;
    temp -> infile = NULL;
    temp -> outfile = NULL;
    temp -> append = 0;
    temp -> backgrnd = 0;
    temp -> psubcmd = NULL;
    temp -> pipe = NULL;
    temp -> next = NULL;
    temp -> type = NXT;
    return temp;
}

static void make_bgrnd(tree t){
    while (t -> pipe != NULL){
        t -> backgrnd = 1;
        t = t -> pipe;  
    } 
}


static char * add_argv(){
    if (argv_cur_size > argv_max_size - 1)
        cur_cmd -> argv = realloc(cur_cmd -> argv,
                (argv_max_size += SIZE)*sizeof(*(cur_cmd -> argv))); 
    cur_cmd -> argv[argv_cur_size] = malloc(strlen(lst[cur_list]) + 1);
    strcpy(cur_cmd -> argv[argv_cur_size++] , lst[cur_list++]);
    return lst[cur_list - 1];
}

static int check_spec(char *s){
    if (strlen(s) == 1)
        switch (s[0]){
            case '|': case '<': case '>': case '&': 
            case ';': case '(': case ')': return 1;
        }
    if (strlen(s) == 2)
        switch (s[0]){
            case '&': return s[1] == '&' ? 1 : 0;
            case '|': return s[1] == '|' ? 1 : 0;
            case '>': return s[1] == '>' ? 1 : 0;
        }
    return 0;
}

static void term_argv(){
    if (argv_cur_size < argv_max_size - 1)
        cur_cmd -> argv = realloc(cur_cmd -> argv,
                (++argv_cur_size)*sizeof(*(cur_cmd -> argv)));
    cur_cmd -> argv[argv_cur_size - 1] = NULL;
    argv_cur_size = 0;
    argv_max_size = 0;
}

static void * begin(){
    char *s;
    beg_cmd = cur_cmd = make_cmd();
    s = add_argv();
    if (!check_spec(s))
        return conv;
    else
        return error;
}

static void * conv(){
    char *s;
    s = lst[cur_list];
    if (s == NULL)
        return end;
    if (!check_spec(s))
    {
        add_argv();
        return conv;
    }
    cur_list++;
    if (strlen(s) == 1)
        switch (s[0]){
            case '|': return conv2;
            case '<': return in(0);
            case '>': return out;
            case '&': return backgrnd;
            case ';': return next(NXT);
            case '(': return subin();
            case ')': return subout();
        }
    if (strlen(s) == 2)
        switch (s[0]){
            case '&': return s[1] == '&' ? (vertex)next(AND) : error;  
            case '|': return s[1] == '|' ? (vertex)next(OR) : error;
            case '>': return s[1] == '>' ? (vertex)in(1) : error;
        }
    return error;
}

static void * conv2(){
    char *s;
    term_argv();
    prev_cmd = cur_cmd;
    cur_cmd = make_cmd();
    s = lst[cur_list];
    if (!check_spec(s)){
        prev_cmd -> pipe = cur_cmd;
        prev_cmd = cur_cmd;
        add_argv();
        return conv;
    }
    return error;
}

static void * next(enum type_of_next nxt){
    char *s;
    term_argv();
    s = lst[cur_list];   
    if (!check_spec(s)){
        prev_cmd = cur_cmd;
        cur_cmd = make_cmd();
        prev_cmd -> next = cur_cmd;
        prev_cmd -> type = nxt;
        add_argv();
        return conv;
    }
    return error;
}

static void * in(int apnd){
    char *s = lst[cur_list++];
    if (!check_spec(s)){
        cur_cmd -> infile = s;
        cur_cmd -> append = apnd; 
        return conv;
    }
    return error;
}

static void * out(){
    char *s = lst[cur_list++];
    if (!check_spec(s)){
        cur_cmd -> outfile = s;
        return conv;
    }
    return error;
}

static void * backgrnd(){
    return conv;
}

static void * subout(){
    return conv;
}

static void * subin(){
    return conv;
}

static void * error(){
    printf("Ошибка синтаксиса :(\n");
    end_flag = 1;
    return conv;
}

static void * end(){
    end_flag = 1;
    return end;
}

static void make_shift(int n){
    while(n--)
        putc(' ', stderr);
}

static void print_argv(char **p, int shift){
    char **q=p;
    if(p!=NULL){
        while(*p!=NULL){
             make_shift(shift);
             fprintf(stderr, "argv[%d]=%s\n",(int) (p-q), *p);
             p++;
        }
    }
}

void print_tree(tree t, int shift){
    char **p;
    if(t==NULL)
        return;
    p=t->argv;
    if(p!=NULL)
        print_argv(p, shift);
    else{
        make_shift(shift);
        fprintf(stderr, "psubshell\n");
    }
    make_shift(shift);
    if(t->infile==NULL)
        fprintf(stderr, "infile=NULL\n");
    else
        fprintf(stderr, "infile=%s\n", t->infile);
    make_shift(shift);
    if(t->outfile==NULL)
        fprintf(stderr, "outfile=NULL\n");
    else
        fprintf(stderr, "outfile=%s\n", t->outfile);
    make_shift(shift);
    fprintf(stderr, "append=%d\n", t->append);
    make_shift(shift);
    fprintf(stderr, "background=%d\n", t->backgrnd);
    make_shift(shift);
    fprintf(stderr, "type=%s\n", t->type==NXT?"NXT": t->type==OR?"OR": "AND" );
    make_shift(shift);
    if(t->psubcmd==NULL)
        fprintf(stderr, "psubcmd=NULL \n");
    else{
        fprintf(stderr, "psubcmd---> \n");
        print_tree(t->psubcmd, shift+5);
    }
    make_shift(shift);
    if(t->pipe==NULL)
        fprintf(stderr, "pipe=NULL \n");
    else{
        fprintf(stderr, "pipe---> \n");
        print_tree(t->pipe, shift+5);
    }
    make_shift(shift);
    if(t->next==NULL)
        fprintf(stderr, "next=NULL \n");
    else{
        fprintf(stderr, "next---> \n");
        print_tree(t->next, shift+5);
    }
}
 
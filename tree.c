#include "tree.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#define SIZE 16

/* Общие переменные внутри модуля */
static tree beg_cmd, cur_cmd, prev_cmd, back_cmd; /* Начальная команда, текущая,
                                                     предыдущая и начало фоновых */
static int end_flag; /* Флаг останова */
static list lst; /* Список слов */
static int cur_list; /* Текущая позиция в нем */
static int argv_cur_size, argv_max_size; /* Текущие размеры argv в cur_cmd */
static int parnts; /* Счетчик скобок */

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
static void * error(char *s); /* Ошибка построения дерева */

/* Впомогательные функции для работы build_tree */
static tree build_tree_recursive(); /* Рекурсивная функция построения дерева 
                                       для вызова изнутри */
static tree make_cmd(); /* Создает дерево из одного элемента с пустыми полями */
static void make_bgrnd(tree t); /* Устанаваливает поле конвеера background = 1 
                                  во всех команда конвеера t */
static char * add_argv(); /* Добавляет очередной элемент в массив argv текущей
                             команды, а также возвращает добавленный элемент */
static void init(); /* Инициализация */
static int check_spec(char* s); /* Проверяет является ли строка спец символом 
                                   кроме скобок */
static int check_parnts(char* s); /* Проверяет является ли строка скобкой */
static void term_argv(); /* Завершает запись в argv cur_cmd */

/* Вспомогательные функции для работы print_tree */
static void make_shift(int n); /* Сдвигает вывод на n пробелов */
static void print_argv(char **t, int shift); /* Печатает аргументы из списка t */


tree build_tree(list lst_loc){
    vertex V = begin;
    if (lst_loc == NULL) return NULL;
    init();
    beg_cmd = cur_cmd = back_cmd = make_cmd();
    lst = lst_loc;
    while (!end_flag){
        V = V();
    }
    if (parnts != 0)
        error("Несоответсвие скобок");
    return beg_cmd;
}

static tree build_tree_recursive(){
    /* Упрятывание значений в локальных переменных ради рекурсии */
    tree old_beg, old_cur, old_prev, sub_tree, old_back;
    int old_list = cur_list, old_end = end_flag;
    int old_argv_cur = argv_cur_size, old_argv_max = argv_max_size;
    vertex V = begin;
    
    old_beg = beg_cmd; old_cur = cur_cmd; old_prev = prev_cmd; old_back = back_cmd;
    init();
    cur_list = old_list;
    beg_cmd = cur_cmd = back_cmd = make_cmd();
    while (!end_flag)
        V = V();
    sub_tree = beg_cmd;
    beg_cmd = old_beg; cur_cmd = old_cur; prev_cmd = old_prev; back_cmd = old_back;
    argv_cur_size = old_argv_cur; argv_max_size = old_argv_max;
    end_flag = old_end;
    return sub_tree;
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
    tree temp = t;
    while (t -> pipe != NULL){
        t -> backgrnd = 1;
        t = t -> pipe;  
    }
    t = temp;
    while (t -> next != NULL){
        t -> backgrnd = 1;
        t = t -> next;
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
            case ';': return 1;
        }
    if (strlen(s) == 2)
        switch (s[0]){
            case '&': return s[1] == '&' ? 1 : 0;
            case '|': return s[1] == '|' ? 1 : 0;
            case '>': return s[1] == '>' ? 1 : 0;
        }
    return 0;
}

static int check_parnts(char *s){
    if (strlen(s) == 1)
        if (s[0] == '(' || s[0] == ')')
            return 1;
    return 0;

}

static void term_argv(){
    if (cur_cmd -> argv == NULL) return;
    if (argv_cur_size < argv_max_size - 1){
        cur_cmd -> argv = realloc(cur_cmd -> argv,
                (++argv_cur_size)*sizeof(*(cur_cmd -> argv)));
        cur_cmd -> argv[argv_cur_size - 1] = NULL;
    }
    argv_cur_size = 0;
    argv_max_size = 0;
}

static void * begin(){
    char *s;
    s = lst[cur_list];
    if (check_parnts(s)){
        if (s[0] == ')') return error("Пустые скобки");
        cur_list++;
        return subin;
    }
    if (!check_spec(s)){
        add_argv();
        return conv;
    }
    else
        return error("Ожидался аргумент, а не управляющий символ");
}

static void * conv(){
    char *s;
    s = lst[cur_list];
    if (s == NULL)
        return end;
    if (!(check_spec(s) || check_parnts(s)))
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
            case '(': return subin;
            case ')': return subout;
        }
    if (strlen(s) == 2)
        switch (s[0]){
            case '&': return s[1] == '&' ? (vertex)next(AND) : error;  
            case '|': return s[1] == '|' ? (vertex)next(OR) : error;
            case '>': return s[1] == '>' ? (vertex)in(1) : error;
        }
    return error("Произошло что-то странное");
}

static void * conv2(){
    char *s = lst[cur_list];
    if (s == NULL) return error("Ожидалась команда после |");
    if (!check_spec(s)){
        term_argv();
        prev_cmd = cur_cmd;
        cur_cmd = make_cmd();
        prev_cmd -> pipe = cur_cmd;
        if (!check_parnts(s))
            add_argv();
        return conv;
    }
    return error("Ожидалась команда после |, а не спец символ");
}

static void * next(enum type_of_next nxt){
    char *s;
    term_argv();
    s = lst[cur_list];
    if (s == NULL) return error("Ожидалась команда после ||, && или ;");   
    if (!check_spec(s)){
        prev_cmd = cur_cmd;
        cur_cmd = make_cmd();
        prev_cmd -> next = cur_cmd;
        prev_cmd -> type = nxt;
        if (nxt == NXT)
            back_cmd = cur_cmd;
        if (!check_parnts(s))
            add_argv();
        return conv;
    }
    return error("Ожидалась команда, а не спец символ после ||, && или ;");
}

static void * in(int apnd){
    char *s = lst[cur_list++];
    if (s == NULL)
        return error("Ожидался аргумент после <");
    if (!check_spec(s)){
        cur_cmd -> infile = realloc(cur_cmd -> infile, strlen(s) + 1);
        strcpy(cur_cmd -> infile, s);
        cur_cmd -> append = apnd; 
        return conv;
    }
    return error("Ожидался аргумент(файл), а не специальный символ");
}

static void * out(){
    char *s = lst[cur_list++];
    if (s == NULL)
        return error("Ожидался аргумент после >");
    if (!check_spec(s)){
        cur_cmd -> infile = realloc(cur_cmd -> infile, strlen(s) + 1);
        strcpy(cur_cmd -> infile, s);    
        return conv;
    }
    return error("Ожидался аргумент(файл), а не специальный символ");
}

static void * backgrnd(){
    make_bgrnd(back_cmd);
    cur_cmd -> backgrnd = 1;
    if (lst[cur_list] == NULL)
        return end;
    if (!strcmp(lst[cur_list], ")")){
        cur_list++;
        return subout;
    }
    else
        return next(NXT);
}

static void * subout(){
    if (parnts <= 0)
        return error("Несоответсвие скобок");
    parnts--;
    return end;
}

static void * subin(){
    parnts++;
    back_cmd = cur_cmd;
    cur_cmd -> psubcmd = build_tree_recursive();
    return conv;
}

static void * error(char *s){
    fprintf(stderr, "Ошибка синтаксиса : %s \n", s);
    term_argv();
    clear_tree(beg_cmd);
    beg_cmd = NULL;
    end_flag = 1;
    return conv;
}

static void * end(){
    term_argv();
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
 
void clear_tree(tree t){
    int i = 0;
    if (t == NULL) return;
    if (t -> psubcmd != NULL)
        clear_tree(t -> psubcmd);
    if (t -> next != NULL)
        clear_tree(t -> next);
    if (t -> pipe != NULL)
        clear_tree(t -> pipe);
    if (t -> argv != NULL){
        for(i = 0; t -> argv[i] != NULL; i++)
                free(t -> argv[i]);
        free(t -> argv);
    }
    free(t -> infile);
    free(t -> outfile);
    free(t);
}

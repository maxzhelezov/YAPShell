#include "tree.h"
#include <stdlib.h>
#include <string.h>

/* Общие переменные внутри модуля */
static tree beg_cmd, cur_cmd, prev_cmd; /* Начальная команда, текущая и предыдущая */
static int end_flag; /* Флаг останова */
static list lst; /* Список слов */
static int cur_list; /* Текущая позиция в нем */

/* Тип - следующая вершина графа*/
typedef void *(*vertex)();

/* Вершины L-графа */
static void * begin();
static void * conv();
static void * conv2();
static void * subin();
static void * subout();
static void * in();
static void * in2();
static void * out();
static void * out2();
static void * backgrnd();
static void * next(enum type_of_next nxt);
static void * append();
static void * end();
static void * error();

static tree make_cmd(); /* Создает дерево из одного элемента, обнуляет все поля */
static void make_bgrnd(tree t); /* Устанаваливает поле конвеера background = 1 
                                   во всех команда конвеера t */
static char * add_argv(); /* Добавляет очередной элемент в массив argv текущей
                             команды, а также возвращает добавленный элемент */
static void init(); /* Инициализация */
static int check_spec(char* s); /* Проверяет является ли строка спец символом*/

tree build_tree(list lst_loc){
    vertex V = begin;
    init();
    lst = lst_loc;
    while (!end_flag){
        V = V();
    }
    return cur_cmd;
}

static void init(){
    beg_cmd = NULL;
    cur_cmd = NULL;
    prev_cmd = NULL;
    end_flag = 0;
    cur_list = 0;
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

/*
static void add_argv(){
    if (cur_cmd -> argv == NULL)
        strcpy(*lst, *(cur_cmd -> argv));
    else{
        for(;cur_cmd -> argv != NULL;);
        cur_cmd -> argv =  
    }
    lst = lst++;

}*/


static void * begin(){
    char *s;
    beg_cmd = cur_cmd = make_cmd();
    s = add_argv();
    prev_cmd = cur_cmd;
    if (!check_spec(s))
        return conv;
    else
        return error;
}

static void * conv(){
    char *s;
    s = lst[cur_list];
    if (!check_spec(s))
    {
        add_argv();
        return conv;
    }
    cur_list++;
    if (strlen(s) == 1)
        switch (s[0]){
            case '|': return conv2;
            case '<': return in;
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
            case '>': return s[1] == '>' ? append : error;
        }
    return error;
}

static void * conv1(){
    char *s;
    cur_cmd = make_cmd();
    s = add_argv();
    if (!check_spec(s)){
        prev_cmd -> pipe = cur_cmd;
        prev_cmd = cur_cmd;
            
        return conv;
    }
    return error;
}

static void * next(enum type_of_next nxt){
    char *s;
    cur_cmd = make_cmd();
    s = add_argv();   
    if (!check_spec(s)){
        prev_cmd -> next = cur_cmd;
        prev_cmd -> type = nxt;
        prev_cmd = cur_cmd;
        return conv;
    }
    return error;
}

static void * in(){
    char *s = lst[cur_list++];
    if (!check_spec(s)){
        cur_cmd -> infile = s;
        return conv;
    }
    return error;
}

static void * in(){
    char *s = lst[cur_list++];
    if (!check_spec(s)){
        cur_cmd -> infile = s;
        return conv;
    }
    return error;
}

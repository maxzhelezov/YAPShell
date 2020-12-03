#ifndef TREE_HEADER
#define TREE_HEADER
#include "list.h"

enum type_of_next{
    NXT, AND, OR   /* Виды связей соседних команд в списке команд */
};

struct cmd_inf {
    char ** argv; /* Список из имени команды и аргументов */ 
    char *infile; /* Переназначенный файл стандартного ввода */
    char *outfile; /* Переназначенный файл стандартного вывода */
    int append; /* Флаг нужно ли открывать файл вывода в режиме append */
    int backgrnd; /* =1, если команда подлежит выполнению в фоновом режиме */
    struct cmd_inf* psubcmd; /* Команды для запуска в дочернем shell */
    struct cmd_inf* pipe; /* Следующая команда после "|" */
    struct cmd_inf* next; /* Следующая после ";" (или после "&") */
    enum type_of_next type; /* Связь со следующей командой через ; или && или || */
};

typedef struct cmd_inf *tree;
typedef struct cmd_inf node;

void print_tree(tree t, int shift); /* Печатает дерево t со сдвигом shift */
tree build_tree(list lst); /* Возвращает указатель на построенное дерево по списку lst */
void clear_tree(tree t); /* Очищает память, занимаемаю деревом t */

#endif

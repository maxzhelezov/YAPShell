#ifndef LIST_HEADER
#define LIST_HEADER
/* Основная структура с которой работает модуль
 * Состоит из ссылки на массив ссылок на строки
 * */

typedef char** list; /* список слов (в виде массива)*/

/* Строит список слов list в переменной, возвращает значение закончился ли ввод */
int build_list(list *lst);

/* Подставляет значения переменных окружения в list lst */
/* $HOME $SHELL $USER $EUID */
void subst(list lst);

/* Печтает список слов lst_loc */
void print_list(list lst_loc);

/* Очищает список lst_loc после работы */
void clear_list(list lst_loc);
#endif

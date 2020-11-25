#ifndef LIST_HEADER
#define LIST_HEADER
/* Основная структура с которой работает модуль
 * Состоит из ссылки на массив ссылок на строки
 * */

typedef char** list; /* список слов (в виде массива)*/

/* Строит список слов, возвращает list */
list build_list();

/* Печтает список слов lst_loc */
void print_list(list lst_loc);

/* Очищает список lst_loc после работы */
void clear_list(list lst_loc);
#endif

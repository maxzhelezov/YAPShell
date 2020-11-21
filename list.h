#ifndef LIST_HEADER
#define LIST_HEADER
/* Основная структура с которой работает модуль
 * Состоит из ссылки на массив ссылок на строки
 * */

typedef char** list; /* список слов (в виде массива)*/

/*Строит список слов*/
void buildlist();

/*Печтает список слов*/
void printlist();

void clearlist();
#endif

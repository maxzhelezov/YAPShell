#ifndef LIST_HEADER
#define LIST_HEADER
/* Основная структура с которой работает модуль
 * Состоит из ссылки на массив ссылок на строки*/

#define SIZE 16
int c; /*текущий символ */
char ** lst; /* список слов (в виде массива)*/

/*Очищает список lst*/
void clearlist();

/*Инициализирует lst нулями*/
void null_list();

/*Завершает создание списка lst, выравнивая память, помещая NULL в конце*/
void termlist();

/*Обнуление буфера ввода символов*/
void nullbuf();

/*Добавляет символ в буффер ввода*/
void addsym();

/*Премещает слово из буфера в lst*/
void addword();

/*Печатает длину списка*/
void printsize();

/*Печтает lst*/
void printlist();

/*Разделители слов*/
int symset(int c);

/*Функция сравнения слов из списка
 * Если a > b то 1, если a == b то 0, если a < b то -1*/
int lstrcmp(char * a, char * b);

/*Сортировка слиянием*/
void bubblesort();
#endif

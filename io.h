#ifndef IO_HEADER
#define IO_HEADER

#define EOF -1 /*Конец файла*/

/*Функция выдает один символ с потока ввода*/
int getchar();

/*Заполняет буфер ввода*/
void fill_buf();

/*Печатает строку*/
void printstr(char *s);

/*Печатает ошибку и умирает*/
void perror(char *s);

/*Печатает число*/
void printint(int a);
#endif

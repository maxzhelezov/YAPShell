#ifndef IO_HEADER
#define IO_HEADER

#define EOF -1 /* Конец файла */

/* Функция выдает один символ с потока ввода */
int get_char();

/* Заполняет буфер ввода */
void fill_buf();

/* Печатает строку */
void print_str(char *s);

/* Печатает ошибку и умирает */
void perr(char *s);

/* Печатает число */
void print_int(int a);
#endif

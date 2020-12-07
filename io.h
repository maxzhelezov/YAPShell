#ifndef IO_HEADER
#define IO_HEADER

#define EOF -1 /* Конец файла */

/* Функция выдает один символ с потока ввода */
int get_char();

/* Заполняет буфер ввода */
void fill_buf();

/* Печатает строку */
void print_str(char *s);

/* Печатает ошибку (soft) */
void sperr(char *s);

/* Печатает две строки и перевод строки на поток ошибок */
void sperr2n(char *s1, char *s2);

/* Печатает ошибку и умирает */
void perr(char *s);

/* Возвращает число строкой */
char * itos(int a);
#endif

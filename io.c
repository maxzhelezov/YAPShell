#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include "io.h"

#define BUF_SIZE 1024 /* Размер буфера */
char RBUF[BUF_SIZE]; /* Буфер символов */
int CUR_BUF = -1; /* Текущий размер буфера */

int get_char()
{
    static int cur_pos;
    if((CUR_BUF == -1) || (cur_pos == CUR_BUF))
    {
        fill_buf();
        if(CUR_BUF == 0)
            return EOF;
        cur_pos = 0;
    }
    return RBUF[cur_pos++];
}
void fill_buf()
{
    errno = 0;
    CUR_BUF = read(0, RBUF, BUF_SIZE);
    if(CUR_BUF == -1)
        perr(strerror(errno));
}

void print_str(char *s)
{
    char buf[BUF_SIZE];
    int cur_buf = 0;
    for(; *s != '\0'; buf[cur_buf++] = *(s++))
        if(cur_buf == BUF_SIZE)
        {
            errno = 0;
            if(write(1, buf, BUF_SIZE) == -1)
                write(2, strerror(errno), strlen(strerror(errno)));
            cur_buf = 0;
        }
    if(cur_buf > 0)
        write(1, buf, cur_buf);
}

void sperr(char *s)
{
    write(2, s, strlen(s));
}

void sperr2n(char *s1, char *s2)
{
    sperr(s1);
    sperr(s2);
    sperr("\n");
}

void perr(char *s)
{
    sperr(s);
    sperr("\n");
    exit(1);
}

char * itos(int a)
{
    int i = 0, n, b = 0;
    static char s[20];/* Числа не могут быть больше из-за размера long long */
    /* На случай 0 элементов */
    if(a == 0)
        i = 1;
    while(a > 0)
    {
        b = b * 10 + a % 10;
        a = a / 10;
        i++;
    }
    n = i;
    for(i = 0; i < n; b /= 10, i++)
        s[i] = b % 10 + '0';
    s[i] = '\0';
    return s;
}

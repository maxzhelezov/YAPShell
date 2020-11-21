#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include "io.h"

#define BUF_SIZE 1024 /*Размер буфера*/
char RBUF[BUF_SIZE]; /*Буфер символов*/
int CUR_BUF = -1; /*Текущий размер буфера*/

int getchar(){
    static int cur_pos;
    if ((CUR_BUF == -1) || (cur_pos == CUR_BUF)){
        fill_buf();
        if (CUR_BUF == 0)
            return EOF;
        cur_pos = 0;
    }
    return RBUF[cur_pos++];
}
void fill_buf(){
    errno = 0;
    CUR_BUF = read(0, RBUF, BUF_SIZE);
    if (CUR_BUF == -1)
        perror(strerror(errno));
}

void printstr(char *s){
    char buf[BUF_SIZE];
    int cur_buf = 0;
    for(; *s != '\0'; buf[cur_buf++] = *(s++))
        if (cur_buf == BUF_SIZE){
            errno = 0;
            if (write(1, buf, BUF_SIZE) == -1) 
                write(2, strerror(errno), strlen(strerror(errno)));
            cur_buf = 0;
        }
    if (cur_buf > 0)
        write(1, buf, cur_buf);
}

void perror(char *s){
    write(2, s, strlen(s));
    write(2, "\n", 1);
    exit(1);
}

void printint(int a){
    int i = 0, n, b = 0;
    char s[20];/*Числа большими по длине быть не могут*/
    /*На случай 0 элементов*/
    if(a == 0)
        i = 1;
    while (a > 0){
        b = b * 10 + a % 10;
        a = a / 10;
        i++;
    }
    n = i;
    for(i = 0; i < n; b /= 10, i++)
        s[i] = b % 10 + '0';
    s[i] = '\0';
    printstr(s);
}

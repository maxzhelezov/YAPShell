#include "exec.h"
#include "io.h"
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>

#define WROPEN O_WRONLY|O_TRUNC|O_CREAT /* Флаги для открытия файла как > */
#define APPOPEN O_RDONLY|O_CREAT|O_APPEND /* Флаги для открытия файла как >> */
#define PIDBUF 8 /* Размер буфера из PID */
 
struct p_table
{
    int *pids;
    int size;
    int max_size;
};
typedef struct p_table pid_table;

  
static int exec_pipe(tree *tr); /* Обработка pipe, перемещает t на конец 
                                  конвеера, возвращает статус завершения */
static void conf_io(tree t); /* Перенапрявляет вход и выход для узла дерева */
static void add(pid_table *pt, int pid); /* Добавляет pid в таблицу процессов */
static void rem(pid_table *pt, int pid); /* Удаляет pid из таблицы процессов */
static void init(pid_table *pt); /* Инциализация таблицы процессов */
static void term(pid_table *pt); /* Завершение таблицы процесов */
static void clean(pid_table *pt); /* Очистка таблицы */

void execute(tree t)
{
    while (t != NULL){
        
        if (t -> pipe != NULL){
            exec_pipe(&t);
        }

    }
}

void clear_zombies()
{

}

static int exec_pipe(tree *tr){
    tree t = *tr;
    int fd[2], in, out, next_in, i, pid;
    pid_table pt;

    pipe(fd);
    out=fd[1];
    next_in=fd[0];
    init(&pt);
    if ((pid = fork())==0)
    {
        close(next_in);
        dup2(out,1);
        close(out);
        conf_io(t);
        errno = 0;
        if(execvp(t -> argv[0], t->argv) == -1)
            perr(strerror(errno));
        exit(-1);
    }
    add(&pt, pid);
    in=next_in; 
    while ((t = t -> pipe) -> pipe != NULL) 
    {
        close(out);
        pipe(fd);
        out=fd[1];
        next_in=fd[0];
        if((pid = fork())==0)
        {
            close(next_in);
            dup2(in,0);
            close(in);
            dup2(out,1);
            close(out);
            conf_io(t);
            errno = 0;
            if(execvp(t -> argv[0], t -> argv) == -1)
                perr(strerror(errno));
            exit(-1);
        }
        add(&pt, pid);
        close(in);
        in=next_in;
    }
    close(out);
    if ((pid = fork())==0)
    {
        dup2(in,0);
        close(in);
        conf_io(t);
        errno = 0;
        if(execvp(t -> argv[0], t -> argv)==-1)
            perr(strerror(errno));
        exit(-1);
    }
    add(&pt, pid);
    close(in);
    term(&pt);
    while (pt.size > 0)
        for(i = 0; i < pt.max_size; i++)
            if (pt.pids[i] != 0)
                if(waitpid(pt.pids[i], NULL, WNOHANG) != 0)
                    rem(&pt, pt.pids[i]);
    clean(&pt);    
    return 0;
}

static void conf_io(tree t){
    int fd;
    if (t -> infile != NULL){
        fd = open(t -> infile, O_RDONLY, 0);
        dup2(fd, 0);
        close(fd);
    }

    if (t -> outfile != NULL){
        fd = open(t -> outfile, t->append ? APPOPEN : WROPEN , 0644);
        dup2(fd, 1);
        close(fd);
    }
}

static void add(pid_table *pt, int pid){
    if (pt -> size > pt -> max_size - 1){
        pt -> pids = realloc(pt -> pids,
                (pt -> max_size += PIDBUF)*sizeof(pt -> max_size));
    }
    pt -> pids[pt -> size++] = pid;
}

static void rem(pid_table *pt, int pid){
    int i;
    for(i = 0; pt -> pids[i] != pid; i++);
    pt -> pids[i] = 0;
    pt -> size--;
}

static void init(pid_table *pt){
    pt -> pids = NULL;
    pt -> size = 0;
    pt -> max_size = 0;   
}

static void term(pid_table *pt){
    pt -> pids = realloc(pt -> pids, (pt -> size)*sizeof(pt -> pids));
    pt -> max_size = pt -> size;
}

static void clean(pid_table *pt){
    free(pt->pids);
    pt -> size = 0;
    pt -> max_size = 0;
}


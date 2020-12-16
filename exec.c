#define _POSIX_SOURCE
#include "exec.h"
#include "io.h"
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/signal.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>
#include "tree.h"

#define WROPEN O_WRONLY|O_TRUNC|O_CREAT /* Флаги для открытия файла как > */
#define APPOPEN O_WRONLY|O_CREAT|O_APPEND /* Флаги для открытия файла как >> */
#define PIDBUF 8 /* Размер буфера из PID */
   
/* Обработка pipe, перемещает t на конец конвеера, возвращает статус завершения */
static int exec_pipe(tree *tr, tree orig, pid_table * pt);
 /* Функция для рекурсивного выполенения сабшелла, дерево orig нужна для 
  * очистки памяти, pt таблица процесов для background  */
static int exec_rec(tree t, tree orig, pid_table * pt);
/* выполняет одну команду */
static int exec_cmd(tree *tr, tree orig, pid_table * pt);
/* Перенапрявляет вход и выход для узла дерева */
static void conf_io(tree t);
/* Выполенение одной команды или сабшелла, для использования в новом процессе */
static void exec_unit(tree t, tree orig, pid_table * pt); 
/* Перенаправление стандартного ввода и вывода, если in = 0, то ничего не 
 * перенаправляется, анлогично для out */
static void redir(int in, int out);
/* Получить статус выполнения процесса */
static int get_status(int status);
/* Сделать процесс фоновым, если он таковой */
static void make_back(tree t);
/* Обработка cd, status - статус команды, возврщает является ли аргумент cd*/
static int make_cd(tree t, int * status);


/* Работа со структурой таблицы процессов */
static void add(pid_table *pt, int pid); /* Добавляет pid в таблицу процессов */
static void add_loud(pid_table *pt, int pid); /* add с выводом */
static void rem(pid_table *pt, int pid); /* Удаляет pid из таблицы процессов */
static void init(pid_table *pt); /* Инциализация таблицы процессов */
static void term(pid_table *pt); /* Завершение таблицы процесов */
static void clean(pid_table *pt); /* Очистка таблицы */

void execute(tree t, pid_table * pt)
{
    exec_rec(t, t, pt);
}

pid_table * init_back(){
    pid_table *pt;
    pt = malloc(sizeof(pid_table));
    init(pt);
    return pt;
}

int check_back(pid_table *pt)
{
    int i, flag = 1;
    if (pt -> pids == NULL) return 1;
    for(i = 0; i < pt -> max_size; i++)
        if(pt -> pids[i] != 0){
            flag = 0;
            if(waitpid(pt->pids[i], NULL, WNOHANG) != 0){
                sperr(itos(i));
                sperr(" - Процесс ");
                sperr2n(itos(pt->pids[i]), " завершился");
                rem(pt, pt->pids[i]);
                pt -> size++; /* Костыль чтобы не портить структуру */
            }
        }
    if(flag && pt -> pids != NULL){
        clean(pt);
        init(pt);
        return 1;
    }
    return 0;
}

void clean_back(pid_table *pt){
    int i;
    for(i = 0; i < pt -> max_size; i++)
        if(pt -> pids[i] != 0)
            kill(i, SIGKILL);
    clean(pt);
    free(pt);
}

static int exec_rec(tree t, tree orig, pid_table * pt){
    int status, pid;
    while (t != NULL){
        

        if (t -> pipe != NULL){
            status = exec_pipe(&t, orig, pt);
        }
        else if (t -> psubcmd != NULL){
            if ((pid = fork()) == 0){
                make_back(t -> psubcmd);
                exec_unit(t, orig, pt);             
            }
            if(t -> backgrnd){
                add_loud(pt, pid);
                status = 0;
            }
            else{
                waitpid(pid, &status, 0);
                status = get_status(status);
            }
        }
        else
            status = exec_cmd(&t, orig, pt);
        
        switch (t -> type){
            case AND: if (status == 0)
                          t = t -> next;
                        else
                            return status;
                        break;
            case OR: if(status != 0)
                         t = t -> next;
                    else
                       return status;
                    break; 
            default: t = t -> next;
        }
        term(pt);
    }
    return status;
}

static void exec_unit(tree t, tree orig, pid_table * pt){
    int errno_saved;
    pid_table * pt_loc;
    signal(SIGINT, SIG_DFL);
    if (t -> psubcmd == NULL) {

        errno = 0;
        if(execvp(t -> argv[0], t -> argv) == -1)
            errno_saved = errno;
        sperr(t -> argv[0]);
        sperr(": ");
        clear_tree(orig);
        clean_back(pt);
        perr(strerror(errno_saved));
    }
    else{
        conf_io(t);
        clean_back(pt);
        pt_loc = init_back(); 
        exec_rec(t -> psubcmd, orig, pt_loc);
        while(check_back(pt_loc) == 0);
        clean_back(pt_loc);
        clear_tree(orig);
        exit(0);
    }
}

static int exec_pipe(tree *tr, tree orig, pid_table * pt_gl){
    tree t = *tr;
    int fd[2], in, out, next_in, i, pid, status, ret = 0;
    pid_table pt;

    pipe(fd);
    out=fd[1];
    next_in=fd[0];
    init(&pt);

    if (t -> argv == NULL || strcmp(t -> argv[0], "cd") != 0){
        if ((pid = fork())==0)
        {
            make_back(t);
            close(next_in);
            clean(&pt);
            redir(0, out);
            conf_io(t);
            exec_unit(t, orig, pt_gl);
        }
        add(&pt, pid);
        if(t -> backgrnd)
            add_loud(pt_gl, pid);
    }
    in=next_in; 
    while ((t = t -> pipe) -> pipe != NULL) 
    {
        close(out);
        pipe(fd);
        out=fd[1];
        next_in=fd[0];
        if (t -> argv == NULL || strcmp(t -> argv[0], "cd") != 0){
            if((pid = fork())==0)
            {
                make_back(t);
                close(next_in);
                clean(&pt);
                redir(in, out);
                conf_io(t);
                exec_unit(t, orig, pt_gl);
            }
            add(&pt, pid);
            if(t -> backgrnd)
                add_loud(pt_gl, pid);
        }
        close(in);
        in=next_in;
    }
    close(out);
    if (t -> argv == NULL || strcmp(t -> argv[0], "cd") != 0){
        if ((pid = fork())==0)
        {
            make_back(t);
            clean(&pt);
            redir(in, 0);
            conf_io(t);
            exec_unit(t, orig, pt_gl);
        }
        add(&pt, pid);
        if(t -> backgrnd)
            add_loud(pt_gl, pid);
    }
    close(in);
    term(&pt);
    if (t -> backgrnd == 1){
        clean(pt_gl);
        return 0;
    }
    while (pt.size > 0)
        for(i = 0; i < pt.max_size; i++)
            if (pt.pids[i] != 0)
                if(waitpid(pt.pids[i], &status, WNOHANG) != 0){
                    if (get_status(status) != 0){
                        ret = -1;          
                    }       
                rem(&pt, pt.pids[i]);

                }
    clean(&pt);  
    return ret;
}

static int exec_cmd(tree *tr, tree orig, pid_table * pt){
    tree t = *tr;
    int pid, status;
    if (make_cd(t, &status))
        return status;
    if ((pid = fork()) == 0){
        make_back(t);
        conf_io(t);
        /* Вообще говоря рекурсии не будет */
        exec_unit(t, orig, pt);
    }
    if (t -> backgrnd){
        add_loud(pt, pid);
        return 0;
    }
    waitpid(pid, &status, 0);
    return get_status(status);
}

static void redir(int in, int out){
    if (in != 0){
        dup2(in,0);
        close(in);
    }
    if (out != 0){
        dup2(out,1);
        close(out);
    }
}

static int get_status(int status){
    int ret;
    if (WIFEXITED(status))
        ret = WEXITSTATUS(status);
    else
        ret = -1;
    return ret;
}

static void conf_io(tree t){
    int fd;
    if (t -> infile != NULL){
        fd = open(t -> infile, O_RDONLY, 0);
        redir(fd, 0);
    }

    if (t -> outfile != NULL){
        fd = open(t -> outfile, t->append ? APPOPEN : WROPEN , 0644);
        redir(0, fd);
    }
}

static void make_back(tree t){
    if (t -> backgrnd == 0) return;
    int nullin = open("/dev/null", O_RDONLY, 0);
    int nullerr = open("/dev/null", O_WRONLY, 0);
    dup2(nullerr, 2);
    close(nullerr);
    signal(SIGINT, SIG_IGN);
    redir(nullin, 0);
}

static int make_cd(tree t, int * status){
    char * path;
    *status = 0;
    if (strcmp(t -> argv[0], "cd") == 0){
        errno = 0;
        path = t -> argv[1];
        if (t -> argv[1] == NULL)
            path = getenv("HOME");
        if(chdir(path)== -1){
            *status = 1;
            sperr2n(strerror(errno), "");
        }
        return 1;
    }
    return 0;
}

static void add(pid_table *pt, int pid){
    if (pt -> size > pt -> max_size - 1){
        pt -> pids = realloc(pt -> pids,
                (pt -> max_size += PIDBUF)*sizeof(pt -> max_size));
    }
    pt -> pids[pt -> size++] = pid;
}

static void add_loud(pid_table *pt, int pid){
    sperr(itos(pt -> size));
    sperr(" - Процесс ");
    sperr2n(itos(pid), " запущен в обработку");
    add(pt, pid);
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


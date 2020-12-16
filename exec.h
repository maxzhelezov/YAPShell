#ifndef EXEC_HEADER
#define EXEC_HEADER
#include "tree.h"

/* Структура таблицы процессов */
struct p_table
{
    int *pids;
    int size;
    int max_size;
};
typedef struct p_table pid_table;

/* Запустить дерево t в обработку */
void execute(tree t, pid_table * pt);

/* Инициализация таблицы фоновых процессов */
pid_table * init_back();

/* Проверка состояний таблицы фоновых процессов, возвращает пуста она или нет */
int check_back(pid_table * pt);

/* Очистка таблицы фоновых процессов, а также их завершение, force - убивать ли
 * процессы из таблицы */
void clean_back(pid_table * pt, int force);

#endif

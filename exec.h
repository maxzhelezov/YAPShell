#ifndef EXEC_HEADER
#define EXEC_HEADER
#include "tree.h"

/* Запустить дерево t в обработку */ 
void execute(tree t);

/* Очистка таблицы зомби, оставленная фоновыми процессами */
void clear_zombies();

#endif

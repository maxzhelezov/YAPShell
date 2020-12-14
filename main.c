#include "list.h"
#include "tree.h"
#include "exec.h"
#include "io.h"

/* Отладка */
#define DEBUG 1

int main()
{
    list lst;
    tree t;
    print_str(">>> ");
    while(!build_list(&lst))
    {
        subst(lst);
        if (DEBUG)
            print_list(lst);
        t = build_tree(lst);
        clear_list(lst);
        if (DEBUG) 
            print_tree(t, 0);
        execute(t);
        clear_zombies();
        clear_tree(t);
        print_str(">>> ");
    }
    print_str("\n");
    return 0;
}


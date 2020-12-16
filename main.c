#include "list.h"
#include "tree.h"
#include "exec.h"
#include "io.h"
#include <sys/signal.h>
#include <sys/types.h>

/* Отладка */
#define DEBUG 0

int main()
{
    list lst;
    tree t;
    pid_table * pt;
    signal(SIGINT, SIG_IGN);
    print_str(">>> ");
    pt = init_back();
    while(!build_list(&lst))
    {
        subst(lst);
        if (DEBUG)
            print_list(lst);
        t = build_tree(lst);
        clear_list(lst);
        if (DEBUG)
            print_tree(t, 0);
        execute(t, pt);
        check_back(pt);
        clear_tree(t);
        print_str(">>> ");
    }
    print_str("\n");
    clean_back(pt);
    return 0;
}


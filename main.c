#include "list.h"
#include "tree.h"
#include <stdlib.h>
#include "io.h"

int main()
{
    list lst;
    tree t;
    print_str(">>> ");
    while(!build_list(&lst))
    {
        subst(lst);
        print_list(lst);
        t = build_tree(lst);
        clear_list(lst);
        print_tree(t, 0);
        clear_tree(t);
        print_str(">>> ");
    }
    print_str("\n");
    return 0;
}


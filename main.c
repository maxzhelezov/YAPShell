#include "list.h"
#include "tree.h"

int main()
{
    list lst;
    tree t;
    lst = build_list();
    subst(lst);
    print_list(lst);
    t = build_tree(lst);
    print_tree(t, 0);
    clear_list(lst);
    clear_tree(t);
    return 0;
}


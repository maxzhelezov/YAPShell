#include "list.h"
#include "tree.h" 

int main() {
    list lst;
    tree t;
    lst = build_list();
    print_list(lst);
    t = build_tree(lst);
    print_tree(t, 5);
    clear_list(lst);
    return 0;
}


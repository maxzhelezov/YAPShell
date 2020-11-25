#include "list.h"
#include "tree.h" 

int main() {
    list lst;
    lst = build_list();
    print_list(lst);
    clear_list(lst);
    return 0;
}


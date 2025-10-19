#include "dlinked_list.h"

#include <stdio.h>

int main()
{
    struct List list = list_init();

    list_push_back(&list, int, 0);
    list_push_back(&list, int, 1);
    list_push_back(&list, int, 2);
    list_push_back(&list, int, 6);

    list_for_each(&list, int, node_name, ptr_name, { printf("%d\n", *ptr_name); });

    list_erase(&list, list.head->next->next);

    list_for_each(&list, int, node_name, ptr_name, { printf("%d\n", *ptr_name); });

    struct List second_list = list_init();
    list_copy(&list, &second_list, int);

    list_destroy(&list);
    list_destroy(&second_list);
}

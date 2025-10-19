#include "dlinked_list.h"

void list_erase(struct List *list, struct ListNode *node)
{
    if (node->prev != 0) {
        node->prev->next = node->next;
    }

    if (node->next != 0) {
        node->next->prev = node->prev;
    }

    if (list->head == node) {
        list->head = node->next;
    }

    if (list->tail == node) {
        list->tail = node->prev;
    }

    LIST_FREE(node);
}

extern inline struct List list_init();

void list_destroy(struct List *list)
{
    list_for_each(list, void, list_node, _, { LIST_FREE(list_node); });
    *list = list_init();
}

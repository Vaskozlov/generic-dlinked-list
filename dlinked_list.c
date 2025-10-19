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

void list_private_finish_insert(
    struct List *list_ptr, struct ListNode *node_ptr, struct ListNode *new_node)
{
    new_node->next = new_node->prev = NULL;

    if (node_ptr == NULL) {
        if (list_ptr->tail == NULL) {
            list_ptr->head = list_ptr->tail = new_node;
        } else {
            list_ptr->tail->next = new_node;
            new_node->prev = list_ptr->tail;
            list_ptr->tail = new_node;
        }
    } else if (list_ptr->head == node_ptr) {
        new_node->next = node_ptr;
        node_ptr->prev = new_node;
        list_ptr->head = new_node;
    } else {
        new_node->next = node_ptr;
        node_ptr->prev->next = new_node;
        node_ptr->prev = new_node;
    }
}

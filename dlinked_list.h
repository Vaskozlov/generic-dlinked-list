#ifndef DLINKED_LIST_H
#define DLINKED_LIST_H

#include <errno.h>
#include <stddef.h>
#include <stdlib.h>

#define LIST_ALLOCATE(N) malloc(N)
#define LIST_FREE(P) free(P)
#define LIST_ALLOC_FAILED exit(ENOMEM)

struct ListNode
{
    struct ListNode *prev;
    struct ListNode *next;
    char obj;
};

struct List
{
    struct ListNode *head;
    struct ListNode *tail;
};

#define list_push_back(list, T, value)                                                             \
    do {                                                                                           \
        struct ListNode *new_node = LIST_ALLOCATE(offsetof(struct ListNode, obj) + sizeof(T));     \
                                                                                                   \
        struct List *list_ptr = (list);                                                            \
                                                                                                   \
        if (new_node == NULL) {                                                                    \
            LIST_ALLOC_FAILED;                                                                     \
        }                                                                                          \
                                                                                                   \
        new_node->prev = list_ptr->tail;                                                           \
        new_node->next = NULL;                                                                     \
                                                                                                   \
        T *value_memory = (T *)(&new_node->obj);                                                   \
        *value_memory = (value);                                                                   \
                                                                                                   \
        if (list_ptr->tail != NULL) {                                                              \
            list_ptr->tail->next = new_node;                                                       \
        } else {                                                                                   \
            list_ptr->tail = list_ptr->head = new_node;                                            \
        }                                                                                          \
                                                                                                   \
        list_ptr->tail = new_node;                                                                 \
    } while (0)

#define list_for_each(list, T, node_name, ptr_name, CODE)                                          \
    do {                                                                                           \
        struct ListNode *node_name = NULL;                                                         \
        struct ListNode *next = (list)->head;                                                      \
                                                                                                   \
        while (next != 0) {                                                                        \
            node_name = next;                                                                      \
            next = next->next;                                                                     \
            T *ptr_name = (T *)(&node_name->obj);                                                  \
            (void)ptr_name;                                                                        \
                                                                                                   \
            {                                                                                      \
                CODE                                                                               \
            }                                                                                      \
        }                                                                                          \
                                                                                                   \
    } while (0)

#define list_copy(from, to, T)                                                                     \
    do {                                                                                           \
        list_destroy(to);                                                                          \
        list_for_each((from), T, _, value, { list_push_back(to, T, *value); });                    \
    } while (0)

#define list_insert(list, T, node, value)                                                          \
    do {                                                                                           \
        struct ListNode *new_node = LIST_ALLOCATE(offsetof(struct ListNode, obj) + sizeof(T));     \
                                                                                                   \
        struct List *list_ptr = (list);                                                            \
        struct ListNode *node_ptr = (node);                                                        \
                                                                                                   \
        if (new_node == NULL) {                                                                    \
            LIST_ALLOC_FAILED;                                                                     \
        }                                                                                          \
                                                                                                   \
        T *value_memory = (T *)(&new_node->obj);                                                   \
        *value_memory = (value);                                                                   \
        new_node->next = new_node->prev = NULL;                                                    \
                                                                                                   \
        if (node_ptr == NULL) {                                                                    \
            if (list_ptr->tail == NULL) {                                                          \
                list_ptr->head = list_ptr->tail = new_node;                                        \
            } else {                                                                               \
                list_ptr->tail->next = new_node;                                                   \
                new_node->prev = list_ptr->tail;                                                   \
                list_ptr->tail = new_node;                                                         \
            }                                                                                      \
        } else if (list_ptr->head == node_ptr) {                                                   \
            new_node->next = node_ptr;                                                             \
            node_ptr->prev = new_node;                                                             \
            list_ptr->head = new_node;                                                             \
        } else {                                                                                   \
            new_node->next = node_ptr;                                                             \
            node_ptr->prev->next = new_node;                                                       \
            node_ptr->prev = new_node;                                                             \
        }                                                                                          \
    } while (0)

#define list_get_node_value(node, T) (*((T *)&(node)->obj))

#define list_get_node_from_value_ptr(value_ptr)                                                    \
    ((struct ListNode *)((char *)(value_ptr) - offsetof(struct ListNode, obj)))

void list_erase(struct List *list, struct ListNode *node);

inline struct List list_init()
{
    return (struct List){
        .head = 0,
        .tail = 0,
    };
}

void list_destroy(struct List *list);

#endif /* DLINKED_LIST_H */

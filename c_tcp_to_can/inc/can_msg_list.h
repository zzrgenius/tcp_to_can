

#ifndef _UTILS_LIST_H_
#define _UTILS_LIST_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdlib.h>

/*
 * ListNode iterator direction
 */
typedef enum { LIST_HEAD, LIST_TAIL } ListDirection;

/*
 * define list node
 */
typedef struct ListNode {
    struct ListNode *prev;
    struct ListNode *next;
    void *           val;
} ListNode;

/*
 * Double Linked List
 */
typedef struct {
    ListNode *   head;
    ListNode *   tail;
    unsigned int len;
    void (*free)(void *val);
    int (*match)(void *a, void *b);
} List;

/*
 * list iterator
 */
typedef struct {
    ListNode *    next;
    ListDirection direction;
} ListIterator;

/* create node */
ListNode *list_node_new(void *val);

/* create list */
List *list_new(void);

ListNode *list_rpush(List *self, ListNode *node);

ListNode *list_lpush(List *self, ListNode *node);

ListNode *list_find(List *self, void *val);

ListNode *list_at(List *self, int index);

ListNode *list_rpop(List *self);

ListNode *list_lpop(List *self);

void list_remove(List *self, ListNode *node);

void list_destroy(List *self);

/* create iterator */
ListIterator *list_iterator_new(List *list, ListDirection direction);

ListIterator *list_iterator_new_from_node(ListNode *node, ListDirection direction);

ListNode *list_iterator_next(ListIterator *self);

void list_iterator_destroy(ListIterator *self);

#ifdef __cplusplus
}
#endif
#endif  // QCLOUD_IOT_UTILS_LIST_H_

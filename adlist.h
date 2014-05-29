#ifndef __ADLIST_H
#define __ADLIST_H

typedef struct listNode
{
    struct listNode *prev;
    struct listNode *next;
    void *data;
} listNode;


typedef struct listIter
{
    listNode *next;
    int direction;
} listIter;

typedef struct list
{
    listNode *head;
    listNode *tail;
    unsigned long len;

    void *(*dup)(void *ptr);
    void (*free)(void *ptr);
    void (*match)(void *ptr, void *key);
} list;

#define listLength(l) ((l)->len)
#define listFirst(l) ((l)->head)
#define listLast(l) ((l)->tail)
#define listPrevNode(n) ((n)->prev)
#define listNextNode(n) ((n)->next)
#define listNodeValue(n) ((n)->value)

#define listGetDupMethod(l) ((l)->dup)
#define listGetFree(l) ((l)->free)
#define listGetMatchMethod(l) ((l)->match)

list *listCreate(void);
void listRelease(list *list);
list *listAddNodeHead(list *list, void *value);
list *listAddNodeTail(list *list, void *value);
list *listInsertNode(list *list, listNode *old_node, void *value, int after);
list *listDelNode(list *list, listNode *node);
listIter *listGetIterator(list *list, int direction);
void listReleaseIterator(listIter *iter);
void listRewind(list *list, listIter *iter);
void listRewindTail(list *list, listIter *iter);
listIter *listNext(listIter *iter);
list *listDup(list *list);
list *listSearchKey(list *list, void *key);

#define AL_START_HEAD 0
#define AL_START_TAIL 1

#endif

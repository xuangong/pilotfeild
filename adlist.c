#include <stdlib.h>
#include "adlist.h"

list *listCreate(void)
/*//{{{*/
{
    struct list *list;

    if((list = malloc(sizeof(*list))) == NULL)
        return NULL;

    list->head = list->tail = NULL;
    list->len = 0;
    list->dup = list->free = list->match = NULL;

    return list;
}
/*}}}*/

void listRelease(list *list)
/*//{{{*/
{
    unsigned long len;
    listNode *current, *next;
    current = list->head;
    len = list->len;
    while(len --)
    {
        next = current->next;
        if(list->free) list->free(current->value);
        free(current);
        current = next;
    }
    free(list);
}
/*}}}*/

list *listAddNodeHead(list *list, void *value)
/*{{{*/
{
    listNode *node;

    if((node = malloc(sizeof(*node))) == NULL)
        return NULL;

    node->value = value;

    if(list->len == 0)
    {
        list->head = list->tail = node;
        node->prev = node->next = NULL;
    }
    else
    {
        node->prev = NULL;
        node->next = list->head;
        list->head->prev = node;
        list->head = node;
    }

    list->len ++;
    return list;
}
/*}}}*/

list *listAddNodeTail(list *list, void *value)
/*{{{*/
{
    listNode *node;
    node->value = value;

    if(list->len == 0)
    {
        node->prev = node->next = NULL;
        list->head = list->tail = node;
    }
    else
    {
        node->prev = list->tail;
        node->next = NULL;
        list->tail->next = node;
        list->tail = node;
    }
    list->len ++;
    return list;
}
/*}}}*/

list *listInsertNode(list *list, listNode *old_node, void *value, int after)
/*{{{*/
{
    listNode *node;
    if((node = malloc(sizeof(*node))) == NULL)
        return NULL;

    node->value = value;

    if(after)
    {
        //在old_node后面
        node->prev = old_node;
        node->next = old_node->next;
        if(list->tail == old_node)
        {
            list->tail = node;
        }
    }
    else
    {
        node->prev = old_node->prev;
        node->next = old_node;
        if(list->head == old_node)
        {
            list->head = node;
        }
    }

    if(node->prev != NULL)
    {
        node->prev->next = node;
    }
    if(node->next != NULL)
    {
        node->next->prev = node;
    }

    list->len ++;
    return list;
}
/*}}}*/

list *listDelNode(list *list, listNode *node)
/*{{{*/
{
    //既然要针对node->prev结构操作，一定要事先检查node->prev是不是null
    if(node->prev)
        node->prev->next = node->next;
    else //node->prev为空怎么办?
        list->head = node->next;

    if(node->next)
        node->next->prev = node->prev;
    else
        list->tail = node->prev;

    if(list->free)
        list->free(node->value);

    free(node);
    list->len --;
    return list;
}
/*}}}*/

listIter *listGetIterator(list *list, int direction)
/*{{{*/
{
    listIter *iter;

    if((iter = malloc(sizeof(*iter))) == NULL)
        return NULL;

    if(direction == AL_START_HEAD)
        iter->next = list->head;
    else
        iter->next = list->tail;

    iter->direction = direction;

    return iter;
}
/*}}}*/

void listReleaseIterator(listIter *iter)
/*{{{*/
{
    free(iter);
}
/*}}}*/

void listRewind(list *list, listIter *iter)
/*{{{*/
{
    iter->next = list->head;
    iter->direction = AL_START_HEAD;
}
/*}}}*/

void listRewindTail(list *list, listIter *iter)
/*{{{*/
{
    iter->next = list->tail;
    iter->direction = AL_START_TAIL;
}
/*}}}*/

listIter *listNext(listIter *iter)
/*{{{*/
{
    listNode *current = iter->next;
    if(current != NULL)
    {
        if(iter->direction == AL_START_HEAD)
        {
            iter->next = current->next;
        }
        else
        {
            iter->next = current->prev;
        }
    }

    return current;
}
/*}}}*/

list *listDup(list *orign)
/*{{{*/
{
    list *cpy;
    listIter *iter;
    listNode *node;

    if((cpy = listCreate()) == NULL)
    {
        return NULL;
    }

    cpy->free = orign->free;
    cpy->dup = orign->dup;
    cpy->match = orign->match;

    iter = listGetIterator(orign, AL_START_HEAD);
    while((node = listNext(iter)) != NULL)
    {
        void *value;
        if(cpy->dup)
        {
            value = cpy->dup(node->value);
            if(value == NULL)
            {
                listRelease(cpy);
                listReleaseIterator(iter);
                return NULL;
            }
        }
        else
        {
            value = node->value;
        }

        //出错只有一种情况,就是malloc失败了,return NULL;
        if(listAddNodeTail(cpy, value) == NULL)
        {
            listRelease(cpy);
            listReleaseIterator(iter);
            return NULL;
        }
    }
    listReleaseIterator(iter);
    return cpy;
}
/*}}}*/

list *listSearchKey(list *list, void *key)
/*{{{*/
{
    listIter *iter;
    listNode *node;

    iter = listGetIterator(list, AL_START_HEAD);
    while((node = listNext(iter)) != NULL)
    {
        if(list->match)
        {
            if(list->match(node->value, key))
            {
                listReleaseIterator(iter);
                return node;
            }
        }
        else
        {
            //如果match函数不存在，理论上讲是比较不了
            //这时候只需要看地址就行了
            if(key == node->value)
            {
                listReleaseIterator(iter);
                return node;
            }
        }
    }
    listReleaseIterator(iter);
    return NULL;
}
/*}}}*/

/**
 * index>=0 从0位置正向开始
 * index<0  从最后一个位置开始
 */
listNode *listIndex(list *list, long index)
/*{{{*/
{
    listNode *node;
    listIter *iter;
    if(index < 0)
    {
        index = -index;
        iter = listGetIterator(list, AL_START_TAIL);
        while(index-- && (node = listNext(iter));
    }
    else
    {
        index++;
        iter = listGetIterator(list, AL_START_HEAD);
        while(index-- && (node = listNext(iter));
    }

    return node;
}
/*}}}*/

void listRotate(list *list)
/*{{{*/
{
    listNode *tail = list->tail;
    if(listLength(list) <= 1) return;

    list->tail = tail->prev;
    list->tail->next = NULL;

    tail->next = list->head;
    list->head->prev = tail;
    tail->prev = NULL;
    list->head = tail;
}
/*}}}*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "sds.h"

//init 一个initlen长度的sds
sds sdsnewlen(const void *init, size_t initlen)
/*{{{*/
{
    struct sdshdr *sh = NULL;

    if(init)
    {
        sh = malloc(sizeof(struct sdshdr) + initlen + 1);
    }

    if(sh == NULL) return NULL;

    sh->len = initlen;
    sh->free = 0;

    if(initlen && init)
        memcpy(sh->buf, init, initlen);

    sh->buf[initlen] = '\0';

    return (char *)sh->buf;
}
/*}}}*/

sds sdsnew(const char * init)
/*{{{*/
{
    size_t initlen = (init == NULL) ? 0 : strlen(init);
    return sdsnewlen(init, initlen);
}
/*}}}*/

sds sdsempty()
/*{{{*/
{
    return sdsnewlen("", 0);
}
/*}}}*/

sds sdsdup(const sds s)
/*{{{*/
{
    return sdsnewlen(s, sdslen(s));
}
/*}}}*/

//释放内存的时候，一定要先检查是否是NULL
void sdsfree(sds s)
/*{{{*/
{
    if(s == NULL) return;
    free(GET_SDSHDR_VOID(s));
}
/*}}}*/

sds sdsMakeRoomFor(sds s, size_t addlen)
/*{{{*/
{
    printf("s's address is %x\n", s);
    struct sdshdr *sh, *newsh;
    size_t free = sdsavail(s);
    size_t len, newlen;

    if(free >= addlen) return s;

    sh = s-(sizeof(struct sdshdr));
    len = sdslen(s);
    newlen = len + addlen;

    //如果当前free不满足需求了，则增加至少不少于addlen的大小
    if(newlen < SDS_MAX_PREALLOC)
        newlen *= 2;
    else
        newlen += SDS_MAX_PREALLOC;

    //realloc返回值可能和sh不一样，如果需要分配空间，会把内存拷贝过去以后
    //把原来的sh开始的空间释放掉
    newsh = realloc(sh, sizeof(struct sdshdr)+newlen+1);
    printf("newsh's address is %x\n", newsh->buf);
    if(newsh == NULL) return NULL;

    //len-->原来已经用的大小
    //newlen-->扩展后的总大小
    newsh->free = newlen - len;

    //realloc来的newsh，所以newsh->len不用改
    //在所以的sds操作中，return出来的是sds，不是sdshdr结构
    return newsh->buf;
}
/*}}}*/

//将sds扩展到len长度，free的部分用\0填充
sds sdsgrowzero(sds s, size_t len)
/*{{{*/
{
    struct sdshdr *sh = GET_SDSHDR_VOID(s);
    size_t totlen, curlen = sh->len;
    if(len <= curlen) return s;

    s = sdsMakeRoomFor(s, len-curlen);
    if(s == NULL) return NULL;

    memset(s+curlen, 0, (len-curlen+1));

    totlen = sh->len + sh->free;
    //虽然是扩展到不少于len的长度，但是我们只要用的是len长度
    sh->len = len;
    sh->free = totlen - len;

    //redis中用的是return s,我觉得有问题，在sdsMakeRoomFor中realloc可能会改变s
    //如果改变了s，s对sh的相对偏移就改变了，应该return sh->buf
    return sh->buf;
}
/*}}}*/

//把t按照len长度追加在sds后面
sds sdscatlen(sds s, const void *t, size_t len)
/*{{{*/
{
    struct sdshdr *sh;
    size_t curlen = sdslen(s);

    s = sdsMakeRoomFor(s, len);
    if(s == NULL) return NULL;

    memcpy(s+curlen, t, len);

    sh = GET_SDSHDR_VOID(s);
    sh->len = curlen + len;
    //虽然free可能更大，但是这个是free的最小值，保证可用
    sh->free = sh->free - len;

    s[curlen + len] = '\0';
    return s;
}
/*}}}*/

sds sdscat(sds s, const void *t)
/*{{{*/
{
    return sdscatlen(s, t, strlen(t));
}
/*}}}*/

sds sdscatsds(sds s, const sds t)
/*{{{*/
{
    return sdscatlen(s, t, sdslen(t));
}
/*}}}*/

sds sdscpylen(sds s, const char *t, size_t len)
/*{{{*/
{
    struct sdshdr *sh = GET_SDSHDR_VOID(s);

    size_t totlen = sh->free + sh->len;
    if(totlen < len)
    {
        s = sdsMakeRoomFor(s, len-sh->len);
        if(s == NULL) return NULL;
        sh = GET_SDSHDR_VOID(s);
        totlen = sh->free + sh->len;
    }

    memcpy(s, t, len);
    s[len] = '\0';
    sh->len = len;
    sh->free = totlen - len;

    return s;
}
/*}}}*/

sds sdscpy(sds s, const char *t)
/*{{{*/
{
    return sdscpylen(s, t, strlen(t));
}
/*}}}*/

sds sdscatvprintf(sds s, const char *fmt, va_list ap)
/*{{{*/
{
    va_list cpy;
    char *buf, *t;
    size_t buflen = 16;

    while(1)
    {
        buf = malloc(buflen);
        if(buf == NULL) return NULL;
        buf[buflen - 2] = '\0';
        va_copy(cpy, ap);
        vsnprintf(buf, buflen, fmt, cpy);
        if(buf[buflen-2] != '\0')
        {
            free(buf);
            buflen *= 2;
            continue;
        }
        break;
    }
    t = sdscat(s, buf);
    free(buf);
    return t;
}
/*}}}*/

sds sdscatprintf(sds s, const char *fmt, ...)
/*{{{*/
{
    va_list ap;
    char *t;
    va_start(ap, fmt);
    t = sdscatvprintf(s, fmt, ap);
    va_end(ap);
    return t;
}
/*}}}*/

int main(int argc, char const *argv[])
{
    struct sdshdr *sh;
    sds x = sdsnew("foo"), y;
    printf("x is %s\n", x);
    y = sdsdup(x);
    sdsfree(x);
    printf("y is %s\n", y);
    printf("y's length is %d\n", sdslen(y));

    printf("y's value is %x\n", y);
    y = sdscpy(y, "concat after here");
    printf("y's length is %d\n", sdslen(y));
    printf("y's free is %d\n", sdsavail(y));
    printf("y's value is %x\n", y);
    printf("y is turned to be \"%s\"\n", y);

    y = sdscat(y, "concat");
    printf("y is turned to be \"%s\"\n", y);

    sdsfree(y);

    return 0;
}

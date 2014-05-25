#include <stdlib.h>
#include <string.h>
#include "sds.h"

sds sdsnewlen(const void *init, size_t initlen)
{
    struct sdshdr *sh;

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

sds sdsnew(const char * init)
{
    size_t initlen = (init == NULL) ? 0 : strlen(init);
    return sdsnewlen(init, initlen);
}

sds sdsempty()
{
    return sdsnewlen("", 0);
}

sds sdsdup(const sds s)
{
    return sdsnewlen(s, sdslen(s));
}

//释放内存的时候，一定要先检查是否是NULL
void sdsfree(sds s)
{
    if(s == NULL) return;
    free(GET_SDSHDR_VOID(s));
}

sds sdsMakeRoomFor(sds s, size_t addlen)
{
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

    newsh = realloc(sh, sizeof(struct sdshdr)+newlen+1);
    if(newsh == NULL) return NULL;

    //len-->原来已经用的大小
    //newlen-->扩展后的总大小
    newsh->free = newlen - len;

    //realloc来的newsh，所以newsh->len不用改
    //在所以的sds操作中，return出来的是sds，不是sdshdr结构
    return newsh;
}

//将sds扩展到len长度，free的部分用\0填充
sds sdsgrowzero(sds s, size_t len)
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

int main(int argc, char const *argv[])
{
    return 0;
}

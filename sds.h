#ifndef __STRING_H
#define __STRING_H

#define SDS_MAX_PREALLOC (1024 * 1024)
#define GET_SDSHDR_VOID(s) ((void *)(s-(sizeof(struct sdshdr))))

#include <sys/types.h>
#include <stdarg.h>

typedef char *sds;

struct sdshdr{
    int len;
    int free;
    char buf[];
};

static inline size_t sdslen(const sds s)
{
    struct sdshdr *sh = GET_SDSHDR_VOID(s);
    return sh->len;
}

static inline size_t sdsavail(const sds s)
{
    struct sdshdr *sh = GET_SDSHDR_VOID(s);
    return sh->free;
}

size_t sdslen(const sds s);
size_t sdsavail(const sds s);
sds sdsempty();
sds sdsnew(const char *init);
sds sdsdup(const sds s);
sds sdsnewlen(const void *init, size_t initlen);
void sdsfree(sds s);
sds sdsMakeRoomFor(sds s, size_t addlen);
sds sdsgrowzero(sds s, size_t len);


#endif

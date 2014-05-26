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
    //这就是个占位符
    //c98不支持这种写法，就写一个char[1]，但是相应的内存模型也会改变
    char buf[0];
};

static inline size_t sdslen(const sds s)
/*{{{*/
{
    struct sdshdr *sh = GET_SDSHDR_VOID(s);
    return sh->len;
}
/*}}}*/

static inline size_t sdsavail(const sds s)
/*{{{*/
{
    struct sdshdr *sh = GET_SDSHDR_VOID(s);
    return sh->free;
}
/*}}}*/

size_t sdslen(const sds s);
size_t sdsavail(const sds s);
sds sdsempty();
sds sdsnew(const char *init);
sds sdsdup(const sds s);
sds sdsnewlen(const void *init, size_t initlen);
void sdsfree(sds s);
sds sdsMakeRoomFor(sds s, size_t addlen);
sds sdsgrowzero(sds s, size_t len);
sds sdscatlen(sds s, const void *t, size_t len);
sds sdscat(sds s, const void *t);
sds sdscatsds(sds s, const sds t);
sds sdscpylen(sds s, const char *t, size_t len);
sds sdscpy(sds s, const char *t);
sds sdscatvprintf(sds s, const char *fmt, va_list ap);
sds sdscatprintf(sds s, const char *fmt, ...);
sds sdstrim(sds s, const char *cset);
sds sdsrange(sds s, int start, int end);

#endif

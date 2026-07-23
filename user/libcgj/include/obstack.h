/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Clean-room glibc-shaped obstack.h (growable object stack). Not GNU glibc.
 */
#pragma once

#include <stddef.h>
#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif

struct _obstack_chunk {
    char                  *limit;
    struct _obstack_chunk *prev;
    char                   contents[4];
};

struct obstack {
    long          chunk_size;
    struct _obstack_chunk *chunk;
    char         *object_base;
    char         *next_free;
    char         *chunk_limit;
    int           tempint;
    int           alignment_mask;
    void *(*chunkfun)(void *, long);
    void  (*freefun)(void *, void *);
    void         *extra_arg;
    unsigned      use_extra_arg : 1;
    unsigned      maybe_empty_object : 1;
    unsigned      alloc_failed : 1;
};

int  _obstack_begin(struct obstack *pH, int nSize, int nAlign,
                    void *(*pfnAlloc)(long), void (*pfnFree)(void *));
int  _obstack_begin_1(struct obstack *pH, int nSize, int nAlign,
                      void *(*pfnAlloc)(void *, long),
                      void (*pfnFree)(void *, void *), void *pArg);
void _obstack_newchunk(struct obstack *pH, int nLength);
void  obstack_free(struct obstack *pH, void *pObj);
int  _obstack_allocated_p(struct obstack *pH, void *pObj);
int  _obstack_memory_used(struct obstack *pH);

#define obstack_base(h)        ((void *)(h)->object_base)
#define obstack_next_free(h)   ((h)->next_free)
#define obstack_object_size(h) ((unsigned)((h)->next_free - (h)->object_base))
#define obstack_room(h)        ((unsigned)((h)->chunk_limit - (h)->next_free))
#define obstack_empty_p(h)     ((h)->chunk == 0 || \
                                (h)->object_base == (h)->next_free)
#define obstack_make_room(h, len)                                         \
    do {                                                                  \
        if (obstack_room(h) < (unsigned)(len)) {                          \
            _obstack_newchunk((h), (int)(len));                           \
        }                                                                 \
    } while (0)
#define obstack_grow(h, where, len)                                       \
    do {                                                                  \
        size_t __cgj_len = (size_t)(len);                                 \
        if (obstack_room(h) < __cgj_len) {                                \
            _obstack_newchunk((h), (int)__cgj_len);                       \
        }                                                                 \
        if (__cgj_len > 0) {                                              \
            memcpy((h)->next_free, (where), __cgj_len);                   \
            (h)->next_free += __cgj_len;                                  \
        }                                                                 \
    } while (0)
#define obstack_1grow(h, datum)                                           \
    do {                                                                  \
        if (obstack_room(h) < 1u) {                                       \
            _obstack_newchunk((h), 1);                                    \
        }                                                                 \
        *(h)->next_free++ = (char)(datum);                                \
    } while (0)
#define obstack_blank(h, len)                                             \
    do {                                                                  \
        size_t __cgj_len = (size_t)(len);                                 \
        if (obstack_room(h) < __cgj_len) {                                \
            _obstack_newchunk((h), (int)__cgj_len);                       \
        }                                                                 \
        (h)->next_free += __cgj_len;                                      \
    } while (0)
void *obstack_finish(struct obstack *pH);

#ifdef __cplusplus
}
#endif

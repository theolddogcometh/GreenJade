/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Clean-room glibc-shaped search.h (subset). Not GNU glibc.
 */
#pragma once

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum { FIND, ENTER } ACTION;
typedef enum { preorder, postorder, endorder, leaf } VISIT;

typedef struct entry {
    char *key;
    void *data;
} ENTRY;

void *tsearch(const void *pKey, void **ppRoot,
              int (*pfnCmp)(const void *, const void *));
void *tfind(const void *pKey, void *const *ppRoot,
            int (*pfnCmp)(const void *, const void *));
void *tdelete(const void *pKey, void **ppRoot,
              int (*pfnCmp)(const void *, const void *));
void  twalk(const void *pRoot,
            void (*pfnAction)(const void *pNode, VISIT v, int nLevel));
void  twalk_r(const void *pRoot,
              void (*pfnAction)(const void *pNode, VISIT v, void *pClosure),
              void *pClosure);
void  tdestroy(void *pRoot, void (*pfnFree)(void *p));

int    hcreate(size_t nElem);
void   hdestroy(void);
ENTRY *hsearch(ENTRY item, ACTION action);

struct hsearch_data;
int  hcreate_r(size_t nElem, struct hsearch_data *pHtab);
void hdestroy_r(struct hsearch_data *pHtab);
int  hsearch_r(ENTRY item, ACTION action, ENTRY **ppRet,
               struct hsearch_data *pHtab);

void *lfind(const void *pKey, const void *pBase, size_t *pNmemb, size_t cb,
            int (*pfnCmp)(const void *, const void *));
void *lsearch(const void *pKey, void *pBase, size_t *pNmemb, size_t cb,
              int (*pfnCmp)(const void *, const void *));

void insque(void *pElem, void *pPred);
void remque(void *pElem);

#ifdef __cplusplus
}
#endif

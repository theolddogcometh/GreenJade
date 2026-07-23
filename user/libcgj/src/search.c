/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * search.h — binary tree, hash table, linear search, insque/remque.
 * Clean-room; not GNU glibc.
 */
#include <search.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

/* ---- binary search tree (tsearch family) -------------------------------- */

struct tnode {
    void *pKey;
    struct tnode *pLeft;
    struct tnode *pRight;
};

void *
tsearch(const void *pKey, void **ppRoot, int (*pfnCmp)(const void *, const void *))
{
    struct tnode **pp;
    struct tnode *pNew;
    int c;

    if (ppRoot == NULL || pfnCmp == NULL) {
        return NULL;
    }
    pp = (struct tnode **)ppRoot;
    while (*pp != NULL) {
        c = pfnCmp(pKey, (*pp)->pKey);
        if (c == 0) {
            return *pp;
        }
        if (c < 0) {
            pp = &(*pp)->pLeft;
        } else {
            pp = &(*pp)->pRight;
        }
    }
    pNew = (struct tnode *)malloc(sizeof(*pNew));
    if (pNew == NULL) {
        return NULL;
    }
    pNew->pKey = (void *)(uintptr_t)pKey;
    pNew->pLeft = NULL;
    pNew->pRight = NULL;
    *pp = pNew;
    return pNew;
}

void *
tfind(const void *pKey, void *const *ppRoot,
      int (*pfnCmp)(const void *, const void *))
{
    struct tnode *p;

    if (ppRoot == NULL || pfnCmp == NULL) {
        return NULL;
    }
    p = *(struct tnode *const *)ppRoot;
    while (p != NULL) {
        int c = pfnCmp(pKey, p->pKey);

        if (c == 0) {
            return p;
        }
        p = (c < 0) ? p->pLeft : p->pRight;
    }
    return NULL;
}

static struct tnode *
tdelete_min(struct tnode **pp)
{
    struct tnode *p;

    while ((*pp)->pLeft != NULL) {
        pp = &(*pp)->pLeft;
    }
    p = *pp;
    *pp = p->pRight;
    return p;
}

void *
tdelete(const void *pKey, void **ppRoot, int (*pfnCmp)(const void *, const void *))
{
    struct tnode **pp;
    struct tnode *p;
    int c;

    if (ppRoot == NULL || pfnCmp == NULL) {
        return NULL;
    }
    pp = (struct tnode **)ppRoot;
    while (*pp != NULL) {
        c = pfnCmp(pKey, (*pp)->pKey);
        if (c == 0) {
            break;
        }
        if (c < 0) {
            pp = &(*pp)->pLeft;
        } else {
            pp = &(*pp)->pRight;
        }
    }
    if (*pp == NULL) {
        return NULL;
    }
    p = *pp;
    if (p->pLeft == NULL) {
        *pp = p->pRight;
    } else if (p->pRight == NULL) {
        *pp = p->pLeft;
    } else {
        struct tnode *pMin = tdelete_min(&p->pRight);

        pMin->pLeft = p->pLeft;
        pMin->pRight = p->pRight;
        *pp = pMin;
    }
    /* Return parent pointer shape is underspecified; return key ptr. */
    {
        void *pKeyRet = p->pKey;

        free(p);
        return pKeyRet;
    }
}

static void
twalk_rec(const struct tnode *p,
          void (*pfnAction)(const void *pNode, VISIT v, int nLevel), int nLevel)
{
    if (p == NULL || pfnAction == NULL) {
        return;
    }
    if (p->pLeft == NULL && p->pRight == NULL) {
        pfnAction(p, leaf, nLevel);
        return;
    }
    pfnAction(p, preorder, nLevel);
    twalk_rec(p->pLeft, pfnAction, nLevel + 1);
    pfnAction(p, postorder, nLevel);
    twalk_rec(p->pRight, pfnAction, nLevel + 1);
    pfnAction(p, endorder, nLevel);
}

void
twalk(const void *pRoot,
      void (*pfnAction)(const void *pNode, VISIT v, int nLevel))
{
    twalk_rec((const struct tnode *)pRoot, pfnAction, 0);
}

static void
twalk_r_rec(const struct tnode *p,
            void (*pfnAction)(const void *pNode, VISIT v, void *pClosure),
            void *pClosure)
{
    if (p == NULL || pfnAction == NULL) {
        return;
    }
    if (p->pLeft == NULL && p->pRight == NULL) {
        pfnAction(p, leaf, pClosure);
        return;
    }
    pfnAction(p, preorder, pClosure);
    twalk_r_rec(p->pLeft, pfnAction, pClosure);
    pfnAction(p, postorder, pClosure);
    twalk_r_rec(p->pRight, pfnAction, pClosure);
    pfnAction(p, endorder, pClosure);
}

void
twalk_r(const void *pRoot,
        void (*pfnAction)(const void *pNode, VISIT v, void *pClosure),
        void *pClosure)
{
    twalk_r_rec((const struct tnode *)pRoot, pfnAction, pClosure);
}

void
tdestroy(void *pRoot, void (*pfnFree)(void *p))
{
    struct tnode *p = (struct tnode *)pRoot;

    if (p == NULL) {
        return;
    }
    tdestroy(p->pLeft, pfnFree);
    tdestroy(p->pRight, pfnFree);
    if (pfnFree != NULL) {
        pfnFree(p->pKey);
    }
    free(p);
}

/* ---- hash table (hsearch family) ---------------------------------------- */

#define H_MAX 128

static ENTRY g_aHash[H_MAX];
static size_t g_nHash;
static size_t g_nHashCap;

int
hcreate(size_t nElem)
{
    size_t i;

    (void)nElem;
    for (i = 0; i < H_MAX; i++) {
        g_aHash[i].key = NULL;
        g_aHash[i].data = NULL;
    }
    g_nHash = 0;
    g_nHashCap = H_MAX;
    return 1;
}

void
hdestroy(void)
{
    size_t i;

    for (i = 0; i < H_MAX; i++) {
        g_aHash[i].key = NULL;
        g_aHash[i].data = NULL;
    }
    g_nHash = 0;
}

ENTRY *
hsearch(ENTRY item, ACTION action)
{
    size_t i;
    size_t free_i = (size_t)-1;

    if (g_nHashCap == 0) {
        if (!hcreate(H_MAX)) {
            return NULL;
        }
    }
    for (i = 0; i < H_MAX; i++) {
        if (g_aHash[i].key == NULL) {
            if (free_i == (size_t)-1) {
                free_i = i;
            }
            continue;
        }
        if (strcmp(g_aHash[i].key, item.key) == 0) {
            return &g_aHash[i];
        }
    }
    if (action == FIND) {
        return NULL;
    }
    if (free_i == (size_t)-1 || g_nHash >= H_MAX) {
        return NULL;
    }
    g_aHash[free_i] = item;
    g_nHash++;
    return &g_aHash[free_i];
}

/* Reentrant stubs share one table (bring-up). */
struct hsearch_data {
    int nDummy;
};

int
hcreate_r(size_t nElem, struct hsearch_data *pHtab)
{
    (void)pHtab;
    return hcreate(nElem);
}

void
hdestroy_r(struct hsearch_data *pHtab)
{
    (void)pHtab;
    hdestroy();
}

int
hsearch_r(ENTRY item, ACTION action, ENTRY **ppRet, struct hsearch_data *pHtab)
{
    ENTRY *p;

    (void)pHtab;
    p = hsearch(item, action);
    if (ppRet != NULL) {
        *ppRet = p;
    }
    return (p != NULL) ? 1 : 0;
}

/* ---- linear search ------------------------------------------------------ */

void *
lfind(const void *pKey, const void *pBase, size_t *pNmemb, size_t cb,
      int (*pfnCmp)(const void *, const void *))
{
    size_t i;
    const unsigned char *p = (const unsigned char *)pBase;

    if (pKey == NULL || pBase == NULL || pNmemb == NULL || pfnCmp == NULL ||
        cb == 0) {
        return NULL;
    }
    for (i = 0; i < *pNmemb; i++) {
        if (pfnCmp(pKey, p + i * cb) == 0) {
            return (void *)(uintptr_t)(p + i * cb);
        }
    }
    return NULL;
}

void *
lsearch(const void *pKey, void *pBase, size_t *pNmemb, size_t cb,
        int (*pfnCmp)(const void *, const void *))
{
    void *pFound;
    unsigned char *p;

    pFound = lfind(pKey, pBase, pNmemb, cb, pfnCmp);
    if (pFound != NULL) {
        return pFound;
    }
    if (pBase == NULL || pNmemb == NULL || cb == 0) {
        return NULL;
    }
    p = (unsigned char *)pBase;
    memcpy(p + (*pNmemb) * cb, pKey, cb);
    (*pNmemb)++;
    return p + ((*pNmemb) - 1) * cb;
}

/* ---- insque / remque (doubly-linked queue) ------------------------------ */

struct qelem {
    struct qelem *q_forw;
    struct qelem *q_back;
};

void
insque(void *pElem, void *pPred)
{
    struct qelem *pE = (struct qelem *)pElem;
    struct qelem *pP = (struct qelem *)pPred;

    if (pE == NULL) {
        return;
    }
    if (pP == NULL) {
        pE->q_forw = NULL;
        pE->q_back = NULL;
        return;
    }
    pE->q_forw = pP->q_forw;
    pE->q_back = pP;
    if (pP->q_forw != NULL) {
        pP->q_forw->q_back = pE;
    }
    pP->q_forw = pE;
}

void
remque(void *pElem)
{
    struct qelem *pE = (struct qelem *)pElem;

    if (pE == NULL) {
        return;
    }
    if (pE->q_back != NULL) {
        pE->q_back->q_forw = pE->q_forw;
    }
    if (pE->q_forw != NULL) {
        pE->q_forw->q_back = pE->q_back;
    }
    pE->q_forw = NULL;
    pE->q_back = NULL;
}

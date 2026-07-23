/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * search.h — binary tree, hash table, linear search, insque/remque.
 * Clean-room soft fill; not GNU glibc.
 */
#include <errno.h>
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

    if (ppRoot == NULL || pfnCmp == NULL || pKey == NULL) {
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

    if (ppRoot == NULL || pfnCmp == NULL || pKey == NULL) {
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
    struct tnode **ppParent;
    struct tnode *p;
    int c;

    if (ppRoot == NULL || pfnCmp == NULL || pKey == NULL) {
        return NULL;
    }
    pp = (struct tnode **)ppRoot;
    ppParent = NULL;
    while (*pp != NULL) {
        c = pfnCmp(pKey, (*pp)->pKey);
        if (c == 0) {
            break;
        }
        ppParent = pp;
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
    free(p);
    /* POSIX: return pointer to parent node (or root if deleted was root). */
    if (ppParent != NULL) {
        return *ppParent;
    }
    return *ppRoot;
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

#define H_DEFAULT 128
#define H_MAX     4096

struct htab {
    ENTRY *paSlot;
    size_t cCap;
    size_t cUsed;
};

/* Global non-reentrant table. */
static struct htab g_ht;

/* Full reentrant object (header only forward-declares the type). */
struct hsearch_data {
    struct htab tab;
};

static size_t
hash_key(const char *sz, size_t cCap)
{
    size_t h = 5381u;
    const unsigned char *p = (const unsigned char *)sz;

    if (sz == NULL || cCap == 0) {
        return 0;
    }
    while (*p != '\0') {
        h = ((h << 5) + h) + *p;
        p++;
    }
    return h % cCap;
}

static int
htab_create(struct htab *pTab, size_t nElem)
{
    size_t cCap;
    size_t i;

    if (pTab == NULL) {
        return 0;
    }
    if (pTab->paSlot != NULL) {
        /* Already created — POSIX says hcreate fails if called twice. */
        errno = ENOMEM;
        return 0;
    }
    cCap = nElem < 16 ? 16 : nElem;
    if (cCap > H_MAX) {
        cCap = H_MAX;
    }
    /* Load factor soft: 2x slots. */
    if (cCap < H_MAX / 2) {
        cCap *= 2;
    }
    pTab->paSlot = (ENTRY *)calloc(cCap, sizeof(ENTRY));
    if (pTab->paSlot == NULL) {
        return 0;
    }
    pTab->cCap = cCap;
    pTab->cUsed = 0;
    for (i = 0; i < cCap; i++) {
        pTab->paSlot[i].key = NULL;
        pTab->paSlot[i].data = NULL;
    }
    return 1;
}

static void
htab_destroy(struct htab *pTab)
{
    if (pTab == NULL) {
        return;
    }
    free(pTab->paSlot);
    pTab->paSlot = NULL;
    pTab->cCap = 0;
    pTab->cUsed = 0;
}

static ENTRY *
htab_search(struct htab *pTab, ENTRY item, ACTION action)
{
    size_t i0;
    size_t i;
    size_t free_i = (size_t)-1;

    if (pTab == NULL || pTab->paSlot == NULL || pTab->cCap == 0) {
        return NULL;
    }
    if (item.key == NULL) {
        return NULL;
    }
    i0 = hash_key(item.key, pTab->cCap);
    for (i = 0; i < pTab->cCap; i++) {
        size_t idx = (i0 + i) % pTab->cCap;

        if (pTab->paSlot[idx].key == NULL) {
            if (free_i == (size_t)-1) {
                free_i = idx;
            }
            break;
        }
        if (strcmp(pTab->paSlot[idx].key, item.key) == 0) {
            return &pTab->paSlot[idx];
        }
    }
    if (action == FIND) {
        return NULL;
    }
    if (free_i == (size_t)-1 || pTab->cUsed >= pTab->cCap) {
        return NULL;
    }
    pTab->paSlot[free_i] = item;
    pTab->cUsed++;
    return &pTab->paSlot[free_i];
}

int
hcreate(size_t nElem)
{
    if (nElem == 0) {
        nElem = H_DEFAULT;
    }
    return htab_create(&g_ht, nElem);
}

void
hdestroy(void)
{
    htab_destroy(&g_ht);
}

ENTRY *
hsearch(ENTRY item, ACTION action)
{
    if (g_ht.paSlot == NULL) {
        if (!hcreate(H_DEFAULT)) {
            return NULL;
        }
    }
    return htab_search(&g_ht, item, action);
}

int
hcreate_r(size_t nElem, struct hsearch_data *pHtab)
{
    if (pHtab == NULL) {
        errno = EINVAL;
        return 0;
    }
    memset(pHtab, 0, sizeof(*pHtab));
    if (nElem == 0) {
        nElem = H_DEFAULT;
    }
    return htab_create(&pHtab->tab, nElem);
}

void
hdestroy_r(struct hsearch_data *pHtab)
{
    if (pHtab == NULL) {
        return;
    }
    htab_destroy(&pHtab->tab);
}

int
hsearch_r(ENTRY item, ACTION action, ENTRY **ppRet, struct hsearch_data *pHtab)
{
    ENTRY *p;

    if (pHtab == NULL) {
        errno = EINVAL;
        if (ppRet != NULL) {
            *ppRet = NULL;
        }
        return 0;
    }
    if (pHtab->tab.paSlot == NULL) {
        if (!htab_create(&pHtab->tab, H_DEFAULT)) {
            if (ppRet != NULL) {
                *ppRet = NULL;
            }
            return 0;
        }
    }
    p = htab_search(&pHtab->tab, item, action);
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
    if (pKey == NULL || pBase == NULL || pNmemb == NULL || cb == 0 ||
        pfnCmp == NULL) {
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

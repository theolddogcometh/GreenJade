/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Clean-room <sys/tree.h> for libcgj. OpenSSH sshd (srclimit, monitor,
 * krl) uses the BSD RB_* API. This is an unbalanced BST that matches
 * that API (insert/find/remove/foreach). Not OpenBSD tree.h source.
 * Dual DoD B OPEN.
 */
#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#define RB_HEAD(name, type) \
    struct name { \
        struct type *rbh_root; \
    }

#define RB_INITIALIZER(root) { NULL }

#define RB_INIT(head) do { \
    (head)->rbh_root = NULL; \
} while (0)

#define RB_ENTRY(type) \
    struct { \
        struct type *rbe_left; \
        struct type *rbe_right; \
        struct type *rbe_parent; \
        int          rbe_color; \
    }

#define RB_LEFT(elm, field)   ((elm)->field.rbe_left)
#define RB_RIGHT(elm, field)  ((elm)->field.rbe_right)
#define RB_PARENT(elm, field) ((elm)->field.rbe_parent)
#define RB_ROOT(head)         ((head)->rbh_root)
#define RB_EMPTY(head)        (RB_ROOT(head) == NULL)

#define RB_SET(elm, parent, field) do { \
    RB_PARENT(elm, field) = (parent); \
    RB_LEFT(elm, field) = NULL; \
    RB_RIGHT(elm, field) = NULL; \
    (elm)->field.rbe_color = 0; \
} while (0)

#define RB_MIN(name, head) name##_RB_MINMAX(head, 0)
#define RB_MAX(name, head) name##_RB_MINMAX(head, 1)

#define RB_INSERT(name, head, elm) name##_RB_INSERT(head, elm)
#define RB_REMOVE(name, head, elm) name##_RB_REMOVE(head, elm)
#define RB_FIND(name, head, elm)   name##_RB_FIND(head, elm)
#define RB_NFIND(name, head, elm)  name##_RB_NFIND(head, elm)
#define RB_NEXT(name, head, elm)   name##_RB_NEXT(elm)
#define RB_PREV(name, head, elm)   name##_RB_PREV(elm)

#define RB_FOREACH(x, name, head) \
    for ((x) = RB_MIN(name, head); (x) != NULL; \
         (x) = name##_RB_NEXT(x))

#define RB_FOREACH_SAFE(x, name, head, y) \
    for ((x) = RB_MIN(name, head); \
         (x) != NULL && ((y) = name##_RB_NEXT(x), 1); \
         (x) = (y))

#define RB_GENERATE_STATIC(name, type, field, cmp) \
    RB_GENERATE(name, type, field, cmp, static)

#define RB_GENERATE(name, type, field, cmp, attr) \
attr struct type * \
name##_RB_MINMAX(struct name *head, int nMax) \
{ \
    struct type *p = RB_ROOT(head); \
    struct type *pWalk; \
    if (p == NULL) { \
        return NULL; \
    } \
    if (nMax) { \
        while ((pWalk = RB_RIGHT(p, field)) != NULL) { \
            p = pWalk; \
        } \
    } else { \
        while ((pWalk = RB_LEFT(p, field)) != NULL) { \
            p = pWalk; \
        } \
    } \
    return p; \
} \
attr struct type * \
name##_RB_NEXT(struct type *elm) \
{ \
    struct type *p; \
    if (RB_RIGHT(elm, field) != NULL) { \
        elm = RB_RIGHT(elm, field); \
        while (RB_LEFT(elm, field) != NULL) { \
            elm = RB_LEFT(elm, field); \
        } \
        return elm; \
    } \
    if ((p = RB_PARENT(elm, field)) == NULL) { \
        return NULL; \
    } \
    while (elm == RB_RIGHT(p, field)) { \
        elm = p; \
        if ((p = RB_PARENT(p, field)) == NULL) { \
            return NULL; \
        } \
    } \
    return p; \
} \
attr struct type * \
name##_RB_PREV(struct type *elm) \
{ \
    struct type *p; \
    if (RB_LEFT(elm, field) != NULL) { \
        elm = RB_LEFT(elm, field); \
        while (RB_RIGHT(elm, field) != NULL) { \
            elm = RB_RIGHT(elm, field); \
        } \
        return elm; \
    } \
    if ((p = RB_PARENT(elm, field)) == NULL) { \
        return NULL; \
    } \
    while (elm == RB_LEFT(p, field)) { \
        elm = p; \
        if ((p = RB_PARENT(p, field)) == NULL) { \
            return NULL; \
        } \
    } \
    return p; \
} \
attr struct type * \
name##_RB_FIND(struct name *head, struct type *elm) \
{ \
    struct type *p = RB_ROOT(head); \
    int nCmp; \
    while (p != NULL) { \
        nCmp = (cmp)(elm, p); \
        if (nCmp == 0) { \
            return p; \
        } \
        p = (nCmp < 0) ? RB_LEFT(p, field) : RB_RIGHT(p, field); \
    } \
    return NULL; \
} \
attr struct type * \
name##_RB_NFIND(struct name *head, struct type *elm) \
{ \
    struct type *p = RB_ROOT(head); \
    struct type *pBest = NULL; \
    int nCmp; \
    while (p != NULL) { \
        nCmp = (cmp)(elm, p); \
        if (nCmp == 0) { \
            return p; \
        } \
        if (nCmp < 0) { \
            pBest = p; \
            p = RB_LEFT(p, field); \
        } else { \
            p = RB_RIGHT(p, field); \
        } \
    } \
    return pBest; \
} \
attr struct type * \
name##_RB_INSERT(struct name *head, struct type *elm) \
{ \
    struct type *p = RB_ROOT(head); \
    struct type *pPar = NULL; \
    int nCmp = 0; \
    RB_SET(elm, NULL, field); \
    if (p == NULL) { \
        RB_ROOT(head) = elm; \
        return NULL; \
    } \
    while (p != NULL) { \
        pPar = p; \
        nCmp = (cmp)(elm, p); \
        if (nCmp == 0) { \
            return p; \
        } \
        p = (nCmp < 0) ? RB_LEFT(p, field) : RB_RIGHT(p, field); \
    } \
    RB_PARENT(elm, field) = pPar; \
    if (nCmp < 0) { \
        RB_LEFT(pPar, field) = elm; \
    } else { \
        RB_RIGHT(pPar, field) = elm; \
    } \
    return NULL; \
} \
attr struct type * \
name##_RB_REMOVE(struct name *head, struct type *elm) \
{ \
    struct type *pChild; \
    struct type *pPar; \
    struct type *pSucc; \
    if (RB_LEFT(elm, field) == NULL) { \
        pChild = RB_RIGHT(elm, field); \
    } else if (RB_RIGHT(elm, field) == NULL) { \
        pChild = RB_LEFT(elm, field); \
    } else { \
        pSucc = RB_RIGHT(elm, field); \
        while (RB_LEFT(pSucc, field) != NULL) { \
            pSucc = RB_LEFT(pSucc, field); \
        } \
        (void)name##_RB_REMOVE(head, pSucc); \
        RB_LEFT(pSucc, field) = RB_LEFT(elm, field); \
        if (RB_LEFT(pSucc, field) != NULL) { \
            RB_PARENT(RB_LEFT(pSucc, field), field) = pSucc; \
        } \
        RB_RIGHT(pSucc, field) = RB_RIGHT(elm, field); \
        if (RB_RIGHT(pSucc, field) != NULL) { \
            RB_PARENT(RB_RIGHT(pSucc, field), field) = pSucc; \
        } \
        pPar = RB_PARENT(elm, field); \
        RB_PARENT(pSucc, field) = pPar; \
        if (pPar == NULL) { \
            RB_ROOT(head) = pSucc; \
        } else if (RB_LEFT(pPar, field) == elm) { \
            RB_LEFT(pPar, field) = pSucc; \
        } else { \
            RB_RIGHT(pPar, field) = pSucc; \
        } \
        return elm; \
    } \
    pPar = RB_PARENT(elm, field); \
    if (pChild != NULL) { \
        RB_PARENT(pChild, field) = pPar; \
    } \
    if (pPar == NULL) { \
        RB_ROOT(head) = pChild; \
    } else if (RB_LEFT(pPar, field) == elm) { \
        RB_LEFT(pPar, field) = pChild; \
    } else { \
        RB_RIGHT(pPar, field) = pChild; \
    } \
    return elm; \
}

#ifdef __cplusplus
}
#endif

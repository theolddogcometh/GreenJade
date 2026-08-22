/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Clean-room BSD queue macros for libcgj. OpenSSH sshd/channels/servconf
 * use TAILQ_HEAD/ENTRY/INIT/INSERT_TAIL/REMOVE/FOREACH/FIRST/LAST.
 * Not a copy of OpenBSD sys/queue.h. Dual DoD B OPEN.
 */
#pragma once

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/* ---- singly-linked list ------------------------------------------------ */

#define SLIST_HEAD(name, type) \
    struct name { \
        struct type *slh_first; \
    }

#define SLIST_HEAD_INITIALIZER(head) { NULL }

#define SLIST_ENTRY(type) \
    struct { \
        struct type *sle_next; \
    }

#define SLIST_FIRST(head) ((head)->slh_first)
#define SLIST_END(head)   ((void *)0)
#define SLIST_EMPTY(head) (SLIST_FIRST(head) == SLIST_END(head))
#define SLIST_NEXT(elm, field) ((elm)->field.sle_next)

#define SLIST_FOREACH(var, head, field) \
    for ((var) = SLIST_FIRST(head); (var) != NULL; \
         (var) = SLIST_NEXT(var, field))

#define SLIST_INIT(head) do { \
    SLIST_FIRST(head) = NULL; \
} while (0)

#define SLIST_INSERT_HEAD(head, elm, field) do { \
    (elm)->field.sle_next = SLIST_FIRST(head); \
    SLIST_FIRST(head) = (elm); \
} while (0)

#define SLIST_REMOVE_HEAD(head, field) do { \
    SLIST_FIRST(head) = SLIST_NEXT(SLIST_FIRST(head), field); \
} while (0)

/* ---- list (doubly-linked) ---------------------------------------------- */

#define LIST_HEAD(name, type) \
    struct name { \
        struct type *lh_first; \
    }

#define LIST_HEAD_INITIALIZER(head) { NULL }

#define LIST_ENTRY(type) \
    struct { \
        struct type  *le_next; \
        struct type **le_prev; \
    }

#define LIST_FIRST(head) ((head)->lh_first)
#define LIST_END(head)   ((void *)0)
#define LIST_EMPTY(head) (LIST_FIRST(head) == LIST_END(head))
#define LIST_NEXT(elm, field) ((elm)->field.le_next)

#define LIST_FOREACH(var, head, field) \
    for ((var) = LIST_FIRST(head); (var) != NULL; \
         (var) = LIST_NEXT(var, field))

#define LIST_INIT(head) do { \
    LIST_FIRST(head) = NULL; \
} while (0)

#define LIST_INSERT_HEAD(head, elm, field) do { \
    if (((elm)->field.le_next = LIST_FIRST(head)) != NULL) \
        LIST_FIRST(head)->field.le_prev = &(elm)->field.le_next; \
    LIST_FIRST(head) = (elm); \
    (elm)->field.le_prev = &LIST_FIRST(head); \
} while (0)

#define LIST_REMOVE(elm, field) do { \
    if ((elm)->field.le_next != NULL) \
        (elm)->field.le_next->field.le_prev = (elm)->field.le_prev; \
    *(elm)->field.le_prev = (elm)->field.le_next; \
} while (0)

#define LIST_INSERT_AFTER(listelm, elm, field) do { \
    if (((elm)->field.le_next = (listelm)->field.le_next) != NULL) \
        (listelm)->field.le_next->field.le_prev = &(elm)->field.le_next; \
    (listelm)->field.le_next = (elm); \
    (elm)->field.le_prev = &(listelm)->field.le_next; \
} while (0)

#define LIST_INSERT_BEFORE(listelm, elm, field) do { \
    (elm)->field.le_prev = (listelm)->field.le_prev; \
    (elm)->field.le_next = (listelm); \
    *(listelm)->field.le_prev = (elm); \
    (listelm)->field.le_prev = &(elm)->field.le_next; \
} while (0)

/* ---- tail queue (OpenSSH sshd / sshconnect2) --------------------------- */

#define TAILQ_HEAD(name, type) \
    struct name { \
        struct type  *tqh_first; \
        struct type **tqh_last; \
    }

#define TAILQ_HEAD_INITIALIZER(head) { NULL, &(head).tqh_first }

#define TAILQ_ENTRY(type) \
    struct { \
        struct type  *tqe_next; \
        struct type **tqe_prev; \
    }

#define TAILQ_FIRST(head) ((head)->tqh_first)
#define TAILQ_END(head)   ((void *)0)
#define TAILQ_EMPTY(head) (TAILQ_FIRST(head) == TAILQ_END(head))
#define TAILQ_NEXT(elm, field) ((elm)->field.tqe_next)

#define TAILQ_LAST(head, headname) \
    (*(((struct headname *)((head)->tqh_last))->tqh_last))

#define TAILQ_FOREACH(var, head, field) \
    for ((var) = TAILQ_FIRST(head); (var) != NULL; \
         (var) = TAILQ_NEXT(var, field))

#define TAILQ_FOREACH_SAFE(var, head, field, tvar) \
    for ((var) = TAILQ_FIRST(head); \
         (var) != NULL && ((tvar) = TAILQ_NEXT(var, field), 1); \
         (var) = (tvar))

#define TAILQ_INIT(head) do { \
    (head)->tqh_first = NULL; \
    (head)->tqh_last = &(head)->tqh_first; \
} while (0)

#define TAILQ_INSERT_HEAD(head, elm, field) do { \
    if (((elm)->field.tqe_next = (head)->tqh_first) != NULL) \
        (head)->tqh_first->field.tqe_prev = &(elm)->field.tqe_next; \
    else \
        (head)->tqh_last = &(elm)->field.tqe_next; \
    (head)->tqh_first = (elm); \
    (elm)->field.tqe_prev = &(head)->tqh_first; \
} while (0)

#define TAILQ_INSERT_TAIL(head, elm, field) do { \
    (elm)->field.tqe_next = NULL; \
    (elm)->field.tqe_prev = (head)->tqh_last; \
    *(head)->tqh_last = (elm); \
    (head)->tqh_last = &(elm)->field.tqe_next; \
} while (0)

#define TAILQ_REMOVE(head, elm, field) do { \
    if (((elm)->field.tqe_next) != NULL) \
        (elm)->field.tqe_next->field.tqe_prev = (elm)->field.tqe_prev; \
    else \
        (head)->tqh_last = (elm)->field.tqe_prev; \
    *(elm)->field.tqe_prev = (elm)->field.tqe_next; \
} while (0)

#define TAILQ_PREV(elm, headname, field) \
    (*(((struct headname *)((elm)->field.tqe_prev))->tqh_last))

#define TAILQ_FOREACH_REVERSE(var, head, headname, field) \
    for ((var) = TAILQ_LAST((head), headname); (var) != NULL; \
         (var) = TAILQ_PREV((var), headname, field))

#define TAILQ_FOREACH_REVERSE_SAFE(var, head, headname, field, tvar) \
    for ((var) = TAILQ_LAST((head), headname); \
         (var) != NULL && \
         ((tvar) = TAILQ_PREV((var), headname, field), 1); \
         (var) = (tvar))

#define TAILQ_INSERT_AFTER(head, listelm, elm, field) do { \
    if (((elm)->field.tqe_next = (listelm)->field.tqe_next) != NULL) \
        (elm)->field.tqe_next->field.tqe_prev = &(elm)->field.tqe_next; \
    else \
        (head)->tqh_last = &(elm)->field.tqe_next; \
    (listelm)->field.tqe_next = (elm); \
    (elm)->field.tqe_prev = &(listelm)->field.tqe_next; \
} while (0)

#define TAILQ_INSERT_BEFORE(listelm, elm, field) do { \
    (elm)->field.tqe_prev = (listelm)->field.tqe_prev; \
    (elm)->field.tqe_next = (listelm); \
    *(listelm)->field.tqe_prev = (elm); \
    (listelm)->field.tqe_prev = &(elm)->field.tqe_next; \
} while (0)

#define TAILQ_CONCAT(head1, head2, field) do { \
    if (!TAILQ_EMPTY(head2)) { \
        *(head1)->tqh_last = (head2)->tqh_first; \
        (head2)->tqh_first->field.tqe_prev = (head1)->tqh_last; \
        (head1)->tqh_last = (head2)->tqh_last; \
        TAILQ_INIT((head2)); \
    } \
} while (0)

#ifdef __cplusplus
}
#endif

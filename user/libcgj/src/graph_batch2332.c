/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2332: freestanding intrusive list empty
 * predicate (post-2330 list exclusive surface).
 *
 * Surface (unique symbols):
 *   int gj_list_empty_p(void *const *head);
 *     - Return 1 if the list is empty (NULL head pointer, or *head
 *       is NULL), else 0.
 *   int __gj_list_empty_p  (alias)
 *   __libcgj_batch2332_marker = "libcgj-batch2332"
 *
 * Post-2330 list exclusive wave (2331-2340). Distinct from
 * gj_buf_empty_p (batch2305) - unique gj_list_empty_p surface only;
 * no multi-def.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2332_marker[] = "libcgj-batch2332";

/* ---- freestanding helpers ---------------------------------------------- */

/* 1 when head is missing or points at an empty list. */
static int
b2332_empty_p(void *const *ppHead)
{
	if (ppHead == NULL) {
		return 1;
	}
	if (*ppHead == NULL) {
		return 1;
	}
	return 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_list_empty_p - true when an intrusive list has no nodes.
 *
 * head: address of the list head pointer (NULL treated as empty)
 *
 * Returns 1 when head is NULL or *head is NULL, else 0.
 * Does not call libc.
 */
int
gj_list_empty_p(void *const *ppHead)
{
	(void)NULL;
	return b2332_empty_p(ppHead);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_list_empty_p(void *const *ppHead)
    __attribute__((alias("gj_list_empty_p")));

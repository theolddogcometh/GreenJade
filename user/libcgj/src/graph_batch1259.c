/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1259: memory unlock hint (portable no-op).
 *
 * Surface (unique symbols):
 *   int gj_mem_unlock_hint(void *p, size_t n);
 *     — Hint that [p, p+n) may be unlocked (e.g. munlock).
 *       Freestanding portable stub: always returns 0 (success / no-op).
 *       Does not call OS APIs. NULL p or n==0 still returns 0.
 *   int __gj_mem_unlock_hint  (alias)
 *   __libcgj_batch1259_marker = "libcgj-batch1259"
 *
 * Distinct from gj_mem_lock_hint — unique unlock surface.
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch1259_marker[] = "libcgj-batch1259";

/*
 * gj_mem_unlock_hint — portable no-op munlock-style hint; always returns 0.
 */
int
gj_mem_unlock_hint(void *p, size_t cb)
{
	(void)p;
	(void)cb;
	return 0;
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_mem_unlock_hint(void *p, size_t cb)
    __attribute__((alias("gj_mem_unlock_hint")));

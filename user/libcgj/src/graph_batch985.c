/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch985: read-prefetch stub (freestanding no-op).
 *
 * Surface (unique symbols):
 *   void gj_prefetch_read(const void *p);
 *     — Hint that *p will be read soon. Freestanding pure-C stub: no-op
 *       (no __builtin_prefetch, no architecture prefetch insn).
 *   void __gj_prefetch_read  (alias)
 *   __libcgj_batch985_marker = "libcgj-batch985"
 *
 * Clean-room freestanding pure C. Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stdint.h>

const char __libcgj_batch985_marker[] = "libcgj-batch985";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_prefetch_read — optional read-locality hint (no-op freestanding).
 *
 * p: address that will be read; may be NULL (still a no-op).
 */
void
gj_prefetch_read(const void *p)
{
	(void)p;
}

/* ---- underscored alias ------------------------------------------------- */

void __gj_prefetch_read(const void *p)
    __attribute__((alias("gj_prefetch_read")));

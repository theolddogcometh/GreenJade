/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch887: plain sequential ticket-lock release.
 *
 * Advances the serving counter so the next ticket holder may enter.
 * Pair with gj_ticket_lock_acquire (batch886). No atomics.
 *
 * Surface (unique symbols):
 *   void gj_ticket_lock_release(uint32_t *serving);
 *     — *serving += 1. NULL → no-op.
 *   void __gj_ticket_lock_release  (alias)
 *   __libcgj_batch887_marker = "libcgj-batch887"
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch887_marker[] = "libcgj-batch887";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_ticket_lock_release — advance serving so the next waiter proceeds.
 *
 * Caller must currently hold the lock (their ticket == serving).
 * NULL serving is a no-op.
 */
void
gj_ticket_lock_release(uint32_t *serving)
{
	if (serving == NULL) {
		return;
	}
	*serving = *serving + 1u;
}

/* ---- underscored alias ------------------------------------------------- */

void __gj_ticket_lock_release(uint32_t *serving)
    __attribute__((alias("gj_ticket_lock_release")));

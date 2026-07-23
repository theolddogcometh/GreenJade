/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch885: plain sequential ticket-lock init.
 *
 * Ticket pair lives in caller-owned uint32_t slots (no struct, no atomics).
 * Cooperative / single-threaded use only.
 *
 * Surface (unique symbols):
 *   void gj_ticket_lock_init(uint32_t *next, uint32_t *serving);
 *     — Zero next and serving (unlocked, first ticket 0).
 *   void __gj_ticket_lock_init  (alias)
 *   __libcgj_batch885_marker = "libcgj-batch885"
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch885_marker[] = "libcgj-batch885";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_ticket_lock_init — reset ticket counters to unlocked state.
 *
 * next:    next ticket to hand out (starts at 0)
 * serving: ticket currently allowed to hold the lock (starts at 0)
 * NULL either pointer → that field is skipped; remaining non-NULL is cleared.
 */
void
gj_ticket_lock_init(uint32_t *next, uint32_t *serving)
{
	if (next != NULL) {
		*next = 0u;
	}
	if (serving != NULL) {
		*serving = 0u;
	}
}

/* ---- underscored alias ------------------------------------------------- */

void __gj_ticket_lock_init(uint32_t *next, uint32_t *serving)
    __attribute__((alias("gj_ticket_lock_init")));

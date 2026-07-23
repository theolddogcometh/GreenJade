/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch886: plain sequential ticket-lock acquire.
 *
 * No atomics — cooperative ticket hand-out + busy-wait on serving.
 * Pair with gj_ticket_lock_init (885) / gj_ticket_lock_release (887).
 *
 * Surface (unique symbols):
 *   void gj_ticket_lock_acquire(uint32_t *next, uint32_t *serving);
 *     — Take *next as ticket, bump *next, spin until *serving == ticket.
 *   void __gj_ticket_lock_acquire  (alias)
 *   __libcgj_batch886_marker = "libcgj-batch886"
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch886_marker[] = "libcgj-batch886";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_ticket_lock_acquire — plain sequential ticket acquire (no atomics).
 *
 * 1) my = *next; *next = my + 1
 * 2) while (*serving != my) spin
 *
 * NULL next or serving is a no-op. Intended for cooperative multitasking
 * or single-threaded critical-section nesting tests — not SMP-safe.
 */
void
gj_ticket_lock_acquire(uint32_t *next, uint32_t *serving)
{
	uint32_t my;

	if (next == NULL || serving == NULL) {
		return;
	}

	my = *next;
	*next = my + 1u;

	/* Busy-wait until our ticket is being served. */
	while (*serving != my) {
		/* cooperative spin — no pause/yield in freestanding pure C */
	}
}

/* ---- underscored alias ------------------------------------------------- */

void __gj_ticket_lock_acquire(uint32_t *next, uint32_t *serving)
    __attribute__((alias("gj_ticket_lock_acquire")));

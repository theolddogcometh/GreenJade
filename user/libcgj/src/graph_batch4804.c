/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4804: event-loop interest bit for write.
 *
 * Surface (unique symbols):
 *   uint32_t gj_ev_interest_write_u(void);
 *     - Returns the write interest flag (2) for event-loop masks.
 *   uint32_t __gj_ev_interest_write_u  (alias)
 *   __libcgj_batch4804_marker = "libcgj-batch4804"
 *
 * Exclusive continuum CREATE-ONLY (4801-4810). Unique
 * gj_ev_interest_write_u surface only; no multi-def. Distinct from
 * gj_ev_interest_read_u (batch4803) and gj_ev_interest_err_u
 * (batch4805). No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4804_marker[] = "libcgj-batch4804";

/* Write interest bit (bit 1). */
#define B4804_INTEREST_WRITE  2u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b4804_interest_write(void)
{
	return B4804_INTEREST_WRITE;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_ev_interest_write_u - report the event-loop write interest bit.
 *
 * Always returns 2 (bit 1). Compose with gj_ev_mask_or_u /
 * gj_ev_mask_has_u. No parent wires.
 */
uint32_t
gj_ev_interest_write_u(void)
{
	(void)NULL;
	return b4804_interest_write();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_ev_interest_write_u(void)
    __attribute__((alias("gj_ev_interest_write_u")));

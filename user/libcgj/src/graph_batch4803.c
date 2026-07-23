/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4803: event-loop interest bit for read.
 *
 * Surface (unique symbols):
 *   uint32_t gj_ev_interest_read_u(void);
 *     - Returns the read interest flag (1) for event-loop masks.
 *   uint32_t __gj_ev_interest_read_u  (alias)
 *   __libcgj_batch4803_marker = "libcgj-batch4803"
 *
 * Exclusive continuum CREATE-ONLY (4801-4810). Unique
 * gj_ev_interest_read_u surface only; no multi-def. Distinct from
 * gj_ev_interest_write_u (batch4804) and gj_ev_interest_err_u
 * (batch4805). No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4803_marker[] = "libcgj-batch4803";

/* Read interest bit (bit 0). */
#define B4803_INTEREST_READ  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b4803_interest_read(void)
{
	return B4803_INTEREST_READ;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_ev_interest_read_u - report the event-loop read interest bit.
 *
 * Always returns 1 (bit 0). Compose with gj_ev_mask_or_u /
 * gj_ev_mask_has_u. No parent wires.
 */
uint32_t
gj_ev_interest_read_u(void)
{
	(void)NULL;
	return b4803_interest_read();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_ev_interest_read_u(void)
    __attribute__((alias("gj_ev_interest_read_u")));

/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4805: event-loop interest bit for error.
 *
 * Surface (unique symbols):
 *   uint32_t gj_ev_interest_err_u(void);
 *     - Returns the error interest flag (4) for event-loop masks.
 *   uint32_t __gj_ev_interest_err_u  (alias)
 *   __libcgj_batch4805_marker = "libcgj-batch4805"
 *
 * Exclusive continuum CREATE-ONLY (4801-4810). Unique
 * gj_ev_interest_err_u surface only; no multi-def. Distinct from
 * gj_ev_interest_read_u (batch4803) and gj_ev_interest_write_u
 * (batch4804). No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4805_marker[] = "libcgj-batch4805";

/* Error interest bit (bit 2). */
#define B4805_INTEREST_ERR  4u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b4805_interest_err(void)
{
	return B4805_INTEREST_ERR;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_ev_interest_err_u - report the event-loop error interest bit.
 *
 * Always returns 4 (bit 2). Compose with gj_ev_mask_or_u /
 * gj_ev_mask_has_u. No parent wires.
 */
uint32_t
gj_ev_interest_err_u(void)
{
	(void)NULL;
	return b4805_interest_err();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_ev_interest_err_u(void)
    __attribute__((alias("gj_ev_interest_err_u")));

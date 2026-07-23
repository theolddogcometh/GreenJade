/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch12158: seat soft id ok stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_seat_ok_u_12158(void);
 *     - Returns 0 (seat soft id stub not asserted ready). Soft
 *       compile-time product tag; not a live seat/device probe.
 *   uint32_t __gj_seat_ok_u_12158  (alias)
 *   __libcgj_batch12158_marker = "libcgj-batch12158"
 *
 * Exclusive CREATE-ONLY (12151-12160: auth soft id stubs —
 * polkit_ok_u_12151, seatd_ok_u_12152, elogind_ok_u_12153,
 * pam_ok_u_12154, shadow_ok_u_12155, sudo_ok_u_12156,
 * session_ok_u_12157, seat_ok_u_12158, auth_soft_ready_u_12159,
 * batch_id_12160). Unique gj_seat_ok_u_12158 surface only; no
 * multi-def. Distinct from prior auth soft waves
 * (gj_*_u_11851..11860, gj_*_u_11651..11660) and sibling
 * auth soft ok_u stubs in this wave. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch12158_marker[] = "libcgj-batch12158";

/* Soft lamp: not asserted. */
#define B12158_SEAT_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b12158_seat_ok(void)
{
	return B12158_SEAT_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_seat_ok_u_12158 - seat soft id ok stub.
 *
 * Always returns 0. Soft pure-data lamp; does not open seat devices,
 * claim VT seats, or probe seatd/logind seat objects. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_seat_ok_u_12158(void)
{
	(void)NULL;
	return b12158_seat_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_seat_ok_u_12158(void)
    __attribute__((alias("gj_seat_ok_u_12158")));

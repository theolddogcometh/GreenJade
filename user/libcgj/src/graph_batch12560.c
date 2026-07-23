/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch12560: auth soft id stubs batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_12560(void);
 *     - Returns the compile-time graph batch number for this TU (12560).
 *   uint32_t __gj_batch_id_12560  (alias)
 *   __libcgj_batch12560_marker = "libcgj-batch12560"
 *
 * Exclusive CREATE-ONLY (12551-12560: auth soft id stubs —
 * polkit_ok_u_12551, seatd_ok_u_12552, elogind_ok_u_12553,
 * pam_ok_u_12554, shadow_ok_u_12555, sudo_ok_u_12556,
 * session_ok_u_12557, seat_ok_u_12558, auth_soft_ready_u_12559,
 * batch_id_12560). Unique gj_batch_id_12560 surface only; no multi-def.
 * Does NOT redefine gj_batch_id / gj_batch_id_12500 / prior batch_id_*
 * symbols - avoid multi-def. Closes the 12551-12560 continuum
 * (polkit/seatd/elogind/pam/shadow/sudo/session/seat soft ok stubs +
 * auth soft ready). Distinct from gj_batch_id_12360,
 * gj_batch_id_12160, and gj_batch_id_11860. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch12560_marker[] = "libcgj-batch12560";

#define B12560_BATCH_ID  12560u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b12560_id(void)
{
	return B12560_BATCH_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_12560 - report this TU's graph batch number.
 *
 * Always returns 12560. Soft pure-data identity for the auth soft id
 * stubs continuum close. Does not call libc. No parent wires.
 */
uint32_t
gj_batch_id_12560(void)
{
	(void)NULL;
	return b12560_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_12560(void)
    __attribute__((alias("gj_batch_id_12560")));

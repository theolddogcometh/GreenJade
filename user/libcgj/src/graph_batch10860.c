/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch10860: auth soft + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_10860(void);
 *     - Returns the compile-time graph batch number for this TU (10860).
 *   uint32_t __gj_batch_id_10860  (alias)
 *   __libcgj_batch10860_marker = "libcgj-batch10860"
 *
 * Exclusive continuum CREATE-ONLY (10851-10860: auth soft id stubs —
 * polkit_ok_u_10851, seatd_ok_u_10852, elogind_ok_u_10853,
 * pam_ok_u_10854, shadow_ok_u_10855, sudo_ok_u_10856,
 * session_ok_u_10857, seat_ok_u_10858, auth_soft_ready_u_10859,
 * batch_id_10860). Unique surfaces only; no multi-def. Does NOT
 * redefine gj_batch_id / gj_graph_milestone / prior batch_id_* symbols
 * — avoid multi-def. Distinct from gj_batch_id_10660 (batch10660) and
 * gj_batch_id_10460 (batch10460). No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch10860_marker[] = "libcgj-batch10860";

#define B10860_BATCH_ID  10860u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b10860_id(void)
{
	return B10860_BATCH_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_10860 - report this TU's graph batch number.
 *
 * Always returns 10860.
 */
uint32_t
gj_batch_id_10860(void)
{
	(void)NULL;
	return b10860_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_10860(void)
    __attribute__((alias("gj_batch_id_10860")));

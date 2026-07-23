/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch10460: auth soft + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_10460(void);
 *     - Returns the compile-time graph batch number for this TU (10460).
 *   uint32_t __gj_batch_id_10460  (alias)
 *   __libcgj_batch10460_marker = "libcgj-batch10460"
 *
 * Exclusive continuum CREATE-ONLY (10451-10460: auth soft id stubs —
 * polkit_ok_u_10451, seatd_ok_u_10452, elogind_ok_u_10453,
 * pam_ok_u_10454, shadow_ok_u_10455, sudo_ok_u_10456,
 * session_ok_u_10457, seat_ok_u_10458, auth_soft_ready_u_10459,
 * batch_id_10460). Unique surfaces only; no multi-def. Does NOT
 * redefine gj_batch_id / gj_graph_milestone / prior batch_id_* symbols
 * — avoid multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch10460_marker[] = "libcgj-batch10460";

#define B10460_BATCH_ID  10460u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b10460_id(void)
{
	return B10460_BATCH_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_10460 - report this TU's graph batch number.
 *
 * Always returns 10460.
 */
uint32_t
gj_batch_id_10460(void)
{
	(void)NULL;
	return b10460_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_10460(void)
    __attribute__((alias("gj_batch_id_10460")));

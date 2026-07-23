/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch10660: auth soft + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_10660(void);
 *     - Returns the compile-time graph batch number for this TU (10660).
 *   uint32_t __gj_batch_id_10660  (alias)
 *   __libcgj_batch10660_marker = "libcgj-batch10660"
 *
 * Exclusive continuum CREATE-ONLY (10651-10660: auth soft id stubs —
 * polkit_ok_u_10651, seatd_ok_u_10652, elogind_ok_u_10653,
 * pam_ok_u_10654, shadow_ok_u_10655, sudo_ok_u_10656,
 * session_ok_u_10657, seat_ok_u_10658, auth_soft_ready_u_10659,
 * batch_id_10660). Unique surfaces only; no multi-def. Does NOT
 * redefine gj_batch_id / gj_graph_milestone / prior batch_id_* symbols
 * — avoid multi-def. Distinct from gj_batch_id_10460 (batch10460) and
 * gj_batch_id_10600 (batch10600). No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch10660_marker[] = "libcgj-batch10660";

#define B10660_BATCH_ID  10660u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b10660_id(void)
{
	return B10660_BATCH_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_10660 - report this TU's graph batch number.
 *
 * Always returns 10660.
 */
uint32_t
gj_batch_id_10660(void)
{
	(void)NULL;
	return b10660_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_10660(void)
    __attribute__((alias("gj_batch_id_10660")));

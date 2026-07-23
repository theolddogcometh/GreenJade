/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch13160: auth soft + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_13160(void);
 *     - Returns the compile-time graph batch number for this TU (13160).
 *   uint32_t __gj_batch_id_13160  (alias)
 *   __libcgj_batch13160_marker = "libcgj-batch13160"
 *
 * Exclusive continuum CREATE-ONLY (13151-13160: auth soft id stubs —
 * polkit_ok_u_13151, seatd_ok_u_13152, elogind_ok_u_13153,
 * pam_ok_u_13154, shadow_ok_u_13155, sudo_ok_u_13156,
 * session_ok_u_13157, seat_ok_u_13158, auth_soft_ready_u_13159,
 * batch_id_13160). Unique surfaces only; no multi-def. Does NOT
 * redefine gj_batch_id / gj_graph_milestone / prior batch_id_* symbols
 * — avoid multi-def. Distinct from gj_batch_id_12960 (batch12960),
 * gj_batch_id_12760 (batch12760). No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch13160_marker[] = "libcgj-batch13160";

#define B13160_BATCH_ID  13160u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b13160_id(void)
{
	return B13160_BATCH_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_13160 - report this TU's graph batch number.
 *
 * Always returns 13160.
 */
uint32_t
gj_batch_id_13160(void)
{
	(void)NULL;
	return b13160_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_13160(void)
    __attribute__((alias("gj_batch_id_13160")));

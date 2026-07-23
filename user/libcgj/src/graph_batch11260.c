/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch11260: auth soft + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_11260(void);
 *     - Returns the compile-time graph batch number for this TU (11260).
 *   uint32_t __gj_batch_id_11260  (alias)
 *   __libcgj_batch11260_marker = "libcgj-batch11260"
 *
 * Exclusive continuum CREATE-ONLY (11251-11260: auth soft id stubs —
 * polkit_ok_u_11251, seatd_ok_u_11252, elogind_ok_u_11253,
 * pam_ok_u_11254, shadow_ok_u_11255, sudo_ok_u_11256,
 * session_ok_u_11257, seat_ok_u_11258, auth_soft_ready_u_11259,
 * batch_id_11260). Unique surfaces only; no multi-def. Does NOT
 * redefine gj_batch_id / gj_graph_milestone / prior batch_id_* symbols
 * — avoid multi-def. Distinct from gj_batch_id_11060 (batch11060),
 * gj_batch_id_10860 (batch10860), gj_batch_id_10660 (batch10660), and
 * gj_batch_id_10460 (batch10460). No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch11260_marker[] = "libcgj-batch11260";

#define B11260_BATCH_ID  11260u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b11260_id(void)
{
	return B11260_BATCH_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_11260 - report this TU's graph batch number.
 *
 * Always returns 11260. Closes the 11251-11260 auth soft continuum.
 */
uint32_t
gj_batch_id_11260(void)
{
	(void)NULL;
	return b11260_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_11260(void)
    __attribute__((alias("gj_batch_id_11260")));

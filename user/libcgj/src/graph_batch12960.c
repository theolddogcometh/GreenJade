/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch12960: auth soft + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_12960(void);
 *     - Returns the compile-time graph batch number for this TU (12960).
 *   uint32_t __gj_batch_id_12960  (alias)
 *   __libcgj_batch12960_marker = "libcgj-batch12960"
 *
 * Exclusive continuum CREATE-ONLY (12951-12960: auth soft id stubs —
 * polkit_ok_u_12951, seatd_ok_u_12952, elogind_ok_u_12953,
 * pam_ok_u_12954, shadow_ok_u_12955, sudo_ok_u_12956,
 * session_ok_u_12957, seat_ok_u_12958, auth_soft_ready_u_12959,
 * batch_id_12960). Unique surfaces only; no multi-def. Does NOT
 * redefine gj_batch_id / gj_graph_milestone / prior batch_id_* symbols
 * — avoid multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch12960_marker[] = "libcgj-batch12960";

#define B12960_BATCH_ID  12960u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b12960_id(void)
{
	return B12960_BATCH_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_12960 - report this TU's graph batch number.
 *
 * Always returns 12960.
 */
uint32_t
gj_batch_id_12960(void)
{
	(void)NULL;
	return b12960_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_12960(void)
    __attribute__((alias("gj_batch_id_12960")));

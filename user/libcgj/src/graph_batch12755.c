/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch12755: shadow soft id ok stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_shadow_ok_u_12755(void);
 *     - Returns 0 (shadow soft id stub not asserted ready). Soft
 *       compile-time product tag; not a live shadow/libshadow probe.
 *   uint32_t __gj_shadow_ok_u_12755  (alias)
 *   __libcgj_batch12755_marker = "libcgj-batch12755"
 *
 * Exclusive CREATE-ONLY (12751-12760: auth soft id stubs —
 * polkit_ok_u_12751, seatd_ok_u_12752, elogind_ok_u_12753,
 * pam_ok_u_12754, shadow_ok_u_12755, sudo_ok_u_12756,
 * session_ok_u_12757, seat_ok_u_12758, auth_soft_ready_u_12759,
 * batch_id_12760). Unique gj_shadow_ok_u_12755 surface only; no
 * multi-def. Distinct from prior auth soft waves
 * (gj_*_u_12551..12560, gj_*_u_12351..12360, gj_*_u_12151..12160)
 * and sibling auth soft ok_u stubs in this wave. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch12755_marker[] = "libcgj-batch12755";

/* Soft lamp: not asserted. */
#define B12755_SHADOW_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b12755_shadow_ok(void)
{
	return B12755_SHADOW_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_shadow_ok_u_12755 - shadow soft id ok stub.
 *
 * Always returns 0. Soft pure-data lamp; does not open /etc/shadow,
 * call getspnam, or probe password databases. Does not call libc.
 * No parent wires.
 */
uint32_t
gj_shadow_ok_u_12755(void)
{
	(void)NULL;
	return b12755_shadow_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_shadow_ok_u_12755(void)
    __attribute__((alias("gj_shadow_ok_u_12755")));

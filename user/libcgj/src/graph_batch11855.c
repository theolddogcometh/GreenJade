/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch11855: shadow soft id ok stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_shadow_ok_u_11855(void);
 *     - Returns 0 (shadow soft id stub not asserted ready). Soft
 *       compile-time product tag; not a live shadow/libshadow probe.
 *   uint32_t __gj_shadow_ok_u_11855  (alias)
 *   __libcgj_batch11855_marker = "libcgj-batch11855"
 *
 * Exclusive CREATE-ONLY (11851-11860: auth soft id stubs —
 * polkit_ok_u_11851, seatd_ok_u_11852, elogind_ok_u_11853,
 * pam_ok_u_11854, shadow_ok_u_11855, sudo_ok_u_11856,
 * session_ok_u_11857, seat_ok_u_11858, auth_soft_ready_u_11859,
 * batch_id_11860). Unique gj_shadow_ok_u_11855 surface only; no
 * multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch11855_marker[] = "libcgj-batch11855";

/* Soft lamp: shadow ok stub (not asserted). */
#define B11855_SHADOW_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b11855_shadow_ok(void)
{
	return B11855_SHADOW_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_shadow_ok_u_11855 - shadow soft id ok stub.
 *
 * Always returns 0. Soft pure-data lamp; does not open /etc/shadow,
 * call getspnam, or probe password databases. Does not call libc.
 * No parent wires.
 */
uint32_t
gj_shadow_ok_u_11855(void)
{
	(void)NULL;
	return b11855_shadow_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_shadow_ok_u_11855(void)
    __attribute__((alias("gj_shadow_ok_u_11855")));

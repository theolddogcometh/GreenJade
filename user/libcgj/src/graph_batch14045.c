/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch14045: shadow soft id ok stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_shadow_ok_u_14045(void);
 *     - Returns 0 (shadow soft id stub not asserted ready). Soft
 *       compile-time product tag; not a live shadow/libshadow probe.
 *   uint32_t __gj_shadow_ok_u_14045  (alias)
 *   __libcgj_batch14045_marker = "libcgj-batch14045"
 *
 * Exclusive continuum CREATE-ONLY (14041-14050: polkit/seatd soft id
 * stubs — polkit_ok_u_14041, seatd_ok_u_14042, elogind_ok_u_14043,
 * pam_ok_u_14044, shadow_ok_u_14045, sudo_ok_u_14046,
 * session_ok_u_14047, seat_ok_u_14048, auth_soft_ready_u_14049,
 * batch_id_14050). Unique gj_shadow_ok_u_14045 surface only; no
 * multi-def. Distinct from gj_shadow_ok_u_10155. No parent wires.
 * No __int128. No shadow implementation.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch14045_marker[] = "libcgj-batch14045";

/* Soft lamp: shadow ok stub (not asserted). */
#define B14045_SHADOW_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b14045_shadow_ok(void)
{
	return B14045_SHADOW_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_shadow_ok_u_14045 - shadow soft id ok stub.
 *
 * Always returns 0. Soft pure-data lamp; does not open /etc/shadow,
 * call getspnam, or probe password databases. Does not call libc.
 * No parent wires.
 */
uint32_t
gj_shadow_ok_u_14045(void)
{
	(void)NULL;
	return b14045_shadow_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_shadow_ok_u_14045(void)
    __attribute__((alias("gj_shadow_ok_u_14045")));

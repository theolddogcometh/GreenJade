/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch10151: polkit soft id ok stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_polkit_ok_u_10151(void);
 *     - Returns 0 (polkit soft id stub not asserted ready). Soft
 *       compile-time product tag; not a live polkit/libpolkit probe.
 *   uint32_t __gj_polkit_ok_u_10151  (alias)
 *   __libcgj_batch10151_marker = "libcgj-batch10151"
 *
 * Exclusive CREATE-ONLY (10151-10160: polkit/seatd soft id stubs). Unique
 * gj_polkit_ok_u_10151 surface only; no multi-def. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch10151_marker[] = "libcgj-batch10151";

/* Soft lamp: polkit ok stub (not asserted). */
#define B10151_POLKIT_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b10151_polkit_ok(void)
{
	return B10151_POLKIT_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_polkit_ok_u_10151 - polkit soft id ok stub.
 *
 * Always returns 0. Soft pure-data lamp; does not contact polkitd,
 * open D-Bus, or probe PolicyKit authorities. Does not call libc.
 * No parent wires.
 */
uint32_t
gj_polkit_ok_u_10151(void)
{
	(void)NULL;
	return b10151_polkit_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_polkit_ok_u_10151(void)
    __attribute__((alias("gj_polkit_ok_u_10151")));

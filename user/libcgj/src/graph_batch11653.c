/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch11653: elogind soft id ok stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_elogind_ok_u_11653(void);
 *     - Returns 0 (elogind soft id stub not asserted ready). Soft
 *       compile-time product tag; not a live elogind/logind probe.
 *   uint32_t __gj_elogind_ok_u_11653  (alias)
 *   __libcgj_batch11653_marker = "libcgj-batch11653"
 *
 * Exclusive CREATE-ONLY (11651-11660: polkit/seatd soft id stubs). Unique
 * gj_elogind_ok_u_11653 surface only; no multi-def. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch11653_marker[] = "libcgj-batch11653";

/* Soft lamp: elogind ok stub (not asserted). */
#define B11653_ELOGIND_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b11653_elogind_ok(void)
{
	return B11653_ELOGIND_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_elogind_ok_u_11653 - elogind soft id ok stub.
 *
 * Always returns 0. Soft pure-data lamp; does not contact elogind,
 * open logind D-Bus, or probe session managers. Does not call libc.
 * No parent wires.
 */
uint32_t
gj_elogind_ok_u_11653(void)
{
	(void)NULL;
	return b11653_elogind_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_elogind_ok_u_11653(void)
    __attribute__((alias("gj_elogind_ok_u_11653")));

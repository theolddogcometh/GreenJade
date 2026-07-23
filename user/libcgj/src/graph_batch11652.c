/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch11652: seatd soft id ok stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_seatd_ok_u_11652(void);
 *     - Returns 0 (seatd soft id stub not asserted ready). Soft
 *       compile-time product tag; not a live seatd/libseat probe.
 *   uint32_t __gj_seatd_ok_u_11652  (alias)
 *   __libcgj_batch11652_marker = "libcgj-batch11652"
 *
 * Exclusive CREATE-ONLY (11651-11660: polkit/seatd soft id stubs). Unique
 * gj_seatd_ok_u_11652 surface only; no multi-def. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch11652_marker[] = "libcgj-batch11652";

/* Soft lamp: seatd ok stub (not asserted). */
#define B11652_SEATD_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b11652_seatd_ok(void)
{
	return B11652_SEATD_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_seatd_ok_u_11652 - seatd soft id ok stub.
 *
 * Always returns 0. Soft pure-data lamp; does not open seatd sockets,
 * call libseat, or probe seat managers. Does not call libc. No parent
 * wires.
 */
uint32_t
gj_seatd_ok_u_11652(void)
{
	(void)NULL;
	return b11652_seatd_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_seatd_ok_u_11652(void)
    __attribute__((alias("gj_seatd_ok_u_11652")));

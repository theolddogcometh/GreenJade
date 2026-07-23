/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7424: access mode R_OK|W_OK composite mask stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_access_rw_mask_7424(void);
 *     - Returns soft R_OK|W_OK mask (0x6): read | write permission bits.
 *   uint32_t __gj_access_rw_mask_7424  (alias)
 *   __libcgj_batch7424_marker = "libcgj-batch7424"
 *
 * Exclusive continuum CREATE-ONLY (7421-7430: access mode R_OK W_OK stubs —
 * r_ok_u, w_ok_u, x_ok_u, rw_mask, known_modes, known_modes_ok, has_rw,
 * rw_both_u, any_u, continuum + batch_id_7430). Unique
 * gj_access_rw_mask_7424 surface only; no multi-def. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7424_marker[] = "libcgj-batch7424";

/*
 * Soft R_OK|W_OK composite (compile-time tags only):
 *   bit 1 W_OK (0x2)
 *   bit 2 R_OK (0x4)
 * Soft value 0x6. Not a kernel or access() probe.
 */
#define B7424_R_OK ((uint32_t)0x00000004u)
#define B7424_W_OK ((uint32_t)0x00000002u)
#define B7424_RW_MASK (B7424_R_OK | B7424_W_OK)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7424_rw_mask(void)
{
	return B7424_RW_MASK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_access_rw_mask_7424 - soft R_OK|W_OK composite mask.
 *
 * Always returns 0x6. Soft pure-data product tag; does not call access().
 * No parent wires.
 */
uint32_t
gj_access_rw_mask_7424(void)
{
	(void)NULL;
	return b7424_rw_mask();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_access_rw_mask_7424(void)
    __attribute__((alias("gj_access_rw_mask_7424")));

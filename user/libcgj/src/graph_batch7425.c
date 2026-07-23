/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7425: access mode known modes soft baseline.
 *
 * Surface (unique symbols):
 *   uint32_t gj_access_known_modes_7425(void);
 *     - Returns soft known access permission bits mask (0x7):
 *       X_OK | W_OK | R_OK. (F_OK is 0 and is not a bit.)
 *   uint32_t __gj_access_known_modes_7425  (alias)
 *   __libcgj_batch7425_marker = "libcgj-batch7425"
 *
 * Exclusive continuum CREATE-ONLY (7421-7430: access mode R_OK W_OK stubs —
 * r_ok_u, w_ok_u, x_ok_u, rw_mask, known_modes, known_modes_ok, has_rw,
 * rw_both_u, any_u, continuum + batch_id_7430). Unique
 * gj_access_known_modes_7425 surface only; no multi-def. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7425_marker[] = "libcgj-batch7425";

/*
 * Soft known access permission bits baseline (compile-time tags only):
 *   bit 0 X_OK (0x1)
 *   bit 1 W_OK (0x2)
 *   bit 2 R_OK (0x4)
 * Soft value 0x7. F_OK is 0 (existence) and is not a permission bit.
 * Not a kernel or access() probe.
 */
#define B7425_KNOWN_MODES ((uint32_t)0x00000007u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7425_known_modes(void)
{
	return B7425_KNOWN_MODES;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_access_known_modes_7425 - soft known access permission bits mask.
 *
 * Always returns 0x7. Soft pure-data product tag; does not call access().
 * No parent wires.
 */
uint32_t
gj_access_known_modes_7425(void)
{
	(void)NULL;
	return b7425_known_modes();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_access_known_modes_7425(void)
    __attribute__((alias("gj_access_known_modes_7425")));

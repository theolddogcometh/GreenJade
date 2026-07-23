/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch11433: IM soft xkbcommon ok stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_xkbcommon_ok_u_11433(void);
 *     - Returns 0 (soft stub: xkbcommon keymap stack slot is
 *       soft/unprobed; not a runtime libxkbcommon load or X11 probe).
 *   uint32_t __gj_xkbcommon_ok_u_11433  (alias)
 *   __libcgj_batch11433_marker = "libcgj-batch11433"
 *
 * Exclusive continuum CREATE-ONLY (11431-11440: IM soft stubs —
 * ok slots all→0; im_soft_ready→1; batch_id→11440). Unique
 * gj_xkbcommon_ok_u_11433 surface only; no multi-def. Distinct from
 * sibling IM soft ok_u stubs and prior xkb keymap soft tags. No parent
 * wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch11433_marker[] = "libcgj-batch11433";

/* Soft-stub xkbcommon ok value (unprobed). */
#define B11433_XKBCOMMON_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b11433_xkbcommon_ok(void)
{
	return B11433_XKBCOMMON_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_xkbcommon_ok_u_11433 - soft xkbcommon ok stub.
 *
 * Always returns 0 (soft/unprobed xkbcommon slot). Does not call libc
 * or load libxkbcommon/X11. No parent wires.
 */
uint32_t
gj_xkbcommon_ok_u_11433(void)
{
	(void)NULL;
	return b11433_xkbcommon_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_xkbcommon_ok_u_11433(void)
    __attribute__((alias("gj_xkbcommon_ok_u_11433")));

/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch12733: IM soft xkbcommon ok stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_xkbcommon_ok_u_12733(void);
 *     - Returns 0 (soft stub: xkbcommon keymap stack slot is
 *       soft/unprobed; not a runtime libxkbcommon load or X11 probe).
 *   uint32_t __gj_xkbcommon_ok_u_12733  (alias)
 *   __libcgj_batch12733_marker = "libcgj-batch12733"
 *
 * Exclusive continuum CREATE-ONLY (12731-12740: IM soft stubs —
 * ok slots all→0; im_soft_ready→1; batch_id→12740). Unique
 * gj_xkbcommon_ok_u_12733 surface only; no multi-def. Distinct from
 * sibling IM soft ok_u stubs and prior xkb keymap soft tags. No parent
 * wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch12733_marker[] = "libcgj-batch12733";

/* Soft-stub xkbcommon ok value (unprobed). */
#define B12733_XKBCOMMON_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b12733_xkbcommon_ok(void)
{
	return B12733_XKBCOMMON_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_xkbcommon_ok_u_12733 - soft xkbcommon ok stub.
 *
 * Always returns 0 (soft/unprobed xkbcommon slot). Does not call libc
 * or load libxkbcommon/X11. No parent wires.
 */
uint32_t
gj_xkbcommon_ok_u_12733(void)
{
	(void)NULL;
	return b12733_xkbcommon_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_xkbcommon_ok_u_12733(void)
    __attribute__((alias("gj_xkbcommon_ok_u_12733")));

/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch10633: IM soft xkbcommon ok stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_xkbcommon_ok_u_10633(void);
 *     - Returns 0 (soft stub: xkbcommon keymap stack slot is
 *       soft/unprobed; not a runtime libxkbcommon load or X11 probe).
 *   uint32_t __gj_xkbcommon_ok_u_10633  (alias)
 *   __libcgj_batch10633_marker = "libcgj-batch10633"
 *
 * Exclusive continuum CREATE-ONLY (10631-10640: IM soft stubs —
 * ok slots all→0; im_soft_ready→1; batch_id→10640). Unique
 * gj_xkbcommon_ok_u_10633 surface only; no multi-def. Distinct from
 * sibling IM soft ok_u stubs and prior xkb keymap soft tags. No parent
 * wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch10633_marker[] = "libcgj-batch10633";

/* Soft-stub xkbcommon ok value (unprobed). */
#define B10633_XKBCOMMON_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b10633_xkbcommon_ok(void)
{
	return B10633_XKBCOMMON_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_xkbcommon_ok_u_10633 - soft xkbcommon ok stub.
 *
 * Always returns 0 (soft/unprobed xkbcommon slot). Does not call libc
 * or load libxkbcommon. No parent wires.
 */
uint32_t
gj_xkbcommon_ok_u_10633(void)
{
	(void)NULL;
	return b10633_xkbcommon_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_xkbcommon_ok_u_10633(void)
    __attribute__((alias("gj_xkbcommon_ok_u_10633")));

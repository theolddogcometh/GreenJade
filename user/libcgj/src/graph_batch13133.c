/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch13133: IM soft xkbcommon ok stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_xkbcommon_ok_u_13133(void);
 *     - Returns 0 (soft stub: xkbcommon keymap stack slot is
 *       soft/unprobed; not a runtime libxkbcommon load or X11 probe).
 *   uint32_t __gj_xkbcommon_ok_u_13133  (alias)
 *   __libcgj_batch13133_marker = "libcgj-batch13133"
 *
 * Exclusive continuum CREATE-ONLY (13131-13140: IM soft stubs —
 * ok slots all→0; im_soft_ready→1; batch_id→13140 —
 * ibus_ok_u_13131, fcitx_ok_u_13132, xkbcommon_ok_u_13133,
 * compose_ok_u_13134, im_module_ok_u_13135, locale_im_ok_u_13136,
 * input_method_ok_u_13137, keyboard_layout_ok_u_13138,
 * im_soft_ready_u_13139, batch_id_13140). Unique
 * gj_xkbcommon_ok_u_13133 surface only; no multi-def. Distinct from
 * sibling IM soft ok_u stubs, prior xkb keymap soft tags, and
 * gj_xkbcommon_ok_u_12933 (batch12933) / gj_xkbcommon_ok_u_10433
 * (batch10433). No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch13133_marker[] = "libcgj-batch13133";

/* Soft-stub xkbcommon ok value (unprobed). */
#define B13133_XKBCOMMON_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b13133_xkbcommon_ok(void)
{
	return B13133_XKBCOMMON_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_xkbcommon_ok_u_13133 - soft xkbcommon ok stub.
 *
 * Always returns 0 (soft/unprobed xkbcommon slot). Does not call libc
 * or load libxkbcommon/X11. No parent wires.
 */
uint32_t
gj_xkbcommon_ok_u_13133(void)
{
	(void)NULL;
	return b13133_xkbcommon_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_xkbcommon_ok_u_13133(void)
    __attribute__((alias("gj_xkbcommon_ok_u_13133")));

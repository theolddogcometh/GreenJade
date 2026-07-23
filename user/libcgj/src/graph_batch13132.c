/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch13132: IM soft fcitx ok stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_fcitx_ok_u_13132(void);
 *     - Returns 0 (soft stub: fcitx/input-method framework slot is
 *       soft/unprobed; not a runtime fcitx or fcitx5 probe).
 *   uint32_t __gj_fcitx_ok_u_13132  (alias)
 *   __libcgj_batch13132_marker = "libcgj-batch13132"
 *
 * Exclusive continuum CREATE-ONLY (13131-13140: IM soft stubs —
 * ok slots all→0; im_soft_ready→1; batch_id→13140 —
 * ibus_ok_u_13131, fcitx_ok_u_13132, xkbcommon_ok_u_13133,
 * compose_ok_u_13134, im_module_ok_u_13135, locale_im_ok_u_13136,
 * input_method_ok_u_13137, keyboard_layout_ok_u_13138,
 * im_soft_ready_u_13139, batch_id_13140). Unique gj_fcitx_ok_u_13132
 * surface only; no multi-def. Distinct from sibling IM soft ok_u stubs
 * in this wave and prior gj_fcitx_ok_u_12932 (batch12932) /
 * gj_fcitx_ok_u_10432 (batch10432). No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch13132_marker[] = "libcgj-batch13132";

/* Soft-stub fcitx ok value (unprobed). */
#define B13132_FCITX_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b13132_fcitx_ok(void)
{
	return B13132_FCITX_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_fcitx_ok_u_13132 - soft fcitx ok stub.
 *
 * Always returns 0 (soft/unprobed fcitx slot). Does not call libc or
 * probe fcitx/fcitx5. No parent wires.
 */
uint32_t
gj_fcitx_ok_u_13132(void)
{
	(void)NULL;
	return b13132_fcitx_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_fcitx_ok_u_13132(void)
    __attribute__((alias("gj_fcitx_ok_u_13132")));

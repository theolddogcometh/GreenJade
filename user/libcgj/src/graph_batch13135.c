/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch13135: IM soft im-module ok stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_im_module_ok_u_13135(void);
 *     - Returns 0 (soft stub: GTK/Qt immodule loader slot is
 *       soft/unprobed; not a runtime GTK_IM_MODULE / QT_IM_MODULE
 *       probe).
 *   uint32_t __gj_im_module_ok_u_13135  (alias)
 *   __libcgj_batch13135_marker = "libcgj-batch13135"
 *
 * Exclusive continuum CREATE-ONLY (13131-13140: IM soft stubs —
 * ok slots all→0; im_soft_ready→1; batch_id→13140 —
 * ibus_ok_u_13131, fcitx_ok_u_13132, xkbcommon_ok_u_13133,
 * compose_ok_u_13134, im_module_ok_u_13135, locale_im_ok_u_13136,
 * input_method_ok_u_13137, keyboard_layout_ok_u_13138,
 * im_soft_ready_u_13139, batch_id_13140). Unique
 * gj_im_module_ok_u_13135 surface only; no multi-def. Distinct from
 * sibling IM soft ok_u stubs in this wave and prior
 * gj_im_module_ok_u_12935 (batch12935) / gj_im_module_ok_u_10435
 * (batch10435). No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch13135_marker[] = "libcgj-batch13135";

/* Soft-stub im-module ok value (unprobed). */
#define B13135_IM_MODULE_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b13135_im_module_ok(void)
{
	return B13135_IM_MODULE_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_im_module_ok_u_13135 - soft im-module ok stub.
 *
 * Always returns 0 (soft/unprobed immodule slot). Does not call libc
 * or inspect GTK_IM_MODULE / QT_IM_MODULE. No parent wires.
 */
uint32_t
gj_im_module_ok_u_13135(void)
{
	(void)NULL;
	return b13135_im_module_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_im_module_ok_u_13135(void)
    __attribute__((alias("gj_im_module_ok_u_13135")));

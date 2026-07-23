/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch13131: IM soft ibus ok stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_ibus_ok_u_13131(void);
 *     - Returns 0 (soft stub: ibus/input-method bus slot is soft/unprobed;
 *       not a runtime ibus daemon or D-Bus probe).
 *   uint32_t __gj_ibus_ok_u_13131  (alias)
 *   __libcgj_batch13131_marker = "libcgj-batch13131"
 *
 * Exclusive continuum CREATE-ONLY (13131-13140: IM soft stubs —
 * ok slots all→0; im_soft_ready→1; batch_id→13140 —
 * ibus_ok_u_13131, fcitx_ok_u_13132, xkbcommon_ok_u_13133,
 * compose_ok_u_13134, im_module_ok_u_13135, locale_im_ok_u_13136,
 * input_method_ok_u_13137, keyboard_layout_ok_u_13138,
 * im_soft_ready_u_13139, batch_id_13140). Unique gj_ibus_ok_u_13131
 * surface only; no multi-def. Distinct from sibling IM soft ok_u stubs
 * in this wave and prior gj_ibus_ok_u_12931 (batch12931) /
 * gj_ibus_ok_u_10431 (batch10431). No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch13131_marker[] = "libcgj-batch13131";

/* Soft-stub ibus ok value (unprobed). */
#define B13131_IBUS_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b13131_ibus_ok(void)
{
	return B13131_IBUS_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_ibus_ok_u_13131 - soft ibus ok stub.
 *
 * Always returns 0 (soft/unprobed ibus slot). Does not call libc or
 * probe ibus/D-Bus. No parent wires.
 */
uint32_t
gj_ibus_ok_u_13131(void)
{
	(void)NULL;
	return b13131_ibus_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_ibus_ok_u_13131(void)
    __attribute__((alias("gj_ibus_ok_u_13131")));

/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch13134: IM soft compose ok stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_compose_ok_u_13134(void);
 *     - Returns 0 (soft stub: X11/compose sequence table slot is
 *       soft/unprobed; not a runtime Compose file or XIM probe).
 *   uint32_t __gj_compose_ok_u_13134  (alias)
 *   __libcgj_batch13134_marker = "libcgj-batch13134"
 *
 * Exclusive continuum CREATE-ONLY (13131-13140: IM soft stubs —
 * ok slots all→0; im_soft_ready→1; batch_id→13140 —
 * ibus_ok_u_13131, fcitx_ok_u_13132, xkbcommon_ok_u_13133,
 * compose_ok_u_13134, im_module_ok_u_13135, locale_im_ok_u_13136,
 * input_method_ok_u_13137, keyboard_layout_ok_u_13138,
 * im_soft_ready_u_13139, batch_id_13140). Unique gj_compose_ok_u_13134
 * surface only; no multi-def. Distinct from sibling IM soft ok_u stubs
 * in this wave and prior gj_compose_ok_u_12934 (batch12934) /
 * gj_compose_ok_u_10434 (batch10434). No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch13134_marker[] = "libcgj-batch13134";

/* Soft-stub compose ok value (unprobed). */
#define B13134_COMPOSE_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b13134_compose_ok(void)
{
	return B13134_COMPOSE_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_compose_ok_u_13134 - soft compose ok stub.
 *
 * Always returns 0 (soft/unprobed compose slot). Does not call libc or
 * read Compose files. No parent wires.
 */
uint32_t
gj_compose_ok_u_13134(void)
{
	(void)NULL;
	return b13134_compose_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_compose_ok_u_13134(void)
    __attribute__((alias("gj_compose_ok_u_13134")));

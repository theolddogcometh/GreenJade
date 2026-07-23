/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch10432: IM soft fcitx ok stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_fcitx_ok_u_10432(void);
 *     - Returns 0 (soft stub: fcitx/input-method framework slot is
 *       soft/unprobed; not a runtime fcitx or fcitx5 probe).
 *   uint32_t __gj_fcitx_ok_u_10432  (alias)
 *   __libcgj_batch10432_marker = "libcgj-batch10432"
 *
 * Exclusive continuum CREATE-ONLY (10431-10440: IM soft stubs —
 * ok slots all→0; im_soft_ready→1; batch_id→10440). Unique
 * gj_fcitx_ok_u_10432 surface only; no multi-def. Distinct from sibling
 * IM soft ok_u stubs in this wave. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch10432_marker[] = "libcgj-batch10432";

/* Soft-stub fcitx ok value (unprobed). */
#define B10432_FCITX_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b10432_fcitx_ok(void)
{
	return B10432_FCITX_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_fcitx_ok_u_10432 - soft fcitx ok stub.
 *
 * Always returns 0 (soft/unprobed fcitx slot). Does not call libc or
 * probe fcitx/fcitx5. No parent wires.
 */
uint32_t
gj_fcitx_ok_u_10432(void)
{
	(void)NULL;
	return b10432_fcitx_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_fcitx_ok_u_10432(void)
    __attribute__((alias("gj_fcitx_ok_u_10432")));

/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch11235: IM soft im-module ok stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_im_module_ok_u_11235(void);
 *     - Returns 0 (soft stub: GTK/Qt immodule loader slot is
 *       soft/unprobed; not a runtime GTK_IM_MODULE / QT_IM_MODULE
 *       probe).
 *   uint32_t __gj_im_module_ok_u_11235  (alias)
 *   __libcgj_batch11235_marker = "libcgj-batch11235"
 *
 * Exclusive continuum CREATE-ONLY (11231-11240: IM soft stubs —
 * ok slots all→0; im_soft_ready→1; batch_id→11240). Unique
 * gj_im_module_ok_u_11235 surface only; no multi-def. Distinct from
 * sibling IM soft ok_u stubs in this wave. No parent wires. No
 * __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch11235_marker[] = "libcgj-batch11235";

/* Soft-stub im-module ok value (unprobed). */
#define B11235_IM_MODULE_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b11235_im_module_ok(void)
{
	return B11235_IM_MODULE_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_im_module_ok_u_11235 - soft im-module ok stub.
 *
 * Always returns 0 (soft/unprobed immodule slot). Does not call libc
 * or inspect GTK_IM_MODULE / QT_IM_MODULE. No parent wires.
 */
uint32_t
gj_im_module_ok_u_11235(void)
{
	(void)NULL;
	return b11235_im_module_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_im_module_ok_u_11235(void)
    __attribute__((alias("gj_im_module_ok_u_11235")));

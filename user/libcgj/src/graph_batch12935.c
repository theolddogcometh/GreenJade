/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch12935: IM soft im-module ok stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_im_module_ok_u_12935(void);
 *     - Returns 0 (soft stub: GTK/Qt immodule loader slot is
 *       soft/unprobed; not a runtime GTK_IM_MODULE / QT_IM_MODULE
 *       probe).
 *   uint32_t __gj_im_module_ok_u_12935  (alias)
 *   __libcgj_batch12935_marker = "libcgj-batch12935"
 *
 * Exclusive continuum CREATE-ONLY (12931-12940: IM soft stubs —
 * ok slots all→0; im_soft_ready→1; batch_id→12940). Unique
 * gj_im_module_ok_u_12935 surface only; no multi-def. Distinct from
 * sibling IM soft ok_u stubs in this wave. No parent wires. No
 * __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch12935_marker[] = "libcgj-batch12935";

/* Soft-stub im-module ok value (unprobed). */
#define B12935_IM_MODULE_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b12935_im_module_ok(void)
{
	return B12935_IM_MODULE_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_im_module_ok_u_12935 - soft im-module ok stub.
 *
 * Always returns 0 (soft/unprobed immodule slot). Does not call libc
 * or inspect GTK_IM_MODULE / QT_IM_MODULE. No parent wires.
 */
uint32_t
gj_im_module_ok_u_12935(void)
{
	(void)NULL;
	return b12935_im_module_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_im_module_ok_u_12935(void)
    __attribute__((alias("gj_im_module_ok_u_12935")));

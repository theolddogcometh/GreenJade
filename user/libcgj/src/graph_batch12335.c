/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch12335: IM module soft identity stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_im_module_ok_u_12335(void);
 *     - Returns 0 (soft: IM module path not ready at compile time).
 *   uint32_t __gj_im_module_ok_u_12335  (alias)
 *   __libcgj_batch12335_marker = "libcgj-batch12335"
 *
 * Exclusive CREATE-ONLY wave 12331-12340 (IM soft: ibus/fcitx soft id
 * stubs — ready→1 others 0). Unique gj_im_module_ok_u_12335 surface
 * only; no multi-def. Distinct from gj_im_module_ok_u_12135
 * (batch12135), gj_im_module_ok_u_11835 (batch11835), and sibling IM
 * soft stubs in this wave. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch12335_marker[] = "libcgj-batch12335";

/* Soft IM module ok lamp: not ready at compile time. */
#define B12335_IM_MODULE_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b12335_ok(void)
{
	return B12335_IM_MODULE_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_im_module_ok_u_12335 - soft IM module path ok lamp.
 *
 * Always returns 0 (not ready at compile time). Does not load IM
 * modules or call libc. No parent wires.
 */
uint32_t
gj_im_module_ok_u_12335(void)
{
	(void)NULL;
	return b12335_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_im_module_ok_u_12335(void)
    __attribute__((alias("gj_im_module_ok_u_12335")));

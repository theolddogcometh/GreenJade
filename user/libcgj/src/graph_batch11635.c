/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch11635: IM module soft identity stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_im_module_ok_u_11635(void);
 *     - Returns 0 (soft: IM module path not ready at compile time).
 *   uint32_t __gj_im_module_ok_u_11635  (alias)
 *   __libcgj_batch11635_marker = "libcgj-batch11635"
 *
 * Exclusive CREATE-ONLY wave 11631-11640 (ibus/fcitx soft id stubs).
 * Unique gj_im_module_ok_u_11635 surface only; no multi-def. No parent
 * wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch11635_marker[] = "libcgj-batch11635";

/* Soft IM module ok lamp: not ready at compile time. */
#define B11635_IM_MODULE_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b11635_ok(void)
{
	return B11635_IM_MODULE_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_im_module_ok_u_11635 - soft input-method module ok lamp.
 *
 * Always returns 0 (not ready at compile time). Does not scan
 * GTK/Qt IM modules or call libc. No parent wires.
 */
uint32_t
gj_im_module_ok_u_11635(void)
{
	(void)NULL;
	return b11635_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_im_module_ok_u_11635(void)
    __attribute__((alias("gj_im_module_ok_u_11635")));

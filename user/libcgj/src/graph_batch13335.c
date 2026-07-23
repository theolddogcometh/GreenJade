/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch13335: IM module soft identity stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_im_module_ok_u_13335(void);
 *     - Returns 0 (soft: IM module path not ready at compile time).
 *   uint32_t __gj_im_module_ok_u_13335  (alias)
 *   __libcgj_batch13335_marker = "libcgj-batch13335"
 *
 * Exclusive CREATE-ONLY wave 13331-13340 (ibus/fcitx soft id stubs).
 * Unique gj_im_module_ok_u_13335 surface only; no multi-def. No parent
 * wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch13335_marker[] = "libcgj-batch13335";

/* Soft IM module ok lamp: not ready at compile time. */
#define B13335_IM_MODULE_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b13335_ok(void)
{
	return B13335_IM_MODULE_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_im_module_ok_u_13335 - soft input-method module ok lamp.
 *
 * Always returns 0 (not ready at compile time). Does not scan
 * GTK/Qt IM modules or call libc. No parent wires.
 */
uint32_t
gj_im_module_ok_u_13335(void)
{
	(void)NULL;
	return b13335_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_im_module_ok_u_13335(void)
    __attribute__((alias("gj_im_module_ok_u_13335")));

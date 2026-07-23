/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch12137: input method soft identity stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_input_method_ok_u_12137(void);
 *     - Returns 0 (soft: generic input method not ready at compile time).
 *   uint32_t __gj_input_method_ok_u_12137  (alias)
 *   __libcgj_batch12137_marker = "libcgj-batch12137"
 *
 * Exclusive CREATE-ONLY wave 12131-12140 (IM soft: ibus/fcitx soft id
 * stubs — ready→1 others 0). Unique gj_input_method_ok_u_12137 surface
 * only; no multi-def. Distinct from gj_input_method_ok_u_11837
 * (batch11837), gj_input_method_ok_u_11637 (batch11637), and sibling IM
 * soft stubs in this wave. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch12137_marker[] = "libcgj-batch12137";

/* Soft input method ok lamp: not ready at compile time. */
#define B12137_INPUT_METHOD_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b12137_ok(void)
{
	return B12137_INPUT_METHOD_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_input_method_ok_u_12137 - soft generic input-method ok lamp.
 *
 * Always returns 0 (not ready at compile time). Does not select an IM
 * framework or call libc. No parent wires.
 */
uint32_t
gj_input_method_ok_u_12137(void)
{
	(void)NULL;
	return b12137_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_input_method_ok_u_12137(void)
    __attribute__((alias("gj_input_method_ok_u_12137")));

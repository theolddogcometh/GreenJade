/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch10137: input method soft identity stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_input_method_ok_u_10137(void);
 *     - Returns 0 (soft: generic input method not ready at compile time).
 *   uint32_t __gj_input_method_ok_u_10137  (alias)
 *   __libcgj_batch10137_marker = "libcgj-batch10137"
 *
 * Exclusive CREATE-ONLY wave 10131-10140 (ibus/fcitx soft id stubs).
 * Unique gj_input_method_ok_u_10137 surface only; no multi-def. No parent
 * wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch10137_marker[] = "libcgj-batch10137";

/* Soft input method ok lamp: not ready at compile time. */
#define B10137_INPUT_METHOD_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b10137_ok(void)
{
	return B10137_INPUT_METHOD_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_input_method_ok_u_10137 - soft generic input-method ok lamp.
 *
 * Always returns 0 (not ready at compile time). Does not select an IM
 * framework or call libc. No parent wires.
 */
uint32_t
gj_input_method_ok_u_10137(void)
{
	(void)NULL;
	return b10137_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_input_method_ok_u_10137(void)
    __attribute__((alias("gj_input_method_ok_u_10137")));

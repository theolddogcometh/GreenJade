/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch12337: input method soft identity stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_input_method_ok_u_12337(void);
 *     - Returns 0 (soft: generic input method not ready at compile time).
 *   uint32_t __gj_input_method_ok_u_12337  (alias)
 *   __libcgj_batch12337_marker = "libcgj-batch12337"
 *
 * Exclusive CREATE-ONLY wave 12331-12340 (IM soft: ibus/fcitx soft id
 * stubs — ready→1 others 0). Unique gj_input_method_ok_u_12337 surface
 * only; no multi-def. Distinct from gj_input_method_ok_u_12137
 * (batch12137), gj_input_method_ok_u_11837 (batch11837), and sibling IM
 * soft stubs in this wave. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch12337_marker[] = "libcgj-batch12337";

/* Soft generic input method ok lamp: not ready at compile time. */
#define B12337_INPUT_METHOD_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b12337_ok(void)
{
	return B12337_INPUT_METHOD_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_input_method_ok_u_12337 - soft generic input-method ok lamp.
 *
 * Always returns 0 (not ready at compile time). Does not probe IM
 * frameworks or call libc. No parent wires.
 */
uint32_t
gj_input_method_ok_u_12337(void)
{
	(void)NULL;
	return b12337_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_input_method_ok_u_12337(void)
    __attribute__((alias("gj_input_method_ok_u_12337")));

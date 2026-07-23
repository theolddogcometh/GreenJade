/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch12132: fcitx soft identity stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_fcitx_ok_u_12132(void);
 *     - Returns 0 (soft: fcitx input method not ready at compile time).
 *   uint32_t __gj_fcitx_ok_u_12132  (alias)
 *   __libcgj_batch12132_marker = "libcgj-batch12132"
 *
 * Exclusive CREATE-ONLY wave 12131-12140 (IM soft: ibus/fcitx soft id
 * stubs — ready→1 others 0). Unique gj_fcitx_ok_u_12132 surface only;
 * no multi-def. Distinct from gj_fcitx_ok_u_11832 (batch11832),
 * gj_fcitx_ok_u_11632 (batch11632), and sibling IM soft stubs in this
 * wave. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch12132_marker[] = "libcgj-batch12132";

/* Soft fcitx ok lamp: not ready at compile time. */
#define B12132_FCITX_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b12132_ok(void)
{
	return B12132_FCITX_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_fcitx_ok_u_12132 - soft fcitx input-method ok lamp.
 *
 * Always returns 0 (not ready at compile time). Does not probe fcitx
 * daemon or call libc. No parent wires.
 */
uint32_t
gj_fcitx_ok_u_12132(void)
{
	(void)NULL;
	return b12132_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_fcitx_ok_u_12132(void)
    __attribute__((alias("gj_fcitx_ok_u_12132")));

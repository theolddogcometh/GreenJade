/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch12131: ibus soft identity stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_ibus_ok_u_12131(void);
 *     - Returns 0 (soft: ibus input method not ready at compile time).
 *   uint32_t __gj_ibus_ok_u_12131  (alias)
 *   __libcgj_batch12131_marker = "libcgj-batch12131"
 *
 * Exclusive CREATE-ONLY wave 12131-12140 (IM soft: ibus/fcitx soft id
 * stubs — ready→1 others 0). Unique gj_ibus_ok_u_12131 surface only;
 * no multi-def. Distinct from gj_ibus_ok_u_11831 (batch11831),
 * gj_ibus_ok_u_11631 (batch11631), and sibling IM soft stubs in this
 * wave. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch12131_marker[] = "libcgj-batch12131";

/* Soft ibus ok lamp: not ready at compile time. */
#define B12131_IBUS_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b12131_ok(void)
{
	return B12131_IBUS_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_ibus_ok_u_12131 - soft ibus input-method ok lamp.
 *
 * Always returns 0 (not ready at compile time). Does not probe ibus
 * daemon or call libc. No parent wires.
 */
uint32_t
gj_ibus_ok_u_12131(void)
{
	(void)NULL;
	return b12131_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_ibus_ok_u_12131(void)
    __attribute__((alias("gj_ibus_ok_u_12131")));

/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch10631: IM soft ibus ok stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_ibus_ok_u_10631(void);
 *     - Returns 0 (soft stub: ibus/input-method bus slot is soft/unprobed;
 *       not a runtime ibus daemon or D-Bus probe).
 *   uint32_t __gj_ibus_ok_u_10631  (alias)
 *   __libcgj_batch10631_marker = "libcgj-batch10631"
 *
 * Exclusive continuum CREATE-ONLY (10631-10640: IM soft stubs —
 * ok slots all→0; im_soft_ready→1; batch_id→10640). Unique
 * gj_ibus_ok_u_10631 surface only; no multi-def. Distinct from sibling
 * IM soft ok_u stubs in this wave. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch10631_marker[] = "libcgj-batch10631";

/* Soft-stub ibus ok value (unprobed). */
#define B10631_IBUS_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b10631_ibus_ok(void)
{
	return B10631_IBUS_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_ibus_ok_u_10631 - soft ibus ok stub.
 *
 * Always returns 0 (soft/unprobed ibus slot). Does not call libc or
 * probe ibus/D-Bus. No parent wires.
 */
uint32_t
gj_ibus_ok_u_10631(void)
{
	(void)NULL;
	return b10631_ibus_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_ibus_ok_u_10631(void)
    __attribute__((alias("gj_ibus_ok_u_10631")));

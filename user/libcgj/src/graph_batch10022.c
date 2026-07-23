/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch10022: RADV soft id ok stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_radv_ok_u_10022(void);
 *     - Returns 0 (RADV soft id stub not asserted ready). Soft
 *       compile-time product tag; not a live radv/Vulkan ICD probe.
 *   uint32_t __gj_radv_ok_u_10022  (alias)
 *   __libcgj_batch10022_marker = "libcgj-batch10022"
 *
 * Exclusive CREATE-ONLY (10021-10030: mesa/radv soft id stubs). Unique
 * gj_radv_ok_u_10022 surface only; no multi-def. Distinct from
 * gj_mesa_ok_u_10021 and gj_mesa_radv_deck_ready_5518. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch10022_marker[] = "libcgj-batch10022";

/* Soft lamp: RADV ok stub (not asserted). */
#define B10022_RADV_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b10022_radv_ok(void)
{
	return B10022_RADV_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_radv_ok_u_10022 - RADV soft id ok stub.
 *
 * Always returns 0. Soft pure-data lamp; does not load radv, enumerate
 * Vulkan devices, or probe ICD paths. Does not call libc. No parent
 * wires.
 */
uint32_t
gj_radv_ok_u_10022(void)
{
	(void)NULL;
	return b10022_radv_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_radv_ok_u_10022(void)
    __attribute__((alias("gj_radv_ok_u_10022")));

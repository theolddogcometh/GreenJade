/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch10024: ANV soft id ok stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_anv_ok_u_10024(void);
 *     - Returns 0 (Intel ANV soft id stub not asserted ready). Soft
 *       compile-time product tag; not a live anv/Vulkan ICD probe.
 *   uint32_t __gj_anv_ok_u_10024  (alias)
 *   __libcgj_batch10024_marker = "libcgj-batch10024"
 *
 * Exclusive CREATE-ONLY (10021-10030: mesa/radv soft id stubs). Unique
 * gj_anv_ok_u_10024 surface only; no multi-def. Distinct from
 * gj_mesa_ok_u_10021, gj_radv_ok_u_10022, and gj_amdvlk_ok_u_10023.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch10024_marker[] = "libcgj-batch10024";

/* Soft lamp: ANV ok stub (not asserted). */
#define B10024_ANV_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b10024_anv_ok(void)
{
	return B10024_ANV_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_anv_ok_u_10024 - ANV soft id ok stub.
 *
 * Always returns 0. Soft pure-data lamp; does not load anv, enumerate
 * Intel Vulkan devices, or probe ICD paths. Does not call libc. No
 * parent wires.
 */
uint32_t
gj_anv_ok_u_10024(void)
{
	(void)NULL;
	return b10024_anv_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_anv_ok_u_10024(void)
    __attribute__((alias("gj_anv_ok_u_10024")));

/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch10724: Intel ANV soft id ok stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_anv_ok_u_10724(void);
 *     - Returns 0 (Intel ANV soft id stub not asserted ready). Soft
 *       compile-time product tag; not a live anv/Vulkan ICD probe.
 *   uint32_t __gj_anv_ok_u_10724  (alias)
 *   __libcgj_batch10724_marker = "libcgj-batch10724"
 *
 * Exclusive continuum CREATE-ONLY (10721-10730: mesa soft stubs —
 * ok slots all→0; mesa_soft_ready→1; batch_id→10730). Unique
 * gj_anv_ok_u_10724 surface only; no multi-def. Distinct from
 * gj_anv_ok_u_10024 and sibling mesa soft ok_u stubs in this wave.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch10724_marker[] = "libcgj-batch10724";

/* Soft lamp: Intel ANV ok stub (not asserted). */
#define B10724_ANV_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b10724_anv_ok(void)
{
	return B10724_ANV_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_anv_ok_u_10724 - Intel ANV soft id ok stub.
 *
 * Always returns 0. Soft pure-data lamp; does not load anv, enumerate
 * Vulkan devices, or probe ICD paths. Does not call libc. No parent
 * wires.
 */
uint32_t
gj_anv_ok_u_10724(void)
{
	(void)NULL;
	return b10724_anv_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_anv_ok_u_10724(void)
    __attribute__((alias("gj_anv_ok_u_10724")));

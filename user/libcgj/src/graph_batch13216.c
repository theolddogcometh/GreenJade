/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch13216: compat-tool ok soft unit (not a probe).
 *
 * Surface (unique symbols):
 *   uint32_t gj_compat_tool_ok_u_13216(void);
 *     - Always returns 0. Soft stub only; intentionally not a real
 *       Steam compat-tool probe for the proton soft continuum.
 *   uint32_t __gj_compat_tool_ok_u_13216  (alias)
 *   __libcgj_batch13216_marker = "libcgj-batch13216"
 *
 * Exclusive continuum CREATE-ONLY (13211-13220: proton soft stubs —
 * ok slots all→0; proton_soft_ready→0; batch_id→13220 —
 * proton_ok_u_13211, wine_ok_u_13212, proton_ge_ok_u_13213,
 * steam_runtime_ok_u_13214, pressure_vessel_ok_u_13215,
 * compat_tool_ok_u_13216, dxvk_ok_u_13217, vkd3d_ok_u_13218,
 * proton_soft_ready_u_13219, batch_id_13220). Unique surface only;
 * no multi-def. Distinct from gj_compat_tool_ok_u_10516 (batch10516)
 * and gj_compat_tool_ok_u_10216 (batch10216). No parent wires. No
 * __int128. No Steam compat-tool implementation.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch13216_marker[] = "libcgj-batch13216";

/* Soft compat-tool-ok lamp: always off (not a real runtime probe). */
#define B13216_COMPAT_TOOL_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b13216_ok(void)
{
	return B13216_COMPAT_TOOL_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_compat_tool_ok_u_13216 - compat-tool ok soft unit flag.
 *
 * Always returns 0. Soft continuum stub; does not probe Steam
 * compatibility-tool presence or call libc. No parent wires.
 */
uint32_t
gj_compat_tool_ok_u_13216(void)
{
	(void)NULL;
	return b13216_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_compat_tool_ok_u_13216(void)
    __attribute__((alias("gj_compat_tool_ok_u_13216")));

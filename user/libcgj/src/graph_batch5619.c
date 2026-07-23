/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5619: VKD3D API version hint integer.
 *
 * Surface (unique symbols):
 *   uint32_t gj_vkd3d_ver_hint_5619(void);
 *     - Returns the soft compile-time VKD3D API version hint
 *       (always 0x0001000d = 1.13 family). Integer product tag only;
 *       does not parse configs or load libraries.
 *   uint32_t __gj_vkd3d_ver_hint_5619  (alias)
 *   __libcgj_batch5619_marker = "libcgj-batch5619"
 *
 * Exclusive continuum CREATE-ONLY (5611-5620: Proton-GE version, wine
 * prefix magic, DXVK/VKD3D readiness integers). Unique
 * gj_vkd3d_ver_hint_5619 surface only; no multi-def. Distinct from
 * gj_vkd3d_ready_5618 and gj_dxvk_ver_hint_5617. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5619_marker[] = "libcgj-batch5619";

/* Soft VKD3D version hint: major=1, minor=13 packed in 16:16. */
#define B5619_VKD3D_VER_HINT  0x0001000du

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5619_ver_hint(void)
{
	return B5619_VKD3D_VER_HINT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_vkd3d_ver_hint_5619 - report VKD3D API version hint integer.
 *
 * Always returns 0x0001000d ((1 << 16) | 13). Soft pure-data only;
 * does not call libc. No parent wires.
 */
uint32_t
gj_vkd3d_ver_hint_5619(void)
{
	(void)NULL;
	return b5619_ver_hint();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_vkd3d_ver_hint_5619(void)
    __attribute__((alias("gj_vkd3d_ver_hint_5619")));

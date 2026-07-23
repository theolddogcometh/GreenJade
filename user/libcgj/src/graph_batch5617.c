/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5617: DXVK API version hint integer.
 *
 * Surface (unique symbols):
 *   uint32_t gj_dxvk_ver_hint_5617(void);
 *     - Returns the soft compile-time DXVK API version hint
 *       (always 0x00020003 = 2.3 family). Integer product tag only;
 *       does not parse dxvk.conf or load libraries.
 *   uint32_t __gj_dxvk_ver_hint_5617  (alias)
 *   __libcgj_batch5617_marker = "libcgj-batch5617"
 *
 * Exclusive continuum CREATE-ONLY (5611-5620: Proton-GE version, wine
 * prefix magic, DXVK/VKD3D readiness integers). Unique
 * gj_dxvk_ver_hint_5617 surface only; no multi-def. Distinct from
 * gj_dxvk_ready_5616. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5617_marker[] = "libcgj-batch5617";

/* Soft DXVK version hint: major=2, minor=3 packed in 16:16. */
#define B5617_DXVK_VER_HINT  0x00020003u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5617_ver_hint(void)
{
	return B5617_DXVK_VER_HINT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_dxvk_ver_hint_5617 - report DXVK API version hint integer.
 *
 * Always returns 0x00020003 ((2 << 16) | 3). Soft pure-data only;
 * does not call libc. No parent wires.
 */
uint32_t
gj_dxvk_ver_hint_5617(void)
{
	(void)NULL;
	return b5617_ver_hint();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_dxvk_ver_hint_5617(void)
    __attribute__((alias("gj_dxvk_ver_hint_5617")));

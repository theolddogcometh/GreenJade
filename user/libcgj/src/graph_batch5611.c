/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5611: Proton-GE major version hint.
 *
 * Surface (unique symbols):
 *   uint32_t gj_proton_ge_major_5611(void);
 *     - Returns the soft compile-time Proton-GE major version hint
 *       (always 9). Integer tag only; does not parse version strings
 *       or probe the filesystem.
 *   uint32_t __gj_proton_ge_major_5611  (alias)
 *   __libcgj_batch5611_marker = "libcgj-batch5611"
 *
 * Exclusive continuum CREATE-ONLY (5611-5620: Proton-GE version, wine
 * prefix magic, DXVK/VKD3D readiness integers). Unique
 * gj_proton_ge_major_5611 surface only; no multi-def. Distinct from
 * gj_proton_ge_ver_ok (batch2366) and gj_proton_major_from_ver
 * (batch1864). No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5611_marker[] = "libcgj-batch5611";

/* Soft Proton-GE major version hint (GE-Proton9-x family). */
#define B5611_PROTON_GE_MAJOR  9u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5611_major(void)
{
	return B5611_PROTON_GE_MAJOR;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_proton_ge_major_5611 - report Proton-GE major version hint.
 *
 * Always returns 9. Soft pure-data product tag; does not parse
 * "GE-Proton*" strings or call libc. No parent wires.
 */
uint32_t
gj_proton_ge_major_5611(void)
{
	(void)NULL;
	return b5611_major();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_proton_ge_major_5611(void)
    __attribute__((alias("gj_proton_ge_major_5611")));

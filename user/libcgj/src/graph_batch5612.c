/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5612: Proton-GE minor / build version hint.
 *
 * Surface (unique symbols):
 *   uint32_t gj_proton_ge_minor_5612(void);
 *     - Returns the soft compile-time Proton-GE minor/build version
 *       hint (always 20). Integer tag only; does not parse version
 *       strings or probe the filesystem.
 *   uint32_t __gj_proton_ge_minor_5612  (alias)
 *   __libcgj_batch5612_marker = "libcgj-batch5612"
 *
 * Exclusive continuum CREATE-ONLY (5611-5620: Proton-GE version, wine
 * prefix magic, DXVK/VKD3D readiness integers). Unique
 * gj_proton_ge_minor_5612 surface only; no multi-def. Distinct from
 * gj_proton_ge_major_5611 and gj_proton_ge_ver_ok (batch2366).
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5612_marker[] = "libcgj-batch5612";

/* Soft Proton-GE minor/build hint (GE-Proton9-20 family). */
#define B5612_PROTON_GE_MINOR  20u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5612_minor(void)
{
	return B5612_PROTON_GE_MINOR;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_proton_ge_minor_5612 - report Proton-GE minor/build version hint.
 *
 * Always returns 20. Soft pure-data product tag; does not parse
 * "GE-Proton*" strings or call libc. No parent wires.
 */
uint32_t
gj_proton_ge_minor_5612(void)
{
	(void)NULL;
	return b5612_minor();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_proton_ge_minor_5612(void)
    __attribute__((alias("gj_proton_ge_minor_5612")));

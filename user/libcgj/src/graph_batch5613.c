/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5613: Proton-GE packed version integer.
 *
 * Surface (unique symbols):
 *   uint32_t gj_proton_ge_ver_pack_5613(void);
 *     - Returns a packed Proton-GE version integer
 *       ((major << 16) | minor) = 0x00090014 for major=9, minor=20.
 *       Soft compile-time product tag; no string parse.
 *   uint32_t __gj_proton_ge_ver_pack_5613  (alias)
 *   __libcgj_batch5613_marker = "libcgj-batch5613"
 *
 * Exclusive continuum CREATE-ONLY (5611-5620: Proton-GE version, wine
 * prefix magic, DXVK/VKD3D readiness integers). Unique
 * gj_proton_ge_ver_pack_5613 surface only; no multi-def. Distinct from
 * gj_proton_ge_major_5611 / gj_proton_ge_minor_5612. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5613_marker[] = "libcgj-batch5613";

/* Packed: major in high 16 bits, minor in low 16 bits (9.20). */
#define B5613_PROTON_GE_VER_PACK  0x00090014u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5613_ver_pack(void)
{
	return B5613_PROTON_GE_VER_PACK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_proton_ge_ver_pack_5613 - report packed Proton-GE version integer.
 *
 * Always returns 0x00090014 ((9 << 16) | 20). Soft pure-data only;
 * does not call libc. No parent wires.
 */
uint32_t
gj_proton_ge_ver_pack_5613(void)
{
	(void)NULL;
	return b5613_ver_pack();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_proton_ge_ver_pack_5613(void)
    __attribute__((alias("gj_proton_ge_ver_pack_5613")));

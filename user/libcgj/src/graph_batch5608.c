/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5608: mesa version pack (pack / fields).
 *
 * Surface (unique symbols):
 *   uint32_t gj_mesa_version_pack_5608(uint32_t major, uint32_t minor,
 *                                      uint32_t patch);
 *     - Pack (major<<16)|(minor<<8)|patch with each field masked to 8 bits.
 *   uint32_t gj_mesa_version_major_5608(uint32_t packed);
 *     - Extract major ((packed >> 16) & 0xFF).
 *   uint32_t gj_mesa_version_minor_5608(uint32_t packed);
 *     - Extract minor ((packed >> 8) & 0xFF).
 *   uint32_t gj_mesa_version_patch_5608(uint32_t packed);
 *     - Extract patch (packed & 0xFF).
 *   uint32_t __gj_mesa_version_pack_5608  (alias)
 *   uint32_t __gj_mesa_version_major_5608  (alias)
 *   uint32_t __gj_mesa_version_minor_5608  (alias)
 *   uint32_t __gj_mesa_version_patch_5608  (alias)
 *   __libcgj_batch5608_marker = "libcgj-batch5608"
 *
 * Exclusive continuum CREATE-ONLY (5601-5610). Mesa version pack for
 * Deck product path; no dlopen of libGL/Mesa. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5608_marker[] = "libcgj-batch5608";

#define B5608_FIELD_MASK  0xFFu

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5608_pack(uint32_t u32Major, uint32_t u32Minor, uint32_t u32Patch)
{
	uint32_t u32M = u32Major & B5608_FIELD_MASK;
	uint32_t u32N = u32Minor & B5608_FIELD_MASK;
	uint32_t u32P = u32Patch & B5608_FIELD_MASK;

	return (u32M << 16) | (u32N << 8) | u32P;
}

static uint32_t
b5608_major(uint32_t u32Packed)
{
	return (u32Packed >> 16) & B5608_FIELD_MASK;
}

static uint32_t
b5608_minor(uint32_t u32Packed)
{
	return (u32Packed >> 8) & B5608_FIELD_MASK;
}

static uint32_t
b5608_patch(uint32_t u32Packed)
{
	return u32Packed & B5608_FIELD_MASK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_mesa_version_pack_5608 - soft pack major.minor.patch into uint32_t.
 *
 * major/minor/patch: each masked to 8 bits
 *
 * Returns (major<<16)|(minor<<8)|patch.
 */
uint32_t
gj_mesa_version_pack_5608(uint32_t u32Major, uint32_t u32Minor, uint32_t u32Patch)
{
	(void)NULL;
	return b5608_pack(u32Major, u32Minor, u32Patch);
}

/*
 * gj_mesa_version_major_5608 - extract major from packed mesa version.
 *
 * packed: value from gj_mesa_version_pack_5608
 *
 * Returns (packed >> 16) & 0xFF.
 */
uint32_t
gj_mesa_version_major_5608(uint32_t u32Packed)
{
	return b5608_major(u32Packed);
}

/*
 * gj_mesa_version_minor_5608 - extract minor from packed mesa version.
 *
 * Returns (packed >> 8) & 0xFF.
 */
uint32_t
gj_mesa_version_minor_5608(uint32_t u32Packed)
{
	return b5608_minor(u32Packed);
}

/*
 * gj_mesa_version_patch_5608 - extract patch from packed mesa version.
 *
 * Returns packed & 0xFF.
 */
uint32_t
gj_mesa_version_patch_5608(uint32_t u32Packed)
{
	return b5608_patch(u32Packed);
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_mesa_version_pack_5608(uint32_t u32Major, uint32_t u32Minor,
    uint32_t u32Patch)
    __attribute__((alias("gj_mesa_version_pack_5608")));

uint32_t __gj_mesa_version_major_5608(uint32_t u32Packed)
    __attribute__((alias("gj_mesa_version_major_5608")));

uint32_t __gj_mesa_version_minor_5608(uint32_t u32Packed)
    __attribute__((alias("gj_mesa_version_minor_5608")));

uint32_t __gj_mesa_version_patch_5608(uint32_t u32Packed)
    __attribute__((alias("gj_mesa_version_patch_5608")));

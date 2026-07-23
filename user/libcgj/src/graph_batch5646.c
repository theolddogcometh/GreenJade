/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5646: installer copy progress (0-100 MiB).
 *
 * Surface (unique symbols):
 *   uint32_t gj_install_copy_progress_5646(uint32_t done_mib, uint32_t total_mib);
 *     - Return percent complete (0..100) for payload copy by MiB.
 *       total_mib==0 → 0; done_mib>=total_mib → 100; else ratio.
 *   uint32_t gj_install_copy_bytes_to_mib_5646(uint32_t bytes_hi,
 *                                             uint32_t bytes_lo);
 *     - Soft floor divide of a 64-bit byte count (hi:lo) by 1048576.
 *       Overflow-safe; returns uint32_t MiB (saturates at UINT32_MAX).
 *   uint32_t __gj_install_copy_progress_5646  (alias)
 *   uint32_t __gj_install_copy_bytes_to_mib_5646  (alias)
 *   __libcgj_batch5646_marker = "libcgj-batch5646"
 *
 * Exclusive continuum CREATE-ONLY (5641-5650: installer stages —
 * partition/format/copy/bootloader/config progress integers). Unique
 * surface only; no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5646_marker[] = "libcgj-batch5646";

/* 1 MiB = 2^20 bytes. */
#define B5646_MIB_SHIFT  20u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5646_clamp_pct(uint32_t u32Pct)
{
	if (u32Pct > 100u) {
		return 100u;
	}
	return u32Pct;
}

static uint32_t
b5646_progress(uint32_t u32DoneMib, uint32_t u32TotalMib)
{
	uint32_t u32Pct;

	if (u32TotalMib == 0u) {
		return 0u;
	}
	if (u32DoneMib >= u32TotalMib) {
		return 100u;
	}
	if (u32DoneMib > (UINT32_MAX / 100u)) {
		u32Pct = u32DoneMib / (u32TotalMib / 100u);
	} else {
		u32Pct = (u32DoneMib * 100u) / u32TotalMib;
	}
	return b5646_clamp_pct(u32Pct);
}

/*
 * Soft MiB from 64-bit byte count without __int128:
 *   mib = (hi << 12) | (lo >> 20)   when hi < 2^20
 *   else saturate.
 */
static uint32_t
b5646_bytes_to_mib(uint32_t u32Hi, uint32_t u32Lo)
{
	uint32_t u32FromHi;
	uint32_t u32FromLo;

	/* If hi >= 2^20, result would be >= 2^32 MiB → saturate. */
	if (u32Hi >= (1u << B5646_MIB_SHIFT)) {
		return UINT32_MAX;
	}
	u32FromHi = u32Hi << (32u - B5646_MIB_SHIFT); /* hi * 2^12 */
	u32FromLo = u32Lo >> B5646_MIB_SHIFT;
	/* No carry between parts beyond UINT32_MAX: fromHi uses top bits. */
	if (u32FromHi > (UINT32_MAX - u32FromLo)) {
		return UINT32_MAX;
	}
	return u32FromHi + u32FromLo;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_install_copy_progress_5646 - copy stage percent complete by MiB.
 *
 * done_mib:  mebibytes copied so far
 * total_mib: total mebibytes in payload (0 → 0)
 *
 * Returns 0..100. Pure integer; no I/O. No parent wires.
 */
uint32_t
gj_install_copy_progress_5646(uint32_t u32DoneMib, uint32_t u32TotalMib)
{
	(void)NULL;
	return b5646_progress(u32DoneMib, u32TotalMib);
}

/*
 * gj_install_copy_bytes_to_mib_5646 - soft 64-bit bytes → MiB floor.
 *
 * bytes_hi: high 32 bits of byte count
 * bytes_lo: low 32 bits of byte count
 *
 * Returns floor(bytes / 1048576), saturating at UINT32_MAX.
 * No __int128; pure shift arithmetic.
 */
uint32_t
gj_install_copy_bytes_to_mib_5646(uint32_t u32BytesHi, uint32_t u32BytesLo)
{
	return b5646_bytes_to_mib(u32BytesHi, u32BytesLo);
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_install_copy_progress_5646(uint32_t u32DoneMib, uint32_t u32TotalMib)
    __attribute__((alias("gj_install_copy_progress_5646")));

uint32_t __gj_install_copy_bytes_to_mib_5646(uint32_t u32BytesHi, uint32_t u32BytesLo)
    __attribute__((alias("gj_install_copy_bytes_to_mib_5646")));

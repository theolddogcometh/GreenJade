/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5636: PCR index ok for measured boot bank.
 *
 * Surface (unique symbols):
 *   uint32_t gj_pcr_index_ok_5636(uint32_t idx);
 *     - Return 1 when idx is in [0, 23] (classic PCR0..PCR23 range for
 *       SHA-256 bank on TPM 2.0 platforms), else 0.
 *   uint32_t __gj_pcr_index_ok_5636  (alias)
 *   __libcgj_batch5636_marker = "libcgj-batch5636"
 *
 * Exclusive continuum CREATE-ONLY (5631-5640: secure boot / TPM /
 * measured boot stubs for install path). Unique
 * gj_pcr_index_ok_5636 surface only; no multi-def. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5636_marker[] = "libcgj-batch5636";

/* Classic platform PCR count (indices 0..23). */
#define B5636_PCR_MAX  23u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5636_pcr_index_ok(uint32_t u32Idx)
{
	return (u32Idx <= B5636_PCR_MAX) ? 1u : 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_pcr_index_ok_5636 - true when PCR index is in the platform bank.
 *
 * idx: PCR index (0..23 accepted)
 *
 * Returns 1 for valid index, else 0. Does not touch TPM hardware.
 * No parent wires.
 */
uint32_t
gj_pcr_index_ok_5636(uint32_t u32Idx)
{
	(void)NULL;
	return b5636_pcr_index_ok(u32Idx);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_pcr_index_ok_5636(uint32_t u32Idx)
    __attribute__((alias("gj_pcr_index_ok_5636")));

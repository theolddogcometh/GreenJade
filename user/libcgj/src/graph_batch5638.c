/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5638: soft measured-boot PCR extend mix.
 *
 * Surface (unique symbols):
 *   uint32_t gj_measured_boot_extend_5638(uint32_t pcr_old,
 *                                         uint32_t event_digest);
 *     - Soft PCR-extend style mix: rotate-left-5 of pcr_old xor
 *       event_digest, then multiply-mix. Integer-only stand-in for
 *       hash(pcr || event) used by install path unit tests.
 *   uint32_t __gj_measured_boot_extend_5638  (alias)
 *   __libcgj_batch5638_marker = "libcgj-batch5638"
 *
 * Exclusive continuum CREATE-ONLY (5631-5640: secure boot / TPM /
 * measured boot stubs for install path). Unique
 * gj_measured_boot_extend_5638 surface only; no multi-def. Not a
 * cryptographic SHA-256; soft mix for pure-C freestanding gates.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5638_marker[] = "libcgj-batch5638";

/* Odd mix constant (public-domain style). */
#define B5638_MIX  0x9e3779b9u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5638_rotl5(uint32_t u32X)
{
	return (u32X << 5) | (u32X >> 27);
}

/*
 * Soft extend: new = mix(rotl5(old) ^ event). Not SHA-256; deterministic
 * 32-bit stand-in safe under freestanding integer-only rules.
 */
static uint32_t
b5638_extend(uint32_t u32PcrOld, uint32_t u32Event)
{
	uint32_t u32T;

	u32T = b5638_rotl5(u32PcrOld) ^ u32Event;
	u32T = (u32T * B5638_MIX) + u32Event;
	return u32T;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_measured_boot_extend_5638 - soft PCR extend of old value with event.
 *
 * pcr_old:       previous soft PCR digest word
 * event_digest:  event measurement word (caller-provided)
 *
 * Returns mixed 32-bit value. Pure integer; no TPM commands.
 * No parent wires.
 */
uint32_t
gj_measured_boot_extend_5638(uint32_t u32PcrOld, uint32_t u32EventDigest)
{
	(void)NULL;
	return b5638_extend(u32PcrOld, u32EventDigest);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_measured_boot_extend_5638(uint32_t u32PcrOld,
    uint32_t u32EventDigest)
    __attribute__((alias("gj_measured_boot_extend_5638")));

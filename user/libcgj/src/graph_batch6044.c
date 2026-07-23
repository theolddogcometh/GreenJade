/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6044: Deck APU package/socket count stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_deck_apu_packages_6044(void);
 *     - Returns soft CPU package (socket) count for the Steam Deck
 *       Van Gogh APU topology stub (always 1). Not a runtime probe.
 *   uint32_t __gj_deck_apu_packages_6044  (alias)
 *   __libcgj_batch6044_marker = "libcgj-batch6044"
 *
 * Exclusive continuum CREATE-ONLY (6041-6050: CPU core/thread topology
 * stubs for Deck APU). Unique gj_deck_apu_packages_6044 surface only;
 * no multi-def. Distinct from core/thread stubs 6041-6043. No parent
 * wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6044_marker[] = "libcgj-batch6044";

/* Steam Deck Van Gogh: single package SoC. */
#define B6044_PACKAGES  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6044_packages(void)
{
	return B6044_PACKAGES;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_deck_apu_packages_6044 - soft Deck APU package/socket count.
 *
 * Always returns 1 (single Van Gogh SoC package). Soft pure-data
 * constant; does not probe hardware. No parent wires.
 */
uint32_t
gj_deck_apu_packages_6044(void)
{
	(void)NULL;
	return b6044_packages();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_deck_apu_packages_6044(void)
    __attribute__((alias("gj_deck_apu_packages_6044")));

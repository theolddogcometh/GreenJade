/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6045: Deck APU CCX count topology stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_deck_apu_ccx_count_6045(void);
 *     - Returns soft CCX (core complex) count for the Steam Deck
 *       Van Gogh APU topology stub (always 1 CCX holding 4 cores).
 *       Not a runtime probe.
 *   uint32_t __gj_deck_apu_ccx_count_6045  (alias)
 *   __libcgj_batch6045_marker = "libcgj-batch6045"
 *
 * Exclusive continuum CREATE-ONLY (6041-6050: CPU core/thread topology
 * stubs for Deck APU). Unique gj_deck_apu_ccx_count_6045 surface only;
 * no multi-def. Distinct from package stub 6044. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6045_marker[] = "libcgj-batch6045";

/* Steam Deck Van Gogh: single CCX with four Zen 2 cores. */
#define B6045_CCX_COUNT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6045_ccx_count(void)
{
	return B6045_CCX_COUNT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_deck_apu_ccx_count_6045 - soft Deck APU CCX count.
 *
 * Always returns 1 (Van Gogh single core complex). Soft pure-data
 * constant; does not probe hardware. No parent wires.
 */
uint32_t
gj_deck_apu_ccx_count_6045(void)
{
	(void)NULL;
	return b6045_ccx_count();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_deck_apu_ccx_count_6045(void)
    __attribute__((alias("gj_deck_apu_ccx_count_6045")));

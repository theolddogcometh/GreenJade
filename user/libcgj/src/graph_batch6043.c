/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6043: Deck APU SMT threads-per-core stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_deck_apu_smt_per_core_6043(void);
 *     - Returns soft SMT (hardware threads per physical core) for the
 *       Steam Deck Van Gogh APU topology stub (always 2). Not a runtime
 *       topology probe.
 *   uint32_t __gj_deck_apu_smt_per_core_6043  (alias)
 *   __libcgj_batch6043_marker = "libcgj-batch6043"
 *
 * Exclusive continuum CREATE-ONLY (6041-6050: CPU core/thread topology
 * stubs for Deck APU). Unique gj_deck_apu_smt_per_core_6043 surface
 * only; no multi-def. Distinct from logical/phys counts 6041/6042.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6043_marker[] = "libcgj-batch6043";

/* Steam Deck Van Gogh: 2 SMT threads per physical core. */
#define B6043_SMT_PER_CORE  2u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6043_smt_per_core(void)
{
	return B6043_SMT_PER_CORE;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_deck_apu_smt_per_core_6043 - soft Deck APU SMT threads per core.
 *
 * Always returns 2 (Zen 2 SMT enabled on Van Gogh). Soft pure-data
 * constant; does not probe hardware. No parent wires.
 */
uint32_t
gj_deck_apu_smt_per_core_6043(void)
{
	(void)NULL;
	return b6043_smt_per_core();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_deck_apu_smt_per_core_6043(void)
    __attribute__((alias("gj_deck_apu_smt_per_core_6043")));

/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3347: cooperative spin-loop pause hint (u).
 *
 * Surface (unique symbols):
 *   uint32_t gj_spin_hint_u(uint32_t spins);
 *     - Soft busy-wait hint: touch a volatile counter spins times so
 *       the call is a real side effect in a tight wait loop. Returns
 *       the number of spin iterations performed (equal to spins).
 *       spins == 0 → immediate return 0. Portable freestanding: no
 *       architecture PAUSE insn, no libc.
 *   uint32_t __gj_spin_hint_u  (alias)
 *   __libcgj_batch3347_marker = "libcgj-batch3347"
 *
 * Milestone 3350 exclusive continuum CREATE-ONLY (3341-3350). Distinct
 * from gj_cpu_pause_hint (batch981 void single touch) and
 * gj_spinlock_hint_* (batch1551-1553) — unique gj_spin_hint_u surface
 * only; no multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3347_marker[] = "libcgj-batch3347";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b3347_spin(uint32_t uSpins)
{
	volatile uint32_t uTmp;
	uint32_t uI;

	uTmp = 0u;
	for (uI = 0u; uI < uSpins; uI++) {
		uTmp = uI;
	}
	(void)uTmp;
	return uSpins;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_spin_hint_u - cooperative spin-loop yield hint for spins iterations.
 *
 * spins: number of volatile touches to perform (0 allowed).
 *
 * Returns spins. Not a full memory barrier. No parent wires.
 */
uint32_t
gj_spin_hint_u(uint32_t uSpins)
{
	(void)NULL;
	return b3347_spin(uSpins);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_spin_hint_u(uint32_t uSpins)
    __attribute__((alias("gj_spin_hint_u")));

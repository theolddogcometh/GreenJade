/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3974: arena address containment probe (_u).
 *
 * Surface (unique symbols):
 *   uint32_t gj_arena_contains_u(uintptr_t base, size_t cap, uintptr_t ptr);
 *     - Return 1 if ptr lies in the half-open span [base, base+cap),
 *       else 0. Overflow-safe: if base + cap wraps past UINTPTR_MAX,
 *       only addresses from base through UINTPTR_MAX are considered
 *       inside when cap is large enough to reach the end of the
 *       address space from base (i.e. cap >= UINTPTR_MAX - base + 1
 *       is treated as "to end of address space").
 *   uint32_t __gj_arena_contains_u  (alias)
 *   __libcgj_batch3974_marker = "libcgj-batch3974"
 *
 * Exclusive continuum CREATE-ONLY (3971-3980). Unique
 * gj_arena_contains_u surface only; no multi-def. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3974_marker[] = "libcgj-batch3974";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b3974_contains(uintptr_t uBase, size_t cbCap, uintptr_t uPtr)
{
	uintptr_t uRoom;
	uintptr_t uEnd;

	if (cbCap == 0u) {
		return 0u;
	}
	if (uPtr < uBase) {
		return 0u;
	}
	/* Room from base to end of address space (inclusive count). */
	uRoom = (uintptr_t)UINTPTR_MAX - uBase;
	/* +1 would be bytes available including base itself as a unit;
	 * for half-open [base, base+cap) we need uPtr - uBase < cap. */
	if ((uintptr_t)cbCap > uRoom) {
		/* Span runs to end of address space: any ptr >= base is in. */
		return 1u;
	}
	uEnd = uBase + (uintptr_t)cbCap;
	return (uPtr < uEnd) ? 1u : 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_arena_contains_u - test whether ptr is inside [base, base+cap).
 *
 * base: arena base address as integer
 * cap:  byte length of the arena span
 * ptr:  candidate address as integer
 *
 * Returns 1 if contained, 0 otherwise.
 * No parent wires.
 */
uint32_t
gj_arena_contains_u(uintptr_t uBase, size_t cbCap, uintptr_t uPtr)
{
	(void)NULL;
	return b3974_contains(uBase, cbCap, uPtr);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_arena_contains_u(uintptr_t uBase, size_t cbCap, uintptr_t uPtr)
    __attribute__((alias("gj_arena_contains_u")));

/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch329: hazard-pointer publish / clear / scan for
 * a single-thread (or test-harness) simulation of HP-based deferred reclaim.
 *
 * Surface (unique symbols):
 *   void gj_hp_publish(void **slot, void *p);
 *     — Store p into *slot (the caller's hazard-pointer slot) with a
 *       volatile write, then a compiler memory barrier so subsequent
 *       loads of the protected object cannot be reordered before the
 *       publish is a side effect. NULL slot is a no-op.
 *   void gj_hp_clear(void **slot);
 *     — Clear *slot to NULL (same volatile + barrier protocol). NULL
 *       slot is a no-op. Use after the reader no longer needs the object.
 *   int gj_hp_protected(void *const *slots, size_t n, void *p);
 *     — Scan slots[0..n) and return 1 if any entry equals p (object is
 *       currently hazard-protected); return 0 otherwise. NULL slots with
 *       n == 0 is not protected (0). NULL slots with n > 0 is 0 (nothing
 *       to scan). A NULL p matches only slots that hold NULL.
 *   __gj_hp_publish / __gj_hp_clear / __gj_hp_protected  (aliases)
 *   __libcgj_batch329_marker = "libcgj-batch329"
 *
 * Model (single-thread sim of Michael's hazard pointers):
 *   Reader:  gj_hp_publish(&hp[i], pObj);  use *pObj;  gj_hp_clear(&hp[i]);
 *   Reclaim: if (!gj_hp_protected(hp, cSlots, pRetired)) free(pRetired);
 * Full multi-thread HP needs hardware fences and retire lists; this TU is
 * the freestanding integer/pointer publish surface for harnesses and
 * single-threaded ownership transfer tests only.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno.
 * No third-party source copied. Not a CPU fence (no mfence/sfence).
 */

#include <stddef.h>

const char __libcgj_batch329_marker[] = "libcgj-batch329";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_hp_publish — announce that p is currently held by this reader slot.
 *
 * The volatile store forces a real write; the empty asm with a "memory"
 * clobber is a compiler barrier only (same pattern as batch295 wipe).
 */
void
gj_hp_publish(void **slot, void *p)
{
	void *volatile *ppV;

	if (slot == NULL) {
		return;
	}

	/* Store of the pointer value is volatile (not *to* volatile void). */
	ppV = (void *volatile *)slot;
	*ppV = p;
	__asm__ __volatile__("" ::: "memory");
}

/*
 * gj_hp_clear — drop protection on the slot (store NULL).
 */
void
gj_hp_clear(void **slot)
{
	void *volatile *ppV;

	if (slot == NULL) {
		return;
	}

	ppV = (void *volatile *)slot;
	*ppV = NULL;
	__asm__ __volatile__("" ::: "memory");
}

/*
 * gj_hp_protected — 1 if any of the n hazard slots equals p.
 *
 * Reclaimers call this before freeing a retired pointer. Equality is
 * pointer identity only (no deep compare). Empty scan (n == 0) or a
 * NULL slots base returns 0.
 */
int
gj_hp_protected(void *const *slots, size_t n, void *p)
{
	size_t iSlot;

	if (slots == NULL || n == 0u) {
		return 0;
	}

	for (iSlot = 0u; iSlot < n; iSlot++) {
		if (slots[iSlot] == p) {
			return 1;
		}
	}
	return 0;
}

void __gj_hp_publish(void **slot, void *p)
    __attribute__((alias("gj_hp_publish")));
void __gj_hp_clear(void **slot)
    __attribute__((alias("gj_hp_clear")));
int __gj_hp_protected(void *const *slots, size_t n, void *p)
    __attribute__((alias("gj_hp_protected")));

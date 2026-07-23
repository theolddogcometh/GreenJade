/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch177: simple fixed-slot string intern table.
 * Clean-room freestanding pure C — integer/pointer only (no SSE required;
 * builds under userspace -msse2). No third-party source was copied.
 *
 * Surface (unique symbols):
 *   typedef struct { const char *s; size_t n; } gj_intern_slot_t;
 *   const char *gj_intern(gj_intern_slot_t *tab, size_t slots,
 *                        const char *s, size_t n);
 *     — Linear-scan intern over a caller-owned fixed slot table.
 *       Empty slots have s == NULL. On hit (equal length + content),
 *       returns the previously stored pointer. On miss, claims the first
 *       empty slot (stores s/n) and returns s. If the table is full and
 *       no match is found, returns NULL. NULL tab or s → NULL.
 *       Content is not copied; the caller must keep s live for the
 *       lifetime of the slot.
 *   __gj_intern  (alias)
 *   __libcgj_batch177_marker = "libcgj-batch177"
 */

#include <stddef.h>

const char __libcgj_batch177_marker[] = "libcgj-batch177";

/* Fixed-slot intern entry: pointer + byte length (not necessarily NUL-term). */
typedef struct {
	const char *s;
	size_t n;
} gj_intern_slot_t;

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * Byte-wise equality of two buffers of length cb. Returns 1 if equal, 0 else.
 */
static int
b177_memeq(const char *pA, const char *pB, size_t cb)
{
	size_t i;

	for (i = 0; i < cb; i++) {
		if ((unsigned char)pA[i] != (unsigned char)pB[i]) {
			return 0;
		}
	}
	return 1;
}

/* ---- intern ------------------------------------------------------------ */

/*
 * gj_intern — look up or insert (s, n) in a fixed slot table.
 *
 * Linear scan: first matching occupied slot wins; otherwise first empty
 * slot is claimed. No allocation, no hash — suitable for small tables
 * of long-lived string literals / stable buffers.
 */
const char *
gj_intern(gj_intern_slot_t *pTab, size_t cSlots, const char *s, size_t n)
{
	size_t i;
	size_t iEmpty;

	if (pTab == NULL || s == NULL || cSlots == 0) {
		return NULL;
	}

	iEmpty = cSlots; /* sentinel: no empty yet */
	for (i = 0; i < cSlots; i++) {
		if (pTab[i].s == NULL) {
			if (iEmpty == cSlots) {
				iEmpty = i;
			}
			continue;
		}
		if (pTab[i].n == n && b177_memeq(pTab[i].s, s, n)) {
			return pTab[i].s;
		}
	}

	if (iEmpty == cSlots) {
		/* Table full, no match. */
		return NULL;
	}

	pTab[iEmpty].s = s;
	pTab[iEmpty].n = n;
	return s;
}

const char *__gj_intern(gj_intern_slot_t *pTab, size_t cSlots, const char *s,
                        size_t n) __attribute__((alias("gj_intern")));

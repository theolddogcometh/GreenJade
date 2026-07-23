/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch247: merge two sorted ascending uint32 arrays.
 *
 * Surface (unique symbols):
 *   size_t gj_merge_u32(const uint32_t *a, size_t na,
 *                       const uint32_t *b, size_t nb,
 *                       uint32_t *out, size_t out_cap);
 *     — merge a[0..na) and b[0..nb) (each sorted ascending) into out.
 *       Caller supplies out capacity. Requires out_cap >= na+nb; on
 *       short capacity returns 0 and writes nothing. Returns the number
 *       of elements written (na+nb) on success.
 *       NULL a with na>0 → 0; NULL b with nb>0 → 0; empty sides ok
 *       (na==0 and/or nb==0). NULL out with need>0 → 0. Overflow of
 *       na+nb → 0. Stable for equal keys: elements from a precede b.
 *   __gj_merge_u32  (alias)
 *   __libcgj_batch247_marker = "libcgj-batch247"
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2. No malloc, no errno, no libc string helpers.
 * No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch247_marker[] = "libcgj-batch247";

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * b247_copy_u32 — copy n uint32_t words from src to dst.
 * src/dst must be non-NULL when n > 0. Handles n == 0 as a no-op.
 */
static void
b247_copy_u32(uint32_t *dst, const uint32_t *src, size_t n)
{
	size_t i;

	for (i = 0u; i < n; i++) {
		dst[i] = src[i];
	}
}

/*
 * b247_merge_u32 — two-way merge of sorted ascending runs into out.
 * a[0..na) and b[0..nb) are sorted non-decreasing. out has room for
 * na+nb elements. Stable: when a[i] == b[j], take from a first.
 * Returns na+nb.
 */
static size_t
b247_merge_u32(const uint32_t *a, size_t na, const uint32_t *b, size_t nb,
               uint32_t *out)
{
	size_t i = 0u;
	size_t j = 0u;
	size_t k = 0u;

	while (i < na && j < nb) {
		if (a[i] <= b[j]) {
			out[k++] = a[i++];
		} else {
			out[k++] = b[j++];
		}
	}
	if (i < na) {
		b247_copy_u32(out + k, a + i, na - i);
		k += na - i;
	}
	if (j < nb) {
		b247_copy_u32(out + k, b + j, nb - j);
		k += nb - j;
	}
	return k;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_merge_u32 — merge two sorted ascending uint32 arrays.
 *
 * Writes na+nb elements into out when out_cap is sufficient; otherwise
 * returns 0 without writing. Empty inputs (na==0 / nb==0) are allowed
 * and simply copy the non-empty side (or write nothing if both empty).
 *
 * Returns:
 *   na+nb  — success (elements written)
 *   0      — capacity shortfall, NULL input with positive length,
 *            NULL out when need > 0, or size overflow of na+nb
 */
size_t
gj_merge_u32(const uint32_t *a, size_t na, const uint32_t *b, size_t nb,
             uint32_t *out, size_t out_cap)
{
	size_t need;

	/* Reject size overflow so out_cap compare stays well-defined. */
	if (nb > (SIZE_MAX - na)) {
		return 0u;
	}
	need = na + nb;

	if (out_cap < need) {
		return 0u;
	}
	if (need == 0u) {
		return 0u;
	}
	if (out == NULL) {
		return 0u;
	}
	if (a == NULL && na > 0u) {
		return 0u;
	}
	if (b == NULL && nb > 0u) {
		return 0u;
	}

	/* One side empty: copy the other (a/b may be NULL when length 0). */
	if (na == 0u) {
		b247_copy_u32(out, b, nb);
		return nb;
	}
	if (nb == 0u) {
		b247_copy_u32(out, a, na);
		return na;
	}

	return b247_merge_u32(a, na, b, nb, out);
}

size_t __gj_merge_u32(const uint32_t *a, size_t na, const uint32_t *b,
                      size_t nb, uint32_t *out, size_t out_cap)
    __attribute__((alias("gj_merge_u32")));

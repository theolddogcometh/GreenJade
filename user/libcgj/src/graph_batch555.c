/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch555: length-based Levenshtein over u8 buffers
 * with caller-supplied work storage.
 *
 * Surface (unique symbols — does NOT redefine batch170 gj_levenshtein /
 * __gj_levenshtein or batch236 gj_edit_distance / __gj_edit_distance):
 *   size_t gj_levenshtein_u8(const unsigned char *a, size_t na,
 *                            const unsigned char *b, size_t nb,
 *                            size_t *work, size_t work_n);
 *     — classic Wagner–Fischer edit distance (insert / delete / substitute
 *       cost 1 each; match cost 0) over raw length-na / length-nb byte
 *       sequences (not NUL-terminated).
 *     Single-row DP in caller storage: work must hold at least (nb + 1)
 *       size_t entries (min work_n == nb + 1). On success work[0..nb] is
 *       left as the final DP row; work[nb] equals the return value.
 *     Returns the edit distance on success.
 *     Returns (size_t)-1 if work is NULL, if work_n < nb + 1 (or nb wraps
 *       SIZE_MAX), or if a is NULL with na > 0 / b is NULL with nb > 0.
 *     Empty buffers are valid: distance equals the other length (0 if both
 *       empty). NULL a with na == 0 (and/or NULL b with nb == 0) is ok.
 *   size_t __gj_levenshtein_u8  (alias)
 *   __libcgj_batch555_marker = "libcgj-batch555"
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc string calls. No third-party source copied.
 */

#include <stddef.h>

const char __libcgj_batch555_marker[] = "libcgj-batch555";

/* ---- freestanding helpers ---------------------------------------------- */

static size_t
b555_min3(size_t nA, size_t nB, size_t nC)
{
	size_t nM = nA;

	if (nB < nM) {
		nM = nB;
	}
	if (nC < nM) {
		nM = nC;
	}
	return nM;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_levenshtein_u8 — single-row Levenshtein distance for u8 byte buffers.
 *
 * a, na:  first buffer and length (a may be NULL only if na == 0)
 * b, nb:  second buffer and length (b may be NULL only if nb == 0)
 * work:   caller-owned array of size_t; must be non-NULL
 * work_n: number of size_t slots in work; must be >= nb + 1
 *
 * Space-optimized Wagner–Fischer: one rolling row of (nb + 1) cells.
 * work[j] holds distance for the current a-prefix vs b[0..j). After
 * each a-byte, the previous diagonal is kept in a local so the row can
 * be updated in place.
 *
 * Returns distance, or (size_t)-1 on invalid args / insufficient work.
 */
size_t
gj_levenshtein_u8(const unsigned char *a, size_t na,
		  const unsigned char *b, size_t nb,
		  size_t *work, size_t work_n)
{
	size_t i;
	size_t j;
	size_t prev_diag;
	size_t saved;

	if ((a == NULL && na > 0u) || (b == NULL && nb > 0u)) {
		return (size_t)-1;
	}
	if (work == NULL) {
		return (size_t)-1;
	}
	/* Need (nb + 1) slots; reject nb == SIZE_MAX so nb+1 cannot wrap. */
	if (nb == (size_t)-1 || work_n < nb + 1u) {
		return (size_t)-1;
	}

	/* Row 0: distance from empty a-prefix to b[0..j). */
	for (j = 0u; j <= nb; j++) {
		work[j] = j;
	}

	for (i = 1u; i <= na; i++) {
		unsigned char chA = a[i - 1u];

		prev_diag = work[0];
		work[0] = i; /* delete all of a[0..i) */
		for (j = 1u; j <= nb; j++) {
			unsigned char chB = b[j - 1u];
			size_t nCost = (chA == chB) ? (size_t)0 : (size_t)1;
			size_t nDel = work[j] + 1u;       /* delete a[i-1] */
			size_t nIns = work[j - 1u] + 1u;  /* insert b[j-1] */
			size_t nSub = prev_diag + nCost;  /* substitute/match */

			saved = work[j];
			work[j] = b555_min3(nDel, nIns, nSub);
			prev_diag = saved;
		}
	}

	return work[nb];
}

size_t __gj_levenshtein_u8(const unsigned char *a, size_t na,
			   const unsigned char *b, size_t nb,
			   size_t *work, size_t work_n)
    __attribute__((alias("gj_levenshtein_u8")));

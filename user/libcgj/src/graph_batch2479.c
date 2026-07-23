/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2479: freestanding in-place sep normalize.
 *
 * Surface (unique symbols):
 *   size_t gj_path_normalize_sep_u(char *path, size_t cap);
 *     — Rewrite '\\' to '/' in place within the first cap bytes of path
 *       (bounded). Stops at the first NUL or when cap bytes have been
 *       examined, whichever comes first. Does not write past cap and
 *       does not write a NUL (existing terminator is left alone when
 *       encountered inside the bound). Returns the number of backslash
 *       bytes rewritten. NULL path or cap == 0 → 0.
 *   size_t __gj_path_normalize_sep_u  (alias)
 *   __libcgj_batch2479_marker = "libcgj-batch2479"
 *
 * Path exclusive wave (2471–2480). Distinct from gj_path_normalize
 * (batch263 full . / .. collapse into a separate out buffer) — this is
 * separator rewrite only, in place, capacity-bounded.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>

const char __libcgj_batch2479_marker[] = "libcgj-batch2479";

/* ---- freestanding helpers ---------------------------------------------- */

static size_t
b2479_norm_sep(char *szPath, size_t cbCap)
{
	size_t i;
	size_t cRewritten;

	if (szPath == NULL || cbCap == 0u) {
		return 0u;
	}

	cRewritten = 0u;
	for (i = 0u; i < cbCap; i++) {
		if (szPath[i] == '\0') {
			break;
		}
		if (szPath[i] == '\\') {
			szPath[i] = '/';
			cRewritten++;
		}
	}
	return cRewritten;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_path_normalize_sep_u — rewrite '\\' → '/' in place, bounded by cap.
 *
 * path: mutable buffer (may be NULL)
 * cap:  maximum number of bytes to examine (not necessarily including
 *       a trailing NUL; scan stops at NUL or cap)
 *
 * Returns count of bytes rewritten from '\\' to '/'.
 *
 * Examples (cap large enough for full string):
 *   "a\\b\\c"  → "a/b/c", return 2
 *   "a/b/c"    → "a/b/c", return 0
 *   ""         → "", return 0
 *   NULL       → return 0
 */
size_t
gj_path_normalize_sep_u(char *szPath, size_t cbCap)
{
	(void)NULL;
	return b2479_norm_sep(szPath, cbCap);
}

/* ---- underscored alias ------------------------------------------------- */

size_t __gj_path_normalize_sep_u(char *szPath, size_t cbCap)
    __attribute__((alias("gj_path_normalize_sep_u")));

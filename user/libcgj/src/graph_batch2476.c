/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2476: freestanding three-way path join length.
 *
 * Surface (unique symbols):
 *   size_t gj_path_join3_len_u(const char *a, const char *b, const char *c);
 *     — Byte length of the string that would result from joining a, b,
 *       and c with a single '/' between non-empty components (including
 *       the trailing NUL). Empty / NULL components contribute nothing
 *       and do not force extra separators. A component that already
 *       ends with '/' or '\\' does not receive an inserted separator
 *       before the next non-empty part. Returns 0 only when all three
 *       are NULL/empty would yield the empty string + NUL → 1 is the
 *       empty-join minimum (just the trailing NUL → returns 1).
 *   size_t __gj_path_join3_len_u  (alias)
 *   __libcgj_batch2476_marker = "libcgj-batch2476"
 *
 * Path exclusive wave (2471–2480). Distinct from gj_str_join_path3
 * (batch1042 buffer write) and gj_path_join2 (batch478) — length-only
 * surface; no write.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>

const char __libcgj_batch2476_marker[] = "libcgj-batch2476";

/* ---- freestanding helpers ---------------------------------------------- */

static int
b2476_is_sep(char ch)
{
	return (ch == '/' || ch == '\\') ? 1 : 0;
}

static size_t
b2476_len(const char *sz)
{
	size_t n = 0u;

	if (sz == NULL) {
		return 0u;
	}
	while (sz[n] != '\0') {
		n++;
	}
	return n;
}

/*
 * Length including trailing NUL of a/b/c joined with '/' as needed.
 */
static size_t
b2476_join3_len(const char *szA, const char *szB, const char *szC)
{
	const char *asz[3];
	size_t an[3];
	size_t nOut;
	size_t iPart;
	int fNeedSep;
	int fEndsSep;

	asz[0] = szA;
	asz[1] = szB;
	asz[2] = szC;
	an[0] = b2476_len(szA);
	an[1] = b2476_len(szB);
	an[2] = b2476_len(szC);

	nOut = 0u;
	fNeedSep = 0;
	fEndsSep = 0;

	for (iPart = 0u; iPart < 3u; iPart++) {
		size_t nPart = an[iPart];
		const char *szPart = asz[iPart];

		if (nPart == 0u) {
			continue;
		}

		if (fNeedSep && !fEndsSep) {
			/* Insert one '/' between parts. */
			nOut++;
		}

		nOut += nPart;
		fEndsSep = b2476_is_sep(szPart[nPart - 1u]);
		fNeedSep = 1;
	}

	/* Trailing NUL. */
	return nOut + 1u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_path_join3_len_u — bytes needed (incl. NUL) for a/b/c path join.
 *
 * Examples (return includes trailing NUL):
 *   ("a","b","c")     → 6  ("a/b/c" + NUL)
 *   ("a/","b","c")    → 5  ("a/b/c" + NUL)
 *   ("","","")        → 1  ("" + NUL)
 *   (NULL,"x",NULL)   → 2  ("x" + NUL)
 *   ("a","","c")      → 4  ("a/c" + NUL)
 */
size_t
gj_path_join3_len_u(const char *szA, const char *szB, const char *szC)
{
	(void)NULL;
	return b2476_join3_len(szA, szB, szC);
}

/* ---- underscored alias ------------------------------------------------- */

size_t __gj_path_join3_len_u(const char *szA, const char *szB, const char *szC)
    __attribute__((alias("gj_path_join3_len_u")));

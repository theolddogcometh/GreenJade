/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch354: join a string array with a separator.
 *
 * Surface (unique symbols):
 *   int gj_join(const char *const *parts, size_t n, const char *sep,
 *               char *out, size_t cap);
 *     — Concatenate parts[0..n) into out with sep between consecutive
 *       elements. Writes a NUL-terminated result when the full string
 *       fits in cap bytes (including the terminator). Returns the number
 *       of characters written (excluding the terminating NUL) on success;
 *       -1 on NULL out, cap == 0, parts == NULL with n > 0, size
 *       overflow, result longer than INT_MAX, or insufficient cap.
 *       NULL sep is treated as empty (""). NULL parts[i] is treated as
 *       empty (""). n == 0 yields the empty string (return 0) when cap
 *       allows a lone NUL.
 *   int __gj_join  (alias)
 *   __libcgj_batch354_marker = "libcgj-batch354"
 *
 * Distinct from gj_path_join (batch149), which joins two path components
 * with a single '/'. Clean-room freestanding pure C (integer/pointer
 * only). Compiles with -ffreestanding -msse2 -Wall -Wextra -Werror.
 * No malloc, no errno, no libc string helpers. No third-party source
 * copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch354_marker[] = "libcgj-batch354";

/* ---- freestanding helpers ---------------------------------------------- */

static size_t
b354_len(const char *sz)
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
 * Add cbAdd into *pCb with saturation check. Returns 0 on success, -1 if
 * the sum would wrap size_t.
 */
static int
b354_add(size_t *pCb, size_t cbAdd)
{
	if (cbAdd > (size_t)-1 - *pCb) {
		return -1;
	}
	*pCb += cbAdd;
	return 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_join — concatenate parts with separator into a bounded buffer.
 *
 * parts: array of n C strings (may be NULL only when n == 0)
 * n:     number of elements in parts
 * sep:   separator placed between elements (NULL → "")
 * out:   destination buffer
 * cap:   capacity of out including space for the terminating NUL
 *
 * On success out is NUL-terminated and the return value is the string
 * length (no NUL). On failure out is left unspecified and -1 is returned.
 */
int
gj_join(const char *const *parts, size_t n, const char *sep, char *out,
    size_t cap)
{
	const char *szSep;
	size_t cbSep;
	size_t cbNeed;
	size_t iPart;
	size_t iOut;
	size_t cbPart;
	size_t iByte;
	const char *szPart;

	if (out == NULL || cap == 0u) {
		return -1;
	}
	if (n > 0u && parts == NULL) {
		return -1;
	}

	szSep = (sep != NULL) ? sep : "";
	cbSep = b354_len(szSep);

	/* Total payload length (excluding NUL). */
	cbNeed = 0u;
	for (iPart = 0u; iPart < n; iPart++) {
		szPart = parts[iPart];
		cbPart = b354_len(szPart);
		if (b354_add(&cbNeed, cbPart) != 0) {
			return -1;
		}
		/* Separator only between consecutive parts. */
		if (iPart + 1u < n) {
			if (b354_add(&cbNeed, cbSep) != 0) {
				return -1;
			}
		}
	}

	/* Need room for payload + terminating NUL. */
	if (cbNeed + 1u < cbNeed || cbNeed + 1u > cap) {
		return -1;
	}
	/* Return value is int; reject lengths that do not fit. */
	if (cbNeed > (size_t)0x7fffffff) {
		return -1;
	}

	iOut = 0u;
	for (iPart = 0u; iPart < n; iPart++) {
		szPart = parts[iPart];
		cbPart = b354_len(szPart);
		for (iByte = 0u; iByte < cbPart; iByte++) {
			out[iOut++] = szPart[iByte];
		}
		if (iPart + 1u < n) {
			for (iByte = 0u; iByte < cbSep; iByte++) {
				out[iOut++] = szSep[iByte];
			}
		}
	}
	out[iOut] = '\0';
	return (int)iOut;
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_join(const char *const *parts, size_t n, const char *sep, char *out,
    size_t cap) __attribute__((alias("gj_join")));

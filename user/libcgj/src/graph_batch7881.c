/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7881: C-locale strcoll stub (byte collate).
 *
 * Surface (unique symbols):
 *   int gj_strcoll_c_7881(const char *a, const char *b);
 *     - C-locale collate of NUL-terminated strings: unsigned octet
 *       strcmp semantics. NULL args: both NULL -> 0; only a NULL -> -1;
 *       only b NULL -> 1.
 *   int __gj_strcoll_c_7881  (alias)
 *   __libcgj_batch7881_marker = "libcgj-batch7881"
 *
 * Exclusive continuum CREATE-ONLY (7881-7890: strcoll/strxfrm stubs —
 * coll_c, xfrm_c, ncoll_c, xfrm_need, coll_ci, coll_eq, memcoll,
 * xfrm_into, coll_l, xfrm_l). Unique gj_strcoll_c_7881 surface only;
 * no multi-def with libc strcoll. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc string helpers. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7881_marker[] = "libcgj-batch7881";

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * Unsigned-octet C-locale collate of two non-NULL NUL strings.
 * Returns <0, 0, or >0 like strcmp.
 */
static int
b7881_coll(const unsigned char *pA, const unsigned char *pB)
{
	size_t i;

	i = 0u;
	while (pA[i] != 0u && pA[i] == pB[i]) {
		i++;
	}
	return (int)pA[i] - (int)pB[i];
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_strcoll_c_7881 - C-locale strcoll stub (byte order).
 *
 * a, b: NUL-terminated C strings (NULL handled as above)
 *
 * Soft freestanding collate only; does not call libc strcoll/strcmp.
 * No parent wires.
 */
int
gj_strcoll_c_7881(const char *a, const char *b)
{
	(void)NULL;
	if (a == NULL || b == NULL) {
		if (a == b) {
			return 0;
		}
		return (a == NULL) ? -1 : 1;
	}
	return b7881_coll((const unsigned char *)a,
	    (const unsigned char *)b);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_strcoll_c_7881(const char *a, const char *b)
    __attribute__((alias("gj_strcoll_c_7881")));

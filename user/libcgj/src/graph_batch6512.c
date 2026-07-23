/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6512: Steam client binary name length deepen.
 *
 * Surface (unique symbols):
 *   uint32_t gj_scli_bin_len_6512(const char *name);
 *     - Return the byte length of name when it is a non-NULL string of
 *       length 0..64 with no '/'; return 0 for NULL, embedded '/', or
 *       length > 64. Soft pure-data length measure for the post-6500
 *       steam client deepen continuum (does not validate charset).
 *   uint32_t __gj_scli_bin_len_6512  (alias)
 *   __libcgj_batch6512_marker = "libcgj-batch6512"
 *
 * Exclusive continuum CREATE-ONLY (6511-6520: post-6500 steam client
 * deepen). Unique gj_scli_bin_len_6512 surface only; no multi-def.
 * Distinct from gj_scli_bin_ok_6511 and gj_bar3_install_path_leaf_len_6006.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6512_marker[] = "libcgj-batch6512";

#define B6512_NAME_MAX  64u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6512_bin_len(const char *szName)
{
	size_t nLen;

	if (szName == NULL) {
		return 0u;
	}
	nLen = 0u;
	while (szName[nLen] != '\0') {
		if (szName[nLen] == '/') {
			return 0u;
		}
		if (nLen >= (size_t)B6512_NAME_MAX) {
			return 0u;
		}
		nLen++;
	}
	return (uint32_t)nLen;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_scli_bin_len_6512 - Steam client binary basename length.
 *
 * name: candidate basename (not a path); NULL / slash / oversize → 0
 *
 * Returns length in [0, 64]. Soft pure-data only. Does not call libc.
 * No parent wires.
 */
uint32_t
gj_scli_bin_len_6512(const char *szName)
{
	(void)NULL;
	return b6512_bin_len(szName);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_scli_bin_len_6512(const char *szName)
    __attribute__((alias("gj_scli_bin_len_6512")));

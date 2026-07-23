/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6511: Steam client binary basename soft-ok
 * (post-6500 deepen).
 *
 * Surface (unique symbols):
 *   uint32_t gj_scli_bin_ok_6511(const char *name);
 *     - Return 1 if name is a non-NULL non-empty basename of length
 *       1..64 with no '/', using only [A-Za-z0-9._-], suitable as a
 *       Steam client binary name (e.g. "steam", "steam.sh"). Soft
 *       shape check; does not resolve or exec.
 *   uint32_t __gj_scli_bin_ok_6511  (alias)
 *   __libcgj_batch6511_marker = "libcgj-batch6511"
 *
 * Exclusive continuum CREATE-ONLY (6511-6520: post-6500 steam client
 * deepen — scli_bin_ok_6511, scli_bin_len_6512, scli_bin_min_6513,
 * scli_bin_max_6514, scli_bin_len_ok_6515, scli_pkgdir_ok_6516,
 * scli_deepen_score_6517, scli_deepen_threshold_6518,
 * scli_deepen_pass_6519, scli_deepen_ready_6520). Unique
 * gj_scli_bin_ok_6511 surface only; no multi-def. Distinct from
 * gj_steam_bin_ok (batch2633) and gj_steam_client_boot_root_5661.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6511_marker[] = "libcgj-batch6511";

/* Max basename length for a Steam client binary product name. */
#define B6511_NAME_MAX  64u

/* ---- freestanding helpers ---------------------------------------------- */

/* 1 if ch is a permitted Steam client binary basename character. */
static int
b6511_ch_ok(char ch)
{
	if (ch >= 'a' && ch <= 'z') {
		return 1;
	}
	if (ch >= 'A' && ch <= 'Z') {
		return 1;
	}
	if (ch >= '0' && ch <= '9') {
		return 1;
	}
	if (ch == '.' || ch == '-' || ch == '_') {
		return 1;
	}
	return 0;
}

/* Soft basename shape check for Steam client binary names. */
static uint32_t
b6511_bin_ok(const char *szName)
{
	size_t nLen;

	if (szName == NULL) {
		return 0u;
	}
	nLen = 0u;
	while (szName[nLen] != '\0') {
		if (nLen >= (size_t)B6511_NAME_MAX) {
			return 0u;
		}
		if (szName[nLen] == '/') {
			return 0u;
		}
		if (b6511_ch_ok(szName[nLen]) == 0) {
			return 0u;
		}
		nLen++;
	}
	if (nLen == 0u) {
		return 0u;
	}
	return 1u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_scli_bin_ok_6511 - soft Steam client binary basename check.
 *
 * name: candidate basename (e.g. "steam", "steam.sh"); not a path
 *
 * Returns 1 when name is non-empty, length <= 64, has no '/', and uses
 * only [A-Za-z0-9._-], else 0. Soft pure-data for the post-6500 steam
 * client deepen continuum. Does not call libc. No parent wires.
 */
uint32_t
gj_scli_bin_ok_6511(const char *szName)
{
	(void)NULL;
	return b6511_bin_ok(szName);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_scli_bin_ok_6511(const char *szName)
    __attribute__((alias("gj_scli_bin_ok_6511")));

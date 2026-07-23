/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8716: POSIX shm name form probe stub (_u).
 *
 * Surface (unique symbols):
 *   uint32_t gj_shm_name_ok_u_8716(const char *name);
 *     - Return 1 if name is a portable POSIX shm object name: leading
 *       '/', then one or more non-'/' body octets, no further '/',
 *       within a 255-octet scan budget. NULL / empty / lone '/' → 0.
 *       Probe only — does not call shm_open or shm_unlink.
 *   uint32_t __gj_shm_name_ok_u_8716  (alias)
 *   __libcgj_batch8716_marker = "libcgj-batch8716"
 *
 * Exclusive continuum CREATE-ONLY (8711-8720: memfd/shm id stubs —
 * memfd_ready_u, memfd_cloexec_id, memfd_allow_seal_id, shm_open_ready_u,
 * shm_unlink_ready_u, shm_name_ok_u, seal_grow_id, seal_shrink_id,
 * seal_write_id, batch_id_8720). Unique gj_shm_name_ok_u_8716 surface
 * only; no multi-def. Distinct from gj_env_name_ok_u_8331 and
 * shm_open flag stubs 7141-7150. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8716_marker[] = "libcgj-batch8716";

/* Inclusive scan budget for shm name octets (NAME_MAX-ish). */
#define B8716_SCAN_MAX  255u

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * 1 if szName is a portable POSIX shm name:
 *   '/' + non-empty body of non-'/' octets, no extra '/', within budget.
 */
static uint32_t
b8716_shm_name_ok(const char *szName)
{
	size_t i;

	if (szName == NULL || szName[0] != '/') {
		return 0u;
	}
	/* Must have at least one body octet after the leading slash. */
	if (szName[1] == '\0') {
		return 0u;
	}
	i = 1u;
	while (i < B8716_SCAN_MAX && szName[i] != '\0') {
		if (szName[i] == '/') {
			return 0u;
		}
		i++;
	}
	/* Over-budget without NUL: reject (unbounded / truncated). */
	if (i >= B8716_SCAN_MAX && szName[i] != '\0') {
		return 0u;
	}
	return 1u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_shm_name_ok_u_8716 - 1 if name is a portable POSIX shm object name.
 *
 * name: NUL-terminated candidate name (may be NULL)
 *
 * Probe stub only — no shm_open / shm_unlink. No parent wires.
 */
uint32_t
gj_shm_name_ok_u_8716(const char *szName)
{
	(void)NULL;
	return b8716_shm_name_ok(szName);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_shm_name_ok_u_8716(const char *szName)
    __attribute__((alias("gj_shm_name_ok_u_8716")));

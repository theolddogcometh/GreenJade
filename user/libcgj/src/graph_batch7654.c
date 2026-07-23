/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7654: uname version field length stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_uname_version_len_7654(void);
 *     - Returns soft utsname.version field length (65).
 *   uint32_t __gj_uname_version_len_7654  (alias)
 *   __libcgj_batch7654_marker = "libcgj-batch7654"
 *
 * Exclusive continuum CREATE-ONLY (7651-7660: uname field length stubs —
 * sysname_len, nodename_len, release_len, version_len, machine_len,
 * domainname_len, nmln, field_maxlen, field_len_ok, continuum +
 * batch_id_7660). Unique gj_uname_version_len_7654 surface only; no
 * multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7654_marker[] = "libcgj-batch7654";

/* Soft Linux _UTSNAME_VERSION_LENGTH / _UTSNAME_LENGTH (incl. NUL). */
#define B7654_VERSION_LEN  65u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7654_version_len(void)
{
	return B7654_VERSION_LEN;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_uname_version_len_7654 - soft utsname.version field byte length.
 *
 * Always returns 65 (Linux soft _UTSNAME_VERSION_LENGTH including the
 * terminating NUL). Soft pure-data product tag; does not call uname.
 * No parent wires.
 */
uint32_t
gj_uname_version_len_7654(void)
{
	(void)NULL;
	return b7654_version_len();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_uname_version_len_7654(void)
    __attribute__((alias("gj_uname_version_len_7654")));

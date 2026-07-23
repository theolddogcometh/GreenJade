/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7657: uname _UTSNAME_LENGTH / SYS_NMLN stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_uname_nmln_7657(void);
 *     - Returns soft _UTSNAME_LENGTH / SYS_NMLN baseline (65).
 *   uint32_t __gj_uname_nmln_7657  (alias)
 *   __libcgj_batch7657_marker = "libcgj-batch7657"
 *
 * Exclusive continuum CREATE-ONLY (7651-7660: uname field length stubs —
 * sysname_len, nodename_len, release_len, version_len, machine_len,
 * domainname_len, nmln, field_maxlen, field_len_ok, continuum +
 * batch_id_7660). Unique gj_uname_nmln_7657 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7657_marker[] = "libcgj-batch7657";

/* Soft Linux _UTSNAME_LENGTH / SYS_NMLN (bytes per utsname field). */
#define B7657_NMLN  65u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7657_nmln(void)
{
	return B7657_NMLN;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_uname_nmln_7657 - soft _UTSNAME_LENGTH / SYS_NMLN baseline.
 *
 * Always returns 65. Soft pure-data product tag for the shared utsname
 * field size on Linux; does not call uname or touch libc headers.
 * No parent wires.
 */
uint32_t
gj_uname_nmln_7657(void)
{
	(void)NULL;
	return b7657_nmln();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_uname_nmln_7657(void)
    __attribute__((alias("gj_uname_nmln_7657")));

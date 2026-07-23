/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7659: uname field length validity stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_uname_field_len_ok_u_7659(uint32_t len);
 *     - Return 1 if len equals soft _UTSNAME_LENGTH (65), else 0.
 *   uint32_t __gj_uname_field_len_ok_u_7659  (alias)
 *   __libcgj_batch7659_marker = "libcgj-batch7659"
 *
 * Exclusive continuum CREATE-ONLY (7651-7660: uname field length stubs —
 * sysname_len, nodename_len, release_len, version_len, machine_len,
 * domainname_len, nmln, field_maxlen, field_len_ok, continuum +
 * batch_id_7660). Unique gj_uname_field_len_ok_u_7659 surface only; no
 * multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7659_marker[] = "libcgj-batch7659";

/* Soft expected utsname field length (bytes, incl. NUL). */
#define B7659_NMLN  65u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7659_field_len_ok(uint32_t u32Len)
{
	return (u32Len == B7659_NMLN) ? 1u : 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_uname_field_len_ok_u_7659 - 1 if len is the soft _UTSNAME_LENGTH.
 *
 * len: candidate field length in bytes
 *
 * Returns 1 when len == 65, else 0. Pure equality test; no parent wires.
 * Does not call uname.
 */
uint32_t
gj_uname_field_len_ok_u_7659(uint32_t u32Len)
{
	(void)NULL;
	return b7659_field_len_ok(u32Len);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_uname_field_len_ok_u_7659(uint32_t u32Len)
    __attribute__((alias("gj_uname_field_len_ok_u_7659")));

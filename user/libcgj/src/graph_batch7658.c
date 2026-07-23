/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7658: uname field max usable chars stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_uname_field_maxlen_7658(void);
 *     - Returns max printable chars per utsname field (64 = 65-1 NUL).
 *   uint32_t __gj_uname_field_maxlen_7658  (alias)
 *   __libcgj_batch7658_marker = "libcgj-batch7658"
 *
 * Exclusive continuum CREATE-ONLY (7651-7660: uname field length stubs —
 * sysname_len, nodename_len, release_len, version_len, machine_len,
 * domainname_len, nmln, field_maxlen, field_len_ok, continuum +
 * batch_id_7660). Unique gj_uname_field_maxlen_7658 surface only; no
 * multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7658_marker[] = "libcgj-batch7658";

/* Soft max usable chars in a 65-byte utsname field (room for NUL). */
#define B7658_FIELD_MAXLEN  64u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7658_field_maxlen(void)
{
	return B7658_FIELD_MAXLEN;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_uname_field_maxlen_7658 - max printable chars per utsname field.
 *
 * Always returns 64 (soft _UTSNAME_LENGTH - 1 for the terminating NUL).
 * Soft pure-data product tag; does not call uname. No parent wires.
 */
uint32_t
gj_uname_field_maxlen_7658(void)
{
	(void)NULL;
	return b7658_field_maxlen();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_uname_field_maxlen_7658(void)
    __attribute__((alias("gj_uname_field_maxlen_7658")));

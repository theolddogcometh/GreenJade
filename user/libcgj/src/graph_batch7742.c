/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7742: getdents syscall number id stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_getdents_nr_id_7742(void);
 *     - Return soft x86_64 __NR_getdents catalog id (78).
 *   uint32_t __gj_getdents_nr_id_7742  (alias)
 *   __libcgj_batch7742_marker = "libcgj-batch7742"
 *
 * Exclusive continuum CREATE-ONLY (7741-7750: getdents dirent stubs —
 * root, getdents_nr, getdents64_nr, dt_dir, dt_reg, dt_ok, reclen_align,
 * reclen_min, dt_errorish, continuum + batch_id_7750). Unique
 * gj_getdents_nr_id_7742 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7742_marker[] = "libcgj-batch7742";

/* Soft x86_64 __NR_getdents (legacy packed dirent). */
#define B7742_NR_GETDENTS  ((uint32_t)78u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7742_nr(void)
{
	return B7742_NR_GETDENTS;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_getdents_nr_id_7742 - soft __NR_getdents catalog constant.
 *
 * Always returns 78 (Linux x86_64 getdents). Catalog id only; does not
 * issue a syscall. No parent wires.
 */
uint32_t
gj_getdents_nr_id_7742(void)
{
	(void)NULL;
	return b7742_nr();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_getdents_nr_id_7742(void)
    __attribute__((alias("gj_getdents_nr_id_7742")));

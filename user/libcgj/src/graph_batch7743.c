/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7743: getdents64 syscall number id stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_getdents64_nr_id_7743(void);
 *     - Return soft x86_64 __NR_getdents64 catalog id (217).
 *   uint32_t __gj_getdents64_nr_id_7743  (alias)
 *   __libcgj_batch7743_marker = "libcgj-batch7743"
 *
 * Exclusive continuum CREATE-ONLY (7741-7750: getdents dirent stubs —
 * root, getdents_nr, getdents64_nr, dt_dir, dt_reg, dt_ok, reclen_align,
 * reclen_min, dt_errorish, continuum + batch_id_7750). Unique
 * gj_getdents64_nr_id_7743 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7743_marker[] = "libcgj-batch7743";

/* Soft x86_64 __NR_getdents64 (linux_dirent64 with d_type). */
#define B7743_NR_GETDENTS64  ((uint32_t)217u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7743_nr(void)
{
	return B7743_NR_GETDENTS64;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_getdents64_nr_id_7743 - soft __NR_getdents64 catalog constant.
 *
 * Always returns 217 (Linux x86_64 getdents64). Catalog id only; does
 * not issue a syscall. No parent wires.
 */
uint32_t
gj_getdents64_nr_id_7743(void)
{
	(void)NULL;
	return b7743_nr();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_getdents64_nr_id_7743(void)
    __attribute__((alias("gj_getdents64_nr_id_7743")));

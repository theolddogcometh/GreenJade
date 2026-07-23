/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8373: mmap/mprotect PROT_WRITE flag id stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_prot_write_id_8373(void);
 *     - Returns soft PROT_WRITE (0x2). Catalog constant only.
 *   uint32_t __gj_prot_write_id_8373  (alias)
 *   __libcgj_batch8373_marker = "libcgj-batch8373"
 *
 * Exclusive continuum CREATE-ONLY (8371-8380: mmap/prot flag stubs
 * integer). Unique batch-suffixed surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8373_marker[] = "libcgj-batch8373";

/* Soft PROT_WRITE (Linux/glibc-shaped). */
#define B8373_PROT_WRITE  ((uint32_t)0x2u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8373_prot_write_id(void)
{
	return B8373_PROT_WRITE;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_prot_write_id_8373 - soft PROT_WRITE flag value.
 *
 * Always returns 2. Pure catalog stub for mmap/mprotect prot bits.
 * No parent wires.
 */
uint32_t
gj_prot_write_id_8373(void)
{
	(void)NULL;
	return b8373_prot_write_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_prot_write_id_8373(void)
    __attribute__((alias("gj_prot_write_id_8373")));

/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8371: mmap/mprotect PROT_NONE flag id stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_prot_none_id_8371(void);
 *     - Returns soft PROT_NONE (0x0). Catalog constant only.
 *   uint32_t __gj_prot_none_id_8371  (alias)
 *   __libcgj_batch8371_marker = "libcgj-batch8371"
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

const char __libcgj_batch8371_marker[] = "libcgj-batch8371";

/* Soft PROT_NONE (Linux/glibc-shaped). */
#define B8371_PROT_NONE  ((uint32_t)0x0u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8371_prot_none_id(void)
{
	return B8371_PROT_NONE;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_prot_none_id_8371 - soft PROT_NONE flag value.
 *
 * Always returns 0. Pure catalog stub for mmap/mprotect prot bits.
 * No parent wires.
 */
uint32_t
gj_prot_none_id_8371(void)
{
	(void)NULL;
	return b8371_prot_none_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_prot_none_id_8371(void)
    __attribute__((alias("gj_prot_none_id_8371")));

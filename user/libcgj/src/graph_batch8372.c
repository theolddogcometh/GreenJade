/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8372: mmap/mprotect PROT_READ flag id stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_prot_read_id_8372(void);
 *     - Returns soft PROT_READ (0x1). Catalog constant only.
 *   uint32_t __gj_prot_read_id_8372  (alias)
 *   __libcgj_batch8372_marker = "libcgj-batch8372"
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

const char __libcgj_batch8372_marker[] = "libcgj-batch8372";

/* Soft PROT_READ (Linux/glibc-shaped). */
#define B8372_PROT_READ  ((uint32_t)0x1u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8372_prot_read_id(void)
{
	return B8372_PROT_READ;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_prot_read_id_8372 - soft PROT_READ flag value.
 *
 * Always returns 1. Pure catalog stub for mmap/mprotect prot bits.
 * No parent wires.
 */
uint32_t
gj_prot_read_id_8372(void)
{
	(void)NULL;
	return b8372_prot_read_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_prot_read_id_8372(void)
    __attribute__((alias("gj_prot_read_id_8372")));

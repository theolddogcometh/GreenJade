/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8374: mmap/mprotect PROT_EXEC flag id stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_prot_exec_id_8374(void);
 *     - Returns soft PROT_EXEC (0x4). Catalog constant only.
 *   uint32_t __gj_prot_exec_id_8374  (alias)
 *   __libcgj_batch8374_marker = "libcgj-batch8374"
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

const char __libcgj_batch8374_marker[] = "libcgj-batch8374";

/* Soft PROT_EXEC (Linux/glibc-shaped). */
#define B8374_PROT_EXEC  ((uint32_t)0x4u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8374_prot_exec_id(void)
{
	return B8374_PROT_EXEC;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_prot_exec_id_8374 - soft PROT_EXEC flag value.
 *
 * Always returns 4. Pure catalog stub for mmap/mprotect prot bits.
 * No parent wires.
 */
uint32_t
gj_prot_exec_id_8374(void)
{
	(void)NULL;
	return b8374_prot_exec_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_prot_exec_id_8374(void)
    __attribute__((alias("gj_prot_exec_id_8374")));

/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7024: rseq_cs critical-section size.
 *
 * Surface (unique symbols):
 *   uint32_t gj_rseq_cs_size_7024(void);
 *     - Returns 32 (soft sizeof(struct rseq_cs) layout size).
 *   uint32_t __gj_rseq_cs_size_7024  (alias)
 *   __libcgj_batch7024_marker = "libcgj-batch7024"
 *
 * Exclusive continuum CREATE-ONLY (7021-7030: rseq signature stubs).
 * Unique gj_rseq_cs_size_7024 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7024_marker[] = "libcgj-batch7024";

/*
 * Soft: struct rseq_cs is version+flags+start_ip+post_commit_offset+
 * abort_ip = 4+4+8+8+8 = 32 bytes.
 */
#define B7024_CS_SIZE  32u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7024_cs_size(void)
{
	return B7024_CS_SIZE;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_rseq_cs_size_7024 - soft sizeof(struct rseq_cs).
 *
 * Always returns 32. Soft pure-data layout tag for the critical-section
 * descriptor pointed at by rseq.rseq_cs. Does not touch TLS. No parent
 * wires.
 */
uint32_t
gj_rseq_cs_size_7024(void)
{
	(void)NULL;
	return b7024_cs_size();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_rseq_cs_size_7024(void)
    __attribute__((alias("gj_rseq_cs_size_7024")));

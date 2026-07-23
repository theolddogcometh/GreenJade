/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7021: rseq signature soft constant.
 *
 * Surface (unique symbols):
 *   uint32_t gj_rseq_sig_7021(void);
 *     - Returns soft RSEQ_SIG (0x53053053, x86_64-style abort signature).
 *   uint32_t __gj_rseq_sig_7021  (alias)
 *   __libcgj_batch7021_marker = "libcgj-batch7021"
 *
 * Exclusive continuum CREATE-ONLY (7021-7030: rseq signature stubs —
 * sig_7021, len_min_7022, flag_unregister_7023, cs_size_7024,
 * cpu_id_special_7025, cs_flags_7026, sig_match_7027, len_ok_7028,
 * flags_ok_7029, continuum + batch_id_7030). Unique gj_rseq_sig_7021
 * surface only; no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7021_marker[] = "libcgj-batch7021";

/* Soft: classic x86_64 RSEQ_SIG abort signature (ud1-encoded pattern). */
#define B7021_RSEQ_SIG  0x53053053u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7021_sig(void)
{
	return B7021_RSEQ_SIG;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_rseq_sig_7021 - soft rseq abort/signature constant.
 *
 * Always returns 0x53053053. Soft pure-data tag for the 32-bit signature
 * passed as the final rseq(2) argument and matched at the abort IP.
 * Does not call rseq. No parent wires.
 */
uint32_t
gj_rseq_sig_7021(void)
{
	(void)NULL;
	return b7021_sig();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_rseq_sig_7021(void)
    __attribute__((alias("gj_rseq_sig_7021")));

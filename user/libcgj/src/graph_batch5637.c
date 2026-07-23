/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5637: preferred PCR hash algorithm (SHA-256).
 *
 * Surface (unique symbols):
 *   uint32_t gj_pcr_bank_alg_sha256_5637(void);
 *     - Returns 0x000B (TCG TPM_ALG_SHA256). Soft constant for install
 *       measured-boot bank selection; no TPM commands issued.
 *   uint32_t __gj_pcr_bank_alg_sha256_5637  (alias)
 *   __libcgj_batch5637_marker = "libcgj-batch5637"
 *
 * Exclusive continuum CREATE-ONLY (5631-5640: secure boot / TPM /
 * measured boot stubs for install path). Unique
 * gj_pcr_bank_alg_sha256_5637 surface only; no multi-def. Distinct
 * from gj_pcr_index_ok_5636 (batch5636). No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5637_marker[] = "libcgj-batch5637";

/* TCG algorithm ID for SHA-256 (public registry constant). */
#define B5637_TPM_ALG_SHA256  0x000Bu

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5637_alg_sha256(void)
{
	return B5637_TPM_ALG_SHA256;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_pcr_bank_alg_sha256_5637 - preferred measured-boot PCR bank alg.
 *
 * Always returns 0x000B (TPM_ALG_SHA256). Soft pure-data only.
 * No parent wires.
 */
uint32_t
gj_pcr_bank_alg_sha256_5637(void)
{
	(void)NULL;
	return b5637_alg_sha256();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_pcr_bank_alg_sha256_5637(void)
    __attribute__((alias("gj_pcr_bank_alg_sha256_5637")));

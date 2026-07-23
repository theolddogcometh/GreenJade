/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5634: minimum TPM family for install path.
 *
 * Surface (unique symbols):
 *   uint32_t gj_tpm_family_min_5634(void);
 *     - Returns 2 (soft minimum TPM family: TPM 2.0). Install continuum
 *       does not accept TPM 1.2 for measured-boot gates.
 *   uint32_t __gj_tpm_family_min_5634  (alias)
 *   __libcgj_batch5634_marker = "libcgj-batch5634"
 *
 * Exclusive continuum CREATE-ONLY (5631-5640: secure boot / TPM /
 * measured boot stubs for install path). Unique
 * gj_tpm_family_min_5634 surface only; no multi-def. Distinct from
 * gj_tpm_present_ok_5633 (batch5633). No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5634_marker[] = "libcgj-batch5634";

/* Soft minimum TPM family (TPM 2.0). */
#define B5634_TPM_FAMILY_MIN  2u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5634_family_min(void)
{
	return B5634_TPM_FAMILY_MIN;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_tpm_family_min_5634 - report minimum accepted TPM family.
 *
 * Always returns 2. Soft pure-data constant for install gating.
 * Does not call TCG commands. No parent wires.
 */
uint32_t
gj_tpm_family_min_5634(void)
{
	(void)NULL;
	return b5634_family_min();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_tpm_family_min_5634(void)
    __attribute__((alias("gj_tpm_family_min_5634")));

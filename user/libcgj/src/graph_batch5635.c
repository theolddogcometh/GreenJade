/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5635: TPM family ok predicate (install path).
 *
 * Surface (unique symbols):
 *   uint32_t gj_tpm_family_ok_5635(uint32_t family);
 *     - Return 1 when family >= 2 (TPM 2.0+), else 0. Matches soft
 *       minimum from gj_tpm_family_min_5634.
 *   uint32_t __gj_tpm_family_ok_5635  (alias)
 *   __libcgj_batch5635_marker = "libcgj-batch5635"
 *
 * Exclusive continuum CREATE-ONLY (5631-5640: secure boot / TPM /
 * measured boot stubs for install path). Unique
 * gj_tpm_family_ok_5635 surface only; no multi-def. Distinct from
 * gj_tpm_family_min_5634 (batch5634). No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5635_marker[] = "libcgj-batch5635";

/* Soft minimum TPM family (matches batch5634). */
#define B5635_TPM_FAMILY_MIN  2u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5635_family_ok(uint32_t u32Family)
{
	return (u32Family >= B5635_TPM_FAMILY_MIN) ? 1u : 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_tpm_family_ok_5635 - true when TPM family meets install minimum.
 *
 * family: reported TPM family major (1 = 1.2, 2 = 2.0)
 *
 * Returns 1 when family >= 2, else 0. Pure compare; no TPM I/O.
 * No parent wires.
 */
uint32_t
gj_tpm_family_ok_5635(uint32_t u32Family)
{
	(void)NULL;
	return b5635_family_ok(u32Family);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_tpm_family_ok_5635(uint32_t u32Family)
    __attribute__((alias("gj_tpm_family_ok_5635")));

/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch9778: tss ok soft unit (off).
 *
 * Surface (unique symbols):
 *   uint32_t gj_tss_ok_u_9778(void);
 *     - Always returns 0. Soft stub only; intentionally not a real
 *       TSS / TCG Software Stack readiness probe.
 *   uint32_t __gj_tss_ok_u_9778  (alias)
 *   __libcgj_batch9778_marker = "libcgj-batch9778"
 *
 * Exclusive continuum CREATE-ONLY (9771-9780: tpm soft id
 * stubs — tpm_device_ok_u_9771, tpm_pcr_ok_u_9772,
 * tpm_nv_ok_u_9773, tpm_quote_ok_u_9774, tpm_seal_ok_u_9775,
 * tpm_unseal_ok_u_9776, tpm2_ok_u_9777, tss_ok_u_9778,
 * tpm_ready_u_9779, batch_id_9780). Unique surface only; no multi-def.
 * No parent wires. No __int128. No TPM/TSS implementation.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch9778_marker[] = "libcgj-batch9778";

/* Soft tss-ok lamp: always off (not a real TSS stack probe). */
#define B9778_TSS_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b9778_tss_ok(void)
{
	return B9778_TSS_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_tss_ok_u_9778 - tss ok soft unit flag.
 *
 * Always returns 0. Soft continuum stub; does not load a TSS stack
 * or call libc. No parent wires.
 */
uint32_t
gj_tss_ok_u_9778(void)
{
	(void)NULL;
	return b9778_tss_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_tss_ok_u_9778(void)
    __attribute__((alias("gj_tss_ok_u_9778")));

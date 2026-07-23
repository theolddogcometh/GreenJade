/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch9775: tpm seal ok soft unit (off).
 *
 * Surface (unique symbols):
 *   uint32_t gj_tpm_seal_ok_u_9775(void);
 *     - Always returns 0. Soft stub only; intentionally not a real
 *       TPM seal / key-wrap readiness probe.
 *   uint32_t __gj_tpm_seal_ok_u_9775  (alias)
 *   __libcgj_batch9775_marker = "libcgj-batch9775"
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

const char __libcgj_batch9775_marker[] = "libcgj-batch9775";

/* Soft tpm-seal-ok lamp: always off (not a real seal probe). */
#define B9775_TPM_SEAL_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b9775_tpm_seal_ok(void)
{
	return B9775_TPM_SEAL_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_tpm_seal_ok_u_9775 - tpm seal ok soft unit flag.
 *
 * Always returns 0. Soft continuum stub; does not seal secrets to
 * TPM state or call libc. No parent wires.
 */
uint32_t
gj_tpm_seal_ok_u_9775(void)
{
	(void)NULL;
	return b9775_tpm_seal_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_tpm_seal_ok_u_9775(void)
    __attribute__((alias("gj_tpm_seal_ok_u_9775")));

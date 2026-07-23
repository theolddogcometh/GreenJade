/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch9777: tpm2 ok soft unit (off).
 *
 * Surface (unique symbols):
 *   uint32_t gj_tpm2_ok_u_9777(void);
 *     - Always returns 0. Soft stub only; intentionally not a real
 *       TPM 2.0 command / resource-manager readiness probe.
 *   uint32_t __gj_tpm2_ok_u_9777  (alias)
 *   __libcgj_batch9777_marker = "libcgj-batch9777"
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

const char __libcgj_batch9777_marker[] = "libcgj-batch9777";

/* Soft tpm2-ok lamp: always off (not a real TPM2 probe). */
#define B9777_TPM2_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b9777_tpm2_ok(void)
{
	return B9777_TPM2_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_tpm2_ok_u_9777 - tpm2 ok soft unit flag.
 *
 * Always returns 0. Soft continuum stub; does not issue TPM2 commands
 * or call libc. No parent wires.
 */
uint32_t
gj_tpm2_ok_u_9777(void)
{
	(void)NULL;
	return b9777_tpm2_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_tpm2_ok_u_9777(void)
    __attribute__((alias("gj_tpm2_ok_u_9777")));

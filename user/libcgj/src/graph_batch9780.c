/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch9780: tpm soft continuum + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_9780(void);
 *     - Returns the compile-time graph batch number for this TU (9780).
 *   uint32_t __gj_batch_id_9780  (alias)
 *   __libcgj_batch9780_marker = "libcgj-batch9780"
 *
 * Exclusive continuum CREATE-ONLY (9771-9780: tpm soft id
 * stubs — tpm_device_ok_u_9771, tpm_pcr_ok_u_9772,
 * tpm_nv_ok_u_9773, tpm_quote_ok_u_9774, tpm_seal_ok_u_9775,
 * tpm_unseal_ok_u_9776, tpm2_ok_u_9777, tss_ok_u_9778,
 * tpm_ready_u_9779, batch_id_9780). Unique surfaces only; no multi-def.
 * Does NOT redefine gj_batch_id / gj_graph_milestone / prior batch_id_*
 * symbols — avoid multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch9780_marker[] = "libcgj-batch9780";

#define B9780_BATCH_ID  9780u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b9780_id(void)
{
	return B9780_BATCH_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_9780 - report this TU's graph batch number.
 *
 * Always returns 9780.
 */
uint32_t
gj_batch_id_9780(void)
{
	(void)NULL;
	return b9780_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_9780(void)
    __attribute__((alias("gj_batch_id_9780")));

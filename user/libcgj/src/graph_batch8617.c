/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8617: SSP fail id stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_ssp_fail_id_8617(void);
 *     - Return soft SSP fail-path catalog id (1 = fail handler present).
 *   uint32_t __gj_ssp_fail_id_8617  (alias)
 *   __libcgj_batch8617_marker = "libcgj-batch8617"
 *
 * Exclusive continuum CREATE-ONLY (8611-8620: stack/canary id stubs —
 * stack_guard_id, stack_canary_ok_u, stack_align_u, stack_grow_down_id,
 * stack_page_id, ssp_ready_u, ssp_fail_id, frame_ok_u, ra_align_u,
 * batch_id_8620). Unique gj_ssp_fail_id_8617 surface only; no
 * multi-def. Catalog id only; does not call __stack_chk_fail.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8617_marker[] = "libcgj-batch8617";

/* Soft catalog: SSP fail path identity tag (handler present). */
#define B8617_SSP_FAIL_ID  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8617_ssp_fail_id(void)
{
	return B8617_SSP_FAIL_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_ssp_fail_id_8617 - soft SSP fail-path catalog id.
 *
 * Always returns 1. Catalog id only; does not abort or write diagnostics.
 * No parent wires.
 */
uint32_t
gj_ssp_fail_id_8617(void)
{
	(void)NULL;
	return b8617_ssp_fail_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_ssp_fail_id_8617(void)
    __attribute__((alias("gj_ssp_fail_id_8617")));

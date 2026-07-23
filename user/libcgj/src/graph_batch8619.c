/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8619: return-address align stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_ra_align_u_8619(void);
 *     - Return soft return-address pointer alignment (8 on LP64).
 *   uint32_t __gj_ra_align_u_8619  (alias)
 *   __libcgj_batch8619_marker = "libcgj-batch8619"
 *
 * Exclusive continuum CREATE-ONLY (8611-8620: stack/canary id stubs —
 * stack_guard_id, stack_canary_ok_u, stack_align_u, stack_grow_down_id,
 * stack_page_id, ssp_ready_u, ssp_fail_id, frame_ok_u, ra_align_u,
 * batch_id_8620). Unique gj_ra_align_u_8619 surface only; no multi-def.
 * Catalog constant only. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8619_marker[] = "libcgj-batch8619";

/* Soft LP64 return-address / code-pointer alignment (bytes). */
#define B8619_RA_ALIGN  8u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8619_ra_align(void)
{
	return B8619_RA_ALIGN;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_ra_align_u_8619 - soft return-address alignment catalog constant.
 *
 * Always returns 8. Catalog value only; does not inspect return slots.
 * No parent wires.
 */
uint32_t
gj_ra_align_u_8619(void)
{
	(void)NULL;
	return b8619_ra_align();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_ra_align_u_8619(void)
    __attribute__((alias("gj_ra_align_u_8619")));

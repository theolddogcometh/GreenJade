/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch14609: continuum dyn soft lamp (wave 46).
 *
 * Surface (unique symbols):
 *   uint32_t gj_continuum_dyn_soft_14609(void);
 *     - Returns 1 (continuum dyn soft surface ready for continuum product
 *       deepen wave 46). Soft compile-time product tag; not a live
 *       dyn probe.
 *   uint32_t __gj_continuum_dyn_soft_14609  (alias)
 *   __libcgj_batch14609_marker = "libcgj-batch14609"
 *
 * Exclusive continuum CREATE-ONLY (14601-14610: continuum product deepen wave 46, post-14600). Unique surface only; no multi-def. Distinct from prior continuum_product_deepen / continuum_smoke_soft / continuum_dyn_soft symbols (1450x/1440x/…). No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch14609_marker[] = "libcgj-batch14609";

/* Continuum dyn soft ready lamp. */
#define B14609_DYN_SOFT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b14609_soft(void)
{
	return B14609_DYN_SOFT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_continuum_dyn_soft_14609 - soft continuum surface
 *
 * Always returns 1u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_continuum_dyn_soft_14609(void)
{
	(void)NULL;
	return b14609_soft();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_continuum_dyn_soft_14609(void)
    __attribute__((alias("gj_continuum_dyn_soft_14609")));

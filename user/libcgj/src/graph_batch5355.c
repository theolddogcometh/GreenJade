/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5355: Proton ABI readiness lamp.
 *
 * Surface (unique symbols):
 *   uint32_t gj_proton_abi_ok_u(void);
 *     - Returns 1 (Proton ABI surface considered ok for this continuum).
 *       Soft compile-time product-path tag only.
 *   uint32_t __gj_proton_abi_ok_u  (alias)
 *   __libcgj_batch5355_marker = "libcgj-batch5355"
 *
 * Exclusive continuum CREATE-ONLY (5351-5360: Steam/Deck product path
 * helpers unique). Unique gj_proton_abi_ok_u surface only; no multi-def.
 * Distinct from gj_proton_major_from_ver (batch1864). No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5355_marker[] = "libcgj-batch5355";

/* Proton ABI ok lamp for product-path continuum. */
#define B5355_PROTON_ABI_OK  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5355_proton_abi_ok(void)
{
	return B5355_PROTON_ABI_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_proton_abi_ok_u - report Proton ABI readiness for product path.
 *
 * Always returns 1 (ABI surface ok). Soft pure-data only; does not
 * probe Proton installs. Does not call libc. No parent wires.
 */
uint32_t
gj_proton_abi_ok_u(void)
{
	(void)NULL;
	return b5355_proton_abi_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_proton_abi_ok_u(void)
    __attribute__((alias("gj_proton_abi_ok_u")));

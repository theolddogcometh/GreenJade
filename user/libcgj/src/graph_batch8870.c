/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8870: getaddrinfo flag id continuum batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_8870(void);
 *     - Returns the compile-time graph batch number for this TU (8870).
 *   uint32_t __gj_batch_id_8870  (alias)
 *   __libcgj_batch8870_marker = "libcgj-batch8870"
 *
 * Exclusive continuum CREATE-ONLY (8861-8870: getaddrinfo flag id stubs —
 * passive_id, canonname_id, numeric_host_id, numeric_serv_id, v4mapped_id,
 * all_id, addrconfig_id, gai_ready_u, gai_error_ok_u, batch_id_8870).
 * Unique gj_batch_id_8870 surface only; no multi-def. Does NOT redefine
 * bare gj_batch_id. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8870_marker[] = "libcgj-batch8870";

#define B8870_BATCH_ID  8870u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8870_id(void)
{
	return B8870_BATCH_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_8870 - report this TU's graph batch number.
 *
 * Always returns 8870. Soft pure-data product tag for the 8861-8870
 * getaddrinfo flag id stubs continuum closeout. No parent wires.
 */
uint32_t
gj_batch_id_8870(void)
{
	(void)NULL;
	return b8870_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_8870(void)
    __attribute__((alias("gj_batch_id_8870")));

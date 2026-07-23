/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8869: getaddrinfo EAI success (ok) tag stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_gai_error_ok_u_8869(void);
 *     - Returns 0 (soft EAI success / ok code for gai error catalog).
 *   uint32_t __gj_gai_error_ok_u_8869  (alias)
 *   __libcgj_batch8869_marker = "libcgj-batch8869"
 *
 * Exclusive continuum CREATE-ONLY (8861-8870: getaddrinfo flag id stubs —
 * passive_id, canonname_id, numeric_host_id, numeric_serv_id, v4mapped_id,
 * all_id, addrconfig_id, gai_ready_u, gai_error_ok_u, batch_id_8870).
 * Unique gj_gai_error_ok_u_8869 surface only; no multi-def. Soft pure-data
 * catalog only; does not call getaddrinfo or gai_strerror. No parent
 * wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8869_marker[] = "libcgj-batch8869";

/* Soft gai error catalog: success / ok (0). */
#define B8869_GAI_ERROR_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8869_error_ok(void)
{
	return B8869_GAI_ERROR_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_gai_error_ok_u_8869 - soft getaddrinfo success / EAI ok code.
 *
 * Always returns 0. Catalog tag only; does not call getaddrinfo or
 * inspect a live gai error. No parent wires.
 */
uint32_t
gj_gai_error_ok_u_8869(void)
{
	(void)NULL;
	return b8869_error_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_gai_error_ok_u_8869(void)
    __attribute__((alias("gj_gai_error_ok_u_8869")));

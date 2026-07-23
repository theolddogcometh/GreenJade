/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8862: getaddrinfo AI_CANONNAME flag id stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_ai_canonname_id_8862(void);
 *     - Return soft AI_CANONNAME hint flag bit (0x0002).
 *   uint32_t __gj_ai_canonname_id_8862  (alias)
 *   __libcgj_batch8862_marker = "libcgj-batch8862"
 *
 * Exclusive continuum CREATE-ONLY (8861-8870: getaddrinfo flag id stubs —
 * passive_id, canonname_id, numeric_host_id, numeric_serv_id, v4mapped_id,
 * all_id, addrconfig_id, gai_ready_u, gai_error_ok_u, batch_id_8870).
 * Unique gj_ai_canonname_id_8862 surface only; no multi-def. Soft pure-data
 * catalog only; does not call getaddrinfo. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8862_marker[] = "libcgj-batch8862";

/* Soft getaddrinfo hint: AI_CANONNAME (request canonical name). */
#define B8862_AI_CANONNAME  ((uint32_t)0x0002u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8862_canonname_id(void)
{
	return B8862_AI_CANONNAME;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_ai_canonname_id_8862 - soft AI_CANONNAME getaddrinfo flag id.
 *
 * Always returns 0x0002. Catalog id only; does not call getaddrinfo.
 * No parent wires.
 */
uint32_t
gj_ai_canonname_id_8862(void)
{
	(void)NULL;
	return b8862_canonname_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_ai_canonname_id_8862(void)
    __attribute__((alias("gj_ai_canonname_id_8862")));

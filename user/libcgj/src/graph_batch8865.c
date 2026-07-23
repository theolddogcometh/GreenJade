/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8865: getaddrinfo AI_V4MAPPED flag id stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_ai_v4mapped_id_8865(void);
 *     - Return soft AI_V4MAPPED hint flag bit (0x0008).
 *   uint32_t __gj_ai_v4mapped_id_8865  (alias)
 *   __libcgj_batch8865_marker = "libcgj-batch8865"
 *
 * Exclusive continuum CREATE-ONLY (8861-8870: getaddrinfo flag id stubs —
 * passive_id, canonname_id, numeric_host_id, numeric_serv_id, v4mapped_id,
 * all_id, addrconfig_id, gai_ready_u, gai_error_ok_u, batch_id_8870).
 * Unique gj_ai_v4mapped_id_8865 surface only; no multi-def. Soft pure-data
 * catalog only; does not call getaddrinfo. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8865_marker[] = "libcgj-batch8865";

/* Soft getaddrinfo hint: AI_V4MAPPED (IPv4-mapped IPv6). */
#define B8865_AI_V4MAPPED  ((uint32_t)0x0008u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8865_v4mapped_id(void)
{
	return B8865_AI_V4MAPPED;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_ai_v4mapped_id_8865 - soft AI_V4MAPPED getaddrinfo flag id.
 *
 * Always returns 0x0008. Catalog id only; does not call getaddrinfo.
 * No parent wires.
 */
uint32_t
gj_ai_v4mapped_id_8865(void)
{
	(void)NULL;
	return b8865_v4mapped_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_ai_v4mapped_id_8865(void)
    __attribute__((alias("gj_ai_v4mapped_id_8865")));

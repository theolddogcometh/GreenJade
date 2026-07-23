/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8864: getaddrinfo AI_NUMERICSERV flag id stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_ai_numeric_serv_id_8864(void);
 *     - Return soft AI_NUMERICSERV hint flag bit (0x0400).
 *   uint32_t __gj_ai_numeric_serv_id_8864  (alias)
 *   __libcgj_batch8864_marker = "libcgj-batch8864"
 *
 * Exclusive continuum CREATE-ONLY (8861-8870: getaddrinfo flag id stubs —
 * passive_id, canonname_id, numeric_host_id, numeric_serv_id, v4mapped_id,
 * all_id, addrconfig_id, gai_ready_u, gai_error_ok_u, batch_id_8870).
 * Unique gj_ai_numeric_serv_id_8864 surface only; no multi-def. Soft
 * pure-data catalog only; does not call getaddrinfo. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8864_marker[] = "libcgj-batch8864";

/* Soft getaddrinfo hint: AI_NUMERICSERV (service as numeric port). */
#define B8864_AI_NUMERICSERV  ((uint32_t)0x0400u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8864_numeric_serv_id(void)
{
	return B8864_AI_NUMERICSERV;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_ai_numeric_serv_id_8864 - soft AI_NUMERICSERV getaddrinfo flag id.
 *
 * Always returns 0x0400. Catalog id only; does not call getaddrinfo.
 * No parent wires.
 */
uint32_t
gj_ai_numeric_serv_id_8864(void)
{
	(void)NULL;
	return b8864_numeric_serv_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_ai_numeric_serv_id_8864(void)
    __attribute__((alias("gj_ai_numeric_serv_id_8864")));

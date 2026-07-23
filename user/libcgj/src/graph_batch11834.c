/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch11834: X compose soft identity stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_compose_ok_u_11834(void);
 *     - Returns 0 (soft: compose sequence path not ready at compile time).
 *   uint32_t __gj_compose_ok_u_11834  (alias)
 *   __libcgj_batch11834_marker = "libcgj-batch11834"
 *
 * Exclusive CREATE-ONLY wave 11831-11840 (ibus/fcitx soft id stubs).
 * Unique gj_compose_ok_u_11834 surface only; no multi-def. No parent
 * wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch11834_marker[] = "libcgj-batch11834";

/* Soft compose ok lamp: not ready at compile time. */
#define B11834_COMPOSE_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b11834_ok(void)
{
	return B11834_COMPOSE_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_compose_ok_u_11834 - soft X compose sequence ok lamp.
 *
 * Always returns 0 (not ready at compile time). Does not read
 * Compose files or call libc. No parent wires.
 */
uint32_t
gj_compose_ok_u_11834(void)
{
	(void)NULL;
	return b11834_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_compose_ok_u_11834(void)
    __attribute__((alias("gj_compose_ok_u_11834")));

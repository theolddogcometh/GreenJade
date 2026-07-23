/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch11034: X compose soft identity stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_compose_ok_u_11034(void);
 *     - Returns 0 (soft: compose sequence path not ready at compile time).
 *   uint32_t __gj_compose_ok_u_11034  (alias)
 *   __libcgj_batch11034_marker = "libcgj-batch11034"
 *
 * Exclusive CREATE-ONLY wave 11031-11040 (ibus/fcitx soft id stubs).
 * Unique gj_compose_ok_u_11034 surface only; no multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch11034_marker[] = "libcgj-batch11034";

/* Soft compose ok lamp: not ready at compile time. */
#define B11034_COMPOSE_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b11034_ok(void)
{
	return B11034_COMPOSE_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_compose_ok_u_11034 - soft X compose sequence ok lamp.
 *
 * Always returns 0 (not ready at compile time). Does not probe compose
 * tables or call libc. No parent wires.
 */
uint32_t
gj_compose_ok_u_11034(void)
{
	(void)NULL;
	return b11034_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_compose_ok_u_11034(void)
    __attribute__((alias("gj_compose_ok_u_11034")));

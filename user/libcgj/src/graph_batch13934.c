/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch13934: X compose soft identity stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_compose_ok_u_13934(void);
 *     - Returns 0 (soft: compose sequence path not ready at compile time).
 *   uint32_t __gj_compose_ok_u_13934  (alias)
 *   __libcgj_batch13934_marker = "libcgj-batch13934"
 *
 * Exclusive CREATE-ONLY wave 13931-13940 (ibus/fcitx soft id stubs).
 * Unique gj_compose_ok_u_13934 surface only; no multi-def. No parent
 * wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch13934_marker[] = "libcgj-batch13934";

/* Soft compose ok lamp: not ready at compile time. */
#define B13934_COMPOSE_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b13934_ok(void)
{
	return B13934_COMPOSE_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_compose_ok_u_13934 - soft X compose sequence ok lamp.
 *
 * Always returns 0 (not ready at compile time). Does not read
 * Compose files or call libc. No parent wires.
 */
uint32_t
gj_compose_ok_u_13934(void)
{
	(void)NULL;
	return b13934_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_compose_ok_u_13934(void)
    __attribute__((alias("gj_compose_ok_u_13934")));

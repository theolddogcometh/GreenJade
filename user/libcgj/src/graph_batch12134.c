/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch12134: X compose soft identity stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_compose_ok_u_12134(void);
 *     - Returns 0 (soft: compose sequence path not ready at compile time).
 *   uint32_t __gj_compose_ok_u_12134  (alias)
 *   __libcgj_batch12134_marker = "libcgj-batch12134"
 *
 * Exclusive CREATE-ONLY wave 12131-12140 (IM soft: ibus/fcitx soft id
 * stubs — ready→1 others 0). Unique gj_compose_ok_u_12134 surface only;
 * no multi-def. Distinct from gj_compose_ok_u_11834 (batch11834),
 * gj_compose_ok_u_11634 (batch11634), and sibling IM soft stubs in this
 * wave. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch12134_marker[] = "libcgj-batch12134";

/* Soft compose ok lamp: not ready at compile time. */
#define B12134_COMPOSE_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b12134_ok(void)
{
	return B12134_COMPOSE_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_compose_ok_u_12134 - soft X compose sequence ok lamp.
 *
 * Always returns 0 (not ready at compile time). Does not read
 * Compose files or call libc. No parent wires.
 */
uint32_t
gj_compose_ok_u_12134(void)
{
	(void)NULL;
	return b12134_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_compose_ok_u_12134(void)
    __attribute__((alias("gj_compose_ok_u_12134")));

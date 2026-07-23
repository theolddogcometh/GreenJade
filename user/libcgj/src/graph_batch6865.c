/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6865: mount_attr attr_set soft apply.
 *
 * Surface (unique symbols):
 *   uint32_t gj_mount_attr_set_6865(uint32_t cur, uint32_t set);
 *     - Return cur | set (soft attr_set: enable listed attr bits).
 *   uint32_t __gj_mount_attr_set_6865  (alias)
 *   __libcgj_batch6865_marker = "libcgj-batch6865"
 *
 * Exclusive continuum CREATE-ONLY (6861-6870: mount_setattr attr stubs).
 * Unique gj_mount_attr_set_6865 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6865_marker[] = "libcgj-batch6865";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6865_set(uint32_t u32Cur, uint32_t u32Set)
{
	return u32Cur | u32Set;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_mount_attr_set_6865 - soft attr_set apply (enable bits).
 *
 * cur: soft current mount attr bitmask (low 32 of attr_set style)
 * set: bits to enable (mirror of mount_attr.attr_set field intent)
 *
 * Returns cur | set. Soft pure-data op; does not call mount_setattr.
 */
uint32_t
gj_mount_attr_set_6865(uint32_t u32Cur, uint32_t u32Set)
{
	(void)NULL;
	return b6865_set(u32Cur, u32Set);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_mount_attr_set_6865(uint32_t u32Cur, uint32_t u32Set)
    __attribute__((alias("gj_mount_attr_set_6865")));

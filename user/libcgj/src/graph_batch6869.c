/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6869: mount_attr propagation soft constants.
 *
 * Surface (unique symbols):
 *   uint32_t gj_mount_attr_prop_unbindable_6869(void);
 *     - Returns 0x20000 (soft MS_UNBINDABLE-style propagation).
 *   uint32_t gj_mount_attr_prop_private_6869(void);
 *     - Returns 0x40000 (soft MS_PRIVATE-style propagation).
 *   uint32_t gj_mount_attr_prop_slave_6869(void);
 *     - Returns 0x80000 (soft MS_SLAVE-style propagation).
 *   uint32_t gj_mount_attr_prop_shared_6869(void);
 *     - Returns 0x100000 (soft MS_SHARED-style propagation).
 *   uint32_t gj_mount_attr_at_recursive_6869(void);
 *     - Returns 0x8000 (soft AT_RECURSIVE for mount_setattr flags).
 *   uint32_t __gj_mount_attr_*_6869  (aliases)
 *   __libcgj_batch6869_marker = "libcgj-batch6869"
 *
 * Exclusive continuum CREATE-ONLY (6861-6870: mount_setattr attr stubs).
 * Unique gj_mount_attr_*_6869 surfaces only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6869_marker[] = "libcgj-batch6869";

/*
 * Soft propagation values (mirror mount_attr.propagation / MS_*):
 *   UNBINDABLE 1<<17, PRIVATE 1<<18, SLAVE 1<<19, SHARED 1<<20
 * Soft AT_RECURSIVE (1<<15) for mount_setattr path flags.
 */
#define B6869_UNBINDABLE  0x00020000u
#define B6869_PRIVATE     0x00040000u
#define B6869_SLAVE       0x00080000u
#define B6869_SHARED      0x00100000u
#define B6869_AT_RECURSE  0x00008000u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6869_unbindable(void)
{
	return B6869_UNBINDABLE;
}

static uint32_t
b6869_private(void)
{
	return B6869_PRIVATE;
}

static uint32_t
b6869_slave(void)
{
	return B6869_SLAVE;
}

static uint32_t
b6869_shared(void)
{
	return B6869_SHARED;
}

static uint32_t
b6869_at_recursive(void)
{
	return B6869_AT_RECURSE;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_mount_attr_prop_unbindable_6869 - soft MS_UNBINDABLE propagation.
 *
 * Always returns 0x20000. Soft pure-data tag; no mount_setattr call.
 */
uint32_t
gj_mount_attr_prop_unbindable_6869(void)
{
	(void)NULL;
	return b6869_unbindable();
}

/*
 * gj_mount_attr_prop_private_6869 - soft MS_PRIVATE propagation.
 *
 * Always returns 0x40000.
 */
uint32_t
gj_mount_attr_prop_private_6869(void)
{
	return b6869_private();
}

/*
 * gj_mount_attr_prop_slave_6869 - soft MS_SLAVE propagation.
 *
 * Always returns 0x80000.
 */
uint32_t
gj_mount_attr_prop_slave_6869(void)
{
	return b6869_slave();
}

/*
 * gj_mount_attr_prop_shared_6869 - soft MS_SHARED propagation.
 *
 * Always returns 0x100000.
 */
uint32_t
gj_mount_attr_prop_shared_6869(void)
{
	return b6869_shared();
}

/*
 * gj_mount_attr_at_recursive_6869 - soft AT_RECURSIVE flag bit.
 *
 * Always returns 0x8000. Soft mount_setattr path-flags tag only.
 */
uint32_t
gj_mount_attr_at_recursive_6869(void)
{
	return b6869_at_recursive();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_mount_attr_prop_unbindable_6869(void)
    __attribute__((alias("gj_mount_attr_prop_unbindable_6869")));

uint32_t __gj_mount_attr_prop_private_6869(void)
    __attribute__((alias("gj_mount_attr_prop_private_6869")));

uint32_t __gj_mount_attr_prop_slave_6869(void)
    __attribute__((alias("gj_mount_attr_prop_slave_6869")));

uint32_t __gj_mount_attr_prop_shared_6869(void)
    __attribute__((alias("gj_mount_attr_prop_shared_6869")));

uint32_t __gj_mount_attr_at_recursive_6869(void)
    __attribute__((alias("gj_mount_attr_at_recursive_6869")));

/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6755: Landlock NET_PORT rule type id stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_ll_rule_type_net_id_6755(void);
 *     - Return LANDLOCK_RULE_NET_PORT constant (2).
 *   uint32_t __gj_ll_rule_type_net_id_6755  (alias)
 *   __libcgj_batch6755_marker = "libcgj-batch6755"
 *
 * Exclusive continuum CREATE-ONLY (6751-6760: landlock rule type stubs —
 * path_beneath, net_port, type_ok, path_id, net_id, pack, needs_path_attr,
 * needs_net_attr, errorish, batch_id_6760). Unique
 * gj_ll_rule_type_net_id_6755 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6755_marker[] = "libcgj-batch6755";

/* LANDLOCK_RULE_NET_PORT = 2 */
#define B6755_LL_RULE_NET_PORT ((uint32_t)2u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6755_ll_rule_type_net_id(void)
{
	return B6755_LL_RULE_NET_PORT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_ll_rule_type_net_id_6755 - compile-time NET_PORT rule type id.
 *
 * Always returns 2 (LANDLOCK_RULE_NET_PORT). Soft UAPI constant
 * export for freestanding graph probes. No parent wires.
 */
uint32_t
gj_ll_rule_type_net_id_6755(void)
{
	(void)NULL;
	return b6755_ll_rule_type_net_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_ll_rule_type_net_id_6755(void)
    __attribute__((alias("gj_ll_rule_type_net_id_6755")));

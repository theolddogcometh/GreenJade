/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6758: Landlock rule type needs net_port attr.
 *
 * Surface (unique symbols):
 *   uint32_t gj_ll_rule_type_needs_net_attr_u_6758(uint32_t rule_type);
 *     - Return 1 if rule_type requires landlock_net_port_attr, else 0.
 *   uint32_t __gj_ll_rule_type_needs_net_attr_u_6758  (alias)
 *   __libcgj_batch6758_marker = "libcgj-batch6758"
 *
 * Exclusive continuum CREATE-ONLY (6751-6760: landlock rule type stubs —
 * path_beneath, net_port, type_ok, path_id, net_id, pack, needs_path_attr,
 * needs_net_attr, errorish, batch_id_6760). Unique
 * gj_ll_rule_type_needs_net_attr_u_6758 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6758_marker[] = "libcgj-batch6758";

/* LANDLOCK_RULE_NET_PORT uses struct landlock_net_port_attr. */
#define B6758_LL_RULE_NET_PORT ((uint32_t)2u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6758_ll_rule_type_needs_net_attr(uint32_t u32RuleType)
{
	return (u32RuleType == B6758_LL_RULE_NET_PORT) ? 1u : 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_ll_rule_type_needs_net_attr_u_6758 - 1 if type uses net_port_attr.
 *
 * rule_type: landlock_add_rule rule_type argument
 *
 * Returns 1 for LANDLOCK_RULE_NET_PORT, else 0.
 * Soft attr-dispatch helper; no parent wires.
 */
uint32_t
gj_ll_rule_type_needs_net_attr_u_6758(uint32_t u32RuleType)
{
	(void)NULL;
	return b6758_ll_rule_type_needs_net_attr(u32RuleType);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_ll_rule_type_needs_net_attr_u_6758(uint32_t u32RuleType)
    __attribute__((alias("gj_ll_rule_type_needs_net_attr_u_6758")));

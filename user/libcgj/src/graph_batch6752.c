/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6752: Landlock LANDLOCK_RULE_NET_PORT type stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_ll_rule_net_port_u_6752(uint32_t rule_type);
 *     - Return 1 if rule_type is LANDLOCK_RULE_NET_PORT (2), else 0.
 *   uint32_t __gj_ll_rule_net_port_u_6752  (alias)
 *   __libcgj_batch6752_marker = "libcgj-batch6752"
 *
 * Exclusive continuum CREATE-ONLY (6751-6760: landlock rule type stubs —
 * path_beneath, net_port, type_ok, path_id, net_id, pack, needs_path_attr,
 * needs_net_attr, errorish, batch_id_6760). Unique
 * gj_ll_rule_net_port_u_6752 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6752_marker[] = "libcgj-batch6752";

/* LANDLOCK_RULE_NET_PORT: landlock_net_port_attr rule (value 2). */
#define B6752_LL_RULE_NET_PORT ((uint32_t)2u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6752_ll_rule_net_port(uint32_t u32RuleType)
{
	return (u32RuleType == B6752_LL_RULE_NET_PORT) ? 1u : 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_ll_rule_net_port_u_6752 - 1 if rule_type is NET_PORT.
 *
 * rule_type: landlock_add_rule rule_type argument
 *
 * Returns 1 when rule_type equals LANDLOCK_RULE_NET_PORT (2), else 0.
 * Pure integer compare; no parent wires.
 */
uint32_t
gj_ll_rule_net_port_u_6752(uint32_t u32RuleType)
{
	(void)NULL;
	return b6752_ll_rule_net_port(u32RuleType);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_ll_rule_net_port_u_6752(uint32_t u32RuleType)
    __attribute__((alias("gj_ll_rule_net_port_u_6752")));

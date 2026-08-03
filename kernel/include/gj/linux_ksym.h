/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Soft Linux-style kernel symbol table for resolving .ko undefined symbols.
 * Clean-room dual MIT OR Apache-2.0 only. Soft ≠ product Linux ABI completeness.
 * Not Linux source / not GPL. Stubs are no-ops for soft module-path development.
 *
 * Greppable (implementation in kernel/mm/linux_ksym.c):
 *   linux_ksym: soft init PASS
 *   linux_ksym: soft inventory n=…
 */
#pragma once

#include <gj/types.h>

/** Fixed soft table capacity (name → address slots). */
#define LINUX_KSYM_MAX 1024u

/**
 * Initialize table and register starter soft stubs.
 * Idempotent. Emits: linux_ksym: soft init PASS
 */
void linux_ksym_init(void);

/**
 * Register (or replace) a symbol by name.
 * @return 0 on success; -1 if name/addr NULL, empty name, or table full
 *         (replace of an existing name always succeeds).
 */
int linux_ksym_register(const char *szName, void *pAddr);

/**
 * Lookup symbol address by name.
 * @return address, or NULL if missing / not inited.
 */
void *linux_ksym_lookup(const char *szName);

/** Number of registered entries. */
u32 linux_ksym_count(void);

/**
 * Greppable soft inventory once per boot.
 * Emits: linux_ksym: soft inventory n=…
 */
void linux_ksym_soft_inventory(void);

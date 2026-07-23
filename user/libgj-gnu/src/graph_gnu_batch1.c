/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Product SO surface (libgj-gnu): GNU string-extension batch 1.
 *
 * Freestanding pure C. Dual-licensed MIT OR Apache-2.0.
 * Intended for the parent Makefile to link into libgj-gnu.so.1 (GNU-hash
 * product SO). Not linked today — libgj_gnu.c alone is the shipped SO.
 * Does NOT touch libcgj.
 *
 * Ownership policy: only MISSING GNU-extension symbols are defined in this
 * tree. Candidate symbols that already live in user/libcgj must not be
 * redefined here. This TU therefore exports a batch marker only until a
 * missing symbol is assigned to this tree.
 *
 * Marker symbols:
 *   gj_gnu_batch1_export — data (0x471); optional future resolve target
 *   gj_gnu_batch1_init   — restores the marker
 *   gj_gnu_batch1_id     — returns current marker (keeps TU -Werror clean)
 */
#include <stddef.h>
#include <stdint.h>

/* Product SO batch presence marker (GLOB_DAT / JUMP_SLOT smoke target). */
volatile uint64_t gj_gnu_batch1_export = 0x471;

void
gj_gnu_batch1_init(void)
{
    gj_gnu_batch1_export = 0x471;
}

/* Keep TU useful under -Werror -Wunused when only the marker is present. */
uint64_t
gj_gnu_batch1_id(void)
{
    return gj_gnu_batch1_export;
}

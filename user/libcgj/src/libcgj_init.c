/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Shared library load marker (soname libc.so.6 product path).
 */
#include <stdint.h>

extern void _libcgj_stdio_init(void);

/* Export a stable marker symbol for dynlink smokes */
volatile uint64_t gj_libc_marker = 0x43474a4c434942u; /* "CGLIB" ish */

void
_libcgj_load(void)
{
    _libcgj_stdio_init();
}

/* DT_INIT-style constructor when loaded as SO (if linker calls .init) */
void
_init(void)
{
    _libcgj_stdio_init();
}


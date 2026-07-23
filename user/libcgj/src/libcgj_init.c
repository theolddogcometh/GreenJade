/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Shared library load marker (soname libc.so.6 product path).
 *
 * greppable: CGJ_INIT_SOFT_ONCE
 * greppable: CGJ_INIT_SOFT_MARKER
 *
 * Soft deepen: one-shot stdio bring-up from _libcgj_load / _init so
 * repeated DT_INIT or explicit load calls stay idempotent.
 */
#include <stdint.h>

extern void _libcgj_stdio_init(void);

/* Export a stable marker symbol for dynlink smokes */
/* greppable: CGJ_INIT_SOFT_MARKER */
volatile uint64_t gj_libc_marker = 0x43474a4c434942u; /* "CGLIB" ish */

static int g_fLibcgjInitDone;

static void
libcgj_soft_init_once(void)
{
    /* greppable: CGJ_INIT_SOFT_ONCE */
    if (g_fLibcgjInitDone) {
        return;
    }
    _libcgj_stdio_init();
    g_fLibcgjInitDone = 1;
}

void
_libcgj_load(void)
{
    libcgj_soft_init_once();
}

/* DT_INIT-style constructor when loaded as SO (if linker calls .init) */
void
_init(void)
{
    libcgj_soft_init_once();
}

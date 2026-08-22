/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Freestanding alloca for vendored dash (parser.c). Dual MIT OR Apache-2.0.
 */
#ifndef DASH_GJ_ALLOCA_H
#define DASH_GJ_ALLOCA_H

#undef alloca
#define alloca(sz) __builtin_alloca((sz))

#endif /* DASH_GJ_ALLOCA_H */

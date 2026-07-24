/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Soft host gates for continuum MILESTONE 78300 markers (CREATE-ONLY).
 * Coherent soft package: this TU + NOTES_milestone_78300.txt + template.
 * Mirror 25800 soft pattern; bar3 still open (gj_bar3_ready_78300 == 0).
 *
 * Manual: cc -std=c11 -Wall -Wextra -O2 -o build/cgj_soft_milestone_78300 \
 *   user/libcgj/tests/cgj_soft_milestone_78300.c -ldl
 * Direct: -DCGJ_SOFT_M78300_DIRECT + graph_batch78291.c … graph_batch78300.c
 */

#define _GNU_SOURCE
#if defined(CGJ_SOFT_M78300_DIRECT)
#define CGJ_SOFT_IS_DIRECT 1
#else
#define CGJ_SOFT_IS_DIRECT 0
#endif
#define CGJ_SOFT_M 78300
#define CGJ_SOFT_B0 78291
#define CGJ_SOFT_B1 78292
#define CGJ_SOFT_B2 78293
#define CGJ_SOFT_B3 78294
#define CGJ_SOFT_B4 78295
#define CGJ_SOFT_B5 78296
#define CGJ_SOFT_B6 78297
#define CGJ_SOFT_B7 78298
#define CGJ_SOFT_B8 78299
#include "_cgj_soft_m_template.inc"

/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Soft host gates for continuum MILESTONE 79300 markers (CREATE-ONLY).
 * Coherent soft package: this TU + NOTES_milestone_79300.txt + template.
 * Mirror 25800 soft pattern; bar3 still open (gj_bar3_ready_79300 == 0).
 *
 * Manual: cc -std=c11 -Wall -Wextra -O2 -o build/cgj_soft_milestone_79300 \
 *   user/libcgj/tests/cgj_soft_milestone_79300.c -ldl
 * Direct: -DCGJ_SOFT_M79300_DIRECT + graph_batch79291.c … graph_batch79300.c
 */

#define _GNU_SOURCE
#if defined(CGJ_SOFT_M79300_DIRECT)
#define CGJ_SOFT_IS_DIRECT 1
#else
#define CGJ_SOFT_IS_DIRECT 0
#endif
#define CGJ_SOFT_M 79300
#define CGJ_SOFT_B0 79291
#define CGJ_SOFT_B1 79292
#define CGJ_SOFT_B2 79293
#define CGJ_SOFT_B3 79294
#define CGJ_SOFT_B4 79295
#define CGJ_SOFT_B5 79296
#define CGJ_SOFT_B6 79297
#define CGJ_SOFT_B7 79298
#define CGJ_SOFT_B8 79299
#include "_cgj_soft_m_template.inc"

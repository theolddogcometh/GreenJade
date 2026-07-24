/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Soft host gates for continuum MILESTONE 77200 markers (CREATE-ONLY).
 * Coherent soft package: this TU + NOTES_milestone_77200.txt + template.
 * Mirror 25800 soft pattern; bar3 still open (gj_bar3_ready_77200 == 0).
 *
 * Manual: cc -std=c11 -Wall -Wextra -O2 -o build/cgj_soft_milestone_77200 \
 *   user/libcgj/tests/cgj_soft_milestone_77200.c -ldl
 * Direct: -DCGJ_SOFT_M77200_DIRECT + graph_batch77191.c … graph_batch77200.c
 */

#define _GNU_SOURCE
#if defined(CGJ_SOFT_M77200_DIRECT)
#define CGJ_SOFT_IS_DIRECT 1
#else
#define CGJ_SOFT_IS_DIRECT 0
#endif
#define CGJ_SOFT_M 77200
#define CGJ_SOFT_B0 77191
#define CGJ_SOFT_B1 77192
#define CGJ_SOFT_B2 77193
#define CGJ_SOFT_B3 77194
#define CGJ_SOFT_B4 77195
#define CGJ_SOFT_B5 77196
#define CGJ_SOFT_B6 77197
#define CGJ_SOFT_B7 77198
#define CGJ_SOFT_B8 77199
#include "_cgj_soft_m_template.inc"

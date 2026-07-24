/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Soft host gates for continuum MILESTONE 78200 markers (CREATE-ONLY).
 * Coherent soft package: this TU + NOTES_milestone_78200.txt + template.
 * Mirror 25800 soft pattern; bar3 still open (gj_bar3_ready_78200 == 0).
 *
 * Manual: cc -std=c11 -Wall -Wextra -O2 -o build/cgj_soft_milestone_78200 \
 *   user/libcgj/tests/cgj_soft_milestone_78200.c -ldl
 * Direct: -DCGJ_SOFT_M78200_DIRECT + graph_batch78191.c … graph_batch78200.c
 */

#define _GNU_SOURCE
#if defined(CGJ_SOFT_M78200_DIRECT)
#define CGJ_SOFT_IS_DIRECT 1
#else
#define CGJ_SOFT_IS_DIRECT 0
#endif
#define CGJ_SOFT_M 78200
#define CGJ_SOFT_B0 78191
#define CGJ_SOFT_B1 78192
#define CGJ_SOFT_B2 78193
#define CGJ_SOFT_B3 78194
#define CGJ_SOFT_B4 78195
#define CGJ_SOFT_B5 78196
#define CGJ_SOFT_B6 78197
#define CGJ_SOFT_B7 78198
#define CGJ_SOFT_B8 78199
#include "_cgj_soft_m_template.inc"

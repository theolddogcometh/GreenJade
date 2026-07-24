/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Soft host gates for continuum MILESTONE 89100 markers (CREATE-ONLY).
 * Coherent soft package: this TU + NOTES_milestone_89100.txt + template.
 * Mirror 25800 soft pattern; bar3 still open (gj_bar3_ready_89100 == 0).
 *
 * Manual: cc -std=c11 -Wall -Wextra -O2 -o build/cgj_soft_milestone_89100 \
 *   user/libcgj/tests/cgj_soft_milestone_89100.c -ldl
 * Direct: -DCGJ_SOFT_M89100_DIRECT + graph_batch89091.c … graph_batch89100.c
 */

#define _GNU_SOURCE
#if defined(CGJ_SOFT_M89100_DIRECT)
#define CGJ_SOFT_IS_DIRECT 1
#else
#define CGJ_SOFT_IS_DIRECT 0
#endif
#define CGJ_SOFT_M 89100
#define CGJ_SOFT_B0 89091
#define CGJ_SOFT_B1 89092
#define CGJ_SOFT_B2 89093
#define CGJ_SOFT_B3 89094
#define CGJ_SOFT_B4 89095
#define CGJ_SOFT_B5 89096
#define CGJ_SOFT_B6 89097
#define CGJ_SOFT_B7 89098
#define CGJ_SOFT_B8 89099
#include "_cgj_soft_m_template.inc"

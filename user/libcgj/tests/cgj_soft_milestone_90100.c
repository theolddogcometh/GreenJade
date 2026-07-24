/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Soft host gates for continuum MILESTONE 90100 markers (CREATE-ONLY).
 * Coherent soft package: this TU + NOTES_milestone_90100.txt + template.
 * Mirror 25800 soft pattern; bar3 still open (gj_bar3_ready_90100 == 0).
 *
 * Manual: cc -std=c11 -Wall -Wextra -O2 -o build/cgj_soft_milestone_90100 \
 *   user/libcgj/tests/cgj_soft_milestone_90100.c -ldl
 * Direct: -DCGJ_SOFT_M90100_DIRECT + graph_batch90091.c … graph_batch90100.c
 */

#define _GNU_SOURCE
#if defined(CGJ_SOFT_M90100_DIRECT)
#define CGJ_SOFT_IS_DIRECT 1
#else
#define CGJ_SOFT_IS_DIRECT 0
#endif
#define CGJ_SOFT_M 90100
#define CGJ_SOFT_B0 90091
#define CGJ_SOFT_B1 90092
#define CGJ_SOFT_B2 90093
#define CGJ_SOFT_B3 90094
#define CGJ_SOFT_B4 90095
#define CGJ_SOFT_B5 90096
#define CGJ_SOFT_B6 90097
#define CGJ_SOFT_B7 90098
#define CGJ_SOFT_B8 90099
#include "_cgj_soft_m_template.inc"

/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Soft host gates for continuum MILESTONE 79100 markers (CREATE-ONLY).
 * Coherent soft package: this TU + NOTES_milestone_79100.txt + template.
 * Mirror 25800 soft pattern; bar3 still open (gj_bar3_ready_79100 == 0).
 *
 * Manual: cc -std=c11 -Wall -Wextra -O2 -o build/cgj_soft_milestone_79100 \
 *   user/libcgj/tests/cgj_soft_milestone_79100.c -ldl
 * Direct: -DCGJ_SOFT_M79100_DIRECT + graph_batch79091.c … graph_batch79100.c
 */

#define _GNU_SOURCE
#if defined(CGJ_SOFT_M79100_DIRECT)
#define CGJ_SOFT_IS_DIRECT 1
#else
#define CGJ_SOFT_IS_DIRECT 0
#endif
#define CGJ_SOFT_M 79100
#define CGJ_SOFT_B0 79091
#define CGJ_SOFT_B1 79092
#define CGJ_SOFT_B2 79093
#define CGJ_SOFT_B3 79094
#define CGJ_SOFT_B4 79095
#define CGJ_SOFT_B5 79096
#define CGJ_SOFT_B6 79097
#define CGJ_SOFT_B7 79098
#define CGJ_SOFT_B8 79099
#include "_cgj_soft_m_template.inc"

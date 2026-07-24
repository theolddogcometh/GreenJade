/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Soft host gates for continuum MILESTONE 79700 markers (CREATE-ONLY).
 * Coherent soft package: this TU + NOTES_milestone_79700.txt + template.
 * Mirror 25800 soft pattern; bar3 still open (gj_bar3_ready_79700 == 0).
 *
 * Manual: cc -std=c11 -Wall -Wextra -O2 -o build/cgj_soft_milestone_79700 \
 *   user/libcgj/tests/cgj_soft_milestone_79700.c -ldl
 * Direct: -DCGJ_SOFT_M79700_DIRECT + graph_batch79691.c … graph_batch79700.c
 */

#define _GNU_SOURCE
#if defined(CGJ_SOFT_M79700_DIRECT)
#define CGJ_SOFT_IS_DIRECT 1
#else
#define CGJ_SOFT_IS_DIRECT 0
#endif
#define CGJ_SOFT_M 79700
#define CGJ_SOFT_B0 79691
#define CGJ_SOFT_B1 79692
#define CGJ_SOFT_B2 79693
#define CGJ_SOFT_B3 79694
#define CGJ_SOFT_B4 79695
#define CGJ_SOFT_B5 79696
#define CGJ_SOFT_B6 79697
#define CGJ_SOFT_B7 79698
#define CGJ_SOFT_B8 79699
#include "_cgj_soft_m_template.inc"

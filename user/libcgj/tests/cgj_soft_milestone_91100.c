/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Soft host gates for continuum MILESTONE 91100 markers (CREATE-ONLY).
 * Coherent soft package: this TU + NOTES_milestone_91100.txt + template.
 * Mirror 25800 soft pattern; bar3 still open (gj_bar3_ready_91100 == 0).
 *
 * Manual: cc -std=c11 -Wall -Wextra -O2 -o build/cgj_soft_milestone_91100 \
 *   user/libcgj/tests/cgj_soft_milestone_91100.c -ldl
 * Direct: -DCGJ_SOFT_M91100_DIRECT + graph_batch91091.c … graph_batch91100.c
 */

#define _GNU_SOURCE
#if defined(CGJ_SOFT_M91100_DIRECT)
#define CGJ_SOFT_IS_DIRECT 1
#else
#define CGJ_SOFT_IS_DIRECT 0
#endif
#define CGJ_SOFT_M 91100
#define CGJ_SOFT_B0 91091
#define CGJ_SOFT_B1 91092
#define CGJ_SOFT_B2 91093
#define CGJ_SOFT_B3 91094
#define CGJ_SOFT_B4 91095
#define CGJ_SOFT_B5 91096
#define CGJ_SOFT_B6 91097
#define CGJ_SOFT_B7 91098
#define CGJ_SOFT_B8 91099
#include "_cgj_soft_m_template.inc"

/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Soft host gates for continuum MILESTONE 77900 markers (CREATE-ONLY).
 * Coherent soft package: this TU + NOTES_milestone_77900.txt + template.
 * Mirror 25800 soft pattern; bar3 still open (gj_bar3_ready_77900 == 0).
 *
 * Manual: cc -std=c11 -Wall -Wextra -O2 -o build/cgj_soft_milestone_77900 \
 *   user/libcgj/tests/cgj_soft_milestone_77900.c -ldl
 * Direct: -DCGJ_SOFT_M77900_DIRECT + graph_batch77891.c … graph_batch77900.c
 */

#define _GNU_SOURCE
#if defined(CGJ_SOFT_M77900_DIRECT)
#define CGJ_SOFT_IS_DIRECT 1
#else
#define CGJ_SOFT_IS_DIRECT 0
#endif
#define CGJ_SOFT_M 77900
#define CGJ_SOFT_B0 77891
#define CGJ_SOFT_B1 77892
#define CGJ_SOFT_B2 77893
#define CGJ_SOFT_B3 77894
#define CGJ_SOFT_B4 77895
#define CGJ_SOFT_B5 77896
#define CGJ_SOFT_B6 77897
#define CGJ_SOFT_B7 77898
#define CGJ_SOFT_B8 77899
#include "_cgj_soft_m_template.inc"

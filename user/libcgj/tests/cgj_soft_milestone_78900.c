/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Soft host gates for continuum MILESTONE 78900 markers (CREATE-ONLY).
 * Coherent soft package: this TU + NOTES_milestone_78900.txt + template.
 * Mirror 25800 soft pattern; bar3 still open (gj_bar3_ready_78900 == 0).
 *
 * Manual: cc -std=c11 -Wall -Wextra -O2 -o build/cgj_soft_milestone_78900 \
 *   user/libcgj/tests/cgj_soft_milestone_78900.c -ldl
 * Direct: -DCGJ_SOFT_M78900_DIRECT + graph_batch78891.c … graph_batch78900.c
 */

#define _GNU_SOURCE
#if defined(CGJ_SOFT_M78900_DIRECT)
#define CGJ_SOFT_IS_DIRECT 1
#else
#define CGJ_SOFT_IS_DIRECT 0
#endif
#define CGJ_SOFT_M 78900
#define CGJ_SOFT_B0 78891
#define CGJ_SOFT_B1 78892
#define CGJ_SOFT_B2 78893
#define CGJ_SOFT_B3 78894
#define CGJ_SOFT_B4 78895
#define CGJ_SOFT_B5 78896
#define CGJ_SOFT_B6 78897
#define CGJ_SOFT_B7 78898
#define CGJ_SOFT_B8 78899
#include "_cgj_soft_m_template.inc"

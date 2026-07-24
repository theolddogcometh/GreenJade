/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Soft host gates for continuum MILESTONE 89900 markers (CREATE-ONLY).
 * Coherent soft package: this TU + NOTES_milestone_89900.txt + template.
 * Mirror 25800 soft pattern; bar3 still open (gj_bar3_ready_89900 == 0).
 *
 * Manual: cc -std=c11 -Wall -Wextra -O2 -o build/cgj_soft_milestone_89900 \
 *   user/libcgj/tests/cgj_soft_milestone_89900.c -ldl
 * Direct: -DCGJ_SOFT_M89900_DIRECT + graph_batch89891.c … graph_batch89900.c
 */

#define _GNU_SOURCE
#if defined(CGJ_SOFT_M89900_DIRECT)
#define CGJ_SOFT_IS_DIRECT 1
#else
#define CGJ_SOFT_IS_DIRECT 0
#endif
#define CGJ_SOFT_M 89900
#define CGJ_SOFT_B0 89891
#define CGJ_SOFT_B1 89892
#define CGJ_SOFT_B2 89893
#define CGJ_SOFT_B3 89894
#define CGJ_SOFT_B4 89895
#define CGJ_SOFT_B5 89896
#define CGJ_SOFT_B6 89897
#define CGJ_SOFT_B7 89898
#define CGJ_SOFT_B8 89899
#include "_cgj_soft_m_template.inc"

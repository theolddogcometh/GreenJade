/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Soft host gates for continuum MILESTONE 79900 markers (CREATE-ONLY).
 * Coherent soft package: this TU + NOTES_milestone_79900.txt + template.
 * Mirror 25800 soft pattern; bar3 still open (gj_bar3_ready_79900 == 0).
 *
 * Manual: cc -std=c11 -Wall -Wextra -O2 -o build/cgj_soft_milestone_79900 \
 *   user/libcgj/tests/cgj_soft_milestone_79900.c -ldl
 * Direct: -DCGJ_SOFT_M79900_DIRECT + graph_batch79891.c … graph_batch79900.c
 */

#define _GNU_SOURCE
#if defined(CGJ_SOFT_M79900_DIRECT)
#define CGJ_SOFT_IS_DIRECT 1
#else
#define CGJ_SOFT_IS_DIRECT 0
#endif
#define CGJ_SOFT_M 79900
#define CGJ_SOFT_B0 79891
#define CGJ_SOFT_B1 79892
#define CGJ_SOFT_B2 79893
#define CGJ_SOFT_B3 79894
#define CGJ_SOFT_B4 79895
#define CGJ_SOFT_B5 79896
#define CGJ_SOFT_B6 79897
#define CGJ_SOFT_B7 79898
#define CGJ_SOFT_B8 79899
#include "_cgj_soft_m_template.inc"

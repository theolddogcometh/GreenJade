/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Soft host gates for continuum MILESTONE 77100 markers (CREATE-ONLY).
 * Coherent soft package: this TU + NOTES_milestone_77100.txt + template.
 * Mirror 25800 soft pattern; bar3 still open (gj_bar3_ready_77100 == 0).
 *
 * Manual: cc -std=c11 -Wall -Wextra -O2 -o build/cgj_soft_milestone_77100 \
 *   user/libcgj/tests/cgj_soft_milestone_77100.c -ldl
 * Direct: -DCGJ_SOFT_M77100_DIRECT + graph_batch77091.c … graph_batch77100.c
 */

#define _GNU_SOURCE
#if defined(CGJ_SOFT_M77100_DIRECT)
#define CGJ_SOFT_IS_DIRECT 1
#else
#define CGJ_SOFT_IS_DIRECT 0
#endif
#define CGJ_SOFT_M 77100
#define CGJ_SOFT_B0 77091
#define CGJ_SOFT_B1 77092
#define CGJ_SOFT_B2 77093
#define CGJ_SOFT_B3 77094
#define CGJ_SOFT_B4 77095
#define CGJ_SOFT_B5 77096
#define CGJ_SOFT_B6 77097
#define CGJ_SOFT_B7 77098
#define CGJ_SOFT_B8 77099
#include "_cgj_soft_m_template.inc"

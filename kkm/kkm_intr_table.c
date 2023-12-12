// SPDX-License-Identifier: GPL-2.0
/*
 * Kontain Kernel Module
 *
 * This module enables Kontain unikernel in absence of
 * hardware support for virtualization
 *
 * Copyright (C) 2020-2021 Kontain Inc.
 *
 * Authors:
 *  Srinivasa Vetsa <svetsa@kontain.app>
 *
 */

#include <linux/mm.h>
#include <asm/irq_vectors.h>
#include "kkm_guest_exit.h"
#include "kkm_intr.h"
#include "kkm_always.h"

uint64_t intr_function_pointers[NR_VECTORS] = {
	(uint64_t)kkm_intr_entry_0,   (uint64_t)kkm_intr_entry_1,
	(uint64_t)kkm_intr_entry_2,   (uint64_t)kkm_intr_entry_3,
	(uint64_t)kkm_intr_entry_4,   (uint64_t)kkm_intr_entry_5,
	(uint64_t)kkm_intr_entry_6,   (uint64_t)kkm_intr_entry_7,
	(uint64_t)kkm_intr_entry_8,   (uint64_t)kkm_intr_entry_9,
	(uint64_t)kkm_intr_entry_10,  (uint64_t)kkm_intr_entry_11,
	(uint64_t)kkm_intr_entry_12,  (uint64_t)kkm_intr_entry_13,
	(uint64_t)kkm_intr_entry_14,  (uint64_t)kkm_intr_entry_15,
	(uint64_t)kkm_intr_entry_16,  (uint64_t)kkm_intr_entry_17,
	(uint64_t)kkm_intr_entry_18,  (uint64_t)kkm_intr_entry_19,
	(uint64_t)kkm_intr_entry_20,  (uint64_t)kkm_intr_entry_21,
	(uint64_t)kkm_intr_entry_22,  (uint64_t)kkm_intr_entry_23,
	(uint64_t)kkm_intr_entry_24,  (uint64_t)kkm_intr_entry_25,
	(uint64_t)kkm_intr_entry_26,  (uint64_t)kkm_intr_entry_27,
	(uint64_t)kkm_intr_entry_28,  (uint64_t)kkm_intr_entry_29,
	(uint64_t)kkm_intr_entry_30,  (uint64_t)kkm_intr_entry_31,
	(uint64_t)kkm_intr_entry_32,  (uint64_t)kkm_intr_entry_33,
	(uint64_t)kkm_intr_entry_34,  (uint64_t)kkm_intr_entry_35,
	(uint64_t)kkm_intr_entry_36,  (uint64_t)kkm_intr_entry_37,
	(uint64_t)kkm_intr_entry_38,  (uint64_t)kkm_intr_entry_39,
	(uint64_t)kkm_intr_entry_40,  (uint64_t)kkm_intr_entry_41,
	(uint64_t)kkm_intr_entry_42,  (uint64_t)kkm_intr_entry_43,
	(uint64_t)kkm_intr_entry_44,  (uint64_t)kkm_intr_entry_45,
	(uint64_t)kkm_intr_entry_46,  (uint64_t)kkm_intr_entry_47,
	(uint64_t)kkm_intr_entry_48,  (uint64_t)kkm_intr_entry_49,
	(uint64_t)kkm_intr_entry_50,  (uint64_t)kkm_intr_entry_51,
	(uint64_t)kkm_intr_entry_52,  (uint64_t)kkm_intr_entry_53,
	(uint64_t)kkm_intr_entry_54,  (uint64_t)kkm_intr_entry_55,
	(uint64_t)kkm_intr_entry_56,  (uint64_t)kkm_intr_entry_57,
	(uint64_t)kkm_intr_entry_58,  (uint64_t)kkm_intr_entry_59,
	(uint64_t)kkm_intr_entry_60,  (uint64_t)kkm_intr_entry_61,
	(uint64_t)kkm_intr_entry_62,  (uint64_t)kkm_intr_entry_63,
	(uint64_t)kkm_intr_entry_64,  (uint64_t)kkm_intr_entry_65,
	(uint64_t)kkm_intr_entry_66,  (uint64_t)kkm_intr_entry_67,
	(uint64_t)kkm_intr_entry_68,  (uint64_t)kkm_intr_entry_69,
	(uint64_t)kkm_intr_entry_70,  (uint64_t)kkm_intr_entry_71,
	(uint64_t)kkm_intr_entry_72,  (uint64_t)kkm_intr_entry_73,
	(uint64_t)kkm_intr_entry_74,  (uint64_t)kkm_intr_entry_75,
	(uint64_t)kkm_intr_entry_76,  (uint64_t)kkm_intr_entry_77,
	(uint64_t)kkm_intr_entry_78,  (uint64_t)kkm_intr_entry_79,
	(uint64_t)kkm_intr_entry_80,  (uint64_t)kkm_intr_entry_81,
	(uint64_t)kkm_intr_entry_82,  (uint64_t)kkm_intr_entry_83,
	(uint64_t)kkm_intr_entry_84,  (uint64_t)kkm_intr_entry_85,
	(uint64_t)kkm_intr_entry_86,  (uint64_t)kkm_intr_entry_87,
	(uint64_t)kkm_intr_entry_88,  (uint64_t)kkm_intr_entry_89,
	(uint64_t)kkm_intr_entry_90,  (uint64_t)kkm_intr_entry_91,
	(uint64_t)kkm_intr_entry_92,  (uint64_t)kkm_intr_entry_93,
	(uint64_t)kkm_intr_entry_94,  (uint64_t)kkm_intr_entry_95,
	(uint64_t)kkm_intr_entry_96,  (uint64_t)kkm_intr_entry_97,
	(uint64_t)kkm_intr_entry_98,  (uint64_t)kkm_intr_entry_99,
	(uint64_t)kkm_intr_entry_100, (uint64_t)kkm_intr_entry_101,
	(uint64_t)kkm_intr_entry_102, (uint64_t)kkm_intr_entry_103,
	(uint64_t)kkm_intr_entry_104, (uint64_t)kkm_intr_entry_105,
	(uint64_t)kkm_intr_entry_106, (uint64_t)kkm_intr_entry_107,
	(uint64_t)kkm_intr_entry_108, (uint64_t)kkm_intr_entry_109,
	(uint64_t)kkm_intr_entry_110, (uint64_t)kkm_intr_entry_111,
	(uint64_t)kkm_intr_entry_112, (uint64_t)kkm_intr_entry_113,
	(uint64_t)kkm_intr_entry_114, (uint64_t)kkm_intr_entry_115,
	(uint64_t)kkm_intr_entry_116, (uint64_t)kkm_intr_entry_117,
	(uint64_t)kkm_intr_entry_118, (uint64_t)kkm_intr_entry_119,
	(uint64_t)kkm_intr_entry_120, (uint64_t)kkm_intr_entry_121,
	(uint64_t)kkm_intr_entry_122, (uint64_t)kkm_intr_entry_123,
	(uint64_t)kkm_intr_entry_124, (uint64_t)kkm_intr_entry_125,
	(uint64_t)kkm_intr_entry_126, (uint64_t)kkm_intr_entry_127,
	(uint64_t)kkm_intr_entry_128, (uint64_t)kkm_intr_entry_129,
	(uint64_t)kkm_intr_entry_130, (uint64_t)kkm_intr_entry_131,
	(uint64_t)kkm_intr_entry_132, (uint64_t)kkm_intr_entry_133,
	(uint64_t)kkm_intr_entry_134, (uint64_t)kkm_intr_entry_135,
	(uint64_t)kkm_intr_entry_136, (uint64_t)kkm_intr_entry_137,
	(uint64_t)kkm_intr_entry_138, (uint64_t)kkm_intr_entry_139,
	(uint64_t)kkm_intr_entry_140, (uint64_t)kkm_intr_entry_141,
	(uint64_t)kkm_intr_entry_142, (uint64_t)kkm_intr_entry_143,
	(uint64_t)kkm_intr_entry_144, (uint64_t)kkm_intr_entry_145,
	(uint64_t)kkm_intr_entry_146, (uint64_t)kkm_intr_entry_147,
	(uint64_t)kkm_intr_entry_148, (uint64_t)kkm_intr_entry_149,
	(uint64_t)kkm_intr_entry_150, (uint64_t)kkm_intr_entry_151,
	(uint64_t)kkm_intr_entry_152, (uint64_t)kkm_intr_entry_153,
	(uint64_t)kkm_intr_entry_154, (uint64_t)kkm_intr_entry_155,
	(uint64_t)kkm_intr_entry_156, (uint64_t)kkm_intr_entry_157,
	(uint64_t)kkm_intr_entry_158, (uint64_t)kkm_intr_entry_159,
	(uint64_t)kkm_intr_entry_160, (uint64_t)kkm_intr_entry_161,
	(uint64_t)kkm_intr_entry_162, (uint64_t)kkm_intr_entry_163,
	(uint64_t)kkm_intr_entry_164, (uint64_t)kkm_intr_entry_165,
	(uint64_t)kkm_intr_entry_166, (uint64_t)kkm_intr_entry_167,
	(uint64_t)kkm_intr_entry_168, (uint64_t)kkm_intr_entry_169,
	(uint64_t)kkm_intr_entry_170, (uint64_t)kkm_intr_entry_171,
	(uint64_t)kkm_intr_entry_172, (uint64_t)kkm_intr_entry_173,
	(uint64_t)kkm_intr_entry_174, (uint64_t)kkm_intr_entry_175,
	(uint64_t)kkm_intr_entry_176, (uint64_t)kkm_intr_entry_177,
	(uint64_t)kkm_intr_entry_178, (uint64_t)kkm_intr_entry_179,
	(uint64_t)kkm_intr_entry_180, (uint64_t)kkm_intr_entry_181,
	(uint64_t)kkm_intr_entry_182, (uint64_t)kkm_intr_entry_183,
	(uint64_t)kkm_intr_entry_184, (uint64_t)kkm_intr_entry_185,
	(uint64_t)kkm_intr_entry_186, (uint64_t)kkm_intr_entry_187,
	(uint64_t)kkm_intr_entry_188, (uint64_t)kkm_intr_entry_189,
	(uint64_t)kkm_intr_entry_190, (uint64_t)kkm_intr_entry_191,
	(uint64_t)kkm_intr_entry_192, (uint64_t)kkm_intr_entry_193,
	(uint64_t)kkm_intr_entry_194, (uint64_t)kkm_intr_entry_195,
	(uint64_t)kkm_intr_entry_196, (uint64_t)kkm_intr_entry_197,
	(uint64_t)kkm_intr_entry_198, (uint64_t)kkm_intr_entry_199,
	(uint64_t)kkm_intr_entry_200, (uint64_t)kkm_intr_entry_201,
	(uint64_t)kkm_intr_entry_202, (uint64_t)kkm_intr_entry_203,
	(uint64_t)kkm_intr_entry_204, (uint64_t)kkm_intr_entry_205,
	(uint64_t)kkm_intr_entry_206, (uint64_t)kkm_intr_entry_207,
	(uint64_t)kkm_intr_entry_208, (uint64_t)kkm_intr_entry_209,
	(uint64_t)kkm_intr_entry_210, (uint64_t)kkm_intr_entry_211,
	(uint64_t)kkm_intr_entry_212, (uint64_t)kkm_intr_entry_213,
	(uint64_t)kkm_intr_entry_214, (uint64_t)kkm_intr_entry_215,
	(uint64_t)kkm_intr_entry_216, (uint64_t)kkm_intr_entry_217,
	(uint64_t)kkm_intr_entry_218, (uint64_t)kkm_intr_entry_219,
	(uint64_t)kkm_intr_entry_220, (uint64_t)kkm_intr_entry_221,
	(uint64_t)kkm_intr_entry_222, (uint64_t)kkm_intr_entry_223,
	(uint64_t)kkm_intr_entry_224, (uint64_t)kkm_intr_entry_225,
	(uint64_t)kkm_intr_entry_226, (uint64_t)kkm_intr_entry_227,
	(uint64_t)kkm_intr_entry_228, (uint64_t)kkm_intr_entry_229,
	(uint64_t)kkm_intr_entry_230, (uint64_t)kkm_intr_entry_231,
	(uint64_t)kkm_intr_entry_232, (uint64_t)kkm_intr_entry_233,
	(uint64_t)kkm_intr_entry_234, (uint64_t)kkm_intr_entry_235,
	(uint64_t)kkm_intr_entry_236, (uint64_t)kkm_intr_entry_237,
	(uint64_t)kkm_intr_entry_238, (uint64_t)kkm_intr_entry_239,
	(uint64_t)kkm_intr_entry_240, (uint64_t)kkm_intr_entry_241,
	(uint64_t)kkm_intr_entry_242, (uint64_t)kkm_intr_entry_243,
	(uint64_t)kkm_intr_entry_244, (uint64_t)kkm_intr_entry_245,
	(uint64_t)kkm_intr_entry_246, (uint64_t)kkm_intr_entry_247,
	(uint64_t)kkm_intr_entry_248, (uint64_t)kkm_intr_entry_249,
	(uint64_t)kkm_intr_entry_250, (uint64_t)kkm_intr_entry_251,
	(uint64_t)kkm_intr_entry_252, (uint64_t)kkm_intr_entry_253,
	(uint64_t)kkm_intr_entry_254, (uint64_t)kkm_intr_entry_255,
};

uint64_t intr_forward_pointers[NR_VECTORS] = {
	(uint64_t)kkm_forward_intr_0,	(uint64_t)kkm_forward_intr_1,
	(uint64_t)kkm_forward_intr_2,	(uint64_t)kkm_forward_intr_3,
	(uint64_t)kkm_forward_intr_4,	(uint64_t)kkm_forward_intr_5,
	(uint64_t)kkm_forward_intr_6,	(uint64_t)kkm_forward_intr_7,
	(uint64_t)kkm_forward_intr_8,	(uint64_t)kkm_forward_intr_9,
	(uint64_t)kkm_forward_intr_10,	(uint64_t)kkm_forward_intr_11,
	(uint64_t)kkm_forward_intr_12,	(uint64_t)kkm_forward_intr_13,
	(uint64_t)kkm_forward_intr_14,	(uint64_t)kkm_forward_intr_15,
	(uint64_t)kkm_forward_intr_16,	(uint64_t)kkm_forward_intr_17,
	(uint64_t)kkm_forward_intr_18,	(uint64_t)kkm_forward_intr_19,
	(uint64_t)kkm_forward_intr_20,	(uint64_t)kkm_forward_intr_21,
	(uint64_t)kkm_forward_intr_22,	(uint64_t)kkm_forward_intr_23,
	(uint64_t)kkm_forward_intr_24,	(uint64_t)kkm_forward_intr_25,
	(uint64_t)kkm_forward_intr_26,	(uint64_t)kkm_forward_intr_27,
	(uint64_t)kkm_forward_intr_28,	(uint64_t)kkm_forward_intr_29,
	(uint64_t)kkm_forward_intr_30,	(uint64_t)kkm_forward_intr_31,
	(uint64_t)kkm_forward_intr_32,	(uint64_t)kkm_forward_intr_33,
	(uint64_t)kkm_forward_intr_34,	(uint64_t)kkm_forward_intr_35,
	(uint64_t)kkm_forward_intr_36,	(uint64_t)kkm_forward_intr_37,
	(uint64_t)kkm_forward_intr_38,	(uint64_t)kkm_forward_intr_39,
	(uint64_t)kkm_forward_intr_40,	(uint64_t)kkm_forward_intr_41,
	(uint64_t)kkm_forward_intr_42,	(uint64_t)kkm_forward_intr_43,
	(uint64_t)kkm_forward_intr_44,	(uint64_t)kkm_forward_intr_45,
	(uint64_t)kkm_forward_intr_46,	(uint64_t)kkm_forward_intr_47,
	(uint64_t)kkm_forward_intr_48,	(uint64_t)kkm_forward_intr_49,
	(uint64_t)kkm_forward_intr_50,	(uint64_t)kkm_forward_intr_51,
	(uint64_t)kkm_forward_intr_52,	(uint64_t)kkm_forward_intr_53,
	(uint64_t)kkm_forward_intr_54,	(uint64_t)kkm_forward_intr_55,
	(uint64_t)kkm_forward_intr_56,	(uint64_t)kkm_forward_intr_57,
	(uint64_t)kkm_forward_intr_58,	(uint64_t)kkm_forward_intr_59,
	(uint64_t)kkm_forward_intr_60,	(uint64_t)kkm_forward_intr_61,
	(uint64_t)kkm_forward_intr_62,	(uint64_t)kkm_forward_intr_63,
	(uint64_t)kkm_forward_intr_64,	(uint64_t)kkm_forward_intr_65,
	(uint64_t)kkm_forward_intr_66,	(uint64_t)kkm_forward_intr_67,
	(uint64_t)kkm_forward_intr_68,	(uint64_t)kkm_forward_intr_69,
	(uint64_t)kkm_forward_intr_70,	(uint64_t)kkm_forward_intr_71,
	(uint64_t)kkm_forward_intr_72,	(uint64_t)kkm_forward_intr_73,
	(uint64_t)kkm_forward_intr_74,	(uint64_t)kkm_forward_intr_75,
	(uint64_t)kkm_forward_intr_76,	(uint64_t)kkm_forward_intr_77,
	(uint64_t)kkm_forward_intr_78,	(uint64_t)kkm_forward_intr_79,
	(uint64_t)kkm_forward_intr_80,	(uint64_t)kkm_forward_intr_81,
	(uint64_t)kkm_forward_intr_82,	(uint64_t)kkm_forward_intr_83,
	(uint64_t)kkm_forward_intr_84,	(uint64_t)kkm_forward_intr_85,
	(uint64_t)kkm_forward_intr_86,	(uint64_t)kkm_forward_intr_87,
	(uint64_t)kkm_forward_intr_88,	(uint64_t)kkm_forward_intr_89,
	(uint64_t)kkm_forward_intr_90,	(uint64_t)kkm_forward_intr_91,
	(uint64_t)kkm_forward_intr_92,	(uint64_t)kkm_forward_intr_93,
	(uint64_t)kkm_forward_intr_94,	(uint64_t)kkm_forward_intr_95,
	(uint64_t)kkm_forward_intr_96,	(uint64_t)kkm_forward_intr_97,
	(uint64_t)kkm_forward_intr_98,	(uint64_t)kkm_forward_intr_99,
	(uint64_t)kkm_forward_intr_100, (uint64_t)kkm_forward_intr_101,
	(uint64_t)kkm_forward_intr_102, (uint64_t)kkm_forward_intr_103,
	(uint64_t)kkm_forward_intr_104, (uint64_t)kkm_forward_intr_105,
	(uint64_t)kkm_forward_intr_106, (uint64_t)kkm_forward_intr_107,
	(uint64_t)kkm_forward_intr_108, (uint64_t)kkm_forward_intr_109,
	(uint64_t)kkm_forward_intr_110, (uint64_t)kkm_forward_intr_111,
	(uint64_t)kkm_forward_intr_112, (uint64_t)kkm_forward_intr_113,
	(uint64_t)kkm_forward_intr_114, (uint64_t)kkm_forward_intr_115,
	(uint64_t)kkm_forward_intr_116, (uint64_t)kkm_forward_intr_117,
	(uint64_t)kkm_forward_intr_118, (uint64_t)kkm_forward_intr_119,
	(uint64_t)kkm_forward_intr_120, (uint64_t)kkm_forward_intr_121,
	(uint64_t)kkm_forward_intr_122, (uint64_t)kkm_forward_intr_123,
	(uint64_t)kkm_forward_intr_124, (uint64_t)kkm_forward_intr_125,
	(uint64_t)kkm_forward_intr_126, (uint64_t)kkm_forward_intr_127,
	(uint64_t)kkm_forward_intr_128, (uint64_t)kkm_forward_intr_129,
	(uint64_t)kkm_forward_intr_130, (uint64_t)kkm_forward_intr_131,
	(uint64_t)kkm_forward_intr_132, (uint64_t)kkm_forward_intr_133,
	(uint64_t)kkm_forward_intr_134, (uint64_t)kkm_forward_intr_135,
	(uint64_t)kkm_forward_intr_136, (uint64_t)kkm_forward_intr_137,
	(uint64_t)kkm_forward_intr_138, (uint64_t)kkm_forward_intr_139,
	(uint64_t)kkm_forward_intr_140, (uint64_t)kkm_forward_intr_141,
	(uint64_t)kkm_forward_intr_142, (uint64_t)kkm_forward_intr_143,
	(uint64_t)kkm_forward_intr_144, (uint64_t)kkm_forward_intr_145,
	(uint64_t)kkm_forward_intr_146, (uint64_t)kkm_forward_intr_147,
	(uint64_t)kkm_forward_intr_148, (uint64_t)kkm_forward_intr_149,
	(uint64_t)kkm_forward_intr_150, (uint64_t)kkm_forward_intr_151,
	(uint64_t)kkm_forward_intr_152, (uint64_t)kkm_forward_intr_153,
	(uint64_t)kkm_forward_intr_154, (uint64_t)kkm_forward_intr_155,
	(uint64_t)kkm_forward_intr_156, (uint64_t)kkm_forward_intr_157,
	(uint64_t)kkm_forward_intr_158, (uint64_t)kkm_forward_intr_159,
	(uint64_t)kkm_forward_intr_160, (uint64_t)kkm_forward_intr_161,
	(uint64_t)kkm_forward_intr_162, (uint64_t)kkm_forward_intr_163,
	(uint64_t)kkm_forward_intr_164, (uint64_t)kkm_forward_intr_165,
	(uint64_t)kkm_forward_intr_166, (uint64_t)kkm_forward_intr_167,
	(uint64_t)kkm_forward_intr_168, (uint64_t)kkm_forward_intr_169,
	(uint64_t)kkm_forward_intr_170, (uint64_t)kkm_forward_intr_171,
	(uint64_t)kkm_forward_intr_172, (uint64_t)kkm_forward_intr_173,
	(uint64_t)kkm_forward_intr_174, (uint64_t)kkm_forward_intr_175,
	(uint64_t)kkm_forward_intr_176, (uint64_t)kkm_forward_intr_177,
	(uint64_t)kkm_forward_intr_178, (uint64_t)kkm_forward_intr_179,
	(uint64_t)kkm_forward_intr_180, (uint64_t)kkm_forward_intr_181,
	(uint64_t)kkm_forward_intr_182, (uint64_t)kkm_forward_intr_183,
	(uint64_t)kkm_forward_intr_184, (uint64_t)kkm_forward_intr_185,
	(uint64_t)kkm_forward_intr_186, (uint64_t)kkm_forward_intr_187,
	(uint64_t)kkm_forward_intr_188, (uint64_t)kkm_forward_intr_189,
	(uint64_t)kkm_forward_intr_190, (uint64_t)kkm_forward_intr_191,
	(uint64_t)kkm_forward_intr_192, (uint64_t)kkm_forward_intr_193,
	(uint64_t)kkm_forward_intr_194, (uint64_t)kkm_forward_intr_195,
	(uint64_t)kkm_forward_intr_196, (uint64_t)kkm_forward_intr_197,
	(uint64_t)kkm_forward_intr_198, (uint64_t)kkm_forward_intr_199,
	(uint64_t)kkm_forward_intr_200, (uint64_t)kkm_forward_intr_201,
	(uint64_t)kkm_forward_intr_202, (uint64_t)kkm_forward_intr_203,
	(uint64_t)kkm_forward_intr_204, (uint64_t)kkm_forward_intr_205,
	(uint64_t)kkm_forward_intr_206, (uint64_t)kkm_forward_intr_207,
	(uint64_t)kkm_forward_intr_208, (uint64_t)kkm_forward_intr_209,
	(uint64_t)kkm_forward_intr_210, (uint64_t)kkm_forward_intr_211,
	(uint64_t)kkm_forward_intr_212, (uint64_t)kkm_forward_intr_213,
	(uint64_t)kkm_forward_intr_214, (uint64_t)kkm_forward_intr_215,
	(uint64_t)kkm_forward_intr_216, (uint64_t)kkm_forward_intr_217,
	(uint64_t)kkm_forward_intr_218, (uint64_t)kkm_forward_intr_219,
	(uint64_t)kkm_forward_intr_220, (uint64_t)kkm_forward_intr_221,
	(uint64_t)kkm_forward_intr_222, (uint64_t)kkm_forward_intr_223,
	(uint64_t)kkm_forward_intr_224, (uint64_t)kkm_forward_intr_225,
	(uint64_t)kkm_forward_intr_226, (uint64_t)kkm_forward_intr_227,
	(uint64_t)kkm_forward_intr_228, (uint64_t)kkm_forward_intr_229,
	(uint64_t)kkm_forward_intr_230, (uint64_t)kkm_forward_intr_231,
	(uint64_t)kkm_forward_intr_232, (uint64_t)kkm_forward_intr_233,
	(uint64_t)kkm_forward_intr_234, (uint64_t)kkm_forward_intr_235,
	(uint64_t)kkm_forward_intr_236, (uint64_t)kkm_forward_intr_237,
	(uint64_t)kkm_forward_intr_238, (uint64_t)kkm_forward_intr_239,
	(uint64_t)kkm_forward_intr_240, (uint64_t)kkm_forward_intr_241,
	(uint64_t)kkm_forward_intr_242, (uint64_t)kkm_forward_intr_243,
	(uint64_t)kkm_forward_intr_244, (uint64_t)kkm_forward_intr_245,
	(uint64_t)kkm_forward_intr_246, (uint64_t)kkm_forward_intr_247,
	(uint64_t)kkm_forward_intr_248, (uint64_t)kkm_forward_intr_249,
	(uint64_t)kkm_forward_intr_250, (uint64_t)kkm_forward_intr_251,
	(uint64_t)kkm_forward_intr_252, (uint64_t)kkm_forward_intr_253,
	(uint64_t)kkm_forward_intr_254, (uint64_t)kkm_forward_intr_255,
};

void (*always_intr_function_pointers[NR_VECTORS])(void) = {
	kkm_aie_0,   kkm_aie_1,	  kkm_aie_2,   kkm_aie_3,   kkm_aie_4,
	kkm_aie_5,   kkm_aie_6,	  kkm_aie_7,   kkm_aie_8,   kkm_aie_9,
	kkm_aie_10,  kkm_aie_11,  kkm_aie_12,  kkm_aie_13,  kkm_aie_14,
	kkm_aie_15,  kkm_aie_16,  kkm_aie_17,  kkm_aie_18,  kkm_aie_19,
	kkm_aie_20,  kkm_aie_21,  kkm_aie_22,  kkm_aie_23,  kkm_aie_24,
	kkm_aie_25,  kkm_aie_26,  kkm_aie_27,  kkm_aie_28,  kkm_aie_29,
	kkm_aie_30,  kkm_aie_31,  kkm_aie_32,  kkm_aie_33,  kkm_aie_34,
	kkm_aie_35,  kkm_aie_36,  kkm_aie_37,  kkm_aie_38,  kkm_aie_39,
	kkm_aie_40,  kkm_aie_41,  kkm_aie_42,  kkm_aie_43,  kkm_aie_44,
	kkm_aie_45,  kkm_aie_46,  kkm_aie_47,  kkm_aie_48,  kkm_aie_49,
	kkm_aie_50,  kkm_aie_51,  kkm_aie_52,  kkm_aie_53,  kkm_aie_54,
	kkm_aie_55,  kkm_aie_56,  kkm_aie_57,  kkm_aie_58,  kkm_aie_59,
	kkm_aie_60,  kkm_aie_61,  kkm_aie_62,  kkm_aie_63,  kkm_aie_64,
	kkm_aie_65,  kkm_aie_66,  kkm_aie_67,  kkm_aie_68,  kkm_aie_69,
	kkm_aie_70,  kkm_aie_71,  kkm_aie_72,  kkm_aie_73,  kkm_aie_74,
	kkm_aie_75,  kkm_aie_76,  kkm_aie_77,  kkm_aie_78,  kkm_aie_79,
	kkm_aie_80,  kkm_aie_81,  kkm_aie_82,  kkm_aie_83,  kkm_aie_84,
	kkm_aie_85,  kkm_aie_86,  kkm_aie_87,  kkm_aie_88,  kkm_aie_89,
	kkm_aie_90,  kkm_aie_91,  kkm_aie_92,  kkm_aie_93,  kkm_aie_94,
	kkm_aie_95,  kkm_aie_96,  kkm_aie_97,  kkm_aie_98,  kkm_aie_99,
	kkm_aie_100, kkm_aie_101, kkm_aie_102, kkm_aie_103, kkm_aie_104,
	kkm_aie_105, kkm_aie_106, kkm_aie_107, kkm_aie_108, kkm_aie_109,
	kkm_aie_110, kkm_aie_111, kkm_aie_112, kkm_aie_113, kkm_aie_114,
	kkm_aie_115, kkm_aie_116, kkm_aie_117, kkm_aie_118, kkm_aie_119,
	kkm_aie_120, kkm_aie_121, kkm_aie_122, kkm_aie_123, kkm_aie_124,
	kkm_aie_125, kkm_aie_126, kkm_aie_127, kkm_aie_128, kkm_aie_129,
	kkm_aie_130, kkm_aie_131, kkm_aie_132, kkm_aie_133, kkm_aie_134,
	kkm_aie_135, kkm_aie_136, kkm_aie_137, kkm_aie_138, kkm_aie_139,
	kkm_aie_140, kkm_aie_141, kkm_aie_142, kkm_aie_143, kkm_aie_144,
	kkm_aie_145, kkm_aie_146, kkm_aie_147, kkm_aie_148, kkm_aie_149,
	kkm_aie_150, kkm_aie_151, kkm_aie_152, kkm_aie_153, kkm_aie_154,
	kkm_aie_155, kkm_aie_156, kkm_aie_157, kkm_aie_158, kkm_aie_159,
	kkm_aie_160, kkm_aie_161, kkm_aie_162, kkm_aie_163, kkm_aie_164,
	kkm_aie_165, kkm_aie_166, kkm_aie_167, kkm_aie_168, kkm_aie_169,
	kkm_aie_170, kkm_aie_171, kkm_aie_172, kkm_aie_173, kkm_aie_174,
	kkm_aie_175, kkm_aie_176, kkm_aie_177, kkm_aie_178, kkm_aie_179,
	kkm_aie_180, kkm_aie_181, kkm_aie_182, kkm_aie_183, kkm_aie_184,
	kkm_aie_185, kkm_aie_186, kkm_aie_187, kkm_aie_188, kkm_aie_189,
	kkm_aie_190, kkm_aie_191, kkm_aie_192, kkm_aie_193, kkm_aie_194,
	kkm_aie_195, kkm_aie_196, kkm_aie_197, kkm_aie_198, kkm_aie_199,
	kkm_aie_200, kkm_aie_201, kkm_aie_202, kkm_aie_203, kkm_aie_204,
	kkm_aie_205, kkm_aie_206, kkm_aie_207, kkm_aie_208, kkm_aie_209,
	kkm_aie_210, kkm_aie_211, kkm_aie_212, kkm_aie_213, kkm_aie_214,
	kkm_aie_215, kkm_aie_216, kkm_aie_217, kkm_aie_218, kkm_aie_219,
	kkm_aie_220, kkm_aie_221, kkm_aie_222, kkm_aie_223, kkm_aie_224,
	kkm_aie_225, kkm_aie_226, kkm_aie_227, kkm_aie_228, kkm_aie_229,
	kkm_aie_230, kkm_aie_231, kkm_aie_232, kkm_aie_233, kkm_aie_234,
	kkm_aie_235, kkm_aie_236, kkm_aie_237, kkm_aie_238, kkm_aie_239,
	kkm_aie_240, kkm_aie_241, kkm_aie_242, kkm_aie_243, kkm_aie_244,
	kkm_aie_245, kkm_aie_246, kkm_aie_247, kkm_aie_248, kkm_aie_249,
	kkm_aie_250, kkm_aie_251, kkm_aie_252, kkm_aie_253, kkm_aie_254,
	kkm_aie_255,
};

void (*always_intr_forward_function_pointers[NR_VECTORS])(uint64_t, uint64_t, uint64_t, uint64_t) = {
	kkm_aife,	kkm_aife,	kkm_aife,	kkm_aife,
	kkm_aife,	kkm_aife,	kkm_aife,	kkm_aife,
	kkm_aife_error, kkm_aife,	kkm_aife_error, kkm_aife_error,
	kkm_aife_error, kkm_aife_error, kkm_aife_error, kkm_aife,
	kkm_aife,	kkm_aife_error, kkm_aife,	kkm_aife,
	kkm_aife,	kkm_aife_error, kkm_aife,	kkm_aife,
	kkm_aife,	kkm_aife,	kkm_aife,	kkm_aife,
	kkm_aife,	kkm_aife_error, kkm_aife_error, kkm_aife,
	kkm_aife,	kkm_aife,	kkm_aife,	kkm_aife,
	kkm_aife,	kkm_aife,	kkm_aife,	kkm_aife,
	kkm_aife,	kkm_aife,	kkm_aife,	kkm_aife,
	kkm_aife,	kkm_aife,	kkm_aife,	kkm_aife,
	kkm_aife,	kkm_aife,	kkm_aife,	kkm_aife,
	kkm_aife,	kkm_aife,	kkm_aife,	kkm_aife,
	kkm_aife,	kkm_aife,	kkm_aife,	kkm_aife,
	kkm_aife,	kkm_aife,	kkm_aife,	kkm_aife,
	kkm_aife,	kkm_aife,	kkm_aife,	kkm_aife,
	kkm_aife,	kkm_aife,	kkm_aife,	kkm_aife,
	kkm_aife,	kkm_aife,	kkm_aife,	kkm_aife,
	kkm_aife,	kkm_aife,	kkm_aife,	kkm_aife,
	kkm_aife,	kkm_aife,	kkm_aife,	kkm_aife,
	kkm_aife,	kkm_aife,	kkm_aife,	kkm_aife,
	kkm_aife,	kkm_aife,	kkm_aife,	kkm_aife,
	kkm_aife,	kkm_aife,	kkm_aife,	kkm_aife,
	kkm_aife,	kkm_aife,	kkm_aife,	kkm_aife,
	kkm_aife,	kkm_aife,	kkm_aife,	kkm_aife,
	kkm_aife,	kkm_aife,	kkm_aife,	kkm_aife,
	kkm_aife,	kkm_aife,	kkm_aife,	kkm_aife,
	kkm_aife,	kkm_aife,	kkm_aife,	kkm_aife,
	kkm_aife,	kkm_aife,	kkm_aife,	kkm_aife,
	kkm_aife,	kkm_aife,	kkm_aife,	kkm_aife,
	kkm_aife,	kkm_aife,	kkm_aife,	kkm_aife,
	kkm_aife,	kkm_aife,	kkm_aife,	kkm_aife,
	kkm_aife,	kkm_aife,	kkm_aife,	kkm_aife,
	kkm_aife,	kkm_aife,	kkm_aife,	kkm_aife,
	kkm_aife,	kkm_aife,	kkm_aife,	kkm_aife,
	kkm_aife,	kkm_aife,	kkm_aife,	kkm_aife,
	kkm_aife,	kkm_aife,	kkm_aife,	kkm_aife,
	kkm_aife,	kkm_aife,	kkm_aife,	kkm_aife,
	kkm_aife,	kkm_aife,	kkm_aife,	kkm_aife,
	kkm_aife,	kkm_aife,	kkm_aife,	kkm_aife,
	kkm_aife,	kkm_aife,	kkm_aife,	kkm_aife,
	kkm_aife,	kkm_aife,	kkm_aife,	kkm_aife,
	kkm_aife,	kkm_aife,	kkm_aife,	kkm_aife,
	kkm_aife,	kkm_aife,	kkm_aife,	kkm_aife,
	kkm_aife,	kkm_aife,	kkm_aife,	kkm_aife,
	kkm_aife,	kkm_aife,	kkm_aife,	kkm_aife,
	kkm_aife,	kkm_aife,	kkm_aife,	kkm_aife,
	kkm_aife,	kkm_aife,	kkm_aife,	kkm_aife,
	kkm_aife,	kkm_aife,	kkm_aife,	kkm_aife,
	kkm_aife,	kkm_aife,	kkm_aife,	kkm_aife,
	kkm_aife,	kkm_aife,	kkm_aife,	kkm_aife,
	kkm_aife,	kkm_aife,	kkm_aife,	kkm_aife,
	kkm_aife,	kkm_aife,	kkm_aife,	kkm_aife,
	kkm_aife,	kkm_aife,	kkm_aife,	kkm_aife,
	kkm_aife,	kkm_aife,	kkm_aife,	kkm_aife,
	kkm_aife,	kkm_aife,	kkm_aife,	kkm_aife,
	kkm_aife,	kkm_aife,	kkm_aife,	kkm_aife,
	kkm_aife,	kkm_aife,	kkm_aife,	kkm_aife,
	kkm_aife,	kkm_aife,	kkm_aife,	kkm_aife,
	kkm_aife,	kkm_aife,	kkm_aife,	kkm_aife,
	kkm_aife,	kkm_aife,	kkm_aife,	kkm_aife,
	kkm_aife,	kkm_aife,	kkm_aife,	kkm_aife,
	kkm_aife,	kkm_aife,	kkm_aife,	kkm_aife,
};

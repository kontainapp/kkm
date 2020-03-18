/*
 * Copyright © 2020-2020 Kontain Inc. All rights reserved.
 *
 * Kontain Inc CONFIDENTIAL
 *
 * This file includes unpublished proprietary source code of Kontain Inc. The
 * copyright notice above does not evidence any actual or intended publication
 * of such source code. Disclosure of this source code or any related
 * proprietary information is strictly prohibited without the express written
 * permission of Kontain Inc.
 */

#ifndef __KKM_INTR_H__
#define __KKM_INTR_H__

void kkm_intr_start(void);
void kkm_intr_fill(void);
void kkm_intr_entry_asm(void);
void kkm_intr_entry_general_protection(void);
void kkm_intr_entry_page_fault(void);

/* one for each intr */
void kkm_intr_entry_0(void);
void kkm_intr_entry_1(void);
void kkm_intr_entry_2(void);
void kkm_intr_entry_3(void);
void kkm_intr_entry_4(void);
void kkm_intr_entry_5(void);
void kkm_intr_entry_6(void);
void kkm_intr_entry_7(void);
void kkm_intr_entry_8(void);
void kkm_intr_entry_9(void);
void kkm_intr_entry_10(void);
void kkm_intr_entry_11(void);
void kkm_intr_entry_12(void);
void kkm_intr_entry_13(void);
void kkm_intr_entry_14(void);
void kkm_intr_entry_15(void);
void kkm_intr_entry_16(void);
void kkm_intr_entry_17(void);
void kkm_intr_entry_18(void);
void kkm_intr_entry_19(void);
void kkm_intr_entry_20(void);
void kkm_intr_entry_21(void);
void kkm_intr_entry_22(void);
void kkm_intr_entry_23(void);
void kkm_intr_entry_24(void);
void kkm_intr_entry_25(void);
void kkm_intr_entry_26(void);
void kkm_intr_entry_27(void);
void kkm_intr_entry_28(void);
void kkm_intr_entry_29(void);
void kkm_intr_entry_30(void);
void kkm_intr_entry_31(void);
void kkm_intr_entry_32(void);
void kkm_intr_entry_33(void);
void kkm_intr_entry_34(void);
void kkm_intr_entry_35(void);
void kkm_intr_entry_36(void);
void kkm_intr_entry_37(void);
void kkm_intr_entry_38(void);
void kkm_intr_entry_39(void);
void kkm_intr_entry_40(void);
void kkm_intr_entry_41(void);
void kkm_intr_entry_42(void);
void kkm_intr_entry_43(void);
void kkm_intr_entry_44(void);
void kkm_intr_entry_45(void);
void kkm_intr_entry_46(void);
void kkm_intr_entry_47(void);
void kkm_intr_entry_48(void);
void kkm_intr_entry_49(void);
void kkm_intr_entry_50(void);
void kkm_intr_entry_51(void);
void kkm_intr_entry_52(void);
void kkm_intr_entry_53(void);
void kkm_intr_entry_54(void);
void kkm_intr_entry_55(void);
void kkm_intr_entry_56(void);
void kkm_intr_entry_57(void);
void kkm_intr_entry_58(void);
void kkm_intr_entry_59(void);
void kkm_intr_entry_60(void);
void kkm_intr_entry_61(void);
void kkm_intr_entry_62(void);
void kkm_intr_entry_63(void);
void kkm_intr_entry_64(void);
void kkm_intr_entry_65(void);
void kkm_intr_entry_66(void);
void kkm_intr_entry_67(void);
void kkm_intr_entry_68(void);
void kkm_intr_entry_69(void);
void kkm_intr_entry_70(void);
void kkm_intr_entry_71(void);
void kkm_intr_entry_72(void);
void kkm_intr_entry_73(void);
void kkm_intr_entry_74(void);
void kkm_intr_entry_75(void);
void kkm_intr_entry_76(void);
void kkm_intr_entry_77(void);
void kkm_intr_entry_78(void);
void kkm_intr_entry_79(void);
void kkm_intr_entry_80(void);
void kkm_intr_entry_81(void);
void kkm_intr_entry_82(void);
void kkm_intr_entry_83(void);
void kkm_intr_entry_84(void);
void kkm_intr_entry_85(void);
void kkm_intr_entry_86(void);
void kkm_intr_entry_87(void);
void kkm_intr_entry_88(void);
void kkm_intr_entry_89(void);
void kkm_intr_entry_90(void);
void kkm_intr_entry_91(void);
void kkm_intr_entry_92(void);
void kkm_intr_entry_93(void);
void kkm_intr_entry_94(void);
void kkm_intr_entry_95(void);
void kkm_intr_entry_96(void);
void kkm_intr_entry_97(void);
void kkm_intr_entry_98(void);
void kkm_intr_entry_99(void);
void kkm_intr_entry_100(void);
void kkm_intr_entry_101(void);
void kkm_intr_entry_102(void);
void kkm_intr_entry_103(void);
void kkm_intr_entry_104(void);
void kkm_intr_entry_105(void);
void kkm_intr_entry_106(void);
void kkm_intr_entry_107(void);
void kkm_intr_entry_108(void);
void kkm_intr_entry_109(void);
void kkm_intr_entry_110(void);
void kkm_intr_entry_111(void);
void kkm_intr_entry_112(void);
void kkm_intr_entry_113(void);
void kkm_intr_entry_114(void);
void kkm_intr_entry_115(void);
void kkm_intr_entry_116(void);
void kkm_intr_entry_117(void);
void kkm_intr_entry_118(void);
void kkm_intr_entry_119(void);
void kkm_intr_entry_120(void);
void kkm_intr_entry_121(void);
void kkm_intr_entry_122(void);
void kkm_intr_entry_123(void);
void kkm_intr_entry_124(void);
void kkm_intr_entry_125(void);
void kkm_intr_entry_126(void);
void kkm_intr_entry_127(void);
void kkm_intr_entry_128(void);
void kkm_intr_entry_129(void);
void kkm_intr_entry_130(void);
void kkm_intr_entry_131(void);
void kkm_intr_entry_132(void);
void kkm_intr_entry_133(void);
void kkm_intr_entry_134(void);
void kkm_intr_entry_135(void);
void kkm_intr_entry_136(void);
void kkm_intr_entry_137(void);
void kkm_intr_entry_138(void);
void kkm_intr_entry_139(void);
void kkm_intr_entry_140(void);
void kkm_intr_entry_141(void);
void kkm_intr_entry_142(void);
void kkm_intr_entry_143(void);
void kkm_intr_entry_144(void);
void kkm_intr_entry_145(void);
void kkm_intr_entry_146(void);
void kkm_intr_entry_147(void);
void kkm_intr_entry_148(void);
void kkm_intr_entry_149(void);
void kkm_intr_entry_150(void);
void kkm_intr_entry_151(void);
void kkm_intr_entry_152(void);
void kkm_intr_entry_153(void);
void kkm_intr_entry_154(void);
void kkm_intr_entry_155(void);
void kkm_intr_entry_156(void);
void kkm_intr_entry_157(void);
void kkm_intr_entry_158(void);
void kkm_intr_entry_159(void);
void kkm_intr_entry_160(void);
void kkm_intr_entry_161(void);
void kkm_intr_entry_162(void);
void kkm_intr_entry_163(void);
void kkm_intr_entry_164(void);
void kkm_intr_entry_165(void);
void kkm_intr_entry_166(void);
void kkm_intr_entry_167(void);
void kkm_intr_entry_168(void);
void kkm_intr_entry_169(void);
void kkm_intr_entry_170(void);
void kkm_intr_entry_171(void);
void kkm_intr_entry_172(void);
void kkm_intr_entry_173(void);
void kkm_intr_entry_174(void);
void kkm_intr_entry_175(void);
void kkm_intr_entry_176(void);
void kkm_intr_entry_177(void);
void kkm_intr_entry_178(void);
void kkm_intr_entry_179(void);
void kkm_intr_entry_180(void);
void kkm_intr_entry_181(void);
void kkm_intr_entry_182(void);
void kkm_intr_entry_183(void);
void kkm_intr_entry_184(void);
void kkm_intr_entry_185(void);
void kkm_intr_entry_186(void);
void kkm_intr_entry_187(void);
void kkm_intr_entry_188(void);
void kkm_intr_entry_189(void);
void kkm_intr_entry_190(void);
void kkm_intr_entry_191(void);
void kkm_intr_entry_192(void);
void kkm_intr_entry_193(void);
void kkm_intr_entry_194(void);
void kkm_intr_entry_195(void);
void kkm_intr_entry_196(void);
void kkm_intr_entry_197(void);
void kkm_intr_entry_198(void);
void kkm_intr_entry_199(void);
void kkm_intr_entry_200(void);
void kkm_intr_entry_201(void);
void kkm_intr_entry_202(void);
void kkm_intr_entry_203(void);
void kkm_intr_entry_204(void);
void kkm_intr_entry_205(void);
void kkm_intr_entry_206(void);
void kkm_intr_entry_207(void);
void kkm_intr_entry_208(void);
void kkm_intr_entry_209(void);
void kkm_intr_entry_210(void);
void kkm_intr_entry_211(void);
void kkm_intr_entry_212(void);
void kkm_intr_entry_213(void);
void kkm_intr_entry_214(void);
void kkm_intr_entry_215(void);
void kkm_intr_entry_216(void);
void kkm_intr_entry_217(void);
void kkm_intr_entry_218(void);
void kkm_intr_entry_219(void);
void kkm_intr_entry_220(void);
void kkm_intr_entry_221(void);
void kkm_intr_entry_222(void);
void kkm_intr_entry_223(void);
void kkm_intr_entry_224(void);
void kkm_intr_entry_225(void);
void kkm_intr_entry_226(void);
void kkm_intr_entry_227(void);
void kkm_intr_entry_228(void);
void kkm_intr_entry_229(void);
void kkm_intr_entry_230(void);
void kkm_intr_entry_231(void);
void kkm_intr_entry_232(void);
void kkm_intr_entry_233(void);
void kkm_intr_entry_234(void);
void kkm_intr_entry_235(void);
void kkm_intr_entry_236(void);
void kkm_intr_entry_237(void);
void kkm_intr_entry_238(void);
void kkm_intr_entry_239(void);
void kkm_intr_entry_240(void);
void kkm_intr_entry_241(void);
void kkm_intr_entry_242(void);
void kkm_intr_entry_243(void);
void kkm_intr_entry_244(void);
void kkm_intr_entry_245(void);
void kkm_intr_entry_246(void);
void kkm_intr_entry_247(void);
void kkm_intr_entry_248(void);
void kkm_intr_entry_249(void);
void kkm_intr_entry_250(void);
void kkm_intr_entry_251(void);
void kkm_intr_entry_252(void);
void kkm_intr_entry_253(void);
void kkm_intr_entry_254(void);
void kkm_intr_entry_255(void);

#endif /* __KKM_INTR_H__ */

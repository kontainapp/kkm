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

#include <linux/linkage.h>
#include <asm/percpu.h>
#include <asm/asm-offsets.h>
#include "kkm_offsets.h"

	.text
/*
 * %rdi -- intr call pointer
 */
	.align 16
	.global kkm_forward_intr
kkm_forward_intr:

	/*
	 * save callee saved registers according to C-ABI
	 */
	pushq	%rbp
	pushq	%rbx
	pushq	%r12
	pushq	%r13
	pushq	%r14
	pushq	%r15

	/*
	 * equivalent of indirect call
	 */
	pushq	$kkm_forward_intr_ret
	pushq	%rdi
	retq

kkm_forward_intr_ret:

	popq	%r15
	popq	%r14
	popq	%r13
	popq	%r12
	popq	%rbx
	popq	%rbp
	retq

	.size kkm_forward_intr, .-kkm_forward_intr

	.align 16
	.global kkm_forward_intr_0
kkm_forward_intr_0:
	int	$0
	retq
	.size kkm_forward_intr_0, .-kkm_forward_intr_0

	.align 16
	.global kkm_forward_intr_1
kkm_forward_intr_1:
	int	$1
	retq
	.size kkm_forward_intr_1, .-kkm_forward_intr_1

	.align 16
	.global kkm_forward_intr_2
kkm_forward_intr_2:
	int	$2
	retq
	.size kkm_forward_intr_2, .-kkm_forward_intr_2

	.align 16
	.global kkm_forward_intr_3
kkm_forward_intr_3:
	int	$3
	retq
	.size kkm_forward_intr_3, .-kkm_forward_intr_3

	.align 16
	.global kkm_forward_intr_4
kkm_forward_intr_4:
	int	$4
	retq
	.size kkm_forward_intr_4, .-kkm_forward_intr_4

	.align 16
	.global kkm_forward_intr_5
kkm_forward_intr_5:
	int	$5
	retq
	.size kkm_forward_intr_5, .-kkm_forward_intr_5

	.align 16
	.global kkm_forward_intr_6
kkm_forward_intr_6:
	int	$6
	retq
	.size kkm_forward_intr_6, .-kkm_forward_intr_6

	.align 16
	.global kkm_forward_intr_7
kkm_forward_intr_7:
	int	$7
	retq
	.size kkm_forward_intr_7, .-kkm_forward_intr_7

	.align 16
	.global kkm_forward_intr_8
kkm_forward_intr_8:
	int	$8
	retq
	.size kkm_forward_intr_8, .-kkm_forward_intr_8

	.align 16
	.global kkm_forward_intr_9
kkm_forward_intr_9:
	int	$9
	retq
	.size kkm_forward_intr_9, .-kkm_forward_intr_9

	.align 16
	.global kkm_forward_intr_10
kkm_forward_intr_10:
	int	$10
	retq
	.size kkm_forward_intr_10, .-kkm_forward_intr_10

	.align 16
	.global kkm_forward_intr_11
kkm_forward_intr_11:
	int	$11
	retq
	.size kkm_forward_intr_11, .-kkm_forward_intr_11

	.align 16
	.global kkm_forward_intr_12
kkm_forward_intr_12:
	int	$12
	retq
	.size kkm_forward_intr_12, .-kkm_forward_intr_12

	.align 16
	.global kkm_forward_intr_13
kkm_forward_intr_13:
	int	$13
	retq
	.size kkm_forward_intr_13, .-kkm_forward_intr_13

	.align 16
	.global kkm_forward_intr_14
kkm_forward_intr_14:
	int	$14
	retq
	.size kkm_forward_intr_14, .-kkm_forward_intr_14

	.align 16
	.global kkm_forward_intr_15
kkm_forward_intr_15:
	int	$15
	retq
	.size kkm_forward_intr_15, .-kkm_forward_intr_15

	.align 16
	.global kkm_forward_intr_16
kkm_forward_intr_16:
	int	$16
	retq
	.size kkm_forward_intr_16, .-kkm_forward_intr_16

	.align 16
	.global kkm_forward_intr_17
kkm_forward_intr_17:
	int	$17
	retq
	.size kkm_forward_intr_17, .-kkm_forward_intr_17

	.align 16
	.global kkm_forward_intr_18
kkm_forward_intr_18:
	int	$18
	retq
	.size kkm_forward_intr_18, .-kkm_forward_intr_18

	.align 16
	.global kkm_forward_intr_19
kkm_forward_intr_19:
	int	$19
	retq
	.size kkm_forward_intr_19, .-kkm_forward_intr_19

	.align 16
	.global kkm_forward_intr_20
kkm_forward_intr_20:
	int	$20
	retq
	.size kkm_forward_intr_20, .-kkm_forward_intr_20

	.align 16
	.global kkm_forward_intr_21
kkm_forward_intr_21:
	int	$21
	retq
	.size kkm_forward_intr_21, .-kkm_forward_intr_21

	.align 16
	.global kkm_forward_intr_22
kkm_forward_intr_22:
	int	$22
	retq
	.size kkm_forward_intr_22, .-kkm_forward_intr_22

	.align 16
	.global kkm_forward_intr_23
kkm_forward_intr_23:
	int	$23
	retq
	.size kkm_forward_intr_23, .-kkm_forward_intr_23

	.align 16
	.global kkm_forward_intr_24
kkm_forward_intr_24:
	int	$24
	retq
	.size kkm_forward_intr_24, .-kkm_forward_intr_24

	.align 16
	.global kkm_forward_intr_25
kkm_forward_intr_25:
	int	$25
	retq
	.size kkm_forward_intr_25, .-kkm_forward_intr_25

	.align 16
	.global kkm_forward_intr_26
kkm_forward_intr_26:
	int	$26
	retq
	.size kkm_forward_intr_26, .-kkm_forward_intr_26

	.align 16
	.global kkm_forward_intr_27
kkm_forward_intr_27:
	int	$27
	retq
	.size kkm_forward_intr_27, .-kkm_forward_intr_27

	.align 16
	.global kkm_forward_intr_28
kkm_forward_intr_28:
	int	$28
	retq
	.size kkm_forward_intr_28, .-kkm_forward_intr_28

	.align 16
	.global kkm_forward_intr_29
kkm_forward_intr_29:
	int	$29
	retq
	.size kkm_forward_intr_29, .-kkm_forward_intr_29

	.align 16
	.global kkm_forward_intr_30
kkm_forward_intr_30:
	int	$30
	retq
	.size kkm_forward_intr_30, .-kkm_forward_intr_30

	.align 16
	.global kkm_forward_intr_31
kkm_forward_intr_31:
	int	$31
	retq
	.size kkm_forward_intr_31, .-kkm_forward_intr_31

	.align 16
	.global kkm_forward_intr_32
kkm_forward_intr_32:
	int	$32
	retq
	.size kkm_forward_intr_32, .-kkm_forward_intr_32

	.align 16
	.global kkm_forward_intr_33
kkm_forward_intr_33:
	int	$33
	retq
	.size kkm_forward_intr_33, .-kkm_forward_intr_33

	.align 16
	.global kkm_forward_intr_34
kkm_forward_intr_34:
	int	$34
	retq
	.size kkm_forward_intr_34, .-kkm_forward_intr_34

	.align 16
	.global kkm_forward_intr_35
kkm_forward_intr_35:
	int	$35
	retq
	.size kkm_forward_intr_35, .-kkm_forward_intr_35

	.align 16
	.global kkm_forward_intr_36
kkm_forward_intr_36:
	int	$36
	retq
	.size kkm_forward_intr_36, .-kkm_forward_intr_36

	.align 16
	.global kkm_forward_intr_37
kkm_forward_intr_37:
	int	$37
	retq
	.size kkm_forward_intr_37, .-kkm_forward_intr_37

	.align 16
	.global kkm_forward_intr_38
kkm_forward_intr_38:
	int	$38
	retq
	.size kkm_forward_intr_38, .-kkm_forward_intr_38

	.align 16
	.global kkm_forward_intr_39
kkm_forward_intr_39:
	int	$39
	retq
	.size kkm_forward_intr_39, .-kkm_forward_intr_39

	.align 16
	.global kkm_forward_intr_40
kkm_forward_intr_40:
	int	$40
	retq
	.size kkm_forward_intr_40, .-kkm_forward_intr_40

	.align 16
	.global kkm_forward_intr_41
kkm_forward_intr_41:
	int	$41
	retq
	.size kkm_forward_intr_41, .-kkm_forward_intr_41

	.align 16
	.global kkm_forward_intr_42
kkm_forward_intr_42:
	int	$42
	retq
	.size kkm_forward_intr_42, .-kkm_forward_intr_42

	.align 16
	.global kkm_forward_intr_43
kkm_forward_intr_43:
	int	$43
	retq
	.size kkm_forward_intr_43, .-kkm_forward_intr_43

	.align 16
	.global kkm_forward_intr_44
kkm_forward_intr_44:
	int	$44
	retq
	.size kkm_forward_intr_44, .-kkm_forward_intr_44

	.align 16
	.global kkm_forward_intr_45
kkm_forward_intr_45:
	int	$45
	retq
	.size kkm_forward_intr_45, .-kkm_forward_intr_45

	.align 16
	.global kkm_forward_intr_46
kkm_forward_intr_46:
	int	$46
	retq
	.size kkm_forward_intr_46, .-kkm_forward_intr_46

	.align 16
	.global kkm_forward_intr_47
kkm_forward_intr_47:
	int	$47
	retq
	.size kkm_forward_intr_47, .-kkm_forward_intr_47

	.align 16
	.global kkm_forward_intr_48
kkm_forward_intr_48:
	int	$48
	retq
	.size kkm_forward_intr_48, .-kkm_forward_intr_48

	.align 16
	.global kkm_forward_intr_49
kkm_forward_intr_49:
	int	$49
	retq
	.size kkm_forward_intr_49, .-kkm_forward_intr_49

	.align 16
	.global kkm_forward_intr_50
kkm_forward_intr_50:
	int	$50
	retq
	.size kkm_forward_intr_50, .-kkm_forward_intr_50

	.align 16
	.global kkm_forward_intr_51
kkm_forward_intr_51:
	int	$51
	retq
	.size kkm_forward_intr_51, .-kkm_forward_intr_51

	.align 16
	.global kkm_forward_intr_52
kkm_forward_intr_52:
	int	$52
	retq
	.size kkm_forward_intr_52, .-kkm_forward_intr_52

	.align 16
	.global kkm_forward_intr_53
kkm_forward_intr_53:
	int	$53
	retq
	.size kkm_forward_intr_53, .-kkm_forward_intr_53

	.align 16
	.global kkm_forward_intr_54
kkm_forward_intr_54:
	int	$54
	retq
	.size kkm_forward_intr_54, .-kkm_forward_intr_54

	.align 16
	.global kkm_forward_intr_55
kkm_forward_intr_55:
	int	$55
	retq
	.size kkm_forward_intr_55, .-kkm_forward_intr_55

	.align 16
	.global kkm_forward_intr_56
kkm_forward_intr_56:
	int	$56
	retq
	.size kkm_forward_intr_56, .-kkm_forward_intr_56

	.align 16
	.global kkm_forward_intr_57
kkm_forward_intr_57:
	int	$57
	retq
	.size kkm_forward_intr_57, .-kkm_forward_intr_57

	.align 16
	.global kkm_forward_intr_58
kkm_forward_intr_58:
	int	$58
	retq
	.size kkm_forward_intr_58, .-kkm_forward_intr_58

	.align 16
	.global kkm_forward_intr_59
kkm_forward_intr_59:
	int	$59
	retq
	.size kkm_forward_intr_59, .-kkm_forward_intr_59

	.align 16
	.global kkm_forward_intr_60
kkm_forward_intr_60:
	int	$60
	retq
	.size kkm_forward_intr_60, .-kkm_forward_intr_60

	.align 16
	.global kkm_forward_intr_61
kkm_forward_intr_61:
	int	$61
	retq
	.size kkm_forward_intr_61, .-kkm_forward_intr_61

	.align 16
	.global kkm_forward_intr_62
kkm_forward_intr_62:
	int	$62
	retq
	.size kkm_forward_intr_62, .-kkm_forward_intr_62

	.align 16
	.global kkm_forward_intr_63
kkm_forward_intr_63:
	int	$63
	retq
	.size kkm_forward_intr_63, .-kkm_forward_intr_63

	.align 16
	.global kkm_forward_intr_64
kkm_forward_intr_64:
	int	$64
	retq
	.size kkm_forward_intr_64, .-kkm_forward_intr_64

	.align 16
	.global kkm_forward_intr_65
kkm_forward_intr_65:
	int	$65
	retq
	.size kkm_forward_intr_65, .-kkm_forward_intr_65

	.align 16
	.global kkm_forward_intr_66
kkm_forward_intr_66:
	int	$66
	retq
	.size kkm_forward_intr_66, .-kkm_forward_intr_66

	.align 16
	.global kkm_forward_intr_67
kkm_forward_intr_67:
	int	$67
	retq
	.size kkm_forward_intr_67, .-kkm_forward_intr_67

	.align 16
	.global kkm_forward_intr_68
kkm_forward_intr_68:
	int	$68
	retq
	.size kkm_forward_intr_68, .-kkm_forward_intr_68

	.align 16
	.global kkm_forward_intr_69
kkm_forward_intr_69:
	int	$69
	retq
	.size kkm_forward_intr_69, .-kkm_forward_intr_69

	.align 16
	.global kkm_forward_intr_70
kkm_forward_intr_70:
	int	$70
	retq
	.size kkm_forward_intr_70, .-kkm_forward_intr_70

	.align 16
	.global kkm_forward_intr_71
kkm_forward_intr_71:
	int	$71
	retq
	.size kkm_forward_intr_71, .-kkm_forward_intr_71

	.align 16
	.global kkm_forward_intr_72
kkm_forward_intr_72:
	int	$72
	retq
	.size kkm_forward_intr_72, .-kkm_forward_intr_72

	.align 16
	.global kkm_forward_intr_73
kkm_forward_intr_73:
	int	$73
	retq
	.size kkm_forward_intr_73, .-kkm_forward_intr_73

	.align 16
	.global kkm_forward_intr_74
kkm_forward_intr_74:
	int	$74
	retq
	.size kkm_forward_intr_74, .-kkm_forward_intr_74

	.align 16
	.global kkm_forward_intr_75
kkm_forward_intr_75:
	int	$75
	retq
	.size kkm_forward_intr_75, .-kkm_forward_intr_75

	.align 16
	.global kkm_forward_intr_76
kkm_forward_intr_76:
	int	$76
	retq
	.size kkm_forward_intr_76, .-kkm_forward_intr_76

	.align 16
	.global kkm_forward_intr_77
kkm_forward_intr_77:
	int	$77
	retq
	.size kkm_forward_intr_77, .-kkm_forward_intr_77

	.align 16
	.global kkm_forward_intr_78
kkm_forward_intr_78:
	int	$78
	retq
	.size kkm_forward_intr_78, .-kkm_forward_intr_78

	.align 16
	.global kkm_forward_intr_79
kkm_forward_intr_79:
	int	$79
	retq
	.size kkm_forward_intr_79, .-kkm_forward_intr_79

	.align 16
	.global kkm_forward_intr_80
kkm_forward_intr_80:
	int	$80
	retq
	.size kkm_forward_intr_80, .-kkm_forward_intr_80

	.align 16
	.global kkm_forward_intr_81
kkm_forward_intr_81:
	int	$81
	retq
	.size kkm_forward_intr_81, .-kkm_forward_intr_81

	.align 16
	.global kkm_forward_intr_82
kkm_forward_intr_82:
	int	$82
	retq
	.size kkm_forward_intr_82, .-kkm_forward_intr_82

	.align 16
	.global kkm_forward_intr_83
kkm_forward_intr_83:
	int	$83
	retq
	.size kkm_forward_intr_83, .-kkm_forward_intr_83

	.align 16
	.global kkm_forward_intr_84
kkm_forward_intr_84:
	int	$84
	retq
	.size kkm_forward_intr_84, .-kkm_forward_intr_84

	.align 16
	.global kkm_forward_intr_85
kkm_forward_intr_85:
	int	$85
	retq
	.size kkm_forward_intr_85, .-kkm_forward_intr_85

	.align 16
	.global kkm_forward_intr_86
kkm_forward_intr_86:
	int	$86
	retq
	.size kkm_forward_intr_86, .-kkm_forward_intr_86

	.align 16
	.global kkm_forward_intr_87
kkm_forward_intr_87:
	int	$87
	retq
	.size kkm_forward_intr_87, .-kkm_forward_intr_87

	.align 16
	.global kkm_forward_intr_88
kkm_forward_intr_88:
	int	$88
	retq
	.size kkm_forward_intr_88, .-kkm_forward_intr_88

	.align 16
	.global kkm_forward_intr_89
kkm_forward_intr_89:
	int	$89
	retq
	.size kkm_forward_intr_89, .-kkm_forward_intr_89

	.align 16
	.global kkm_forward_intr_90
kkm_forward_intr_90:
	int	$90
	retq
	.size kkm_forward_intr_90, .-kkm_forward_intr_90

	.align 16
	.global kkm_forward_intr_91
kkm_forward_intr_91:
	int	$91
	retq
	.size kkm_forward_intr_91, .-kkm_forward_intr_91

	.align 16
	.global kkm_forward_intr_92
kkm_forward_intr_92:
	int	$92
	retq
	.size kkm_forward_intr_92, .-kkm_forward_intr_92

	.align 16
	.global kkm_forward_intr_93
kkm_forward_intr_93:
	int	$93
	retq
	.size kkm_forward_intr_93, .-kkm_forward_intr_93

	.align 16
	.global kkm_forward_intr_94
kkm_forward_intr_94:
	int	$94
	retq
	.size kkm_forward_intr_94, .-kkm_forward_intr_94

	.align 16
	.global kkm_forward_intr_95
kkm_forward_intr_95:
	int	$95
	retq
	.size kkm_forward_intr_95, .-kkm_forward_intr_95

	.align 16
	.global kkm_forward_intr_96
kkm_forward_intr_96:
	int	$96
	retq
	.size kkm_forward_intr_96, .-kkm_forward_intr_96

	.align 16
	.global kkm_forward_intr_97
kkm_forward_intr_97:
	int	$97
	retq
	.size kkm_forward_intr_97, .-kkm_forward_intr_97

	.align 16
	.global kkm_forward_intr_98
kkm_forward_intr_98:
	int	$98
	retq
	.size kkm_forward_intr_98, .-kkm_forward_intr_98

	.align 16
	.global kkm_forward_intr_99
kkm_forward_intr_99:
	int	$99
	retq
	.size kkm_forward_intr_99, .-kkm_forward_intr_99

	.align 16
	.global kkm_forward_intr_100
kkm_forward_intr_100:
	int	$100
	retq
	.size kkm_forward_intr_100, .-kkm_forward_intr_100

	.align 16
	.global kkm_forward_intr_101
kkm_forward_intr_101:
	int	$101
	retq
	.size kkm_forward_intr_101, .-kkm_forward_intr_101

	.align 16
	.global kkm_forward_intr_102
kkm_forward_intr_102:
	int	$102
	retq
	.size kkm_forward_intr_102, .-kkm_forward_intr_102

	.align 16
	.global kkm_forward_intr_103
kkm_forward_intr_103:
	int	$103
	retq
	.size kkm_forward_intr_103, .-kkm_forward_intr_103

	.align 16
	.global kkm_forward_intr_104
kkm_forward_intr_104:
	int	$104
	retq
	.size kkm_forward_intr_104, .-kkm_forward_intr_104

	.align 16
	.global kkm_forward_intr_105
kkm_forward_intr_105:
	int	$105
	retq
	.size kkm_forward_intr_105, .-kkm_forward_intr_105

	.align 16
	.global kkm_forward_intr_106
kkm_forward_intr_106:
	int	$106
	retq
	.size kkm_forward_intr_106, .-kkm_forward_intr_106

	.align 16
	.global kkm_forward_intr_107
kkm_forward_intr_107:
	int	$107
	retq
	.size kkm_forward_intr_107, .-kkm_forward_intr_107

	.align 16
	.global kkm_forward_intr_108
kkm_forward_intr_108:
	int	$108
	retq
	.size kkm_forward_intr_108, .-kkm_forward_intr_108

	.align 16
	.global kkm_forward_intr_109
kkm_forward_intr_109:
	int	$109
	retq
	.size kkm_forward_intr_109, .-kkm_forward_intr_109

	.align 16
	.global kkm_forward_intr_110
kkm_forward_intr_110:
	int	$110
	retq
	.size kkm_forward_intr_110, .-kkm_forward_intr_110

	.align 16
	.global kkm_forward_intr_111
kkm_forward_intr_111:
	int	$111
	retq
	.size kkm_forward_intr_111, .-kkm_forward_intr_111

	.align 16
	.global kkm_forward_intr_112
kkm_forward_intr_112:
	int	$112
	retq
	.size kkm_forward_intr_112, .-kkm_forward_intr_112

	.align 16
	.global kkm_forward_intr_113
kkm_forward_intr_113:
	int	$113
	retq
	.size kkm_forward_intr_113, .-kkm_forward_intr_113

	.align 16
	.global kkm_forward_intr_114
kkm_forward_intr_114:
	int	$114
	retq
	.size kkm_forward_intr_114, .-kkm_forward_intr_114

	.align 16
	.global kkm_forward_intr_115
kkm_forward_intr_115:
	int	$115
	retq
	.size kkm_forward_intr_115, .-kkm_forward_intr_115

	.align 16
	.global kkm_forward_intr_116
kkm_forward_intr_116:
	int	$116
	retq
	.size kkm_forward_intr_116, .-kkm_forward_intr_116

	.align 16
	.global kkm_forward_intr_117
kkm_forward_intr_117:
	int	$117
	retq
	.size kkm_forward_intr_117, .-kkm_forward_intr_117

	.align 16
	.global kkm_forward_intr_118
kkm_forward_intr_118:
	int	$118
	retq
	.size kkm_forward_intr_118, .-kkm_forward_intr_118

	.align 16
	.global kkm_forward_intr_119
kkm_forward_intr_119:
	int	$119
	retq
	.size kkm_forward_intr_119, .-kkm_forward_intr_119

	.align 16
	.global kkm_forward_intr_120
kkm_forward_intr_120:
	int	$120
	retq
	.size kkm_forward_intr_120, .-kkm_forward_intr_120

	.align 16
	.global kkm_forward_intr_121
kkm_forward_intr_121:
	int	$121
	retq
	.size kkm_forward_intr_121, .-kkm_forward_intr_121

	.align 16
	.global kkm_forward_intr_122
kkm_forward_intr_122:
	int	$122
	retq
	.size kkm_forward_intr_122, .-kkm_forward_intr_122

	.align 16
	.global kkm_forward_intr_123
kkm_forward_intr_123:
	int	$123
	retq
	.size kkm_forward_intr_123, .-kkm_forward_intr_123

	.align 16
	.global kkm_forward_intr_124
kkm_forward_intr_124:
	int	$124
	retq
	.size kkm_forward_intr_124, .-kkm_forward_intr_124

	.align 16
	.global kkm_forward_intr_125
kkm_forward_intr_125:
	int	$125
	retq
	.size kkm_forward_intr_125, .-kkm_forward_intr_125

	.align 16
	.global kkm_forward_intr_126
kkm_forward_intr_126:
	int	$126
	retq
	.size kkm_forward_intr_126, .-kkm_forward_intr_126

	.align 16
	.global kkm_forward_intr_127
kkm_forward_intr_127:
	int	$127
	retq
	.size kkm_forward_intr_127, .-kkm_forward_intr_127

	.align 16
	.global kkm_forward_intr_128
kkm_forward_intr_128:
	int	$128
	retq
	.size kkm_forward_intr_128, .-kkm_forward_intr_128

	.align 16
	.global kkm_forward_intr_129
kkm_forward_intr_129:
	int	$129
	retq
	.size kkm_forward_intr_129, .-kkm_forward_intr_129

	.align 16
	.global kkm_forward_intr_130
kkm_forward_intr_130:
	int	$130
	retq
	.size kkm_forward_intr_130, .-kkm_forward_intr_130

	.align 16
	.global kkm_forward_intr_131
kkm_forward_intr_131:
	int	$131
	retq
	.size kkm_forward_intr_131, .-kkm_forward_intr_131

	.align 16
	.global kkm_forward_intr_132
kkm_forward_intr_132:
	int	$132
	retq
	.size kkm_forward_intr_132, .-kkm_forward_intr_132

	.align 16
	.global kkm_forward_intr_133
kkm_forward_intr_133:
	int	$133
	retq
	.size kkm_forward_intr_133, .-kkm_forward_intr_133

	.align 16
	.global kkm_forward_intr_134
kkm_forward_intr_134:
	int	$134
	retq
	.size kkm_forward_intr_134, .-kkm_forward_intr_134

	.align 16
	.global kkm_forward_intr_135
kkm_forward_intr_135:
	int	$135
	retq
	.size kkm_forward_intr_135, .-kkm_forward_intr_135

	.align 16
	.global kkm_forward_intr_136
kkm_forward_intr_136:
	int	$136
	retq
	.size kkm_forward_intr_136, .-kkm_forward_intr_136

	.align 16
	.global kkm_forward_intr_137
kkm_forward_intr_137:
	int	$137
	retq
	.size kkm_forward_intr_137, .-kkm_forward_intr_137

	.align 16
	.global kkm_forward_intr_138
kkm_forward_intr_138:
	int	$138
	retq
	.size kkm_forward_intr_138, .-kkm_forward_intr_138

	.align 16
	.global kkm_forward_intr_139
kkm_forward_intr_139:
	int	$139
	retq
	.size kkm_forward_intr_139, .-kkm_forward_intr_139

	.align 16
	.global kkm_forward_intr_140
kkm_forward_intr_140:
	int	$140
	retq
	.size kkm_forward_intr_140, .-kkm_forward_intr_140

	.align 16
	.global kkm_forward_intr_141
kkm_forward_intr_141:
	int	$141
	retq
	.size kkm_forward_intr_141, .-kkm_forward_intr_141

	.align 16
	.global kkm_forward_intr_142
kkm_forward_intr_142:
	int	$142
	retq
	.size kkm_forward_intr_142, .-kkm_forward_intr_142

	.align 16
	.global kkm_forward_intr_143
kkm_forward_intr_143:
	int	$143
	retq
	.size kkm_forward_intr_143, .-kkm_forward_intr_143

	.align 16
	.global kkm_forward_intr_144
kkm_forward_intr_144:
	int	$144
	retq
	.size kkm_forward_intr_144, .-kkm_forward_intr_144

	.align 16
	.global kkm_forward_intr_145
kkm_forward_intr_145:
	int	$145
	retq
	.size kkm_forward_intr_145, .-kkm_forward_intr_145

	.align 16
	.global kkm_forward_intr_146
kkm_forward_intr_146:
	int	$146
	retq
	.size kkm_forward_intr_146, .-kkm_forward_intr_146

	.align 16
	.global kkm_forward_intr_147
kkm_forward_intr_147:
	int	$147
	retq
	.size kkm_forward_intr_147, .-kkm_forward_intr_147

	.align 16
	.global kkm_forward_intr_148
kkm_forward_intr_148:
	int	$148
	retq
	.size kkm_forward_intr_148, .-kkm_forward_intr_148

	.align 16
	.global kkm_forward_intr_149
kkm_forward_intr_149:
	int	$149
	retq
	.size kkm_forward_intr_149, .-kkm_forward_intr_149

	.align 16
	.global kkm_forward_intr_150
kkm_forward_intr_150:
	int	$150
	retq
	.size kkm_forward_intr_150, .-kkm_forward_intr_150

	.align 16
	.global kkm_forward_intr_151
kkm_forward_intr_151:
	int	$151
	retq
	.size kkm_forward_intr_151, .-kkm_forward_intr_151

	.align 16
	.global kkm_forward_intr_152
kkm_forward_intr_152:
	int	$152
	retq
	.size kkm_forward_intr_152, .-kkm_forward_intr_152

	.align 16
	.global kkm_forward_intr_153
kkm_forward_intr_153:
	int	$153
	retq
	.size kkm_forward_intr_153, .-kkm_forward_intr_153

	.align 16
	.global kkm_forward_intr_154
kkm_forward_intr_154:
	int	$154
	retq
	.size kkm_forward_intr_154, .-kkm_forward_intr_154

	.align 16
	.global kkm_forward_intr_155
kkm_forward_intr_155:
	int	$155
	retq
	.size kkm_forward_intr_155, .-kkm_forward_intr_155

	.align 16
	.global kkm_forward_intr_156
kkm_forward_intr_156:
	int	$156
	retq
	.size kkm_forward_intr_156, .-kkm_forward_intr_156

	.align 16
	.global kkm_forward_intr_157
kkm_forward_intr_157:
	int	$157
	retq
	.size kkm_forward_intr_157, .-kkm_forward_intr_157

	.align 16
	.global kkm_forward_intr_158
kkm_forward_intr_158:
	int	$158
	retq
	.size kkm_forward_intr_158, .-kkm_forward_intr_158

	.align 16
	.global kkm_forward_intr_159
kkm_forward_intr_159:
	int	$159
	retq
	.size kkm_forward_intr_159, .-kkm_forward_intr_159

	.align 16
	.global kkm_forward_intr_160
kkm_forward_intr_160:
	int	$160
	retq
	.size kkm_forward_intr_160, .-kkm_forward_intr_160

	.align 16
	.global kkm_forward_intr_161
kkm_forward_intr_161:
	int	$161
	retq
	.size kkm_forward_intr_161, .-kkm_forward_intr_161

	.align 16
	.global kkm_forward_intr_162
kkm_forward_intr_162:
	int	$162
	retq
	.size kkm_forward_intr_162, .-kkm_forward_intr_162

	.align 16
	.global kkm_forward_intr_163
kkm_forward_intr_163:
	int	$163
	retq
	.size kkm_forward_intr_163, .-kkm_forward_intr_163

	.align 16
	.global kkm_forward_intr_164
kkm_forward_intr_164:
	int	$164
	retq
	.size kkm_forward_intr_164, .-kkm_forward_intr_164

	.align 16
	.global kkm_forward_intr_165
kkm_forward_intr_165:
	int	$165
	retq
	.size kkm_forward_intr_165, .-kkm_forward_intr_165

	.align 16
	.global kkm_forward_intr_166
kkm_forward_intr_166:
	int	$166
	retq
	.size kkm_forward_intr_166, .-kkm_forward_intr_166

	.align 16
	.global kkm_forward_intr_167
kkm_forward_intr_167:
	int	$167
	retq
	.size kkm_forward_intr_167, .-kkm_forward_intr_167

	.align 16
	.global kkm_forward_intr_168
kkm_forward_intr_168:
	int	$168
	retq
	.size kkm_forward_intr_168, .-kkm_forward_intr_168

	.align 16
	.global kkm_forward_intr_169
kkm_forward_intr_169:
	int	$169
	retq
	.size kkm_forward_intr_169, .-kkm_forward_intr_169

	.align 16
	.global kkm_forward_intr_170
kkm_forward_intr_170:
	int	$170
	retq
	.size kkm_forward_intr_170, .-kkm_forward_intr_170

	.align 16
	.global kkm_forward_intr_171
kkm_forward_intr_171:
	int	$171
	retq
	.size kkm_forward_intr_171, .-kkm_forward_intr_171

	.align 16
	.global kkm_forward_intr_172
kkm_forward_intr_172:
	int	$172
	retq
	.size kkm_forward_intr_172, .-kkm_forward_intr_172

	.align 16
	.global kkm_forward_intr_173
kkm_forward_intr_173:
	int	$173
	retq
	.size kkm_forward_intr_173, .-kkm_forward_intr_173

	.align 16
	.global kkm_forward_intr_174
kkm_forward_intr_174:
	int	$174
	retq
	.size kkm_forward_intr_174, .-kkm_forward_intr_174

	.align 16
	.global kkm_forward_intr_175
kkm_forward_intr_175:
	int	$175
	retq
	.size kkm_forward_intr_175, .-kkm_forward_intr_175

	.align 16
	.global kkm_forward_intr_176
kkm_forward_intr_176:
	int	$176
	retq
	.size kkm_forward_intr_176, .-kkm_forward_intr_176

	.align 16
	.global kkm_forward_intr_177
kkm_forward_intr_177:
	int	$177
	retq
	.size kkm_forward_intr_177, .-kkm_forward_intr_177

	.align 16
	.global kkm_forward_intr_178
kkm_forward_intr_178:
	int	$178
	retq
	.size kkm_forward_intr_178, .-kkm_forward_intr_178

	.align 16
	.global kkm_forward_intr_179
kkm_forward_intr_179:
	int	$179
	retq
	.size kkm_forward_intr_179, .-kkm_forward_intr_179

	.align 16
	.global kkm_forward_intr_180
kkm_forward_intr_180:
	int	$180
	retq
	.size kkm_forward_intr_180, .-kkm_forward_intr_180

	.align 16
	.global kkm_forward_intr_181
kkm_forward_intr_181:
	int	$181
	retq
	.size kkm_forward_intr_181, .-kkm_forward_intr_181

	.align 16
	.global kkm_forward_intr_182
kkm_forward_intr_182:
	int	$182
	retq
	.size kkm_forward_intr_182, .-kkm_forward_intr_182

	.align 16
	.global kkm_forward_intr_183
kkm_forward_intr_183:
	int	$183
	retq
	.size kkm_forward_intr_183, .-kkm_forward_intr_183

	.align 16
	.global kkm_forward_intr_184
kkm_forward_intr_184:
	int	$184
	retq
	.size kkm_forward_intr_184, .-kkm_forward_intr_184

	.align 16
	.global kkm_forward_intr_185
kkm_forward_intr_185:
	int	$185
	retq
	.size kkm_forward_intr_185, .-kkm_forward_intr_185

	.align 16
	.global kkm_forward_intr_186
kkm_forward_intr_186:
	int	$186
	retq
	.size kkm_forward_intr_186, .-kkm_forward_intr_186

	.align 16
	.global kkm_forward_intr_187
kkm_forward_intr_187:
	int	$187
	retq
	.size kkm_forward_intr_187, .-kkm_forward_intr_187

	.align 16
	.global kkm_forward_intr_188
kkm_forward_intr_188:
	int	$188
	retq
	.size kkm_forward_intr_188, .-kkm_forward_intr_188

	.align 16
	.global kkm_forward_intr_189
kkm_forward_intr_189:
	int	$189
	retq
	.size kkm_forward_intr_189, .-kkm_forward_intr_189

	.align 16
	.global kkm_forward_intr_190
kkm_forward_intr_190:
	int	$190
	retq
	.size kkm_forward_intr_190, .-kkm_forward_intr_190

	.align 16
	.global kkm_forward_intr_191
kkm_forward_intr_191:
	int	$191
	retq
	.size kkm_forward_intr_191, .-kkm_forward_intr_191

	.align 16
	.global kkm_forward_intr_192
kkm_forward_intr_192:
	int	$192
	retq
	.size kkm_forward_intr_192, .-kkm_forward_intr_192

	.align 16
	.global kkm_forward_intr_193
kkm_forward_intr_193:
	int	$193
	retq
	.size kkm_forward_intr_193, .-kkm_forward_intr_193

	.align 16
	.global kkm_forward_intr_194
kkm_forward_intr_194:
	int	$194
	retq
	.size kkm_forward_intr_194, .-kkm_forward_intr_194

	.align 16
	.global kkm_forward_intr_195
kkm_forward_intr_195:
	int	$195
	retq
	.size kkm_forward_intr_195, .-kkm_forward_intr_195

	.align 16
	.global kkm_forward_intr_196
kkm_forward_intr_196:
	int	$196
	retq
	.size kkm_forward_intr_196, .-kkm_forward_intr_196

	.align 16
	.global kkm_forward_intr_197
kkm_forward_intr_197:
	int	$197
	retq
	.size kkm_forward_intr_197, .-kkm_forward_intr_197

	.align 16
	.global kkm_forward_intr_198
kkm_forward_intr_198:
	int	$198
	retq
	.size kkm_forward_intr_198, .-kkm_forward_intr_198

	.align 16
	.global kkm_forward_intr_199
kkm_forward_intr_199:
	int	$199
	retq
	.size kkm_forward_intr_199, .-kkm_forward_intr_199

	.align 16
	.global kkm_forward_intr_200
kkm_forward_intr_200:
	int	$200
	retq
	.size kkm_forward_intr_200, .-kkm_forward_intr_200

	.align 16
	.global kkm_forward_intr_201
kkm_forward_intr_201:
	int	$201
	retq
	.size kkm_forward_intr_201, .-kkm_forward_intr_201

	.align 16
	.global kkm_forward_intr_202
kkm_forward_intr_202:
	int	$202
	retq
	.size kkm_forward_intr_202, .-kkm_forward_intr_202

	.align 16
	.global kkm_forward_intr_203
kkm_forward_intr_203:
	int	$203
	retq
	.size kkm_forward_intr_203, .-kkm_forward_intr_203

	.align 16
	.global kkm_forward_intr_204
kkm_forward_intr_204:
	int	$204
	retq
	.size kkm_forward_intr_204, .-kkm_forward_intr_204

	.align 16
	.global kkm_forward_intr_205
kkm_forward_intr_205:
	int	$205
	retq
	.size kkm_forward_intr_205, .-kkm_forward_intr_205

	.align 16
	.global kkm_forward_intr_206
kkm_forward_intr_206:
	int	$206
	retq
	.size kkm_forward_intr_206, .-kkm_forward_intr_206

	.align 16
	.global kkm_forward_intr_207
kkm_forward_intr_207:
	int	$207
	retq
	.size kkm_forward_intr_207, .-kkm_forward_intr_207

	.align 16
	.global kkm_forward_intr_208
kkm_forward_intr_208:
	int	$208
	retq
	.size kkm_forward_intr_208, .-kkm_forward_intr_208

	.align 16
	.global kkm_forward_intr_209
kkm_forward_intr_209:
	int	$209
	retq
	.size kkm_forward_intr_209, .-kkm_forward_intr_209

	.align 16
	.global kkm_forward_intr_210
kkm_forward_intr_210:
	int	$210
	retq
	.size kkm_forward_intr_210, .-kkm_forward_intr_210

	.align 16
	.global kkm_forward_intr_211
kkm_forward_intr_211:
	int	$211
	retq
	.size kkm_forward_intr_211, .-kkm_forward_intr_211

	.align 16
	.global kkm_forward_intr_212
kkm_forward_intr_212:
	int	$212
	retq
	.size kkm_forward_intr_212, .-kkm_forward_intr_212

	.align 16
	.global kkm_forward_intr_213
kkm_forward_intr_213:
	int	$213
	retq
	.size kkm_forward_intr_213, .-kkm_forward_intr_213

	.align 16
	.global kkm_forward_intr_214
kkm_forward_intr_214:
	int	$214
	retq
	.size kkm_forward_intr_214, .-kkm_forward_intr_214

	.align 16
	.global kkm_forward_intr_215
kkm_forward_intr_215:
	int	$215
	retq
	.size kkm_forward_intr_215, .-kkm_forward_intr_215

	.align 16
	.global kkm_forward_intr_216
kkm_forward_intr_216:
	int	$216
	retq
	.size kkm_forward_intr_216, .-kkm_forward_intr_216

	.align 16
	.global kkm_forward_intr_217
kkm_forward_intr_217:
	int	$217
	retq
	.size kkm_forward_intr_217, .-kkm_forward_intr_217

	.align 16
	.global kkm_forward_intr_218
kkm_forward_intr_218:
	int	$218
	retq
	.size kkm_forward_intr_218, .-kkm_forward_intr_218

	.align 16
	.global kkm_forward_intr_219
kkm_forward_intr_219:
	int	$219
	retq
	.size kkm_forward_intr_219, .-kkm_forward_intr_219

	.align 16
	.global kkm_forward_intr_220
kkm_forward_intr_220:
	int	$220
	retq
	.size kkm_forward_intr_220, .-kkm_forward_intr_220

	.align 16
	.global kkm_forward_intr_221
kkm_forward_intr_221:
	int	$221
	retq
	.size kkm_forward_intr_221, .-kkm_forward_intr_221

	.align 16
	.global kkm_forward_intr_222
kkm_forward_intr_222:
	int	$222
	retq
	.size kkm_forward_intr_222, .-kkm_forward_intr_222

	.align 16
	.global kkm_forward_intr_223
kkm_forward_intr_223:
	int	$223
	retq
	.size kkm_forward_intr_223, .-kkm_forward_intr_223

	.align 16
	.global kkm_forward_intr_224
kkm_forward_intr_224:
	int	$224
	retq
	.size kkm_forward_intr_224, .-kkm_forward_intr_224

	.align 16
	.global kkm_forward_intr_225
kkm_forward_intr_225:
	int	$225
	retq
	.size kkm_forward_intr_225, .-kkm_forward_intr_225

	.align 16
	.global kkm_forward_intr_226
kkm_forward_intr_226:
	int	$226
	retq
	.size kkm_forward_intr_226, .-kkm_forward_intr_226

	.align 16
	.global kkm_forward_intr_227
kkm_forward_intr_227:
	int	$227
	retq
	.size kkm_forward_intr_227, .-kkm_forward_intr_227

	.align 16
	.global kkm_forward_intr_228
kkm_forward_intr_228:
	int	$228
	retq
	.size kkm_forward_intr_228, .-kkm_forward_intr_228

	.align 16
	.global kkm_forward_intr_229
kkm_forward_intr_229:
	int	$229
	retq
	.size kkm_forward_intr_229, .-kkm_forward_intr_229

	.align 16
	.global kkm_forward_intr_230
kkm_forward_intr_230:
	int	$230
	retq
	.size kkm_forward_intr_230, .-kkm_forward_intr_230

	.align 16
	.global kkm_forward_intr_231
kkm_forward_intr_231:
	int	$231
	retq
	.size kkm_forward_intr_231, .-kkm_forward_intr_231

	.align 16
	.global kkm_forward_intr_232
kkm_forward_intr_232:
	int	$232
	retq
	.size kkm_forward_intr_232, .-kkm_forward_intr_232

	.align 16
	.global kkm_forward_intr_233
kkm_forward_intr_233:
	int	$233
	retq
	.size kkm_forward_intr_233, .-kkm_forward_intr_233

	.align 16
	.global kkm_forward_intr_234
kkm_forward_intr_234:
	int	$234
	retq
	.size kkm_forward_intr_234, .-kkm_forward_intr_234

	.align 16
	.global kkm_forward_intr_235
kkm_forward_intr_235:
	int	$235
	retq
	.size kkm_forward_intr_235, .-kkm_forward_intr_235

	.align 16
	.global kkm_forward_intr_236
kkm_forward_intr_236:
	int	$236
	retq
	.size kkm_forward_intr_236, .-kkm_forward_intr_236

	.align 16
	.global kkm_forward_intr_237
kkm_forward_intr_237:
	int	$237
	retq
	.size kkm_forward_intr_237, .-kkm_forward_intr_237

	.align 16
	.global kkm_forward_intr_238
kkm_forward_intr_238:
	int	$238
	retq
	.size kkm_forward_intr_238, .-kkm_forward_intr_238

	.align 16
	.global kkm_forward_intr_239
kkm_forward_intr_239:
	int	$239
	retq
	.size kkm_forward_intr_239, .-kkm_forward_intr_239

	.align 16
	.global kkm_forward_intr_240
kkm_forward_intr_240:
	int	$240
	retq
	.size kkm_forward_intr_240, .-kkm_forward_intr_240

	.align 16
	.global kkm_forward_intr_241
kkm_forward_intr_241:
	int	$241
	retq
	.size kkm_forward_intr_241, .-kkm_forward_intr_241

	.align 16
	.global kkm_forward_intr_242
kkm_forward_intr_242:
	int	$242
	retq
	.size kkm_forward_intr_242, .-kkm_forward_intr_242

	.align 16
	.global kkm_forward_intr_243
kkm_forward_intr_243:
	int	$243
	retq
	.size kkm_forward_intr_243, .-kkm_forward_intr_243

	.align 16
	.global kkm_forward_intr_244
kkm_forward_intr_244:
	int	$244
	retq
	.size kkm_forward_intr_244, .-kkm_forward_intr_244

	.align 16
	.global kkm_forward_intr_245
kkm_forward_intr_245:
	int	$245
	retq
	.size kkm_forward_intr_245, .-kkm_forward_intr_245

	.align 16
	.global kkm_forward_intr_246
kkm_forward_intr_246:
	int	$246
	retq
	.size kkm_forward_intr_246, .-kkm_forward_intr_246

	.align 16
	.global kkm_forward_intr_247
kkm_forward_intr_247:
	int	$247
	retq
	.size kkm_forward_intr_247, .-kkm_forward_intr_247

	.align 16
	.global kkm_forward_intr_248
kkm_forward_intr_248:
	int	$248
	retq
	.size kkm_forward_intr_248, .-kkm_forward_intr_248

	.align 16
	.global kkm_forward_intr_249
kkm_forward_intr_249:
	int	$249
	retq
	.size kkm_forward_intr_249, .-kkm_forward_intr_249

	.align 16
	.global kkm_forward_intr_250
kkm_forward_intr_250:
	int	$250
	retq
	.size kkm_forward_intr_250, .-kkm_forward_intr_250

	.align 16
	.global kkm_forward_intr_251
kkm_forward_intr_251:
	int	$251
	retq
	.size kkm_forward_intr_251, .-kkm_forward_intr_251

	.align 16
	.global kkm_forward_intr_252
kkm_forward_intr_252:
	int	$252
	retq
	.size kkm_forward_intr_252, .-kkm_forward_intr_252

	.align 16
	.global kkm_forward_intr_253
kkm_forward_intr_253:
	int	$253
	retq
	.size kkm_forward_intr_253, .-kkm_forward_intr_253

	.align 16
	.global kkm_forward_intr_254
kkm_forward_intr_254:
	int	$254
	retq
	.size kkm_forward_intr_254, .-kkm_forward_intr_254

	.align 16
	.global kkm_forward_intr_255
kkm_forward_intr_255:
	int	$255
	retq
	.size kkm_forward_intr_255, .-kkm_forward_intr_255


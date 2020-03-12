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


#define	FIRST_IRQ	(0)
#define	TOTAL_IRQ	(256)
#define TOTAL_CODE_SIZE (2 * 4096)

/*
 * This code will be physically copied to 
 * kx entry area which starts right next to IDT
 * This code is running on stack from tss->stack0.
 */
	.text
	.align 4096


	.align 16
	.global kkm_intr_entry_0
kkm_intr_entry_0:
	clac
	clc
	push $0
	jmp kkm_intr_entry_asm
	.size kkm_intr_entry_0, .-kkm_intr_entry_0

	.align 16
	.global kkm_intr_entry_1
kkm_intr_entry_1:
	clac
	clc
	push $1
	jmp kkm_intr_entry_asm
	.size kkm_intr_entry_1, .-kkm_intr_entry_1

	.align 16
	.global kkm_intr_entry_2
kkm_intr_entry_2:
	clac
	clc
	push $2
	jmp kkm_intr_entry_asm
	.size kkm_intr_entry_2, .-kkm_intr_entry_2

	.align 16
	.global kkm_intr_entry_3
kkm_intr_entry_3:
	clac
	clc
	push $3
	jmp kkm_intr_entry_asm
	.size kkm_intr_entry_3, .-kkm_intr_entry_3

	.align 16
	.global kkm_intr_entry_4
kkm_intr_entry_4:
	clac
	clc
	push $4
	jmp kkm_intr_entry_asm
	.size kkm_intr_entry_4, .-kkm_intr_entry_4

	.align 16
	.global kkm_intr_entry_5
kkm_intr_entry_5:
	clac
	clc
	push $5
	jmp kkm_intr_entry_asm
	.size kkm_intr_entry_5, .-kkm_intr_entry_5

	.align 16
	.global kkm_intr_entry_6
kkm_intr_entry_6:
	clac
	clc
	push $6
	jmp kkm_intr_entry_asm
	.size kkm_intr_entry_6, .-kkm_intr_entry_6

	.align 16
	.global kkm_intr_entry_7
kkm_intr_entry_7:
	clac
	clc
	push $7
	jmp kkm_intr_entry_asm
	.size kkm_intr_entry_7, .-kkm_intr_entry_7

	.align 16
	.global kkm_intr_entry_8
kkm_intr_entry_8:
	clac
	clc
	push $8
	jmp kkm_intr_entry_asm
	.size kkm_intr_entry_8, .-kkm_intr_entry_8

	.align 16
	.global kkm_intr_entry_9
kkm_intr_entry_9:
	clac
	clc
	push $9
	jmp kkm_intr_entry_asm
	.size kkm_intr_entry_9, .-kkm_intr_entry_9

	.align 16
	.global kkm_intr_entry_10
kkm_intr_entry_10:
	clac
	clc
	push $10
	jmp kkm_intr_entry_asm
	.size kkm_intr_entry_10, .-kkm_intr_entry_10

	.align 16
	.global kkm_intr_entry_11
kkm_intr_entry_11:
	clac
	clc
	push $11
	jmp kkm_intr_entry_asm
	.size kkm_intr_entry_11, .-kkm_intr_entry_11

	.align 16
	.global kkm_intr_entry_12
kkm_intr_entry_12:
	clac
	clc
	push $12
	jmp kkm_intr_entry_asm
	.size kkm_intr_entry_12, .-kkm_intr_entry_12

	.align 16
	.global kkm_intr_entry_13
kkm_intr_entry_13:
	clac
	clc
	push $13
	jmp kkm_intr_entry_asm
	.size kkm_intr_entry_13, .-kkm_intr_entry_13

	.align 16
	.global kkm_intr_entry_14
kkm_intr_entry_14:
	clac
	clc
	push $14
	jmp kkm_intr_entry_asm
	.size kkm_intr_entry_14, .-kkm_intr_entry_14

	.align 16
	.global kkm_intr_entry_15
kkm_intr_entry_15:
	clac
	clc
	push $15
	jmp kkm_intr_entry_asm
	.size kkm_intr_entry_15, .-kkm_intr_entry_15

	.align 16
	.global kkm_intr_entry_16
kkm_intr_entry_16:
	clac
	clc
	push $16
	jmp kkm_intr_entry_asm
	.size kkm_intr_entry_16, .-kkm_intr_entry_16

	.align 16
	.global kkm_intr_entry_17
kkm_intr_entry_17:
	clac
	clc
	push $17
	jmp kkm_intr_entry_asm
	.size kkm_intr_entry_17, .-kkm_intr_entry_17

	.align 16
	.global kkm_intr_entry_18
kkm_intr_entry_18:
	clac
	clc
	push $18
	jmp kkm_intr_entry_asm
	.size kkm_intr_entry_18, .-kkm_intr_entry_18

	.align 16
	.global kkm_intr_entry_19
kkm_intr_entry_19:
	clac
	clc
	push $19
	jmp kkm_intr_entry_asm
	.size kkm_intr_entry_19, .-kkm_intr_entry_19

	.align 16
	.global kkm_intr_entry_20
kkm_intr_entry_20:
	clac
	clc
	push $20
	jmp kkm_intr_entry_asm
	.size kkm_intr_entry_20, .-kkm_intr_entry_20

	.align 16
	.global kkm_intr_entry_21
kkm_intr_entry_21:
	clac
	clc
	push $21
	jmp kkm_intr_entry_asm
	.size kkm_intr_entry_21, .-kkm_intr_entry_21

	.align 16
	.global kkm_intr_entry_22
kkm_intr_entry_22:
	clac
	clc
	push $22
	jmp kkm_intr_entry_asm
	.size kkm_intr_entry_22, .-kkm_intr_entry_22

	.align 16
	.global kkm_intr_entry_23
kkm_intr_entry_23:
	clac
	clc
	push $23
	jmp kkm_intr_entry_asm
	.size kkm_intr_entry_23, .-kkm_intr_entry_23

	.align 16
	.global kkm_intr_entry_24
kkm_intr_entry_24:
	clac
	clc
	push $24
	jmp kkm_intr_entry_asm
	.size kkm_intr_entry_24, .-kkm_intr_entry_24

	.align 16
	.global kkm_intr_entry_25
kkm_intr_entry_25:
	clac
	clc
	push $25
	jmp kkm_intr_entry_asm
	.size kkm_intr_entry_25, .-kkm_intr_entry_25

	.align 16
	.global kkm_intr_entry_26
kkm_intr_entry_26:
	clac
	clc
	push $26
	jmp kkm_intr_entry_asm
	.size kkm_intr_entry_26, .-kkm_intr_entry_26

	.align 16
	.global kkm_intr_entry_27
kkm_intr_entry_27:
	clac
	clc
	push $27
	jmp kkm_intr_entry_asm
	.size kkm_intr_entry_27, .-kkm_intr_entry_27

	.align 16
	.global kkm_intr_entry_28
kkm_intr_entry_28:
	clac
	clc
	push $28
	jmp kkm_intr_entry_asm
	.size kkm_intr_entry_28, .-kkm_intr_entry_28

	.align 16
	.global kkm_intr_entry_29
kkm_intr_entry_29:
	clac
	clc
	push $29
	jmp kkm_intr_entry_asm
	.size kkm_intr_entry_29, .-kkm_intr_entry_29

	.align 16
	.global kkm_intr_entry_30
kkm_intr_entry_30:
	clac
	clc
	push $30
	jmp kkm_intr_entry_asm
	.size kkm_intr_entry_30, .-kkm_intr_entry_30

	.align 16
	.global kkm_intr_entry_31
kkm_intr_entry_31:
	clac
	clc
	push $31
	jmp kkm_intr_entry_asm
	.size kkm_intr_entry_31, .-kkm_intr_entry_31

	.align 16
	.global kkm_intr_entry_32
kkm_intr_entry_32:
	clac
	clc
	push $32
	jmp kkm_intr_entry_asm
	.size kkm_intr_entry_32, .-kkm_intr_entry_32

	.align 16
	.global kkm_intr_entry_33
kkm_intr_entry_33:
	clac
	clc
	push $33
	jmp kkm_intr_entry_asm
	.size kkm_intr_entry_33, .-kkm_intr_entry_33

	.align 16
	.global kkm_intr_entry_34
kkm_intr_entry_34:
	clac
	clc
	push $34
	jmp kkm_intr_entry_asm
	.size kkm_intr_entry_34, .-kkm_intr_entry_34

	.align 16
	.global kkm_intr_entry_35
kkm_intr_entry_35:
	clac
	clc
	push $35
	jmp kkm_intr_entry_asm
	.size kkm_intr_entry_35, .-kkm_intr_entry_35

	.align 16
	.global kkm_intr_entry_36
kkm_intr_entry_36:
	clac
	clc
	push $36
	jmp kkm_intr_entry_asm
	.size kkm_intr_entry_36, .-kkm_intr_entry_36

	.align 16
	.global kkm_intr_entry_37
kkm_intr_entry_37:
	clac
	clc
	push $37
	jmp kkm_intr_entry_asm
	.size kkm_intr_entry_37, .-kkm_intr_entry_37

	.align 16
	.global kkm_intr_entry_38
kkm_intr_entry_38:
	clac
	clc
	push $38
	jmp kkm_intr_entry_asm
	.size kkm_intr_entry_38, .-kkm_intr_entry_38

	.align 16
	.global kkm_intr_entry_39
kkm_intr_entry_39:
	clac
	clc
	push $39
	jmp kkm_intr_entry_asm
	.size kkm_intr_entry_39, .-kkm_intr_entry_39

	.align 16
	.global kkm_intr_entry_40
kkm_intr_entry_40:
	clac
	clc
	push $40
	jmp kkm_intr_entry_asm
	.size kkm_intr_entry_40, .-kkm_intr_entry_40

	.align 16
	.global kkm_intr_entry_41
kkm_intr_entry_41:
	clac
	clc
	push $41
	jmp kkm_intr_entry_asm
	.size kkm_intr_entry_41, .-kkm_intr_entry_41

	.align 16
	.global kkm_intr_entry_42
kkm_intr_entry_42:
	clac
	clc
	push $42
	jmp kkm_intr_entry_asm
	.size kkm_intr_entry_42, .-kkm_intr_entry_42

	.align 16
	.global kkm_intr_entry_43
kkm_intr_entry_43:
	clac
	clc
	push $43
	jmp kkm_intr_entry_asm
	.size kkm_intr_entry_43, .-kkm_intr_entry_43

	.align 16
	.global kkm_intr_entry_44
kkm_intr_entry_44:
	clac
	clc
	push $44
	jmp kkm_intr_entry_asm
	.size kkm_intr_entry_44, .-kkm_intr_entry_44

	.align 16
	.global kkm_intr_entry_45
kkm_intr_entry_45:
	clac
	clc
	push $45
	jmp kkm_intr_entry_asm
	.size kkm_intr_entry_45, .-kkm_intr_entry_45

	.align 16
	.global kkm_intr_entry_46
kkm_intr_entry_46:
	clac
	clc
	push $46
	jmp kkm_intr_entry_asm
	.size kkm_intr_entry_46, .-kkm_intr_entry_46

	.align 16
	.global kkm_intr_entry_47
kkm_intr_entry_47:
	clac
	clc
	push $47
	jmp kkm_intr_entry_asm
	.size kkm_intr_entry_47, .-kkm_intr_entry_47

	.align 16
	.global kkm_intr_entry_48
kkm_intr_entry_48:
	clac
	clc
	push $48
	jmp kkm_intr_entry_asm
	.size kkm_intr_entry_48, .-kkm_intr_entry_48

	.align 16
	.global kkm_intr_entry_49
kkm_intr_entry_49:
	clac
	clc
	push $49
	jmp kkm_intr_entry_asm
	.size kkm_intr_entry_49, .-kkm_intr_entry_49

	.align 16
	.global kkm_intr_entry_50
kkm_intr_entry_50:
	clac
	clc
	push $50
	jmp kkm_intr_entry_asm
	.size kkm_intr_entry_50, .-kkm_intr_entry_50

	.align 16
	.global kkm_intr_entry_51
kkm_intr_entry_51:
	clac
	clc
	push $51
	jmp kkm_intr_entry_asm
	.size kkm_intr_entry_51, .-kkm_intr_entry_51

	.align 16
	.global kkm_intr_entry_52
kkm_intr_entry_52:
	clac
	clc
	push $52
	jmp kkm_intr_entry_asm
	.size kkm_intr_entry_52, .-kkm_intr_entry_52

	.align 16
	.global kkm_intr_entry_53
kkm_intr_entry_53:
	clac
	clc
	push $53
	jmp kkm_intr_entry_asm
	.size kkm_intr_entry_53, .-kkm_intr_entry_53

	.align 16
	.global kkm_intr_entry_54
kkm_intr_entry_54:
	clac
	clc
	push $54
	jmp kkm_intr_entry_asm
	.size kkm_intr_entry_54, .-kkm_intr_entry_54

	.align 16
	.global kkm_intr_entry_55
kkm_intr_entry_55:
	clac
	clc
	push $55
	jmp kkm_intr_entry_asm
	.size kkm_intr_entry_55, .-kkm_intr_entry_55

	.align 16
	.global kkm_intr_entry_56
kkm_intr_entry_56:
	clac
	clc
	push $56
	jmp kkm_intr_entry_asm
	.size kkm_intr_entry_56, .-kkm_intr_entry_56

	.align 16
	.global kkm_intr_entry_57
kkm_intr_entry_57:
	clac
	clc
	push $57
	jmp kkm_intr_entry_asm
	.size kkm_intr_entry_57, .-kkm_intr_entry_57

	.align 16
	.global kkm_intr_entry_58
kkm_intr_entry_58:
	clac
	clc
	push $58
	jmp kkm_intr_entry_asm
	.size kkm_intr_entry_58, .-kkm_intr_entry_58

	.align 16
	.global kkm_intr_entry_59
kkm_intr_entry_59:
	clac
	clc
	push $59
	jmp kkm_intr_entry_asm
	.size kkm_intr_entry_59, .-kkm_intr_entry_59

	.align 16
	.global kkm_intr_entry_60
kkm_intr_entry_60:
	clac
	clc
	push $60
	jmp kkm_intr_entry_asm
	.size kkm_intr_entry_60, .-kkm_intr_entry_60

	.align 16
	.global kkm_intr_entry_61
kkm_intr_entry_61:
	clac
	clc
	push $61
	jmp kkm_intr_entry_asm
	.size kkm_intr_entry_61, .-kkm_intr_entry_61

	.align 16
	.global kkm_intr_entry_62
kkm_intr_entry_62:
	clac
	clc
	push $62
	jmp kkm_intr_entry_asm
	.size kkm_intr_entry_62, .-kkm_intr_entry_62

	.align 16
	.global kkm_intr_entry_63
kkm_intr_entry_63:
	clac
	clc
	push $63
	jmp kkm_intr_entry_asm
	.size kkm_intr_entry_63, .-kkm_intr_entry_63

	.align 16
	.global kkm_intr_entry_64
kkm_intr_entry_64:
	clac
	clc
	push $64
	jmp kkm_intr_entry_asm
	.size kkm_intr_entry_64, .-kkm_intr_entry_64

	.align 16
	.global kkm_intr_entry_65
kkm_intr_entry_65:
	clac
	clc
	push $65
	jmp kkm_intr_entry_asm
	.size kkm_intr_entry_65, .-kkm_intr_entry_65

	.align 16
	.global kkm_intr_entry_66
kkm_intr_entry_66:
	clac
	clc
	push $66
	jmp kkm_intr_entry_asm
	.size kkm_intr_entry_66, .-kkm_intr_entry_66

	.align 16
	.global kkm_intr_entry_67
kkm_intr_entry_67:
	clac
	clc
	push $67
	jmp kkm_intr_entry_asm
	.size kkm_intr_entry_67, .-kkm_intr_entry_67

	.align 16
	.global kkm_intr_entry_68
kkm_intr_entry_68:
	clac
	clc
	push $68
	jmp kkm_intr_entry_asm
	.size kkm_intr_entry_68, .-kkm_intr_entry_68

	.align 16
	.global kkm_intr_entry_69
kkm_intr_entry_69:
	clac
	clc
	push $69
	jmp kkm_intr_entry_asm
	.size kkm_intr_entry_69, .-kkm_intr_entry_69

	.align 16
	.global kkm_intr_entry_70
kkm_intr_entry_70:
	clac
	clc
	push $70
	jmp kkm_intr_entry_asm
	.size kkm_intr_entry_70, .-kkm_intr_entry_70

	.align 16
	.global kkm_intr_entry_71
kkm_intr_entry_71:
	clac
	clc
	push $71
	jmp kkm_intr_entry_asm
	.size kkm_intr_entry_71, .-kkm_intr_entry_71

	.align 16
	.global kkm_intr_entry_72
kkm_intr_entry_72:
	clac
	clc
	push $72
	jmp kkm_intr_entry_asm
	.size kkm_intr_entry_72, .-kkm_intr_entry_72

	.align 16
	.global kkm_intr_entry_73
kkm_intr_entry_73:
	clac
	clc
	push $73
	jmp kkm_intr_entry_asm
	.size kkm_intr_entry_73, .-kkm_intr_entry_73

	.align 16
	.global kkm_intr_entry_74
kkm_intr_entry_74:
	clac
	clc
	push $74
	jmp kkm_intr_entry_asm
	.size kkm_intr_entry_74, .-kkm_intr_entry_74

	.align 16
	.global kkm_intr_entry_75
kkm_intr_entry_75:
	clac
	clc
	push $75
	jmp kkm_intr_entry_asm
	.size kkm_intr_entry_75, .-kkm_intr_entry_75

	.align 16
	.global kkm_intr_entry_76
kkm_intr_entry_76:
	clac
	clc
	push $76
	jmp kkm_intr_entry_asm
	.size kkm_intr_entry_76, .-kkm_intr_entry_76

	.align 16
	.global kkm_intr_entry_77
kkm_intr_entry_77:
	clac
	clc
	push $77
	jmp kkm_intr_entry_asm
	.size kkm_intr_entry_77, .-kkm_intr_entry_77

	.align 16
	.global kkm_intr_entry_78
kkm_intr_entry_78:
	clac
	clc
	push $78
	jmp kkm_intr_entry_asm
	.size kkm_intr_entry_78, .-kkm_intr_entry_78

	.align 16
	.global kkm_intr_entry_79
kkm_intr_entry_79:
	clac
	clc
	push $79
	jmp kkm_intr_entry_asm
	.size kkm_intr_entry_79, .-kkm_intr_entry_79

	.align 16
	.global kkm_intr_entry_80
kkm_intr_entry_80:
	clac
	clc
	push $80
	jmp kkm_intr_entry_asm
	.size kkm_intr_entry_80, .-kkm_intr_entry_80

	.align 16
	.global kkm_intr_entry_81
kkm_intr_entry_81:
	clac
	clc
	push $81
	jmp kkm_intr_entry_asm
	.size kkm_intr_entry_81, .-kkm_intr_entry_81

	.align 16
	.global kkm_intr_entry_82
kkm_intr_entry_82:
	clac
	clc
	push $82
	jmp kkm_intr_entry_asm
	.size kkm_intr_entry_82, .-kkm_intr_entry_82

	.align 16
	.global kkm_intr_entry_83
kkm_intr_entry_83:
	clac
	clc
	push $83
	jmp kkm_intr_entry_asm
	.size kkm_intr_entry_83, .-kkm_intr_entry_83

	.align 16
	.global kkm_intr_entry_84
kkm_intr_entry_84:
	clac
	clc
	push $84
	jmp kkm_intr_entry_asm
	.size kkm_intr_entry_84, .-kkm_intr_entry_84

	.align 16
	.global kkm_intr_entry_85
kkm_intr_entry_85:
	clac
	clc
	push $85
	jmp kkm_intr_entry_asm
	.size kkm_intr_entry_85, .-kkm_intr_entry_85

	.align 16
	.global kkm_intr_entry_86
kkm_intr_entry_86:
	clac
	clc
	push $86
	jmp kkm_intr_entry_asm
	.size kkm_intr_entry_86, .-kkm_intr_entry_86

	.align 16
	.global kkm_intr_entry_87
kkm_intr_entry_87:
	clac
	clc
	push $87
	jmp kkm_intr_entry_asm
	.size kkm_intr_entry_87, .-kkm_intr_entry_87

	.align 16
	.global kkm_intr_entry_88
kkm_intr_entry_88:
	clac
	clc
	push $88
	jmp kkm_intr_entry_asm
	.size kkm_intr_entry_88, .-kkm_intr_entry_88

	.align 16
	.global kkm_intr_entry_89
kkm_intr_entry_89:
	clac
	clc
	push $89
	jmp kkm_intr_entry_asm
	.size kkm_intr_entry_89, .-kkm_intr_entry_89

	.align 16
	.global kkm_intr_entry_90
kkm_intr_entry_90:
	clac
	clc
	push $90
	jmp kkm_intr_entry_asm
	.size kkm_intr_entry_90, .-kkm_intr_entry_90

	.align 16
	.global kkm_intr_entry_91
kkm_intr_entry_91:
	clac
	clc
	push $91
	jmp kkm_intr_entry_asm
	.size kkm_intr_entry_91, .-kkm_intr_entry_91

	.align 16
	.global kkm_intr_entry_92
kkm_intr_entry_92:
	clac
	clc
	push $92
	jmp kkm_intr_entry_asm
	.size kkm_intr_entry_92, .-kkm_intr_entry_92

	.align 16
	.global kkm_intr_entry_93
kkm_intr_entry_93:
	clac
	clc
	push $93
	jmp kkm_intr_entry_asm
	.size kkm_intr_entry_93, .-kkm_intr_entry_93

	.align 16
	.global kkm_intr_entry_94
kkm_intr_entry_94:
	clac
	clc
	push $94
	jmp kkm_intr_entry_asm
	.size kkm_intr_entry_94, .-kkm_intr_entry_94

	.align 16
	.global kkm_intr_entry_95
kkm_intr_entry_95:
	clac
	clc
	push $95
	jmp kkm_intr_entry_asm
	.size kkm_intr_entry_95, .-kkm_intr_entry_95

	.align 16
	.global kkm_intr_entry_96
kkm_intr_entry_96:
	clac
	clc
	push $96
	jmp kkm_intr_entry_asm
	.size kkm_intr_entry_96, .-kkm_intr_entry_96

	.align 16
	.global kkm_intr_entry_97
kkm_intr_entry_97:
	clac
	clc
	push $97
	jmp kkm_intr_entry_asm
	.size kkm_intr_entry_97, .-kkm_intr_entry_97

	.align 16
	.global kkm_intr_entry_98
kkm_intr_entry_98:
	clac
	clc
	push $98
	jmp kkm_intr_entry_asm
	.size kkm_intr_entry_98, .-kkm_intr_entry_98

	.align 16
	.global kkm_intr_entry_99
kkm_intr_entry_99:
	clac
	clc
	push $99
	jmp kkm_intr_entry_asm
	.size kkm_intr_entry_99, .-kkm_intr_entry_99

	.align 16
	.global kkm_intr_entry_100
kkm_intr_entry_100:
	clac
	clc
	push $100
	jmp kkm_intr_entry_asm
	.size kkm_intr_entry_100, .-kkm_intr_entry_100

	.align 16
	.global kkm_intr_entry_101
kkm_intr_entry_101:
	clac
	clc
	push $101
	jmp kkm_intr_entry_asm
	.size kkm_intr_entry_101, .-kkm_intr_entry_101

	.align 16
	.global kkm_intr_entry_102
kkm_intr_entry_102:
	clac
	clc
	push $102
	jmp kkm_intr_entry_asm
	.size kkm_intr_entry_102, .-kkm_intr_entry_102

	.align 16
	.global kkm_intr_entry_103
kkm_intr_entry_103:
	clac
	clc
	push $103
	jmp kkm_intr_entry_asm
	.size kkm_intr_entry_103, .-kkm_intr_entry_103

	.align 16
	.global kkm_intr_entry_104
kkm_intr_entry_104:
	clac
	clc
	push $104
	jmp kkm_intr_entry_asm
	.size kkm_intr_entry_104, .-kkm_intr_entry_104

	.align 16
	.global kkm_intr_entry_105
kkm_intr_entry_105:
	clac
	clc
	push $105
	jmp kkm_intr_entry_asm
	.size kkm_intr_entry_105, .-kkm_intr_entry_105

	.align 16
	.global kkm_intr_entry_106
kkm_intr_entry_106:
	clac
	clc
	push $106
	jmp kkm_intr_entry_asm
	.size kkm_intr_entry_106, .-kkm_intr_entry_106

	.align 16
	.global kkm_intr_entry_107
kkm_intr_entry_107:
	clac
	clc
	push $107
	jmp kkm_intr_entry_asm
	.size kkm_intr_entry_107, .-kkm_intr_entry_107

	.align 16
	.global kkm_intr_entry_108
kkm_intr_entry_108:
	clac
	clc
	push $108
	jmp kkm_intr_entry_asm
	.size kkm_intr_entry_108, .-kkm_intr_entry_108

	.align 16
	.global kkm_intr_entry_109
kkm_intr_entry_109:
	clac
	clc
	push $109
	jmp kkm_intr_entry_asm
	.size kkm_intr_entry_109, .-kkm_intr_entry_109

	.align 16
	.global kkm_intr_entry_110
kkm_intr_entry_110:
	clac
	clc
	push $110
	jmp kkm_intr_entry_asm
	.size kkm_intr_entry_110, .-kkm_intr_entry_110

	.align 16
	.global kkm_intr_entry_111
kkm_intr_entry_111:
	clac
	clc
	push $111
	jmp kkm_intr_entry_asm
	.size kkm_intr_entry_111, .-kkm_intr_entry_111

	.align 16
	.global kkm_intr_entry_112
kkm_intr_entry_112:
	clac
	clc
	push $112
	jmp kkm_intr_entry_asm
	.size kkm_intr_entry_112, .-kkm_intr_entry_112

	.align 16
	.global kkm_intr_entry_113
kkm_intr_entry_113:
	clac
	clc
	push $113
	jmp kkm_intr_entry_asm
	.size kkm_intr_entry_113, .-kkm_intr_entry_113

	.align 16
	.global kkm_intr_entry_114
kkm_intr_entry_114:
	clac
	clc
	push $114
	jmp kkm_intr_entry_asm
	.size kkm_intr_entry_114, .-kkm_intr_entry_114

	.align 16
	.global kkm_intr_entry_115
kkm_intr_entry_115:
	clac
	clc
	push $115
	jmp kkm_intr_entry_asm
	.size kkm_intr_entry_115, .-kkm_intr_entry_115

	.align 16
	.global kkm_intr_entry_116
kkm_intr_entry_116:
	clac
	clc
	push $116
	jmp kkm_intr_entry_asm
	.size kkm_intr_entry_116, .-kkm_intr_entry_116

	.align 16
	.global kkm_intr_entry_117
kkm_intr_entry_117:
	clac
	clc
	push $117
	jmp kkm_intr_entry_asm
	.size kkm_intr_entry_117, .-kkm_intr_entry_117

	.align 16
	.global kkm_intr_entry_118
kkm_intr_entry_118:
	clac
	clc
	push $118
	jmp kkm_intr_entry_asm
	.size kkm_intr_entry_118, .-kkm_intr_entry_118

	.align 16
	.global kkm_intr_entry_119
kkm_intr_entry_119:
	clac
	clc
	push $119
	jmp kkm_intr_entry_asm
	.size kkm_intr_entry_119, .-kkm_intr_entry_119

	.align 16
	.global kkm_intr_entry_120
kkm_intr_entry_120:
	clac
	clc
	push $120
	jmp kkm_intr_entry_asm
	.size kkm_intr_entry_120, .-kkm_intr_entry_120

	.align 16
	.global kkm_intr_entry_121
kkm_intr_entry_121:
	clac
	clc
	push $121
	jmp kkm_intr_entry_asm
	.size kkm_intr_entry_121, .-kkm_intr_entry_121

	.align 16
	.global kkm_intr_entry_122
kkm_intr_entry_122:
	clac
	clc
	push $122
	jmp kkm_intr_entry_asm
	.size kkm_intr_entry_122, .-kkm_intr_entry_122

	.align 16
	.global kkm_intr_entry_123
kkm_intr_entry_123:
	clac
	clc
	push $123
	jmp kkm_intr_entry_asm
	.size kkm_intr_entry_123, .-kkm_intr_entry_123

	.align 16
	.global kkm_intr_entry_124
kkm_intr_entry_124:
	clac
	clc
	push $124
	jmp kkm_intr_entry_asm
	.size kkm_intr_entry_124, .-kkm_intr_entry_124

	.align 16
	.global kkm_intr_entry_125
kkm_intr_entry_125:
	clac
	clc
	push $125
	jmp kkm_intr_entry_asm
	.size kkm_intr_entry_125, .-kkm_intr_entry_125

	.align 16
	.global kkm_intr_entry_126
kkm_intr_entry_126:
	clac
	clc
	push $126
	jmp kkm_intr_entry_asm
	.size kkm_intr_entry_126, .-kkm_intr_entry_126

	.align 16
	.global kkm_intr_entry_127
kkm_intr_entry_127:
	clac
	clc
	push $127
	jmp kkm_intr_entry_asm
	.size kkm_intr_entry_127, .-kkm_intr_entry_127

	.align 16
	.global kkm_intr_entry_128
kkm_intr_entry_128:
	clac
	clc
	push $128
	jmp kkm_intr_entry_asm
	.size kkm_intr_entry_128, .-kkm_intr_entry_128

	.align 16
	.global kkm_intr_entry_129
kkm_intr_entry_129:
	clac
	clc
	push $129
	jmp kkm_intr_entry_asm
	.size kkm_intr_entry_129, .-kkm_intr_entry_129

	.align 16
	.global kkm_intr_entry_130
kkm_intr_entry_130:
	clac
	clc
	push $130
	jmp kkm_intr_entry_asm
	.size kkm_intr_entry_130, .-kkm_intr_entry_130

	.align 16
	.global kkm_intr_entry_131
kkm_intr_entry_131:
	clac
	clc
	push $131
	jmp kkm_intr_entry_asm
	.size kkm_intr_entry_131, .-kkm_intr_entry_131

	.align 16
	.global kkm_intr_entry_132
kkm_intr_entry_132:
	clac
	clc
	push $132
	jmp kkm_intr_entry_asm
	.size kkm_intr_entry_132, .-kkm_intr_entry_132

	.align 16
	.global kkm_intr_entry_133
kkm_intr_entry_133:
	clac
	clc
	push $133
	jmp kkm_intr_entry_asm
	.size kkm_intr_entry_133, .-kkm_intr_entry_133

	.align 16
	.global kkm_intr_entry_134
kkm_intr_entry_134:
	clac
	clc
	push $134
	jmp kkm_intr_entry_asm
	.size kkm_intr_entry_134, .-kkm_intr_entry_134

	.align 16
	.global kkm_intr_entry_135
kkm_intr_entry_135:
	clac
	clc
	push $135
	jmp kkm_intr_entry_asm
	.size kkm_intr_entry_135, .-kkm_intr_entry_135

	.align 16
	.global kkm_intr_entry_136
kkm_intr_entry_136:
	clac
	clc
	push $136
	jmp kkm_intr_entry_asm
	.size kkm_intr_entry_136, .-kkm_intr_entry_136

	.align 16
	.global kkm_intr_entry_137
kkm_intr_entry_137:
	clac
	clc
	push $137
	jmp kkm_intr_entry_asm
	.size kkm_intr_entry_137, .-kkm_intr_entry_137

	.align 16
	.global kkm_intr_entry_138
kkm_intr_entry_138:
	clac
	clc
	push $138
	jmp kkm_intr_entry_asm
	.size kkm_intr_entry_138, .-kkm_intr_entry_138

	.align 16
	.global kkm_intr_entry_139
kkm_intr_entry_139:
	clac
	clc
	push $139
	jmp kkm_intr_entry_asm
	.size kkm_intr_entry_139, .-kkm_intr_entry_139

	.align 16
	.global kkm_intr_entry_140
kkm_intr_entry_140:
	clac
	clc
	push $140
	jmp kkm_intr_entry_asm
	.size kkm_intr_entry_140, .-kkm_intr_entry_140

	.align 16
	.global kkm_intr_entry_141
kkm_intr_entry_141:
	clac
	clc
	push $141
	jmp kkm_intr_entry_asm
	.size kkm_intr_entry_141, .-kkm_intr_entry_141

	.align 16
	.global kkm_intr_entry_142
kkm_intr_entry_142:
	clac
	clc
	push $142
	jmp kkm_intr_entry_asm
	.size kkm_intr_entry_142, .-kkm_intr_entry_142

	.align 16
	.global kkm_intr_entry_143
kkm_intr_entry_143:
	clac
	clc
	push $143
	jmp kkm_intr_entry_asm
	.size kkm_intr_entry_143, .-kkm_intr_entry_143

	.align 16
	.global kkm_intr_entry_144
kkm_intr_entry_144:
	clac
	clc
	push $144
	jmp kkm_intr_entry_asm
	.size kkm_intr_entry_144, .-kkm_intr_entry_144

	.align 16
	.global kkm_intr_entry_145
kkm_intr_entry_145:
	clac
	clc
	push $145
	jmp kkm_intr_entry_asm
	.size kkm_intr_entry_145, .-kkm_intr_entry_145

	.align 16
	.global kkm_intr_entry_146
kkm_intr_entry_146:
	clac
	clc
	push $146
	jmp kkm_intr_entry_asm
	.size kkm_intr_entry_146, .-kkm_intr_entry_146

	.align 16
	.global kkm_intr_entry_147
kkm_intr_entry_147:
	clac
	clc
	push $147
	jmp kkm_intr_entry_asm
	.size kkm_intr_entry_147, .-kkm_intr_entry_147

	.align 16
	.global kkm_intr_entry_148
kkm_intr_entry_148:
	clac
	clc
	push $148
	jmp kkm_intr_entry_asm
	.size kkm_intr_entry_148, .-kkm_intr_entry_148

	.align 16
	.global kkm_intr_entry_149
kkm_intr_entry_149:
	clac
	clc
	push $149
	jmp kkm_intr_entry_asm
	.size kkm_intr_entry_149, .-kkm_intr_entry_149

	.align 16
	.global kkm_intr_entry_150
kkm_intr_entry_150:
	clac
	clc
	push $150
	jmp kkm_intr_entry_asm
	.size kkm_intr_entry_150, .-kkm_intr_entry_150

	.align 16
	.global kkm_intr_entry_151
kkm_intr_entry_151:
	clac
	clc
	push $151
	jmp kkm_intr_entry_asm
	.size kkm_intr_entry_151, .-kkm_intr_entry_151

	.align 16
	.global kkm_intr_entry_152
kkm_intr_entry_152:
	clac
	clc
	push $152
	jmp kkm_intr_entry_asm
	.size kkm_intr_entry_152, .-kkm_intr_entry_152

	.align 16
	.global kkm_intr_entry_153
kkm_intr_entry_153:
	clac
	clc
	push $153
	jmp kkm_intr_entry_asm
	.size kkm_intr_entry_153, .-kkm_intr_entry_153

	.align 16
	.global kkm_intr_entry_154
kkm_intr_entry_154:
	clac
	clc
	push $154
	jmp kkm_intr_entry_asm
	.size kkm_intr_entry_154, .-kkm_intr_entry_154

	.align 16
	.global kkm_intr_entry_155
kkm_intr_entry_155:
	clac
	clc
	push $155
	jmp kkm_intr_entry_asm
	.size kkm_intr_entry_155, .-kkm_intr_entry_155

	.align 16
	.global kkm_intr_entry_156
kkm_intr_entry_156:
	clac
	clc
	push $156
	jmp kkm_intr_entry_asm
	.size kkm_intr_entry_156, .-kkm_intr_entry_156

	.align 16
	.global kkm_intr_entry_157
kkm_intr_entry_157:
	clac
	clc
	push $157
	jmp kkm_intr_entry_asm
	.size kkm_intr_entry_157, .-kkm_intr_entry_157

	.align 16
	.global kkm_intr_entry_158
kkm_intr_entry_158:
	clac
	clc
	push $158
	jmp kkm_intr_entry_asm
	.size kkm_intr_entry_158, .-kkm_intr_entry_158

	.align 16
	.global kkm_intr_entry_159
kkm_intr_entry_159:
	clac
	clc
	push $159
	jmp kkm_intr_entry_asm
	.size kkm_intr_entry_159, .-kkm_intr_entry_159

	.align 16
	.global kkm_intr_entry_160
kkm_intr_entry_160:
	clac
	clc
	push $160
	jmp kkm_intr_entry_asm
	.size kkm_intr_entry_160, .-kkm_intr_entry_160

	.align 16
	.global kkm_intr_entry_161
kkm_intr_entry_161:
	clac
	clc
	push $161
	jmp kkm_intr_entry_asm
	.size kkm_intr_entry_161, .-kkm_intr_entry_161

	.align 16
	.global kkm_intr_entry_162
kkm_intr_entry_162:
	clac
	clc
	push $162
	jmp kkm_intr_entry_asm
	.size kkm_intr_entry_162, .-kkm_intr_entry_162

	.align 16
	.global kkm_intr_entry_163
kkm_intr_entry_163:
	clac
	clc
	push $163
	jmp kkm_intr_entry_asm
	.size kkm_intr_entry_163, .-kkm_intr_entry_163

	.align 16
	.global kkm_intr_entry_164
kkm_intr_entry_164:
	clac
	clc
	push $164
	jmp kkm_intr_entry_asm
	.size kkm_intr_entry_164, .-kkm_intr_entry_164

	.align 16
	.global kkm_intr_entry_165
kkm_intr_entry_165:
	clac
	clc
	push $165
	jmp kkm_intr_entry_asm
	.size kkm_intr_entry_165, .-kkm_intr_entry_165

	.align 16
	.global kkm_intr_entry_166
kkm_intr_entry_166:
	clac
	clc
	push $166
	jmp kkm_intr_entry_asm
	.size kkm_intr_entry_166, .-kkm_intr_entry_166

	.align 16
	.global kkm_intr_entry_167
kkm_intr_entry_167:
	clac
	clc
	push $167
	jmp kkm_intr_entry_asm
	.size kkm_intr_entry_167, .-kkm_intr_entry_167

	.align 16
	.global kkm_intr_entry_168
kkm_intr_entry_168:
	clac
	clc
	push $168
	jmp kkm_intr_entry_asm
	.size kkm_intr_entry_168, .-kkm_intr_entry_168

	.align 16
	.global kkm_intr_entry_169
kkm_intr_entry_169:
	clac
	clc
	push $169
	jmp kkm_intr_entry_asm
	.size kkm_intr_entry_169, .-kkm_intr_entry_169

	.align 16
	.global kkm_intr_entry_170
kkm_intr_entry_170:
	clac
	clc
	push $170
	jmp kkm_intr_entry_asm
	.size kkm_intr_entry_170, .-kkm_intr_entry_170

	.align 16
	.global kkm_intr_entry_171
kkm_intr_entry_171:
	clac
	clc
	push $171
	jmp kkm_intr_entry_asm
	.size kkm_intr_entry_171, .-kkm_intr_entry_171

	.align 16
	.global kkm_intr_entry_172
kkm_intr_entry_172:
	clac
	clc
	push $172
	jmp kkm_intr_entry_asm
	.size kkm_intr_entry_172, .-kkm_intr_entry_172

	.align 16
	.global kkm_intr_entry_173
kkm_intr_entry_173:
	clac
	clc
	push $173
	jmp kkm_intr_entry_asm
	.size kkm_intr_entry_173, .-kkm_intr_entry_173

	.align 16
	.global kkm_intr_entry_174
kkm_intr_entry_174:
	clac
	clc
	push $174
	jmp kkm_intr_entry_asm
	.size kkm_intr_entry_174, .-kkm_intr_entry_174

	.align 16
	.global kkm_intr_entry_175
kkm_intr_entry_175:
	clac
	clc
	push $175
	jmp kkm_intr_entry_asm
	.size kkm_intr_entry_175, .-kkm_intr_entry_175

	.align 16
	.global kkm_intr_entry_176
kkm_intr_entry_176:
	clac
	clc
	push $176
	jmp kkm_intr_entry_asm
	.size kkm_intr_entry_176, .-kkm_intr_entry_176

	.align 16
	.global kkm_intr_entry_177
kkm_intr_entry_177:
	clac
	clc
	push $177
	jmp kkm_intr_entry_asm
	.size kkm_intr_entry_177, .-kkm_intr_entry_177

	.align 16
	.global kkm_intr_entry_178
kkm_intr_entry_178:
	clac
	clc
	push $178
	jmp kkm_intr_entry_asm
	.size kkm_intr_entry_178, .-kkm_intr_entry_178

	.align 16
	.global kkm_intr_entry_179
kkm_intr_entry_179:
	clac
	clc
	push $179
	jmp kkm_intr_entry_asm
	.size kkm_intr_entry_179, .-kkm_intr_entry_179

	.align 16
	.global kkm_intr_entry_180
kkm_intr_entry_180:
	clac
	clc
	push $180
	jmp kkm_intr_entry_asm
	.size kkm_intr_entry_180, .-kkm_intr_entry_180

	.align 16
	.global kkm_intr_entry_181
kkm_intr_entry_181:
	clac
	clc
	push $181
	jmp kkm_intr_entry_asm
	.size kkm_intr_entry_181, .-kkm_intr_entry_181

	.align 16
	.global kkm_intr_entry_182
kkm_intr_entry_182:
	clac
	clc
	push $182
	jmp kkm_intr_entry_asm
	.size kkm_intr_entry_182, .-kkm_intr_entry_182

	.align 16
	.global kkm_intr_entry_183
kkm_intr_entry_183:
	clac
	clc
	push $183
	jmp kkm_intr_entry_asm
	.size kkm_intr_entry_183, .-kkm_intr_entry_183

	.align 16
	.global kkm_intr_entry_184
kkm_intr_entry_184:
	clac
	clc
	push $184
	jmp kkm_intr_entry_asm
	.size kkm_intr_entry_184, .-kkm_intr_entry_184

	.align 16
	.global kkm_intr_entry_185
kkm_intr_entry_185:
	clac
	clc
	push $185
	jmp kkm_intr_entry_asm
	.size kkm_intr_entry_185, .-kkm_intr_entry_185

	.align 16
	.global kkm_intr_entry_186
kkm_intr_entry_186:
	clac
	clc
	push $186
	jmp kkm_intr_entry_asm
	.size kkm_intr_entry_186, .-kkm_intr_entry_186

	.align 16
	.global kkm_intr_entry_187
kkm_intr_entry_187:
	clac
	clc
	push $187
	jmp kkm_intr_entry_asm
	.size kkm_intr_entry_187, .-kkm_intr_entry_187

	.align 16
	.global kkm_intr_entry_188
kkm_intr_entry_188:
	clac
	clc
	push $188
	jmp kkm_intr_entry_asm
	.size kkm_intr_entry_188, .-kkm_intr_entry_188

	.align 16
	.global kkm_intr_entry_189
kkm_intr_entry_189:
	clac
	clc
	push $189
	jmp kkm_intr_entry_asm
	.size kkm_intr_entry_189, .-kkm_intr_entry_189

	.align 16
	.global kkm_intr_entry_190
kkm_intr_entry_190:
	clac
	clc
	push $190
	jmp kkm_intr_entry_asm
	.size kkm_intr_entry_190, .-kkm_intr_entry_190

	.align 16
	.global kkm_intr_entry_191
kkm_intr_entry_191:
	clac
	clc
	push $191
	jmp kkm_intr_entry_asm
	.size kkm_intr_entry_191, .-kkm_intr_entry_191

	.align 16
	.global kkm_intr_entry_192
kkm_intr_entry_192:
	clac
	clc
	push $192
	jmp kkm_intr_entry_asm
	.size kkm_intr_entry_192, .-kkm_intr_entry_192

	.align 16
	.global kkm_intr_entry_193
kkm_intr_entry_193:
	clac
	clc
	push $193
	jmp kkm_intr_entry_asm
	.size kkm_intr_entry_193, .-kkm_intr_entry_193

	.align 16
	.global kkm_intr_entry_194
kkm_intr_entry_194:
	clac
	clc
	push $194
	jmp kkm_intr_entry_asm
	.size kkm_intr_entry_194, .-kkm_intr_entry_194

	.align 16
	.global kkm_intr_entry_195
kkm_intr_entry_195:
	clac
	clc
	push $195
	jmp kkm_intr_entry_asm
	.size kkm_intr_entry_195, .-kkm_intr_entry_195

	.align 16
	.global kkm_intr_entry_196
kkm_intr_entry_196:
	clac
	clc
	push $196
	jmp kkm_intr_entry_asm
	.size kkm_intr_entry_196, .-kkm_intr_entry_196

	.align 16
	.global kkm_intr_entry_197
kkm_intr_entry_197:
	clac
	clc
	push $197
	jmp kkm_intr_entry_asm
	.size kkm_intr_entry_197, .-kkm_intr_entry_197

	.align 16
	.global kkm_intr_entry_198
kkm_intr_entry_198:
	clac
	clc
	push $198
	jmp kkm_intr_entry_asm
	.size kkm_intr_entry_198, .-kkm_intr_entry_198

	.align 16
	.global kkm_intr_entry_199
kkm_intr_entry_199:
	clac
	clc
	push $199
	jmp kkm_intr_entry_asm
	.size kkm_intr_entry_199, .-kkm_intr_entry_199

	.align 16
	.global kkm_intr_entry_200
kkm_intr_entry_200:
	clac
	clc
	push $200
	jmp kkm_intr_entry_asm
	.size kkm_intr_entry_200, .-kkm_intr_entry_200

	.align 16
	.global kkm_intr_entry_201
kkm_intr_entry_201:
	clac
	clc
	push $201
	jmp kkm_intr_entry_asm
	.size kkm_intr_entry_201, .-kkm_intr_entry_201

	.align 16
	.global kkm_intr_entry_202
kkm_intr_entry_202:
	clac
	clc
	push $202
	jmp kkm_intr_entry_asm
	.size kkm_intr_entry_202, .-kkm_intr_entry_202

	.align 16
	.global kkm_intr_entry_203
kkm_intr_entry_203:
	clac
	clc
	push $203
	jmp kkm_intr_entry_asm
	.size kkm_intr_entry_203, .-kkm_intr_entry_203

	.align 16
	.global kkm_intr_entry_204
kkm_intr_entry_204:
	clac
	clc
	push $204
	jmp kkm_intr_entry_asm
	.size kkm_intr_entry_204, .-kkm_intr_entry_204

	.align 16
	.global kkm_intr_entry_205
kkm_intr_entry_205:
	clac
	clc
	push $205
	jmp kkm_intr_entry_asm
	.size kkm_intr_entry_205, .-kkm_intr_entry_205

	.align 16
	.global kkm_intr_entry_206
kkm_intr_entry_206:
	clac
	clc
	push $206
	jmp kkm_intr_entry_asm
	.size kkm_intr_entry_206, .-kkm_intr_entry_206

	.align 16
	.global kkm_intr_entry_207
kkm_intr_entry_207:
	clac
	clc
	push $207
	jmp kkm_intr_entry_asm
	.size kkm_intr_entry_207, .-kkm_intr_entry_207

	.align 16
	.global kkm_intr_entry_208
kkm_intr_entry_208:
	clac
	clc
	push $208
	jmp kkm_intr_entry_asm
	.size kkm_intr_entry_208, .-kkm_intr_entry_208

	.align 16
	.global kkm_intr_entry_209
kkm_intr_entry_209:
	clac
	clc
	push $209
	jmp kkm_intr_entry_asm
	.size kkm_intr_entry_209, .-kkm_intr_entry_209

	.align 16
	.global kkm_intr_entry_210
kkm_intr_entry_210:
	clac
	clc
	push $210
	jmp kkm_intr_entry_asm
	.size kkm_intr_entry_210, .-kkm_intr_entry_210

	.align 16
	.global kkm_intr_entry_211
kkm_intr_entry_211:
	clac
	clc
	push $211
	jmp kkm_intr_entry_asm
	.size kkm_intr_entry_211, .-kkm_intr_entry_211

	.align 16
	.global kkm_intr_entry_212
kkm_intr_entry_212:
	clac
	clc
	push $212
	jmp kkm_intr_entry_asm
	.size kkm_intr_entry_212, .-kkm_intr_entry_212

	.align 16
	.global kkm_intr_entry_213
kkm_intr_entry_213:
	clac
	clc
	push $213
	jmp kkm_intr_entry_asm
	.size kkm_intr_entry_213, .-kkm_intr_entry_213

	.align 16
	.global kkm_intr_entry_214
kkm_intr_entry_214:
	clac
	clc
	push $214
	jmp kkm_intr_entry_asm
	.size kkm_intr_entry_214, .-kkm_intr_entry_214

	.align 16
	.global kkm_intr_entry_215
kkm_intr_entry_215:
	clac
	clc
	push $215
	jmp kkm_intr_entry_asm
	.size kkm_intr_entry_215, .-kkm_intr_entry_215

	.align 16
	.global kkm_intr_entry_216
kkm_intr_entry_216:
	clac
	clc
	push $216
	jmp kkm_intr_entry_asm
	.size kkm_intr_entry_216, .-kkm_intr_entry_216

	.align 16
	.global kkm_intr_entry_217
kkm_intr_entry_217:
	clac
	clc
	push $217
	jmp kkm_intr_entry_asm
	.size kkm_intr_entry_217, .-kkm_intr_entry_217

	.align 16
	.global kkm_intr_entry_218
kkm_intr_entry_218:
	clac
	clc
	push $218
	jmp kkm_intr_entry_asm
	.size kkm_intr_entry_218, .-kkm_intr_entry_218

	.align 16
	.global kkm_intr_entry_219
kkm_intr_entry_219:
	clac
	clc
	push $219
	jmp kkm_intr_entry_asm
	.size kkm_intr_entry_219, .-kkm_intr_entry_219

	.align 16
	.global kkm_intr_entry_220
kkm_intr_entry_220:
	clac
	clc
	push $220
	jmp kkm_intr_entry_asm
	.size kkm_intr_entry_220, .-kkm_intr_entry_220

	.align 16
	.global kkm_intr_entry_221
kkm_intr_entry_221:
	clac
	clc
	push $221
	jmp kkm_intr_entry_asm
	.size kkm_intr_entry_221, .-kkm_intr_entry_221

	.align 16
	.global kkm_intr_entry_222
kkm_intr_entry_222:
	clac
	clc
	push $222
	jmp kkm_intr_entry_asm
	.size kkm_intr_entry_222, .-kkm_intr_entry_222

	.align 16
	.global kkm_intr_entry_223
kkm_intr_entry_223:
	clac
	clc
	push $223
	jmp kkm_intr_entry_asm
	.size kkm_intr_entry_223, .-kkm_intr_entry_223

	.align 16
	.global kkm_intr_entry_224
kkm_intr_entry_224:
	clac
	clc
	push $224
	jmp kkm_intr_entry_asm
	.size kkm_intr_entry_224, .-kkm_intr_entry_224

	.align 16
	.global kkm_intr_entry_225
kkm_intr_entry_225:
	clac
	clc
	push $225
	jmp kkm_intr_entry_asm
	.size kkm_intr_entry_225, .-kkm_intr_entry_225

	.align 16
	.global kkm_intr_entry_226
kkm_intr_entry_226:
	clac
	clc
	push $226
	jmp kkm_intr_entry_asm
	.size kkm_intr_entry_226, .-kkm_intr_entry_226

	.align 16
	.global kkm_intr_entry_227
kkm_intr_entry_227:
	clac
	clc
	push $227
	jmp kkm_intr_entry_asm
	.size kkm_intr_entry_227, .-kkm_intr_entry_227

	.align 16
	.global kkm_intr_entry_228
kkm_intr_entry_228:
	clac
	clc
	push $228
	jmp kkm_intr_entry_asm
	.size kkm_intr_entry_228, .-kkm_intr_entry_228

	.align 16
	.global kkm_intr_entry_229
kkm_intr_entry_229:
	clac
	clc
	push $229
	jmp kkm_intr_entry_asm
	.size kkm_intr_entry_229, .-kkm_intr_entry_229

	.align 16
	.global kkm_intr_entry_230
kkm_intr_entry_230:
	clac
	clc
	push $230
	jmp kkm_intr_entry_asm
	.size kkm_intr_entry_230, .-kkm_intr_entry_230

	.align 16
	.global kkm_intr_entry_231
kkm_intr_entry_231:
	clac
	clc
	push $231
	jmp kkm_intr_entry_asm
	.size kkm_intr_entry_231, .-kkm_intr_entry_231

	.align 16
	.global kkm_intr_entry_232
kkm_intr_entry_232:
	clac
	clc
	push $232
	jmp kkm_intr_entry_asm
	.size kkm_intr_entry_232, .-kkm_intr_entry_232

	.align 16
	.global kkm_intr_entry_233
kkm_intr_entry_233:
	clac
	clc
	push $233
	jmp kkm_intr_entry_asm
	.size kkm_intr_entry_233, .-kkm_intr_entry_233

	.align 16
	.global kkm_intr_entry_234
kkm_intr_entry_234:
	clac
	clc
	push $234
	jmp kkm_intr_entry_asm
	.size kkm_intr_entry_234, .-kkm_intr_entry_234

	.align 16
	.global kkm_intr_entry_235
kkm_intr_entry_235:
	clac
	clc
	push $235
	jmp kkm_intr_entry_asm
	.size kkm_intr_entry_235, .-kkm_intr_entry_235

	.align 16
	.global kkm_intr_entry_236
kkm_intr_entry_236:
	clac
	clc
	push $236
	jmp kkm_intr_entry_asm
	.size kkm_intr_entry_236, .-kkm_intr_entry_236

	.align 16
	.global kkm_intr_entry_237
kkm_intr_entry_237:
	clac
	clc
	push $237
	jmp kkm_intr_entry_asm
	.size kkm_intr_entry_237, .-kkm_intr_entry_237

	.align 16
	.global kkm_intr_entry_238
kkm_intr_entry_238:
	clac
	clc
	push $238
	jmp kkm_intr_entry_asm
	.size kkm_intr_entry_238, .-kkm_intr_entry_238

	.align 16
	.global kkm_intr_entry_239
kkm_intr_entry_239:
	clac
	clc
	push $239
	jmp kkm_intr_entry_asm
	.size kkm_intr_entry_239, .-kkm_intr_entry_239

	.align 16
	.global kkm_intr_entry_240
kkm_intr_entry_240:
	clac
	clc
	push $240
	jmp kkm_intr_entry_asm
	.size kkm_intr_entry_240, .-kkm_intr_entry_240

	.align 16
	.global kkm_intr_entry_241
kkm_intr_entry_241:
	clac
	clc
	push $241
	jmp kkm_intr_entry_asm
	.size kkm_intr_entry_241, .-kkm_intr_entry_241

	.align 16
	.global kkm_intr_entry_242
kkm_intr_entry_242:
	clac
	clc
	push $242
	jmp kkm_intr_entry_asm
	.size kkm_intr_entry_242, .-kkm_intr_entry_242

	.align 16
	.global kkm_intr_entry_243
kkm_intr_entry_243:
	clac
	clc
	push $243
	jmp kkm_intr_entry_asm
	.size kkm_intr_entry_243, .-kkm_intr_entry_243

	.align 16
	.global kkm_intr_entry_244
kkm_intr_entry_244:
	clac
	clc
	push $244
	jmp kkm_intr_entry_asm
	.size kkm_intr_entry_244, .-kkm_intr_entry_244

	.align 16
	.global kkm_intr_entry_245
kkm_intr_entry_245:
	clac
	clc
	push $245
	jmp kkm_intr_entry_asm
	.size kkm_intr_entry_245, .-kkm_intr_entry_245

	.align 16
	.global kkm_intr_entry_246
kkm_intr_entry_246:
	clac
	clc
	push $246
	jmp kkm_intr_entry_asm
	.size kkm_intr_entry_246, .-kkm_intr_entry_246

	.align 16
	.global kkm_intr_entry_247
kkm_intr_entry_247:
	clac
	clc
	push $247
	jmp kkm_intr_entry_asm
	.size kkm_intr_entry_247, .-kkm_intr_entry_247

	.align 16
	.global kkm_intr_entry_248
kkm_intr_entry_248:
	clac
	clc
	push $248
	jmp kkm_intr_entry_asm
	.size kkm_intr_entry_248, .-kkm_intr_entry_248

	.align 16
	.global kkm_intr_entry_249
kkm_intr_entry_249:
	clac
	clc
	push $249
	jmp kkm_intr_entry_asm
	.size kkm_intr_entry_249, .-kkm_intr_entry_249

	.align 16
	.global kkm_intr_entry_250
kkm_intr_entry_250:
	clac
	clc
	push $250
	jmp kkm_intr_entry_asm
	.size kkm_intr_entry_250, .-kkm_intr_entry_250

	.align 16
	.global kkm_intr_entry_251
kkm_intr_entry_251:
	clac
	clc
	push $251
	jmp kkm_intr_entry_asm
	.size kkm_intr_entry_251, .-kkm_intr_entry_251

	.align 16
	.global kkm_intr_entry_252
kkm_intr_entry_252:
	clac
	clc
	push $252
	jmp kkm_intr_entry_asm
	.size kkm_intr_entry_252, .-kkm_intr_entry_252

	.align 16
	.global kkm_intr_entry_253
kkm_intr_entry_253:
	clac
	clc
	push $253
	jmp kkm_intr_entry_asm
	.size kkm_intr_entry_253, .-kkm_intr_entry_253

	.align 16
	.global kkm_intr_entry_254
kkm_intr_entry_254:
	clac
	clc
	push $254
	jmp kkm_intr_entry_asm
	.size kkm_intr_entry_254, .-kkm_intr_entry_254

	.align 16
	.global kkm_intr_entry_255
kkm_intr_entry_255:
	clac
	clc
	push $255
	jmp kkm_intr_entry_asm
	.size kkm_intr_entry_255, .-kkm_intr_entry_255

	.global kkm_intr_entry_asm
kkm_intr_entry_asm:
	/*
	 * set flags, entered from unknown code
	 */
	clac
	clc

	/*
	 * change to kernel thread local storage
	 */
	swapgs

	/*
	 * create scratch registers
	 */
	pushq	%rdi
	pushq	%rsi
	/*
	 * trap stack looks like this now
	 *      ----------- bottom of stack
	 *      |   SS    |
	 *      ----------
	 *      |   RSP   |
	 *      -----------
	 *      |  RFLAGS |
	 *      -----------
	 *      |   CS    |
	 *      -----------
	 *      |   RIP   |
	 *      -----------
	 *      |  ERROR  |
	 *      -----------
	 *      | INTR NO |
	 *      -----------
	 *      |   RDI   |
	 *      -----------
	 *      |   RSI   |
	 *      ----------- top of stack
	 */

	/*
	 * save current stack top to rsi
	 * we need this to save trap information
	 */
	movq	%rsp, %rsi

	/*
	 * switch to guest kernel address space
	 * kernel and payload PML4 are allocated next to each
	 * other(even page is kernel, odd page is guest) in physical memory.
	 * no need to flush TLB
	 * guest kernel address space is superset of guest payload address space
	 */
	movq	%cr3, %rdi
	andq	$0xffffffffffffefff, %rdi
	movq	%rdi, %cr3

	/*
	 * %rsp is currently in guest area
	 * masking off bottom 12 bits points to ga in kx area
	 */
	movq	%rsp, %rdi
	andq	$0xffffffffffffe000, %rdi

	/*
	 * save registers to kkm_guest_area
	 */
	movq	%r15, OFF_R15(%rdi)
	movq	%r14, OFF_R14(%rdi)
	movq	%r13, OFF_R13(%rdi)
	movq	%r12, OFF_R12(%rdi)

	movq	%r11, OFF_R11(%rdi)
	movq	%r10, OFF_R10(%rdi)
	movq	%r9, OFF_R9(%rdi)
	movq	%r8, OFF_R8(%rdi)

	movq	%rbp, OFF_RBP(%rdi)
	/*
	 * guest %rsi, %rsp, %rdi are on interrupt stack
	 * save them using %rsi which has interrupt stack
	 * use %rbp as scratch register
	 */
	movq	OFF_TS_RSP(%rsi), %rbp
	movq	%rbp, OFF_RSP(%rdi)

	movq	OFF_TS_RDI(%rsi), %rbp
	movq	%rbp, OFF_RDI(%rdi)

	movq	OFF_TS_RSI(%rsi), %rbp
	movq	%rbp, OFF_RSI(%rdi)

	movq	%rdx, OFF_RDX(%rdi)
	movq	%rcx, OFF_RCX(%rdi)
	movq	%rbx, OFF_RBX(%rdi)
	movq	%rax, OFF_RAX(%rdi)

	/*
	 * save control registers
	 */
	movq    %cr0, %rax
	movq    %rax, OFF_CR0(%rdi)
	movq    %cr2, %rax
	movq    %rax, OFF_CR2(%rdi)
	movq    %cr3, %rax
	movq    %rax, OFF_CR3(%rdi)
	movq    %cr4, %rax
	movq    %rax, OFF_CR4(%rdi)
	movq    %cr8, %rax
	movq    %rax, OFF_CR8(%rdi)

	/*
	 * save trap information from trap stack to kkm_guest_area
	 */
	movq	OFF_TS_ERROR(%rsi), %rbp
	movq	%rbp, OFF_TI_ERROR(%rdi)

	movq	OFF_TS_RIP(%rsi), %rbp
	movq	%rbp, OFF_TI_RIP(%rdi)

	movq	OFF_TS_CS(%rsi), %rbp
	movq	%rbp, OFF_TI_CS(%rdi)

	movq	OFF_TS_RFLAGS(%rsi), %rbp
	movq	%rbp, OFF_TI_RFLAGS(%rdi)

	movq	OFF_TS_RSP(%rsi), %rbp
	movq	%rbp, OFF_TI_RSP(%rdi)

	movq	OFF_TS_SS(%rsi), %rbp
	movq	%rbp, OFF_TI_SS(%rdi)

	/*
	 * save interrupt no
	 */
	movq	OFF_TS_INTR_NO(%rsi), %rbp
	movq	%rbp, OFF_TI_INTR_NO(%rdi)

	call	kkm_switch_to_host_kernel

	/* NOTREACHED */
	iretq
	.size kkm_intr_entry_asm, .-kkm_intr_entry_asm

	.global kkm_intr_fill
kkm_intr_fill:
	.fill  TOTAL_CODE_SIZE - kkm_intr_fill - kkm_intr_entry_0, 1, 0x90
	.size kkm_intr_fill, .-kkm_intr_fill

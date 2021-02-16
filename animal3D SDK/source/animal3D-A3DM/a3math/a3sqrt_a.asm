COMMENT @
/*
	Copyright 2011-2021 Daniel S. Buckstein

	Licensed under the Apache License, Version 2.0 (the "License");
	you may not use this file except in compliance with the License.
	You may obtain a copy of the License at

		http://www.apache.org/licenses/LICENSE-2.0

	Unless required by applicable law or agreed to in writing, software
	distributed under the License is distributed on an "AS IS" BASIS,
	WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
	See the License for the specific language governing permissions and
	limitations under the License.
*/

/*
	animal3D SDK: Minimal 3D Animation Framework
	animal3D Math (A3DM) SDK
	By Daniel S. Buckstein

	a3sqrt_a.asm
	Assembly definitions of ultra fast sqrt function for all configs.

	Help: 
	https://stackoverflow.com/questions/4548763/compiling-assembly-in-visual-studio
	http://lallouslab.net/2016/01/11/introduction-to-writing-x64-assembly-in-visual-studio/
	http://rayseyfarth.com/asm/pdf/ch11-floating-point.pdf

	Info: 
	<op>ss	scalar single: one float in the register (quarter of xmm [e.g. xmm0])
	<op>ps	packed single: four floats in the register (all of xmm)
	<op>sd	scalar double: one double in the register (half of xmm)
	<op>pd	packed double: two doubles in the register (all of xmm)
	
	THIS CODE IS ASSEMBLED USING THE ASSEMBLER...
		WINDOWS: masm
		LINUX: nasm, clang, as (new); (could use gas)
		MAC: clang; gcc is deprecated

	Summary: 
		-add ASM file to project
		-right click project > build dependencies > build customizations
		-enable MASM
		-right click ASM file > properties > item type > microsoft macro assembler
*/
@

;//////////////////////////////////////////////////////////////////////////////
; 64-bit defined if rax register exists
IFDEF RAX

.data
one			EQU		1.0		; effectively the same as define macro
fs1			dd		one		; constant single (float) set to 1
fd1			dq		one		; constant double set to 1

.code


a3sqrtf PROC
	sqrtss		xmm0,	xmm0	; do scalar single sqrt on xmm0 (already contains arg from function setup)
	ret							; exit function
a3sqrtf ENDP

a3sqrtfInverse PROC
; rsqrt is handy but does not give correct result; only ss available
;	rsqrtss		xmm0,	xmm0	; do scalar single recip sqrt on xmm0
; result is not correct with recip either, closer when recip is first; only ss available
;	rcpss		xmm0,	xmm0	; do reciprocal on xmm0
; DO RECIP MANUALLY
	movss		xmm1,	[fs1]	; copy constant to register xmm1
	divss		xmm1,	xmm0	; do reciprocal of xmm0, store in xmm1
	sqrtss		xmm0,	xmm1	; do scalar single sqrt on xmm1, store in xmm0
	ret							; exit
a3sqrtfInverse ENDP

a3sqrtd PROC
	sqrtsd		xmm0,	xmm0	; do scalar double sqrt on xmm0
	ret							; exit function
a3sqrtd ENDP

a3sqrtdInverse PROC
	movsd		xmm1,	[fd1]	; copy constant to register xmm1
	divsd		xmm1,	xmm0	; do reciprocal of xmm0, store in xmm1
	sqrtsd		xmm0,	xmm1	; do scalar double sqrt on xmm1, store in xmm0
	ret							; exit
a3sqrtdInverse ENDP


;//////////////////////////////////////////////////////////////////////////////
; 32-bit
ELSE	; !RAX

.model flat, c
.code


a3sqrtf PROC
	fld		dword ptr	[esp+4]	; load input to float stack (st0) as float (int size); arg is 4 bytes past return address (which is int size)
	fsqrt						; do sqrt on top of float stack (st0)
	ret							; exit function
a3sqrtf ENDP

a3sqrtfInverse PROC
	fld1						; load 1 to float stack (st0)
	fld		dword ptr	[esp+4]	; load input to float stack (st0), pushes previously loaded '1' to st1
	fdivp						; divide st1 by st0, store in st1 and pop
	fsqrt						; do sqrt on top of stack
	ret							; exit
a3sqrtfInverse ENDP

a3sqrtd PROC
	fld		qword ptr	[esp+4]	; load input to float stack (st0) as double (large int size)
	fsqrt						; do sqrt on top of float stack (st0)
	ret							; exit function
a3sqrtd ENDP

a3sqrtdInverse PROC
	fld1						; load 1 to float stack (st0)
	fld		qword ptr	[esp+4]	; load input to float stack (st0), pushes previously loaded '1' to st1
	fdivp						; divide st1 by st0, store in st1 and pop
	fsqrt						; do sqrt on top of stack
	ret							; exit
a3sqrtdInverse ENDP


;//////////////////////////////////////////////////////////////////////////////
; done
ENDIF

; required at end (no newline)
END
/* **********************************************************
 * Copyright (c) 2011-2014 Google, Inc.  All rights reserved.
 * Copyright (c) 2008-2009 VMware, Inc.  All rights reserved.
 * ********************************************************** */

/*
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * * Redistributions of source code must retain the above copyright notice,
 *   this list of conditions and the following disclaimer.
 *
 * * Redistributions in binary form must reproduce the above copyright notice,
 *   this list of conditions and the following disclaimer in the documentation
 *   and/or other materials provided with the distribution.
 *
 * * Neither the name of VMware, Inc. nor the names of its contributors may be
 *   used to endorse or promote products derived from this software without
 *   specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL VMWARE, INC. OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH
 * DAMAGE.
 */

#ifndef _ASM_DEFINES_ASM_
#define _ASM_DEFINES_ASM_ 1

/* Preprocessor macro definitions shared among all .asm files.
 * Since cpp macros can't generate newlines we have a later
 * script replace @N@ for us.
 */



#if defined(X86_64) && !defined(X64)
# define X64
#endif

/****************************************************/
#if defined(ASSEMBLE_WITH_GAS)
# define START_FILE .text
# define END_FILE /* nothing */
# define DECLARE_FUNC(symbol) \
.align 0 @N@\
.global symbol @N@\
.hidden symbol @N@\
.type symbol, %function
# define DECLARE_EXPORTED_FUNC(symbol) \
.align 0 @N@\
.global symbol @N@\
.type symbol, %function
# define END_FUNC(symbol) /* nothing */
# define DECLARE_GLOBAL(symbol) \
.global symbol @N@\
.hidden symbol
# define GLOBAL_LABEL(label) label
# define ADDRTAKEN_LABEL(label) label
# define GLOBAL_REF(label) label
# define BYTE byte ptr
# define WORD word ptr
# define DWORD dword ptr
# define QWORD qword ptr
# define MMWORD qword ptr
# define XMMWORD oword ptr
# define YMMWORD ymmword ptr
# ifdef X64
/* w/o the rip, gas won't use rip-rel and adds relocs that ld trips over */
#  define SYMREF(sym) [rip + sym]
# else
#  define SYMREF(sym) [sym]
# endif
# define HEX(n) 0x##n
# define SEGMEM(seg,mem) [seg:mem]
# define DECL_EXTERN(symbol) /* nothing */
/* include newline so we can put multiple on one line */
# define RAW(n) .byte HEX(n) @N@
# define DECLARE_FUNC_SEH(symbol) DECLARE_FUNC(symbol)
# define PUSH_SEH(reg) push reg
# define PUSH_NONCALLEE_SEH(reg) push reg
# define END_PROLOG /* nothing */
/* PR 212290: avoid text relocations.
 * @GOT returns the address and is for extern vars; @GOTOFF gets the value.
 * Note that using ## to paste =>
 *   "error: pasting "initstack_mutex" and "@" does not give a valid preprocessing token"
 * but whitespace separation seems fine.
 */
# define ADDR_VIA_GOT(base, sym) [sym @GOT + base]
# define VAR_VIA_GOT(base, sym) [sym @GOTOFF + base]
/****************************************************/
#elif defined(ASSEMBLE_WITH_MASM)
# ifdef X64
#  define START_FILE .CODE
# else
#  define START_FILE \
.686 /* default is 8086! need 686 for sysenter */ @N@\
.XMM /* needed for fnclex and fxrstor */ @N@\
.MODEL flat, c @N@\
ASSUME fs:_DATA @N@\
.CODE
# endif
# define END_FILE END
/* we don't seem to need EXTERNDEF or GLOBAL */
# define DECLARE_FUNC(symbol) symbol PROC
# define DECLARE_EXPORTED_FUNC(symbol) symbol PROC EXPORT
# define END_FUNC(symbol) symbol ENDP
# define DECLARE_GLOBAL(symbol) PUBLIC symbol
/* XXX: this should be renamed FUNC_ENTRY_LABEL */
# define GLOBAL_LABEL(label)
/* use double colon (param always has its own colon) to make label visible outside proc */
# define ADDRTAKEN_LABEL(label) label:
# define GLOBAL_REF(label) label
# define BYTE byte ptr
# define WORD word ptr
# define DWORD dword ptr
# define QWORD qword ptr
# define MMWORD mmword ptr
# define XMMWORD xmmword ptr
# define YMMWORD ymmword ptr /* added in VS 2010 */
/* ml64 uses rip-rel automatically */
# define SYMREF(sym) [sym]
# define HEX(n) 0##n##h
# define SEGMEM(seg,mem) seg:[mem]
# define DECL_EXTERN(symbol) EXTERN symbol:PROC
/* include newline so we can put multiple on one line */
# define RAW(n) DB HEX(n) @N@
# ifdef X64
/* 64-bit SEH directives */
/* Declare non-leaf function (adjusts stack; makes calls; saves non-volatile regs): */
#  define DECLARE_FUNC_SEH(symbol) symbol PROC FRAME
/* Push a non-volatile register in prolog: */
#  define PUSH_SEH(reg) push reg @N@ .pushreg reg
/* Push a volatile register or an immed in prolog: */
#  define PUSH_NONCALLEE_SEH(reg) push reg @N@ .allocstack 8
#  define END_PROLOG .endprolog
# else
#  define DECLARE_FUNC_SEH(symbol) DECLARE_FUNC(symbol)
#  define PUSH_SEH(reg) push reg
#  define PUSH_NONCALLEE_SEH(reg) push reg
#  define END_PROLOG /* nothing */
# endif
/****************************************************/
#elif defined(ASSEMBLE_WITH_NASM)
# define START_FILE SECTION .text
# define END_FILE /* nothing */
/* for MacOS, at least, we have to add _ ourselves */
# define DECLARE_FUNC(symbol) global _##symbol
# define DECLARE_EXPORTED_FUNC(symbol) global _##symbol
# define END_FUNC(symbol) /* nothing */
# define DECLARE_GLOBAL(symbol) global _##symbol
# define GLOBAL_LABEL(label) _##label
# define ADDRTAKEN_LABEL(label) _##label
# define GLOBAL_REF(label) _##label
# define BYTE byte
# define WORD word
# define DWORD dword
# define QWORD qword
# define MMWORD qword
# define XMMWORD oword
# define YMMWORD yword
# ifdef X64
#  define SYMREF(sym) [rel GLOBAL_REF(sym)]
# else
#  define SYMREF(sym) [GLOBAL_REF(sym)]
# endif
# define HEX(n) 0x##n
# define SEGMEM(seg,mem) [seg:mem]
# define DECL_EXTERN(symbol) EXTERN GLOBAL_REF(symbol)
# define RAW(n) DB HEX(n) @N@
# define DECLARE_FUNC_SEH(symbol) DECLARE_FUNC(symbol)
# define PUSH_SEH(reg) push reg
# define PUSH_NONCALLEE_SEH(reg) push reg
# define END_PROLOG /* nothing */
/****************************************************/
#else
# error Unknown assembler: set one of the ASSEMBLE_WITH_{GAS,MASM,NASM} defines
#endif

/****************************************************/
/* Macros for writing cross-platform 64-bit + 32-bit code */
#ifdef X64
# define REG_XAX rax
# define REG_XBX rbx
# define REG_XCX rcx
# define REG_XDX rdx
# define REG_XSI rsi
# define REG_XDI rdi
# define REG_XBP rbp
# define REG_XSP rsp
# define SEG_TLS gs /* keep in sync w/ {linux,win32}/os_exports.h defines */
# ifdef WINDOWS
/* Arguments are passed in: rcx, rdx, r8, r9, then on stack right-to-left, but
 * leaving space on stack for the 1st 4.
 */
#  define ARG1 rcx
#  define ARG2 rdx
#  define ARG3 r8
#  define ARG4 r9
#  define ARG5 QWORD [40 + rsp] /* includes ret addr */
#  define ARG6 QWORD [48 + rsp]
#  define ARG7 QWORD [56 + rsp]
#  define ARG8 QWORD [64 + rsp]
#  define ARG9 QWORD [72 + rsp]
#  define ARG10 QWORD [80 + rsp]
#  define ARG5_NORETADDR QWORD [32 + rsp]
#  define ARG6_NORETADDR QWORD [40 + rsp]
#  define ARG7_NORETADDR QWORD [48 + rsp]
#  define ARG8_NORETADDR QWORD [56 + rsp]
#  define ARG9_NORETADDR QWORD [64 + rsp]
#  define ARG10_NORETADDR QWORD [72 + rsp]
# else
/* Arguments are passed in: rdi, rsi, rdx, rcx, r8, r9, then on stack right-to-left,
 * without leaving any space on stack for the 1st 6.
 */
#  define ARG1 rdi
#  define ARG2 rsi
#  define ARG3 rdx
#  define ARG4 rcx
#  define ARG5 r8
#  define ARG6 r9
#  define ARG7 QWORD [8 + rsp]
#  define ARG8 QWORD [16 + rsp]
#  define ARG9 QWORD [24 + rsp]
#  define ARG10 QWORD [32 + rsp]
#  define ARG5_NORETADDR ARG5
#  define ARG6_NORETADDR ARG6
#  define ARG7_NORETADDR QWORD [rsp]
#  define ARG8_NORETADDR QWORD [8 + rsp]
#  define ARG9_NORETADDR QWORD [16 + rsp]
#  define ARG10_NORETADDR QWORD [24 + rsp]
# endif
# define ARG1_NORETADDR ARG1
# define ARG2_NORETADDR ARG2
# define ARG3_NORETADDR ARG3
# define ARG4_NORETADDR ARG4
# define ARG_SZ 8
# define PTRSZ QWORD
#else /* x86 */
# define REG_XAX eax
# define REG_XBX ebx
# define REG_XCX ecx
# define REG_XDX edx
# define REG_XSI esi
# define REG_XDI edi
# define REG_XBP ebp
# define REG_XSP esp
# define SEG_TLS fs /* keep in sync w/ {linux,win32}/os_exports.h defines */
# define ARG_SZ 4
# define PTRSZ DWORD
/* Arguments are passed on stack right-to-left. */
# define ARG1 DWORD [4 + esp] /* includes ret addr */
# define ARG2 DWORD [8 + esp]
# define ARG3 DWORD [12 + esp]
# define ARG4 DWORD [16 + esp]
# define ARG5 DWORD [20 + esp]
# define ARG6 DWORD [24 + esp]
# define ARG7 DWORD [28 + esp]
# define ARG8 DWORD [32 + esp]
# define ARG9 DWORD [36 + esp]
# define ARG10 DWORD [40 + esp]
# define ARG1_NORETADDR DWORD [0 + esp]
# define ARG2_NORETADDR DWORD [4 + esp]
# define ARG3_NORETADDR DWORD [8 + esp]
# define ARG4_NORETADDR DWORD [12 + esp]
# define ARG5_NORETADDR DWORD [16 + esp]
# define ARG6_NORETADDR DWORD [20 + esp]
# define ARG7_NORETADDR DWORD [24 + esp]
# define ARG8_NORETADDR DWORD [28 + esp]
# define ARG9_NORETADDR DWORD [32 + esp]
# define ARG10_NORETADDR DWORD [36 + esp]
#endif

/* Keep in sync with arch_exports.h. */
#define FRAME_ALIGNMENT 16

/* From globals_shared.h, but we can't include that into asm code. */
#ifdef X64
# define IF_X64(x) x
# define IF_X64_ELSE(x, y) x
# define IF_NOT_X64(x)
#else
# define IF_X64(x)
# define IF_X64_ELSE(x, y) y
# define IF_NOT_X64(x) x
#endif

#ifdef X64
# define PUSHF   pushfq
# define POPF    popfq
# ifdef WINDOWS
#  define STACK_PAD(tot, gt4, mod4) \
        lea      REG_XSP, [-32 - ARG_SZ*gt4 + REG_XSP]
#  define STACK_PAD_NOPUSH(tot, gt4, mod4) STACK_PAD(tot, gt4, mod4)
#  define STACK_UNPAD(tot, gt4, mod4) \
        lea      REG_XSP, [32 + ARG_SZ*gt4 + REG_XSP]
/* we split these out just to avoid nop lea for x64 linux */
#  define STACK_PAD_LE4(tot, mod4) STACK_PAD(0/*doesn't matter*/, 0, mod4)
#  define STACK_UNPAD_LE4(tot, mod4) STACK_UNPAD(tot, 0, mod4)
# else
#  define STACK_PAD(tot, gt4, mod4) \
        lea      REG_XSP, [-ARG_SZ*gt4 + REG_XSP]
#  define STACK_PAD_NOPUSH(tot, gt4, mod4) STACK_PAD(tot, gt4, mod4)
#  define STACK_UNPAD(tot, gt4, mod4) \
        lea      REG_XSP, [ARG_SZ*gt4 + REG_XSP]
#  define STACK_PAD_LE4(tot, mod4) /* nothing */
#  define STACK_UNPAD_LE4(tot, mod4) /* nothing */
# endif
/* we split these out just to avoid nop lea for x86 mac */
# define STACK_PAD_ZERO STACK_PAD_LE4(0, 4)
# define STACK_UNPAD_ZERO STACK_UNPAD_LE4(0, 4)
# define SETARG(argoffs, argreg, p) \
        mov      argreg, p
#else
# define PUSHF   pushfd
# define POPF    popfd
# ifdef MACOS
/* Mac requires 32-bit to have 16-byte stack alignment (at call site)
 * Pass 4 instead of 0 for mod4 to avoid wasting stack space.
 */
#  define STACK_PAD(tot, gt4, mod4) \
        lea      REG_XSP, [-ARG_SZ*((tot) + 4 - (mod4)) + REG_XSP]
#  define STACK_PAD_NOPUSH(tot, gt4, mod4) STACK_PAD(tot, gt4, mod4)
#  define STACK_UNPAD(tot, gt4, mod4) \
        lea      REG_XSP, [ARG_SZ*((tot) + 4 - (mod4)) + REG_XSP]
#  define STACK_PAD_LE4(tot, mod4) STACK_PAD(tot, 0, mod4)
#  define STACK_UNPAD_LE4(tot, mod4) STACK_UNPAD(tot, 0, mod4)
#  define STACK_PAD_ZERO /* nothing */
#  define STACK_UNPAD_ZERO /* nothing */
/* p cannot be a memory operand, naturally */
#  define SETARG(argoffs, argreg, p) \
        mov      PTRSZ [ARG_SZ*argoffs + REG_XSP], p
# else
#  define STACK_PAD(tot, gt4, mod4) /* nothing */
#  define STACK_PAD_NOPUSH(tot, gt4, mod4) \
        lea      REG_XSP, [-ARG_SZ*tot + REG_XSP]
#  define STACK_UNPAD(tot, gt4, mod4) \
        lea      REG_XSP, [ARG_SZ*tot + REG_XSP]
#  define STACK_PAD_LE4(tot, mod4) /* nothing */
#  define STACK_UNPAD_LE4(tot, mod4) STACK_UNPAD(tot, 0, mod4)
#  define STACK_PAD_ZERO /* nothing */
#  define STACK_UNPAD_ZERO /* nothing */
/* SETARG usage is order-dependent on 32-bit.  we could avoid that
 * by having STACK_PAD allocate the stack space and do a mov here.
 */
#  define SETARG(argoffs, argreg, p) \
        push     p
# endif
#endif /* !X64 */
/* CALLC* are for C calling convention callees only.
 * Caller must ensure that if params are passed in regs there are no conflicts.
 * Caller can rely on us storing each parameter in reverse order.
 * For x64, caller must arrange for 16-byte alignment at end of arg setup.
 */
#define CALLC0(callee)     \
        STACK_PAD_ZERO  @N@\
        call     callee @N@\
        STACK_UNPAD_ZERO
#define CALLC1(callee, p1)     \
        STACK_PAD_LE4(1, 1) @N@\
        SETARG(0, ARG1, p1) @N@\
        call     callee     @N@\
        STACK_UNPAD_LE4(1, 1)
#define CALLC2(callee, p1, p2)    \
        STACK_PAD_LE4(2, 2)    @N@\
        SETARG(1, ARG2, p2)    @N@\
        SETARG(0, ARG1, p1)    @N@\
        call     callee        @N@\
        STACK_UNPAD_LE4(2, 2)
#define CALLC3(callee, p1, p2, p3)    \
        STACK_PAD_LE4(3, 3)        @N@\
        SETARG(2, ARG3, p3)        @N@\
        SETARG(1, ARG2, p2)        @N@\
        SETARG(0, ARG1, p1)        @N@\
        call     callee            @N@\
        STACK_UNPAD_LE4(3, 3)
#define CALLC4(callee, p1, p2, p3, p4)    \
        STACK_PAD_LE4(4, 4)            @N@\
        SETARG(3, ARG4, p4)            @N@\
        SETARG(2, ARG3, p3)            @N@\
        SETARG(1, ARG2, p2)            @N@\
        SETARG(0, ARG1, p1)            @N@\
        call     callee                @N@\
        STACK_UNPAD_LE4(4, 4)
#define CALLC5(callee, p1, p2, p3, p4, p5)    \
        STACK_PAD(5, 1, 1)                 @N@\
        SETARG(4, ARG5_NORETADDR, p5)      @N@\
        SETARG(3, ARG4, p4)                @N@\
        SETARG(2, ARG3, p3)                @N@\
        SETARG(1, ARG2, p2)                @N@\
        SETARG(0, ARG1, p1)                @N@\
        call     callee                    @N@\
        STACK_UNPAD(5, 1, 1)
#define CALLC6(callee, p1, p2, p3, p4, p5, p6)\
        STACK_PAD(6, 2, 2)                 @N@\
        SETARG(5, ARG6_NORETADDR, p6)      @N@\
        SETARG(4, ARG5_NORETADDR, p5)      @N@\
        SETARG(3, ARG4, p4)                @N@\
        SETARG(2, ARG3, p3)                @N@\
        SETARG(1, ARG2, p2)                @N@\
        SETARG(0, ARG1, p1)                @N@\
        call     callee                    @N@\
        STACK_UNPAD(6, 2, 2)
#define CALLC7(callee, p1, p2, p3, p4, p5, p6, p7)\
        STACK_PAD(7, 3, 3)                 @N@\
        SETARG(6, ARG7_NORETADDR, p7)      @N@\
        SETARG(5, ARG6_NORETADDR, p6)      @N@\
        SETARG(4, ARG5_NORETADDR, p5)      @N@\
        SETARG(3, ARG4, p4)                @N@\
        SETARG(2, ARG3, p3)                @N@\
        SETARG(1, ARG2, p2)                @N@\
        SETARG(0, ARG1, p1)                @N@\
        call     callee                    @N@\
        STACK_UNPAD(7, 3, 3)

/* Versions to help with Mac stack aligmnment.  _FRESH means that the
 * stack has not been changed since function entry and thus for Mac 32-bit
 * esp ends in 0xc.  We simply add 1 to the # args the pad code should assume,
 * resulting in correct alignment when combined with the retaddr.
 */
#define CALLC1_FRESH(callee, p1) \
        STACK_PAD_LE4(1, 2) @N@\
        SETARG(0, ARG1, p1) @N@\
        call     callee     @N@\
        STACK_UNPAD_LE4(1, 2)
#define CALLC2_FRESH(callee, p1, p2) \
        STACK_PAD_LE4(2, 3)    @N@\
        SETARG(1, ARG2, p2)    @N@\
        SETARG(0, ARG1, p1)    @N@\
        call     callee        @N@\
        STACK_UNPAD_LE4(2, 3)

/* For stdcall callees */
#ifdef X64
# define CALLWIN0 CALLC0
# define CALLWIN1 CALLC1
# define CALLWIN2 CALLC2
#else
# define CALLWIN0(callee)     \
        STACK_PAD_ZERO     @N@\
        call     callee
# define CALLWIN1(callee, p1)    \
        STACK_PAD_LE4(1, 1)   @N@\
        SETARG(0, ARG1, p1)   @N@\
        call     callee
# define CALLWIN2(callee, p1, p2)    \
        STACK_PAD_LE4(2, 2)       @N@\
        SETARG(1, ARG2, p2)       @N@\
        SETARG(0, ARG1, p1)       @N@\
        call     callee
#endif


#endif /* _ASM_DEFINES_ASM_ */

/**
 * @file
 *
 * @date 26.11.10
 * @author Nikolay Korotky
 */

#include "longjmp.h"

        .text

        .global setjmp
setjmp:
	movl    4(%esp), %ecx   /* fetch buffer */
	movl    %ebx, 0(%ecx)
	movl    %esi, 4(%ecx)
	movl    %edi, 8(%ecx)
	movl    %ebp, 12(%ecx)  /* save frame pointer of caller */
	popl    %edx
	movl    %esp, 16(%ecx)  /* save stack pointer of caller */
	movl    %edx, 20(%ecx)  /* save pc of caller */
	xorl    %eax, %eax
	jmp     *%edx

	.global longjmp
longjmp:
	movl    8(%esp), %eax    /* return(v) */
	movl    4(%esp), %ecx    /* fetch buffer */
	movl    0(%ecx), %ebx
	movl    4(%ecx), %esi
	movl    8(%ecx), %edi
	movl    12(%ecx), %ebp
	movl    16(%ecx), %esp
	orl     %eax, %eax
	jnz     0f
	incl    %eax
0:	jmp     *20(%ecx)         /* done, return.... */

// https://github.com/embox/embox

/*
Copyright 2008-2016, Mathematics and Mechanics faculty
                  of Saint-Petersburg State University. All rights reserved.
Copyright 2008-2016, Embox Ltd. All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions
are met:
1. Redistributions of source code must retain the above copyright
   notice, this list of conditions and the following disclaimer.
2. Redistributions in binary form must reproduce the above copyright
   notice, this list of conditions and the following disclaimer in the
   documentation and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
SUCH DAMAGE.
*/
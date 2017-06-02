/*
 * Copyright (c) 2010 Luigi Rizzo, Universita` di Pisa
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */
/*
 * headers to build userland ipfw under tcc.
 */
 
#ifndef _TCC_GLUE_H
#define	_TCC_GLUE_H

//#define	__restrict
//#define	NULL	((void *)0)
//typedef int size_t;
typedef unsigned char	u_char;
typedef unsigned char	uint8_t;
typedef unsigned char	u_int8_t;
typedef unsigned short	u_short;
typedef unsigned short	uint16_t;
typedef unsigned short	u_int16_t;
typedef int		__int32_t;
typedef int		int32_t;
//typedef int		socklen_t;
typedef int		pid_t;
//typedef unsigned int	time_t;
typedef unsigned int	uint;
typedef unsigned int	u_int;
typedef unsigned int	uint32_t;
typedef unsigned int	u_int32_t;
typedef unsigned int	gid_t;
typedef unsigned int	uid_t;
typedef unsigned long	u_long;
typedef unsigned long	uintptr_t;
//typedef long long int	int64_t;
//typedef unsigned long long	int uint64_t;
//typedef unsigned long long	int u_int64_t;

#endif /* _TCC_GLUE_H */

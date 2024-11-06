/*********************************************************************
 * Copyright (C) 2017-2022 Xilinx, Inc.
 * Copyright (C) 2022-2024 Advanced Micro Devices, Inc.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 51 Franklin St, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 *
 ********************************************************************/

#ifndef INCLUDE_HELPER_H_
#define INCLUDE_HELPER_H_

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ARRAY_SIZE(a)  (sizeof(a)/sizeof((a)[0]))
#define ERRSTR strerror(errno)
#define ASSERT2(cond, ...) 					\
		do {							\
			if (!(cond)) { 				\
				int errsv = errno;			\
				fprintf(stderr, "ERROR(%s:%d) : ",	\
						__FILE__, __LINE__);	\
				errno = errsv;				\
				fprintf(stderr,  __VA_ARGS__);		\
				abort();				\
			}						\
		} while(0)

#define BIT(n)	(1 << (n))

#endif /* INCLUDE_HELPER_H_ */

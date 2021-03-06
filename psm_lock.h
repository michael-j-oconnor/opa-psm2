/*

  This file is provided under a dual BSD/GPLv2 license.  When using or
  redistributing this file, you may do so under either license.

  GPL LICENSE SUMMARY

  Copyright(c) 2015 Intel Corporation.

  This program is free software; you can redistribute it and/or modify
  it under the terms of version 2 of the GNU General Public License as
  published by the Free Software Foundation.

  This program is distributed in the hope that it will be useful, but
  WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  General Public License for more details.

  Contact Information:
  Intel Corporation, www.intel.com

  BSD LICENSE

  Copyright(c) 2015 Intel Corporation.

  Redistribution and use in source and binary forms, with or without
  modification, are permitted provided that the following conditions
  are met:

    * Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in
      the documentation and/or other materials provided with the
      distribution.
    * Neither the name of Intel Corporation nor the names of its
      contributors may be used to endorse or promote products derived
      from this software without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

*/

/* Copyright (c) 2003-2014 Intel Corporation. All rights reserved. */

#ifndef _PSMI_IN_USER_H
#error psm_lock.h not meant to be included directly, include psm_user.h instead
#endif

#ifndef _PSMI_LOCK_H
#define _PSMI_LOCK_H

#ifndef PSMI_USE_PTHREAD_SPINLOCKS
#define PSMI_USE_PTHREAD_SPINLOCKS 0
#endif

#if PSMI_USE_PTHREAD_SPINLOCKS
typedef pthread_spinlock_t psmi_spinlock_t;

#define psmi_spin_init(lock)	  pthread_spin_init(lock, \
					PTHREAD_PROCESS_PRIVATE)
#define psmi_spin_lock(lock)	  pthread_spin_lock(lock)
#define psmi_spin_trylock(lock) pthread_spin_trylock(lock)
#define psmi_spin_unlock(lock)  pthread_spin_unlock(lock)
#else
typedef ips_atomic_t psmi_spinlock_t;
#define PSMI_SPIN_LOCKED    1
#define PSMI_SPIN_UNLOCKED  0

PSMI_ALWAYS_INLINE(int psmi_spin_init(psmi_spinlock_t *lock))
{
	ips_atomic_set(lock, PSMI_SPIN_UNLOCKED);
	return 0;
}

PSMI_ALWAYS_INLINE(int psmi_spin_trylock(psmi_spinlock_t *lock))
{
	if (ips_atomic_cmpxchg(lock, PSMI_SPIN_UNLOCKED, PSMI_SPIN_LOCKED)
			== PSMI_SPIN_UNLOCKED)
		return 0;
	else
		return EBUSY;
}

PSMI_ALWAYS_INLINE(int psmi_spin_lock(psmi_spinlock_t *lock))
{
	while (psmi_spin_trylock(lock) == EBUSY) {
	}
	return 0;
}

PSMI_ALWAYS_INLINE(int psmi_spin_unlock(psmi_spinlock_t *lock))
{
	atomic_set(lock, PSMI_SPIN_UNLOCKED);
	return 0;
}
#endif /* PSMI_USE_PTHREAD_SPINLOCKS */

#endif /* _PSMI_LOCK_H */

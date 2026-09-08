/*============================================================================
  OS, an object-oriented C operating system information class.
  Copyright (C) 2009-2023, 2026 by Zack T Smith.

  Object-Oriented C is free software: you can redistribute it and/or modify
  it under the terms of the GNU Lesser General Public License as published
  by the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.
 
  Object-Oriented C is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU Lesser General Public License for more details.
 
  You should have received a copy of the GNU Lesser General Public License
  along with this software.  If not, see <http://www.gnu.org/licenses/>.

  The author may be reached at 3 at zs3 dot me.
 *===========================================================================*/

#ifndef _OOC_OPERATINGSYSTEM_H
#define _OOC_OPERATINGSYSTEM_H

#include "String.h"
#include "MutableArray.h"

#define DECLARE_OPERATINGSYSTEM_METHODS(TYPE_POINTER) \

struct operatingsysteminfo;

typedef struct operatingsysteminfoclass {
	DECLARE_OBJECT_CLASS_VARS
	DECLARE_OBJECT_METHODS(struct operatingsysteminfo*)
	DECLARE_OPERATINGSYSTEM_METHODS(struct operatingsysteminfo*)
} OSClass;

extern OSClass *_OSClass;
extern OSClass* OSClass_init (OSClass*);

#define DECLARE_OPERATINGSYSTEM_INSTANCE_VARS(TYPE_POINTER) 

typedef struct operatingsysteminfo {
	OSClass *is_a;
	DECLARE_OBJECT_INSTANCE_VARS(struct operatingsysteminfo*)
	DECLARE_OPERATINGSYSTEM_INSTANCE_VARS(struct operatingsysteminfo*)
} OS;

extern OS* OS_init (OS* object);
extern void OS_destroy (Any* object);

extern bool OS_isRoot ();
extern long OS_load ();
extern long OS_uptime ();
extern unsigned OS_memoryPageSize ();
extern String* OS_type ();
extern String* OS_name ();
extern float OS_version ();
extern String* OS_kernelName ();
extern String* OS_kernelRelease ();
extern String* OS_username ();
extern unsigned OS_userid ();
extern size_t OS_freeRAM ();
extern size_t OS_totalRAM ();

#endif


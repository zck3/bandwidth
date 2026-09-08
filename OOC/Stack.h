/*============================================================================
  Stack implements a stack of Objects.
  Copyright (C) Zack Smith 2018

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

#ifndef _OOC_STACK_H
#define _OOC_STACK_H


#include "MutableArray.h"

#define DECLARE_STACK_INSTANCE_VARS(TYPE_POINTER) 

#define DECLARE_STACK_METHODS(TYPE_POINTER) \
	bool (*isEmpty) (TYPE_POINTER); \
	void (*push) (TYPE_POINTER, Any *item); \
	void (*pushCString) (TYPE_POINTER, const char*); \
	void (*pushInt) (TYPE_POINTER, int); \
	void (*pushDouble) (TYPE_POINTER, double); \
	Any* (*pop) (TYPE_POINTER); \
	Any* (*tos) (TYPE_POINTER); \
	void (*swap) (TYPE_POINTER);

struct stack;

typedef struct stackclass {
	DECLARE_OBJECT_CLASS_VARS
        DECLARE_OBJECT_METHODS(struct stack*)
        DECLARE_ARRAY_METHODS(struct stack*)
        DECLARE_MUTABLE_ARRAY_METHODS(struct stack*)
        DECLARE_STACK_METHODS(struct stack*)
} StackClass;

extern StackClass *_StackClass;
extern StackClass *StackClass_init (StackClass*);

typedef struct stack {
        StackClass *is_a;
	DECLARE_OBJECT_INSTANCE_VARS(struct stack*)
	DECLARE_ARRAY_INSTANCE_VARS(struct stack*)
	DECLARE_MUTABLE_ARRAY_INSTANCE_VARS(struct stack*)
	DECLARE_STACK_INSTANCE_VARS(struct stack*)
} Stack;

extern Stack *Stack_new ();
extern Stack *Stack_init (Stack*);
extern void Stack_destroy (Any *);

#endif

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

#include "Stack.h"
#include "String.h"
#include "Int.h"
#include "Double.h"
#include "Log.h"

StackClass *_StackClass = NULL;

void Stack_destroy (Any *self)
{
	if (!self) {
		return;
	}
	verifyCorrectClass(self,Stack);

	MutableArray_destroy (self);
}

Stack *Stack_init (Stack* restrict self)
{
	ENSURE_CLASS_READY(Stack);
	if (self) {
		MutableArray_init ((MutableArray*) self);
		self->is_a = _StackClass;
	}
	return self;
}

static void Stack_push (Stack* restrict self, Any *item)
{
	if (!self || !item) {
		return;
	}
	verifyCorrectClass(self,Stack);

	$(self, append, item);
}

// NOTE! This always returns an object that has been retained.
static Any *Stack_pop (Stack* restrict self)
{
	if (!self) {
		return NULL;
	}
	verifyCorrectClass(self,Stack);

	Object *last = $(self, last);
	if (last) {
		retain(last);
		$(self, removeLast);
	}
	return last;
}

static Any *Stack_tos (Stack* restrict self)
{
	if (!self) {
		return NULL;
	}
	verifyCorrectClass(self,Stack);

	return $(self, last);
}

static bool Stack_isEmpty (Stack* restrict self)
{
	if (!self) {
		return true;
	}
	verifyCorrectClass(self,Stack);

	return 0 == $(self, count);
}

static void Stack_swap (Stack* restrict self)
{
	if (!self) {
		return;
	}
	verifyCorrectClass(self,Stack);

	unsigned count = $(self, count);
	if (count >= 2) {
		Any *last = self->array[count-1];
		Any *nextToLast = self->array[count-2];
		self->array[count-1] = nextToLast;
		self->array[count-2] = last;
	}
}

void Stack_pushCString (Stack *self, const char*string) 
{
	if (!self) {
		return;
	}
	verifyCorrectClass(self,Stack);
	Stack_push (self, _String(string ?: ""));
}

void Stack_pushInt (Stack *self, int integer)
{
	if (!self) {
		return;
	}
	verifyCorrectClass(self,Stack);
	Stack_push (self, _Int(integer));
}

void Stack_pushDouble (Stack *self, double d)
{
	if (!self) {
		return;
	}
	verifyCorrectClass(self,Stack);
	Stack_push (self, _Double(d));
}

StackClass *StackClass_init (StackClass* class)
{
        SET_SUPERCLASS(MutableArray);

	SET_OVERRIDDEN_METHOD_POINTER(Stack,destroy);

	SET_METHOD_POINTER(Stack,isEmpty);
	SET_METHOD_POINTER(Stack,push);
	SET_METHOD_POINTER(Stack,pop);
	SET_METHOD_POINTER(Stack,tos);
	SET_METHOD_POINTER(Stack,swap);
	SET_METHOD_POINTER(Stack,pushCString);
	SET_METHOD_POINTER(Stack,pushInt);
	SET_METHOD_POINTER(Stack,pushDouble);

        VALIDATE_CLASS_STRUCT(class);
	return class;
}


/*============================================================================
  Object, an object-oriented C object base class.
  Copyright (C) 2019, 2023 by Zack T Smith.

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

#include <stdio.h>
#include <stdlib.h>

#include "Object.h"

ObjectClass *_ObjectClass = NULL;

void Object_destroy (Any* self)
{
        DEBUG_DESTROY;

	if (!self) {
		return;
	}
	verifyCorrectClassOrSubclass(self,Object);
}

static void Object_describe (Object* restrict self, FILE* output)
{
	if (!self) {
		return;
	}
	verifyCorrectClassOrSubclass(self,Object);
	fprintf (output ?: stdout, "%s", $(self, className));
}

static void Object_print (Object* restrict self, FILE* output)
{
}

// This must be overridden in subclasses.
static unsigned Object_hash (Object* restrict self)
{
	return 0;
}

// This must be overridden in subclasses.
static bool Object_equals (Object* restrict self, Any *other)
{
	return false;
}

// This must be overridden in subclasses.
static int Object_compare (Object* restrict self, Any *other)
{
	return 0;
}

static const char *Object_className (Object* restrict self)
{
	if (!self) {
		return NULL;
	}
	EmptyClass *class = (EmptyClass*)self->is_a;
	if (!class) {
		return NULL;
	}
	return class->_className;
}

static void *Object_class (Object* restrict self)
{
	if (!self) {
		return NULL;
	}
	return self->is_a;
}

static long Object_message (Object* restrict self, long message, Any *sender, long first, long second)
{
	return 0;
}

ObjectClass* ObjectClass_init (ObjectClass *class)
{
	SET_METHOD_POINTER(Object,class);
	SET_METHOD_POINTER(Object,className);
	SET_METHOD_POINTER(Object,describe);
	SET_METHOD_POINTER(Object,destroy);
	SET_METHOD_POINTER(Object,equals);
	SET_METHOD_POINTER(Object,print);
	SET_METHOD_POINTER(Object,message);
	SET_METHOD_POINTER(Object,hash);
	SET_METHOD_POINTER(Object,compare);

	VALIDATE_CLASS_STRUCT(class);
	return class;
}

Object* Object_init (Object *object)
{
	ENSURE_CLASS_READY(Object);

	if (object == NULL) {
		return NULL;
	}

	object->is_a = _ObjectClass;
	object->magic = OBJECT_MAGIC_NUMBER;
	object->retainCount = 0;

	return object;
}


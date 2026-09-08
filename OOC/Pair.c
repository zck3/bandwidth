/*============================================================================
  Pair, an object-oriented C cluster of objects class.
  Copyright (C) 2026 by Zack T Smith.

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

#include "Pair.h"

#include <stdlib.h>

PairClass *_PairClass = NULL;

void Pair_destroy (Any *self_)
{
	DEBUG_DESTROY;
	if (!self_) {
		return;
	}
	verifyCorrectClass(self_,Pair);

	Pair *self = self_;
	releaseAndClear (self->first);
	releaseAndClear (self->second);

	Object_destroy((Object*)self);
}

static void Pair_print (Pair* restrict self, FILE *outputFile) 
{ 
	if (!self) {
		return;
	}
	verifyCorrectClass(self,Pair);

	if (!outputFile) {
		outputFile = stdout;
	}

	fputc ('(', outputFile);
	$((Object*)self->first, print, outputFile);
	if (self->second) {
		fputc (',', outputFile);
		$((Object*)self->second, print, outputFile);
	}
	fputc (')', outputFile);
}

static void Pair_describe (Pair* restrict self, FILE *outputFile) 
{ 
	if (!self) {
		return;
	}
	verifyCorrectClass(self,Pair);

	if (!outputFile) {
		outputFile = stdout;
	}

	fprintf (outputFile, "%s", $(self, className));
}

MAKE_GETTER_SCALAR(Pair,Any*,first)
MAKE_GETTER_SCALAR(Pair,Any*,second)

MAKE_SETTER_OBJECT(Pair,Any*,first,setFirst)
MAKE_SETTER_OBJECT(Pair,Any*,second,setSecond)

Pair *Pair_with (Any* first, Any* second)
{
	Pair *self = new(Pair);
	if (first) {
		self->first = retain(first);
	}
	if (second) {
		self->second = retain(second);
	}
	return self;
}

PairClass* PairClass_init (PairClass *class)
{
	SET_SUPERCLASS(Object);

	SET_OVERRIDDEN_METHOD_POINTER(Pair,destroy);
	SET_OVERRIDDEN_METHOD_POINTER(Pair,describe);
	SET_OVERRIDDEN_METHOD_POINTER(Pair,print);

	SET_METHOD_POINTER(Pair,first);
	SET_METHOD_POINTER(Pair,second);

	SET_METHOD_POINTER(Pair,setFirst);
	SET_METHOD_POINTER(Pair,setSecond);
	
        VALIDATE_CLASS_STRUCT(_PairClass);
	return class;
}

Pair* Pair_init (Pair* restrict self)
{
	ENSURE_CLASS_READY(Pair);

	if (self) {
		Object_init ((Object*) self);
		self->is_a = _PairClass;

		self->first = NULL;
		self->second = NULL;
	}

	return self;
}


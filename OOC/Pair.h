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

#ifndef _OOC_PAIR_H
#define _OOC_PAIR_H

#include <stdbool.h>
#include <math.h>

#include "Object.h"

#define DECLARE_PAIR_INSTANCE_VARS(FOO) \
	Any *first; \
	Any *second; 

#define DECLARE_PAIR_METHODS(TYPE_POINTER) \
	Any *(*first) (TYPE_POINTER); \
	Any *(*second) (TYPE_POINTER); \
	void (*setFirst) (TYPE_POINTER, Any*); \
	void (*setSecond) (TYPE_POINTER, Any*); 

struct pair;

typedef struct pairclass {
	DECLARE_OBJECT_CLASS_VARS
        DECLARE_OBJECT_METHODS(struct pair*)
        DECLARE_PAIR_METHODS(struct pair*)
} PairClass;

extern PairClass *_PairClass;
extern PairClass* PairClass_init (PairClass*);

typedef struct pair {
        PairClass *is_a;
	DECLARE_OBJECT_INSTANCE_VARS(struct pair*)
	DECLARE_PAIR_INSTANCE_VARS(struct pair*)
} Pair;

extern void Pair_destroy (Any *);
extern Pair *Pair_init (Pair *self);

// Need to implement vararg
extern Pair* Pair_with (Any*, Any*);

#endif

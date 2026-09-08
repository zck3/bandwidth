/*============================================================================
  ProgressBar, an object-oriented C progress bar class.
  Copyright (C) 2019, 2022, 2026 by Zack T Smith.

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

// This is based on my FrugalWidget project's ProgressBar class.

#ifndef _OOC_PROGRESSBAR_H
#define _OOC_PROGRESSBAR_H

#include "View.h"
#include "Font.h"

#define DECLARE_PROGRESSBAR_INSTANCE_VARS(FOO) \
	float percentage; \
	bool showingGradient; \
	RGBA upperGradientColor; \
	RGBA lowerGradientColor; 

#define DECLARE_PROGRESSBAR_METHODS(TYPE_POINTER) \
	float (*percentage) (TYPE_POINTER); \
	void (*setPercentage) (TYPE_POINTER, float); \
	void (*setGradientColors) (TYPE_POINTER, RGB, RGB);

struct progressbar;

typedef struct progressbarclass {
	DECLARE_OBJECT_CLASS_VARS
        DECLARE_OBJECT_METHODS(struct progressbar*)
        DECLARE_VIEW_METHODS(struct progressbar*)
        DECLARE_PROGRESSBAR_METHODS(struct progressbar*)
} ProgressBarClass;

extern ProgressBarClass *_ProgressBarClass;
extern ProgressBarClass* ProgressBarClass_init (ProgressBarClass*);

typedef struct progressbar {
        ProgressBarClass *is_a;
	DECLARE_OBJECT_INSTANCE_VARS(struct progressbar*)
	DECLARE_VIEW_INSTANCE_VARS(struct progressbar*)
	DECLARE_PROGRESSBAR_INSTANCE_VARS(struct progressbar*)
} ProgressBar;

extern ProgressBar *ProgressBar_new ();
extern void ProgressBar_destroy (Any *);
extern ProgressBar *ProgressBar_init (ProgressBar *self);
extern ProgressBar* ProgressBar_withPercentage (float);

// Convenience instantiator macro
#define _ProgressBar(VALUE) ProgressBar_withPercentage(VALUE)

#endif

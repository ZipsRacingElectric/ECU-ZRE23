// Macros
//   Author: Derek Dunn
//   Created: 21.09.13
//   Updated: 23.02.16
//   Description: File to store #defined macros.
//   
//   using #define to define a macro will substitute the code in where the macro is called
//   i.e. let MACRO be code declared using #define. Instead of the program jumping to the function code and adding to the stack,
//   wherever MACRO appears in the code, 'MACRO' is replaced by the actual code that 'MACRO' represents.

#ifndef MACROS_H
#define	MACROS_H

#include "configuration_variables.h"

#define CALC_SCALED_APPS2(x)            (x - APPS2_OFFSET) / APPS2_SCALING_FACTOR
#define CALC_APPS2_FROM_APPS1(x)        APPS2_SCALING_FACTOR * x + APPS2_OFFSET

#endif	/* MACROS_H */
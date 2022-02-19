/* 
 * File:   macro_functions.h
 * Author: Derek Dunn
 * Description: File to store #defined macro functions.
 * using #define to define a function will substitute the function code in where the macro is called
 * i.e. let MACRO_FUNCTION be a function declared using #define. Instead of the program jumping to the function code and adding to the stack, 
 * wherever MACRO_FUNCTION appears in the code, 'MACRO_FUNCTION' is replaced by the actual code that 'MACRO_CODE' represents.
 * This type of function is good to use in interrupts, as it is bad practice to call functions in interrupts.
 * Created on September 13, 2021, 7:52 PM
 */
#include "configuration_variables.h"

#ifndef MACRO_FUNCTIONS_H
#define	MACRO_FUNCTIONS_H

#define CALC_SCALED_APPS2(x)            (x - APPS2_OFFSET) / APPS2_SCALING_FACTOR
#define CALC_APPS2_FROM_APPS1(x)        APPS2_SCALING_FACTOR * x + APPS2_OFFSET

#endif	/* MACRO_FUNCTIONS_H */
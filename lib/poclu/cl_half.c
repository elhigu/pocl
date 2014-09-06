/* cl_half - helper functions for the cl_half datatype

   Copyright (c) 2013 Pekka Jääskeläinen / Tampere University of Technology
                      Timo Viitanen / Tampere University of Technology
   
   Permission is hereby granted, free of charge, to any person obtaining a copy
   of this software and associated documentation files (the "Software"), to deal
   in the Software without restriction, including without limitation the rights
   to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
   copies of the Software, and to permit persons to whom the Software is
   furnished to do so, subject to the following conditions:
   
   The above copyright notice and this permission notice shall be included in
   all copies or substantial portions of the Software.
   
   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
   IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
   FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
   AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
   OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
   THE SOFTWARE.
*/

#include <math.h>
#include "poclu.h"
#include <CL/opencl.h>
#include <stdint.h>
#include "config.h"

typedef union 
{
  int32_t i;
  float f;
} FloatConvUnion;

POCLU_API cl_half POCLU_CALL
poclu_float_to_cl_half(float value) 
{
  FloatConvUnion u;
  u.f = value;
  cl_half half = (u.i & 0x007FFFFF) >> 13;
  half |=(u.i & 0x07800000) >> 13;
  half |=(u.i & 0x40000000) >> 16;
  half |=(u.i & 0x80000000) >> 16;
  // TODO saturate overflows to inf
  return half;
}

#ifndef INFINITY
#define INFINITY 1.0/0.0
#endif

#ifndef NAN
#define NAN 0.0/0.0
#endif

POCLU_API float POCLU_CALL
poclu_cl_half_to_float(cl_half value) 
{
  if (value == 0xFC00) {
    return -INFINITY;
  }
  if (value == 0x7C00) {
    return INFINITY;
  }

  int sgn = ((value & 0x8000) >> 15);
  int exp = (value & 0x7C00) >> 10;
  int mant = value & 0x03FF;

  if (exp == 0x1F && mant != 0) {
    return NAN;
  }

  float v = (exp == 0) ? mant : mant | 0x0400; // 1.x if not denormal
  v /= 0x400;
  float mul = exp2(exp - 15);
  v *= mul;
  if (sgn) {
    v *= -1;
  }
  return v;
}

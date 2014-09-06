/* poclu - byte swap functions

   Copyright (c) 2012 Pekka Jääskeläinen / Tampere University of Technology
   
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

#include "poclu.h"
#include <CL/opencl.h>
#include "config.h"

#define GENERIC_BYTESWAP(__DTYPE, __WORD)                         \
  do {                                                            \
    int __i;                                                      \
    union _word_union                                             \
    {                                                             \
      __DTYPE full_word;                                          \
      unsigned char bytes[sizeof(__DTYPE)];                       \
    } __old, __neww;                                              \
                                                                  \
    __old.full_word = __WORD;                                     \
    for (__i = 0; __i < sizeof(__DTYPE); ++__i)                   \
      __neww.bytes[__i] = __old.bytes[sizeof(__DTYPE) - __i - 1]; \
    __WORD = __neww.full_word;                                    \
  } while(0)

static int
needs_swap(cl_device_id device) 
{
  cl_bool deviceLittle;
  clGetDeviceInfo 
    (device, CL_DEVICE_ENDIAN_LITTLE, sizeof(cl_bool), 
     &deviceLittle, NULL);

#if defined(WORDS_BIGENDIAN) && WORDS_BIGENDIAN == 1
  return deviceLittle;
#else
  return !deviceLittle;
#endif
}

POCLU_API cl_int POCLU_CALL
poclu_bswap_cl_int(cl_device_id device, cl_int original) 
{
  if (!needs_swap (device)) return original;
  GENERIC_BYTESWAP (cl_int, original);
  return original;
}

POCLU_API cl_half POCLU_CALL
poclu_bswap_cl_half(cl_device_id device, cl_half original)
{
  if (!needs_swap (device)) return original;
  GENERIC_BYTESWAP (cl_half, original);
  return original;
}

POCLU_API cl_float POCLU_CALL
poclu_bswap_cl_float(cl_device_id device, cl_float original)
{
  if (!needs_swap (device)) return original;
  GENERIC_BYTESWAP (cl_float, original);
  return original;
}

POCLU_API void POCLU_CALL
poclu_bswap_cl_int_array(cl_device_id device, cl_int* array, 
                         size_t num_elements)
{
  size_t i;
  if (!needs_swap (device)) return;
  for (i = 0; i < num_elements; ++i) 
    {
      GENERIC_BYTESWAP (cl_int, array[i]);
    }
}

POCLU_API void POCLU_CALL
poclu_bswap_cl_half_array(cl_device_id device, cl_half* array, 
                           size_t num_elements)
{
  size_t i;
  if (!needs_swap (device)) return;
  for (i = 0; i < num_elements; ++i) 
    {
      GENERIC_BYTESWAP (cl_half, array[i]);
    }
}

POCLU_API void POCLU_CALL
poclu_bswap_cl_float_array(cl_device_id device, cl_float* array, 
                           size_t num_elements)
{
  size_t i;
  if (!needs_swap (device)) return;
  for (i = 0; i < num_elements; ++i) 
    {
      GENERIC_BYTESWAP (cl_float, array[i]);
    }
}

POCLU_API void POCLU_CALL
poclu_bswap_cl_float2_array(cl_device_id device, cl_float2* array, 
                            size_t num_elements)
{
  size_t i;
  if (!needs_swap (device)) return;
  for (i = 0; i < num_elements; ++i) 
    {
      GENERIC_BYTESWAP (cl_float, array[i].s[0]);
      GENERIC_BYTESWAP (cl_float, array[i].s[1]);
    }
}

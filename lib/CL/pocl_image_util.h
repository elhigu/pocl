/* OpenCL runtime library: pocl_image_util image utility functions

   Copyright (c) 2012 Timo Viitanen / Tampere University of Technology
   
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

#ifndef POCL_IMAGE_UTIL_H
#define POCL_IMAGE_UTIL_H

#include "pocl.h"

#pragma GCC visibility push(hidden)

extern cl_int 
pocl_check_image_origin_region (const cl_mem image, 
                                const size_t *origin, 
                                const size_t *region);

extern void
pocl_get_image_information (cl_channel_order  ch_order, 
                            cl_channel_type   ch_type,
                            cl_int*           host_channels,
                            cl_int*           host_elem_size);

extern cl_int
pocl_write_image(cl_mem               image,
                 cl_device_id         device_id,
                 const size_t *       origin_, /*[3]*/
                 const size_t *       region_, /*[3]*/
                 size_t               host_row_pitch,
                 size_t               host_slice_pitch, 
                 const void *         ptr);

extern cl_int         
pocl_read_image(cl_mem               image,
                cl_device_id         device,
                const size_t *       origin, /*[3]*/
                const size_t *       region, /*[3]*/
                size_t               host_row_pitch,
                size_t               host_slice_pitch, 
                void *               ptr);


#pragma GCC visibility pop
                   
#endif

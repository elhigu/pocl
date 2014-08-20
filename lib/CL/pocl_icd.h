#include "config.h"

/* Installable Client Driver-realated things. */
#ifndef POCL_ICD_H
#define POCL_ICD_H

// stub out ICD related stuff 
#ifndef BUILD_ICD

#  define POCL_DEVICE_ICD_DISPATCH
#  define POCL_INIT_ICD_OBJECT(__obj__)

// rest of the file: ICD is enabled 
#else

// this define is a kludge!
// The ICD loaders seem to require OCL 1.1, so we cannot (can we?) leave deprecated 
// functions out
// Answer: not really. ICD loader will call OCL 1.1 function throught the
// function table, but the registered function can be then only stubs
// (perhaps with a warning) or even NULL (in this case, a program using
// OCL 1.1 function will crash: ICD Loaders does not do any check)
#  define CL_USE_DEPRECATED_OPENCL_1_1_APIS

#  ifndef CL_USE_DEPRECATED_OPENCL_1_1_APIS
#    error CL_USE_DEPRECATED_OPENCL_1_1_APIS not in use
#  endif

extern struct _cl_icd_dispatch pocl_dispatch;  //from clGetPlatformIDs.c

#  define POCL_DEVICE_ICD_DISPATCH &pocl_dispatch,
#  define POCL_INIT_ICD_OBJECT(__obj__) (__obj__)->dispatch=&pocl_dispatch

/* Define the ICD dispatch structure that gets filled below. 
 * Prefer to get it from ocl-icd, as that has compile time type checking
 * of the function signatures. This checks that they are in correct order.
 */
#ifdef HAVE_OCL_ICD 
#include <ocl_icd.h>
#else
struct _cl_icd_dispatch {
	void *funcptr[122];
};
#endif

/* The "implementation" of the _cl_device_id struct. 
 * Instantiated in clGetPlatformIDs.c
 *
 * TODO: the NULL entries are functions that lack implementation (or even stubs) in pocl
 */
#define POCL_ICD_DISPATCH {           \
  &POname(clGetPlatformIDs),          \
  &POname(clGetPlatformInfo), \
  &POname(clGetDeviceIDs), \
  &POname(clGetDeviceInfo), \
  &POname(clCreateContext), \
  &POname(clCreateContextFromType), \
  &POname(clRetainContext), \
  &POname(clReleaseContext), \
  &POname(clGetContextInfo), \
  &POname(clCreateCommandQueue), \
  &POname(clRetainCommandQueue), /* 10 */           \
  &POname(clReleaseCommandQueue), \
  &POname(clGetCommandQueueInfo), \
  NULL /*clSetCommandQueueProperty*/, \
  &POname(clCreateBuffer), \
  &POname(clCreateImage2D), \
  &POname(clCreateImage3D), \
  &POname(clRetainMemObject), \
  &POname(clReleaseMemObject), \
  &POname(clGetSupportedImageFormats), \
  &POname(clGetMemObjectInfo), /* 20 */             \
  &POname(clGetImageInfo), \
  &POname(clCreateSampler), \
  &POname(clRetainSampler), \
  &POname(clReleaseSampler), \
  &POname(clGetSamplerInfo), \
  &POname(clCreateProgramWithSource), \
  &POname(clCreateProgramWithBinary), \
  &POname(clRetainProgram), \
  &POname(clReleaseProgram), \
  &POname(clBuildProgram), /* 30 */ \
  &POname(clUnloadCompiler), \
  &POname(clGetProgramInfo), \
  &POname(clGetProgramBuildInfo), \
  &POname(clCreateKernel), \
  &POname(clCreateKernelsInProgram), \
  &POname(clRetainKernel), \
  &POname(clReleaseKernel), \
  &POname(clSetKernelArg), \
  &POname(clGetKernelInfo), \
  &POname(clGetKernelWorkGroupInfo), /* 40 */       \
  &POname(clWaitForEvents), \
  &POname(clGetEventInfo), \
  &POname(clRetainEvent), \
  &POname(clReleaseEvent), \
  &POname(clGetEventProfilingInfo), \
  &POname(clFlush), \
  &POname(clFinish), \
  &POname(clEnqueueReadBuffer), \
  &POname(clEnqueueWriteBuffer), \
  &POname(clEnqueueCopyBuffer), /* 50 */  \
  &POname(clEnqueueReadImage), \
  &POname(clEnqueueWriteImage), \
  &POname(clEnqueueCopyImage), \
  &POname(clEnqueueCopyImageToBuffer), \
  &POname(clEnqueueCopyBufferToImage), \
  &POname(clEnqueueMapBuffer), \
  &POname(clEnqueueMapImage), \
  &POname(clEnqueueUnmapMemObject), \
  &POname(clEnqueueNDRangeKernel), \
  &POname(clEnqueueTask), /* 60 */  \
  &POname(clEnqueueNativeKernel), \
  &POname(clEnqueueMarker), \
  &POname(clEnqueueWaitForEvents), \
  &POname(clEnqueueBarrier), \
  &POname(clGetExtensionFunctionAddress), \
  NULL, /* &POclCreateFromGLBuffer,      */ \
  &POname(clCreateFromGLTexture2D), \
  &POname(clCreateFromGLTexture3D), \
  NULL, /* &POclCreateFromGLRenderbuffer, */ \
  NULL, /* &POclGetGLObjectInfo,  70       */ \
  NULL, /* &POclGetGLTextureInfo,        */ \
  NULL, /* &POclEnqueueAcquireGLObjects, */ \
  NULL, /* &POclEnqueueReleaseGLObjects, */ \
  NULL, /* &POclGetGLContextInfoKHR,     */ \
  NULL, /* &clUnknown75 */      \
  NULL, /* &clUnknown76 */      \
  NULL, /* &clUnknown77 */      \
  NULL, /* &clUnknown78 */      \
  NULL, /* &clUnknown79 */      \
  NULL, /* &clUnknown80 */      \
  &POname(clSetEventCallback), \
  &POname(clCreateSubBuffer), \
  &POname(clSetMemObjectDestructorCallback), \
  &POname(clCreateUserEvent), \
  &POname(clSetUserEventStatus), \
  &POname(clEnqueueReadBufferRect), \
  &POname(clEnqueueWriteBufferRect), \
  &POname(clEnqueueCopyBufferRect), \
  NULL, /* &POclCreateSubDevicesEXT,     */ \
  &POname(clRetainDevice), /* &POclRetainDeviceEXT,         */ \
  &POname(clReleaseDevice), /* &POclReleaseDeviceEXT,        */ \
  NULL, /* &clUnknown92 */      \
  NULL, /* &POclCreateSubDevices,        */ \
  &POname(clRetainDevice), \
  &POname(clReleaseDevice), \
  &POname(clCreateImage), \
  NULL, /* &POclCreateProgramWithBuiltInKernels, */ \
  NULL, /* &POclCompileProgram,          */ \
  NULL, /* &POclLinkProgram,             */ \
  NULL, /* &POclUnloadPlatformCompiler,  */ \
  &POname(clGetKernelArgInfo), \
  NULL, /* &POclEnqueueFillBuffer,        */ \
  &POname(clEnqueueFillImage), \
  NULL, /* &POclEnqueueMigrateMemObjects, */ \
  &POname(clEnqueueMarkerWithWaitList), \
  NULL, /* &POclEnqueueBarrierWithWaitList, */ \
  NULL, /* &POclGetExtensionFunctionAddressForPlatform, */ \
  NULL, /* &POclCreateFromGLTexture,     */ \
}

#endif
#endif


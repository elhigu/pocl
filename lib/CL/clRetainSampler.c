#include "pocl_cl.h"
#include <stdlib.h>

CL_API_ENTRY cl_int CL_API_CALL
POname(clRetainSampler)(cl_sampler  sampler ) CL_API_SUFFIX__VERSION_1_0
{
  POCL_ABORT_UNIMPLEMENTED();
  return CL_SUCCESS;
}

POsym(clRetainSampler)

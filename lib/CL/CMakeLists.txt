include_directories(BEFORE "../../fix-include/OpenCL")
include_directories(AFTER "devices" ".")

add_subdirectory("devices")

set(POCL_LIB_SOURCES  "clCreateContextFromType.c"
                   "clReleaseContext.c"
                   "clRetainContext.c"
                   "clGetContextInfo.c"
                   "clCreateCommandQueue.c"
                   "clReleaseCommandQueue.c"
                   "clRetainCommandQueue.c"
                   "clGetCommandQueueInfo.c"
                   "clCreateBuffer.c"
                   "clCreateSubBuffer.c"
                   "clEnqueueFillImage.c"
                   "clEnqueueReadBuffer.c"
                   "clEnqueueReadBufferRect.c"
                   "clEnqueueMapBuffer.c"  "clEnqueueMapBuffer.h"
                   "clEnqueueUnmapMemObject.c"
                   "clEnqueueMarkerWithWaitList.c"
                   "clReleaseMemObject.c"
                   "clRetainMemObject.c"
                   "clGetMemObjectInfo.c"
                   "clSetMemObjectDestructorCallback.c"
                   "clCreateProgramWithSource.c"
                   "clCreateProgramWithBinary.c"
                   "clReleaseProgram.c"
                   "clRetainProgram.c"
                   "clBuildProgram.c"
                   "clCreateKernel.c"
                   "clReleaseKernel.c"
                   "clRetainKernel.c"
                   "clSetKernelArg.c"
                   "clEnqueueNDRangeKernel.c"
                   "clEnqueueNativeKernel.c"
                   "clCreateKernelsInProgram.c"
                   "clGetKernelInfo.c"
                   "clGetKernelArgInfo.c"
                   "clGetPlatformIDs.c"
                   "clGetPlatformInfo.c"
                   "clGetDeviceIDs.c"
                   "clGetDeviceInfo.c"
                   "clCreateContext.c"
                   "clGetProgramBuildInfo.c"
                   "clEnqueueWriteBuffer.c"
                   "clEnqueueWriteBufferRect.c"
                   "clEnqueueCopyBuffer.c"
                   "clEnqueueCopyBufferRect.c"
                   "clEnqueueCopyBufferToImage.c"
                   "clEnqueueCopyImageToBuffer.c"
                   "clFinish.c"
                   "clFlush.c"
                   "clEnqueueBarrier.c"
                   "clEnqueueMarker.c"
                   "clGetKernelWorkGroupInfo.c"
                   "clGetProgramInfo.c"
                   "clGetEventProfilingInfo.c"
                   "clWaitForEvents.c"
                   "clEnqueueWaitForEvents.c"
                   "clReleaseEvent.c"
                   "clRetainEvent.c"
                   "clSetEventCallback.c"
                   "clGetEventInfo.c"
                   "clCreateUserEvent.c"
                   "clSetUserEventStatus.c"
                   "clEnqueueTask.c"
                   "clCreateImage.c"
                   "clCreateImage2D.c"
                   "clCreateImage3D.c"
                   "clEnqueueReadImage.c"
                   "clEnqueueWriteImage.c"
                   "clCreateSampler.c"
                   "clReleaseSampler.c"
                   "clRetainSampler.c"
                   "clGetSamplerInfo.c"
                   "clEnqueueCopyImage.c"
                   "clEnqueueMapImage.c"
                   "clGetImageInfo.c"
                   "clCreateFromGLTexture2D.c"
                   "clCreateFromGLTexture3D.c"
                   "clUnloadCompiler.c"
                   "clGetSupportedImageFormats.c"
                   "clGetExtensionFunctionAddress.c"
                   "clIcdGetPlatformIDsKHR.c"
                   "clReleaseDevice.c"
                   "clRetainDevice.c"
                   "pocl_cl.h" "pocl_util.h" "pocl_util.c"
                   "pocl_image_util.c" "pocl_image_util.h"
                   "pocl_icd.h" "pocl_intfn.h" "pocl_llvm.h"
                   "pocl_runtime_config.c" "pocl_runtime_config.h"
                   "pocl_mem_management.c"  "pocl_mem_management.h"
                   "pocl_llvm_api.cc")

# With visual studio c++ compiler also for .c files
if (MSVC)
SET_SOURCE_FILES_PROPERTIES( ${POCL_LIB_SOURCES} PROPERTIES LANGUAGE CXX )
endif (MSVC)

set(LIBPOCL_OBJS "$<TARGET_OBJECTS:llvmpasses>;$<TARGET_OBJECTS:libpocl_unlinked_objs>;${POCL_DEVICES_OBJS}")

add_compile_options(${OCL_ICD_CFLAGS})

# set llvm C++ flags on pocl_llvm_api.cc
set_source_files_properties("pocl_llvm_api.cc" PROPERTIES COMPILE_FLAGS "${LLVM_CXXFLAGS} -I\"${CMAKE_CURRENT_SOURCE_DIR}/../llvmopencl\"")

# this is so that we don't compile twice when building both libpocl and libOpenCL
add_library("libpocl_unlinked_objs" OBJECT ${POCL_LIB_SOURCES})

#################################################################

set(POCL_PUBLIC_LINK_LIST "-lstdc++" "${LTDL_LIB}" ${POCL_DEVICES_LINK_LIST})

# TODO test static POCL_LLVM_LIBS... probably we should fingure out everything about 
#      environment already in base configuration e.g. if we need to use static libs and 
#      if we need some special flags etc.
set(POCL_PRIVATE_LINK_LIST ${POCL_LLVM_LIBS})
list(APPEND POCL_PRIVATE_LINK_LIST -lclangFrontend -lclangDriver -lclangParse -lclangSema -lclangEdit -lclangLex -lclangSerialization -lclangBasic -lclangFrontendTool -lclangRewriteFrontend -lclangStaticAnalyzerFrontend -lclangStaticAnalyzerCore -lclangAnalysis -lclangCodeGen -lclangAST)

# There is no dynamic llvm libs for windows -> change list to static .lib refs
if (MSVC)
  SET(LLVM_MSVC_STATIC_LIB_LIST "")
  foreach(LIBFLAG ${POCL_PRIVATE_LINK_LIST})
	STRING(REGEX REPLACE "^-l" "" STATIC_LIB_NAME ${LIBFLAG})
	list(APPEND LLVM_MSVC_STATIC_LIB_LIST "${STATIC_LIB_NAME}.lib")
  endforeach()
endif()

#################################################################


add_library("pocl" ${LIBPOCL_OBJS} )
set_target_properties("pocl" PROPERTIES 
	SOVERSION "${LIB_API_VERSION}" 
	VERSION "${LIB_BUILD_VERSION}" 
	LINK_FLAGS "${LLVM_LDFLAGS} ${ICD_LD_FLAGS}")

if (MSVC)
	target_link_libraries("pocl" ${LLVM_MSVC_STATIC_LIB_LIST} ${Hwloc_LIBRARIES} ${Pthreads_LIBRARIES})
else()
	target_link_libraries("pocl" PUBLIC ${POCL_PUBLIC_LINK_LIST} PRIVATE ${POCL_PRIVATE_LINK_LIST} )
endif ()

install(TARGETS "pocl"
        LIBRARY DESTINATION ${POCL_INSTALL_PUBLIC_LIBDIR}
        ARCHIVE DESTINATION ${POCL_INSTALL_PUBLIC_LIBDIR}/static)


if(DIRECT_LINKAGE)
  add_library("OpenCL" ${LIBPOCL_OBJS})
  set_target_properties("OpenCL" PROPERTIES SOVERSION "${LIB_API_VERSION}" VERSION "${LIB_BUILD_VERSION}" LINK_FLAGS "${LLVM_LDFLAGS} ${ICD_LD_FLAGS}")
  if (MSVC)
    target_link_libraries("OpenCL" ${LLVM_MSVC_STATIC_LIB_LIST} ${Hwloc_LIBRARIES} ${Pthreads_LIBRARIES})
  else()
    target_link_libraries("OpenCL" PUBLIC ${POCL_PUBLIC_LINK_LIST} PRIVATE ${POCL_PRIVATE_LINK_LIST} )
  endif ()

  if(NOT ENABLE_ICD)
    install(TARGETS "OpenCL"
          LIBRARY DESTINATION ${POCL_INSTALL_PUBLIC_LIBDIR}
          ARCHIVE DESTINATION ${POCL_INSTALL_PUBLIC_LIBDIR}/static)
  endif()
endif()



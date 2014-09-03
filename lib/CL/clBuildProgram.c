/* OpenCL runtime library: clBuildProgram()

   Copyright (c) 2011-2013 Universidad Rey Juan Carlos,
                           Pekka Jääskeläinen / Tampere Univ. of Technology
   
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

#include "pocl_cl.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <malloc.h>
#ifdef _MSC_VER
    #include <io.h>
	#define R_OK    4
	#define W_OK    2
	#define F_OK    0


	#include <stdlib.h>
    #include <direct.h>
	#define MKDIR(x) mkdir(x)
    #define STRTOK(a,b,c) strtok_s(a,b,c)
    #define SNPRINTF(...) _snprintf(__VA_ARGS__)

	// http://stackoverflow.com/questions/2915672/snprintf-and-visual-studio-2010
	#include <stdarg.h>

	inline int c99_vsnprintf(char* str, size_t size, const char* format, va_list ap)
	{
		int count = -1;

		if (size != 0)
			count = _vsnprintf_s(str, size, _TRUNCATE, format, ap);
		if (count == -1)
			count = _vscprintf(format, ap);

		return count;
	}

	inline int c99_snprintf(char* str, size_t size, const char* format, ...)
	{
		int count;
		va_list ap;

		va_start(ap, format);
		count = c99_vsnprintf(str, size, format, ap);
		va_end(ap);

		return count;
	}

#else
	#include <unistd.h>
    #define MKDIR(x) mkdir(x, S_IRWXU) 
    #define STRTOK(a,b,c) strtok_r(a,b,c)
    #define SNPRINTF(...) snprintf(__VA_ARGS__)
#endif
#include <sys/stat.h>
#include "pocl_llvm.h"

/* supported compiler parameters which should pass to the frontend directly
   by using -Xclang */
static char cl_parameters[] = 
  "-cl-single-precision-constant "
  "-cl-fp32-correctly-rounded-divide-sqrt "
  "-cl-opt-disable "
  "-cl-mad-enable "
  "-cl-unsafe-math-optimizations "
  "-cl-finite-math-only "
  "-cl-fast-relaxed-math "
  "-cl-std=CL1.2 "
  "-cl-std=CL1.1 "
  "-cl-kernel-arg-info "
  "-w "
  "-g ";

static char cl_parameters_not_yet_supported_by_clang[] = 
  "-cl-strict-aliasing "
  "-cl-denorms-are-zero "
  "-cl-no-signed-zeros ";

#define MEM_ASSERT(x, err_jmp) do{ if (x){errcode = CL_OUT_OF_HOST_MEMORY;goto err_jmp;}} while(0)
#define COMMAND_LENGTH 4096

CL_API_ENTRY cl_int CL_API_CALL
POname(clBuildProgram)(cl_program program,
                       cl_uint num_devices,
                       const cl_device_id *device_list,
                       const char *options,
                       void (CL_CALLBACK *pfn_notify) (cl_program program, 
                                                       void *user_data),
                       void *user_data) 
CL_API_SUFFIX__VERSION_1_0
{
  char tmpdir[POCL_FILENAME_LENGTH];
  char device_tmpdir[POCL_FILENAME_LENGTH];
  char source_file_name[POCL_FILENAME_LENGTH], binary_file_name[POCL_FILENAME_LENGTH];
  FILE *source_file, *binary_file;
  size_t n;
  int errcode;
  int i;
  int error;
  unsigned char *binary;
  unsigned real_num_devices;
  const cl_device_id *real_device_list;
  /* The default build script for .cl files. */
  int device_i = 0;
  const char *user_options = "";
  char *temp_options;
  char *modded_options = NULL;
  char *token;
  char *saveptr;

  if (program == NULL)
  {
    errcode = CL_INVALID_PROGRAM;
	goto END_ERROR;
  }

  if (pfn_notify == NULL && user_data != NULL)
  {
    errcode = CL_INVALID_VALUE;
	goto END_ERROR;
  }

  if (program->kernels)
  {
    errcode = CL_INVALID_OPERATION;
	goto END_ERROR;
  }
  
  if (options != NULL)
    {
      modded_options = (char*)calloc (512, 1);
      temp_options = strdup (options);
      token = STRTOK (temp_options, " ", &saveptr);
      while (token != NULL)
        {
          /* check if parameter is supported compiler parameter */
          if (memcmp (token, "-cl", 3) == 0 || memcmp (token, "-w", 2) == 0 
              || memcmp(token, "-g", 2) == 0)
            {
              if (strstr (cl_parameters, token))
                {
                  /* the LLVM API call pushes the parameters directly to the 
                     frontend without using -Xclang */
                }
              else if (strstr (cl_parameters_not_yet_supported_by_clang, token))
                {
                  token = STRTOK (NULL, " ", &saveptr);  
                  continue;
                }
              else
                {
                  errcode = CL_INVALID_BUILD_OPTIONS;
                  goto ERROR_CLEAN_OPTIONS;
                }
            }
          else if (memcmp (token, "-D", 2) == 0 || memcmp (token, "-I", 2) == 0)
            {
              strcat (modded_options, token);
              strcat (modded_options, " ");
              /* if there is a space in between, then next token is part 
                 of the option */
              if (strlen (token) == 2)
                token = STRTOK (NULL, " ", &saveptr);
            }
          else
            {
              errcode = CL_INVALID_BUILD_OPTIONS;
              goto ERROR_CLEAN_OPTIONS;
            }
          strcat (modded_options, token);
          strcat (modded_options, " ");
          token = STRTOK (NULL, " ", &saveptr);  
        }
      free (temp_options);
      
      user_options = modded_options;
      program->compiler_options = strdup (modded_options);
    }
  else
    {
      free(program->compiler_options);
      program->compiler_options = NULL;        
    }  

  if (program->source == NULL && program->binaries == NULL)
  {
    errcode = CL_INVALID_PROGRAM;
	goto END_ERROR;
  }

  if ((num_devices > 0 && device_list == NULL) ||
      (num_devices == 0 && device_list != NULL))
  {
    errcode = CL_INVALID_VALUE;
	goto END_ERROR;
  }
      
  if (num_devices == 0)
    {
      real_num_devices = program->num_devices;
      real_device_list = program->devices;
    } else
    {
      real_num_devices = num_devices;
      real_device_list = device_list;
    }

  if (program->binaries == NULL)
    {
      SNPRINTF (tmpdir, POCL_FILENAME_LENGTH, "%s/", program->temp_dir);
      MKDIR (tmpdir);

      /* FIXME: these might have allocated already. The user might want to
         build the program with different compiler options and calls this
         repeatedly for the same source. In that case there will be a memory
         leak at the moment. */
      if (((program->binary_sizes =
            (size_t *) malloc (sizeof (size_t) * real_num_devices)) == NULL) || 
          ((program->binaries = 
            (unsigned char**) calloc (real_num_devices, sizeof (unsigned char*))) == NULL) ||
          ((program->llvm_irs = 
            (void**) calloc (real_num_devices, sizeof (void*))) == NULL)) 
      {
        errcode = CL_OUT_OF_HOST_MEMORY;
        goto ERROR_CLEAN_BINARIES;
      }

      /* Build the fully linked non-parallel bitcode for all
         devices. */
      for (device_i = 0; device_i < real_num_devices; ++device_i)
        {
          program->binaries[device_i] = NULL;
          cl_device_id device = real_device_list[device_i];
		  SNPRINTF (device_tmpdir, POCL_FILENAME_LENGTH, "%s/%s",
                    program->temp_dir, device->short_name);
          MKDIR (device_tmpdir);

		  SNPRINTF
            (binary_file_name, POCL_FILENAME_LENGTH, "%s/%s", 
             device_tmpdir, POCL_PROGRAM_BC_FILENAME);

          error = pocl_llvm_build_program
              (program, device, device_i, tmpdir,
               binary_file_name, device_tmpdir,
               user_options);     

          if (error != 0)
          {
            errcode = CL_BUILD_PROGRAM_FAILURE;
            goto ERROR_CLEAN_BINARIES;
          }

          /* In case we cached the llvm::Module, we might not have
             dumped the bitcode yet. FIXME: always assume this and
             fix this in the binary query API. */
          if (program->llvm_irs[device->dev_id] == NULL)
            {
              binary_file = fopen(binary_file_name, "r");
              if (binary_file == NULL)
                {
                  errcode = CL_OUT_OF_HOST_MEMORY;
                  goto ERROR_CLEAN_BINARIES;
                }

              fseek(binary_file, 0, SEEK_END);
              
              program->binary_sizes[device_i] = ftell(binary_file);
              fseek(binary_file, 0, SEEK_SET);

              binary = (unsigned char *) malloc(program->binary_sizes[device_i]);
              if (binary == NULL)
                {
                  errcode = CL_OUT_OF_HOST_MEMORY;
                  goto ERROR_CLEAN_BINARIES;
                }

              n = fread(binary, 1, program->binary_sizes[device_i], binary_file);
              if (n < program->binary_sizes[device_i])
                {
                  errcode = CL_OUT_OF_HOST_MEMORY;
                  goto ERROR_CLEAN_BINARIES;
                }
              program->binaries[device_i] = binary;
            }
        }
        
    }
  else
    {
      /* Build from a binary. The "binaries" (LLVM bitcodes) are loaded to
         memory in the clCreateProgramWithBinary(). Dump them to the files. */
      for (device_i = 0; device_i < real_num_devices; ++device_i)
        {
          int count;
		  count = SNPRINTF (device_tmpdir, POCL_FILENAME_LENGTH, "%s/%s",
                    program->temp_dir, real_device_list[device_i]->short_name);
          MEM_ASSERT(count >= POCL_FILENAME_LENGTH, ERROR_CLEAN_PROGRAM);

          error = MKDIR (device_tmpdir);
          MEM_ASSERT(error, ERROR_CLEAN_PROGRAM);

		  count = SNPRINTF
            (binary_file_name, POCL_FILENAME_LENGTH, "%s/%s", 
             device_tmpdir, POCL_PROGRAM_BC_FILENAME);
          MEM_ASSERT(count >= POCL_FILENAME_LENGTH, ERROR_CLEAN_PROGRAM);

          binary_file = fopen(binary_file_name, "w");
          MEM_ASSERT(binary_file == NULL, ERROR_CLEAN_PROGRAM);

          fwrite (program->binaries[device_i], 1, program->binary_sizes[device_i],
                  binary_file);

          fclose (binary_file);
        }      
    }

  return CL_SUCCESS;

  /* Set pointers to NULL during cleanup so that clProgramRelease won't
   * cause a double free. */

ERROR_CLEAN_BINARIES:
  for(i = 0; i < device_i; i++)
  {
    free(program->binaries[i]);
    program->binaries[i] = NULL;
  }
ERROR_CLEAN_PROGRAM:
  free(program->binaries);
  program->binaries = NULL;
  free(program->binary_sizes);
  program->binary_sizes = NULL;
ERROR_CLEAN_OPTIONS:
  free (modded_options);
END_ERROR:
  return errcode;
}
POsym(clBuildProgram)

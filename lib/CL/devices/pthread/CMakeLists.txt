add_library("pocl-devices-pthread" OBJECT pocl-pthread.h pthread.c)
if (MSVC)
  SET_SOURCE_FILES_PROPERTIES( pocl-pthread.h pthread.c PROPERTIES LANGUAGE CXX )
endif (MSVC)

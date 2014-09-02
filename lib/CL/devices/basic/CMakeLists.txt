add_library("pocl-devices-basic" OBJECT basic.h basic.c)
if (MSVC)
  SET_SOURCE_FILES_PROPERTIES(  basic.h basic.c PROPERTIES LANGUAGE CXX )
endif (MSVC)

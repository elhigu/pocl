
add_subdirectory("pthread")
add_subdirectory("basic")
add_subdirectory("topology")
set(POCL_DEVICES_LINK_LIST ${HWLOC_LDFLAGS})
set(POCL_DEVICES_OBJS "$<TARGET_OBJECTS:pocl-devices>"
      "$<TARGET_OBJECTS:pocl-devices-pthread>"
      "$<TARGET_OBJECTS:pocl-devices-basic>"
      "$<TARGET_OBJECTS:pocl-devices-topology>")

if(ENABLE_TCE)
  add_subdirectory("tce")
  list(APPEND POCL_DEVICES_LINK_LIST ${TCE_LIBS})
endif()

if(ENABLE_SPU)
  add_subdirectory("cellspu")
  list(APPEND POCL_DEVICES_LINK_LIST "spe2")
endif()

set(POCL_DEVICES_SOURCES
        devices.h  devices.c
        bufalloc.c  dev_image.h
        common.h common.c
        bufalloc.h  cpuinfo.c cpuinfo.h) 

add_library("pocl-devices" OBJECT ${POCL_DEVICES_SOURCES})

set(POCL_DEVICES_LINK_LIST ${POCL_DEVICES_LINK_LIST} PARENT_SCOPE)
set(POCL_DEVICES_OBJS ${POCL_DEVICES_OBJS} PARENT_SCOPE)

# With visual studio c++ compiler also for .c files
if (MSVC)
  SET_SOURCE_FILES_PROPERTIES( ${POCL_DEVICES_SOURCES} PROPERTIES LANGUAGE CXX )
endif (MSVC)

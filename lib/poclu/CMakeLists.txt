
add_library("poclu" bswap.c misc.c cl_half.c)

set_target_properties("poclu" PROPERTIES SOVERSION "${LIB_API_VERSION}" VERSION "${LIB_BUILD_VERSION}")

#libpoclu_la_LIBADD = @OPENCL_LIBS@
# not sure about -lm
if (MSVC)
  target_link_libraries("poclu" OpenCL)
else()
  target_link_libraries("poclu" "-lm" ${OPENCL_LIBS})
endif()

install(TARGETS "poclu"
        LIBRARY DESTINATION ${POCL_INSTALL_PUBLIC_LIBDIR}
        ARCHIVE DESTINATION ${POCL_INSTALL_PUBLIC_LIBDIR}/static)


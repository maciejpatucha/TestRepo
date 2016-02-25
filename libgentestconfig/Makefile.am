#######################################
# The list of libraries we are building seperated by spaces.
# The 'lib_' indicates that these build products will be installed
# in the $(libdir) directory. For example /usr/lib
lib_LTLIBRARIES = libgentestconfig.la

#######################################
# Build information for each library

# Sources for libgentestconfig
libgentestconfig_la_SOURCES = gen_test_config.c

# Linker options libTestProgram
libgentestconfig_la_LDFLAGS = ${top_srcdir}/libdevicelist/libdevicelist.la \
                              ${top_srcdir}/libdeviceproperties/libdeviceproperties.la

# Compiler options. Here we are adding the include directory
# to be searched for headers included in the source code.
libgentestconfig_la_CPPFLAGS = -I${top_srcdir}/libgentestconfig \
                               -I${top_srcdir}/libdevicelist \
                               -I${top_srcdir}/libdeviceproperties


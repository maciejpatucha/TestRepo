#######################################
# The list of libraries we are building seperated by spaces.
# The 'lib_' indicates that these build products will be installed
# in the $(libdir) directory. For example /usr/lib
lib_LTLIBRARIES = libdiscoverios.la

#######################################
# Build information for each library

# Sources for libdiscoverios
libdiscoverios_la_SOURCES = discover_ios.c

# Linker options libTestProgram
libdiscoverios_la_LDFLAGS = ../libdeviceproperties/libdeviceproperties.la \
                            ../libdevicelist/libdevicelist.la

# Compiler options. Here we are adding the include directory
# to be searched for headers included in the source code.
libdiscoverios_la_CPPFLAGS = -I${top_srcdir}/libdiscoverios \
                             -I${top_srcdir}/libdeviceproperties \
                             -I${top_srcdir}/libdevicelist


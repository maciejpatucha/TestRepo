#######################################
# The list of libraries we are building seperated by spaces.
# The 'lib_' indicates that these build products will be installed
# in the $(libdir) directory. For example /usr/lib
lib_LTLIBRARIES = libdiscoverandroid.la

#######################################
# Build information for each library

# Sources for libdiscoverandroid
libdiscoverandroid_la_SOURCES = discover_android.c

# Linker options libTestProgram
libdiscoverandroid_la_LDFLAGS = ../libdeviceproperties/libdeviceproperties.la \
                                ../libdevicelist/libdevicelist.la

# Compiler options. Here we are adding the include directory
# to be searched for headers included in the source code.
libdiscoverandroid_la_CPPFLAGS = -I$(top_srcdir)/libdiscoverandroid \
                                 -I${top_srcdir}/libdeviceproperties \
                                 -I${top_srcdir}/libdevicelist


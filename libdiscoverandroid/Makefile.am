#######################################
# The list of libraries we are building seperated by spaces.
# The 'lib_' indicates that these build products will be installed
# in the $(libdir) directory. For example /usr/lib
lib_LTLIBRARIES = libdiscoverandroid.la

#######################################
# Build information for each library

# Sources for libdiscoverandroid
libdiscoverandroid_la_SOURCES = discover_android.c discover_android_threads.c

# Linker options libTestProgram
libdiscoverandroid_la_LDFLAGS = -lpthread \
                                ${top_srcdir}/libdeviceproperties/libdeviceproperties.la \
                                ${top_srddir}/libdevicelist/libdevicelist.la

# Compiler options. Here we are adding the include directory
# to be searched for headers included in the source code.
libdiscoverandroid_la_CPPFLAGS = -I${top_srcdir}/libdeviceproperties \
                                 -I${top_srcdir}/libdevicelist


#######################################
# The list of executables we are building seperated by spaces
# the 'bin_' indicates that these build products will be installed
# in the $(bindir) directory. For example /usr/bin
bin_PROGRAMS=testApp

# Because a.out is only a sample program we don't want it to be installed.
# The 'noinst_' prefix indicates that the following targets are not to be
# installed.
#noinst_PROGRAMS=testApp

#######################################
# Build information for each executable. The variable name is derived
# by use the name of the executable with each non alpha-numeric character is
# replaced by '_'. So a.out becomes a_out and the appropriate suffex added.
# '_SOURCES' for example.

# Sources for the a.out 
testApp_SOURCES= main.c

# Linker options for a.out
testApp_LDFLAGS = $(top_srcdir)/libdevicelist/libdevicelist.la \
                  $(top_srcdir)/libdeviceproperties/libdeviceproperties.la \
                  $(top_srcdir)/libdiscoverandroid/libdiscoverandroid.la \
                  $(top_srcdir)/libdiscoverios/libdiscoverios.la

# Compiler options for a.out
testApp_CPPFLAGS = -I$(top_srcdir)/libdevicelist \
                   -I$(top_srcdir)/libdeviceproperties \
                   -I$(top_srcdir)/libdiscoverandroid \
                   -I$(top_srcdir)/libdiscoverios

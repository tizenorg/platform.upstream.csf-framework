#
#  Copyright (c) 2014, McAfee, Inc.
#  
#  All rights reserved.
#  
#  Redistribution and use in source and binary forms, with or without modification,
#  are permitted provided that the following conditions are met:
#  
#  Redistributions of source code must retain the above copyright notice, this list
#  of conditions and the following disclaimer.
#  
#  Redistributions in binary form must reproduce the above copyright notice, this
#  list of conditions and the following disclaimer in the documentation and/or other
#  materials provided with the distribution.
#  
#  Neither the name of McAfee, Inc. nor the names of its contributors may be used
#  to endorse or promote products derived from this software without specific prior
#  written permission.
#  
#  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
#  ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
#  WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
#  IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
#  INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
#  BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
#  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
#  LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
#  OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
#  OF THE POSSIBILITY OF SUCH DAMAGE.
#

OUTDIR=bin
TARGET=$(OUTDIR)/tpcsserdaemontest
SRCDIR=.
LD_FLAGS := $(LD_FLAGS) -ldl

ifeq ($(TCS_CC), )
	CC = gcc
else
	CC = $(TCS_CC)
endif
ifeq ($(TCS_LD), )
	LD = ld
else
	LD = $(TCS_LD)
endif

# Define a list of pkg-config packages we want to use
pkg_packages = dbus-glib-1 dlog

PKG_CFLAGS  = $(shell pkg-config --cflags $(pkg_packages))
PKG_LDFLAGS = $(shell pkg-config --libs $(pkg_packages))

TSC_ROOT_PATH=../../../framework

# Combine user supplied, additional, and pkg-config flags
LD_FLAGS += $(PKG_LDFLAGS) -lscclient -lscserver -L$(TSC_ROOT_PATH)/lib -lxml2

GBS_CFLAGS = -I${SYSROOT}/usr/include/dbus-1.0 -I${SYSROOT}/usr/lib/dbus-1.0/include -I${SYSROOT}/usr/include/libxml2

ifeq ($(TCS_CFG), release)
	CFLAGS := $(CFLAGS) -fPIE -g -Wall -Werror -O3 -I$(SRCDIR) -I$(TSC_ROOT_PATH) $(PKCL_CFLAGS) $(GBS_CFLAGS) $(PKG_CFLAGS) -I../
else
	CFLAGS := $(CFLAGS) -fPIE -g -Wall -Werror -O3 -DDEBUG -I$(SRCDIR) -I$(TSC_ROOT_PATH) $(PKCL_CFLAGS) $(GBS_CFLAGS) $(PKG_CFLAGS) -I../
endif

SOURCES=$(SRCDIR)/TPCSSerDaemonTestUtils.c \
		$(SRCDIR)/TPCSSerDaemonTest.c

OBJECTS=$(OUTDIR)/TPCSSerDaemonTestUtils.o \
		$(OUTDIR)/TPCSSerDaemonTest.o

MKDEP = mkdep -f .depend

$(OUTDIR)/%.o: $(SRCDIR)/%.c
	$(CC) $(CFLAGS) -o $(OUTDIR)/$*.o -c $(SRCDIR)/$*.c

all: $(OUTDIR) .depend $(TARGET)

.depend: $(SOURCES)
	$(MKDEP) $(CFLAGS) $(SOURCES)

$(TARGET): $(OUTDIR) $(OBJECTS) $(SOURCES)
	$(LD) -Wl,-zdefs -o $(TARGET) $(OBJECTS) $(LD_FLAGS)

$(OUTDIR):
	@mkdir $(OUTDIR)

install: all
	mkdir -p ${DESTDIR}/bin
	cp ${TARGET} ${DESTDIR}/bin/

distclean: clean
	@rm -f .depend
#	@rm -rf $(OUTDIR)

clean:
	@rm -f $(TARGET)
	@rm -f $(OBJECTS) *~
	@rm -f *.bb *.bbg *.da *.gcov

-include .depend


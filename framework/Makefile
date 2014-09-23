#
#  Copyright (c) 2013, McAfee, Inc.
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

OUTDIR = lib
TARGET = $(OUTDIR)/libsecfw.so
SRCDIR = .
INCLUDE = -I. $(TCS_INC) -I../plugin
LD_FLAGS := $(LD_FLAGS) -ldl -lpthread $(shell pkg-config --libs libtzplatform-config)

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
ifeq ($(TCS_AR), )
	AR = ar
else
	AR = $(TCS_AR)
endif
ifeq ($(TCS_STRIP), )
	STRIP = strip
else
	STRIP = $(TCS_STRIP)
endif

ifeq ($(TCS_CFG), release)
	CFLAGS := -O3 -fPIC $(INCLUDE) -DUNIX $(CFLAGS)
else
	CFLAGS := -g -fPIC $(INCLUDE) -DUNIX -DDEBUG $(CFLAGS)
endif

CFLAGS := $(CFLAGS) $(PKCL_CFLAGS) $(TCS_CFLAGS) -Wall -Werror

# Define a list of pkg-config packages we want to use
pkg_packages = dlog

PKG_CFLAGS  = $(shell pkg-config --cflags $(pkg_packages))
PKG_LDFLAGS = $(shell pkg-config --libs $(pkg_packages))

# Combine user supplied, additional, and pkg-config flags
ifeq ($(TCS_CFG), release)
	PKGLD_FLAGS = 
	PKGCFLAGS =
else
	PKGLD_FLAGS += $(PKG_LDFLAGS) -L./lib
	PKGCFLAGS += -Wall -I$(SRCDIR) $(PKCL_CFLAGS) $(PKG_CFLAGS)
endif

SOURCES = $(SRCDIR)/TCSImpl.c $(SRCDIR)/TWPImpl.c

OBJECTS = $(OUTDIR)/TCSImpl.o $(OUTDIR)/TWPImpl.o

$(OUTDIR)/%.o: $(SRCDIR)/%.c
	$(CC) $(CFLAGS) $(PKGCFLAGS) -I. -o $(OUTDIR)/$*.o -c $(SRCDIR)/$*.c

all: $(OUTDIR) $(TARGET)

$(TARGET): $(OBJECTS)
<<<<<<< HEAD
	$(LD) -shared -Wl,-zdefs -o $(TARGET) $(OBJECTS) $(LD_FLAGS) $(PKGLD_FLAGS)

#	$(AR) -cr $(TARGET) $(OBJECTS)

	$(STRIP) $(TARGET)
=======
	$(CC) -shared -Wl,-zdefs -o $(TARGET) $(OBJECTS) $(LD_FLAGS)
>>>>>>> Fixed the HTTP test code for TWP testing per Samsung report

$(OUTDIR):
	@mkdir $(OUTDIR)

distclean: clean
	@rm -rf $(OUTDIR)

clean:
	@rm -f $(TARGET)
	@rm -f $(OBJECTS) *~
	@rm -f *.bb *.bbg *.da *.gcov

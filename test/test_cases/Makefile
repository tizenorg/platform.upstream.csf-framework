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

OUTDIR=bin
SRCDIR=.

ifeq ($(TCS_CC), )
	CC = gcc
else
	CC = $(TCS_CC)
endif
ifeq ($(TCS_LD), )
	LD = gcc
else
	LD = $(TCS_LD)
endif

TCS_HEADER_FILE_PATH=../../framework

CFLAGS := $(CFLAGS) -fPIC -g -Wall -O3 -I$(SRCDIR)/port -I$(SRCDIR)/tests -I$(TCS_HEADER_FILE_PATH)
ifeq ($(LD_FLAGS), )
LDFLAGS= $(LD_FLAGS) -lc -pthread -L../../framework/lib -lsecfw -ldl
else
LDFLAGS= $(LD_FLAGS) -lc -L../../framework/lib -lsecfw -ldl
endif

TARGET=$(OUTDIR)/tcstest

SOURCES=$(SRCDIR)/TCSTest.c \
		$(SRCDIR)/TCSTestUtils.c \
		$(SRCDIR)/SampleInfo.c
OBJECTS=$(OUTDIR)/TCSTest.o \
		$(OUTDIR)/TCSTestUtils.o \
		$(OUTDIR)/SampleInfo.o

$(OUTDIR)/%.o: $(SRCDIR)/%.c
	$(CC) $(CFLAGS) -o $(OUTDIR)/$*.o -c $(SRCDIR)/$*.c

$(TARGET): $(OUTDIR) $(OBJECTS) $(SOURCES)
	$(LD) -o $(TARGET) $(OBJECTS) $(LDFLAGS)

all: $(TARGET)

$(OUTDIR):
	@mkdir $(OUTDIR)

clean:
	@rm -rf $(OUTDIR)

distclean: clean


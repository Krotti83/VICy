#
# VICy - Open Cognitive Assist System
#
# Copyright (c) 2026 Johannes Krottmayer <krotti83@proton.me>
#
# Permission to use, copy, modify, and/or distribute this software for any
# purpose with or without fee is hereby granted, provided that the above
# copyright notice and this permission notice appear in all copies.
#
# THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
# WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
# MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
# ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
# WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
# ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
# OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
#

# Target
TARGET			:= vicy
TARGET_MAP		:= $(TARGET).map

# Toolchain
CXX			:= g++
CXXLD			:= g++

# Generic shell commands
RM			:= rm -rf
CP			:= cp -Rf

# C++ compiler flags
CXXFLAGS		+= -g
CXXFLAGS		+= -Wall
CXXFLAGS		+= -Wextra
CXXFLAGS		+= -pedantic
CXXFLAGS		+= -std=c++20
CXXFLAGS		+= -ffunction-sections
CXXFLAGS		+= -fdata-sections

# GNU ld flags
LDFLAGS			+= -Wl,--build-id=none
LDFLAGS			+= -Wl,--gc-sections
LDFLAGS			+= -Wl,-Map $(TARGET_MAP)
LDFLAGS			+= -Wl,--print-map-locals

include ./files.mk

VICY_CXXOBJ		+= $(VICY_CXXSRC:.cpp=.o)

#
# All
#

all: $(TARGET)

#
# VICy
#

# Link object files
$(TARGET): $(VICY_CXXOBJ)
	@echo "   [CXXLD] $(TARGET)"
	@$(CXXLD) $(CXXFLAGS) $(LDFLAGS) -o $(TARGET) $(VICY_CXXOBJ)

# Compile C++ sources
$(VICY_CXXOBJ): %.o: %.cpp
	@echo "   [CXX]   $@"
	@$(CXX) -c $(CXXFLAGS) -o $@ $<

#
# Clean
#

.PHONY: clean
clean:
	@echo "   [CLEAN]"
	@$(RM) $(VICY_CXXOBJ)
	@$(RM) $(TARGET)

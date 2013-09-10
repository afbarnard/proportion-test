# Copyright (c) 2010 Aubrey Barnard.  This is free software.  See
# LICENSE for details.

.PHONY: default clean test

# Set important locations
yapSrcIncludeDir := /usr/include/Yap  # e.g. $(HOME)/opt/yap/include
checkSrcIncludeDir :=  # e.g. $(HOME)/opt/check/src
checkLibIncludeDir :=  # e.g. $(HOME)/opt/check/lib

# Set up includes for sources and libraries
srcIncludes := $(patsubst %,-I%,$(yapSrcIncludeDir) $(checkSrcIncludeDir))
libIncludes := $(patsubst %,-L%,$(checkLibIncludeDir))


default: proportionTest.so

proportionTest.o: proportionTest.c
	gcc -Wall $(srcIncludes) -std=c99 -shared -fPIC -c proportionTest.c

proportionTest.so: proportionTest.o
	gcc -Wall -lm -shared -o proportionTest.so proportionTest.o

proportionTestTest.o: proportionTestTest.c proportionTest.c
	gcc -Wall $(srcIncludes) -std=c99 -c proportionTestTest.c

proportionTestTest: proportionTestTest.o
	gcc -Wall $(libIncludes) -lm -lcheck -o proportionTestTest proportionTestTest.o

test: proportionTestTest
	@echo
	@LD_LIBRARY_PATH=~/opt/check/lib:$$LD_LIBRARY_PATH ./proportionTestTest

clean:
	@rm -f *~ *.so *.o proportionTestTest

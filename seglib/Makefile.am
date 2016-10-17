include $(top_srcdir)/build/Makefile.mk

bin_PROGRAMS=lsplit csplit segment build fword fword2 fword3

lsplit_SOURCES=lsplit.cpp

csplit_SOURCES=csplit.cpp

segment_SOURCES=segment.cpp
segment_LDADD=../DRWordSeg/libHCSegDll.a

build_SOURCES=build.cpp
build_LDADD=../DRWordSeg/libHCSegDll.a

fword_SOURCES=fword.cpp

fword2_SOURCES=fword2.cpp
fword2_CPPFLAGS=${AM_CPPFLAGE} -std=c++11

fword3_SOURCES=fword3.cpp
fword3_LDADD=../hcac/libhcac.a

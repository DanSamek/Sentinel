EXE ?= Sentinel

CXX = g++
CXXSTD = -std=c++20
CXXFLAGS = -Wall -pedantic $(CXXSTD)
CXXFLAGS_DEBUG = -g -fsanitize=address,undefined
CXXFLAGS_RELEASE = -O3

INCLUDES = -Isrc -Isrc/tests

SRCS = src/main.cpp \
       src/movegen.cpp \
       src/magics.cpp \
       src/board.cpp \
       src/move.cpp \
       src/uci.cpp \
       src/search.cpp \
       src/movepick.cpp \
       src/tt.cpp \
       src/timer.cpp \
       src/evaluation.cpp \
       src/see.cpp \
       src/timemanager.cpp \
       src/nnue/datagen.cpp \
       src/nnue/accumulator.cpp \
       src/nnue/nnue.cpp \
       src/tunable.cpp

OBJS = $(SRCS:.cpp=.o)

ifdef EXE
BUILD_TYPE = Release
ENABLE_AVX = 1
else
BUILD_TYPE ?= Debug
endif

ifeq ($(BUILD_TYPE),Release)
    CXXFLAGS += $(CXXFLAGS_RELEASE)
    LDFLAGS = -static -static-libgcc -static-libstdc++ -lpthread
else
    CXXFLAGS += $(CXXFLAGS_DEBUG)
endif

AVX_CHECK := $(shell echo | $(CXX) -mavx -E - > /dev/null 2>&1 && echo 1 || echo 0)
AVX2_CHECK := $(shell echo | $(CXX) -mavx2 -E - > /dev/null 2>&1 && echo 1 || echo 0)
AVX512_CHECK := $(shell echo | $(CXX) -mavx512f -E - > /dev/null 2>&1 && echo 1 || echo 0)
SSE_CHECK := $(shell echo | $(CXX) -msse -E - > /dev/null 2>&1 && echo 1 || echo 0)

ifeq ($(ENABLE_AVX),1)
    ifeq ($(AVX_CHECK),1)
        CXXFLAGS += -mavx -DENABLE_AVX
    endif
    ifeq ($(AVX2_CHECK),1)
        CXXFLAGS += -mavx2
    endif
endif

ifeq ($(ENABLE_AVX_512),1)
    ifeq ($(AVX512_CHECK),1)
        CXXFLAGS += -mavx512f -DENABLE_AVX_512
    endif
endif

ifeq ($(ENABLE_SSE),1)
    ifeq ($(SSE_CHECK),1)
        CXXFLAGS += -msse -msse4.1 -DENABLE_SSE
    endif
endif

all: $(EXE)

$(EXE): $(OBJS)
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -o $(EXE) $(OBJS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

clean:
	rm -f $(OBJS) $(EXE)

debug:
	$(MAKE) BUILD_TYPE=Debug

release:
	$(MAKE) BUILD_TYPE=Release

avx:
	$(MAKE) ENABLE_AVX=1

avx512:
	$(MAKE) ENABLE_AVX_512=1

sse:
	$(MAKE) ENABLE_SSE=1

.PHONY: all clean debug release avx avx512 sse

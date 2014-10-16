CPP := g++
CPP_OSX := clang++
TARGET := demoinfogo
OBJECTS := src/demofile.cpp src/demofiledump.cpp src/demoinfogo.cpp src/demofilebitbuf.cpp src/demofilepropdecode.cpp
PROTO_OBJECTS := src/generated_proto/netmessages_public.pb.cc src/generated_proto/cstrike15_usermessages_public.pb.cc

PROTOBUF_DIR := protobuf-2.5.0
PROTOBUF_SRC := $(PROTOBUF_DIR)/src
PROTOBUF_LIB := $(PROTOBUF_DIR)/src/.libs

INCLUDE := -I$(PROTOBUF_SRC)
OS := $(shell uname -s)
ARCH := $(shell getconf LONG_BIT)
CPP_FLAGS := -m32 -pthread -Wall -fpermissive --std=c++0x

ifneq "$(OS)" "Darwin"
	LD_FLAGS := -static
else
	CPP = $(CPP_OSX)
endif

LD_FLAGS += -L$(PROTOBUF_LIB) -lprotobuf

default: all

get_protobuf:
	curl https://protobuf.googlecode.com/files/protobuf-2.5.0.tar.gz | tar xvz
	cd protobuf-2.5.0 ; ./configure --build=i686-pc-linux-gnu CFLAGS="-m32 -DNDEBUG" CXXFLAGS="-m32 -DNDEBUG" LDFLAGS=-m32 ; make

generated_proto:
	mkdir -p src/generated_proto
	$(PROTOBUF_SRC)/protoc --proto_path=./src --proto_path=$(PROTOBUF_SRC) --cpp_out=./src/generated_proto ./src/netmessages_public.proto
	$(PROTOBUF_SRC)/protoc --proto_path=./src --proto_path=$(PROTOBUF_SRC) --cpp_out=./src/generated_proto ./src/cstrike15_usermessages_public.proto

all: generated_proto
	$(CPP) $(INCLUDE) $(CPP_FLAGS) $(OBJECTS) $(PROTO_OBJECTS) $(LD_FLAGS) -o $(TARGET)

clean:
	rm -rf src/generated_proto

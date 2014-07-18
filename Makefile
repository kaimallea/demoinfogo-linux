CPP := g++
CPP_OSX := clang++
TARGET := demoinfogo
OBJECTS := src/demofile.cpp src/demofiledump.cpp src/demoinfogo.cpp
PROTO_OBJECTS := src/generated_proto/netmessages_public.pb.cc src/generated_proto/cstrike15_usermessages_public.pb.cc

PROTOBUF_DIR := protobuf-2.5.0
PROTOBUF_SRC := $(PROTOBUF_DIR)/src
PROTOBUF_LIB := $(PROTOBUF_DIR)/src/.libs

JSON_LIB := rapidjson/include

INCLUDE := -I$(JSON_LIB) -I$(PROTOBUF_SRC)
OS := $(shell uname -s)
ARCH := $(shell getconf LONG_BIT)
CPP_FLAGS_32 := -m32
CPP_FLAGS_64 := -m64 -fpermissive
CPP_FLAGS := $(CPP_FLAGS_$(ARCH)) -pthread -Wall

ifneq "$(OS)" "Darwin"
	LD_FLAGS := -static
else
	CPP = $(CPP_OSX)
endif

LD_FLAGS += -L$(PROTOBUF_LIB) -lprotobuf

default: all

get_protobuf:
	curl https://protobuf.googlecode.com/files/protobuf-2.5.0.tar.gz | tar xvz
	cd protobuf-2.5.0 ; ./configure ; make

generated_proto:
	mkdir -p src/generated_proto
	$(PROTOBUF_SRC)/protoc --proto_path=./src --proto_path=$(PROTOBUF_SRC) --cpp_out=./src/generated_proto ./src/netmessages_public.proto
	$(PROTOBUF_SRC)/protoc --proto_path=./src --proto_path=$(PROTOBUF_SRC) --cpp_out=./src/generated_proto ./src/cstrike15_usermessages_public.proto

all: generated_proto
	$(CPP) $(INCLUDE) $(CPP_FLAGS) $(OBJECTS) $(PROTO_OBJECTS) $(LD_FLAGS) -o $(TARGET)

clean:
	rm -rf src/generated_proto
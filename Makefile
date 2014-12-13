CC=g++
CC_OSX=clang
PROTOBUF_DIR=protobuf-2.5.0
PROTOBUF_SRC=$(PROTOBUF_DIR)/src
PROTOBUF_LIB=$(PROTOBUF_DIR)/src/.libs/libprotobuf.a
INC=-I$(PROTOBUF_SRC)
CFLAGS=-m32 -O3 -Wall -fpermissive --std=c++0x
LDFLAGS=-m32 -pthread

OS=$(shell uname -s)
ifneq "$(OS)" "Darwin"
	LDFLAGS += -static
else
	CC=$(CC_OSX)
	LDFLAGS += -lc++
endif

default: demoinfogo

demoinfogo: src/demofile.o src/demofiledump.o src/demoinfogo.o src/demofilebitbuf.o src/demofilepropdecode.o src/generated_proto/netmessages_public.pb.cc src/generated_proto/cstrike15_usermessages_public.pb.cc
	$(CC) $(INC) $^ $(PROTOBUF_LIB) $(LDFLAGS) -o $@

src/demofile.o: src/demofile.cpp
	$(CC) $(CFLAGS) $(INC) -c $< -o $@

src/demofiledump.o: src/demofiledump.cpp src/generated_proto/netmessages_public.pb.cc src/generated_proto/cstrike15_usermessages_public.pb.cc
	$(CC) $(CFLAGS) $(INC) -c $< -o $@

src/demoinfogo.o: src/demoinfogo.cpp
	$(CC) $(CFLAGS) $(INC) -c $< -o $@

src/demofilebitbuf.o: src/demofilebitbuf.cpp
	$(CC) $(CFLAGS) $(INC) -c $< -o $@

src/demofilepropdecode.o: src/demofilepropdecode.cpp
	$(CC) $(CFLAGS) $(INC) -c $< -o $@

src/generated_proto/netmessages_public.pb.cc: src/netmessages_public.proto
	mkdir -p src/generated_proto
	$(PROTOBUF_SRC)/protoc --proto_path=./src --proto_path=$(PROTOBUF_SRC) --cpp_out=./src/generated_proto $<

src/generated_proto/cstrike15_usermessages_public.pb.cc: src/cstrike15_usermessages_public.proto
	mkdir -p src/generated_proto
	$(PROTOBUF_SRC)/protoc --proto_path=./src --proto_path=$(PROTOBUF_SRC) --cpp_out=./src/generated_proto $<

.PHONY: clean generated_proto protobuf

clean:
	rm -rf src/*.o src/generated_proto/*.pb.* demoinfogo

get_protobuf:
	curl https://protobuf.googlecode.com/files/protobuf-2.5.0.tar.gz | tar xvz

protobuf:
	cd protobuf-2.5.0 ; ./configure CFLAGS="-O3 -m32 -DNDEBUG" CXXFLAGS="-m32 -DNDEBUG" LDFLAGS=-m32 ; make

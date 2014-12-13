[![Build Status](https://api.travis-ci.org/csgo-data/demoinfogo-linux.svg?branch=travis)](https://travis-ci.org/csgo-data/demoinfogo-linux)

CS:GO Demos and Network Messages
--------------------------------

Demos and network messages in CS:GO use Google's Protocol Buffers (protobuf). Protobuf is a message/object serialization language that generates code to serialize the objects efficiently. For information about protobuf, see https://developers.google.com/protocol-buffers/docs/overview

`demoinfogo` is a tool that parses CS:GO demo files (ending in .dem) and dumps out every message in the demo. Using this tool, third parties can parse the demo for various game events to generate information and statistics.

The repo contains the source code required to build this tool on linux. The source includes the proto files containing network message definitions.


Building demoinfogo
------------------

Prequisites:

- On Debian/Ubuntu, install the essential build tools: `sudo apt-get install build-essential`
- Additionally, if your Debian/Ubuntu OS is 64-bit, you'll also need g++ multilib: `sudo apt-get install g++-multilib`
- On OSX, install the Command Line Tools: `xcode-select install`

In order to build demoinfogo, follow these steps:

1. Run `make get_protobuf` to download [protobuf 2.5.0](https://protobuf.googlecode.com/files/protobuf-2.5.0.tar.gz) and extract it to the root of this repo.
2. Run `make protobuf` to configure and compile it.
3. Run `make` to compile and create `demoinfogo`

Both protobuf and demoinfogo are cross-compiled for 32-bit; otherwise it will not work.

Working with Network Messages
-----------------------------

Building demoinfogo generates C++ classes from the network protobuf files. Follow these steps to generate these files for use in your application without building demoinfogo:

1. Follow the instructions above for downloading and compiling protobuf 2.5.0 from source.
2. Run `make generated_proto`. The generated C++ files are created in subdirectory named `src/generated_proto`.

You can now use the generated classes in your C++ code to send user messages. Here is an example of how to send the HintText message

```cpp
CSingleUserRecipientFilter filter( this );
filter.MakeReliable();
CCSUsrMsg_HintText msg;
msg.set_text( "ExampleHint" );
SendUserMessage( filter, CS_UM_HintText, msg );
```

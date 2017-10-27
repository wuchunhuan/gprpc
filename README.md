# gprpc
A fast and light wight RPC framework based on Google protobuf.

## Overview
gprpc is a RPC library which utilized Boost asio to achive high performence network communicating, and implemented Google's protobuf service framework. with the cross-platform Protobuf data exchange protocol, gprpc, as of now only support c++ on both client and server sides, could be potencially extented to support more upcoming programing language. gprpc has been under active development.

## Highlight
- Simple and easy to use
- Ligth wight and fast
- High performence

## Features
- Synchronous and asynchronous call
- Precise time out control on rpc calls
- Verbose debugging trace log

## TODO
- Server side connection management
- Flow control on both client and server side
- RPC data en/decryption
- RPC data compression
- Multiple language support

## Documentation
- [gprpc detail](docs/gprpc_detail.md)
- Example

## Building

### Dependencies
Only headers of libraries listed below are needed to build gprpc library:
- [Boost](http://www.boost.org/)
- [protobuf](https://developers.google.com/protocol-buffers)

### Build and install
gprpc library will be installed into `/usr/local` by default, modify the line `set(CMAKE_INSTALL_PREFIX your/prefered/directory)` in CMakelists.txt if you would like to change it.
```
git clone https://github.com/heroperseus/gprpc.git
cd gprpc
mkdir build && cmake.. && make
sudo make install
```

## Support
wuchunhuan@gmail.com

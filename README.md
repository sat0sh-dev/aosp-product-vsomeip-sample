# vsomeip Sample Applications

Phase 4: SOME/IP integration

## Overview

Sample applications demonstrating vsomeip usage in AOSP.

## Applications

| Binary | Description |
|--------|-------------|
| vsomeip_request_sample | Client (sends requests) |
| vsomeip_response_sample | Server (sends responses) |

## Directory Structure

```
product/vsomeip_sample/
├── Android.bp
├── src/
│   ├── request_sample.cpp
│   ├── response_sample.cpp
│   └── sample_ids.hpp
├── config/
│   └── vsomeip.json
└── sepolicy/
    └── vsomeip_sample.te
```

## Usage

### Server (Response Sample)

```bash
# Terminal 1: Start server
adb shell vsomeip_response_sample
```

### Client (Request Sample)

```bash
# Terminal 2: Send request
adb shell vsomeip_request_sample
```

## Configuration

Configuration file: `/product/etc/vsomeip/vsomeip.json`

### Service IDs

```cpp
// sample_ids.hpp
#define SAMPLE_SERVICE_ID   0x1234
#define SAMPLE_INSTANCE_ID  0x5678
#define SAMPLE_METHOD_ID    0x0421
```

## Build

```bash
# In AOSP build environment
m vsomeip_request_sample vsomeip_response_sample
```

## References

- [vsomeip Examples](https://github.com/COVESA/vsomeip/tree/master/examples)

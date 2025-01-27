# Cuckatoo Reference Miner

### Description
Cuckatoo miner that supports cuckatoo10 to cuckatoo32 and implements lean and mean edge trimming on the GPU using OpenCL and Metal.

### Building
It's recommended that you build this program on the same system that you'll be running it on since it uses the `-march=native` and `-mtune=native` compiler flags to optimize itself for the current system's available features. Because of this, I don't release prebuilt versions of this program.

#### Linux
This program can be built and ran with Linux with the following commands where `EDGE_BITS` is set to the cuckatoo variation that you want to use (e.g. `EDGE_BITS=30` for cuckatoo30):
```
sudo apt install make g++ opencl-headers ocl-icd-opencl-dev
make EDGE_BITS=30
make run
```

#### macOS
After installing [Xcode](https://developer.apple.com/xcode), this program can be built and ran with macOS with the following commands where `EDGE_BITS` is set to the cuckatoo variation that you want to use (e.g. `EDGE_BITS=30` for cuckatoo30):
```
make EDGE_BITS=30
make run
```

#### Windows
After installing [MinGW-w64](https://winlibs.com), this program can be built and ran with Windows with the following commands where `EDGE_BITS` is set to the cuckatoo variation that you want to use (e.g. `EDGE_BITS=30` for cuckatoo30):
```
mingw32-make windowsDependencies
mingw32-make EDGE_BITS=30
mingw32-make run
```

#### Cross-compiling for Windows
This program can be built with Linux for Windows with the following commands where `EDGE_BITS` is set to the cuckatoo variation that you want to use (e.g. `EDGE_BITS=30` for cuckatoo30):
```
sudo apt install make g++-mingw-w64 cmake
make crossCompilingDependencies CC=x86_64-w64-mingw32-g++-posix
make CC=x86_64-w64-mingw32-g++-posix EDGE_BITS=30
```

### Tuning
A `TRIMMING_ROUNDS` setting can be provided when building this program that can be adjusted to make the `Searching` time displayed when running this program as close to, but not greater than, the `Trimming` time displayed to maximize this program's performance (without regard for power usage). Increasing the `TRIMMING_ROUNDS` setting decreases the `Searching` time and increases the `Trimming` time, and decreasing the `TRIMMING_ROUNDS` setting increases the `Searching` time and decreases the `Trimming` time while also increasing CPU RAM usage.
```
make EDGE_BITS=30 TRIMMING_ROUNDS=25
```

Also be sure to set your operating system's power mode to best performance when running this program.

Increasing your operating system's page size may lead to better performance when using this program.

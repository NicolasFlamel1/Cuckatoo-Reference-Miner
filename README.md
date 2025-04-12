# Cuckatoo Reference Miner

### Description
Cuckatoo miner that supports cuckatoo10 to cuckatoo32 and implements mean, slean, and lean edge trimming on the GPU using OpenCL and Metal.

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

### Usage
A stratum server address, port, and username can be provided when running this program to set the stratum server that it will mine to. For example, the following command will connect to the stratum server with the address `127.0.0.1` at port `3416` using the username `username`. Once the software connects to a stratum server, it will start mining using mean, slean, or lean edge trimming depending on your available hardware and display the `Searching` and `Trimming` times for each graph that it processes. Those two pipeline stages run in parallel with the GPU being responsible for trimming and the CPU being responsible for searching, and you can determine your mining rate in graphs/second by dividing 1 by the greater of the two times.
```
"./Cuckatoo Reference Miner" --stratum_server_address 127.0.0.1 --stratum_server_port 3416 --stratum_server_username username
```

### Tuning
A `TRIMMING_ROUNDS` setting can be provided when building this program that can be adjusted to make the `Searching` time displayed when running this program as close to, but not greater than, the `Trimming` time displayed to maximize this program's performance (without regard for power usage). Increasing the `TRIMMING_ROUNDS` setting decreases the `Searching` time and increases the `Trimming` time, and decreasing the `TRIMMING_ROUNDS` setting increases the `Searching` time and decreases the `Trimming` time while also increasing CPU RAM usage.
```
make EDGE_BITS=30 TRIMMING_ROUNDS=25
```
You can provide a `SLEAN_TRIMMING_PARTS` setting when building this program to adjust the number of parts used when slean trimming. This value must be a power of two, and decreasing it will increase slean trimming's GPU RAM requirement while increasing it will decrease slean trimming's GPU RAM requirement.
```
make EDGE_BITS=30 SLEAN_TRIMMING_PARTS=4
```
You can provide a `LOCAL_RAM_KILOBYTES` setting when building this program to adjust the targeted GPU local RAM size in kilobytes used when mean or slean trimming. This value must be a power of two, and it should match your GPU's local RAM size for potentially better performance.
```
make EDGE_BITS=30 LOCAL_RAM_KILOBYTES=64
```
A `TUNING=1` setting can be provided when building this program which will make it skip connecting to a stratum server. This is only useful for tuning the software since it can't mine blocks without being connected to a stratum server.
```
make EDGE_BITS=30 TRIMMING_ROUNDS=25 SLEAN_TRIMMING_PARTS=4 LOCAL_RAM_KILOBYTES=64 TUNING=1
```

Also be sure to set your operating system's power mode to best performance when running this program.

Increasing your operating system's page size may lead to better performance when using this program.

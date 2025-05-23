# Cuckatoo Reference Miner

### Description
Cuckatoo miner that supports cuckatoo10 to cuckatoo32 and implements mean, slean then mean, slean, and lean edge trimming on the GPU using OpenCL and Metal.

### Building
It's recommended that you build this program on the same system that you'll be running it on since it uses the `-march=native` and `-mtune=native` compiler flags to optimize itself for the current system's available features. Because of this, I don't release prebuilt versions of this program.

#### Linux
This program can be built and ran with Linux with the following commands where `EDGE_BITS` is set to the cuckatoo variation that you want to use (e.g. `EDGE_BITS=30` for cuckatoo30):
```
sudo apt install make g++ opencl-headers ocl-icd-opencl-dev libdbus-1-dev
make EDGE_BITS=30
make run
```

#### macOS
After installing [Xcode](https://developer.apple.com/xcode), this program can be built and ran with macOS with the following commands where `EDGE_BITS` is set to the cuckatoo variation that you want to use (e.g. `EDGE_BITS=30` for cuckatoo30):
```
make EDGE_BITS=30
make run
```
This program will use Metal as the GPU backend on macOS by default, however it can be configured to use OpenCL as the GPU backend instead by providing a `USE_OPENCL=1` setting when building it. Keep in mind that Apple has deprecated OpenCL on macOS and their OpenCL implementation may not conform to the standard, so this program may not function correctly under macOS when using OpenCL as the GPU backend.
```
make EDGE_BITS=30 USE_OPENCL=1
```

#### iOS
After installing [Xcode](https://developer.apple.com/xcode), this program can be built for iOS with the following commands where `EDGE_BITS` is set to the cuckatoo variation that you want to use (e.g. `EDGE_BITS=30` for cuckatoo30):
```
sudo xcode-select --switch "/Applications/Xcode.app"
make CC="\"$(xcrun --sdk iphoneos --find g++)\" -isysroot \"$(xcrun --sdk iphoneos --show-sdk-path)\"" EDGE_BITS=30
```
The resulting ipa file is unsigned, so you'll need to sign and provision it if you want to install it on an iPhone. Keep in mind that this program doesn't include any iOS UI.

This program can also be built for and ran with Xcode's iOS simulator with the following commands where `EDGE_BITS` is set to the cuckatoo variation that you want to use (e.g. `EDGE_BITS=30` for cuckatoo30):
```
sudo xcode-select --switch "/Applications/Xcode.app"
make CC="\"$(xcrun --sdk iphonesimulator --find g++)\" -isysroot \"$(xcrun --sdk iphonesimulator --show-sdk-path)\"" EDGE_BITS=30
make CC="\"$(xcrun --sdk iphonesimulator --find g++)\" -isysroot \"$(xcrun --sdk iphonesimulator --show-sdk-path)\"" run
```

#### Windows
After installing [MinGW-w64](https://winlibs.com), this program can be built and ran with Windows with the following commands where `EDGE_BITS` is set to the cuckatoo variation that you want to use (e.g. `EDGE_BITS=30` for cuckatoo30):
```
mingw32-make EDGE_BITS=30
mingw32-make run
```

#### Cross-compiling for Windows
This program can be built with Linux for Windows with the following commands where `EDGE_BITS` is set to the cuckatoo variation that you want to use (e.g. `EDGE_BITS=30` for cuckatoo30):
```
sudo apt install make g++-mingw-w64 cmake
make CC=x86_64-w64-mingw32-g++-posix crossCompilingDependencies
make CC=x86_64-w64-mingw32-g++-posix EDGE_BITS=30
```

### Usage
A stratum server address, port, username, and password can be provided when running this program to set the stratum server that it will mine to. For example, the following command will connect to the stratum server with the address `127.0.0.1` at port `3416` using the username `username` and password `password`. Once the software connects to a stratum server, it will start mining using mean, slean then mean, slean, or lean edge trimming depending on your available hardware and display the searching and trimming times for each graph that it processes. Those two pipeline stages run in parallel with the GPU being responsible for trimming and the CPU being responsible for searching.
```
"./Cuckatoo Reference Miner" --stratum_server_address 127.0.0.1 --stratum_server_port 3416 --stratum_server_username username --stratum_server_password password
```
You can display all your available GPUs and their indices that this program can use by running it with the `--display_gpus` command line argument.
```
"./Cuckatoo Reference Miner" --display_gpus
```
This program will attempt to use the first applicable GPU that it finds. However you can make it use a specific GPU by running it with the `--gpu` command line argument with the index of the GPU. For example, the following command will use the GPU with the index `1`.
```
"./Cuckatoo Reference Miner" --gpu 1
```
If you're running this program on a system with a configurable amount of GPU RAM, then you can set your GPU's RAM to a specified amount by running this program with the `--gpu_ram` command line argument with the amount of RAM in gigabytes to use. For example, the following command will configure your system to dedicate `16` GB of RAM to your GPU.
```
sudo "./Cuckatoo Reference Miner" --gpu_ram 16
```
This program can use only one GPU at a time, so if you want to mine with multiple GPUs then you'll need to run multiple instances of this program while specifying which GPU each instance should use with the `--gpu` command line argument. You should also specify the total number of instances that you are going to run with the `--total_number_of_instances` command line argument and the index of the current instance with the `--instance` command line argument. These command line arguments allow this program to avoid using the CPU cores that other instances are using which results in better performance. For example, the following command would be used if you're running `4` instances in total and this is instance `3` that you are running.
```
"./Cuckatoo Reference Miner" --total_number_of_instances 4 --instance 3
```
This program will try to use mean trimming, followed by slean then mean trimming, then slean trimming, and finally lean trimming in that order until it's able to perform one of those trimming types. This results in the fastest edge trimming type being used, however you can choose to only try certain trimming types by running this program with the `--mean_trimming`, `--slean_then_mean_trimming`, `--slean_trimming`, and/or `--lean_trimming` command line arguments for mean trimming, slean then mean trimming, slean trimming, and lean trimming respectively. For example, the following command will only try to use slean and lean trimming.
```
"./Cuckatoo Reference Miner" --slean_trimming --lean_trimming
```

### Tuning
A `TRIMMING_ROUNDS` setting can be provided when building this program that can be adjusted to make the `Searching time` displayed when running this program as close to, but not greater than, the `Trimming time` displayed to maximize this program's performance (without regard for power usage). Increasing the `TRIMMING_ROUNDS` setting decreases the `Searching time` and increases the `Trimming time`, and decreasing the `TRIMMING_ROUNDS` setting increases the `Searching time` and decreases the `Trimming time`.
```
make EDGE_BITS=30 TRIMMING_ROUNDS=25
```
You can provide a `SLEAN_TRIMMING_PARTS` setting when building this program to adjust the number of parts used by slean then mean trimming and slean trimming. This value must be a power of two, and lower values for this result in faster trimming. Decreasing this will increase the trimming type's GPU RAM requirement while increasing it will decrease the trimming type's GPU RAM requirement.
```
make EDGE_BITS=30 SLEAN_TRIMMING_PARTS=4
```
You can provide a `SLEAN_THEN_MEAN_SLEAN_TRIMMING_ROUNDS` setting when building this program to adjust the number of trimming rounds that are performed by slean trimming before the remaining trimming rounds are performed by mean trimming when using slean then mean trimming. Lower values for this result in faster trimming. Decreasing this value will increase the trimming type's GPU RAM requirement while increasing it will decrease the trimming type's GPU RAM requirement. You should first increase the `SLEAN_TRIMMING_PARTS` setting while this setting's value is `1` until slean trimming runs on your system, then you can increase this setting's value until slean then mean trimming runs on your system to maximize its performance.
```
make EDGE_BITS=30 SLEAN_THEN_MEAN_SLEAN_TRIMMING_ROUNDS=2
```
You can provide a `LOCAL_RAM_KILOBYTES` setting when building this program to adjust the targeted GPU local RAM size in kilobytes used by mean trimming, slean then mean trimming, and slean trimming. This value must be a power of two, and it should be as close to your GPU's local RAM size for potentially better performance.
```
make EDGE_BITS=30 LOCAL_RAM_KILOBYTES=64
```
A `TUNING=1` setting can be provided when building this program which will make it skip connecting to a stratum server. This is only useful for tuning the software since it can't mine blocks without being connected to a stratum server.
```
make EDGE_BITS=30 TRIMMING_ROUNDS=25 SLEAN_TRIMMING_PARTS=4 SLEAN_THEN_MEAN_SLEAN_TRIMMING_ROUNDS=2 LOCAL_RAM_KILOBYTES=64 TUNING=1
```
Also be sure to set your operating system's power mode to best performance when running this program.

Increasing your operating system's page size may lead to better performance when using this program.

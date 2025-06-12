# Cuckatoo Reference Miner

### Description
Cuckatoo miner that supports cuckatoo10 to cuckatoo32 and implements mean, slean then mean, slean, and lean edge trimming on the GPU using OpenCL and Metal. It can be used to mine MimbleWimble Coin (MWC) and MimbleWimble Coin-like cryptocurrencies, and it can run on Linux, macOS, iOS, Windows, and Android.

### Building
If you're building this program for a desktop operating system then it's recommended that you build this program on the same system that you'll be running it on since it uses the `-march=native` and `-mtune=native` compiler flags to optimize itself for the current system's available features. Because of this, I don't recommend using the prebuilt versions of this program for desktop operating systems.

#### Linux
This program can be built and ran with Linux with the following commands where `EDGE_BITS` is set to the cuckatoo variation that you want to use (e.g. `EDGE_BITS=31` for cuckatoo31):
```
sudo apt install make g++ opencl-headers ocl-icd-opencl-dev libdbus-1-dev
make EDGE_BITS=31
make run
```

#### macOS
After installing [Xcode](https://developer.apple.com/xcode), this program can be built and ran with macOS with the following commands where `EDGE_BITS` is set to the cuckatoo variation that you want to use (e.g. `EDGE_BITS=31` for cuckatoo31):
```
make EDGE_BITS=31
make run
```
This program will use Metal as the GPU backend on macOS by default, however it can be configured to use OpenCL as the GPU backend instead by providing a `USE_OPENCL=1` setting when building it. Keep in mind that Apple has deprecated OpenCL on macOS and their OpenCL implementation may not conform to the standard, so this program may not function correctly under macOS when using OpenCL as the GPU backend.
```
make EDGE_BITS=31 USE_OPENCL=1
```

#### iOS
After installing [Xcode](https://developer.apple.com/xcode), this program can be built for iOS with the following commands where `EDGE_BITS` is set to the cuckatoo variation that you want to use (e.g. `EDGE_BITS=31` for cuckatoo31):
```
sudo xcode-select --switch "/Applications/Xcode.app"
make CC="$(xcrun --sdk iphoneos --find g++)" SDK="$(xcrun --sdk iphoneos --show-sdk-path)" EDGE_BITS=31
```
The resulting ipa file is unsigned, so you'll need to sign and provision it if you want to install it on an iPhone.

This program can also be built for and ran with Xcode's iOS simulator with the following commands where `EDGE_BITS` is set to the cuckatoo variation that you want to use (e.g. `EDGE_BITS=31` for cuckatoo31):
```
sudo xcode-select --switch "/Applications/Xcode.app"
make CC="$(xcrun --sdk iphonesimulator --find g++)" SDK="$(xcrun --sdk iphonesimulator --show-sdk-path)" EDGE_BITS=31
make CC="$(xcrun --sdk iphonesimulator --find g++)" SDK="$(xcrun --sdk iphonesimulator --show-sdk-path)" run
```

#### Windows
After installing [MinGW-w64](https://winlibs.com), this program can be built and ran with Windows with the following commands where `EDGE_BITS` is set to the cuckatoo variation that you want to use (e.g. `EDGE_BITS=31` for cuckatoo31):
```
mingw32-make EDGE_BITS=31
mingw32-make run
```

#### Android
After installing [Android Studio](https://developer.android.com/studio) and [Android NDK](https://developer.android.com/ndk), this program can be built for and ran with Android with the following commands where `EDGE_BITS` is set to the cuckatoo variation that you want to use (e.g. `EDGE_BITS=31` for cuckatoo31), `CC` is set to the location of the Android NDK compiler that you want to use which is also used to determine the Android ABI and API level to use, `BUILD_TOOLS` is set to the location of the Android SDK build tools that you want to use, `ANDROID_JAR` is set to the location of the Android Java library that you want to use, `JBR_BIN` is set to the location of an Android JetBrains Runtime, and `ADB` is set to the location of an Android ADB platform tool:

If using Linux:
```
~/Android/Sdk/ndk/29.0.13113456/prebuilt/linux-x86_64/bin/make CC="~/Android/Sdk/ndk/29.0.13113456/toolchains/llvm/prebuilt/linux-x86_64/bin/aarch64-linux-android29-clang++" BUILD_TOOLS="~/Android/Sdk/build-tools/36.0.0" ANDROID_JAR="~/Android/Sdk/platforms/android-35/android.jar" JBR_BIN="~/android-studio/jbr/bin" ADB="~/Android/Sdk/platform-tools/adb" EDGE_BITS=31
~/Android/Sdk/ndk/29.0.13113456/prebuilt/linux-x86_64/bin/make CC="~/Android/Sdk/ndk/29.0.13113456/toolchains/llvm/prebuilt/linux-x86_64/bin/aarch64-linux-android29-clang++" BUILD_TOOLS="~/Android/Sdk/build-tools/36.0.0" ANDROID_JAR="~/Android/Sdk/platforms/android-35/android.jar" JBR_BIN="~/android-studio/jbr/bin" ADB="~/Android/Sdk/platform-tools/adb" run
```

If using macOS:
```
~/Library/Android/sdk/ndk/29.0.13113456/prebuilt/darwin-x86_64/bin/make CC="~/Library/Android/sdk/ndk/29.0.13113456/toolchains/llvm/prebuilt/darwin-x86_64/bin/aarch64-linux-android29-clang++" BUILD_TOOLS="~/Library/Android/Sdk/build-tools/36.0.0" ANDROID_JAR="~/Library/Android/Sdk/platforms/android-35/android.jar" JBR_BIN="/Applications/Android Studio.app/Contents/jbr/Contents/Home/bin" ADB="~/Library/Android/Sdk/platform-tools/adb" EDGE_BITS=31
~/Library/Android/sdk/ndk/29.0.13113456/prebuilt/darwin-x86_64/bin/make CC="~/Library/Android/sdk/ndk/29.0.13113456/toolchains/llvm/prebuilt/darwin-x86_64/bin/aarch64-linux-android29-clang++" BUILD_TOOLS="~/Library/Android/Sdk/build-tools/36.0.0" ANDROID_JAR="~/Library/Android/Sdk/platforms/android-35/android.jar" JBR_BIN="/Applications/Android Studio.app/Contents/jbr/Contents/Home/bin" ADB="~/Library/Android/Sdk/platform-tools/adb" run
```

If using Windows:
```
"%LOCALAPPDATA%\Android\Sdk\ndk\29.0.13113456\prebuilt\windows-x86_64\bin\make" CC="%LOCALAPPDATA%\Android\Sdk\ndk\29.0.13113456\toolchains\llvm\prebuilt\windows-x86_64\bin\aarch64-linux-android29-clang++" BUILD_TOOLS="%LOCALAPPDATA%\Android\Sdk\build-tools\36.0.0" ANDROID_JAR="%LOCALAPPDATA%\Android\Sdk\platforms\android-35\android.jar" JBR_BIN="%PROGRAMFILES%\Android\Android Studio\jbr\bin" ADB="%LOCALAPPDATA%\Android\Sdk\platform-tools\adb" EDGE_BITS=31
"%LOCALAPPDATA%\Android\Sdk\ndk\29.0.13113456\prebuilt\windows-x86_64\bin\make" CC="%LOCALAPPDATA%\Android\Sdk\ndk\29.0.13113456\toolchains\llvm\prebuilt\windows-x86_64\bin\aarch64-linux-android29-clang++" BUILD_TOOLS="%LOCALAPPDATA%\Android\Sdk\build-tools\36.0.0" ANDROID_JAR="%LOCALAPPDATA%\Android\Sdk\platforms\android-35\android.jar" JBR_BIN="%PROGRAMFILES%\Android\Android Studio\jbr\bin" ADB="%LOCALAPPDATA%\Android\Sdk\platform-tools\adb" run
```

#### Cross-compiling for Windows
This program can be built with Linux for Windows with the following commands where `EDGE_BITS` is set to the cuckatoo variation that you want to use (e.g. `EDGE_BITS=31` for cuckatoo31):
```
sudo apt install make g++-mingw-w64 cmake
make CC=x86_64-w64-mingw32-g++-posix crossCompilingDependencies
make CC=x86_64-w64-mingw32-g++-posix EDGE_BITS=31
```

### Usage
A stratum server address, port, username, and password can be provided when running this program to set the stratum server that it will mine to. For example, the following command will connect to the stratum server with the address `127.0.0.1` at port `3416` using the username `username` and password `password`. Once the software connects to a stratum server, it will start mining using mean, slean then mean, slean, or lean edge trimming depending on your available hardware and display the searching and trimming times for each graph that it processes. Those two pipeline stages run in parallel with the GPU being responsible for trimming and the CPU being responsible for searching.
```
"./Cuckatoo Reference Miner" --stratum_server_address 127.0.0.1 --stratum_server_port 3416 --stratum_server_username username --stratum_server_password password
```
Here's commands that can be used to mine to some  of the MimbleWimble Coin (MWC) mining pools:

* [2Miners](https://2miners.com/mwc-mining-pool) (Replace `YOUR_WALLETS_TOR_OR_MWCMQS_ADDRESS.RIG_ID` with your values, for example `rvscna33zmif2j4fz65wmvjvjevhywrlezzfm2q7jxyg5hejmpfuhcid.device_1`)
```
"./Cuckatoo Reference Miner" -a mwc.2miners.com:7575 -u YOUR_WALLETS_TOR_OR_MWCMQS_ADDRESS.RIG_ID
```
* [WoolyPooly](https://woolypooly.com/en/coin/mwc) (Replace `YOUR_USERNAME_GENERATED_BY_WOOLYPOOLY.RIG_ID` with your values, for example `WPHaHR0cDovL3J2c2NuYTMzem1pZjJqNGZ6NjV3bXZqdmpldmh5d3JsZXp6Zm0ycTdqeHlnNWhlam1wZnVoY2lkLm9uaW9u.device_1`)
```
"./Cuckatoo Reference Miner" -a pool.woolypooly.com:11000 -u YOUR_USERNAME_GENERATED_BY_WOOLYPOOLY.RIG_ID
```
* [MWC Pool](https://mwcpool.com) (Replace `YOUR_MWC_POOL_USERNAME.RIG_ID` with your values, for example `nicolasflamel.device_1`)
```
"./Cuckatoo Reference Miner" -a stratum+tcp://stratum2.mwcpool.com:2222 -u YOUR_MWC_POOL_USERNAME.RIG_ID
```
* [Pacific Pool](https://pacificpool.ws) (Replace `YOUR_PACIFIC_POOL_USERNAME.RIG_ID` with your values, for example `nicolasflamel.device_1`)
```
"./Cuckatoo Reference Miner" -a stratum+tcp://stratum2.pacificpool.ws:2222 -u YOUR_PACIFIC_POOL_USERNAME.RIG_ID
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
make EDGE_BITS=31 TRIMMING_ROUNDS=90
```
You can provide a `SLEAN_TRIMMING_PARTS` setting when building this program to adjust the number of parts used by slean then mean trimming and slean trimming. This value must be a power of two, and lower values for this result in faster trimming. Decreasing this will increase the trimming type's GPU RAM requirement while increasing it will decrease the trimming type's GPU RAM requirement.
```
make EDGE_BITS=31 SLEAN_TRIMMING_PARTS=4
```
You can provide a `SLEAN_THEN_MEAN_SLEAN_TRIMMING_ROUNDS` setting when building this program to adjust the number of trimming rounds that are performed by slean trimming before the remaining trimming rounds are performed by mean trimming when using slean then mean trimming. Lower values for this result in faster trimming. Decreasing this value will increase the trimming type's GPU RAM requirement while increasing it will decrease the trimming type's GPU RAM requirement. You should first increase the `SLEAN_TRIMMING_PARTS` setting while this setting's value is `1` until slean trimming runs on your system, then you can increase this setting's value until slean then mean trimming runs on your system to maximize its performance.
```
make EDGE_BITS=31 SLEAN_THEN_MEAN_SLEAN_TRIMMING_ROUNDS=2
```
You can provide a `LOCAL_RAM_KILOBYTES` setting when building this program to adjust the targeted GPU local RAM size in kilobytes used by mean trimming, slean then mean trimming, and slean trimming. This value must be a power of two, and it should be as close to your GPU's local RAM size for potentially better performance.
```
make EDGE_BITS=31 LOCAL_RAM_KILOBYTES=64
```
A `TUNING=1` setting can be provided when building this program which will make it skip connecting to a stratum server. This is only useful for tuning the software since it can't mine blocks without being connected to a stratum server.
```
make EDGE_BITS=31 TRIMMING_ROUNDS=90 SLEAN_TRIMMING_PARTS=4 SLEAN_THEN_MEAN_SLEAN_TRIMMING_ROUNDS=2 LOCAL_RAM_KILOBYTES=64 TUNING=1
```
Also be sure to set your operating system's power mode to best performance when running this program.

Increasing your operating system's page size may lead to better performance when using this program.

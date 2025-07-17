# Parameters
NAME = Cuckatoo Reference Miner
VERSION = 0.3.0
EDGE_BITS = 31
TRIMMING_ROUNDS = 90
SLEAN_TRIMMING_PARTS = 2
SLEAN_THEN_MEAN_SLEAN_TRIMMING_ROUNDS = 1
LOCAL_RAM_KILOBYTES = 32
CC = g++
CFLAGS = -O3 -ffast-math -Wall -Wextra -Wno-unknown-warning-option -Wno-vla -Wno-vla-cxx-extension -Wno-type-limits -Wno-missing-field-initializers -Wno-nan-infinity-disabled -std=c++20 -fno-exceptions -fno-rtti -finput-charset=UTF-8 -fexec-charset=UTF-8 -funsigned-char -DNAME="$(NAME)" -DVERSION=$(VERSION) -DEDGE_BITS=$(EDGE_BITS) -DTRIMMING_ROUNDS=$(TRIMMING_ROUNDS) -DSLEAN_TRIMMING_PARTS=$(SLEAN_TRIMMING_PARTS) -DSLEAN_THEN_MEAN_SLEAN_TRIMMING_ROUNDS=$(SLEAN_THEN_MEAN_SLEAN_TRIMMING_ROUNDS) -DLOCAL_RAM_KILOBYTES=$(LOCAL_RAM_KILOBYTES)
LIBS = -lm
SRCS = "./main.cpp"

# Check if using OpenCL
ifeq ($(USE_OPENCL),1)

	# Use OpenCL
	CFLAGS += -DUSE_OPENCL
endif

# Check if tuning
ifeq ($(TUNING),1)

	# Build for tuning
	CFLAGS += -DTUNING
endif

# Check if compiling for Android
ifneq (,$(findstring android,$(CC)))

	# Program name
	PROGRAM_NAME = lib$(subst $\ ,_,$(NAME)).so
	
	# Run command
	RUN_COMMAND = ("$(shell echo $(ADB))" uninstall "com.$(subst $\ ,_,$(NAME))" || cd .) && "$(shell echo $(ADB))" install "./$(NAME).apk" && "$(shell echo $(ADB))" shell am start -n "com.$(subst $\ ,_,$(NAME))/.MainActivity" && "$(shell echo $(ADB))" logcat -c && "$(shell echo $(ADB))" logcat -s "com.$(subst $\ ,_,$(NAME))"
	
	# Set flags and link libraries
	CFLAGS += -DUSE_OTHER_MAIN_FUNCTION -shared -fpic -fvisibility=hidden -static-libstdc++ -I"./opencl_headers"
	
	# Check if using Windows
	ifeq ($(OS),Windows_NT)
	
		# Set flags and link libraries
		LIBS += -L"./opencl_loader/dist/$(shell for %%a in ("$(CC)") do for /f "tokens=1 delims=-" %%b in ('echo %%~na') do echo %%b)/lib" -lOpenCL
		
		# Delete command
		DELETE_COMMAND = del /q
		
		# Null location
		NULL_LOCATION = "nul"
		
		# Package command
		PACKAGE_COMMAND = && (if exist "./build" rd /q /s "./build" > "nul") && md "./build/apk/lib/$(shell type "./opencl_loader/dist/$(shell for %%a in ("$(CC)") do for /f "tokens=1 delims=-" %%b in ('echo %%~na') do echo %%b)\abi")" && move "./$(PROGRAM_NAME)" "./build/apk/lib/$(shell type "./opencl_loader/dist/$(shell for %%a in ("$(CC)") do for /f "tokens=1 delims=-" %%b in ('echo %%~na') do echo %%b)\abi")" > "nul" && "$(shell echo $(JBR_BIN))\javac" -classpath "$(shell echo $(ANDROID_JAR))" -d "./build/obj" "./android/com/$(subst $\ ,_,$(NAME))/MainActivity.java" && set PATH="$(shell echo $(JBR_BIN))";%PATH% && "$(shell echo $(BUILD_TOOLS))\d8" --min-api $(shell for %%a in ("$(CC)") do for /f "tokens=3 delims=-" %%b in ('echo %%~na') do for /f "tokens=3 delims=d" %%c in ('echo %%b') do echo %%c) --release --lib "$(shell echo $(ANDROID_JAR))" --output "./build/apk" "./build/obj/com/$(subst $\ ,_,$(NAME))/MainActivity.class" && ((echo | set /p a="<?xml version="1.0" encoding="utf-8"?>") && echo.&& (echo | set /p a="<manifest xmlns:android="http://schemas.android.com/apk/res/android" package="com.$(subst $\ ,_,$(NAME))" android:versionCode="0" android:versionName="$(VERSION)">") && echo.&& (echo | set /p a="<uses-sdk android:minSdkVersion="$(shell for %%a in ("$(CC)") do for /f "tokens=3 delims=-" %%b in ('echo %%~na') do for /f "tokens=3 delims=d" %%c in ('echo %%b') do echo %%c)" android:targetSdkVersion="35"/>") && echo.&& (echo | set /p a="<uses-permission android:name="android.permission.INTERNET"/>") && echo.&& (echo | set /p a="<application android:label="$(NAME)" android:hardwareAccelerated="false">") && echo.&& (echo | set /p a="<uses-native-library android:name="libOpenCL.so" android:required="false"/>") && echo.&& (echo | set /p a="<activity android:name=".MainActivity" android:exported="true" android:launchMode="singleTask" android:configChanges=") && echo "colorMode|density|fontScale|fontWeightAdjustment|grammaticalGender|keyboard|keyboardHidden|layoutDirection|locale|mcc|mnc|navigation|orientation|screenLayout|screenSize|smallestScreenSize|touchscreen|uiMode"&& (echo | set /p a=">") && echo.&& (echo | set /p a="<intent-filter>") && echo.&& (echo | set /p a="<action android:name="android.intent.action.MAIN"/>") && echo.&& (echo | set /p a="<category android:name="android.intent.category.LAUNCHER"/>") && echo.&& (echo | set /p a="</intent-filter>") && echo.&& (echo | set /p a="</activity>") && echo.&& (echo | set /p a="</application>") && echo.&& (echo | set /p a="</manifest>") && echo.) > "./build/AndroidManifest.xml" && "$(shell echo $(BUILD_TOOLS))\aapt" package -M "./build/AndroidManifest.xml" -I "$(shell echo $(ANDROID_JAR))" -F "./build/$(NAME)_unsigned.apk" "./build/apk" && "$(shell echo $(BUILD_TOOLS))\zipalign" -p 4 "./build/$(NAME)_unsigned.apk" "./build/$(NAME)_aligned.apk" && "$(shell echo $(JBR_BIN))\keytool" -genkeypair -keystore "./build/keystore.jks" -keyalg RSA -storepass android -dname CN= -validity 36500 > "nul" 2>&1 && "$(shell echo $(BUILD_TOOLS))\apksigner" sign --ks "./build/keystore.jks" --ks-pass pass:android --out "./$(NAME).apk" "./build/$(NAME)_aligned.apk" && rd /q /s "./build" > "nul"
		
	# Otherwise
	else
	
		# Set flags and link libraries
		LIBS += -L"./opencl_loader/dist/$(shell basename "$(CC)" | cut -d"-" -f1)/lib" -lOpenCL
		
		# Delete command
		DELETE_COMMAND = rm -rf
		
		# Null location
		NULL_LOCATION = "/dev/null"
		
		# Package command
		PACKAGE_COMMAND = && rm -rf "./build" && mkdir -p "./build/apk/lib/$(shell cat "./opencl_loader/dist/$(shell basename "$(CC)" | cut -d"-" -f1)/abi")" && mv "./$(PROGRAM_NAME)" "./build/apk/lib/$(shell cat "./opencl_loader/dist/$(shell basename "$(CC)" | cut -d"-" -f1)/abi")" && "$(shell echo $(JBR_BIN))/javac" -classpath "$(shell echo $(ANDROID_JAR))" -d "./build/obj" "./android/com/$(subst $\ ,_,$(NAME))/MainActivity.java" && PATH="$(shell echo $(JBR_BIN))":$$PATH "$(shell echo $(BUILD_TOOLS))/d8" --min-api $(shell basename "$(CC)" | cut -d"-" -f3 | cut -d"d" -f3) --release --lib "$(shell echo $(ANDROID_JAR))" --output "./build/apk" "./build/obj/com/$(subst $\ ,_,$(NAME))/MainActivity.class" && echo "<?xml version=\"1.0\" encoding=\"utf-8\"?>\n<manifest xmlns:android=\"http://schemas.android.com/apk/res/android\" package=\"com.$(subst $\ ,_,$(NAME))\" android:versionCode=\"0\" android:versionName=\"$(VERSION)\">\n<uses-sdk android:minSdkVersion=\"$(shell basename "$(CC)" | cut -d"-" -f3 | cut -d"d" -f3)\" android:targetSdkVersion=\"35\"/>\n<uses-permission android:name=\"android.permission.INTERNET\"/>\n<application android:label=\"$(NAME)\" android:hardwareAccelerated=\"false\">\n<uses-native-library android:name=\"libOpenCL.so\" android:required=\"false\"/>\n<activity android:name=\".MainActivity\" android:exported=\"true\" android:launchMode=\"singleTask\" android:configChanges=\"colorMode|density|fontScale|fontWeightAdjustment|grammaticalGender|keyboard|keyboardHidden|layoutDirection|locale|mcc|mnc|navigation|orientation|screenLayout|screenSize|smallestScreenSize|touchscreen|uiMode\"\n>\n<intent-filter>\n<action android:name=\"android.intent.action.MAIN\"/>\n<category android:name=\"android.intent.category.LAUNCHER\"/>\n</intent-filter>\n</activity>\n</application>\n</manifest>" > "./build/AndroidManifest.xml" && "$(shell echo $(BUILD_TOOLS))/aapt" package -M "./build/AndroidManifest.xml" -I "$(shell echo $(ANDROID_JAR))" -F "./build/$(NAME)_unsigned.apk" "./build/apk" && "$(shell echo $(BUILD_TOOLS))/zipalign" -p 4 "./build/$(NAME)_unsigned.apk" "./build/$(NAME)_aligned.apk" && "$(shell echo $(JBR_BIN))/keytool" -genkeypair -keystore "./build/keystore.jks" -keyalg RSA -storepass android -dname CN= -validity 36500 > "/dev/null" 2>&1 && PATH="$(shell echo $(JBR_BIN))":$$PATH "$(shell echo $(BUILD_TOOLS))/apksigner" sign --ks "./build/keystore.jks" --ks-pass pass:android --out "./$(NAME).apk" "./build/$(NAME)_aligned.apk" && rm -r "./build"
	endif
	
# Otherwise check if compiling for Windows
else ifeq ($(OS),Windows_NT)

	# Program name
	PROGRAM_NAME = $(NAME).exe
	
	# Run command
	RUN_COMMAND = "./$(PROGRAM_NAME)"
	
	# Set flags and link libraries
	CFLAGS += -march=native -mtune=native -static-libstdc++ -static-libgcc -I"./opencl_headers"
	LIBS += -Wl,-Bstatic -lstdc++ -lpthread -Wl,-Bdynamic -lws2_32 "$(shell echo %SYSTEMROOT%)\System32\OpenCL.dll"
	
	# Delete command
	DELETE_COMMAND = del /q
	
	# Null location
	NULL_LOCATION = "nul"
	
# Otherwise check if cross-compiling for Windows
else ifneq (,$(findstring mingw,$(CC)))

	# Program name
	PROGRAM_NAME = $(NAME).exe
	
	# Run command
	RUN_COMMAND = wine "./$(PROGRAM_NAME)"
	
	# Set flags and link libraries
	CFLAGS += -march=native -mtune=native -static-libstdc++ -static-libgcc -I"./opencl_headers"
	LIBS += -Wl,-Bstatic -lstdc++ -lpthread -Wl,-Bdynamic -lws2_32 -L"./opencl_loader_cross_compiling/dist/lib" -lOpenCL
	
	# Delete command
	DELETE_COMMAND = rm -rf
	
	# Null location
	NULL_LOCATION = "/dev/null"
	
# Otherwise check if compiling for iOS
else ifneq (,$(findstring iPhoneOS,$(SDK)))

	# Program name
	PROGRAM_NAME = $(NAME)
	
	# Run command
	RUN_COMMAND = echo Sign and provision \"./$(NAME).ipa\" and install it on an iPhone to run it
	
	# Set flags and link libraries
	CFLAGS += -DUSE_OTHER_MAIN_FUNCTION -isysroot "$(shell echo $(SDK))" -mios-version-min=14 -fobjc-arc -arch arm64
	LIBS += -framework Foundation -framework Metal -framework UIKit
	SRCS += "./ios/main.mm"
	
	# Delete command
	DELETE_COMMAND = rm -rf
	
	# Null location
	NULL_LOCATION = "/dev/null"
	
	# Package command
	PACKAGE_COMMAND = && rm -rf "./Payload" && mkdir -p "./Payload/$(NAME).app" && mv "./$(PROGRAM_NAME)" "./Payload/$(NAME).app" && echo "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n<!DOCTYPE plist PUBLIC \"-//Apple//DTD PLIST 1.0//EN\" \"http://www.apple.com/DTDs/PropertyList-1.0.dtd\">\n<plist version=\"1.0\">\n<dict>\n<key>CFBundleExecutable</key>\n<string>$(NAME)</string>\n<key>CFBundleName</key>\n<string>$(NAME)</string>\n<key>CFBundleIdentifier</key>\n<string>$(NAME)</string>\n<key>CFBundleVersion</key>\n<string>$(VERSION)</string>\n<key>CFBundleShortVersionString</key>\n<string>$(VERSION)</string>\n<key>MinimumOSVersion</key>\n<string>14.0</string>\n<key>UILaunchScreen</key>\n<dict>\n</dict>\n</dict>\n</plist>" > "./Payload/$(NAME).app/Info.plist" && zip -rq "./$(NAME).ipa" "./Payload" && rm -r "./Payload"
	
# Otherwise check if compiling for iOS simulator
else ifneq (,$(findstring iPhoneSimulator,$(SDK)))

	# Program name
	PROGRAM_NAME = $(NAME)
	
	# Run command
	RUN_COMMAND = xcrun simctl install booted "./$(NAME).app" && xcrun simctl launch --console booted "$(NAME)"
	
	# Set flags and link libraries
	CFLAGS += -DUSE_OTHER_MAIN_FUNCTION -isysroot "$(shell echo $(SDK))" -mios-version-min=14 -fobjc-arc
	LIBS += -framework Foundation -framework Metal -framework UIKit
	SRCS += "./ios/main.mm"
	
	# Delete command
	DELETE_COMMAND = rm -rf
	
	# Null location
	NULL_LOCATION = "/dev/null"
	
	# Package command
	PACKAGE_COMMAND = && rm -rf "./$(NAME).app" && mkdir "./$(NAME).app" && mv "./$(PROGRAM_NAME)" "./$(NAME).app" && echo "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n<!DOCTYPE plist PUBLIC \"-//Apple//DTD PLIST 1.0//EN\" \"http://www.apple.com/DTDs/PropertyList-1.0.dtd\">\n<plist version=\"1.0\">\n<dict>\n<key>CFBundleExecutable</key>\n<string>$(NAME)</string>\n<key>CFBundleName</key>\n<string>$(NAME)</string>\n<key>CFBundleIdentifier</key>\n<string>$(NAME)</string>\n<key>CFBundleVersion</key>\n<string>$(VERSION)</string>\n<key>CFBundleShortVersionString</key>\n<string>$(VERSION)</string>\n<key>MinimumOSVersion</key>\n<string>14.0</string>\n<key>UILaunchScreen</key>\n<dict>\n</dict>\n</dict>\n</plist>" > "./$(NAME).app/Info.plist"
	
# Otherwise check if compiling for macOS
else ifeq ($(shell uname),Darwin)

	# Program name
	PROGRAM_NAME = $(NAME)
	
	# Run command
	RUN_COMMAND = "./$(PROGRAM_NAME)"
	
	# Set flags and link libraries
	CFLAGS += -march=native -mtune=native
	LIBS += -framework Foundation -framework Metal -framework IOKit
	
	# Check if using OpenCL
	ifeq ($(USE_OPENCL),1)
	
		# Set flags and link libraries
		LIBS += -framework OpenCL
	endif
	
	# Delete command
	DELETE_COMMAND = rm -rf
	
	# Null location
	NULL_LOCATION = "/dev/null"
	
# Otherwise
else

	# Program name
	PROGRAM_NAME = $(NAME)
	
	# Run command
	RUN_COMMAND = "./$(PROGRAM_NAME)"
	
	# Link libstdc++, OpenCL, and D-Bus
	CFLAGS += -march=native -mtune=native `pkg-config --cflags dbus-1`
	LIBS += -lstdc++ -lOpenCL `pkg-config --libs dbus-1`
	
	# Delete command
	DELETE_COMMAND = rm -rf
	
	# Null location
	NULL_LOCATION = "/dev/null"
endif

# Make
make:
	"$(shell echo $(CC))" $(CFLAGS) -o "./$(PROGRAM_NAME)" $(SRCS) $(LIBS) $(PACKAGE_COMMAND)

# Make run
run:
	$(RUN_COMMAND)

# Make clean
clean:
	$(DELETE_COMMAND) "./$(NAME)" "./$(NAME).exe" "./$(NAME).ipa" "./$(NAME).app" "./lib$(subst $\ ,_,$(NAME)).so" "./$(NAME).apk" "./$(NAME).apk.idsig" "./v2024.10.24.tar.gz" "./OpenCL-Headers-2024.10.24" "./OpenCL-ICD-Loader-2024.10.24" "./opencl_loader_cross_compiling" "./metal-cpp_macOS15.2_iOS18.2.zip" "./metal-cpp" "./Payload" "./build" > $(NULL_LOCATION) 2>&1

# Make Android dependencies (This command works when using Linux)
androidDependencies:
	
	# OpenCL headers
	rm -rf "./v2024.10.24.tar.gz" "./OpenCL-Headers-2024.10.24" "./opencl_headers"
	wget "https://github.com/KhronosGroup/OpenCL-Headers/archive/refs/tags/v2024.10.24.tar.gz"
	tar -xf "./v2024.10.24.tar.gz"
	rm "./v2024.10.24.tar.gz"
	mv "./OpenCL-Headers-2024.10.24" "./opencl_headers"
	
	# OpenCL loader
	rm -rf "./OpenCL-ICD-Loader-2024.10.24" "./opencl_loader"
	wget "https://github.com/KhronosGroup/OpenCL-ICD-Loader/archive/refs/tags/v2024.10.24.tar.gz"
	tar -xf "./v2024.10.24.tar.gz"
	mv "./OpenCL-ICD-Loader-2024.10.24" "./opencl_loader"
	cd "./opencl_loader" && rm -f "./CMakeCache.txt" && cmake -DCMAKE_INSTALL_PREFIX="$(CURDIR)/opencl_loader/dist/armv7a" -DCMAKE_ANDROID_ARCH_ABI=armeabi-v7a -DCMAKE_BUILD_TYPE=Release -DOPENCL_ICD_LOADER_HEADERS_DIR="$(CURDIR)/opencl_headers" -DCMAKE_SYSTEM_NAME=Android -DCMAKE_ANDROID_NDK="$(shell echo $(CC) | awk -F"/toolchains/" "{print \$$1}")" "./CMakeLists.txt" && make && make install && make clean && echo -n "armeabi-v7a" > "./dist/armv7a/abi"
	cd "./opencl_loader" && rm -f "./CMakeCache.txt" && cmake -DCMAKE_INSTALL_PREFIX="$(CURDIR)/opencl_loader/dist/aarch64" -DCMAKE_ANDROID_ARCH_ABI=arm64-v8a -DCMAKE_BUILD_TYPE=Release -DOPENCL_ICD_LOADER_HEADERS_DIR="$(CURDIR)/opencl_headers" -DCMAKE_SYSTEM_NAME=Android -DCMAKE_ANDROID_NDK="$(shell echo $(CC) | awk -F"/toolchains/" "{print \$$1}")" "./CMakeLists.txt" && make && make install && make clean && echo -n "arm64-v8a" > "./dist/aarch64/abi"
	cd "./opencl_loader" && rm -f "./CMakeCache.txt" && cmake -DCMAKE_INSTALL_PREFIX="$(CURDIR)/opencl_loader/dist/i686" -DCMAKE_ANDROID_ARCH_ABI=x86 -DCMAKE_BUILD_TYPE=Release -DOPENCL_ICD_LOADER_HEADERS_DIR="$(CURDIR)/opencl_headers" -DCMAKE_SYSTEM_NAME=Android -DCMAKE_ANDROID_NDK="$(shell echo $(CC) | awk -F"/toolchains/" "{print \$$1}")" "./CMakeLists.txt" && make && make install && make clean && echo -n "x86" > "./dist/i686/abi"
	cd "./opencl_loader" && rm -f "./CMakeCache.txt" && cmake -DCMAKE_INSTALL_PREFIX="$(CURDIR)/opencl_loader/dist/x86_64" -DCMAKE_ANDROID_ARCH_ABI=x86_64 -DCMAKE_BUILD_TYPE=Release -DOPENCL_ICD_LOADER_HEADERS_DIR="$(CURDIR)/opencl_headers" -DCMAKE_SYSTEM_NAME=Android -DCMAKE_ANDROID_NDK="$(shell echo $(CC) | awk -F"/toolchains/" "{print \$$1}")" "./CMakeLists.txt" && make && make install && make clean && echo -n "x86_64" > "./dist/x86_64/abi"
	find "./opencl_loader/dist" ! -name "libOpenCL.so" ! -name "abi" -type f -delete && find "./opencl_loader/dist" -empty -type d -delete
	tar -xf "./v2024.10.24.tar.gz"
	rm "./v2024.10.24.tar.gz"
	mv "./opencl_loader/dist" "./OpenCL-ICD-Loader-2024.10.24"
	rm -r "./opencl_loader"
	mv "./OpenCL-ICD-Loader-2024.10.24" "./opencl_loader"

# Make cross-compiling dependencies (This command works when using Linux)
crossCompilingDependencies:
	
	# OpenCL headers
	rm -rf "./v2024.10.24.tar.gz" "./OpenCL-Headers-2024.10.24" "./opencl_headers"
	wget "https://github.com/KhronosGroup/OpenCL-Headers/archive/refs/tags/v2024.10.24.tar.gz"
	tar -xf "./v2024.10.24.tar.gz"
	rm "./v2024.10.24.tar.gz"
	mv "./OpenCL-Headers-2024.10.24" "./opencl_headers"
	
	# OpenCL loader
	rm -rf "./OpenCL-ICD-Loader-2024.10.24" "./opencl_loader_cross_compiling"
	wget "https://github.com/KhronosGroup/OpenCL-ICD-Loader/archive/refs/tags/v2024.10.24.tar.gz"
	tar -xf "./v2024.10.24.tar.gz"
	rm "./v2024.10.24.tar.gz"
	mv "./OpenCL-ICD-Loader-2024.10.24" "./opencl_loader_cross_compiling"
	cd "./opencl_loader_cross_compiling" && cmake -DCMAKE_INSTALL_PREFIX="$(CURDIR)/opencl_loader_cross_compiling/dist" -DCMAKE_BUILD_TYPE=Release -DOPENCL_ICD_LOADER_HEADERS_DIR="$(CURDIR)/opencl_headers" -DCMAKE_SYSTEM_NAME=Windows -DCMAKE_C_COMPILER="$(shell echo $(subst g++,gcc,$(CC)))" -DCMAKE_CXX_COMPILER="$(shell echo $(CC))" "./CMakeLists.txt" && make && make install

# Make Apple dependencies (This command works when using macOS)
appleDependencies:
	
	# Metal-cpp
	rm -rf "./metal-cpp_macOS15.2_iOS18.2.zip" "./metal-cpp" "./metal.h"
	curl -LO "https://developer.apple.com/metal/cpp/files/metal-cpp_macOS15.2_iOS18.2.zip"
	unzip "./metal-cpp_macOS15.2_iOS18.2.zip"
	rm "./metal-cpp_macOS15.2_iOS18.2.zip"
	cd "./metal-cpp" && "./SingleHeader/MakeSingleHeader.py" -o "../metal.h" "./Metal/Metal.hpp"
	rm -r "./metal-cpp"

# Make Windows dependencies (This command works when using Windows)
windowsDependencies:
	
	rem OpenCL headers
	del /q "./v2024.10.24.tar.gz" > "nul" 2>&1
	if exist "./OpenCL-Headers-2024.10.24" rd /q /s "./OpenCL-Headers-2024.10.24" > "nul"
	if exist "./opencl_headers" rd /q /s "./opencl_headers" > "nul"
	curl -LO "https://github.com/KhronosGroup/OpenCL-Headers/archive/refs/tags/v2024.10.24.tar.gz"
	tar -xf "./v2024.10.24.tar.gz"
	del "./v2024.10.24.tar.gz"
	rename "./OpenCL-Headers-2024.10.24" "./opencl_headers"

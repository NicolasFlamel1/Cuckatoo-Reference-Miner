# Parameters
NAME = "Cuckatoo Reference Miner"
VERSION = 0.1.0
EDGE_BITS = 30
TRIMMING_ROUNDS = 65
SLEAN_TRIMMING_PARTS = 2
SLEAN_THEN_MEAN_SLEAN_TRIMMING_ROUNDS = 1
LOCAL_RAM_KILOBYTES = 32
CC = g++
CFLAGS = -march=native -mtune=native -Ofast -Wall -Wextra -Wno-type-limits -Wno-missing-field-initializers -std=c++20 -fno-exceptions -fno-rtti -finput-charset=UTF-8 -fexec-charset=UTF-8 -funsigned-char -DNAME=$(NAME) -DVERSION=$(VERSION) -DEDGE_BITS=$(EDGE_BITS) -DTRIMMING_ROUNDS=$(TRIMMING_ROUNDS) -DSLEAN_TRIMMING_PARTS=$(SLEAN_TRIMMING_PARTS) -DSLEAN_THEN_MEAN_SLEAN_TRIMMING_ROUNDS=$(SLEAN_THEN_MEAN_SLEAN_TRIMMING_ROUNDS) -DLOCAL_RAM_KILOBYTES=$(LOCAL_RAM_KILOBYTES)
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

# Check if compiling for Windows
ifeq ($(OS),Windows_NT)

	# Link libstdc++, libgcc, pthread, winsock, and OpenCL
	CFLAGS += -static-libstdc++ -static-libgcc -I"./opencl_headers"
	LIBS += -Wl,-Bstatic -lstdc++ -lpthread -Wl,-Bdynamic -lws2_32 "$(shell echo %SYSTEMROOT%)\System32\OpenCL.dll"
	
	# Program name
	PROGRAM_NAME = $(subst $\",,$(NAME)).exe
	
	# Delete command
	DELETE_COMMAND = del /q
	
	# Null location
	NULL_LOCATION = "nul"
	
# Otherwise check if cross-compiling for Windows
else ifneq (,$(findstring mingw,$(CC)))

	# Link libstdc++, libgcc, pthread, winsock, and OpenCL
	CFLAGS += -static-libstdc++ -static-libgcc -I"./opencl_headers_cross_compiling/dist/include"
	LIBS += -Wl,-Bstatic -lstdc++ -lpthread -Wl,-Bdynamic -lws2_32 -L"./opencl_loader/dist/lib" -lOpenCL
	
	# Program name
	PROGRAM_NAME = $(subst $\",,$(NAME)).exe
	
	# Delete command
	DELETE_COMMAND = rm -rf
	
	# Null location
	NULL_LOCATION = "/dev/null"
	
	# Run command
	RUN_COMMAND = wine
	
	# Dependencies flags
	DEPENDENCIES_FLAGS = -DCMAKE_SYSTEM_NAME=Windows -DCMAKE_C_COMPILER=$(subst g++,gcc,$(CC)) -DCMAKE_CXX_COMPILER=$(CC)
	
# Otherwise check if compiling for macOS
else ifeq ($(shell uname),Darwin)

	# Link Foundation, Metal, and IOKit
	LIBS += -framework Foundation -framework Metal -framework IOKit
	
	# Check if using OpenCL
	ifeq ($(USE_OPENCL),1)

		# Link OpenCL
		LIBS += -framework OpenCL
	endif
	
	# Program name
	PROGRAM_NAME = $(subst $\",,$(NAME))
	
	# Delete command
	DELETE_COMMAND = rm -rf
	
	# Null location
	NULL_LOCATION = "/dev/null"
	
# Otherwise
else

	# Link libstdc++, OpenCL, and D-Bus
	CFLAGS += `pkg-config --cflags dbus-1`
	LIBS += -lstdc++ -lOpenCL `pkg-config --libs dbus-1`
	
	# Program name
	PROGRAM_NAME = $(subst $\",,$(NAME))
	
	# Delete command
	DELETE_COMMAND = rm -rf
	
	# Null location
	NULL_LOCATION = "/dev/null"
endif

# Make
make:
	$(CC) $(CFLAGS) -o "./$(PROGRAM_NAME)" $(SRCS) $(LIBS)

# Make run
run:
	$(RUN_COMMAND) "./$(PROGRAM_NAME)"

# Make clean
clean:
	$(DELETE_COMMAND) "./$(subst $\",,$(NAME))" "./$(subst $\",,$(NAME)).exe" "./v2024.10.24.tar.gz" "./OpenCL-Headers-2024.10.24" "./opencl_headers_cross_compiling" "./OpenCL-ICD-Loader-2024.10.24" "./opencl_loader" "./metal-cpp_macOS15.2_iOS18.2.zip" "./metal-cpp" >$(NULL_LOCATION) 2>&1

# Make cross-compiling dependencies (This command works when using Linux)
crossCompilingDependencies:
	
	# OpenCL headers
	rm -rf "./v2024.10.24.tar.gz" "./OpenCL-Headers-2024.10.24" "./opencl_headers_cross_compiling"
	wget "https://github.com/KhronosGroup/OpenCL-Headers/archive/refs/tags/v2024.10.24.tar.gz"
	tar -xf "./v2024.10.24.tar.gz"
	rm "./v2024.10.24.tar.gz"
	mv "./OpenCL-Headers-2024.10.24" "./opencl_headers_cross_compiling"
	cd "./opencl_headers_cross_compiling" && cmake -DCMAKE_INSTALL_PREFIX="$(CURDIR)/opencl_headers_cross_compiling/dist" -DCMAKE_BUILD_TYPE=Release $(DEPENDENCIES_FLAGS) "./CMakeLists.txt" && make && make install
	
	# OpenCL loader
	rm -rf "./OpenCL-ICD-Loader-2024.10.24" "./opencl_loader"
	wget "https://github.com/KhronosGroup/OpenCL-ICD-Loader/archive/refs/tags/v2024.10.24.tar.gz"
	tar -xf "./v2024.10.24.tar.gz"
	rm "./v2024.10.24.tar.gz"
	mv "./OpenCL-ICD-Loader-2024.10.24" "./opencl_loader"
	cd "./opencl_loader" && cmake -DCMAKE_INSTALL_PREFIX="$(CURDIR)/opencl_loader/dist" -DCMAKE_BUILD_TYPE=Release -DOPENCL_ICD_LOADER_HEADERS_DIR="$(CURDIR)/opencl_headers_cross_compiling/dist/include" $(DEPENDENCIES_FLAGS) "./CMakeLists.txt" && make && make install

# Make macOS dependencies (This command works when using macOS)
macOSDependencies:
	
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
	del /q "./v2024.10.24.tar.gz" >nul 2>&1
	if exist "./OpenCL-Headers-2024.10.24" rd /q /s "./OpenCL-Headers-2024.10.24" >nul 2>&1
	if exist "./opencl_headers" rd /q /s "./opencl_headers" >nul 2>&1
	curl -LO "https://github.com/KhronosGroup/OpenCL-Headers/archive/refs/tags/v2024.10.24.tar.gz"
	tar -xf "./v2024.10.24.tar.gz"
	del "./v2024.10.24.tar.gz"
	rename "./OpenCL-Headers-2024.10.24" "./opencl_headers"

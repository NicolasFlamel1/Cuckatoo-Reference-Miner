# Parameters
NAME = "Cuckatoo Reference Miner"
VERSION = 0.1.0
EDGE_BITS = 30
TRIMMING_ROUNDS = 25
SLEAN_TRIMMING_PARTS = 2
LOCAL_RAM_KILOBYTES = 32
CC = "g++"
CFLAGS = -march=native -mtune=native -Ofast -Wall -Wextra -Wno-vla -Wno-type-limits -Wno-missing-field-initializers -Wno-implicit-fallthrough -Wno-write-strings -std=c++20 -fno-exceptions -fno-rtti -finput-charset=UTF-8 -fexec-charset=UTF-8 -funsigned-char -DNAME=$(NAME) -DVERSION=$(VERSION) -DEDGE_BITS=$(EDGE_BITS) -DTRIMMING_ROUNDS=$(TRIMMING_ROUNDS) -DSLEAN_TRIMMING_PARTS=$(SLEAN_TRIMMING_PARTS) -DLOCAL_RAM_KILOBYTES=$(LOCAL_RAM_KILOBYTES)
LIBS = -lm
SRCS = "./main.cpp"

# Check if tuning
ifeq ($(TUNING),1)

	# Build for tuning
	CFLAGS += -D TUNING
endif

# Check if compiling for Windows
ifeq ($(OS),Windows_NT)

	# Link libstdc++, libgcc, pthread, and winsock
	CFLAGS += -static-libstdc++ -static-libgcc
	LIBS += -Wl,-Bstatic -lstdc++ -lpthread -Wl,-Bdynamic -lws2_32
	
	# Link OpenCL
	CFLAGS += -I"./opencl_headers"
	LIBS += "$(shell echo %SYSTEMROOT%)\System32\OpenCL.dll"
	
	# Program name
	PROGRAM_NAME = $(subst $\",,$(NAME)).exe
	
# Otherwise check if cross-compiling for Windows
else ifneq (,$(findstring mingw,$(CC)))

	# Link libstdc++, libgcc, pthread, and winsock
	CFLAGS += -static-libstdc++ -static-libgcc
	LIBS += -Wl,-Bstatic -lstdc++ -lpthread -Wl,-Bdynamic -lws2_32
	
	# Link OpenCL
	CFLAGS += -I"./opencl_headers/dist/include"
	LIBS += -L"./opencl_loader/dist/lib" -lOpenCL
	
	# Program name
	PROGRAM_NAME = $(subst $\",,$(NAME)).exe
	
	# Run command
	RUN_COMMAND = "wine"
	
	# Dependencies flags
	DEPENDENCIES_FLAGS = -DCMAKE_SYSTEM_NAME=Windows -DCMAKE_C_COMPILER=$(subst g++,gcc,$(CC)) -DCMAKE_CXX_COMPILER=$(CC)
	
# Otherwise check if compiling for macOS
else ifeq ($(shell uname),Darwin)

	# Link Foundation and Metal
	LIBS += -framework Foundation -framework Metal
	
	# Program name
	PROGRAM_NAME = $(subst $\",,$(NAME))
	
# Otherwise
else

	# Link libstdc++ and OpenCL
	LIBS += -lstdc++ -lOpenCL
	
	# Program name
	PROGRAM_NAME = $(subst $\",,$(NAME))
endif

# Make
make:
	$(CC) $(CFLAGS) -o "./$(PROGRAM_NAME)" $(SRCS) $(LIBS)

# Make run
run:
	$(RUN_COMMAND) "./$(PROGRAM_NAME)"

# Make clean
clean:
	rm -rf "./$(PROGRAM_NAME)" "./$(PROGRAM_NAME).exe" "./v2024.10.24.tar.gz" "./OpenCL-Headers-2024.10.24" "./opencl_headers" "./OpenCL-ICD-Loader-2024.10.24" "./opencl_loader" "./metal-cpp_macOS15.2_iOS18.2.zip" "./metal-cpp"

# Make cross-compiling dependencies
crossCompilingDependencies:
	
	# OpenCL headers
	wget "https://github.com/KhronosGroup/OpenCL-Headers/archive/refs/tags/v2024.10.24.tar.gz"
	tar -xf "./v2024.10.24.tar.gz"
	rm "./v2024.10.24.tar.gz"
	mv "./OpenCL-Headers-2024.10.24" "./opencl_headers"
	cd "./opencl_headers" && cmake -DCMAKE_INSTALL_PREFIX="$(CURDIR)/opencl_headers/dist" -DCMAKE_BUILD_TYPE=Release $(DEPENDENCIES_FLAGS) "./CMakeLists.txt" && make && make install
	
	# OpenCL loader
	wget "https://github.com/KhronosGroup/OpenCL-ICD-Loader/archive/refs/tags/v2024.10.24.tar.gz"
	tar -xf "./v2024.10.24.tar.gz"
	rm "./v2024.10.24.tar.gz"
	mv "./OpenCL-ICD-Loader-2024.10.24" "./opencl_loader"
	cd "./opencl_loader" && cmake -DCMAKE_INSTALL_PREFIX="$(CURDIR)/opencl_loader/dist" -DCMAKE_BUILD_TYPE=Release -DOPENCL_ICD_LOADER_HEADERS_DIR="$(CURDIR)/opencl_headers/dist/include" $(DEPENDENCIES_FLAGS) "./CMakeLists.txt" && make && make install

# Make macOS dependencies
macOSDependencies:
	
	# Metal-cpp
	curl -LO "https://developer.apple.com/metal/cpp/files/metal-cpp_macOS15.2_iOS18.2.zip"
	unzip "./metal-cpp_macOS15.2_iOS18.2.zip"
	rm "./metal-cpp_macOS15.2_iOS18.2.zip"
	cd "./metal-cpp" && "./SingleHeader/MakeSingleHeader.py" -o "../metal.h" "./Metal/Metal.hpp"
	rm -r "./metal-cpp"

# Make Windows dependencies
windowsDependencies:
	curl -LO "https://github.com/KhronosGroup/OpenCL-Headers/archive/refs/tags/v2024.10.24.tar.gz"
	tar -xf "./v2024.10.24.tar.gz"
	del "./v2024.10.24.tar.gz"
	rename "./OpenCL-Headers-2024.10.24" "./opencl_headers"

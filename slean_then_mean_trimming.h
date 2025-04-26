// Header guard
#ifndef SLEAN_THEN_MEAN_TRIMMING_H
#define SLEAN_THEN_MEAN_TRIMMING_H


// Header files
using namespace std;


// Constants

// Slean then mean trimming required RAM bytes
#define SLEAN_THEN_MEAN_TRIMMING_REQUIRED_RAM_BYTES 0

// Slean then mean trimming required work group RAM bytes
#define SLEAN_THEN_MEAN_TRIMMING_REQUIRED_WORK_GROUP_RAM_BYTES 0


// Function prototypes

// Check if using macOS
#ifdef __APPLE__

	// Create slean then mean trimming context
	static inline MTL::Device *createSleanThenMeanTrimmingContext(const unsigned int deviceIndex) noexcept;
	
	// Perform slean then mean trimming loop
	static inline bool performSleanThenMeanTrimmingLoop(MTL::Device *device, const unsigned int deviceIndex) noexcept;
	
// Otherwise
#else

	// Create slean then mean trimming context
	static inline cl_context createSleanThenMeanTrimmingContext(const cl_platform_id platforms[], const cl_uint numberOfPlatforms, const unsigned int deviceIndex) noexcept;
	
	// Perform slean then mean trimming loop
	static inline bool performSleanThenMeanTrimmingLoop(const cl_context context, const unsigned int deviceIndex) noexcept;
#endif


// Supporting function implementation

// Check if using macOS
#ifdef __APPLE__

	// Create slean then mean trimming context
	MTL::Device *createSleanThenMeanTrimmingContext(const unsigned int deviceIndex) noexcept {
	
		// TODO
		return nullptr;
		
		// Set index to zero
		unsigned int index = 0;
		
		// Check if getting all devices was successful
		const unique_ptr<NS::Array, void(*)(NS::Array *)> devices(MTL::CopyAllDevices(), [](NS::Array *devices) noexcept {
		
			// Free devices
			devices->release();
		});
		if(devices) {
		
			// Go through all devices
			for(NS::UInteger i = 0; i < devices->count(); ++i) {
			
				// Get device
				MTL::Device *device = reinterpret_cast<MTL::Device *>(devices->object(i));
				
				// Check if device supports the Metal version
				if(device && device->supportsFamily(MTL::GPUFamilyMetal3)) {
				
					// Check if device's name exists
					const NS::String *name = device->name();
					if(name) {
					
						// Check if name's UTF-8 string exists
						const char *utf8String = name->utf8String();
						if(utf8String) {
						
							// Check if device index isn't specified or index is for the specified device
							if(deviceIndex == ALL_DEVICES || ++index == deviceIndex) {
							
								// Check if device's memory size and work group memory size are large enough
								if(device->recommendedMaxWorkingSetSize() >= SLEAN_THEN_MEAN_TRIMMING_REQUIRED_RAM_BYTES && device->maxThreadgroupMemoryLength() >= SLEAN_THEN_MEAN_TRIMMING_REQUIRED_WORK_GROUP_RAM_BYTES) {
							
									// Don't free device when devices is freed
									device->retain();
									
									// Return device
									return device;
								}
							}
						}
					}
				}
			}
		}
		
		// Return nothing
		return nullptr;
	}
	
// Otherwise
#else

	// Create slean then mean trimming context
	cl_context createSleanThenMeanTrimmingContext(const cl_platform_id platforms[], const cl_uint numberOfPlatforms, const unsigned int deviceIndex) noexcept {
	
		// TODO
		return nullptr;
		
		// Set index to zero
		unsigned int index = 0;
		
		// Go through all platforms
		for(cl_uint i = 0; i < numberOfPlatforms; ++i) {
			
			// Check if getting platform's number of devices was successful and devices exist
			cl_uint numberOfDevices;
			if(clGetDeviceIDs(platforms[i], CL_DEVICE_TYPE_GPU, 0, nullptr, &numberOfDevices) == CL_SUCCESS && numberOfDevices) {
			
				// Check if getting platform's devices was successful
				cl_device_id devices[numberOfDevices];
				if(clGetDeviceIDs(platforms[i], CL_DEVICE_TYPE_GPU, numberOfDevices, devices, nullptr) == CL_SUCCESS) {
				
					// Go through all of the platform's devices
					for(cl_uint j = 0; j < numberOfDevices; ++j) {
					
						// Check if getting device's availability and profile size was successful, device is available, and profile exists
						cl_bool isAvailable;
						size_t profileSize;
						if(clGetDeviceInfo(devices[j], CL_DEVICE_AVAILABLE, sizeof(isAvailable), &isAvailable, nullptr) == CL_SUCCESS && clGetDeviceInfo(devices[j], CL_DEVICE_PROFILE, 0, nullptr, &profileSize) == CL_SUCCESS && isAvailable == CL_TRUE && profileSize) {
						
							// Check if getting device's profile and OpenCL version size was successful, profile is full profile, and OpenCL version exists
							char profile[profileSize];
							size_t openClVersionSize;
							if(clGetDeviceInfo(devices[j], CL_DEVICE_PROFILE, profileSize, profile, nullptr) == CL_SUCCESS && clGetDeviceInfo(devices[j], CL_DEVICE_OPENCL_C_VERSION, 0, nullptr, &openClVersionSize) == CL_SUCCESS && !strcmp(profile, "FULL_PROFILE") && openClVersionSize) {
							
								// Check if getting device's OpenCL version, is little endian, and name size was successful, OpenCL version is compatible, device is little endian, and its name exists
								char openClVersion[openClVersionSize];
								cl_bool isLittleEndian;
								size_t nameSize;
								if(clGetDeviceInfo(devices[j], CL_DEVICE_OPENCL_C_VERSION, openClVersionSize, openClVersion, nullptr) == CL_SUCCESS && clGetDeviceInfo(devices[j], CL_DEVICE_ENDIAN_LITTLE, sizeof(isLittleEndian), &isLittleEndian, nullptr) == CL_SUCCESS && clGetDeviceInfo(devices[j], CL_DEVICE_NAME, 0, nullptr, &nameSize) == CL_SUCCESS && !strncmp(openClVersion, "OpenCL C ", sizeof("OpenCL C ") - sizeof('\0')) && strtod(&openClVersion[sizeof("OpenCL C ") - sizeof('\0')], nullptr) >= 1.2 && isLittleEndian == CL_TRUE && nameSize) {
								
									// Check if getting device's name was successful
									char name[nameSize];
									if(clGetDeviceInfo(devices[j], CL_DEVICE_NAME, nameSize, name, nullptr) == CL_SUCCESS) {
									
										// Check if device index isn't specified or index is for the specified device
										if(deviceIndex == ALL_DEVICES || ++index == deviceIndex) {
										
											// Check if getting device's memory size and work group memory size was successful, memory size is large enough, and work group memory size is large enough
											cl_ulong memorySize;
											cl_ulong workGroupMemorySize;
											if(clGetDeviceInfo(devices[j], CL_DEVICE_GLOBAL_MEM_SIZE, sizeof(memorySize), &memorySize, nullptr) == CL_SUCCESS && clGetDeviceInfo(devices[j], CL_DEVICE_LOCAL_MEM_SIZE, sizeof(workGroupMemorySize), &workGroupMemorySize, nullptr) == CL_SUCCESS && memorySize >= SLEAN_THEN_MEAN_TRIMMING_REQUIRED_RAM_BYTES && workGroupMemorySize >= SLEAN_THEN_MEAN_TRIMMING_REQUIRED_WORK_GROUP_RAM_BYTES) {
											
												// Check if creating context for the device was successful
												cl_context context = clCreateContext(unmove((const cl_context_properties[]){CL_CONTEXT_PLATFORM, reinterpret_cast<cl_context_properties>(platforms[i]), 0}), 1, &devices[j], nullptr, nullptr, nullptr);
												if(context) {
												
													// Return context
													return context;
												}
											}
										}
									}
								}
							}
						}
					}
				}
			}
		}
		
		// Return nothing
		return nullptr;
	}
#endif

// Check if using macOS
#ifdef __APPLE__

	// Perform slean then mean trimming loop
	bool performSleanThenMeanTrimmingLoop(__attribute__((unused)) MTL::Device *device, __attribute__((unused)) const unsigned int deviceIndex) noexcept {
		
		// TODO
		return false;
	}
	
// Otherwise
#else

	// Perform slean then mean trimming loop
	bool performSleanThenMeanTrimmingLoop(__attribute__((unused)) const cl_context context, __attribute__((unused)) const unsigned int deviceIndex) noexcept {
	
		// TODO
		return false;
	}
#endif


#endif

// Header guard
#ifndef SLEAN_TRIMMING_H
#define SLEAN_TRIMMING_H


// Header files
using namespace std;


// Configurable constants

// Slean trimming number of edges per step one work item
#define SLEAN_TRIMMING_NUMBER_OF_EDGES_PER_STEP_ONE_WORK_ITEM min(static_cast<uint64_t>(1024), NUMBER_OF_EDGES / SLEAN_TRIMMING_PARTS)

// Slean trimming work items per work group for step one
#define SLEAN_TRIMMING_WORK_ITEMS_PER_WORK_GROUP_STEP_ONE 512


// Constants

// Slean trimming number of bitmap bytes
#define SLEAN_TRIMMING_NUMBER_OF_BITMAP_BYTES min(static_cast<uint64_t>(LOCAL_RAM_KILOBYTES * BYTES_IN_A_KILOBYTE), NUMBER_OF_EDGES / BITS_IN_A_BYTE)

// Slean trimming number of buckets
#define SLEAN_TRIMMING_NUMBER_OF_BUCKETS ((NUMBER_OF_EDGES / BITS_IN_A_BYTE) / SLEAN_TRIMMING_NUMBER_OF_BITMAP_BYTES)

// Slean trimming number of least significant bits ignored during bucket sorting
#define SLEAN_TRIMMING_NUMBER_OF_LEAST_SIGNIFICANT_BITS_IGNORED_DURING_BUCKET_SORTING (EDGE_BITS - bit_width(SLEAN_TRIMMING_NUMBER_OF_BUCKETS) + 1)

// Slean trimming number of items per bucket
#define SLEAN_TRIMMING_NUMBER_OF_ITEMS_PER_BUCKET ((NUMBER_OF_EDGES / SLEAN_TRIMMING_PARTS) / SLEAN_TRIMMING_NUMBER_OF_BUCKETS)

// Slean trimming initial max number of edges per bucket (Divide by 2 and multiply by 2 makes the result a product of 2)
#define SLEAN_TRIMMING_INITIAL_MAX_NUMBER_OF_EDGES_PER_BUCKET ((static_cast<uint32_t>(SLEAN_TRIMMING_NUMBER_OF_ITEMS_PER_BUCKET * 1.05) / 2) * 2)

// Slean trimming after trimming round max number of edges per bucket
#define SLEAN_TRIMMING_AFTER_TRIMMING_ROUND_MAX_NUMBER_OF_EDGES_PER_BUCKET static_cast<uint32_t>(SLEAN_TRIMMING_NUMBER_OF_ITEMS_PER_BUCKET * 0.73)

// Slean trimming number of remaining edges bitmap bytes
#define SLEAN_TRIMMING_NUMBER_OF_REMAINING_EDGES_BITMAP_BYTES min(static_cast<uint64_t>(LOCAL_RAM_KILOBYTES * BYTES_IN_A_KILOBYTE), (NUMBER_OF_EDGES / BITS_IN_A_BYTE) / SLEAN_TRIMMING_PARTS)

// Slean trimming number of remaining edges buckets
#define SLEAN_TRIMMING_NUMBER_OF_REMAINING_EDGES_BUCKETS (((NUMBER_OF_EDGES / BITS_IN_A_BYTE) / SLEAN_TRIMMING_PARTS) / SLEAN_TRIMMING_NUMBER_OF_REMAINING_EDGES_BITMAP_BYTES)

// Slean trimming number of least significant bits ignored during remaining edges bucket sorting
#define SLEAN_TRIMMING_NUMBER_OF_LEAST_SIGNIFICANT_BITS_IGNORED_DURING_REMAINING_EDGES_BUCKET_SORTING (EDGE_BITS - bit_width(SLEAN_TRIMMING_NUMBER_OF_REMAINING_EDGES_BUCKETS) + 1)

// Slean trimming number of items per remaining edges bucket
#define SLEAN_TRIMMING_NUMBER_OF_ITEMS_PER_REMAINING_EDGES_BUCKET ((NUMBER_OF_EDGES / SLEAN_TRIMMING_PARTS) / SLEAN_TRIMMING_NUMBER_OF_REMAINING_EDGES_BUCKETS)

// Slean trimming after trimming round max number of edges per remaining edges bucket
#define SLEAN_TRIMMING_AFTER_TRIMMING_ROUND_MAX_NUMBER_OF_EDGES_PER_REMAINING_EDGES_BUCKET static_cast<uint32_t>(SLEAN_TRIMMING_NUMBER_OF_ITEMS_PER_REMAINING_EDGES_BUCKET * 0.38)

// Check if using macOS
#ifdef __APPLE__

	// Slean trimming required RAM bytes
	#define SLEAN_TRIMMING_REQUIRED_RAM_BYTES (static_cast<uint64_t>(SLEAN_TRIMMING_INITIAL_MAX_NUMBER_OF_EDGES_PER_BUCKET) * SLEAN_TRIMMING_NUMBER_OF_BUCKETS * sizeof(uint32_t) + static_cast<uint64_t>(SLEAN_TRIMMING_AFTER_TRIMMING_ROUND_MAX_NUMBER_OF_EDGES_PER_REMAINING_EDGES_BUCKET) * SLEAN_TRIMMING_NUMBER_OF_REMAINING_EDGES_BUCKETS * sizeof(uint32_t) + SLEAN_TRIMMING_NUMBER_OF_BUCKETS * sizeof(uint32_t) + SLEAN_TRIMMING_NUMBER_OF_REMAINING_EDGES_BUCKETS * sizeof(uint32_t) + NUMBER_OF_EDGES / BITS_IN_A_BYTE * 3)
	
// Otherwise
#else

	// Slean trimming required RAM bytes
	#define SLEAN_TRIMMING_REQUIRED_RAM_BYTES (static_cast<uint64_t>(SLEAN_TRIMMING_INITIAL_MAX_NUMBER_OF_EDGES_PER_BUCKET) * SLEAN_TRIMMING_NUMBER_OF_BUCKETS * sizeof(cl_uint) + static_cast<uint64_t>(SLEAN_TRIMMING_AFTER_TRIMMING_ROUND_MAX_NUMBER_OF_EDGES_PER_REMAINING_EDGES_BUCKET) * SLEAN_TRIMMING_NUMBER_OF_REMAINING_EDGES_BUCKETS * sizeof(cl_uint) + SLEAN_TRIMMING_NUMBER_OF_BUCKETS * sizeof(cl_uint) + SLEAN_TRIMMING_NUMBER_OF_REMAINING_EDGES_BUCKETS * sizeof(cl_uint) + NUMBER_OF_EDGES / BITS_IN_A_BYTE * 3)
#endif

// Slean trimming required work group RAM bytes
#define SLEAN_TRIMMING_REQUIRED_WORK_GROUP_RAM_BYTES SLEAN_TRIMMING_NUMBER_OF_BITMAP_BYTES

// Slean trimming max local buckets size
#define SLEAN_TRIMMING_MAX_LOCAL_BUCKETS_SIZE min(static_cast<uint64_t>(4), (SLEAN_TRIMMING_NUMBER_OF_BUCKETS + 256 - 1) / 256)


// Function prototypes

// Check if using macOS
#ifdef __APPLE__

	// Create slean trimming context
	static inline MTL::Device *createSleanTrimmingContext(const unsigned int deviceIndex) noexcept;
	
	// Perform slean trimming loop
	static inline bool performSleanTrimmingLoop(MTL::Device *device, const unsigned int deviceIndex) noexcept;
	
// Otherwise
#else

	// Create slean trimming context
	static inline cl_context createSleanTrimmingContext(const cl_platform_id platforms[], const cl_uint numberOfPlatforms, const unsigned int deviceIndex) noexcept;
	
	// Perform slean trimming loop
	static inline bool performSleanTrimmingLoop(const cl_context context, const unsigned int deviceIndex) noexcept;
#endif


// Supporting function implementation

// Check if using macOS
#ifdef __APPLE__

	// Create slean trimming context
	MTL::Device *createSleanTrimmingContext(const unsigned int deviceIndex) noexcept {
	
		// TODO
		return nullptr;
		
		// Set index to zero
		unsigned int index = 0;
		
		// Check if getting all devices was successful
		static const unique_ptr<NS::Array, void(*)(NS::Array *)> devices(MTL::CopyAllDevices(), [](NS::Array *devices) noexcept {
		
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
								if(device->recommendedMaxWorkingSetSize() >= SLEAN_TRIMMING_REQUIRED_RAM_BYTES && device->maxThreadgroupMemoryLength() >= SLEAN_TRIMMING_REQUIRED_WORK_GROUP_RAM_BYTES) {
							
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

	// Create slean trimming context
	cl_context createSleanTrimmingContext(const cl_platform_id platforms[], const cl_uint numberOfPlatforms, const unsigned int deviceIndex) noexcept {
	
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
											if(clGetDeviceInfo(devices[j], CL_DEVICE_GLOBAL_MEM_SIZE, sizeof(memorySize), &memorySize, nullptr) == CL_SUCCESS && clGetDeviceInfo(devices[j], CL_DEVICE_LOCAL_MEM_SIZE, sizeof(workGroupMemorySize), &workGroupMemorySize, nullptr) == CL_SUCCESS && memorySize >= SLEAN_TRIMMING_REQUIRED_RAM_BYTES && workGroupMemorySize >= SLEAN_TRIMMING_REQUIRED_WORK_GROUP_RAM_BYTES) {
											
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

	// Perform slean trimming loop
	bool performSleanTrimmingLoop(MTL::Device *device, const unsigned int deviceIndex) noexcept {
	
		// TODO
		
		// Return false
		return false;
	}
	
// Otherwise
#else

	// Perform slean trimming loop
	bool performSleanTrimmingLoop(const cl_context context, const unsigned int deviceIndex) noexcept {
	
		// Check if getting context's device failed
		cl_device_id device;
		if(clGetContextInfo(context, CL_CONTEXT_DEVICES, sizeof(device), &device, nullptr) != CL_SUCCESS) {
		
			// Display message
			cout << "Getting OpenCL context's GPU failed" << endl;
			
			// Return false
			return false;
		}
		
		// Check if getting device's queue properties, max work group size, max memory allocation size, or work group memory size failed, or its max work group size don't exists
		cl_command_queue_properties queueProperties;
		size_t maxWorkGroupSize;
		cl_ulong maxMemoryAllocationSize;
		cl_ulong workGroupMemorySize;
		if(clGetDeviceInfo(device, CL_DEVICE_QUEUE_PROPERTIES, sizeof(queueProperties), &queueProperties, nullptr) != CL_SUCCESS || clGetDeviceInfo(device, CL_DEVICE_MAX_WORK_GROUP_SIZE, sizeof(maxWorkGroupSize), &maxWorkGroupSize, nullptr) != CL_SUCCESS || clGetDeviceInfo(device, CL_DEVICE_MAX_MEM_ALLOC_SIZE, sizeof(maxMemoryAllocationSize), &maxMemoryAllocationSize, nullptr) != CL_SUCCESS || clGetDeviceInfo(device, CL_DEVICE_LOCAL_MEM_SIZE, sizeof(workGroupMemorySize), &workGroupMemorySize, nullptr) != CL_SUCCESS || !maxWorkGroupSize) {
		
			// Display message
			cout << "Getting GPU's info failed" << endl;
			
			// Return false
			return false;
		}
		
		// Check if device index is specified
		if(deviceIndex != ALL_DEVICES) {
		
			// Display message
			cout << "Using GPU for slean trimming" << endl;
		}
		
		// Otherwise
		else {
		
			// Check if getting device's name size failed or its name exists
			size_t nameSize;
			if(clGetDeviceInfo(device, CL_DEVICE_NAME, 0, nullptr, &nameSize) != CL_SUCCESS || !nameSize) {
			
				// Display message
				cout << "Getting GPU's info failed" << endl;
				
				// Return false
				return false;
			}
			
			// Check if getting device's name failed
			char name[nameSize];
			if(clGetDeviceInfo(device, CL_DEVICE_NAME, nameSize, name, nullptr) != CL_SUCCESS) {
			
				// Display message
				cout << "Getting GPU's info failed" << endl;
				
				// Return false
				return false;
			}
			
			// Display message
			cout << "Using " << name << " for slean trimming" << endl;
		}
		
		// Check if device's work group memory isn't fully utilized
		if(bit_floor(workGroupMemorySize) / BYTES_IN_A_KILOBYTE > LOCAL_RAM_KILOBYTES) {
		
			// Display message
			cout << "GPU's local RAM won't be fully utilized. Build this program with LOCAL_RAM_KILOBYTES=" << (bit_floor(workGroupMemorySize) / BYTES_IN_A_KILOBYTE) << " for potentially better performance" << endl;
		}
		
		// Try to allocate more than the max memory allocation size
		cl_int errorCode;
		static unique_ptr<remove_pointer<cl_mem>::type, decltype(&clReleaseMemObject)> moreThanMaxMemoryAllocation(clCreateBuffer(context, CL_MEM_READ_WRITE | CL_MEM_HOST_NO_ACCESS, maxMemoryAllocationSize + 1, nullptr, &errorCode), clReleaseMemObject);
		
		// Set enforce max memory allocation size to if the allocation failed because of an invalid buffer size
		const bool enforceMaxMemoryAllocationSize = errorCode == CL_INVALID_BUFFER_SIZE;
		moreThanMaxMemoryAllocation.reset();
		
		// Check if creating program for the device failed
		const char *source = (
			#include "./slean_trimming.cl"
		);
		const size_t sourceSize = strlen(source);
		static unique_ptr<remove_pointer<cl_program>::type, decltype(&clReleaseProgram)> program(clCreateProgramWithSource(context, 1, &source, &sourceSize, nullptr), clReleaseProgram);
		if(!program) {
		
			// Display message
			cout << "Creating program for the GPU failed" << endl;
			
			// Return false
			return false;
		}
		
		// Set buckets one number of buckets based on if enforcing max memory allocation size
		const size_t bucketsOneNumberOfBuckets = (enforceMaxMemoryAllocationSize && static_cast<uint64_t>(SLEAN_TRIMMING_INITIAL_MAX_NUMBER_OF_EDGES_PER_BUCKET) * SLEAN_TRIMMING_NUMBER_OF_BUCKETS * sizeof(cl_uint) > maxMemoryAllocationSize) ? (SLEAN_TRIMMING_AFTER_TRIMMING_ROUND_MAX_NUMBER_OF_EDGES_PER_BUCKET * SLEAN_TRIMMING_NUMBER_OF_BUCKETS + SLEAN_TRIMMING_INITIAL_MAX_NUMBER_OF_EDGES_PER_BUCKET - 1) / SLEAN_TRIMMING_INITIAL_MAX_NUMBER_OF_EDGES_PER_BUCKET : SLEAN_TRIMMING_NUMBER_OF_BUCKETS;
		
		// Set local buckets size based on the device's work group memory size
		const unsigned int localBucketsSize = min(bit_floor((workGroupMemorySize - (SLEAN_TRIMMING_NUMBER_OF_BUCKETS + sizeof(cl_uint) - 1)) / (sizeof(cl_uint) * SLEAN_TRIMMING_NUMBER_OF_BUCKETS) + 1), SLEAN_TRIMMING_MAX_LOCAL_BUCKETS_SIZE);
		
		// Check if building program for the device failed
		if(clBuildProgram(program.get(), 1, &device, ("-cl-std=CL1.2 -Werror -DEDGE_BITS=" TO_STRING(EDGE_BITS) " -DSLEAN_TRIMMING_PARTS=" TO_STRING(SLEAN_TRIMMING_PARTS) " -DNUMBER_OF_EDGES_PER_STEP_ONE_WORK_ITEM=" + to_string(SLEAN_TRIMMING_NUMBER_OF_EDGES_PER_STEP_ONE_WORK_ITEM) + " -DNUMBER_OF_BITMAP_BYTES=" + to_string(SLEAN_TRIMMING_NUMBER_OF_BITMAP_BYTES) + " -DNUMBER_OF_BUCKETS=" + to_string(SLEAN_TRIMMING_NUMBER_OF_BUCKETS) + " -DNUMBER_OF_LEAST_SIGNIFICANT_BITS_IGNORED_DURING_BUCKET_SORTING=" + to_string(SLEAN_TRIMMING_NUMBER_OF_LEAST_SIGNIFICANT_BITS_IGNORED_DURING_BUCKET_SORTING) + " -DINITIAL_MAX_NUMBER_OF_EDGES_PER_BUCKET=" + to_string(SLEAN_TRIMMING_INITIAL_MAX_NUMBER_OF_EDGES_PER_BUCKET) + " -DAFTER_TRIMMING_ROUND_MAX_NUMBER_OF_EDGES_PER_REMAINING_EDGES_BUCKET=" + to_string(SLEAN_TRIMMING_AFTER_TRIMMING_ROUND_MAX_NUMBER_OF_EDGES_PER_REMAINING_EDGES_BUCKET) + " -DNUMBER_OF_LEAST_SIGNIFICANT_BITS_IGNORED_DURING_REMAINING_EDGES_BUCKET_SORTING=" + to_string(SLEAN_TRIMMING_NUMBER_OF_LEAST_SIGNIFICANT_BITS_IGNORED_DURING_REMAINING_EDGES_BUCKET_SORTING) + " -DNUMBER_OF_REMAINING_EDGES_BITMAP_BYTES=" + to_string(SLEAN_TRIMMING_NUMBER_OF_REMAINING_EDGES_BITMAP_BYTES) + " -DINITIAL_BUCKETS_NUMBER_OF_BUCKETS=" + to_string(bucketsOneNumberOfBuckets) + " -DLOCAL_BUCKETS_SIZE=" + to_string(localBucketsSize)).c_str(), nullptr, nullptr) != CL_SUCCESS) {
		
			// Display message
			cout << "Building program for the GPU failed" << endl;
			
			// Check if getting log size for building the program was successful
			size_t logSize;
			if(clGetProgramBuildInfo(program.get(), device, CL_PROGRAM_BUILD_LOG, 0, nullptr, &logSize) == CL_SUCCESS && logSize) {
			
				// Check if getting log for building the program was successful
				char log[logSize];
				if(clGetProgramBuildInfo(program.get(), device, CL_PROGRAM_BUILD_LOG, logSize, log, nullptr) == CL_SUCCESS) {
				
					// Display log
					cout << log << endl;
				}
			}
			
			// Return false
			return false;
		}
		
		// Check if creating kernels for the device failed
		static unique_ptr<remove_pointer<cl_kernel>::type, decltype(&clReleaseKernel)> stepOneKernel(clCreateKernel(program.get(), "trimEdgesStepOne", nullptr), clReleaseKernel);
		static unique_ptr<remove_pointer<cl_kernel>::type, decltype(&clReleaseKernel)> stepTwoKernel(clCreateKernel(program.get(), "trimEdgesStepTwo", nullptr), clReleaseKernel);
		static unique_ptr<remove_pointer<cl_kernel>::type, decltype(&clReleaseKernel)> stepThreeKernel(clCreateKernel(program.get(), "trimEdgesStepThree", nullptr), clReleaseKernel);
		static unique_ptr<remove_pointer<cl_kernel>::type, decltype(&clReleaseKernel)> stepFourKernel(clCreateKernel(program.get(), "trimEdgesStepFour", nullptr), clReleaseKernel);
		static unique_ptr<remove_pointer<cl_kernel>::type, decltype(&clReleaseKernel)> stepFiveKernel(clCreateKernel(program.get(), "trimEdgesStepFive", nullptr), clReleaseKernel);
		static unique_ptr<remove_pointer<cl_kernel>::type, decltype(&clReleaseKernel)> stepSixKernel(clCreateKernel(program.get(), "trimEdgesStepSix", nullptr), clReleaseKernel);
		if(!stepOneKernel || !stepTwoKernel || !stepThreeKernel || !stepFourKernel || !stepFiveKernel || !stepSixKernel) {
		
			// Display message
			cout << "Creating kernels for the GPU failed" << endl;
			
			// Return false
			return false;
		}
		
		// Set total number of work items based on the kernels and work items per work group
		const size_t totalNumberOfWorkItems[] = {
		
			// Trim edges step one kernel
			(NUMBER_OF_EDGES / SLEAN_TRIMMING_PARTS) / SLEAN_TRIMMING_NUMBER_OF_EDGES_PER_STEP_ONE_WORK_ITEM,
			
			// Trim edges step two kernel
			SLEAN_TRIMMING_NUMBER_OF_BUCKETS * min(bit_floor(maxWorkGroupSize), bit_floor(static_cast<size_t>(INT16_MAX / 2))),
			
			// Trim edges step three kernel
			SLEAN_TRIMMING_NUMBER_OF_BUCKETS * min(bit_floor(maxWorkGroupSize), bit_floor(static_cast<size_t>(INT16_MAX / 2))),
			
			// Trim edges step four kernel
			SLEAN_TRIMMING_NUMBER_OF_BUCKETS * min(bit_floor(maxWorkGroupSize), bit_floor(static_cast<size_t>(INT16_MAX / 2))),
			
			// Trim edges step five kernel
			SLEAN_TRIMMING_NUMBER_OF_REMAINING_EDGES_BUCKETS * min(bit_floor(maxWorkGroupSize), bit_floor(static_cast<size_t>(INT16_MAX / 2))),
			
			// Trim edges step six kernel
			SLEAN_TRIMMING_NUMBER_OF_BUCKETS * min(bit_floor(maxWorkGroupSize), bit_floor(static_cast<size_t>(INT16_MAX / 2)))
		};
		
		// Set work items per work group based on the total number of work items and max work group size
		const size_t workItemsPerWorkGroup[] = {
		
			// Trim edges step one kernel
			min(min(bit_floor(min(static_cast<size_t>(SLEAN_TRIMMING_WORK_ITEMS_PER_WORK_GROUP_STEP_ONE), maxWorkGroupSize)), bit_floor(static_cast<size_t>(INT16_MAX / 2))), totalNumberOfWorkItems[0]),
			
			// Trim edges step two kernel
			min(bit_floor(maxWorkGroupSize), bit_floor(static_cast<size_t>(INT16_MAX / 2))),
			
			// Trim edges step three kernel
			min(bit_floor(maxWorkGroupSize), bit_floor(static_cast<size_t>(INT16_MAX / 2))),
			
			// Trim edges step four kernel
			min(bit_floor(maxWorkGroupSize), bit_floor(static_cast<size_t>(INT16_MAX / 2))),
			
			// Trim edges step five kernel
			min(bit_floor(maxWorkGroupSize), bit_floor(static_cast<size_t>(INT16_MAX / 2))),
			
			// Trim edges step six kernel
			min(bit_floor(maxWorkGroupSize), bit_floor(static_cast<size_t>(INT16_MAX / 2)))
		};
		
		// Free kernels
		stepOneKernel.reset();
		stepTwoKernel.reset();
		stepThreeKernel.reset();
		stepFourKernel.reset();
		stepFiveKernel.reset();
		stepSixKernel.reset();
		
		// Check if recreating program for the device failed
		program = unique_ptr<remove_pointer<cl_program>::type, decltype(&clReleaseProgram)>(clCreateProgramWithSource(context, 1, &source, &sourceSize, nullptr), clReleaseProgram);
		if(!program) {
		
			// Display message
			cout << "Creating program for the GPU failed" << endl;
			
			// Return false
			return false;
		}
		
		// Check if rebuilding program for the device with hardcoded work items per work groups failed
		if(clBuildProgram(program.get(), 1, &device, ("-cl-std=CL1.2 -Werror -DEDGE_BITS=" TO_STRING(EDGE_BITS) " -DSLEAN_TRIMMING_PARTS=" TO_STRING(SLEAN_TRIMMING_PARTS) " -DNUMBER_OF_EDGES_PER_STEP_ONE_WORK_ITEM=" + to_string(SLEAN_TRIMMING_NUMBER_OF_EDGES_PER_STEP_ONE_WORK_ITEM) + " -DNUMBER_OF_BITMAP_BYTES=" + to_string(SLEAN_TRIMMING_NUMBER_OF_BITMAP_BYTES) + " -DNUMBER_OF_BUCKETS=" + to_string(SLEAN_TRIMMING_NUMBER_OF_BUCKETS) + " -DNUMBER_OF_LEAST_SIGNIFICANT_BITS_IGNORED_DURING_BUCKET_SORTING=" + to_string(SLEAN_TRIMMING_NUMBER_OF_LEAST_SIGNIFICANT_BITS_IGNORED_DURING_BUCKET_SORTING) + " -DINITIAL_MAX_NUMBER_OF_EDGES_PER_BUCKET=" + to_string(SLEAN_TRIMMING_INITIAL_MAX_NUMBER_OF_EDGES_PER_BUCKET) + " -DAFTER_TRIMMING_ROUND_MAX_NUMBER_OF_EDGES_PER_REMAINING_EDGES_BUCKET=" + to_string(SLEAN_TRIMMING_AFTER_TRIMMING_ROUND_MAX_NUMBER_OF_EDGES_PER_REMAINING_EDGES_BUCKET) + " -DNUMBER_OF_LEAST_SIGNIFICANT_BITS_IGNORED_DURING_REMAINING_EDGES_BUCKET_SORTING=" + to_string(SLEAN_TRIMMING_NUMBER_OF_LEAST_SIGNIFICANT_BITS_IGNORED_DURING_REMAINING_EDGES_BUCKET_SORTING) + " -DNUMBER_OF_REMAINING_EDGES_BITMAP_BYTES=" + to_string(SLEAN_TRIMMING_NUMBER_OF_REMAINING_EDGES_BITMAP_BYTES) + " -DINITIAL_BUCKETS_NUMBER_OF_BUCKETS=" + to_string(bucketsOneNumberOfBuckets) + " -DLOCAL_BUCKETS_SIZE=" + to_string(localBucketsSize) + " -DTRIM_EDGES_STEP_ONE_WORK_ITEMS_PER_WORK_GROUP=" + to_string(workItemsPerWorkGroup[0]) + " -DTRIM_EDGES_STEP_TWO_WORK_ITEMS_PER_WORK_GROUP=" + to_string(workItemsPerWorkGroup[1]) + " -DTRIM_EDGES_STEP_THREE_WORK_ITEMS_PER_WORK_GROUP=" + to_string(workItemsPerWorkGroup[2]) + " -DTRIM_EDGES_STEP_FOUR_WORK_ITEMS_PER_WORK_GROUP=" + to_string(workItemsPerWorkGroup[3]) + " -DTRIM_EDGES_STEP_FIVE_WORK_ITEMS_PER_WORK_GROUP=" + to_string(workItemsPerWorkGroup[4]) + " -DTRIM_EDGES_STEP_SIX_WORK_ITEMS_PER_WORK_GROUP=" + to_string(workItemsPerWorkGroup[5])).c_str(), nullptr, nullptr) != CL_SUCCESS) {
		
			// Display message
			cout << "Building program for the GPU failed" << endl;
			
			// Check if getting log size for building the program was successful
			size_t logSize;
			if(clGetProgramBuildInfo(program.get(), device, CL_PROGRAM_BUILD_LOG, 0, nullptr, &logSize) == CL_SUCCESS && logSize) {
			
				// Check if getting log for building the program was successful
				char log[logSize];
				if(clGetProgramBuildInfo(program.get(), device, CL_PROGRAM_BUILD_LOG, logSize, log, nullptr) == CL_SUCCESS) {
				
					// Display log
					cout << log << endl;
				}
			}
			
			// Return false
			return false;
		}
		
		// Check if recreating kernels for the device failed
		stepOneKernel = unique_ptr<remove_pointer<cl_kernel>::type, decltype(&clReleaseKernel)>(clCreateKernel(program.get(), "trimEdgesStepOne", nullptr), clReleaseKernel);
		stepTwoKernel = unique_ptr<remove_pointer<cl_kernel>::type, decltype(&clReleaseKernel)>(clCreateKernel(program.get(), "trimEdgesStepTwo", nullptr), clReleaseKernel);
		stepThreeKernel = unique_ptr<remove_pointer<cl_kernel>::type, decltype(&clReleaseKernel)>(clCreateKernel(program.get(), "trimEdgesStepThree", nullptr), clReleaseKernel);
		stepFourKernel = unique_ptr<remove_pointer<cl_kernel>::type, decltype(&clReleaseKernel)>(clCreateKernel(program.get(), "trimEdgesStepFour", nullptr), clReleaseKernel);
		stepFiveKernel = unique_ptr<remove_pointer<cl_kernel>::type, decltype(&clReleaseKernel)>(clCreateKernel(program.get(), "trimEdgesStepFive", nullptr), clReleaseKernel);
		stepSixKernel = unique_ptr<remove_pointer<cl_kernel>::type, decltype(&clReleaseKernel)>(clCreateKernel(program.get(), "trimEdgesStepSix", nullptr), clReleaseKernel);
		if(!stepOneKernel || !stepTwoKernel || !stepThreeKernel || !stepFourKernel || !stepFiveKernel || !stepSixKernel) {
		
			// Display message
			cout << "Creating kernels for the GPU failed" << endl;
			
			// Return false
			return false;
		}
		
		// Check if allocating memory on the device failed
		static const unique_ptr<remove_pointer<cl_mem>::type, decltype(&clReleaseMemObject)> bucketsOne(clCreateBuffer(context, CL_MEM_READ_WRITE | CL_MEM_HOST_NO_ACCESS, static_cast<uint64_t>(SLEAN_TRIMMING_INITIAL_MAX_NUMBER_OF_EDGES_PER_BUCKET) * bucketsOneNumberOfBuckets * sizeof(cl_uint), nullptr, nullptr), clReleaseMemObject);
		static const unique_ptr<remove_pointer<cl_mem>::type, decltype(&clReleaseMemObject)> bucketsOneSecondPart(clCreateBuffer(context, CL_MEM_READ_WRITE | CL_MEM_HOST_NO_ACCESS, static_cast<uint64_t>(SLEAN_TRIMMING_INITIAL_MAX_NUMBER_OF_EDGES_PER_BUCKET) * (SLEAN_TRIMMING_NUMBER_OF_BUCKETS - bucketsOneNumberOfBuckets) * sizeof(cl_uint), nullptr, nullptr), clReleaseMemObject);
		static const unique_ptr<remove_pointer<cl_mem>::type, decltype(&clReleaseMemObject)> bucketsTwo(clCreateBuffer(context, CL_MEM_READ_WRITE | CL_MEM_HOST_NO_ACCESS, static_cast<uint64_t>(SLEAN_TRIMMING_AFTER_TRIMMING_ROUND_MAX_NUMBER_OF_EDGES_PER_REMAINING_EDGES_BUCKET) * SLEAN_TRIMMING_NUMBER_OF_REMAINING_EDGES_BUCKETS * sizeof(cl_uint), nullptr, nullptr), clReleaseMemObject);
		static const unique_ptr<remove_pointer<cl_mem>::type, decltype(&clReleaseMemObject)> numberOfEdgesPerBucketOne(clCreateBuffer(context, CL_MEM_READ_WRITE | CL_MEM_HOST_NO_ACCESS, SLEAN_TRIMMING_NUMBER_OF_BUCKETS * sizeof(cl_uint), nullptr, nullptr), clReleaseMemObject);
		static const unique_ptr<remove_pointer<cl_mem>::type, decltype(&clReleaseMemObject)> numberOfEdgesPerBucketTwo(clCreateBuffer(context, CL_MEM_READ_WRITE | CL_MEM_HOST_NO_ACCESS, SLEAN_TRIMMING_NUMBER_OF_REMAINING_EDGES_BUCKETS * sizeof(cl_uint), nullptr, nullptr), clReleaseMemObject);
		static const unique_ptr<remove_pointer<cl_mem>::type, decltype(&clReleaseMemObject)> edgesBitmapOne(clCreateBuffer(context, CL_MEM_READ_WRITE | CL_MEM_HOST_READ_ONLY | CL_MEM_ALLOC_HOST_PTR, NUMBER_OF_EDGES / BITS_IN_A_BYTE, nullptr, nullptr), clReleaseMemObject);
		static const unique_ptr<remove_pointer<cl_mem>::type, decltype(&clReleaseMemObject)> edgesBitmapTwo(clCreateBuffer(context, CL_MEM_READ_WRITE | CL_MEM_HOST_READ_ONLY | CL_MEM_ALLOC_HOST_PTR, NUMBER_OF_EDGES / BITS_IN_A_BYTE, nullptr, nullptr), clReleaseMemObject);
		static const unique_ptr<remove_pointer<cl_mem>::type, decltype(&clReleaseMemObject)> nodesBitmap(clCreateBuffer(context, CL_MEM_READ_WRITE | CL_MEM_HOST_NO_ACCESS, NUMBER_OF_EDGES / BITS_IN_A_BYTE, nullptr, nullptr), clReleaseMemObject);
		if(!bucketsOne || (bucketsOneNumberOfBuckets != SLEAN_TRIMMING_NUMBER_OF_BUCKETS && !bucketsOneSecondPart) || !bucketsTwo || !numberOfEdgesPerBucketOne || !numberOfEdgesPerBucketTwo || !edgesBitmapOne || !edgesBitmapTwo || !nodesBitmap) {
		
			// Display message
			cout << "Allocating memory on the GPU failed" << endl;
			
			// Return false
			return false;
		}
		
		// Check if creating command queue for the device failed
		static const unique_ptr<remove_pointer<cl_command_queue>::type, decltype(&clReleaseCommandQueue)> commandQueue(clCreateCommandQueue(context, device, queueProperties & (CL_QUEUE_OUT_OF_ORDER_EXEC_MODE_ENABLE | CL_QUEUE_PROFILING_ENABLE), nullptr), clReleaseCommandQueue);
		if(!commandQueue) {
		
			// Display message
			cout << "Creating command queue for the GPU failed" << endl;
			
			// Return false
			return false;
		}
		
		// Check if setting program's unchanging arguments failed
		if(clSetKernelArg(stepOneKernel.get(), 0, sizeof(bucketsOne.get()), &unmove(bucketsOne.get())) != CL_SUCCESS || clSetKernelArg(stepOneKernel.get(), 1, sizeof(numberOfEdgesPerBucketOne.get()), &unmove(numberOfEdgesPerBucketOne.get())) != CL_SUCCESS || clSetKernelArg(stepTwoKernel.get(), 0, sizeof(bucketsOne.get()), &unmove(bucketsOne.get())) != CL_SUCCESS || clSetKernelArg(stepTwoKernel.get(), 1, sizeof(numberOfEdgesPerBucketOne.get()), &unmove(numberOfEdgesPerBucketOne.get())) != CL_SUCCESS || clSetKernelArg(stepTwoKernel.get(), 2, sizeof(nodesBitmap.get()), &unmove(nodesBitmap.get())) != CL_SUCCESS || clSetKernelArg(stepThreeKernel.get(), 0, sizeof(bucketsOne.get()), &unmove(bucketsOne.get())) != CL_SUCCESS || clSetKernelArg(stepThreeKernel.get(), 1, sizeof(numberOfEdgesPerBucketOne.get()), &unmove(numberOfEdgesPerBucketOne.get())) != CL_SUCCESS || clSetKernelArg(stepThreeKernel.get(), 2, sizeof(nodesBitmap.get()), &unmove(nodesBitmap.get())) != CL_SUCCESS || clSetKernelArg(stepFourKernel.get(), 0, sizeof(bucketsOne.get()), &unmove(bucketsOne.get())) != CL_SUCCESS || clSetKernelArg(stepFourKernel.get(), 1, sizeof(numberOfEdgesPerBucketOne.get()), &unmove(numberOfEdgesPerBucketOne.get())) != CL_SUCCESS || clSetKernelArg(stepFourKernel.get(), 2, sizeof(nodesBitmap.get()), &unmove(nodesBitmap.get())) != CL_SUCCESS || clSetKernelArg(stepFourKernel.get(), 3, sizeof(bucketsTwo.get()), &unmove(bucketsTwo.get())) != CL_SUCCESS || clSetKernelArg(stepFourKernel.get(), 4, sizeof(numberOfEdgesPerBucketTwo.get()), &unmove(numberOfEdgesPerBucketTwo.get())) != CL_SUCCESS || clSetKernelArg(stepFiveKernel.get(), 0, sizeof(bucketsTwo.get()), &unmove(bucketsTwo.get())) != CL_SUCCESS || clSetKernelArg(stepFiveKernel.get(), 1, sizeof(numberOfEdgesPerBucketTwo.get()), &unmove(numberOfEdgesPerBucketTwo.get())) != CL_SUCCESS || clSetKernelArg(stepSixKernel.get(), 0, sizeof(bucketsOne.get()), &unmove(bucketsOne.get())) != CL_SUCCESS || clSetKernelArg(stepSixKernel.get(), 1, sizeof(numberOfEdgesPerBucketOne.get()), &unmove(numberOfEdgesPerBucketOne.get())) != CL_SUCCESS || clSetKernelArg(stepSixKernel.get(), 2, sizeof(nodesBitmap.get()), &unmove(nodesBitmap.get())) != CL_SUCCESS || clSetKernelArg(stepSixKernel.get(), 3, sizeof(bucketsTwo.get()), &unmove(bucketsTwo.get())) != CL_SUCCESS || clSetKernelArg(stepSixKernel.get(), 4, sizeof(numberOfEdgesPerBucketTwo.get()), &unmove(numberOfEdgesPerBucketTwo.get())) != CL_SUCCESS) {
		
			// Display message
			cout << "Setting program's arguments on the GPU failed" << endl;
			
			// Return false
			return false;
		}
		
		// Check if using buckets one second part
		if(bucketsOneNumberOfBuckets != SLEAN_TRIMMING_NUMBER_OF_BUCKETS) {
		
			// Check if setting program's unchanging arguments failed
			if(clSetKernelArg(stepOneKernel.get(), 4, sizeof(bucketsOneSecondPart.get()), &unmove(bucketsOneSecondPart.get())) != CL_SUCCESS || clSetKernelArg(stepTwoKernel.get(), 4, sizeof(bucketsOneSecondPart.get()), &unmove(bucketsOneSecondPart.get())) != CL_SUCCESS || clSetKernelArg(stepThreeKernel.get(), 4, sizeof(bucketsOneSecondPart.get()), &unmove(bucketsOneSecondPart.get())) != CL_SUCCESS || clSetKernelArg(stepFourKernel.get(), 6, sizeof(bucketsOneSecondPart.get()), &unmove(bucketsOneSecondPart.get())) != CL_SUCCESS || clSetKernelArg(stepSixKernel.get(), 7, sizeof(bucketsOneSecondPart.get()), &unmove(bucketsOneSecondPart.get())) != CL_SUCCESS) {
			
				// Display message
				cout << "Setting program's arguments on the GPU failed" << endl;
				
				// Return false
				return false;
			}
		}
		
		// Check if queuing clearing number of edges per bucket one on the device failed
		static Event clearNumberOfEdgesPerBucketEvents[SLEAN_TRIMMING_PARTS * 3 - 1];
		if(clEnqueueFillBuffer(commandQueue.get(), numberOfEdgesPerBucketOne.get(), (const cl_ulong[]){0}, (SLEAN_TRIMMING_NUMBER_OF_BUCKETS == 1) ? sizeof(cl_uint) : sizeof(cl_ulong), 0, SLEAN_TRIMMING_NUMBER_OF_BUCKETS * sizeof(cl_uint), 0, nullptr, clearNumberOfEdgesPerBucketEvents[0].getAddress()) != CL_SUCCESS) {
		
			// Display message
			cout << "Preparing program's arguments on the GPU failed" << endl;
			
			// Return false
			return false;
		}
		
		// Get SipHash keys from job's header and nonce
		uint64_t __attribute__((vector_size(sizeof(uint64_t) * SIPHASH_KEYS_SIZE))) sipHashKeysOne;
		uint64_t heightOne = jobHeight;
		uint64_t idOne = jobId;
		uint64_t nonceOne = jobNonce++;
		blake2b(sipHashKeysOne, jobHeader, nonceOne);
		
		// Check if setting program's part and SipHash keys arguments failed
		if(clSetKernelArg(stepOneKernel.get(), 2, sizeof(cl_uchar), &unmove(static_cast<cl_uchar>(0))) != CL_SUCCESS || clSetKernelArg(stepOneKernel.get(), 3, sizeof(sipHashKeysOne), &sipHashKeysOne) != CL_SUCCESS) {
		
			// Display message
			cout << "Setting program's arguments on the GPU failed" << endl;
			
			// Return false
			return false;
		}
		
		// Check if queuing running step one on the device failed
		static Event runStepEvents[SLEAN_TRIMMING_PARTS * 5 - 2];
		if(clEnqueueNDRangeKernel(commandQueue.get(), stepOneKernel.get(), 1, nullptr, &totalNumberOfWorkItems[0], &workItemsPerWorkGroup[0], 1, clearNumberOfEdgesPerBucketEvents[0].getAddress(), runStepEvents[0].getAddress()) != CL_SUCCESS) {
		
			// Display message
			cout << "Running program on the GPU failed" << endl;
			
			// Return false
			return false;
		}
		
		// Check if setting program's SipHash keys argument failed
		if(clSetKernelArg(stepTwoKernel.get(), 3, sizeof(sipHashKeysOne), &sipHashKeysOne) != CL_SUCCESS) {
		
			// Display message
			cout << "Setting program's arguments on the GPU failed" << endl;
			
			// Return false
			return false;
		}
		
		// Check if queuing running step two on the device failed
		if(clEnqueueNDRangeKernel(commandQueue.get(), stepTwoKernel.get(), 1, nullptr, &totalNumberOfWorkItems[1], &workItemsPerWorkGroup[1], 1, runStepEvents[0].getAddress(), runStepEvents[1].getAddress()) != CL_SUCCESS) {
		
			// Display message
			cout << "Running program on the GPU failed" << endl;
			
			// Return false
			return false;
		}
		
		// Check if queuing clearing number of edges per bucket two on the device failed
		if(clEnqueueFillBuffer(commandQueue.get(), numberOfEdgesPerBucketTwo.get(), (const cl_ulong[]){0}, (SLEAN_TRIMMING_NUMBER_OF_REMAINING_EDGES_BUCKETS == 1) ? sizeof(cl_uint) : sizeof(cl_ulong), 0, SLEAN_TRIMMING_NUMBER_OF_REMAINING_EDGES_BUCKETS * sizeof(cl_uint), 0, nullptr, clearNumberOfEdgesPerBucketEvents[1].getAddress()) != CL_SUCCESS) {
		
			// Display message
			cout << "Preparing program's arguments on the GPU failed" << endl;
			
			// Return false
			return false;
		}
		
		// Check if setting program's SipHash keys argument failed
		if(clSetKernelArg(stepThreeKernel.get(), 3, sizeof(sipHashKeysOne), &sipHashKeysOne) != CL_SUCCESS) {
		
			// Display message
			cout << "Setting program's arguments on the GPU failed" << endl;
			
			// Return false
			return false;
		}
		
		// Go through all remaining trimming parts
		for(unsigned int i = 1; i < SLEAN_TRIMMING_PARTS; ++i) {
		
			// Check if queuing clearing number of edges per bucket one on the device failed
			if(clEnqueueFillBuffer(commandQueue.get(), numberOfEdgesPerBucketOne.get(), (const cl_ulong[]){0}, (SLEAN_TRIMMING_NUMBER_OF_BUCKETS == 1) ? sizeof(cl_uint) : sizeof(cl_ulong), 0, SLEAN_TRIMMING_NUMBER_OF_BUCKETS * sizeof(cl_uint), 1, runStepEvents[(i - 1) * 2 + 1].getAddress(), clearNumberOfEdgesPerBucketEvents[i + 1].getAddress()) != CL_SUCCESS) {
			
				// Display message
				cout << "Preparing program's arguments on the GPU failed" << endl;
				
				// Return false
				return false;
			}
			
			// Check if setting program's part argument failed
			if(clSetKernelArg(stepOneKernel.get(), 2, sizeof(cl_uchar), &unmove(static_cast<cl_uchar>(i))) != CL_SUCCESS) {
			
				// Display message
				cout << "Setting program's arguments on the GPU failed" << endl;
				
				// Return false
				return false;
			}
			
			// Check if queuing running step one on the device failed
			if(clEnqueueNDRangeKernel(commandQueue.get(), stepOneKernel.get(), 1, nullptr, &totalNumberOfWorkItems[0], &workItemsPerWorkGroup[0], 1, clearNumberOfEdgesPerBucketEvents[i + 1].getAddress(), runStepEvents[(i - 1) * 2 + 2].getAddress()) != CL_SUCCESS) {
			
				// Display message
				cout << "Running program on the GPU failed" << endl;
				
				// Return false
				return false;
			}
			
			// Check if not the last trimming part
			if(i != SLEAN_TRIMMING_PARTS - 1) {
			
				// Check if queuing running step three on the device failed
				if(clEnqueueNDRangeKernel(commandQueue.get(), stepThreeKernel.get(), 1, nullptr, &totalNumberOfWorkItems[2], &workItemsPerWorkGroup[2], 1, runStepEvents[(i - 1) * 2 + 2].getAddress(), runStepEvents[(i - 1) * 2 + 3].getAddress()) != CL_SUCCESS) {
				
					// Display message
					cout << "Running program on the GPU failed" << endl;
					
					// Return false
					return false;
				}
			}
			
			// Otherwise
			else {
			
				// Check if setting program's SipHash keys argument failed
				if(clSetKernelArg(stepFourKernel.get(), 5, sizeof(sipHashKeysOne), &sipHashKeysOne) != CL_SUCCESS) {
				
					// Display message
					cout << "Setting program's arguments on the GPU failed" << endl;
					
					// Return false
					return false;
				}
				
				// Check if queuing running step four on the device failed
				if(clEnqueueNDRangeKernel(commandQueue.get(), stepFourKernel.get(), 1, nullptr, &totalNumberOfWorkItems[3], &workItemsPerWorkGroup[3], 2, unmove((const cl_event []){runStepEvents[(i - 1) * 2 + 2].get(), clearNumberOfEdgesPerBucketEvents[1].get()}), runStepEvents[(i - 1) * 2 + 3].getAddress()) != CL_SUCCESS) {
				
					// Display message
					cout << "Running program on the GPU failed" << endl;
					
					// Return false
					return false;
				}
			}
		}
		
		// Check if setting program's edges bitmap or part arguments failed
		if(clSetKernelArg(stepFiveKernel.get(), 2, sizeof(edgesBitmapOne.get()), &unmove(edgesBitmapOne.get())) != CL_SUCCESS || clSetKernelArg(stepFiveKernel.get(), 3, sizeof(cl_uchar), &unmove(static_cast<cl_uchar>(SLEAN_TRIMMING_PARTS - 1))) != CL_SUCCESS) {
		
			// Display message
			cout << "Setting program's arguments on the GPU failed" << endl;
			
			// Return false
			return false;
		}
		
		// Check if queuing running step five on the device failed
		if(clEnqueueNDRangeKernel(commandQueue.get(), stepFiveKernel.get(), 1, nullptr, &totalNumberOfWorkItems[4], &workItemsPerWorkGroup[4], 1, runStepEvents[SLEAN_TRIMMING_PARTS * 2 - 1].getAddress(), runStepEvents[SLEAN_TRIMMING_PARTS * 2].getAddress()) != CL_SUCCESS) {
		
			// Display message
			cout << "Running program on the GPU failed" << endl;
			
			// Return false
			return false;
		}
		
		// Check if setting program's SipHash keys argument failed
		if(clSetKernelArg(stepSixKernel.get(), 6, sizeof(sipHashKeysOne), &sipHashKeysOne) != CL_SUCCESS) {
		
			// Display message
			cout << "Setting program's arguments on the GPU failed" << endl;
			
			// Return false
			return false;
		}
		
		// Go through all remaining trimming parts except the last one
		for(unsigned int i = 0; i < SLEAN_TRIMMING_PARTS - 1; ++i) {
		
			// Check if queuing clearing number of edges per bucket one on the device failed
			if(clEnqueueFillBuffer(commandQueue.get(), numberOfEdgesPerBucketOne.get(), (const cl_ulong[]){0}, (SLEAN_TRIMMING_NUMBER_OF_BUCKETS == 1) ? sizeof(cl_uint) : sizeof(cl_ulong), 0, SLEAN_TRIMMING_NUMBER_OF_BUCKETS * sizeof(cl_uint), 1, runStepEvents[i * 3 + SLEAN_TRIMMING_PARTS * 2 - 1].getAddress(), clearNumberOfEdgesPerBucketEvents[i * 2 + SLEAN_TRIMMING_PARTS + 1].getAddress()) != CL_SUCCESS) {
			
				// Display message
				cout << "Preparing program's arguments on the GPU failed" << endl;
				
				// Return false
				return false;
			}
			
			// Check if setting program's part argument failed
			if(clSetKernelArg(stepOneKernel.get(), 2, sizeof(cl_uchar), &unmove(static_cast<cl_uchar>(i))) != CL_SUCCESS) {
			
				// Display message
				cout << "Setting program's arguments on the GPU failed" << endl;
				
				// Return false
				return false;
			}
			
			// Check if queuing running step one on the device failed
			if(clEnqueueNDRangeKernel(commandQueue.get(), stepOneKernel.get(), 1, nullptr, &totalNumberOfWorkItems[0], &workItemsPerWorkGroup[0], 1, clearNumberOfEdgesPerBucketEvents[i * 2 + SLEAN_TRIMMING_PARTS + 1].getAddress(), runStepEvents[i * 3 + SLEAN_TRIMMING_PARTS * 2 + 1].getAddress()) != CL_SUCCESS) {
			
				// Display message
				cout << "Running program on the GPU failed" << endl;
				
				// Return false
				return false;
			}
			
			// Check if queuing clearing number of edges per bucket two on the device failed
			if(clEnqueueFillBuffer(commandQueue.get(), numberOfEdgesPerBucketTwo.get(), (const cl_ulong[]){0}, (SLEAN_TRIMMING_NUMBER_OF_REMAINING_EDGES_BUCKETS == 1) ? sizeof(cl_uint) : sizeof(cl_ulong), 0, SLEAN_TRIMMING_NUMBER_OF_REMAINING_EDGES_BUCKETS * sizeof(cl_uint), 1, runStepEvents[i * 3 + SLEAN_TRIMMING_PARTS * 2].getAddress(), clearNumberOfEdgesPerBucketEvents[i * 2 + SLEAN_TRIMMING_PARTS + 2].getAddress()) != CL_SUCCESS) {
			
				// Display message
				cout << "Preparing program's arguments on the GPU failed" << endl;
				
				// Return false
				return false;
			}
			
			// Check if setting program's part argument failed
			if(clSetKernelArg(stepSixKernel.get(), 5, sizeof(cl_uchar), &unmove(static_cast<cl_uchar>(i))) != CL_SUCCESS) {
			
				// Display message
				cout << "Setting program's arguments on the GPU failed" << endl;
				
				// Return false
				return false;
			}
			
			// Check if queuing running step six on the device failed
			if(clEnqueueNDRangeKernel(commandQueue.get(), stepSixKernel.get(), 1, nullptr, &totalNumberOfWorkItems[5], &workItemsPerWorkGroup[5], 2, unmove((const cl_event []){runStepEvents[i * 3 + SLEAN_TRIMMING_PARTS * 2 + 1].get(), clearNumberOfEdgesPerBucketEvents[i * 2 + SLEAN_TRIMMING_PARTS + 2].get()}), runStepEvents[i * 3 + SLEAN_TRIMMING_PARTS * 2 + 2].getAddress()) != CL_SUCCESS) {
			
				// Display message
				cout << "Running program on the GPU failed" << endl;
				
				// Return false
				return false;
			}
			
			// Check if setting program's part argument failed
			if(clSetKernelArg(stepFiveKernel.get(), 3, sizeof(cl_uchar), &unmove(static_cast<cl_uchar>(i))) != CL_SUCCESS) {
			
				// Display message
				cout << "Setting program's arguments on the GPU failed" << endl;
				
				// Return false
				return false;
			}
			
			// Check if queuing running step five on the device failed
			if(clEnqueueNDRangeKernel(commandQueue.get(), stepFiveKernel.get(), 1, nullptr, &totalNumberOfWorkItems[4], &workItemsPerWorkGroup[4], 1, runStepEvents[i * 3 + SLEAN_TRIMMING_PARTS * 2 + 2].getAddress(), runStepEvents[i * 3 + SLEAN_TRIMMING_PARTS * 2 + 3].getAddress()) != CL_SUCCESS) {
			
				// Display message
				cout << "Running program on the GPU failed" << endl;
				
				// Return false
				return false;
			}
		}
		
		// Check if queuing map result failed
		static Event mapEvent;
		static uint64_t *resultOne = reinterpret_cast<uint64_t *>(clEnqueueMapBuffer(commandQueue.get(), edgesBitmapOne.get(), CL_FALSE, CL_MAP_READ, 0, NUMBER_OF_EDGES / BITS_IN_A_BYTE, 1, runStepEvents[SLEAN_TRIMMING_PARTS * 5 - 3].getAddress(), mapEvent.getAddress(), nullptr));
		if(!resultOne) {
		
			// Display message
			cout << "Getting result from the GPU failed" << endl;
			
			// Return false
			return false;
		}
		
		// Automatically unmap result one when done
		static const unique_ptr<uint64_t, void(*)(uint64_t *)> resultOneUniquePointer(resultOne, [](__attribute__((unused)) uint64_t *unused) noexcept {
		
			// Check if result one exists
			if(resultOne) {
			
				// Queue unmapping result one
				Event unmapEvent;
				clEnqueueUnmapMemObject(commandQueue.get(), edgesBitmapOne.get(), reinterpret_cast<void *>(resultOne), mapEvent.get() ? 1 : 0, mapEvent.getAddress(), unmapEvent.getAddress());
			}
		});
		
		// Check if closing
		if(closing) {
			
			// Return true
			return true;
		}
		
		// Check if waiting for map event to finish failed
		if(clWaitForEvents(1, mapEvent.getAddress()) != CL_SUCCESS) {
		
			// Display message
			cout << "Getting result from the GPU failed" << endl;
			
			// Return false
			return false;
		}
		
		// Check if getting trimming time failed
		cl_ulong startTime;
		cl_ulong endTime;
		if(clGetEventProfilingInfo(clearNumberOfEdgesPerBucketEvents[0].get(), CL_PROFILING_COMMAND_QUEUED, sizeof(startTime), &startTime, nullptr) != CL_SUCCESS || clGetEventProfilingInfo(mapEvent.get(), CL_PROFILING_COMMAND_END, sizeof(endTime), &endTime, nullptr) != CL_SUCCESS) {
		
			// Display message
			cout << "Getting GPU trimming time failed" << endl;
			
			// Return false
			return false;
		}
		
		// Display message
		cout << endl << "Pipeline stages:" << endl << "Trimming:\t" << static_cast<chrono::duration<double>>(static_cast<chrono::nanoseconds>(endTime - startTime)).count() << " second(s)" << endl;
		
		uint64_t remainingEdges = 0;
		for(uint32_t i = 0; i < (NUMBER_OF_EDGES / BITS_IN_A_BYTE) / sizeof(uint64_t); ++i) {
			remainingEdges += __builtin_popcountll(resultOne[i]);
		}
		cout << remainingEdges << endl;
		
		// TODO
		
		// Return false
		return false;
	}
#endif


#endif

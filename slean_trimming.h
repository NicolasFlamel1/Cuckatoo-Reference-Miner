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

// Slean trimming initial max number of edges per bucket (Divide by 4 and multiply by 4 makes the result a product of 4)
#define SLEAN_TRIMMING_INITIAL_MAX_NUMBER_OF_EDGES_PER_BUCKET ((static_cast<uint32_t>(SLEAN_TRIMMING_NUMBER_OF_ITEMS_PER_BUCKET * 1.05) / 4) * 4)

// Slean trimming after trimming round max number of edges per bucket (Divide by 4 and multiply by 4 makes the result a product of 4)
#define SLEAN_TRIMMING_AFTER_TRIMMING_ROUND_MAX_NUMBER_OF_EDGES_PER_BUCKET ((static_cast<uint32_t>(SLEAN_TRIMMING_NUMBER_OF_ITEMS_PER_BUCKET * 0.73) / 4) * 4)

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
		
		// Check if device index is specified
		if(deviceIndex != ALL_DEVICES) {
		
			// Display message
			cout << "Using the GPU for slean trimming" << endl;
		}
		
		// Otherwise
		else {
		
			// Check if device's name doesn't exist
			const NS::String *name = device->name();
			if(!name) {
			
				// Display message
				cout << "Getting device's name failed" << endl;
				
				// Return false
				return false;
			}
			
			// Check if name's UTF-8 string doesn't exist
			const char *utf8String = name->utf8String();
			if(!utf8String) {
			
				// Display message
				cout << "Getting device's name failed" << endl;
				
				// Return false
				return false;
			}
			
			// Display message
			cout << "Using " << utf8String << " for slean trimming" << endl;
		}
		
		// Check if device's work group memory isn't fully utilized
		if(bit_floor(device->maxThreadgroupMemoryLength()) / BYTES_IN_A_KILOBYTE > LOCAL_RAM_KILOBYTES) {
		
			// Display message
			cout << "GPU's local RAM won't be fully utilized. Build this program with LOCAL_RAM_KILOBYTES=" << (bit_floor(device->maxThreadgroupMemoryLength()) / BYTES_IN_A_KILOBYTE) << " for potentially better performance" << endl;
		}
		
		// Try to allocate more than the max memory allocation size
		unique_ptr<MTL::Buffer, void(*)(MTL::Buffer *)> moreThanMaxMemoryAllocation(device->newBuffer(device->maxBufferLength() + 1, MTL::ResourceStorageModePrivate | MTL::ResourceHazardTrackingModeUntracked), [](MTL::Buffer *moreThanMaxMemoryAllocation) noexcept {
		
			// Free more than max memory allocation
			moreThanMaxMemoryAllocation->release();
		});
		
		// Set enforce max memory allocation size to if the allocation failed
		const bool enforceMaxMemoryAllocationSize = !moreThanMaxMemoryAllocation;
		moreThanMaxMemoryAllocation.reset();
		
		// Set buckets one number of buckets based on if enforcing max memory allocation size
		const size_t bucketsOneNumberOfBuckets = (enforceMaxMemoryAllocationSize && static_cast<uint64_t>(SLEAN_TRIMMING_INITIAL_MAX_NUMBER_OF_EDGES_PER_BUCKET) * SLEAN_TRIMMING_NUMBER_OF_BUCKETS * sizeof(uint32_t) > device->maxBufferLength()) ? (SLEAN_TRIMMING_AFTER_TRIMMING_ROUND_MAX_NUMBER_OF_EDGES_PER_BUCKET * SLEAN_TRIMMING_NUMBER_OF_BUCKETS + SLEAN_TRIMMING_INITIAL_MAX_NUMBER_OF_EDGES_PER_BUCKET - 1) / SLEAN_TRIMMING_INITIAL_MAX_NUMBER_OF_EDGES_PER_BUCKET : SLEAN_TRIMMING_NUMBER_OF_BUCKETS;
		
		// Set local buckets size based on the device's work group memory size
		const unsigned int localBucketsSize = min(bit_floor((device->maxThreadgroupMemoryLength() - (SLEAN_TRIMMING_NUMBER_OF_BUCKETS + sizeof(uint32_t) - 1)) / (sizeof(uint32_t) * SLEAN_TRIMMING_NUMBER_OF_BUCKETS) + 1), SLEAN_TRIMMING_MAX_LOCAL_BUCKETS_SIZE);
		
		// Check if creating preprocessor macros failed
		static const unique_ptr<NS::Dictionary, void(*)(NS::Dictionary *)> preprocessorMacros(NS::Dictionary::alloc()->init((const NS::Object *[]){
		
			// Edge bits value
			MTLSTR(TO_STRING(EDGE_BITS)),
			
			// Slean trimming parts value
			MTLSTR(TO_STRING(SLEAN_TRIMMING_PARTS)),
			
			// Number of edges per step one work item value
			unique_ptr<NS::Number, void(*)(NS::Number *)>(NS::Number::alloc()->init(SLEAN_TRIMMING_NUMBER_OF_EDGES_PER_STEP_ONE_WORK_ITEM), [](NS::Number *numberOfEdgesPerStepOneWorkItemValue) noexcept {
			
				// Free number of edges per step one work item value
				numberOfEdgesPerStepOneWorkItemValue->release();
				
			}).get(),
			
			// Number of bitmap bytes value
			unique_ptr<NS::Number, void(*)(NS::Number *)>(NS::Number::alloc()->init(SLEAN_TRIMMING_NUMBER_OF_BITMAP_BYTES), [](NS::Number *numberOfBitmapBytesValue) noexcept {
			
				// Free number of bitmap bytes value
				numberOfBitmapBytesValue->release();
				
			}).get(),
			
			// Number of buckets value
			unique_ptr<NS::Number, void(*)(NS::Number *)>(NS::Number::alloc()->init(SLEAN_TRIMMING_NUMBER_OF_BUCKETS), [](NS::Number *numberOfBucketsValue) noexcept {
			
				// Free number of buckets value
				numberOfBucketsValue->release();
				
			}).get(),
			
			// Number of least significant bits ignored during bucket sorting value
			unique_ptr<NS::Number, void(*)(NS::Number *)>(NS::Number::alloc()->init(SLEAN_TRIMMING_NUMBER_OF_LEAST_SIGNIFICANT_BITS_IGNORED_DURING_BUCKET_SORTING), [](NS::Number *numberOfLeastSignificantBitsIgnoredDuringBucketSortingValue) noexcept {
			
				// Free number of least significant bits ignored during bucket sorting value
				numberOfLeastSignificantBitsIgnoredDuringBucketSortingValue->release();
				
			}).get(),
			
			// Initial max number of edges per bucket value
			unique_ptr<NS::Number, void(*)(NS::Number *)>(NS::Number::alloc()->init(SLEAN_TRIMMING_INITIAL_MAX_NUMBER_OF_EDGES_PER_BUCKET), [](NS::Number *initialMaxNumberOfEdgesPerBucketValue) noexcept {
			
				// Free initial max number of edges per bucket value
				initialMaxNumberOfEdgesPerBucketValue->release();
				
			}).get(),
			
			// After trimming round max number of edges per bucket value
			unique_ptr<NS::Number, void(*)(NS::Number *)>(NS::Number::alloc()->init(SLEAN_TRIMMING_AFTER_TRIMMING_ROUND_MAX_NUMBER_OF_EDGES_PER_BUCKET), [](NS::Number *afterTrimmingRoundMaxNumberOfEdgesPerBucketValue) noexcept {
			
				// Free after trimming round max number of edges per bucket value
				afterTrimmingRoundMaxNumberOfEdgesPerBucketValue->release();
				
			}).get(),
			
			// After trimming round max number of edges per remaining edges bucket value
			unique_ptr<NS::Number, void(*)(NS::Number *)>(NS::Number::alloc()->init(SLEAN_TRIMMING_AFTER_TRIMMING_ROUND_MAX_NUMBER_OF_EDGES_PER_REMAINING_EDGES_BUCKET), [](NS::Number *afterTrimmingRoundMaxNumberOfEdgesPerRemainingEdgesBucketValue) noexcept {
			
				// Free after trimming round max number of edges per remaining edges bucket value
				afterTrimmingRoundMaxNumberOfEdgesPerRemainingEdgesBucketValue->release();
				
			}).get(),
			
			// Number of least significant bits ignored during remaining edges bucket sorting value
			unique_ptr<NS::Number, void(*)(NS::Number *)>(NS::Number::alloc()->init(SLEAN_TRIMMING_NUMBER_OF_LEAST_SIGNIFICANT_BITS_IGNORED_DURING_REMAINING_EDGES_BUCKET_SORTING), [](NS::Number *numberOfLeastSignificantBitsIgnoredDuringRemainingEdgesBucketSortingValue) noexcept {
			
				// Free number of least significant bits ignored during remaining edges bucket sorting value
				numberOfLeastSignificantBitsIgnoredDuringRemainingEdgesBucketSortingValue->release();
				
			}).get(),
			
			// Number of remaining edges bitmap bytes value
			unique_ptr<NS::Number, void(*)(NS::Number *)>(NS::Number::alloc()->init(SLEAN_TRIMMING_NUMBER_OF_REMAINING_EDGES_BITMAP_BYTES), [](NS::Number *numberOfRemainingEdgesBitmapBytesValue) noexcept {
			
				// Free number of remaining edges bitmap bytes value
				numberOfRemainingEdgesBitmapBytesValue->release();
				
			}).get(),
			
			// Number of remaining edges buckets value
			unique_ptr<NS::Number, void(*)(NS::Number *)>(NS::Number::alloc()->init(SLEAN_TRIMMING_NUMBER_OF_REMAINING_EDGES_BUCKETS), [](NS::Number *numberOfRemainingEdgesBucketsValue) noexcept {
			
				// Free number of remaining edges buckets value
				numberOfRemainingEdgesBucketsValue->release();
				
			}).get(),
			
			// Initial buckets number of buckets value
			unique_ptr<NS::Number, void(*)(NS::Number *)>(NS::Number::alloc()->init(bucketsOneNumberOfBuckets), [](NS::Number *initialBucketsNumberOfBucketsValue) noexcept {
			
				// Free initial buckets number of buckets value
				initialBucketsNumberOfBucketsValue->release();
				
			}).get(),
			
			// Local buckets size value
			unique_ptr<NS::Number, void(*)(NS::Number *)>(NS::Number::alloc()->init(localBucketsSize), [](NS::Number *localBucketsSizeValue) noexcept {
			
				// Free local buckets size value
				localBucketsSizeValue->release();
				
			}).get()
		
		}, (const NS::Object *[]){
		
			// Edge bits key
			MTLSTR("EDGE_BITS"),
			
			// Slean trimming parts key
			MTLSTR("SLEAN_TRIMMING_PARTS"),
			
			// Number of edges per step one work item key
			MTLSTR("NUMBER_OF_EDGES_PER_STEP_ONE_WORK_ITEM"),
			
			// Number of bitmap bytes key
			MTLSTR("NUMBER_OF_BITMAP_BYTES"),
			
			// Number of buckets key
			MTLSTR("NUMBER_OF_BUCKETS"),
			
			// Number of least significant bits ignored during bucket sorting key
			MTLSTR("NUMBER_OF_LEAST_SIGNIFICANT_BITS_IGNORED_DURING_BUCKET_SORTING"),
			
			// Initial max number of edges per bucket key
			MTLSTR("INITIAL_MAX_NUMBER_OF_EDGES_PER_BUCKET"),
			
			// After trimming round max number of edges per bucket key
			MTLSTR("AFTER_TRIMMING_ROUND_MAX_NUMBER_OF_EDGES_PER_BUCKET"),
			
			// After trimming round max number of edges per remaining edges bucket key
			MTLSTR("AFTER_TRIMMING_ROUND_MAX_NUMBER_OF_EDGES_PER_REMAINING_EDGES_BUCKET"),
			
			// Number of least significant bits ignored during remaining edges bucket sorting key
			MTLSTR("NUMBER_OF_LEAST_SIGNIFICANT_BITS_IGNORED_DURING_REMAINING_EDGES_BUCKET_SORTING"),
			
			// Number of remaining edges bitmap bytes key
			MTLSTR("NUMBER_OF_REMAINING_EDGES_BITMAP_BYTES"),
			
			// Number of remaining edges buckets key
			MTLSTR("NUMBER_OF_REMAINING_EDGES_BUCKETS"),
			
			// Initial buckets number of buckets key
			MTLSTR("INITIAL_BUCKETS_NUMBER_OF_BUCKETS"),
			
			// Local buckets size key
			MTLSTR("LOCAL_BUCKETS_SIZE")
		
		}, 14), [](NS::Dictionary *preprocessorMacros) noexcept {
		
			// Free preprocessor macros
			preprocessorMacros->release();
		});
		if(!preprocessorMacros) {
		
			// Display message
			cout << "Creating preprocessor macros for the kernels failed" << endl;
			
			// Return false
			return false;
		}
		
		// Check if creating compile options for the kernels failed
		static const unique_ptr<MTL::CompileOptions, void(*)(MTL::CompileOptions *)> compileOptions(MTL::CompileOptions::alloc()->init(), [](MTL::CompileOptions *compileOptions) noexcept {
		
			// Free compile options
			compileOptions->release();
		});
		if(!compileOptions) {
		
			// Display message
			cout << "Creating compile options for the kernels failed" << endl;
			
			// Return false
			return false;
		}
		
		// Configure compiler options
		compileOptions->setPreprocessorMacros(preprocessorMacros.get());
		compileOptions->setLanguageVersion(METAL_TARGET_VERSION);
		
		// Check if creating library for the device failed
		const NS::String *source = (
			#include "./slean_trimming.metal"
		);
		NS::Error *createLibraryError;
		static const unique_ptr<MTL::Library, void(*)(MTL::Library *)> library(device->newLibrary(source, compileOptions.get(), &createLibraryError), [](MTL::Library *library) noexcept {
		
			// Free library
			library->release();
		});
		if(!library) {
		
			// Display message
			cout << "Creating library for the GPU failed" << endl;
			
			// Check if an error exists
			if(createLibraryError) {
			
				// Check if error's localized description exists
				const NS::String *localizedDescription = createLibraryError->localizedDescription();
				if(localizedDescription) {
				
					// Check if localized description's UTF-8 string exists
					const char *utf8String = localizedDescription->utf8String();
					if(utf8String) {
					
						// Display message
						cout << utf8String << endl;
					}
				}
			}
			
			// Return false
			return false;
		}
		
		// Check if getting kernels from the library failed
		static const unique_ptr<MTL::Function, void(*)(MTL::Function *)> stepOneKernel(library->newFunction(MTLSTR("trimEdgesStepOne")), [](MTL::Function *stepOneKernel) noexcept {
		
			// Free step one kernel
			stepOneKernel->release();
		});
		static const unique_ptr<MTL::Function, void(*)(MTL::Function *)> stepTwoKernel(library->newFunction(MTLSTR("trimEdgesStepTwo")), [](MTL::Function *stepTwoKernel) noexcept {
		
			// Free step two kernel
			stepTwoKernel->release();
		});
		static const unique_ptr<MTL::Function, void(*)(MTL::Function *)> stepThreeKernel(library->newFunction(MTLSTR("trimEdgesStepThree")), [](MTL::Function *stepThreeKernel) noexcept {
		
			// Free step three kernel
			stepThreeKernel->release();
		});
		static const unique_ptr<MTL::Function, void(*)(MTL::Function *)> stepFourKernel(library->newFunction(MTLSTR("trimEdgesStepFour")), [](MTL::Function *stepFourKernel) noexcept {
		
			// Free step four kernel
			stepFourKernel->release();
		});
		static const unique_ptr<MTL::Function, void(*)(MTL::Function *)> stepFiveKernel(library->newFunction(MTLSTR("trimEdgesStepFive")), [](MTL::Function *stepFiveKernel) noexcept {
		
			// Free step five kernel
			stepFiveKernel->release();
		});
		static const unique_ptr<MTL::Function, void(*)(MTL::Function *)> stepSixKernel(library->newFunction(MTLSTR("trimEdgesStepSix")), [](MTL::Function *stepSixKernel) noexcept {
		
			// Free step six kernel
			stepSixKernel->release();
		});
		static const unique_ptr<MTL::Function, void(*)(MTL::Function *)> stepSevenKernel(library->newFunction(MTLSTR("trimEdgesStepSeven")), [](MTL::Function *stepSevenKernel) noexcept {
		
			// Free step seven kernel
			stepSevenKernel->release();
		});
		static const unique_ptr<MTL::Function, void(*)(MTL::Function *)> stepEightKernel(library->newFunction(MTLSTR("trimEdgesStepEight")), [](MTL::Function *stepEightKernel) noexcept {
		
			// Free step eight kernel
			stepEightKernel->release();
		});
		static const unique_ptr<MTL::Function, void(*)(MTL::Function *)> stepNineKernel(library->newFunction(MTLSTR("trimEdgesStepNine")), [](MTL::Function *stepNineKernel) noexcept {
		
			// Free step nine kernel
			stepNineKernel->release();
		});
		static const unique_ptr<MTL::Function, void(*)(MTL::Function *)> stepTenKernel(library->newFunction(MTLSTR("trimEdgesStepTen")), [](MTL::Function *stepTenKernel) noexcept {
		
			// Free step ten kernel
			stepTenKernel->release();
		});
		static const unique_ptr<MTL::Function, void(*)(MTL::Function *)> stepElevenKernel(library->newFunction(MTLSTR("trimEdgesStepEleven")), [](MTL::Function *stepElevenKernel) noexcept {
		
			// Free step eleven kernel
			stepElevenKernel->release();
		});
		static const unique_ptr<MTL::Function, void(*)(MTL::Function *)> stepTwelveKernel(library->newFunction(MTLSTR("trimEdgesStepTwelve")), [](MTL::Function *stepTwelveKernel) noexcept {
		
			// Free step twelve kernel
			stepTwelveKernel->release();
		});
		static const unique_ptr<MTL::Function, void(*)(MTL::Function *)> stepThirteenKernel(library->newFunction(MTLSTR("trimEdgesStepThirteen")), [](MTL::Function *stepThirteenKernel) noexcept {
		
			// Free step thirteen kernel
			stepThirteenKernel->release();
		});
		static const unique_ptr<MTL::Function, void(*)(MTL::Function *)> stepFourteenKernel(library->newFunction(MTLSTR("trimEdgesStepFourteen")), [](MTL::Function *stepFourteenKernel) noexcept {
		
			// Free step fourteen kernel
			stepFourteenKernel->release();
		});
		static const unique_ptr<MTL::Function, void(*)(MTL::Function *)> stepFifteenKernel(library->newFunction(MTLSTR("trimEdgesStepFifteen")), [](MTL::Function *stepFifteenKernel) noexcept {
		
			// Free step fifteen kernel
			stepFifteenKernel->release();
		});
		static const unique_ptr<MTL::Function, void(*)(MTL::Function *)> stepSixteenKernel(library->newFunction(MTLSTR("trimEdgesStepSixteen")), [](MTL::Function *stepSixteenKernel) noexcept {
		
			// Free step sixteen kernel
			stepSixteenKernel->release();
		});
		static const unique_ptr<MTL::Function, void(*)(MTL::Function *)> stepSeventeenKernel(library->newFunction(MTLSTR("trimEdgesStepSeventeen")), [](MTL::Function *stepSeventeenKernel) noexcept {
		
			// Free step seventeen kernel
			stepSeventeenKernel->release();
		});
		static const unique_ptr<MTL::Function, void(*)(MTL::Function *)> stepEighteenKernel(library->newFunction(MTLSTR("trimEdgesStepEighteen")), [](MTL::Function *stepEighteenKernel) noexcept {
		
			// Free step eighteen kernel
			stepEighteenKernel->release();
		});
		static const unique_ptr<MTL::Function, void(*)(MTL::Function *)> clearNumberOfEdgesPerSourceBucketKernel(library->newFunction(MTLSTR("clearNumberOfEdgesPerSourceBucket")), [](MTL::Function *clearNumberOfEdgesPerSourceBucketKernel) noexcept {
		
			// Free clear number of edges per source bucket kernel
			clearNumberOfEdgesPerSourceBucketKernel->release();
		});
		static const unique_ptr<MTL::Function, void(*)(MTL::Function *)> clearNumberOfEdgesPerDestinationBucketKernel(library->newFunction(MTLSTR("clearNumberOfEdgesPerDestinationBucket")), [](MTL::Function *clearNumberOfEdgesPerDestinationBucketKernel) noexcept {
		
			// Free clear number of edges per destination bucket kernel
			clearNumberOfEdgesPerDestinationBucketKernel->release();
		});
		if(!stepOneKernel || !stepTwoKernel || !stepThreeKernel || !stepFourKernel || !stepFiveKernel || !stepSixKernel || !stepSevenKernel || !stepEightKernel || !stepNineKernel || !stepTenKernel || !stepElevenKernel || !stepTwelveKernel || !stepThirteenKernel || !stepFourteenKernel || !stepFifteenKernel || !stepSixteenKernel || !stepSeventeenKernel || !stepEighteenKernel || !clearNumberOfEdgesPerSourceBucketKernel || !clearNumberOfEdgesPerDestinationBucketKernel) {
		
			// Display message
			cout << "Getting kernels from the library failed" << endl;
			
			// Return false
			return false;
		}
		
		// Check if creating pipelines for the device failed
		NS::Error *createPipelineOneError;
		static unique_ptr<MTL::ComputePipelineState, void(*)(MTL::ComputePipelineState *)> stepOnePipeline(device->newComputePipelineState(stepOneKernel.get(), &createPipelineOneError), [](MTL::ComputePipelineState *stepOnePipeline) noexcept {
		
			// Free step one pipeline
			stepOnePipeline->release();
		});
		NS::Error *createPipelineTwoError;
		static unique_ptr<MTL::ComputePipelineState, void(*)(MTL::ComputePipelineState *)> stepTwoPipeline(device->newComputePipelineState(stepTwoKernel.get(), &createPipelineTwoError), [](MTL::ComputePipelineState *stepTwoPipeline) noexcept {
		
			// Free step two pipeline
			stepTwoPipeline->release();
		});
		NS::Error *createPipelineThreeError;
		static unique_ptr<MTL::ComputePipelineState, void(*)(MTL::ComputePipelineState *)> stepThreePipeline(device->newComputePipelineState(stepThreeKernel.get(), &createPipelineThreeError), [](MTL::ComputePipelineState *stepThreePipeline) noexcept {
		
			// Free step three pipeline
			stepThreePipeline->release();
		});
		NS::Error *createPipelineFourError;
		static unique_ptr<MTL::ComputePipelineState, void(*)(MTL::ComputePipelineState *)> stepFourPipeline(device->newComputePipelineState(stepFourKernel.get(), &createPipelineFourError), [](MTL::ComputePipelineState *stepFourPipeline) noexcept {
		
			// Free step four pipeline
			stepFourPipeline->release();
		});
		NS::Error *createPipelineFiveError;
		static unique_ptr<MTL::ComputePipelineState, void(*)(MTL::ComputePipelineState *)> stepFivePipeline(device->newComputePipelineState(stepFiveKernel.get(), &createPipelineFiveError), [](MTL::ComputePipelineState *stepFivePipeline) noexcept {
		
			// Free step five pipeline
			stepFivePipeline->release();
		});
		NS::Error *createPipelineSixError;
		static unique_ptr<MTL::ComputePipelineState, void(*)(MTL::ComputePipelineState *)> stepSixPipeline(device->newComputePipelineState(stepSixKernel.get(), &createPipelineSixError), [](MTL::ComputePipelineState *stepSixPipeline) noexcept {
		
			// Free step six pipeline
			stepSixPipeline->release();
		});
		NS::Error *createPipelineSevenError;
		static unique_ptr<MTL::ComputePipelineState, void(*)(MTL::ComputePipelineState *)> stepSevenPipeline(device->newComputePipelineState(stepSevenKernel.get(), &createPipelineSevenError), [](MTL::ComputePipelineState *stepSevenPipeline) noexcept {
		
			// Free step seven pipeline
			stepSevenPipeline->release();
		});
		NS::Error *createPipelineEightError;
		static unique_ptr<MTL::ComputePipelineState, void(*)(MTL::ComputePipelineState *)> stepEightPipeline(device->newComputePipelineState(stepEightKernel.get(), &createPipelineEightError), [](MTL::ComputePipelineState *stepEightPipeline) noexcept {
		
			// Free step eight pipeline
			stepEightPipeline->release();
		});
		NS::Error *createPipelineNineError;
		static unique_ptr<MTL::ComputePipelineState, void(*)(MTL::ComputePipelineState *)> stepNinePipeline(device->newComputePipelineState(stepNineKernel.get(), &createPipelineNineError), [](MTL::ComputePipelineState *stepNinePipeline) noexcept {
		
			// Free step nine pipeline
			stepNinePipeline->release();
		});
		NS::Error *createPipelineTenError;
		static unique_ptr<MTL::ComputePipelineState, void(*)(MTL::ComputePipelineState *)> stepTenPipeline(device->newComputePipelineState(stepTenKernel.get(), &createPipelineTenError), [](MTL::ComputePipelineState *stepTenPipeline) noexcept {
		
			// Free step ten pipeline
			stepTenPipeline->release();
		});
		NS::Error *createPipelineElevenError;
		static unique_ptr<MTL::ComputePipelineState, void(*)(MTL::ComputePipelineState *)> stepElevenPipeline(device->newComputePipelineState(stepElevenKernel.get(), &createPipelineElevenError), [](MTL::ComputePipelineState *stepElevenPipeline) noexcept {
		
			// Free step eleven pipeline
			stepElevenPipeline->release();
		});
		NS::Error *createPipelineTwelveError;
		static unique_ptr<MTL::ComputePipelineState, void(*)(MTL::ComputePipelineState *)> stepTwelvePipeline(device->newComputePipelineState(stepTwelveKernel.get(), &createPipelineTwelveError), [](MTL::ComputePipelineState *stepTwelvePipeline) noexcept {
		
			// Free step twelve pipeline
			stepTwelvePipeline->release();
		});
		NS::Error *createPipelineThirteenError;
		static unique_ptr<MTL::ComputePipelineState, void(*)(MTL::ComputePipelineState *)> stepThirteenPipeline(device->newComputePipelineState(stepThirteenKernel.get(), &createPipelineThirteenError), [](MTL::ComputePipelineState *stepThirteenPipeline) noexcept {
		
			// Free step thirteen pipeline
			stepThirteenPipeline->release();
		});
		NS::Error *createPipelineFourteenError;
		static unique_ptr<MTL::ComputePipelineState, void(*)(MTL::ComputePipelineState *)> stepFourteenPipeline(device->newComputePipelineState(stepFourteenKernel.get(), &createPipelineFourteenError), [](MTL::ComputePipelineState *stepFourteenPipeline) noexcept {
		
			// Free step fourteen pipeline
			stepFourteenPipeline->release();
		});
		NS::Error *createPipelineFifteenError;
		static unique_ptr<MTL::ComputePipelineState, void(*)(MTL::ComputePipelineState *)> stepFifteenPipeline(device->newComputePipelineState(stepFifteenKernel.get(), &createPipelineFifteenError), [](MTL::ComputePipelineState *stepFifteenPipeline) noexcept {
		
			// Free step fifteen pipeline
			stepFifteenPipeline->release();
		});
		NS::Error *createPipelineSixteenError;
		static unique_ptr<MTL::ComputePipelineState, void(*)(MTL::ComputePipelineState *)> stepSixteenPipeline(device->newComputePipelineState(stepSixteenKernel.get(), &createPipelineSixteenError), [](MTL::ComputePipelineState *stepSixteenPipeline) noexcept {
		
			// Free step sixteen pipeline
			stepSixteenPipeline->release();
		});
		NS::Error *createPipelineSeventeenError;
		static unique_ptr<MTL::ComputePipelineState, void(*)(MTL::ComputePipelineState *)> stepSeventeenPipeline(device->newComputePipelineState(stepSeventeenKernel.get(), &createPipelineSeventeenError), [](MTL::ComputePipelineState *stepSeventeenPipeline) noexcept {
		
			// Free step seventeen pipeline
			stepSeventeenPipeline->release();
		});
		NS::Error *createPipelineEighteenError;
		static unique_ptr<MTL::ComputePipelineState, void(*)(MTL::ComputePipelineState *)> stepEighteenPipeline(device->newComputePipelineState(stepEighteenKernel.get(), &createPipelineEighteenError), [](MTL::ComputePipelineState *stepEighteenPipeline) noexcept {
		
			// Free step eighteen pipeline
			stepEighteenPipeline->release();
		});
		NS::Error *createPipelineClearNumberOfEdgesPerSourceBucketError;
		static unique_ptr<MTL::ComputePipelineState, void(*)(MTL::ComputePipelineState *)> clearNumberOfEdgesPerSourceBucketPipeline(device->newComputePipelineState(clearNumberOfEdgesPerSourceBucketKernel.get(), &createPipelineClearNumberOfEdgesPerSourceBucketError), [](MTL::ComputePipelineState *clearNumberOfEdgesPerSourceBucketPipeline) noexcept {
		
			// Free clear number of edges per source bucket pipeline
			clearNumberOfEdgesPerSourceBucketPipeline->release();
		});
		NS::Error *createPipelineClearNumberOfEdgesPerDestinationBucketError;
		static unique_ptr<MTL::ComputePipelineState, void(*)(MTL::ComputePipelineState *)> clearNumberOfEdgesPerDestinationBucketPipeline(device->newComputePipelineState(clearNumberOfEdgesPerDestinationBucketKernel.get(), &createPipelineClearNumberOfEdgesPerDestinationBucketError), [](MTL::ComputePipelineState *clearNumberOfEdgesPerDestinationBucketPipeline) noexcept {
		
			// Free clear number of edges per destination bucket pipeline
			clearNumberOfEdgesPerDestinationBucketPipeline->release();
		});
		if(!stepOnePipeline || !stepTwoPipeline || !stepThreePipeline || !stepFourPipeline || !stepFivePipeline || !stepSixPipeline || !stepSevenPipeline || !stepEightPipeline || !stepNinePipeline || !stepTenPipeline || !stepElevenPipeline || !stepTwelvePipeline || !stepThirteenPipeline || !stepFourteenPipeline || !stepFifteenPipeline || !stepSixteenPipeline || !stepSeventeenPipeline || !stepEighteenPipeline || !clearNumberOfEdgesPerSourceBucketPipeline || !clearNumberOfEdgesPerDestinationBucketPipeline) {
		
			// Display message
			cout << "Creating pipelines for the GPU failed" << endl;
			
			// Check if creating pipeline one failed and an error exists
			if(!stepOnePipeline && createPipelineOneError) {
			
				// Check if error's localized description exists
				const NS::String *localizedDescription = createPipelineOneError->localizedDescription();
				if(localizedDescription) {
				
					// Check if localized description's UTF-8 string exists
					const char *utf8String = localizedDescription->utf8String();
					if(utf8String) {
					
						// Display message
						cout << utf8String << endl;
					}
				}
			}
			
			// Check if creating pipeline two failed and an error exists
			if(!stepTwoPipeline && createPipelineTwoError) {
			
				// Check if error's localized description exists
				const NS::String *localizedDescription = createPipelineTwoError->localizedDescription();
				if(localizedDescription) {
				
					// Check if localized description's UTF-8 string exists
					const char *utf8String = localizedDescription->utf8String();
					if(utf8String) {
					
						// Display message
						cout << utf8String << endl;
					}
				}
			}
			
			// Check if creating pipeline three failed and an error exists
			if(!stepThreePipeline && createPipelineThreeError) {
			
				// Check if error's localized description exists
				const NS::String *localizedDescription = createPipelineThreeError->localizedDescription();
				if(localizedDescription) {
				
					// Check if localized description's UTF-8 string exists
					const char *utf8String = localizedDescription->utf8String();
					if(utf8String) {
					
						// Display message
						cout << utf8String << endl;
					}
				}
			}
			
			// Check if creating pipeline four failed and an error exists
			if(!stepFourPipeline && createPipelineFourError) {
			
				// Check if error's localized description exists
				const NS::String *localizedDescription = createPipelineFourError->localizedDescription();
				if(localizedDescription) {
				
					// Check if localized description's UTF-8 string exists
					const char *utf8String = localizedDescription->utf8String();
					if(utf8String) {
					
						// Display message
						cout << utf8String << endl;
					}
				}
			}
			
			// Check if creating pipeline five failed and an error exists
			if(!stepFivePipeline && createPipelineFiveError) {
			
				// Check if error's localized description exists
				const NS::String *localizedDescription = createPipelineFiveError->localizedDescription();
				if(localizedDescription) {
				
					// Check if localized description's UTF-8 string exists
					const char *utf8String = localizedDescription->utf8String();
					if(utf8String) {
					
						// Display message
						cout << utf8String << endl;
					}
				}
			}
			
			// Check if creating pipeline six failed and an error exists
			if(!stepSixPipeline && createPipelineSixError) {
			
				// Check if error's localized description exists
				const NS::String *localizedDescription = createPipelineSixError->localizedDescription();
				if(localizedDescription) {
				
					// Check if localized description's UTF-8 string exists
					const char *utf8String = localizedDescription->utf8String();
					if(utf8String) {
					
						// Display message
						cout << utf8String << endl;
					}
				}
			}
			
			// Check if creating pipeline seven failed and an error exists
			if(!stepSevenPipeline && createPipelineSevenError) {
			
				// Check if error's localized description exists
				const NS::String *localizedDescription = createPipelineSevenError->localizedDescription();
				if(localizedDescription) {
				
					// Check if localized description's UTF-8 string exists
					const char *utf8String = localizedDescription->utf8String();
					if(utf8String) {
					
						// Display message
						cout << utf8String << endl;
					}
				}
			}
			
			// Check if creating pipeline eight failed and an error exists
			if(!stepEightPipeline && createPipelineEightError) {
			
				// Check if error's localized description exists
				const NS::String *localizedDescription = createPipelineEightError->localizedDescription();
				if(localizedDescription) {
				
					// Check if localized description's UTF-8 string exists
					const char *utf8String = localizedDescription->utf8String();
					if(utf8String) {
					
						// Display message
						cout << utf8String << endl;
					}
				}
			}
			
			// Check if creating pipeline nine failed and an error exists
			if(!stepNinePipeline && createPipelineNineError) {
			
				// Check if error's localized description exists
				const NS::String *localizedDescription = createPipelineNineError->localizedDescription();
				if(localizedDescription) {
				
					// Check if localized description's UTF-8 string exists
					const char *utf8String = localizedDescription->utf8String();
					if(utf8String) {
					
						// Display message
						cout << utf8String << endl;
					}
				}
			}
			
			// Check if creating pipeline ten failed and an error exists
			if(!stepTenPipeline && createPipelineTenError) {
			
				// Check if error's localized description exists
				const NS::String *localizedDescription = createPipelineTenError->localizedDescription();
				if(localizedDescription) {
				
					// Check if localized description's UTF-8 string exists
					const char *utf8String = localizedDescription->utf8String();
					if(utf8String) {
					
						// Display message
						cout << utf8String << endl;
					}
				}
			}
			
			// Check if creating pipeline eleven failed and an error exists
			if(!stepElevenPipeline && createPipelineElevenError) {
			
				// Check if error's localized description exists
				const NS::String *localizedDescription = createPipelineElevenError->localizedDescription();
				if(localizedDescription) {
				
					// Check if localized description's UTF-8 string exists
					const char *utf8String = localizedDescription->utf8String();
					if(utf8String) {
					
						// Display message
						cout << utf8String << endl;
					}
				}
			}
			
			// Check if creating pipeline twelve failed and an error exists
			if(!stepTwelvePipeline && createPipelineTwelveError) {
			
				// Check if error's localized description exists
				const NS::String *localizedDescription = createPipelineTwelveError->localizedDescription();
				if(localizedDescription) {
				
					// Check if localized description's UTF-8 string exists
					const char *utf8String = localizedDescription->utf8String();
					if(utf8String) {
					
						// Display message
						cout << utf8String << endl;
					}
				}
			}
			
			// Check if creating pipeline thirteen failed and an error exists
			if(!stepThirteenPipeline && createPipelineThirteenError) {
			
				// Check if error's localized description exists
				const NS::String *localizedDescription = createPipelineThirteenError->localizedDescription();
				if(localizedDescription) {
				
					// Check if localized description's UTF-8 string exists
					const char *utf8String = localizedDescription->utf8String();
					if(utf8String) {
					
						// Display message
						cout << utf8String << endl;
					}
				}
			}
			
			// Check if creating pipeline fourteen failed and an error exists
			if(!stepFourteenPipeline && createPipelineFourteenError) {
			
				// Check if error's localized description exists
				const NS::String *localizedDescription = createPipelineFourteenError->localizedDescription();
				if(localizedDescription) {
				
					// Check if localized description's UTF-8 string exists
					const char *utf8String = localizedDescription->utf8String();
					if(utf8String) {
					
						// Display message
						cout << utf8String << endl;
					}
				}
			}
			
			// Check if creating pipeline fifteen failed and an error exists
			if(!stepFifteenPipeline && createPipelineFifteenError) {
			
				// Check if error's localized description exists
				const NS::String *localizedDescription = createPipelineFifteenError->localizedDescription();
				if(localizedDescription) {
				
					// Check if localized description's UTF-8 string exists
					const char *utf8String = localizedDescription->utf8String();
					if(utf8String) {
					
						// Display message
						cout << utf8String << endl;
					}
				}
			}
			
			// Check if creating pipeline sixteen failed and an error exists
			if(!stepSixteenPipeline && createPipelineSixteenError) {
			
				// Check if error's localized description exists
				const NS::String *localizedDescription = createPipelineSixteenError->localizedDescription();
				if(localizedDescription) {
				
					// Check if localized description's UTF-8 string exists
					const char *utf8String = localizedDescription->utf8String();
					if(utf8String) {
					
						// Display message
						cout << utf8String << endl;
					}
				}
			}
			
			// Check if creating pipeline seventeen failed and an error exists
			if(!stepSeventeenPipeline && createPipelineSeventeenError) {
			
				// Check if error's localized description exists
				const NS::String *localizedDescription = createPipelineSeventeenError->localizedDescription();
				if(localizedDescription) {
				
					// Check if localized description's UTF-8 string exists
					const char *utf8String = localizedDescription->utf8String();
					if(utf8String) {
					
						// Display message
						cout << utf8String << endl;
					}
				}
			}
			
			// Check if creating pipeline eighteen failed and an error exists
			if(!stepEighteenPipeline && createPipelineEighteenError) {
			
				// Check if error's localized description exists
				const NS::String *localizedDescription = createPipelineEighteenError->localizedDescription();
				if(localizedDescription) {
				
					// Check if localized description's UTF-8 string exists
					const char *utf8String = localizedDescription->utf8String();
					if(utf8String) {
					
						// Display message
						cout << utf8String << endl;
					}
				}
			}
			
			// Check if creating pipeline clear number of edges per source bucket failed and an error exists
			if(!clearNumberOfEdgesPerSourceBucketPipeline && createPipelineClearNumberOfEdgesPerSourceBucketError) {
			
				// Check if error's localized description exists
				const NS::String *localizedDescription = createPipelineClearNumberOfEdgesPerSourceBucketError->localizedDescription();
				if(localizedDescription) {
				
					// Check if localized description's UTF-8 string exists
					const char *utf8String = localizedDescription->utf8String();
					if(utf8String) {
					
						// Display message
						cout << utf8String << endl;
					}
				}
			}
			
			// Check if creating pipeline clear number of edges per destination bucket failed and an error exists
			if(!clearNumberOfEdgesPerDestinationBucketPipeline && createPipelineClearNumberOfEdgesPerDestinationBucketError) {
			
				// Check if error's localized description exists
				const NS::String *localizedDescription = createPipelineClearNumberOfEdgesPerDestinationBucketError->localizedDescription();
				if(localizedDescription) {
				
					// Check if localized description's UTF-8 string exists
					const char *utf8String = localizedDescription->utf8String();
					if(utf8String) {
					
						// Display message
						cout << utf8String << endl;
					}
				}
			}
			
			// Return false
			return false;
		}
		
		// Set total number of work items based on the kernels and work items per work group
		const MTL::Size totalNumberOfWorkItems[] = {
		
			// Trim edges step one kernel
			{(NUMBER_OF_EDGES / SLEAN_TRIMMING_PARTS) / SLEAN_TRIMMING_NUMBER_OF_EDGES_PER_STEP_ONE_WORK_ITEM, 1, 1},
			
			// Trim edges step two kernel
			{SLEAN_TRIMMING_NUMBER_OF_BUCKETS * min(bit_floor(device->maxThreadsPerThreadgroup().width), bit_floor(static_cast<NS::UInteger>(INT16_MAX / 2))), 1, 1},
			
			// Trim edges step three kernel
			{SLEAN_TRIMMING_NUMBER_OF_BUCKETS * min(bit_floor(device->maxThreadsPerThreadgroup().width), bit_floor(static_cast<NS::UInteger>(INT16_MAX / 2))), 1, 1},
			
			// Trim edges step four kernel
			{SLEAN_TRIMMING_NUMBER_OF_BUCKETS * min(bit_floor(device->maxThreadsPerThreadgroup().width), bit_floor(static_cast<NS::UInteger>(INT16_MAX / 2))), 1, 1},
			
			// Trim edges step five kernel
			{SLEAN_TRIMMING_NUMBER_OF_REMAINING_EDGES_BUCKETS * min(bit_floor(device->maxThreadsPerThreadgroup().width), bit_floor(static_cast<NS::UInteger>(INT16_MAX / 2))), 1, 1},
			
			// Trim edges step six kernel
			{SLEAN_TRIMMING_NUMBER_OF_BUCKETS * min(bit_floor(device->maxThreadsPerThreadgroup().width), bit_floor(static_cast<NS::UInteger>(INT16_MAX / 2))), 1, 1},
			
			// Trim edges step seven kernel
			{(NUMBER_OF_EDGES / SLEAN_TRIMMING_PARTS) / (sizeof(uint64_t) * BITS_IN_A_BYTE), 1, 1},
			
			// Trim edges step eight kernel
			{SLEAN_TRIMMING_NUMBER_OF_BUCKETS * min(bit_floor(device->maxThreadsPerThreadgroup().width), bit_floor(static_cast<NS::UInteger>(INT16_MAX / 2))), 1, 1},
			
			// Trim edges step nine kernel
			{SLEAN_TRIMMING_NUMBER_OF_BUCKETS * min(bit_floor(device->maxThreadsPerThreadgroup().width), bit_floor(static_cast<NS::UInteger>(INT16_MAX / 2))), 1, 1},
			
			// Trim edges step ten kernel
			{SLEAN_TRIMMING_NUMBER_OF_BUCKETS * min(bit_floor(device->maxThreadsPerThreadgroup().width), bit_floor(static_cast<NS::UInteger>(INT16_MAX / 2))), 1, 1},
			
			// Trim edges step eleven kernel
			{SLEAN_TRIMMING_NUMBER_OF_REMAINING_EDGES_BUCKETS * min(bit_floor(device->maxThreadsPerThreadgroup().width), bit_floor(static_cast<NS::UInteger>(INT16_MAX / 2))), 1, 1},
			
			// Trim edges step twelve kernel
			{SLEAN_TRIMMING_NUMBER_OF_BUCKETS * min(bit_floor(device->maxThreadsPerThreadgroup().width), bit_floor(static_cast<NS::UInteger>(INT16_MAX / 2))), 1, 1},
			
			// Trim edges step thirteen kernel
			{(NUMBER_OF_EDGES / SLEAN_TRIMMING_PARTS) / (sizeof(uint64_t) * BITS_IN_A_BYTE), 1, 1},
			
			// Trim edges step fourteen kernel
			{SLEAN_TRIMMING_NUMBER_OF_BUCKETS * min(bit_floor(device->maxThreadsPerThreadgroup().width), bit_floor(static_cast<NS::UInteger>(INT16_MAX / 2))), 1, 1},
			
			// Trim edges step fifteen kernel
			{SLEAN_TRIMMING_NUMBER_OF_BUCKETS * min(bit_floor(device->maxThreadsPerThreadgroup().width), bit_floor(static_cast<NS::UInteger>(INT16_MAX / 2))), 1, 1},
			
			// Trim edges step sixteen kernel
			{SLEAN_TRIMMING_NUMBER_OF_BUCKETS * min(bit_floor(device->maxThreadsPerThreadgroup().width), bit_floor(static_cast<NS::UInteger>(INT16_MAX / 2))), 1, 1},
			
			// Trim edges step seventeen kernel
			{SLEAN_TRIMMING_NUMBER_OF_REMAINING_EDGES_BUCKETS * min(bit_floor(device->maxThreadsPerThreadgroup().width), bit_floor(static_cast<NS::UInteger>(INT16_MAX / 2))), 1, 1},
			
			// Trim edges step eighteen kernel
			{SLEAN_TRIMMING_NUMBER_OF_BUCKETS * min(bit_floor(device->maxThreadsPerThreadgroup().width), bit_floor(static_cast<NS::UInteger>(INT16_MAX / 2))), 1, 1},
			
			// Clear number of edges per source bucket kernel
			{((SLEAN_TRIMMING_NUMBER_OF_BUCKETS * sizeof(uint32_t) + sizeof(uint64_t) - 1)) / sizeof(uint64_t), 1, 1},
			
			// Clear number of edges per destination bucket kernel
			{((SLEAN_TRIMMING_NUMBER_OF_REMAINING_EDGES_BUCKETS * sizeof(uint32_t) + sizeof(uint64_t) - 1)) / sizeof(uint64_t), 1, 1}
		};
		
		// Set work items per work group based on the total number of work items and max work group size
		const MTL::Size workItemsPerWorkGroup[] = {
		
			// Trim edges step one kernel
			{min(min(bit_floor(min(static_cast<NS::UInteger>(SLEAN_TRIMMING_WORK_ITEMS_PER_WORK_GROUP_STEP_ONE), device->maxThreadsPerThreadgroup().width)), bit_floor(static_cast<NS::UInteger>(INT16_MAX / 2))), totalNumberOfWorkItems[0].width), 1, 1},
			
			// Trim edges step two kernel
			{min(bit_floor(device->maxThreadsPerThreadgroup().width), bit_floor(static_cast<NS::UInteger>(INT16_MAX / 2))), 1, 1},
			
			// Trim edges step three kernel
			{min(bit_floor(device->maxThreadsPerThreadgroup().width), bit_floor(static_cast<NS::UInteger>(INT16_MAX / 2))), 1, 1},
			
			// Trim edges step four kernel
			{min(bit_floor(device->maxThreadsPerThreadgroup().width), bit_floor(static_cast<NS::UInteger>(INT16_MAX / 2))), 1, 1},
			
			// Trim edges step five kernel
			{min(bit_floor(device->maxThreadsPerThreadgroup().width), bit_floor(static_cast<NS::UInteger>(INT16_MAX / 2))), 1, 1},
			
			// Trim edges step six kernel
			{min(bit_floor(device->maxThreadsPerThreadgroup().width), bit_floor(static_cast<NS::UInteger>(INT16_MAX / 2))), 1, 1},
			
			// Trim edges step seven kernel
			{min(min(bit_floor(device->maxThreadsPerThreadgroup().width), bit_floor(static_cast<NS::UInteger>(INT16_MAX / 2))), totalNumberOfWorkItems[6].width), 1, 1},
			
			// Trim edges step eight kernel
			{min(bit_floor(device->maxThreadsPerThreadgroup().width), bit_floor(static_cast<NS::UInteger>(INT16_MAX / 2))), 1, 1},
			
			// Trim edges step nine kernel
			{min(bit_floor(device->maxThreadsPerThreadgroup().width), bit_floor(static_cast<NS::UInteger>(INT16_MAX / 2))), 1, 1},
			
			// Trim edges step ten kernel
			{min(bit_floor(device->maxThreadsPerThreadgroup().width), bit_floor(static_cast<NS::UInteger>(INT16_MAX / 2))), 1, 1},
			
			// Trim edges step eleven kernel
			{min(bit_floor(device->maxThreadsPerThreadgroup().width), bit_floor(static_cast<NS::UInteger>(INT16_MAX / 2))), 1, 1},
			
			// Trim edges step twelve kernel
			{min(bit_floor(device->maxThreadsPerThreadgroup().width), bit_floor(static_cast<NS::UInteger>(INT16_MAX / 2))), 1, 1},
			
			// Trim edges step thirteen kernel
			{min(min(bit_floor(device->maxThreadsPerThreadgroup().width), bit_floor(static_cast<NS::UInteger>(INT16_MAX / 2))), totalNumberOfWorkItems[12].width), 1, 1},
			
			// Trim edges step fourteen kernel
			{min(bit_floor(device->maxThreadsPerThreadgroup().width), bit_floor(static_cast<NS::UInteger>(INT16_MAX / 2))), 1, 1},
			
			// Trim edges step fifteen kernel
			{min(bit_floor(device->maxThreadsPerThreadgroup().width), bit_floor(static_cast<NS::UInteger>(INT16_MAX / 2))), 1, 1},
			
			// Trim edges step sixteen kernel
			{min(bit_floor(device->maxThreadsPerThreadgroup().width), bit_floor(static_cast<NS::UInteger>(INT16_MAX / 2))), 1, 1},
			
			// Trim edges step seventeen kernel
			{min(bit_floor(device->maxThreadsPerThreadgroup().width), bit_floor(static_cast<NS::UInteger>(INT16_MAX / 2))), 1, 1},
			
			// Trim edges step eighteen kernel
			{min(bit_floor(device->maxThreadsPerThreadgroup().width), bit_floor(static_cast<NS::UInteger>(INT16_MAX / 2))), 1, 1},
			
			// Clear number of edges per source bucket kernel
			{min(bit_floor(device->maxThreadsPerThreadgroup().width), totalNumberOfWorkItems[18].width), 1, 1},
			
			// Clear number of edges per destination bucket kernel
			{min(bit_floor(device->maxThreadsPerThreadgroup().width), totalNumberOfWorkItems[19].width), 1, 1}
		};
		
		// Check if creating pipeline descriptors failed
		static const unique_ptr<MTL::ComputePipelineDescriptor, void(*)(MTL::ComputePipelineDescriptor *)> stepOnePipelineDescriptor(MTL::ComputePipelineDescriptor::alloc()->init(), [](MTL::ComputePipelineDescriptor *stepOnePipelineDescriptor) noexcept {
		
			// Free step one pipeline descriptor
			stepOnePipelineDescriptor->release();
		});
		static const unique_ptr<MTL::ComputePipelineDescriptor, void(*)(MTL::ComputePipelineDescriptor *)> stepTwoPipelineDescriptor(MTL::ComputePipelineDescriptor::alloc()->init(), [](MTL::ComputePipelineDescriptor *stepTwoPipelineDescriptor) noexcept {
		
			// Free step two pipeline descriptor
			stepTwoPipelineDescriptor->release();
		});
		static const unique_ptr<MTL::ComputePipelineDescriptor, void(*)(MTL::ComputePipelineDescriptor *)> stepThreePipelineDescriptor(MTL::ComputePipelineDescriptor::alloc()->init(), [](MTL::ComputePipelineDescriptor *stepThreePipelineDescriptor) noexcept {
		
			// Free step three pipeline descriptor
			stepThreePipelineDescriptor->release();
		});
		static const unique_ptr<MTL::ComputePipelineDescriptor, void(*)(MTL::ComputePipelineDescriptor *)> stepFourPipelineDescriptor(MTL::ComputePipelineDescriptor::alloc()->init(), [](MTL::ComputePipelineDescriptor *stepFourPipelineDescriptor) noexcept {
		
			// Free step four pipeline descriptor
			stepFourPipelineDescriptor->release();
		});
		static const unique_ptr<MTL::ComputePipelineDescriptor, void(*)(MTL::ComputePipelineDescriptor *)> stepFivePipelineDescriptor(MTL::ComputePipelineDescriptor::alloc()->init(), [](MTL::ComputePipelineDescriptor *stepFivePipelineDescriptor) noexcept {
		
			// Free step five pipeline descriptor
			stepFivePipelineDescriptor->release();
		});
		static const unique_ptr<MTL::ComputePipelineDescriptor, void(*)(MTL::ComputePipelineDescriptor *)> stepSixPipelineDescriptor(MTL::ComputePipelineDescriptor::alloc()->init(), [](MTL::ComputePipelineDescriptor *stepSixPipelineDescriptor) noexcept {
		
			// Free step six pipeline descriptor
			stepSixPipelineDescriptor->release();
		});
		static const unique_ptr<MTL::ComputePipelineDescriptor, void(*)(MTL::ComputePipelineDescriptor *)> stepSevenPipelineDescriptor(MTL::ComputePipelineDescriptor::alloc()->init(), [](MTL::ComputePipelineDescriptor *stepSevenPipelineDescriptor) noexcept {
		
			// Free step seven pipeline descriptor
			stepSevenPipelineDescriptor->release();
		});
		static const unique_ptr<MTL::ComputePipelineDescriptor, void(*)(MTL::ComputePipelineDescriptor *)> stepEightPipelineDescriptor(MTL::ComputePipelineDescriptor::alloc()->init(), [](MTL::ComputePipelineDescriptor *stepEightPipelineDescriptor) noexcept {
		
			// Free step eight pipeline descriptor
			stepEightPipelineDescriptor->release();
		});
		static const unique_ptr<MTL::ComputePipelineDescriptor, void(*)(MTL::ComputePipelineDescriptor *)> stepNinePipelineDescriptor(MTL::ComputePipelineDescriptor::alloc()->init(), [](MTL::ComputePipelineDescriptor *stepNinePipelineDescriptor) noexcept {
		
			// Free step nine pipeline descriptor
			stepNinePipelineDescriptor->release();
		});
		static const unique_ptr<MTL::ComputePipelineDescriptor, void(*)(MTL::ComputePipelineDescriptor *)> stepTenPipelineDescriptor(MTL::ComputePipelineDescriptor::alloc()->init(), [](MTL::ComputePipelineDescriptor *stepTenPipelineDescriptor) noexcept {
		
			// Free step ten pipeline descriptor
			stepTenPipelineDescriptor->release();
		});
		static const unique_ptr<MTL::ComputePipelineDescriptor, void(*)(MTL::ComputePipelineDescriptor *)> stepElevenPipelineDescriptor(MTL::ComputePipelineDescriptor::alloc()->init(), [](MTL::ComputePipelineDescriptor *stepElevenPipelineDescriptor) noexcept {
		
			// Free step eleven pipeline descriptor
			stepElevenPipelineDescriptor->release();
		});
		static const unique_ptr<MTL::ComputePipelineDescriptor, void(*)(MTL::ComputePipelineDescriptor *)> stepTwelvePipelineDescriptor(MTL::ComputePipelineDescriptor::alloc()->init(), [](MTL::ComputePipelineDescriptor *stepTwelvePipelineDescriptor) noexcept {
		
			// Free step twelve pipeline descriptor
			stepTwelvePipelineDescriptor->release();
		});
		static const unique_ptr<MTL::ComputePipelineDescriptor, void(*)(MTL::ComputePipelineDescriptor *)> stepThirteenPipelineDescriptor(MTL::ComputePipelineDescriptor::alloc()->init(), [](MTL::ComputePipelineDescriptor *stepThirteenPipelineDescriptor) noexcept {
		
			// Free step thirteen pipeline descriptor
			stepThirteenPipelineDescriptor->release();
		});
		static const unique_ptr<MTL::ComputePipelineDescriptor, void(*)(MTL::ComputePipelineDescriptor *)> stepFourteenPipelineDescriptor(MTL::ComputePipelineDescriptor::alloc()->init(), [](MTL::ComputePipelineDescriptor *stepFourteenPipelineDescriptor) noexcept {
		
			// Free step fourteen pipeline descriptor
			stepFourteenPipelineDescriptor->release();
		});
		static const unique_ptr<MTL::ComputePipelineDescriptor, void(*)(MTL::ComputePipelineDescriptor *)> stepFifteenPipelineDescriptor(MTL::ComputePipelineDescriptor::alloc()->init(), [](MTL::ComputePipelineDescriptor *stepFifteenPipelineDescriptor) noexcept {
		
			// Free step fifteen pipeline descriptor
			stepFifteenPipelineDescriptor->release();
		});
		static const unique_ptr<MTL::ComputePipelineDescriptor, void(*)(MTL::ComputePipelineDescriptor *)> stepSixteenPipelineDescriptor(MTL::ComputePipelineDescriptor::alloc()->init(), [](MTL::ComputePipelineDescriptor *stepSixteenPipelineDescriptor) noexcept {
		
			// Free step sixteen pipeline descriptor
			stepSixteenPipelineDescriptor->release();
		});
		static const unique_ptr<MTL::ComputePipelineDescriptor, void(*)(MTL::ComputePipelineDescriptor *)> stepSeventeenPipelineDescriptor(MTL::ComputePipelineDescriptor::alloc()->init(), [](MTL::ComputePipelineDescriptor *stepSeventeenPipelineDescriptor) noexcept {
		
			// Free step seventeen pipeline descriptor
			stepSeventeenPipelineDescriptor->release();
		});
		static const unique_ptr<MTL::ComputePipelineDescriptor, void(*)(MTL::ComputePipelineDescriptor *)> stepEighteenPipelineDescriptor(MTL::ComputePipelineDescriptor::alloc()->init(), [](MTL::ComputePipelineDescriptor *stepEighteenPipelineDescriptor) noexcept {
		
			// Free step eighteen pipeline descriptor
			stepEighteenPipelineDescriptor->release();
		});
		static const unique_ptr<MTL::ComputePipelineDescriptor, void(*)(MTL::ComputePipelineDescriptor *)> clearNumberOfEdgesPerSourceBucketPipelineDescriptor(MTL::ComputePipelineDescriptor::alloc()->init(), [](MTL::ComputePipelineDescriptor *clearNumberOfEdgesPerSourceBucketPipelineDescriptor) noexcept {
		
			// Free clear number of edges per source bucket pipeline descriptor
			clearNumberOfEdgesPerSourceBucketPipelineDescriptor->release();
		});
		static const unique_ptr<MTL::ComputePipelineDescriptor, void(*)(MTL::ComputePipelineDescriptor *)> clearNumberOfEdgesPerDestinationBucketPipelineDescriptor(MTL::ComputePipelineDescriptor::alloc()->init(), [](MTL::ComputePipelineDescriptor *clearNumberOfEdgesPerDestinationBucketPipelineDescriptor) noexcept {
		
			// Free clear number of edges per destination bucket pipeline descriptor
			clearNumberOfEdgesPerDestinationBucketPipelineDescriptor->release();
		});
		if(!stepOnePipelineDescriptor || !stepTwoPipelineDescriptor || !stepThreePipelineDescriptor || !stepFourPipelineDescriptor || !stepFivePipelineDescriptor || !stepSixPipelineDescriptor || !stepSevenPipelineDescriptor || !stepEightPipelineDescriptor || !stepNinePipelineDescriptor || !stepTenPipelineDescriptor || !stepElevenPipelineDescriptor || !stepTwelvePipelineDescriptor || !stepThirteenPipelineDescriptor || !stepFourteenPipelineDescriptor || !stepFifteenPipelineDescriptor || !stepSixteenPipelineDescriptor || !stepSeventeenPipelineDescriptor || !stepEighteenPipelineDescriptor || !clearNumberOfEdgesPerSourceBucketPipelineDescriptor || !clearNumberOfEdgesPerDestinationBucketPipelineDescriptor) {
		
			// Display message
			cout << "Creating pipeline descriptors failed" << endl;
			
			// Return false
			return false;
		}
		
		// Configure pipeline descriptors
		stepOnePipelineDescriptor->setComputeFunction(stepOneKernel.get());
		stepOnePipelineDescriptor->setMaxTotalThreadsPerThreadgroup(workItemsPerWorkGroup[0].width);
		stepOnePipelineDescriptor->setThreadGroupSizeIsMultipleOfThreadExecutionWidth(workItemsPerWorkGroup[0].width % stepOnePipeline->threadExecutionWidth() == 0);
		
		stepTwoPipelineDescriptor->setComputeFunction(stepTwoKernel.get());
		stepTwoPipelineDescriptor->setMaxTotalThreadsPerThreadgroup(workItemsPerWorkGroup[1].width);
		stepTwoPipelineDescriptor->setThreadGroupSizeIsMultipleOfThreadExecutionWidth(workItemsPerWorkGroup[1].width % stepTwoPipeline->threadExecutionWidth() == 0);
		
		stepThreePipelineDescriptor->setComputeFunction(stepThreeKernel.get());
		stepThreePipelineDescriptor->setMaxTotalThreadsPerThreadgroup(workItemsPerWorkGroup[2].width);
		stepThreePipelineDescriptor->setThreadGroupSizeIsMultipleOfThreadExecutionWidth(workItemsPerWorkGroup[2].width % stepThreePipeline->threadExecutionWidth() == 0);
		
		stepFourPipelineDescriptor->setComputeFunction(stepFourKernel.get());
		stepFourPipelineDescriptor->setMaxTotalThreadsPerThreadgroup(workItemsPerWorkGroup[3].width);
		stepFourPipelineDescriptor->setThreadGroupSizeIsMultipleOfThreadExecutionWidth(workItemsPerWorkGroup[3].width % stepFourPipeline->threadExecutionWidth() == 0);
		
		stepFivePipelineDescriptor->setComputeFunction(stepFiveKernel.get());
		stepFivePipelineDescriptor->setMaxTotalThreadsPerThreadgroup(workItemsPerWorkGroup[4].width);
		stepFivePipelineDescriptor->setThreadGroupSizeIsMultipleOfThreadExecutionWidth(workItemsPerWorkGroup[4].width % stepFivePipeline->threadExecutionWidth() == 0);
		
		stepSixPipelineDescriptor->setComputeFunction(stepSixKernel.get());
		stepSixPipelineDescriptor->setMaxTotalThreadsPerThreadgroup(workItemsPerWorkGroup[5].width);
		stepSixPipelineDescriptor->setThreadGroupSizeIsMultipleOfThreadExecutionWidth(workItemsPerWorkGroup[5].width % stepSixPipeline->threadExecutionWidth() == 0);
		
		stepSevenPipelineDescriptor->setComputeFunction(stepSevenKernel.get());
		stepSevenPipelineDescriptor->setMaxTotalThreadsPerThreadgroup(workItemsPerWorkGroup[6].width);
		stepSevenPipelineDescriptor->setThreadGroupSizeIsMultipleOfThreadExecutionWidth(workItemsPerWorkGroup[6].width % stepSevenPipeline->threadExecutionWidth() == 0);
		
		stepEightPipelineDescriptor->setComputeFunction(stepEightKernel.get());
		stepEightPipelineDescriptor->setMaxTotalThreadsPerThreadgroup(workItemsPerWorkGroup[7].width);
		stepEightPipelineDescriptor->setThreadGroupSizeIsMultipleOfThreadExecutionWidth(workItemsPerWorkGroup[7].width % stepEightPipeline->threadExecutionWidth() == 0);
		
		stepNinePipelineDescriptor->setComputeFunction(stepNineKernel.get());
		stepNinePipelineDescriptor->setMaxTotalThreadsPerThreadgroup(workItemsPerWorkGroup[8].width);
		stepNinePipelineDescriptor->setThreadGroupSizeIsMultipleOfThreadExecutionWidth(workItemsPerWorkGroup[8].width % stepNinePipeline->threadExecutionWidth() == 0);
		
		stepTenPipelineDescriptor->setComputeFunction(stepTenKernel.get());
		stepTenPipelineDescriptor->setMaxTotalThreadsPerThreadgroup(workItemsPerWorkGroup[9].width);
		stepTenPipelineDescriptor->setThreadGroupSizeIsMultipleOfThreadExecutionWidth(workItemsPerWorkGroup[9].width % stepTenPipeline->threadExecutionWidth() == 0);
		
		stepElevenPipelineDescriptor->setComputeFunction(stepElevenKernel.get());
		stepElevenPipelineDescriptor->setMaxTotalThreadsPerThreadgroup(workItemsPerWorkGroup[10].width);
		stepElevenPipelineDescriptor->setThreadGroupSizeIsMultipleOfThreadExecutionWidth(workItemsPerWorkGroup[10].width % stepElevenPipeline->threadExecutionWidth() == 0);
		
		stepTwelvePipelineDescriptor->setComputeFunction(stepTwelveKernel.get());
		stepTwelvePipelineDescriptor->setMaxTotalThreadsPerThreadgroup(workItemsPerWorkGroup[11].width);
		stepTwelvePipelineDescriptor->setThreadGroupSizeIsMultipleOfThreadExecutionWidth(workItemsPerWorkGroup[11].width % stepTwelvePipeline->threadExecutionWidth() == 0);
		
		stepThirteenPipelineDescriptor->setComputeFunction(stepThirteenKernel.get());
		stepThirteenPipelineDescriptor->setMaxTotalThreadsPerThreadgroup(workItemsPerWorkGroup[12].width);
		stepThirteenPipelineDescriptor->setThreadGroupSizeIsMultipleOfThreadExecutionWidth(workItemsPerWorkGroup[12].width % stepThirteenPipeline->threadExecutionWidth() == 0);
		
		stepFourteenPipelineDescriptor->setComputeFunction(stepFourteenKernel.get());
		stepFourteenPipelineDescriptor->setMaxTotalThreadsPerThreadgroup(workItemsPerWorkGroup[13].width);
		stepFourteenPipelineDescriptor->setThreadGroupSizeIsMultipleOfThreadExecutionWidth(workItemsPerWorkGroup[13].width % stepFourteenPipeline->threadExecutionWidth() == 0);
		
		stepFifteenPipelineDescriptor->setComputeFunction(stepFifteenKernel.get());
		stepFifteenPipelineDescriptor->setMaxTotalThreadsPerThreadgroup(workItemsPerWorkGroup[14].width);
		stepFifteenPipelineDescriptor->setThreadGroupSizeIsMultipleOfThreadExecutionWidth(workItemsPerWorkGroup[14].width % stepFifteenPipeline->threadExecutionWidth() == 0);
		
		stepSixteenPipelineDescriptor->setComputeFunction(stepSixteenKernel.get());
		stepSixteenPipelineDescriptor->setMaxTotalThreadsPerThreadgroup(workItemsPerWorkGroup[15].width);
		stepSixteenPipelineDescriptor->setThreadGroupSizeIsMultipleOfThreadExecutionWidth(workItemsPerWorkGroup[15].width % stepSixteenPipeline->threadExecutionWidth() == 0);
		
		stepSeventeenPipelineDescriptor->setComputeFunction(stepSeventeenKernel.get());
		stepSeventeenPipelineDescriptor->setMaxTotalThreadsPerThreadgroup(workItemsPerWorkGroup[16].width);
		stepSeventeenPipelineDescriptor->setThreadGroupSizeIsMultipleOfThreadExecutionWidth(workItemsPerWorkGroup[16].width % stepSeventeenPipeline->threadExecutionWidth() == 0);
		
		stepEighteenPipelineDescriptor->setComputeFunction(stepEighteenKernel.get());
		stepEighteenPipelineDescriptor->setMaxTotalThreadsPerThreadgroup(workItemsPerWorkGroup[17].width);
		stepEighteenPipelineDescriptor->setThreadGroupSizeIsMultipleOfThreadExecutionWidth(workItemsPerWorkGroup[17].width % stepEighteenPipeline->threadExecutionWidth() == 0);
		
		clearNumberOfEdgesPerSourceBucketPipelineDescriptor->setComputeFunction(clearNumberOfEdgesPerSourceBucketKernel.get());
		clearNumberOfEdgesPerSourceBucketPipelineDescriptor->setMaxTotalThreadsPerThreadgroup(workItemsPerWorkGroup[18].width);
		clearNumberOfEdgesPerSourceBucketPipelineDescriptor->setThreadGroupSizeIsMultipleOfThreadExecutionWidth(workItemsPerWorkGroup[18].width % clearNumberOfEdgesPerSourceBucketPipeline->threadExecutionWidth() == 0);
		
		clearNumberOfEdgesPerDestinationBucketPipelineDescriptor->setComputeFunction(clearNumberOfEdgesPerDestinationBucketKernel.get());
		clearNumberOfEdgesPerDestinationBucketPipelineDescriptor->setMaxTotalThreadsPerThreadgroup(workItemsPerWorkGroup[19].width);
		clearNumberOfEdgesPerDestinationBucketPipelineDescriptor->setThreadGroupSizeIsMultipleOfThreadExecutionWidth(workItemsPerWorkGroup[19].width % clearNumberOfEdgesPerDestinationBucketPipeline->threadExecutionWidth() == 0);
		
		// Check if recreating pipelines for the device with hardcoded work items per work group failed
		stepOnePipeline = unique_ptr<MTL::ComputePipelineState, void(*)(MTL::ComputePipelineState *)>(device->newComputePipelineState(stepOnePipelineDescriptor.get(), MTL::PipelineOptionNone, nullptr, &createPipelineOneError), [](MTL::ComputePipelineState *stepOnePipeline) noexcept {
		
			// Free step one pipeline
			stepOnePipeline->release();
		});
		stepTwoPipeline = unique_ptr<MTL::ComputePipelineState, void(*)(MTL::ComputePipelineState *)>(device->newComputePipelineState(stepTwoPipelineDescriptor.get(), MTL::PipelineOptionNone, nullptr, &createPipelineTwoError), [](MTL::ComputePipelineState *stepTwoPipeline) noexcept {
		
			// Free step two pipeline
			stepTwoPipeline->release();
		});
		stepThreePipeline = unique_ptr<MTL::ComputePipelineState, void(*)(MTL::ComputePipelineState *)>(device->newComputePipelineState(stepThreePipelineDescriptor.get(), MTL::PipelineOptionNone, nullptr, &createPipelineThreeError), [](MTL::ComputePipelineState *stepThreePipeline) noexcept {
		
			// Free step three pipeline
			stepThreePipeline->release();
		});
		stepFourPipeline = unique_ptr<MTL::ComputePipelineState, void(*)(MTL::ComputePipelineState *)>(device->newComputePipelineState(stepFourPipelineDescriptor.get(), MTL::PipelineOptionNone, nullptr, &createPipelineFourError), [](MTL::ComputePipelineState *stepFourPipeline) noexcept {
		
			// Free step four pipeline
			stepFourPipeline->release();
		});
		stepFivePipeline = unique_ptr<MTL::ComputePipelineState, void(*)(MTL::ComputePipelineState *)>(device->newComputePipelineState(stepFivePipelineDescriptor.get(), MTL::PipelineOptionNone, nullptr, &createPipelineFiveError), [](MTL::ComputePipelineState *stepFivePipeline) noexcept {
		
			// Free step five pipeline
			stepFivePipeline->release();
		});
		stepSixPipeline = unique_ptr<MTL::ComputePipelineState, void(*)(MTL::ComputePipelineState *)>(device->newComputePipelineState(stepSixPipelineDescriptor.get(), MTL::PipelineOptionNone, nullptr, &createPipelineSixError), [](MTL::ComputePipelineState *stepSixPipeline) noexcept {
		
			// Free step six pipeline
			stepSixPipeline->release();
		});
		stepSevenPipeline = unique_ptr<MTL::ComputePipelineState, void(*)(MTL::ComputePipelineState *)>(device->newComputePipelineState(stepSevenPipelineDescriptor.get(), MTL::PipelineOptionNone, nullptr, &createPipelineSevenError), [](MTL::ComputePipelineState *stepSevenPipeline) noexcept {
		
			// Free step seven pipeline
			stepSevenPipeline->release();
		});
		stepEightPipeline = unique_ptr<MTL::ComputePipelineState, void(*)(MTL::ComputePipelineState *)>(device->newComputePipelineState(stepEightPipelineDescriptor.get(), MTL::PipelineOptionNone, nullptr, &createPipelineEightError), [](MTL::ComputePipelineState *stepEightPipeline) noexcept {
		
			// Free step eight pipeline
			stepEightPipeline->release();
		});
		stepNinePipeline = unique_ptr<MTL::ComputePipelineState, void(*)(MTL::ComputePipelineState *)>(device->newComputePipelineState(stepNinePipelineDescriptor.get(), MTL::PipelineOptionNone, nullptr, &createPipelineNineError), [](MTL::ComputePipelineState *stepNinePipeline) noexcept {
		
			// Free step nine pipeline
			stepNinePipeline->release();
		});
		stepTenPipeline = unique_ptr<MTL::ComputePipelineState, void(*)(MTL::ComputePipelineState *)>(device->newComputePipelineState(stepTenPipelineDescriptor.get(), MTL::PipelineOptionNone, nullptr, &createPipelineTenError), [](MTL::ComputePipelineState *stepTenPipeline) noexcept {
		
			// Free step ten pipeline
			stepTenPipeline->release();
		});
		stepElevenPipeline = unique_ptr<MTL::ComputePipelineState, void(*)(MTL::ComputePipelineState *)>(device->newComputePipelineState(stepElevenPipelineDescriptor.get(), MTL::PipelineOptionNone, nullptr, &createPipelineElevenError), [](MTL::ComputePipelineState *stepElevenPipeline) noexcept {
		
			// Free step eleven pipeline
			stepElevenPipeline->release();
		});
		stepTwelvePipeline = unique_ptr<MTL::ComputePipelineState, void(*)(MTL::ComputePipelineState *)>(device->newComputePipelineState(stepTwelvePipelineDescriptor.get(), MTL::PipelineOptionNone, nullptr, &createPipelineTwelveError), [](MTL::ComputePipelineState *stepTwelvePipeline) noexcept {
		
			// Free step twelve pipeline
			stepTwelvePipeline->release();
		});
		stepThirteenPipeline = unique_ptr<MTL::ComputePipelineState, void(*)(MTL::ComputePipelineState *)>(device->newComputePipelineState(stepThirteenPipelineDescriptor.get(), MTL::PipelineOptionNone, nullptr, &createPipelineThirteenError), [](MTL::ComputePipelineState *stepThirteenPipeline) noexcept {
		
			// Free step thirteen pipeline
			stepThirteenPipeline->release();
		});
		stepFourteenPipeline = unique_ptr<MTL::ComputePipelineState, void(*)(MTL::ComputePipelineState *)>(device->newComputePipelineState(stepFourteenPipelineDescriptor.get(), MTL::PipelineOptionNone, nullptr, &createPipelineFourteenError), [](MTL::ComputePipelineState *stepFourteenPipeline) noexcept {
		
			// Free step fourteen pipeline
			stepFourteenPipeline->release();
		});
		stepFifteenPipeline = unique_ptr<MTL::ComputePipelineState, void(*)(MTL::ComputePipelineState *)>(device->newComputePipelineState(stepFifteenPipelineDescriptor.get(), MTL::PipelineOptionNone, nullptr, &createPipelineFifteenError), [](MTL::ComputePipelineState *stepFifteenPipeline) noexcept {
		
			// Free step fifteen pipeline
			stepFifteenPipeline->release();
		});
		stepSixteenPipeline = unique_ptr<MTL::ComputePipelineState, void(*)(MTL::ComputePipelineState *)>(device->newComputePipelineState(stepSixteenPipelineDescriptor.get(), MTL::PipelineOptionNone, nullptr, &createPipelineSixteenError), [](MTL::ComputePipelineState *stepSixteenPipeline) noexcept {
		
			// Free step sixteen pipeline
			stepSixteenPipeline->release();
		});
		stepSeventeenPipeline = unique_ptr<MTL::ComputePipelineState, void(*)(MTL::ComputePipelineState *)>(device->newComputePipelineState(stepSeventeenPipelineDescriptor.get(), MTL::PipelineOptionNone, nullptr, &createPipelineSeventeenError), [](MTL::ComputePipelineState *stepSeventeenPipeline) noexcept {
		
			// Free step seventeen pipeline
			stepSeventeenPipeline->release();
		});
		stepEighteenPipeline = unique_ptr<MTL::ComputePipelineState, void(*)(MTL::ComputePipelineState *)>(device->newComputePipelineState(stepEighteenPipelineDescriptor.get(), MTL::PipelineOptionNone, nullptr, &createPipelineEighteenError), [](MTL::ComputePipelineState *stepEighteenPipeline) noexcept {
		
			// Free step eighteen pipeline
			stepEighteenPipeline->release();
		});
		clearNumberOfEdgesPerSourceBucketPipeline = unique_ptr<MTL::ComputePipelineState, void(*)(MTL::ComputePipelineState *)>(device->newComputePipelineState(clearNumberOfEdgesPerSourceBucketPipelineDescriptor.get(), MTL::PipelineOptionNone, nullptr, &createPipelineClearNumberOfEdgesPerSourceBucketError), [](MTL::ComputePipelineState *clearNumberOfEdgesPerSourceBucketPipeline) noexcept {
		
			// Free clear number of edges per source bucket pipeline
			clearNumberOfEdgesPerSourceBucketPipeline->release();
		});
		clearNumberOfEdgesPerDestinationBucketPipeline = unique_ptr<MTL::ComputePipelineState, void(*)(MTL::ComputePipelineState *)>(device->newComputePipelineState(clearNumberOfEdgesPerDestinationBucketPipelineDescriptor.get(), MTL::PipelineOptionNone, nullptr, &createPipelineClearNumberOfEdgesPerDestinationBucketError), [](MTL::ComputePipelineState *clearNumberOfEdgesPerDestinationBucketPipeline) noexcept {
		
			// Free clear number of edges per destination bucket pipeline
			clearNumberOfEdgesPerDestinationBucketPipeline->release();
		});
		if(!stepOnePipeline || !stepTwoPipeline || !stepThreePipeline || !stepFourPipeline || !stepFivePipeline || !stepSixPipeline || !stepSevenPipeline || !stepEightPipeline || !stepNinePipeline || !stepTenPipeline || !stepElevenPipeline || !stepTwelvePipeline || !stepThirteenPipeline || !stepFourteenPipeline || !stepFifteenPipeline || !stepSixteenPipeline || !stepSeventeenPipeline || !stepEighteenPipeline || !clearNumberOfEdgesPerSourceBucketPipeline || !clearNumberOfEdgesPerDestinationBucketPipeline) {
		
			// Display message
			cout << "Creating pipelines for the GPU failed" << endl;
			
			// Check if recreating pipeline one failed and an error exists
			if(!stepOnePipeline && createPipelineOneError) {
			
				// Check if error's localized description exists
				const NS::String *localizedDescription = createPipelineOneError->localizedDescription();
				if(localizedDescription) {
				
					// Check if localized description's UTF-8 string exists
					const char *utf8String = localizedDescription->utf8String();
					if(utf8String) {
					
						// Display message
						cout << utf8String << endl;
					}
				}
			}
			
			// Check if recreating pipeline two failed and an error exists
			if(!stepTwoPipeline && createPipelineTwoError) {
			
				// Check if error's localized description exists
				const NS::String *localizedDescription = createPipelineTwoError->localizedDescription();
				if(localizedDescription) {
				
					// Check if localized description's UTF-8 string exists
					const char *utf8String = localizedDescription->utf8String();
					if(utf8String) {
					
						// Display message
						cout << utf8String << endl;
					}
				}
			}
			
			// Check if recreating pipeline three failed and an error exists
			if(!stepThreePipeline && createPipelineThreeError) {
			
				// Check if error's localized description exists
				const NS::String *localizedDescription = createPipelineThreeError->localizedDescription();
				if(localizedDescription) {
				
					// Check if localized description's UTF-8 string exists
					const char *utf8String = localizedDescription->utf8String();
					if(utf8String) {
					
						// Display message
						cout << utf8String << endl;
					}
				}
			}
			
			// Check if recreating pipeline four failed and an error exists
			if(!stepFourPipeline && createPipelineFourError) {
			
				// Check if error's localized description exists
				const NS::String *localizedDescription = createPipelineFourError->localizedDescription();
				if(localizedDescription) {
				
					// Check if localized description's UTF-8 string exists
					const char *utf8String = localizedDescription->utf8String();
					if(utf8String) {
					
						// Display message
						cout << utf8String << endl;
					}
				}
			}
			
			// Check if recreating pipeline five failed and an error exists
			if(!stepFivePipeline && createPipelineFiveError) {
			
				// Check if error's localized description exists
				const NS::String *localizedDescription = createPipelineFiveError->localizedDescription();
				if(localizedDescription) {
				
					// Check if localized description's UTF-8 string exists
					const char *utf8String = localizedDescription->utf8String();
					if(utf8String) {
					
						// Display message
						cout << utf8String << endl;
					}
				}
			}
			
			// Check if recreating pipeline six failed and an error exists
			if(!stepSixPipeline && createPipelineSixError) {
			
				// Check if error's localized description exists
				const NS::String *localizedDescription = createPipelineSixError->localizedDescription();
				if(localizedDescription) {
				
					// Check if localized description's UTF-8 string exists
					const char *utf8String = localizedDescription->utf8String();
					if(utf8String) {
					
						// Display message
						cout << utf8String << endl;
					}
				}
			}
			
			// Check if recreating pipeline seven failed and an error exists
			if(!stepSevenPipeline && createPipelineSevenError) {
			
				// Check if error's localized description exists
				const NS::String *localizedDescription = createPipelineSevenError->localizedDescription();
				if(localizedDescription) {
				
					// Check if localized description's UTF-8 string exists
					const char *utf8String = localizedDescription->utf8String();
					if(utf8String) {
					
						// Display message
						cout << utf8String << endl;
					}
				}
			}
			
			// Check if recreating pipeline eight failed and an error exists
			if(!stepEightPipeline && createPipelineEightError) {
			
				// Check if error's localized description exists
				const NS::String *localizedDescription = createPipelineEightError->localizedDescription();
				if(localizedDescription) {
				
					// Check if localized description's UTF-8 string exists
					const char *utf8String = localizedDescription->utf8String();
					if(utf8String) {
					
						// Display message
						cout << utf8String << endl;
					}
				}
			}
			
			// Check if recreating pipeline nine failed and an error exists
			if(!stepNinePipeline && createPipelineNineError) {
			
				// Check if error's localized description exists
				const NS::String *localizedDescription = createPipelineNineError->localizedDescription();
				if(localizedDescription) {
				
					// Check if localized description's UTF-8 string exists
					const char *utf8String = localizedDescription->utf8String();
					if(utf8String) {
					
						// Display message
						cout << utf8String << endl;
					}
				}
			}
			
			// Check if recreating pipeline ten failed and an error exists
			if(!stepTenPipeline && createPipelineTenError) {
			
				// Check if error's localized description exists
				const NS::String *localizedDescription = createPipelineTenError->localizedDescription();
				if(localizedDescription) {
				
					// Check if localized description's UTF-8 string exists
					const char *utf8String = localizedDescription->utf8String();
					if(utf8String) {
					
						// Display message
						cout << utf8String << endl;
					}
				}
			}
			
			// Check if recreating pipeline eleven failed and an error exists
			if(!stepElevenPipeline && createPipelineElevenError) {
			
				// Check if error's localized description exists
				const NS::String *localizedDescription = createPipelineElevenError->localizedDescription();
				if(localizedDescription) {
				
					// Check if localized description's UTF-8 string exists
					const char *utf8String = localizedDescription->utf8String();
					if(utf8String) {
					
						// Display message
						cout << utf8String << endl;
					}
				}
			}
			
			// Check if recreating pipeline twelve failed and an error exists
			if(!stepTwelvePipeline && createPipelineTwelveError) {
			
				// Check if error's localized description exists
				const NS::String *localizedDescription = createPipelineTwelveError->localizedDescription();
				if(localizedDescription) {
				
					// Check if localized description's UTF-8 string exists
					const char *utf8String = localizedDescription->utf8String();
					if(utf8String) {
					
						// Display message
						cout << utf8String << endl;
					}
				}
			}
			
			// Check if recreating pipeline thirteen failed and an error exists
			if(!stepThirteenPipeline && createPipelineThirteenError) {
			
				// Check if error's localized description exists
				const NS::String *localizedDescription = createPipelineThirteenError->localizedDescription();
				if(localizedDescription) {
				
					// Check if localized description's UTF-8 string exists
					const char *utf8String = localizedDescription->utf8String();
					if(utf8String) {
					
						// Display message
						cout << utf8String << endl;
					}
				}
			}
			
			// Check if recreating pipeline fourteen failed and an error exists
			if(!stepFourteenPipeline && createPipelineFourteenError) {
			
				// Check if error's localized description exists
				const NS::String *localizedDescription = createPipelineFourteenError->localizedDescription();
				if(localizedDescription) {
				
					// Check if localized description's UTF-8 string exists
					const char *utf8String = localizedDescription->utf8String();
					if(utf8String) {
					
						// Display message
						cout << utf8String << endl;
					}
				}
			}
			
			// Check if recreating pipeline fifteen failed and an error exists
			if(!stepFifteenPipeline && createPipelineFifteenError) {
			
				// Check if error's localized description exists
				const NS::String *localizedDescription = createPipelineFifteenError->localizedDescription();
				if(localizedDescription) {
				
					// Check if localized description's UTF-8 string exists
					const char *utf8String = localizedDescription->utf8String();
					if(utf8String) {
					
						// Display message
						cout << utf8String << endl;
					}
				}
			}
			
			// Check if recreating pipeline sixteen failed and an error exists
			if(!stepSixteenPipeline && createPipelineSixteenError) {
			
				// Check if error's localized description exists
				const NS::String *localizedDescription = createPipelineSixteenError->localizedDescription();
				if(localizedDescription) {
				
					// Check if localized description's UTF-8 string exists
					const char *utf8String = localizedDescription->utf8String();
					if(utf8String) {
					
						// Display message
						cout << utf8String << endl;
					}
				}
			}
			
			// Check if recreating pipeline seventeen failed and an error exists
			if(!stepSeventeenPipeline && createPipelineSeventeenError) {
			
				// Check if error's localized description exists
				const NS::String *localizedDescription = createPipelineSeventeenError->localizedDescription();
				if(localizedDescription) {
				
					// Check if localized description's UTF-8 string exists
					const char *utf8String = localizedDescription->utf8String();
					if(utf8String) {
					
						// Display message
						cout << utf8String << endl;
					}
				}
			}
			
			// Check if recreating pipeline eighteen failed and an error exists
			if(!stepEighteenPipeline && createPipelineEighteenError) {
			
				// Check if error's localized description exists
				const NS::String *localizedDescription = createPipelineEighteenError->localizedDescription();
				if(localizedDescription) {
				
					// Check if localized description's UTF-8 string exists
					const char *utf8String = localizedDescription->utf8String();
					if(utf8String) {
					
						// Display message
						cout << utf8String << endl;
					}
				}
			}
			
			// Check if recreating pipeline clear number of edges per source bucket failed and an error exists
			if(!clearNumberOfEdgesPerSourceBucketPipeline && createPipelineClearNumberOfEdgesPerSourceBucketError) {
			
				// Check if error's localized description exists
				const NS::String *localizedDescription = createPipelineClearNumberOfEdgesPerSourceBucketError->localizedDescription();
				if(localizedDescription) {
				
					// Check if localized description's UTF-8 string exists
					const char *utf8String = localizedDescription->utf8String();
					if(utf8String) {
					
						// Display message
						cout << utf8String << endl;
					}
				}
			}
			
			// Check if recreating pipeline clear number of edges per destination bucket failed and an error exists
			if(!clearNumberOfEdgesPerDestinationBucketPipeline && createPipelineClearNumberOfEdgesPerDestinationBucketError) {
			
				// Check if error's localized description exists
				const NS::String *localizedDescription = createPipelineClearNumberOfEdgesPerDestinationBucketError->localizedDescription();
				if(localizedDescription) {
				
					// Check if localized description's UTF-8 string exists
					const char *utf8String = localizedDescription->utf8String();
					if(utf8String) {
					
						// Display message
						cout << utf8String << endl;
					}
				}
			}
			
			// Return false
			return false;
		}
		
		// Check if allocating memory on the device failed
		static const unique_ptr<MTL::Buffer, void(*)(MTL::Buffer *)> bucketsOne(device->newBuffer(static_cast<uint64_t>(SLEAN_TRIMMING_INITIAL_MAX_NUMBER_OF_EDGES_PER_BUCKET) * bucketsOneNumberOfBuckets * sizeof(uint32_t), MTL::ResourceStorageModePrivate | MTL::ResourceHazardTrackingModeUntracked), [](MTL::Buffer *bucketsOne) noexcept {
		
			// Free buckets one
			bucketsOne->release();
		});
		static const unique_ptr<MTL::Buffer, void(*)(MTL::Buffer *)> bucketsOneSecondPart(device->newBuffer(static_cast<uint64_t>(SLEAN_TRIMMING_INITIAL_MAX_NUMBER_OF_EDGES_PER_BUCKET) * (SLEAN_TRIMMING_NUMBER_OF_BUCKETS - bucketsOneNumberOfBuckets) * sizeof(uint32_t), MTL::ResourceStorageModePrivate | MTL::ResourceHazardTrackingModeUntracked), [](MTL::Buffer *bucketsOneSecondPart) noexcept {
		
			// Free buckets one second part
			bucketsOneSecondPart->release();
		});
		static const unique_ptr<MTL::Buffer, void(*)(MTL::Buffer *)> bucketsTwo(device->newBuffer(static_cast<uint64_t>(SLEAN_TRIMMING_AFTER_TRIMMING_ROUND_MAX_NUMBER_OF_EDGES_PER_REMAINING_EDGES_BUCKET) * SLEAN_TRIMMING_NUMBER_OF_REMAINING_EDGES_BUCKETS * sizeof(uint32_t), MTL::ResourceStorageModePrivate | MTL::ResourceHazardTrackingModeUntracked), [](MTL::Buffer *bucketsTwo) noexcept {
		
			// Free buckets two
			bucketsTwo->release();
		});
		static const unique_ptr<MTL::Buffer, void(*)(MTL::Buffer *)> numberOfEdgesPerBucketOne(device->newBuffer(SLEAN_TRIMMING_NUMBER_OF_BUCKETS * sizeof(uint32_t), MTL::ResourceStorageModePrivate | MTL::ResourceHazardTrackingModeUntracked), [](MTL::Buffer *numberOfEdgesPerBucketOne) noexcept {
		
			// Free number of edges per bucket one
			numberOfEdgesPerBucketOne->release();
		});
		static const unique_ptr<MTL::Buffer, void(*)(MTL::Buffer *)> numberOfEdgesPerBucketTwo(device->newBuffer(SLEAN_TRIMMING_NUMBER_OF_REMAINING_EDGES_BUCKETS * sizeof(uint32_t), MTL::ResourceStorageModePrivate | MTL::ResourceHazardTrackingModeUntracked), [](MTL::Buffer *numberOfEdgesPerBucketTwo) noexcept {
		
			// Free number of edges per bucket two
			numberOfEdgesPerBucketTwo->release();
		});
		static const unique_ptr<MTL::Buffer, void(*)(MTL::Buffer *)> edgesBitmapOne(device->newBuffer(NUMBER_OF_EDGES / BITS_IN_A_BYTE, MTL::ResourceStorageModeShared | MTL::ResourceHazardTrackingModeUntracked), [](MTL::Buffer *edgesBitmapOne) noexcept {
		
			// Free edges bitmap one
			edgesBitmapOne->release();
		});
		static const unique_ptr<MTL::Buffer, void(*)(MTL::Buffer *)> edgesBitmapTwo(device->newBuffer(NUMBER_OF_EDGES / BITS_IN_A_BYTE, MTL::ResourceStorageModeShared | MTL::ResourceHazardTrackingModeUntracked), [](MTL::Buffer *edgesBitmapTwo) noexcept {
		
			// Free edges bitmap two
			edgesBitmapTwo->release();
		});
		static const unique_ptr<MTL::Buffer, void(*)(MTL::Buffer *)> nodesBitmap(device->newBuffer(NUMBER_OF_EDGES / BITS_IN_A_BYTE, MTL::ResourceStorageModePrivate | MTL::ResourceHazardTrackingModeUntracked), [](MTL::Buffer *nodesBitmap) noexcept {
		
			// Free nodes bitmap
			nodesBitmap->release();
		});
		if(!bucketsOne || (bucketsOneNumberOfBuckets != SLEAN_TRIMMING_NUMBER_OF_BUCKETS && !bucketsOneSecondPart) || !bucketsTwo || !numberOfEdgesPerBucketOne || !numberOfEdgesPerBucketTwo || !edgesBitmapOne || !edgesBitmapTwo || !nodesBitmap) {
		
			// Display message
			cout << "Allocating memory on the GPU failed" << endl;
			
			// Return false
			return false;
		}
		
		// Check if creating command queue for the device failed
		static const unique_ptr<MTL::CommandQueue, void(*)(MTL::CommandQueue *)> commandQueue(device->newCommandQueue(), [](MTL::CommandQueue *commandQueue) noexcept {
		
			// Free command queue
			commandQueue->release();
		});
		if(!commandQueue) {
		
			// Display message
			cout << "Creating command queue for the GPU failed" << endl;
			
			// Return false
			return false;
		}
		
		// Display message
		cout << "Mining started" << endl << endl << "Mining info:" << endl << "\tMining rate:\t 0 graph(s)/second" << endl << "\tGraphs checked:\t 0" << endl;
		
		// Check if not tuning
		#ifndef TUNING
		
			// Display message
			cout << "\tSolutions found: 0" << endl;
		#endif
		
		// Display message
		cout << "Pipeline stages:" << endl;
		
		// Set previous graph processed time to now
		previousGraphProcessedTime = chrono::high_resolution_clock::now();
		
		// Set start time to now
		chrono::high_resolution_clock::time_point startTime = previousGraphProcessedTime;
		
		// Check if creating autorelease pool failed
		static unique_ptr<NS::AutoreleasePool, void(*)(NS::AutoreleasePool *)> autoreleasePool(NS::AutoreleasePool::alloc()->init(), [](NS::AutoreleasePool *autoreleasePool) noexcept {
		
			// Free autorelease pool
			autoreleasePool->release();
		});
		if(!autoreleasePool) {
		
			// Display message
			cout << "Creating autorelease pool failed" << endl;
			
			// Return false
			return false;
		}
		
		// Check if creating command queue's command buffer failed
		MTL::CommandBuffer *commandBuffer = commandQueue->commandBuffer();
		if(!commandBuffer) {
		
			// Display message
			cout << "Creating command queue's command buffer failed" << endl;
			
			// Return false
			return false;
		}
		
		// Check if creating command buffer's compute pass encoder failed
		MTL::ComputeCommandEncoder *computePassEncoder = commandBuffer->computeCommandEncoder();
		if(!computePassEncoder) {
		
			// Display message
			cout << "Creating command buffer's compute pass encoder failed" << endl;
			
			// Return false
			return false;
		}
		
		// Get SipHash keys from job's header and nonce
		uint64_t __attribute__((vector_size(sizeof(uint64_t) * SIPHASH_KEYS_SIZE))) sipHashKeysOne;
		uint64_t heightOne = jobHeight;
		uint64_t idOne = jobId;
		uint64_t nonceOne = jobNonce++;
		blake2b(sipHashKeysOne, jobHeader, nonceOne);
		
		// Set compute pass's buckets, number of edges per bucket, SipHash keys, nodes bitmap, destination buckets, number of edges per destination bucket, and edges bitmap arguments
		computePassEncoder->setBuffer(bucketsOne.get(), 0, 0);
		computePassEncoder->setBuffer(numberOfEdgesPerBucketOne.get(), 0, 1);
		computePassEncoder->setBytes(&sipHashKeysOne, sizeof(sipHashKeysOne), 3);
		computePassEncoder->setBuffer(nodesBitmap.get(), 0, 5);
		computePassEncoder->setBuffer(bucketsTwo.get(), 0, 6);
		computePassEncoder->setBuffer(numberOfEdgesPerBucketTwo.get(), 0, 7);
		computePassEncoder->setBuffer(edgesBitmapOne.get(), 0, 8);
		
		// Check if using buckets one second part
		if(bucketsOneNumberOfBuckets != SLEAN_TRIMMING_NUMBER_OF_BUCKETS) {
		
			// Set compute pass's buckets second part argument
			computePassEncoder->setBuffer(bucketsOneSecondPart.get(), 0, 4);
		}
		
		// Add clearing number of edges per bucket one to the compute pass
		computePassEncoder->setComputePipelineState(clearNumberOfEdgesPerSourceBucketPipeline.get());
		computePassEncoder->dispatchThreads(totalNumberOfWorkItems[18], workItemsPerWorkGroup[18]);
		
		// Set compute pass's part argument
		computePassEncoder->setBytes(&unmove(static_cast<uint8_t>(0)), sizeof(uint8_t), 2);
		
		// Add running step one to the compute pass
		computePassEncoder->setComputePipelineState(stepOnePipeline.get());
		computePassEncoder->dispatchThreads(totalNumberOfWorkItems[0], workItemsPerWorkGroup[0]);
		
		// Add running step two to the compute pass
		computePassEncoder->setComputePipelineState(stepTwoPipeline.get());
		computePassEncoder->dispatchThreads(totalNumberOfWorkItems[1], workItemsPerWorkGroup[1]);
		
		// Go through all remaining trimming parts
		for(unsigned int i = 1; i < SLEAN_TRIMMING_PARTS; ++i) {
		
			// Add clearing number of edges per bucket one to the compute pass
			computePassEncoder->setComputePipelineState(clearNumberOfEdgesPerSourceBucketPipeline.get());
			computePassEncoder->dispatchThreads(totalNumberOfWorkItems[18], workItemsPerWorkGroup[18]);
			
			// Set compute pass's part argument
			computePassEncoder->setBytes(&unmove(static_cast<uint8_t>(i)), sizeof(uint8_t), 2);
			
			// Add running step one to the compute pass
			computePassEncoder->setComputePipelineState(stepOnePipeline.get());
			computePassEncoder->dispatchThreads(totalNumberOfWorkItems[0], workItemsPerWorkGroup[0]);
			
			// Check if not the last trimming part
			if(i != SLEAN_TRIMMING_PARTS - 1) {
			
				// Add running step three to the compute pass
				computePassEncoder->setComputePipelineState(stepThreePipeline.get());
				computePassEncoder->dispatchThreads(totalNumberOfWorkItems[2], workItemsPerWorkGroup[2]);
			}
			
			// Otherwise
			else {
			
				// Add clearing number of edges per bucket two to the compute pass
				computePassEncoder->setComputePipelineState(clearNumberOfEdgesPerDestinationBucketPipeline.get());
				computePassEncoder->dispatchThreads(totalNumberOfWorkItems[19], workItemsPerWorkGroup[19]);
				
				// Add running step four to the compute pass
				computePassEncoder->setComputePipelineState(stepFourPipeline.get());
				computePassEncoder->dispatchThreads(totalNumberOfWorkItems[3], workItemsPerWorkGroup[3]);
			}
		}
		
		// Add running step five to the compute pass
		computePassEncoder->setComputePipelineState(stepFivePipeline.get());
		computePassEncoder->dispatchThreads(totalNumberOfWorkItems[4], workItemsPerWorkGroup[4]);
		
		// Go through all remaining trimming parts except the last one
		for(unsigned int i = 0; i < SLEAN_TRIMMING_PARTS - 1; ++i) {
		
			// Add clearing number of edges per bucket one to the compute pass
			computePassEncoder->setComputePipelineState(clearNumberOfEdgesPerSourceBucketPipeline.get());
			computePassEncoder->dispatchThreads(totalNumberOfWorkItems[18], workItemsPerWorkGroup[18]);
			
			// Set compute pass's part argument
			computePassEncoder->setBytes(&unmove(static_cast<uint8_t>(i)), sizeof(uint8_t), 2);
			
			// Add running step one to the compute pass
			computePassEncoder->setComputePipelineState(stepOnePipeline.get());
			computePassEncoder->dispatchThreads(totalNumberOfWorkItems[0], workItemsPerWorkGroup[0]);
			
			// Add clearing number of edges per bucket two to the compute pass
			computePassEncoder->setComputePipelineState(clearNumberOfEdgesPerDestinationBucketPipeline.get());
			computePassEncoder->dispatchThreads(totalNumberOfWorkItems[19], workItemsPerWorkGroup[19]);
			
			// Add running step six to the compute pass
			computePassEncoder->setComputePipelineState(stepSixPipeline.get());
			computePassEncoder->dispatchThreads(totalNumberOfWorkItems[5], workItemsPerWorkGroup[5]);
			
			// Add running step five to the compute pass
			computePassEncoder->setComputePipelineState(stepFivePipeline.get());
			computePassEncoder->dispatchThreads(totalNumberOfWorkItems[4], workItemsPerWorkGroup[4]);
		}
		
		// Check if trimming more than one round
		if(TRIMMING_ROUNDS > 1) {
		
			// Add clearing number of edges per bucket one to the compute pass
			computePassEncoder->setComputePipelineState(clearNumberOfEdgesPerSourceBucketPipeline.get());
			computePassEncoder->dispatchThreads(totalNumberOfWorkItems[18], workItemsPerWorkGroup[18]);
			
			// Set compute pass's part argument
			computePassEncoder->setBytes(&unmove(static_cast<uint8_t>(0)), sizeof(uint8_t), 2);
			
			// Add running step seven to the compute pass
			computePassEncoder->setComputePipelineState(stepSevenPipeline.get());
			computePassEncoder->dispatchThreads(totalNumberOfWorkItems[6], workItemsPerWorkGroup[6]);
			
			// Add running step eight to the compute pass
			computePassEncoder->setComputePipelineState(stepEightPipeline.get());
			computePassEncoder->dispatchThreads(totalNumberOfWorkItems[7], workItemsPerWorkGroup[7]);
			
			// Go through all remaining trimming parts
			for(unsigned int i = 1; i < SLEAN_TRIMMING_PARTS; ++i) {
			
				// Add clearing number of edges per bucket one to the compute pass
				computePassEncoder->setComputePipelineState(clearNumberOfEdgesPerSourceBucketPipeline.get());
				computePassEncoder->dispatchThreads(totalNumberOfWorkItems[18], workItemsPerWorkGroup[18]);
				
				// Set compute pass's part argument
				computePassEncoder->setBytes(&unmove(static_cast<uint8_t>(i)), sizeof(uint8_t), 2);
				
				// Add running step seven to the compute pass
				computePassEncoder->setComputePipelineState(stepSevenPipeline.get());
				computePassEncoder->dispatchThreads(totalNumberOfWorkItems[6], workItemsPerWorkGroup[6]);
				
				// Check if not the last trimming part
				if(i != SLEAN_TRIMMING_PARTS - 1) {
				
					// Add running step nine to the compute pass
					computePassEncoder->setComputePipelineState(stepNinePipeline.get());
					computePassEncoder->dispatchThreads(totalNumberOfWorkItems[8], workItemsPerWorkGroup[8]);
				}
				
				// Otherwise
				else {
				
					// Add clearing number of edges per bucket two to the compute pass
					computePassEncoder->setComputePipelineState(clearNumberOfEdgesPerDestinationBucketPipeline.get());
					computePassEncoder->dispatchThreads(totalNumberOfWorkItems[19], workItemsPerWorkGroup[19]);
					
					// Add running step ten to the compute pass
					computePassEncoder->setComputePipelineState(stepTenPipeline.get());
					computePassEncoder->dispatchThreads(totalNumberOfWorkItems[9], workItemsPerWorkGroup[9]);
				}
			}
			
			// Add running step eleven to the compute pass
			computePassEncoder->setComputePipelineState(stepElevenPipeline.get());
			computePassEncoder->dispatchThreads(totalNumberOfWorkItems[10], workItemsPerWorkGroup[10]);
			
			// Go through all remaining trimming parts except the last one
			for(unsigned int i = 0; i < SLEAN_TRIMMING_PARTS - 1; ++i) {
			
				// Add clearing number of edges per bucket one to the compute pass
				computePassEncoder->setComputePipelineState(clearNumberOfEdgesPerSourceBucketPipeline.get());
				computePassEncoder->dispatchThreads(totalNumberOfWorkItems[18], workItemsPerWorkGroup[18]);
				
				// Set compute pass's part argument
				computePassEncoder->setBytes(&unmove(static_cast<uint8_t>(i)), sizeof(uint8_t), 2);
				
				// Add running step seven to the compute pass
				computePassEncoder->setComputePipelineState(stepSevenPipeline.get());
				computePassEncoder->dispatchThreads(totalNumberOfWorkItems[6], workItemsPerWorkGroup[6]);
				
				// Add clearing number of edges per bucket two to the compute pass
				computePassEncoder->setComputePipelineState(clearNumberOfEdgesPerDestinationBucketPipeline.get());
				computePassEncoder->dispatchThreads(totalNumberOfWorkItems[19], workItemsPerWorkGroup[19]);
				
				// Add running step twelve to the compute pass
				computePassEncoder->setComputePipelineState(stepTwelvePipeline.get());
				computePassEncoder->dispatchThreads(totalNumberOfWorkItems[11], workItemsPerWorkGroup[11]);
				
				// Add running step eleven to the compute pass
				computePassEncoder->setComputePipelineState(stepElevenPipeline.get());
				computePassEncoder->dispatchThreads(totalNumberOfWorkItems[10], workItemsPerWorkGroup[10]);
			}
			
			// Go through all remaining trimming rounds
			for(unsigned int i = 2; i < TRIMMING_ROUNDS; ++i) {
			
				// Add clearing number of edges per bucket one to the compute pass
				computePassEncoder->setComputePipelineState(clearNumberOfEdgesPerSourceBucketPipeline.get());
				computePassEncoder->dispatchThreads(totalNumberOfWorkItems[18], workItemsPerWorkGroup[18]);
				
				// Set compute pass's part and nodes in second partition arguments
				computePassEncoder->setBytes(&unmove(static_cast<uint8_t>(0)), sizeof(uint8_t), 2);
				computePassEncoder->setBytes(&unmove(static_cast<uint8_t>(i % 2)), sizeof(uint8_t), 9);
				
				// Add running step thirteen to the compute pass
				computePassEncoder->setComputePipelineState(stepThirteenPipeline.get());
				computePassEncoder->dispatchThreads(totalNumberOfWorkItems[12], workItemsPerWorkGroup[12]);
				
				// Add running step fourteen to the compute pass
				computePassEncoder->setComputePipelineState(stepFourteenPipeline.get());
				computePassEncoder->dispatchThreads(totalNumberOfWorkItems[13], workItemsPerWorkGroup[13]);
				
				// Go through all remaining trimming parts
				for(unsigned int j = 1; j < SLEAN_TRIMMING_PARTS; ++j) {
				
					// Add clearing number of edges per bucket one to the compute pass
					computePassEncoder->setComputePipelineState(clearNumberOfEdgesPerSourceBucketPipeline.get());
					computePassEncoder->dispatchThreads(totalNumberOfWorkItems[18], workItemsPerWorkGroup[18]);
					
					// Set compute pass's part argument
					computePassEncoder->setBytes(&unmove(static_cast<uint8_t>(j)), sizeof(uint8_t), 2);
					
					// Add running step thirteen to the compute pass
					computePassEncoder->setComputePipelineState(stepThirteenPipeline.get());
					computePassEncoder->dispatchThreads(totalNumberOfWorkItems[12], workItemsPerWorkGroup[12]);
					
					// Check if not the last trimming part
					if(j != SLEAN_TRIMMING_PARTS - 1) {
					
						// Add running step fifteen to the compute pass
						computePassEncoder->setComputePipelineState(stepFifteenPipeline.get());
						computePassEncoder->dispatchThreads(totalNumberOfWorkItems[14], workItemsPerWorkGroup[14]);
					}
					
					// Otherwise
					else {
					
						// Add clearing number of edges per bucket two to the compute pass
						computePassEncoder->setComputePipelineState(clearNumberOfEdgesPerDestinationBucketPipeline.get());
						computePassEncoder->dispatchThreads(totalNumberOfWorkItems[19], workItemsPerWorkGroup[19]);
						
						// Add running step sixteen to the compute pass
						computePassEncoder->setComputePipelineState(stepSixteenPipeline.get());
						computePassEncoder->dispatchThreads(totalNumberOfWorkItems[15], workItemsPerWorkGroup[15]);
					}
				}
				
				// Add running step seventeen to the compute pass
				computePassEncoder->setComputePipelineState(stepSeventeenPipeline.get());
				computePassEncoder->dispatchThreads(totalNumberOfWorkItems[16], workItemsPerWorkGroup[16]);
				
				// Go through all remaining trimming parts except the last one
				for(unsigned int j = 0; j < SLEAN_TRIMMING_PARTS - 1; ++j) {
				
					// Add clearing number of edges per bucket one to the compute pass
					computePassEncoder->setComputePipelineState(clearNumberOfEdgesPerSourceBucketPipeline.get());
					computePassEncoder->dispatchThreads(totalNumberOfWorkItems[18], workItemsPerWorkGroup[18]);
					
					// Set compute pass's part argument
					computePassEncoder->setBytes(&unmove(static_cast<uint8_t>(j)), sizeof(uint8_t), 2);
					
					// Add running step thirteen to the compute pass
					computePassEncoder->setComputePipelineState(stepThirteenPipeline.get());
					computePassEncoder->dispatchThreads(totalNumberOfWorkItems[12], workItemsPerWorkGroup[12]);
					
					// Add clearing number of edges per bucket two to the compute pass
					computePassEncoder->setComputePipelineState(clearNumberOfEdgesPerDestinationBucketPipeline.get());
					computePassEncoder->dispatchThreads(totalNumberOfWorkItems[19], workItemsPerWorkGroup[19]);
					
					// Add running step eighteen to the compute pass
					computePassEncoder->setComputePipelineState(stepEighteenPipeline.get());
					computePassEncoder->dispatchThreads(totalNumberOfWorkItems[17], workItemsPerWorkGroup[17]);
					
					// Add running step seventeen to the compute pass
					computePassEncoder->setComputePipelineState(stepSeventeenPipeline.get());
					computePassEncoder->dispatchThreads(totalNumberOfWorkItems[16], workItemsPerWorkGroup[16]);
				}
			}
		}
		
		// Finish adding commands to the compute pass
		computePassEncoder->endEncoding();
		
		// Set end time to now
		chrono::high_resolution_clock::time_point endTime = chrono::high_resolution_clock::now();
		
		// Run the compute pass
		commandBuffer->commit();
		
		// Check if closing
		if(closing) {
			
			// Return true
			return true;
		}
		
		// Wait until the compute pass has finished
		commandBuffer->waitUntilCompleted();
		
		// Check if running compute pass failed
		if(commandBuffer->error()) {
		
			// Display message
			cout << "Running compute pass failed" << endl;
			
			// Return false
			return false;
		}
		
		// Display message
		cout << "\tTrimming time:\t " << (commandBuffer->GPUEndTime() - commandBuffer->kernelStartTime() + static_cast<chrono::duration<double>>(endTime - startTime).count()) << " second(s)" << endl;
		
		// Set start time to now
		startTime = chrono::high_resolution_clock::now();
		
		// Free objects with autorelease memory
		autoreleasePool.reset();
		
		// Check if recreating autorelease pool failed
		autoreleasePool = unique_ptr<NS::AutoreleasePool, void(*)(NS::AutoreleasePool *)>(NS::AutoreleasePool::alloc()->init(), [](NS::AutoreleasePool *autoreleasePool) noexcept {
		
			// Free autorelease pool
			autoreleasePool->release();
		});
		if(!autoreleasePool) {
		
			// Display message
			cout << "Creating autorelease pool failed" << endl;
			
			// Return false
			return false;
		}
		
		// Check if creating command queue's command buffer failed
		commandBuffer = commandQueue->commandBuffer();
		if(!commandBuffer) {
		
			// Display message
			cout << "Creating command queue's command buffer failed" << endl;
			
			// Return false
			return false;
		}
		
		// Check if creating command buffer's compute pass encoder failed
		computePassEncoder = commandBuffer->computeCommandEncoder();
		if(!computePassEncoder) {
		
			// Display message
			cout << "Creating command buffer's compute pass encoder failed" << endl;
			
			// Return false
			return false;
		}
		
		// Get SipHash keys from job's header and nonce
		uint64_t __attribute__((vector_size(sizeof(uint64_t) * SIPHASH_KEYS_SIZE))) sipHashKeysTwo;
		uint64_t heightTwo = jobHeight;
		uint64_t idTwo = jobId;
		uint64_t nonceTwo = jobNonce++;
		blake2b(sipHashKeysTwo, jobHeader, nonceTwo);
		
		// Set compute pass's buckets, number of edges per bucket, SipHash keys, nodes bitmap, destination buckets, number of edges per destination bucket, and edges bitmap arguments
		computePassEncoder->setBuffer(bucketsOne.get(), 0, 0);
		computePassEncoder->setBuffer(numberOfEdgesPerBucketOne.get(), 0, 1);
		computePassEncoder->setBytes(&sipHashKeysTwo, sizeof(sipHashKeysTwo), 3);
		computePassEncoder->setBuffer(nodesBitmap.get(), 0, 5);
		computePassEncoder->setBuffer(bucketsTwo.get(), 0, 6);
		computePassEncoder->setBuffer(numberOfEdgesPerBucketTwo.get(), 0, 7);
		computePassEncoder->setBuffer(edgesBitmapTwo.get(), 0, 8);
		
		// Check if using buckets one second part
		if(bucketsOneNumberOfBuckets != SLEAN_TRIMMING_NUMBER_OF_BUCKETS) {
		
			// Set compute pass's buckets second part argument
			computePassEncoder->setBuffer(bucketsOneSecondPart.get(), 0, 4);
		}
		
		// Add clearing number of edges per bucket one to the compute pass
		computePassEncoder->setComputePipelineState(clearNumberOfEdgesPerSourceBucketPipeline.get());
		computePassEncoder->dispatchThreads(totalNumberOfWorkItems[18], workItemsPerWorkGroup[18]);
		
		// Set compute pass's part argument
		computePassEncoder->setBytes(&unmove(static_cast<uint8_t>(0)), sizeof(uint8_t), 2);
		
		// Add running step one to the compute pass
		computePassEncoder->setComputePipelineState(stepOnePipeline.get());
		computePassEncoder->dispatchThreads(totalNumberOfWorkItems[0], workItemsPerWorkGroup[0]);
		
		// Add running step two to the compute pass
		computePassEncoder->setComputePipelineState(stepTwoPipeline.get());
		computePassEncoder->dispatchThreads(totalNumberOfWorkItems[1], workItemsPerWorkGroup[1]);
		
		// Go through all remaining trimming parts
		for(unsigned int i = 1; i < SLEAN_TRIMMING_PARTS; ++i) {
		
			// Add clearing number of edges per bucket one to the compute pass
			computePassEncoder->setComputePipelineState(clearNumberOfEdgesPerSourceBucketPipeline.get());
			computePassEncoder->dispatchThreads(totalNumberOfWorkItems[18], workItemsPerWorkGroup[18]);
			
			// Set compute pass's part argument
			computePassEncoder->setBytes(&unmove(static_cast<uint8_t>(i)), sizeof(uint8_t), 2);
			
			// Add running step one to the compute pass
			computePassEncoder->setComputePipelineState(stepOnePipeline.get());
			computePassEncoder->dispatchThreads(totalNumberOfWorkItems[0], workItemsPerWorkGroup[0]);
			
			// Check if not the last trimming part
			if(i != SLEAN_TRIMMING_PARTS - 1) {
			
				// Add running step three to the compute pass
				computePassEncoder->setComputePipelineState(stepThreePipeline.get());
				computePassEncoder->dispatchThreads(totalNumberOfWorkItems[2], workItemsPerWorkGroup[2]);
			}
			
			// Otherwise
			else {
			
				// Add clearing number of edges per bucket two to the compute pass
				computePassEncoder->setComputePipelineState(clearNumberOfEdgesPerDestinationBucketPipeline.get());
				computePassEncoder->dispatchThreads(totalNumberOfWorkItems[19], workItemsPerWorkGroup[19]);
				
				// Add running step four to the compute pass
				computePassEncoder->setComputePipelineState(stepFourPipeline.get());
				computePassEncoder->dispatchThreads(totalNumberOfWorkItems[3], workItemsPerWorkGroup[3]);
			}
		}
		
		// Add running step five to the compute pass
		computePassEncoder->setComputePipelineState(stepFivePipeline.get());
		computePassEncoder->dispatchThreads(totalNumberOfWorkItems[4], workItemsPerWorkGroup[4]);
		
		// Go through all remaining trimming parts except the last one
		for(unsigned int i = 0; i < SLEAN_TRIMMING_PARTS - 1; ++i) {
		
			// Add clearing number of edges per bucket one to the compute pass
			computePassEncoder->setComputePipelineState(clearNumberOfEdgesPerSourceBucketPipeline.get());
			computePassEncoder->dispatchThreads(totalNumberOfWorkItems[18], workItemsPerWorkGroup[18]);
			
			// Set compute pass's part argument
			computePassEncoder->setBytes(&unmove(static_cast<uint8_t>(i)), sizeof(uint8_t), 2);
			
			// Add running step one to the compute pass
			computePassEncoder->setComputePipelineState(stepOnePipeline.get());
			computePassEncoder->dispatchThreads(totalNumberOfWorkItems[0], workItemsPerWorkGroup[0]);
			
			// Add clearing number of edges per bucket two to the compute pass
			computePassEncoder->setComputePipelineState(clearNumberOfEdgesPerDestinationBucketPipeline.get());
			computePassEncoder->dispatchThreads(totalNumberOfWorkItems[19], workItemsPerWorkGroup[19]);
			
			// Add running step six to the compute pass
			computePassEncoder->setComputePipelineState(stepSixPipeline.get());
			computePassEncoder->dispatchThreads(totalNumberOfWorkItems[5], workItemsPerWorkGroup[5]);
			
			// Add running step five to the compute pass
			computePassEncoder->setComputePipelineState(stepFivePipeline.get());
			computePassEncoder->dispatchThreads(totalNumberOfWorkItems[4], workItemsPerWorkGroup[4]);
		}
		
		// Check if trimming more than one round
		if(TRIMMING_ROUNDS > 1) {
		
			// Add clearing number of edges per bucket one to the compute pass
			computePassEncoder->setComputePipelineState(clearNumberOfEdgesPerSourceBucketPipeline.get());
			computePassEncoder->dispatchThreads(totalNumberOfWorkItems[18], workItemsPerWorkGroup[18]);
			
			// Set compute pass's part argument
			computePassEncoder->setBytes(&unmove(static_cast<uint8_t>(0)), sizeof(uint8_t), 2);
			
			// Add running step seven to the compute pass
			computePassEncoder->setComputePipelineState(stepSevenPipeline.get());
			computePassEncoder->dispatchThreads(totalNumberOfWorkItems[6], workItemsPerWorkGroup[6]);
			
			// Add running step eight to the compute pass
			computePassEncoder->setComputePipelineState(stepEightPipeline.get());
			computePassEncoder->dispatchThreads(totalNumberOfWorkItems[7], workItemsPerWorkGroup[7]);
			
			// Go through all remaining trimming parts
			for(unsigned int i = 1; i < SLEAN_TRIMMING_PARTS; ++i) {
			
				// Add clearing number of edges per bucket one to the compute pass
				computePassEncoder->setComputePipelineState(clearNumberOfEdgesPerSourceBucketPipeline.get());
				computePassEncoder->dispatchThreads(totalNumberOfWorkItems[18], workItemsPerWorkGroup[18]);
				
				// Set compute pass's part argument
				computePassEncoder->setBytes(&unmove(static_cast<uint8_t>(i)), sizeof(uint8_t), 2);
				
				// Add running step seven to the compute pass
				computePassEncoder->setComputePipelineState(stepSevenPipeline.get());
				computePassEncoder->dispatchThreads(totalNumberOfWorkItems[6], workItemsPerWorkGroup[6]);
				
				// Check if not the last trimming part
				if(i != SLEAN_TRIMMING_PARTS - 1) {
				
					// Add running step nine to the compute pass
					computePassEncoder->setComputePipelineState(stepNinePipeline.get());
					computePassEncoder->dispatchThreads(totalNumberOfWorkItems[8], workItemsPerWorkGroup[8]);
				}
				
				// Otherwise
				else {
				
					// Add clearing number of edges per bucket two to the compute pass
					computePassEncoder->setComputePipelineState(clearNumberOfEdgesPerDestinationBucketPipeline.get());
					computePassEncoder->dispatchThreads(totalNumberOfWorkItems[19], workItemsPerWorkGroup[19]);
					
					// Add running step ten to the compute pass
					computePassEncoder->setComputePipelineState(stepTenPipeline.get());
					computePassEncoder->dispatchThreads(totalNumberOfWorkItems[9], workItemsPerWorkGroup[9]);
				}
			}
			
			// Add running step eleven to the compute pass
			computePassEncoder->setComputePipelineState(stepElevenPipeline.get());
			computePassEncoder->dispatchThreads(totalNumberOfWorkItems[10], workItemsPerWorkGroup[10]);
			
			// Go through all remaining trimming parts except the last one
			for(unsigned int i = 0; i < SLEAN_TRIMMING_PARTS - 1; ++i) {
			
				// Add clearing number of edges per bucket one to the compute pass
				computePassEncoder->setComputePipelineState(clearNumberOfEdgesPerSourceBucketPipeline.get());
				computePassEncoder->dispatchThreads(totalNumberOfWorkItems[18], workItemsPerWorkGroup[18]);
				
				// Set compute pass's part argument
				computePassEncoder->setBytes(&unmove(static_cast<uint8_t>(i)), sizeof(uint8_t), 2);
				
				// Add running step seven to the compute pass
				computePassEncoder->setComputePipelineState(stepSevenPipeline.get());
				computePassEncoder->dispatchThreads(totalNumberOfWorkItems[6], workItemsPerWorkGroup[6]);
				
				// Add clearing number of edges per bucket two to the compute pass
				computePassEncoder->setComputePipelineState(clearNumberOfEdgesPerDestinationBucketPipeline.get());
				computePassEncoder->dispatchThreads(totalNumberOfWorkItems[19], workItemsPerWorkGroup[19]);
				
				// Add running step twelve to the compute pass
				computePassEncoder->setComputePipelineState(stepTwelvePipeline.get());
				computePassEncoder->dispatchThreads(totalNumberOfWorkItems[11], workItemsPerWorkGroup[11]);
				
				// Add running step eleven to the compute pass
				computePassEncoder->setComputePipelineState(stepElevenPipeline.get());
				computePassEncoder->dispatchThreads(totalNumberOfWorkItems[10], workItemsPerWorkGroup[10]);
			}
			
			// Go through all remaining trimming rounds
			for(unsigned int i = 2; i < TRIMMING_ROUNDS; ++i) {
			
				// Add clearing number of edges per bucket one to the compute pass
				computePassEncoder->setComputePipelineState(clearNumberOfEdgesPerSourceBucketPipeline.get());
				computePassEncoder->dispatchThreads(totalNumberOfWorkItems[18], workItemsPerWorkGroup[18]);
				
				// Set compute pass's part and nodes in second partition arguments
				computePassEncoder->setBytes(&unmove(static_cast<uint8_t>(0)), sizeof(uint8_t), 2);
				computePassEncoder->setBytes(&unmove(static_cast<uint8_t>(i % 2)), sizeof(uint8_t), 9);
				
				// Add running step thirteen to the compute pass
				computePassEncoder->setComputePipelineState(stepThirteenPipeline.get());
				computePassEncoder->dispatchThreads(totalNumberOfWorkItems[12], workItemsPerWorkGroup[12]);
				
				// Add running step fourteen to the compute pass
				computePassEncoder->setComputePipelineState(stepFourteenPipeline.get());
				computePassEncoder->dispatchThreads(totalNumberOfWorkItems[13], workItemsPerWorkGroup[13]);
				
				// Go through all remaining trimming parts
				for(unsigned int j = 1; j < SLEAN_TRIMMING_PARTS; ++j) {
				
					// Add clearing number of edges per bucket one to the compute pass
					computePassEncoder->setComputePipelineState(clearNumberOfEdgesPerSourceBucketPipeline.get());
					computePassEncoder->dispatchThreads(totalNumberOfWorkItems[18], workItemsPerWorkGroup[18]);
					
					// Set compute pass's part argument
					computePassEncoder->setBytes(&unmove(static_cast<uint8_t>(j)), sizeof(uint8_t), 2);
					
					// Add running step thirteen to the compute pass
					computePassEncoder->setComputePipelineState(stepThirteenPipeline.get());
					computePassEncoder->dispatchThreads(totalNumberOfWorkItems[12], workItemsPerWorkGroup[12]);
					
					// Check if not the last trimming part
					if(j != SLEAN_TRIMMING_PARTS - 1) {
					
						// Add running step fifteen to the compute pass
						computePassEncoder->setComputePipelineState(stepFifteenPipeline.get());
						computePassEncoder->dispatchThreads(totalNumberOfWorkItems[14], workItemsPerWorkGroup[14]);
					}
					
					// Otherwise
					else {
					
						// Add clearing number of edges per bucket two to the compute pass
						computePassEncoder->setComputePipelineState(clearNumberOfEdgesPerDestinationBucketPipeline.get());
						computePassEncoder->dispatchThreads(totalNumberOfWorkItems[19], workItemsPerWorkGroup[19]);
						
						// Add running step sixteen to the compute pass
						computePassEncoder->setComputePipelineState(stepSixteenPipeline.get());
						computePassEncoder->dispatchThreads(totalNumberOfWorkItems[15], workItemsPerWorkGroup[15]);
					}
				}
				
				// Add running step seventeen to the compute pass
				computePassEncoder->setComputePipelineState(stepSeventeenPipeline.get());
				computePassEncoder->dispatchThreads(totalNumberOfWorkItems[16], workItemsPerWorkGroup[16]);
				
				// Go through all remaining trimming parts except the last one
				for(unsigned int j = 0; j < SLEAN_TRIMMING_PARTS - 1; ++j) {
				
					// Add clearing number of edges per bucket one to the compute pass
					computePassEncoder->setComputePipelineState(clearNumberOfEdgesPerSourceBucketPipeline.get());
					computePassEncoder->dispatchThreads(totalNumberOfWorkItems[18], workItemsPerWorkGroup[18]);
					
					// Set compute pass's part argument
					computePassEncoder->setBytes(&unmove(static_cast<uint8_t>(j)), sizeof(uint8_t), 2);
					
					// Add running step thirteen to the compute pass
					computePassEncoder->setComputePipelineState(stepThirteenPipeline.get());
					computePassEncoder->dispatchThreads(totalNumberOfWorkItems[12], workItemsPerWorkGroup[12]);
					
					// Add clearing number of edges per bucket two to the compute pass
					computePassEncoder->setComputePipelineState(clearNumberOfEdgesPerDestinationBucketPipeline.get());
					computePassEncoder->dispatchThreads(totalNumberOfWorkItems[19], workItemsPerWorkGroup[19]);
					
					// Add running step eighteen to the compute pass
					computePassEncoder->setComputePipelineState(stepEighteenPipeline.get());
					computePassEncoder->dispatchThreads(totalNumberOfWorkItems[17], workItemsPerWorkGroup[17]);
					
					// Add running step seventeen to the compute pass
					computePassEncoder->setComputePipelineState(stepSeventeenPipeline.get());
					computePassEncoder->dispatchThreads(totalNumberOfWorkItems[16], workItemsPerWorkGroup[16]);
				}
			}
		}
		
		// Finish adding commands to the compute pass
		computePassEncoder->endEncoding();
		
		// Set end time to now
		endTime = chrono::high_resolution_clock::now();
		
		// Run the compute pass
		commandBuffer->commit();
		
		// Trimming finished
		trimmingFinished(edgesBitmapOne->contents(), sipHashKeysOne, heightOne, idOne, nonceOne);
		
		// While not closing
		while(!closing) {
		
			// Wait until the compute pass has finished
			commandBuffer->waitUntilCompleted();
			
			// Check if running compute pass failed
			if(commandBuffer->error()) {
			
				// Display message
				cout << "Running compute pass failed" << endl;
				
				// Return false
				return false;
			}
			
			// Display message
			cout << "\tTrimming time:\t " << (commandBuffer->GPUEndTime() - commandBuffer->kernelStartTime() + static_cast<chrono::duration<double>>(endTime - startTime).count()) << " second(s)" << endl;
			
			// Set start time to now
			startTime = chrono::high_resolution_clock::now();
			
			// Free objects with autorelease memory
			autoreleasePool.reset();
			
			// Check if recreating autorelease pool failed
			autoreleasePool = unique_ptr<NS::AutoreleasePool, void(*)(NS::AutoreleasePool *)>(NS::AutoreleasePool::alloc()->init(), [](NS::AutoreleasePool *autoreleasePool) noexcept {
			
				// Free autorelease pool
				autoreleasePool->release();
			});
			if(!autoreleasePool) {
			
				// Display message
				cout << "Creating autorelease pool failed" << endl;
				
				// Return false
				return false;
			}
			
			// Check if creating command queue's command buffer failed
			commandBuffer = commandQueue->commandBuffer();
			if(!commandBuffer) {
			
				// Display message
				cout << "Creating command queue's command buffer failed" << endl;
				
				// Return false
				return false;
			}
			
			// Check if creating command buffer's compute pass encoder failed
			computePassEncoder = commandBuffer->computeCommandEncoder();
			if(!computePassEncoder) {
			
				// Display message
				cout << "Creating command buffer's compute pass encoder failed" << endl;
				
				// Return false
				return false;
			}
			
			// Get SipHash keys from job's header and nonce
			heightOne = jobHeight;
			idOne = jobId;
			nonceOne = jobNonce++;
			blake2b(sipHashKeysOne, jobHeader, nonceOne);
			
			// Set compute pass's buckets, number of edges per bucket, SipHash keys, nodes bitmap, destination buckets, number of edges per destination bucket, and edges bitmap arguments
			computePassEncoder->setBuffer(bucketsOne.get(), 0, 0);
			computePassEncoder->setBuffer(numberOfEdgesPerBucketOne.get(), 0, 1);
			computePassEncoder->setBytes(&sipHashKeysOne, sizeof(sipHashKeysOne), 3);
			computePassEncoder->setBuffer(nodesBitmap.get(), 0, 5);
			computePassEncoder->setBuffer(bucketsTwo.get(), 0, 6);
			computePassEncoder->setBuffer(numberOfEdgesPerBucketTwo.get(), 0, 7);
			computePassEncoder->setBuffer(edgesBitmapOne.get(), 0, 8);
			
			// Check if using buckets one second part
			if(bucketsOneNumberOfBuckets != SLEAN_TRIMMING_NUMBER_OF_BUCKETS) {
			
				// Set compute pass's buckets second part argument
				computePassEncoder->setBuffer(bucketsOneSecondPart.get(), 0, 4);
			}
			
			// Add clearing number of edges per bucket one to the compute pass
			computePassEncoder->setComputePipelineState(clearNumberOfEdgesPerSourceBucketPipeline.get());
			computePassEncoder->dispatchThreads(totalNumberOfWorkItems[18], workItemsPerWorkGroup[18]);
			
			// Set compute pass's part argument
			computePassEncoder->setBytes(&unmove(static_cast<uint8_t>(0)), sizeof(uint8_t), 2);
			
			// Add running step one to the compute pass
			computePassEncoder->setComputePipelineState(stepOnePipeline.get());
			computePassEncoder->dispatchThreads(totalNumberOfWorkItems[0], workItemsPerWorkGroup[0]);
			
			// Add running step two to the compute pass
			computePassEncoder->setComputePipelineState(stepTwoPipeline.get());
			computePassEncoder->dispatchThreads(totalNumberOfWorkItems[1], workItemsPerWorkGroup[1]);
			
			// Go through all remaining trimming parts
			for(unsigned int i = 1; i < SLEAN_TRIMMING_PARTS; ++i) {
			
				// Add clearing number of edges per bucket one to the compute pass
				computePassEncoder->setComputePipelineState(clearNumberOfEdgesPerSourceBucketPipeline.get());
				computePassEncoder->dispatchThreads(totalNumberOfWorkItems[18], workItemsPerWorkGroup[18]);
				
				// Set compute pass's part argument
				computePassEncoder->setBytes(&unmove(static_cast<uint8_t>(i)), sizeof(uint8_t), 2);
				
				// Add running step one to the compute pass
				computePassEncoder->setComputePipelineState(stepOnePipeline.get());
				computePassEncoder->dispatchThreads(totalNumberOfWorkItems[0], workItemsPerWorkGroup[0]);
				
				// Check if not the last trimming part
				if(i != SLEAN_TRIMMING_PARTS - 1) {
				
					// Add running step three to the compute pass
					computePassEncoder->setComputePipelineState(stepThreePipeline.get());
					computePassEncoder->dispatchThreads(totalNumberOfWorkItems[2], workItemsPerWorkGroup[2]);
				}
				
				// Otherwise
				else {
				
					// Add clearing number of edges per bucket two to the compute pass
					computePassEncoder->setComputePipelineState(clearNumberOfEdgesPerDestinationBucketPipeline.get());
					computePassEncoder->dispatchThreads(totalNumberOfWorkItems[19], workItemsPerWorkGroup[19]);
					
					// Add running step four to the compute pass
					computePassEncoder->setComputePipelineState(stepFourPipeline.get());
					computePassEncoder->dispatchThreads(totalNumberOfWorkItems[3], workItemsPerWorkGroup[3]);
				}
			}
			
			// Add running step five to the compute pass
			computePassEncoder->setComputePipelineState(stepFivePipeline.get());
			computePassEncoder->dispatchThreads(totalNumberOfWorkItems[4], workItemsPerWorkGroup[4]);
			
			// Go through all remaining trimming parts except the last one
			for(unsigned int i = 0; i < SLEAN_TRIMMING_PARTS - 1; ++i) {
			
				// Add clearing number of edges per bucket one to the compute pass
				computePassEncoder->setComputePipelineState(clearNumberOfEdgesPerSourceBucketPipeline.get());
				computePassEncoder->dispatchThreads(totalNumberOfWorkItems[18], workItemsPerWorkGroup[18]);
				
				// Set compute pass's part argument
				computePassEncoder->setBytes(&unmove(static_cast<uint8_t>(i)), sizeof(uint8_t), 2);
				
				// Add running step one to the compute pass
				computePassEncoder->setComputePipelineState(stepOnePipeline.get());
				computePassEncoder->dispatchThreads(totalNumberOfWorkItems[0], workItemsPerWorkGroup[0]);
				
				// Add clearing number of edges per bucket two to the compute pass
				computePassEncoder->setComputePipelineState(clearNumberOfEdgesPerDestinationBucketPipeline.get());
				computePassEncoder->dispatchThreads(totalNumberOfWorkItems[19], workItemsPerWorkGroup[19]);
				
				// Add running step six to the compute pass
				computePassEncoder->setComputePipelineState(stepSixPipeline.get());
				computePassEncoder->dispatchThreads(totalNumberOfWorkItems[5], workItemsPerWorkGroup[5]);
				
				// Add running step five to the compute pass
				computePassEncoder->setComputePipelineState(stepFivePipeline.get());
				computePassEncoder->dispatchThreads(totalNumberOfWorkItems[4], workItemsPerWorkGroup[4]);
			}
			
			// Check if trimming more than one round
			if(TRIMMING_ROUNDS > 1) {
			
				// Add clearing number of edges per bucket one to the compute pass
				computePassEncoder->setComputePipelineState(clearNumberOfEdgesPerSourceBucketPipeline.get());
				computePassEncoder->dispatchThreads(totalNumberOfWorkItems[18], workItemsPerWorkGroup[18]);
				
				// Set compute pass's part argument
				computePassEncoder->setBytes(&unmove(static_cast<uint8_t>(0)), sizeof(uint8_t), 2);
				
				// Add running step seven to the compute pass
				computePassEncoder->setComputePipelineState(stepSevenPipeline.get());
				computePassEncoder->dispatchThreads(totalNumberOfWorkItems[6], workItemsPerWorkGroup[6]);
				
				// Add running step eight to the compute pass
				computePassEncoder->setComputePipelineState(stepEightPipeline.get());
				computePassEncoder->dispatchThreads(totalNumberOfWorkItems[7], workItemsPerWorkGroup[7]);
				
				// Go through all remaining trimming parts
				for(unsigned int i = 1; i < SLEAN_TRIMMING_PARTS; ++i) {
				
					// Add clearing number of edges per bucket one to the compute pass
					computePassEncoder->setComputePipelineState(clearNumberOfEdgesPerSourceBucketPipeline.get());
					computePassEncoder->dispatchThreads(totalNumberOfWorkItems[18], workItemsPerWorkGroup[18]);
					
					// Set compute pass's part argument
					computePassEncoder->setBytes(&unmove(static_cast<uint8_t>(i)), sizeof(uint8_t), 2);
					
					// Add running step seven to the compute pass
					computePassEncoder->setComputePipelineState(stepSevenPipeline.get());
					computePassEncoder->dispatchThreads(totalNumberOfWorkItems[6], workItemsPerWorkGroup[6]);
					
					// Check if not the last trimming part
					if(i != SLEAN_TRIMMING_PARTS - 1) {
					
						// Add running step nine to the compute pass
						computePassEncoder->setComputePipelineState(stepNinePipeline.get());
						computePassEncoder->dispatchThreads(totalNumberOfWorkItems[8], workItemsPerWorkGroup[8]);
					}
					
					// Otherwise
					else {
					
						// Add clearing number of edges per bucket two to the compute pass
						computePassEncoder->setComputePipelineState(clearNumberOfEdgesPerDestinationBucketPipeline.get());
						computePassEncoder->dispatchThreads(totalNumberOfWorkItems[19], workItemsPerWorkGroup[19]);
						
						// Add running step ten to the compute pass
						computePassEncoder->setComputePipelineState(stepTenPipeline.get());
						computePassEncoder->dispatchThreads(totalNumberOfWorkItems[9], workItemsPerWorkGroup[9]);
					}
				}
				
				// Add running step eleven to the compute pass
				computePassEncoder->setComputePipelineState(stepElevenPipeline.get());
				computePassEncoder->dispatchThreads(totalNumberOfWorkItems[10], workItemsPerWorkGroup[10]);
				
				// Go through all remaining trimming parts except the last one
				for(unsigned int i = 0; i < SLEAN_TRIMMING_PARTS - 1; ++i) {
				
					// Add clearing number of edges per bucket one to the compute pass
					computePassEncoder->setComputePipelineState(clearNumberOfEdgesPerSourceBucketPipeline.get());
					computePassEncoder->dispatchThreads(totalNumberOfWorkItems[18], workItemsPerWorkGroup[18]);
					
					// Set compute pass's part argument
					computePassEncoder->setBytes(&unmove(static_cast<uint8_t>(i)), sizeof(uint8_t), 2);
					
					// Add running step seven to the compute pass
					computePassEncoder->setComputePipelineState(stepSevenPipeline.get());
					computePassEncoder->dispatchThreads(totalNumberOfWorkItems[6], workItemsPerWorkGroup[6]);
					
					// Add clearing number of edges per bucket two to the compute pass
					computePassEncoder->setComputePipelineState(clearNumberOfEdgesPerDestinationBucketPipeline.get());
					computePassEncoder->dispatchThreads(totalNumberOfWorkItems[19], workItemsPerWorkGroup[19]);
					
					// Add running step twelve to the compute pass
					computePassEncoder->setComputePipelineState(stepTwelvePipeline.get());
					computePassEncoder->dispatchThreads(totalNumberOfWorkItems[11], workItemsPerWorkGroup[11]);
					
					// Add running step eleven to the compute pass
					computePassEncoder->setComputePipelineState(stepElevenPipeline.get());
					computePassEncoder->dispatchThreads(totalNumberOfWorkItems[10], workItemsPerWorkGroup[10]);
				}
				
				// Go through all remaining trimming rounds
				for(unsigned int i = 2; i < TRIMMING_ROUNDS; ++i) {
				
					// Add clearing number of edges per bucket one to the compute pass
					computePassEncoder->setComputePipelineState(clearNumberOfEdgesPerSourceBucketPipeline.get());
					computePassEncoder->dispatchThreads(totalNumberOfWorkItems[18], workItemsPerWorkGroup[18]);
					
					// Set compute pass's part and nodes in second partition arguments
					computePassEncoder->setBytes(&unmove(static_cast<uint8_t>(0)), sizeof(uint8_t), 2);
					computePassEncoder->setBytes(&unmove(static_cast<uint8_t>(i % 2)), sizeof(uint8_t), 9);
					
					// Add running step thirteen to the compute pass
					computePassEncoder->setComputePipelineState(stepThirteenPipeline.get());
					computePassEncoder->dispatchThreads(totalNumberOfWorkItems[12], workItemsPerWorkGroup[12]);
					
					// Add running step fourteen to the compute pass
					computePassEncoder->setComputePipelineState(stepFourteenPipeline.get());
					computePassEncoder->dispatchThreads(totalNumberOfWorkItems[13], workItemsPerWorkGroup[13]);
					
					// Go through all remaining trimming parts
					for(unsigned int j = 1; j < SLEAN_TRIMMING_PARTS; ++j) {
					
						// Add clearing number of edges per bucket one to the compute pass
						computePassEncoder->setComputePipelineState(clearNumberOfEdgesPerSourceBucketPipeline.get());
						computePassEncoder->dispatchThreads(totalNumberOfWorkItems[18], workItemsPerWorkGroup[18]);
						
						// Set compute pass's part argument
						computePassEncoder->setBytes(&unmove(static_cast<uint8_t>(j)), sizeof(uint8_t), 2);
						
						// Add running step thirteen to the compute pass
						computePassEncoder->setComputePipelineState(stepThirteenPipeline.get());
						computePassEncoder->dispatchThreads(totalNumberOfWorkItems[12], workItemsPerWorkGroup[12]);
						
						// Check if not the last trimming part
						if(j != SLEAN_TRIMMING_PARTS - 1) {
						
							// Add running step fifteen to the compute pass
							computePassEncoder->setComputePipelineState(stepFifteenPipeline.get());
							computePassEncoder->dispatchThreads(totalNumberOfWorkItems[14], workItemsPerWorkGroup[14]);
						}
						
						// Otherwise
						else {
						
							// Add clearing number of edges per bucket two to the compute pass
							computePassEncoder->setComputePipelineState(clearNumberOfEdgesPerDestinationBucketPipeline.get());
							computePassEncoder->dispatchThreads(totalNumberOfWorkItems[19], workItemsPerWorkGroup[19]);
							
							// Add running step sixteen to the compute pass
							computePassEncoder->setComputePipelineState(stepSixteenPipeline.get());
							computePassEncoder->dispatchThreads(totalNumberOfWorkItems[15], workItemsPerWorkGroup[15]);
						}
					}
					
					// Add running step seventeen to the compute pass
					computePassEncoder->setComputePipelineState(stepSeventeenPipeline.get());
					computePassEncoder->dispatchThreads(totalNumberOfWorkItems[16], workItemsPerWorkGroup[16]);
					
					// Go through all remaining trimming parts except the last one
					for(unsigned int j = 0; j < SLEAN_TRIMMING_PARTS - 1; ++j) {
					
						// Add clearing number of edges per bucket one to the compute pass
						computePassEncoder->setComputePipelineState(clearNumberOfEdgesPerSourceBucketPipeline.get());
						computePassEncoder->dispatchThreads(totalNumberOfWorkItems[18], workItemsPerWorkGroup[18]);
						
						// Set compute pass's part argument
						computePassEncoder->setBytes(&unmove(static_cast<uint8_t>(j)), sizeof(uint8_t), 2);
						
						// Add running step thirteen to the compute pass
						computePassEncoder->setComputePipelineState(stepThirteenPipeline.get());
						computePassEncoder->dispatchThreads(totalNumberOfWorkItems[12], workItemsPerWorkGroup[12]);
						
						// Add clearing number of edges per bucket two to the compute pass
						computePassEncoder->setComputePipelineState(clearNumberOfEdgesPerDestinationBucketPipeline.get());
						computePassEncoder->dispatchThreads(totalNumberOfWorkItems[19], workItemsPerWorkGroup[19]);
						
						// Add running step eighteen to the compute pass
						computePassEncoder->setComputePipelineState(stepEighteenPipeline.get());
						computePassEncoder->dispatchThreads(totalNumberOfWorkItems[17], workItemsPerWorkGroup[17]);
						
						// Add running step seventeen to the compute pass
						computePassEncoder->setComputePipelineState(stepSeventeenPipeline.get());
						computePassEncoder->dispatchThreads(totalNumberOfWorkItems[16], workItemsPerWorkGroup[16]);
					}
				}
			}
			
			// Finish adding commands to the compute pass
			computePassEncoder->endEncoding();
			
			// Set end time to now
			endTime = chrono::high_resolution_clock::now();
			
			// Run the compute pass
			commandBuffer->commit();
			
			// Trimming finished
			trimmingFinished(edgesBitmapTwo->contents(), sipHashKeysTwo, heightTwo, idTwo, nonceTwo);
			
			// Check if closing
			if(closing) {
			
				// Return true
				return true;
			}
			
			// Wait until the compute pass has finished
			commandBuffer->waitUntilCompleted();
			
			// Check if running compute pass failed
			if(commandBuffer->error()) {
			
				// Display message
				cout << "Running compute pass failed" << endl;
				
				// Return false
				return false;
			}
			
			// Display message
			cout << "\tTrimming time:\t " << (commandBuffer->GPUEndTime() - commandBuffer->kernelStartTime() + static_cast<chrono::duration<double>>(endTime - startTime).count()) << " second(s)" << endl;
			
			// Set start time to now
			startTime = chrono::high_resolution_clock::now();
			
			// Free objects with autorelease memory
			autoreleasePool.reset();
			
			// Check if recreating autorelease pool failed
			autoreleasePool = unique_ptr<NS::AutoreleasePool, void(*)(NS::AutoreleasePool *)>(NS::AutoreleasePool::alloc()->init(), [](NS::AutoreleasePool *autoreleasePool) noexcept {
			
				// Free autorelease pool
				autoreleasePool->release();
			});
			if(!autoreleasePool) {
			
				// Display message
				cout << "Creating autorelease pool failed" << endl;
				
				// Return false
				return false;
			}
			
			// Check if creating command queue's command buffer failed
			commandBuffer = commandQueue->commandBuffer();
			if(!commandBuffer) {
			
				// Display message
				cout << "Creating command queue's command buffer failed" << endl;
				
				// Return false
				return false;
			}
			
			// Check if creating command buffer's compute pass encoder failed
			computePassEncoder = commandBuffer->computeCommandEncoder();
			if(!computePassEncoder) {
			
				// Display message
				cout << "Creating command buffer's compute pass encoder failed" << endl;
				
				// Return false
				return false;
			}
			
			// Get SipHash keys from job's header and nonce
			heightTwo = jobHeight;
			idTwo = jobId;
			nonceTwo = jobNonce++;
			blake2b(sipHashKeysTwo, jobHeader, nonceTwo);
			
			// Set compute pass's buckets, number of edges per bucket, SipHash keys, nodes bitmap, destination buckets, number of edges per destination bucket, and edges bitmap arguments
			computePassEncoder->setBuffer(bucketsOne.get(), 0, 0);
			computePassEncoder->setBuffer(numberOfEdgesPerBucketOne.get(), 0, 1);
			computePassEncoder->setBytes(&sipHashKeysTwo, sizeof(sipHashKeysTwo), 3);
			computePassEncoder->setBuffer(nodesBitmap.get(), 0, 5);
			computePassEncoder->setBuffer(bucketsTwo.get(), 0, 6);
			computePassEncoder->setBuffer(numberOfEdgesPerBucketTwo.get(), 0, 7);
			computePassEncoder->setBuffer(edgesBitmapTwo.get(), 0, 8);
			
			// Check if using buckets one second part
			if(bucketsOneNumberOfBuckets != SLEAN_TRIMMING_NUMBER_OF_BUCKETS) {
			
				// Set compute pass's buckets second part argument
				computePassEncoder->setBuffer(bucketsOneSecondPart.get(), 0, 4);
			}
			
			// Add clearing number of edges per bucket one to the compute pass
			computePassEncoder->setComputePipelineState(clearNumberOfEdgesPerSourceBucketPipeline.get());
			computePassEncoder->dispatchThreads(totalNumberOfWorkItems[18], workItemsPerWorkGroup[18]);
			
			// Set compute pass's part argument
			computePassEncoder->setBytes(&unmove(static_cast<uint8_t>(0)), sizeof(uint8_t), 2);
			
			// Add running step one to the compute pass
			computePassEncoder->setComputePipelineState(stepOnePipeline.get());
			computePassEncoder->dispatchThreads(totalNumberOfWorkItems[0], workItemsPerWorkGroup[0]);
			
			// Add running step two to the compute pass
			computePassEncoder->setComputePipelineState(stepTwoPipeline.get());
			computePassEncoder->dispatchThreads(totalNumberOfWorkItems[1], workItemsPerWorkGroup[1]);
			
			// Go through all remaining trimming parts
			for(unsigned int i = 1; i < SLEAN_TRIMMING_PARTS; ++i) {
			
				// Add clearing number of edges per bucket one to the compute pass
				computePassEncoder->setComputePipelineState(clearNumberOfEdgesPerSourceBucketPipeline.get());
				computePassEncoder->dispatchThreads(totalNumberOfWorkItems[18], workItemsPerWorkGroup[18]);
				
				// Set compute pass's part argument
				computePassEncoder->setBytes(&unmove(static_cast<uint8_t>(i)), sizeof(uint8_t), 2);
				
				// Add running step one to the compute pass
				computePassEncoder->setComputePipelineState(stepOnePipeline.get());
				computePassEncoder->dispatchThreads(totalNumberOfWorkItems[0], workItemsPerWorkGroup[0]);
				
				// Check if not the last trimming part
				if(i != SLEAN_TRIMMING_PARTS - 1) {
				
					// Add running step three to the compute pass
					computePassEncoder->setComputePipelineState(stepThreePipeline.get());
					computePassEncoder->dispatchThreads(totalNumberOfWorkItems[2], workItemsPerWorkGroup[2]);
				}
				
				// Otherwise
				else {
				
					// Add clearing number of edges per bucket two to the compute pass
					computePassEncoder->setComputePipelineState(clearNumberOfEdgesPerDestinationBucketPipeline.get());
					computePassEncoder->dispatchThreads(totalNumberOfWorkItems[19], workItemsPerWorkGroup[19]);
					
					// Add running step four to the compute pass
					computePassEncoder->setComputePipelineState(stepFourPipeline.get());
					computePassEncoder->dispatchThreads(totalNumberOfWorkItems[3], workItemsPerWorkGroup[3]);
				}
			}
			
			// Add running step five to the compute pass
			computePassEncoder->setComputePipelineState(stepFivePipeline.get());
			computePassEncoder->dispatchThreads(totalNumberOfWorkItems[4], workItemsPerWorkGroup[4]);
			
			// Go through all remaining trimming parts except the last one
			for(unsigned int i = 0; i < SLEAN_TRIMMING_PARTS - 1; ++i) {
			
				// Add clearing number of edges per bucket one to the compute pass
				computePassEncoder->setComputePipelineState(clearNumberOfEdgesPerSourceBucketPipeline.get());
				computePassEncoder->dispatchThreads(totalNumberOfWorkItems[18], workItemsPerWorkGroup[18]);
				
				// Set compute pass's part argument
				computePassEncoder->setBytes(&unmove(static_cast<uint8_t>(i)), sizeof(uint8_t), 2);
				
				// Add running step one to the compute pass
				computePassEncoder->setComputePipelineState(stepOnePipeline.get());
				computePassEncoder->dispatchThreads(totalNumberOfWorkItems[0], workItemsPerWorkGroup[0]);
				
				// Add clearing number of edges per bucket two to the compute pass
				computePassEncoder->setComputePipelineState(clearNumberOfEdgesPerDestinationBucketPipeline.get());
				computePassEncoder->dispatchThreads(totalNumberOfWorkItems[19], workItemsPerWorkGroup[19]);
				
				// Add running step six to the compute pass
				computePassEncoder->setComputePipelineState(stepSixPipeline.get());
				computePassEncoder->dispatchThreads(totalNumberOfWorkItems[5], workItemsPerWorkGroup[5]);
				
				// Add running step five to the compute pass
				computePassEncoder->setComputePipelineState(stepFivePipeline.get());
				computePassEncoder->dispatchThreads(totalNumberOfWorkItems[4], workItemsPerWorkGroup[4]);
			}
			
			// Check if trimming more than one round
			if(TRIMMING_ROUNDS > 1) {
			
				// Add clearing number of edges per bucket one to the compute pass
				computePassEncoder->setComputePipelineState(clearNumberOfEdgesPerSourceBucketPipeline.get());
				computePassEncoder->dispatchThreads(totalNumberOfWorkItems[18], workItemsPerWorkGroup[18]);
				
				// Set compute pass's part argument
				computePassEncoder->setBytes(&unmove(static_cast<uint8_t>(0)), sizeof(uint8_t), 2);
				
				// Add running step seven to the compute pass
				computePassEncoder->setComputePipelineState(stepSevenPipeline.get());
				computePassEncoder->dispatchThreads(totalNumberOfWorkItems[6], workItemsPerWorkGroup[6]);
				
				// Add running step eight to the compute pass
				computePassEncoder->setComputePipelineState(stepEightPipeline.get());
				computePassEncoder->dispatchThreads(totalNumberOfWorkItems[7], workItemsPerWorkGroup[7]);
				
				// Go through all remaining trimming parts
				for(unsigned int i = 1; i < SLEAN_TRIMMING_PARTS; ++i) {
				
					// Add clearing number of edges per bucket one to the compute pass
					computePassEncoder->setComputePipelineState(clearNumberOfEdgesPerSourceBucketPipeline.get());
					computePassEncoder->dispatchThreads(totalNumberOfWorkItems[18], workItemsPerWorkGroup[18]);
					
					// Set compute pass's part argument
					computePassEncoder->setBytes(&unmove(static_cast<uint8_t>(i)), sizeof(uint8_t), 2);
					
					// Add running step seven to the compute pass
					computePassEncoder->setComputePipelineState(stepSevenPipeline.get());
					computePassEncoder->dispatchThreads(totalNumberOfWorkItems[6], workItemsPerWorkGroup[6]);
					
					// Check if not the last trimming part
					if(i != SLEAN_TRIMMING_PARTS - 1) {
					
						// Add running step nine to the compute pass
						computePassEncoder->setComputePipelineState(stepNinePipeline.get());
						computePassEncoder->dispatchThreads(totalNumberOfWorkItems[8], workItemsPerWorkGroup[8]);
					}
					
					// Otherwise
					else {
					
						// Add clearing number of edges per bucket two to the compute pass
						computePassEncoder->setComputePipelineState(clearNumberOfEdgesPerDestinationBucketPipeline.get());
						computePassEncoder->dispatchThreads(totalNumberOfWorkItems[19], workItemsPerWorkGroup[19]);
						
						// Add running step ten to the compute pass
						computePassEncoder->setComputePipelineState(stepTenPipeline.get());
						computePassEncoder->dispatchThreads(totalNumberOfWorkItems[9], workItemsPerWorkGroup[9]);
					}
				}
				
				// Add running step eleven to the compute pass
				computePassEncoder->setComputePipelineState(stepElevenPipeline.get());
				computePassEncoder->dispatchThreads(totalNumberOfWorkItems[10], workItemsPerWorkGroup[10]);
				
				// Go through all remaining trimming parts except the last one
				for(unsigned int i = 0; i < SLEAN_TRIMMING_PARTS - 1; ++i) {
				
					// Add clearing number of edges per bucket one to the compute pass
					computePassEncoder->setComputePipelineState(clearNumberOfEdgesPerSourceBucketPipeline.get());
					computePassEncoder->dispatchThreads(totalNumberOfWorkItems[18], workItemsPerWorkGroup[18]);
					
					// Set compute pass's part argument
					computePassEncoder->setBytes(&unmove(static_cast<uint8_t>(i)), sizeof(uint8_t), 2);
					
					// Add running step seven to the compute pass
					computePassEncoder->setComputePipelineState(stepSevenPipeline.get());
					computePassEncoder->dispatchThreads(totalNumberOfWorkItems[6], workItemsPerWorkGroup[6]);
					
					// Add clearing number of edges per bucket two to the compute pass
					computePassEncoder->setComputePipelineState(clearNumberOfEdgesPerDestinationBucketPipeline.get());
					computePassEncoder->dispatchThreads(totalNumberOfWorkItems[19], workItemsPerWorkGroup[19]);
					
					// Add running step twelve to the compute pass
					computePassEncoder->setComputePipelineState(stepTwelvePipeline.get());
					computePassEncoder->dispatchThreads(totalNumberOfWorkItems[11], workItemsPerWorkGroup[11]);
					
					// Add running step eleven to the compute pass
					computePassEncoder->setComputePipelineState(stepElevenPipeline.get());
					computePassEncoder->dispatchThreads(totalNumberOfWorkItems[10], workItemsPerWorkGroup[10]);
				}
				
				// Go through all remaining trimming rounds
				for(unsigned int i = 2; i < TRIMMING_ROUNDS; ++i) {
				
					// Add clearing number of edges per bucket one to the compute pass
					computePassEncoder->setComputePipelineState(clearNumberOfEdgesPerSourceBucketPipeline.get());
					computePassEncoder->dispatchThreads(totalNumberOfWorkItems[18], workItemsPerWorkGroup[18]);
					
					// Set compute pass's part and nodes in second partition arguments
					computePassEncoder->setBytes(&unmove(static_cast<uint8_t>(0)), sizeof(uint8_t), 2);
					computePassEncoder->setBytes(&unmove(static_cast<uint8_t>(i % 2)), sizeof(uint8_t), 9);
					
					// Add running step thirteen to the compute pass
					computePassEncoder->setComputePipelineState(stepThirteenPipeline.get());
					computePassEncoder->dispatchThreads(totalNumberOfWorkItems[12], workItemsPerWorkGroup[12]);
					
					// Add running step fourteen to the compute pass
					computePassEncoder->setComputePipelineState(stepFourteenPipeline.get());
					computePassEncoder->dispatchThreads(totalNumberOfWorkItems[13], workItemsPerWorkGroup[13]);
					
					// Go through all remaining trimming parts
					for(unsigned int j = 1; j < SLEAN_TRIMMING_PARTS; ++j) {
					
						// Add clearing number of edges per bucket one to the compute pass
						computePassEncoder->setComputePipelineState(clearNumberOfEdgesPerSourceBucketPipeline.get());
						computePassEncoder->dispatchThreads(totalNumberOfWorkItems[18], workItemsPerWorkGroup[18]);
						
						// Set compute pass's part argument
						computePassEncoder->setBytes(&unmove(static_cast<uint8_t>(j)), sizeof(uint8_t), 2);
						
						// Add running step thirteen to the compute pass
						computePassEncoder->setComputePipelineState(stepThirteenPipeline.get());
						computePassEncoder->dispatchThreads(totalNumberOfWorkItems[12], workItemsPerWorkGroup[12]);
						
						// Check if not the last trimming part
						if(j != SLEAN_TRIMMING_PARTS - 1) {
						
							// Add running step fifteen to the compute pass
							computePassEncoder->setComputePipelineState(stepFifteenPipeline.get());
							computePassEncoder->dispatchThreads(totalNumberOfWorkItems[14], workItemsPerWorkGroup[14]);
						}
						
						// Otherwise
						else {
						
							// Add clearing number of edges per bucket two to the compute pass
							computePassEncoder->setComputePipelineState(clearNumberOfEdgesPerDestinationBucketPipeline.get());
							computePassEncoder->dispatchThreads(totalNumberOfWorkItems[19], workItemsPerWorkGroup[19]);
							
							// Add running step sixteen to the compute pass
							computePassEncoder->setComputePipelineState(stepSixteenPipeline.get());
							computePassEncoder->dispatchThreads(totalNumberOfWorkItems[15], workItemsPerWorkGroup[15]);
						}
					}
					
					// Add running step seventeen to the compute pass
					computePassEncoder->setComputePipelineState(stepSeventeenPipeline.get());
					computePassEncoder->dispatchThreads(totalNumberOfWorkItems[16], workItemsPerWorkGroup[16]);
					
					// Go through all remaining trimming parts except the last one
					for(unsigned int j = 0; j < SLEAN_TRIMMING_PARTS - 1; ++j) {
					
						// Add clearing number of edges per bucket one to the compute pass
						computePassEncoder->setComputePipelineState(clearNumberOfEdgesPerSourceBucketPipeline.get());
						computePassEncoder->dispatchThreads(totalNumberOfWorkItems[18], workItemsPerWorkGroup[18]);
						
						// Set compute pass's part argument
						computePassEncoder->setBytes(&unmove(static_cast<uint8_t>(j)), sizeof(uint8_t), 2);
						
						// Add running step thirteen to the compute pass
						computePassEncoder->setComputePipelineState(stepThirteenPipeline.get());
						computePassEncoder->dispatchThreads(totalNumberOfWorkItems[12], workItemsPerWorkGroup[12]);
						
						// Add clearing number of edges per bucket two to the compute pass
						computePassEncoder->setComputePipelineState(clearNumberOfEdgesPerDestinationBucketPipeline.get());
						computePassEncoder->dispatchThreads(totalNumberOfWorkItems[19], workItemsPerWorkGroup[19]);
						
						// Add running step eighteen to the compute pass
						computePassEncoder->setComputePipelineState(stepEighteenPipeline.get());
						computePassEncoder->dispatchThreads(totalNumberOfWorkItems[17], workItemsPerWorkGroup[17]);
						
						// Add running step seventeen to the compute pass
						computePassEncoder->setComputePipelineState(stepSeventeenPipeline.get());
						computePassEncoder->dispatchThreads(totalNumberOfWorkItems[16], workItemsPerWorkGroup[16]);
					}
				}
			}
			
			// Finish adding commands to the compute pass
			computePassEncoder->endEncoding();
			
			// Set end time to now
			endTime = chrono::high_resolution_clock::now();
			
			// Run the compute pass
			commandBuffer->commit();
			
			// Trimming finished
			trimmingFinished(edgesBitmapOne->contents(), sipHashKeysOne, heightOne, idOne, nonceOne);
		}
		
		// Return true
		return true;
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
		
		// Check if getting device's max work group size, max memory allocation size, or work group memory size failed, or its max work group size don't exists
		size_t maxWorkGroupSize;
		cl_ulong maxMemoryAllocationSize;
		cl_ulong workGroupMemorySize;
		if(clGetDeviceInfo(device, CL_DEVICE_MAX_WORK_GROUP_SIZE, sizeof(maxWorkGroupSize), &maxWorkGroupSize, nullptr) != CL_SUCCESS || clGetDeviceInfo(device, CL_DEVICE_MAX_MEM_ALLOC_SIZE, sizeof(maxMemoryAllocationSize), &maxMemoryAllocationSize, nullptr) != CL_SUCCESS || clGetDeviceInfo(device, CL_DEVICE_LOCAL_MEM_SIZE, sizeof(workGroupMemorySize), &workGroupMemorySize, nullptr) != CL_SUCCESS || !maxWorkGroupSize) {
		
			// Display message
			cout << "Getting GPU's info failed" << endl;
			
			// Return false
			return false;
		}
		
		// Check if device index is specified
		if(deviceIndex != ALL_DEVICES) {
		
			// Display message
			cout << "Using the GPU for slean trimming" << endl;
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
		unique_ptr<remove_pointer<cl_mem>::type, decltype(&clReleaseMemObject)> moreThanMaxMemoryAllocation(clCreateBuffer(context, CL_MEM_READ_WRITE | CL_MEM_HOST_NO_ACCESS, maxMemoryAllocationSize + 1, nullptr, &errorCode), clReleaseMemObject);
		
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
		if(clBuildProgram(program.get(), 1, &device, ("-cl-std=CL1.2 -Werror -DEDGE_BITS=" TO_STRING(EDGE_BITS) " -DSLEAN_TRIMMING_PARTS=" TO_STRING(SLEAN_TRIMMING_PARTS) " -DNUMBER_OF_EDGES_PER_STEP_ONE_WORK_ITEM=" + to_string(SLEAN_TRIMMING_NUMBER_OF_EDGES_PER_STEP_ONE_WORK_ITEM) + " -DNUMBER_OF_BITMAP_BYTES=" + to_string(SLEAN_TRIMMING_NUMBER_OF_BITMAP_BYTES) + " -DNUMBER_OF_BUCKETS=" + to_string(SLEAN_TRIMMING_NUMBER_OF_BUCKETS) + " -DNUMBER_OF_LEAST_SIGNIFICANT_BITS_IGNORED_DURING_BUCKET_SORTING=" + to_string(SLEAN_TRIMMING_NUMBER_OF_LEAST_SIGNIFICANT_BITS_IGNORED_DURING_BUCKET_SORTING) + " -DINITIAL_MAX_NUMBER_OF_EDGES_PER_BUCKET=" + to_string(SLEAN_TRIMMING_INITIAL_MAX_NUMBER_OF_EDGES_PER_BUCKET) + " -DAFTER_TRIMMING_ROUND_MAX_NUMBER_OF_EDGES_PER_BUCKET=" + to_string(SLEAN_TRIMMING_AFTER_TRIMMING_ROUND_MAX_NUMBER_OF_EDGES_PER_BUCKET) + " -DAFTER_TRIMMING_ROUND_MAX_NUMBER_OF_EDGES_PER_REMAINING_EDGES_BUCKET=" + to_string(SLEAN_TRIMMING_AFTER_TRIMMING_ROUND_MAX_NUMBER_OF_EDGES_PER_REMAINING_EDGES_BUCKET) + " -DNUMBER_OF_LEAST_SIGNIFICANT_BITS_IGNORED_DURING_REMAINING_EDGES_BUCKET_SORTING=" + to_string(SLEAN_TRIMMING_NUMBER_OF_LEAST_SIGNIFICANT_BITS_IGNORED_DURING_REMAINING_EDGES_BUCKET_SORTING) + " -DNUMBER_OF_REMAINING_EDGES_BITMAP_BYTES=" + to_string(SLEAN_TRIMMING_NUMBER_OF_REMAINING_EDGES_BITMAP_BYTES) + " -DINITIAL_BUCKETS_NUMBER_OF_BUCKETS=" + to_string(bucketsOneNumberOfBuckets) + " -DLOCAL_BUCKETS_SIZE=" + to_string(localBucketsSize)).c_str(), nullptr, nullptr) != CL_SUCCESS) {
		
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
		static unique_ptr<remove_pointer<cl_kernel>::type, decltype(&clReleaseKernel)> stepSevenKernel(clCreateKernel(program.get(), "trimEdgesStepSeven", nullptr), clReleaseKernel);
		static unique_ptr<remove_pointer<cl_kernel>::type, decltype(&clReleaseKernel)> stepEightKernel(clCreateKernel(program.get(), "trimEdgesStepEight", nullptr), clReleaseKernel);
		static unique_ptr<remove_pointer<cl_kernel>::type, decltype(&clReleaseKernel)> stepNineKernel(clCreateKernel(program.get(), "trimEdgesStepNine", nullptr), clReleaseKernel);
		static unique_ptr<remove_pointer<cl_kernel>::type, decltype(&clReleaseKernel)> stepTenKernel(clCreateKernel(program.get(), "trimEdgesStepTen", nullptr), clReleaseKernel);
		static unique_ptr<remove_pointer<cl_kernel>::type, decltype(&clReleaseKernel)> stepElevenKernel(clCreateKernel(program.get(), "trimEdgesStepEleven", nullptr), clReleaseKernel);
		static unique_ptr<remove_pointer<cl_kernel>::type, decltype(&clReleaseKernel)> stepTwelveKernel(clCreateKernel(program.get(), "trimEdgesStepTwelve", nullptr), clReleaseKernel);
		static unique_ptr<remove_pointer<cl_kernel>::type, decltype(&clReleaseKernel)> stepThirteenKernel(clCreateKernel(program.get(), "trimEdgesStepThirteen", nullptr), clReleaseKernel);
		static unique_ptr<remove_pointer<cl_kernel>::type, decltype(&clReleaseKernel)> stepFourteenKernel(clCreateKernel(program.get(), "trimEdgesStepFourteen", nullptr), clReleaseKernel);
		static unique_ptr<remove_pointer<cl_kernel>::type, decltype(&clReleaseKernel)> stepFifteenKernel(clCreateKernel(program.get(), "trimEdgesStepFifteen", nullptr), clReleaseKernel);
		static unique_ptr<remove_pointer<cl_kernel>::type, decltype(&clReleaseKernel)> stepSixteenKernel(clCreateKernel(program.get(), "trimEdgesStepSixteen", nullptr), clReleaseKernel);
		static unique_ptr<remove_pointer<cl_kernel>::type, decltype(&clReleaseKernel)> stepSeventeenKernel(clCreateKernel(program.get(), "trimEdgesStepSeventeen", nullptr), clReleaseKernel);
		static unique_ptr<remove_pointer<cl_kernel>::type, decltype(&clReleaseKernel)> stepEighteenKernel(clCreateKernel(program.get(), "trimEdgesStepEighteen", nullptr), clReleaseKernel);
		if(!stepOneKernel || !stepTwoKernel || !stepThreeKernel || !stepFourKernel || !stepFiveKernel || !stepSixKernel || !stepSevenKernel || !stepEightKernel || !stepNineKernel || !stepTenKernel || !stepElevenKernel || !stepTwelveKernel || !stepThirteenKernel || !stepFourteenKernel || !stepFifteenKernel || !stepSixteenKernel || !stepSeventeenKernel || !stepEighteenKernel) {
		
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
			SLEAN_TRIMMING_NUMBER_OF_BUCKETS * min(bit_floor(maxWorkGroupSize), bit_floor(static_cast<size_t>(INT16_MAX / 2))),
			
			// Trim edges step seven kernel
			(NUMBER_OF_EDGES / SLEAN_TRIMMING_PARTS) / (sizeof(cl_ulong) * BITS_IN_A_BYTE),
			
			// Trim edges step eight kernel
			SLEAN_TRIMMING_NUMBER_OF_BUCKETS * min(bit_floor(maxWorkGroupSize), bit_floor(static_cast<size_t>(INT16_MAX / 2))),
			
			// Trim edges step nine kernel
			SLEAN_TRIMMING_NUMBER_OF_BUCKETS * min(bit_floor(maxWorkGroupSize), bit_floor(static_cast<size_t>(INT16_MAX / 2))),
			
			// Trim edges step ten kernel
			SLEAN_TRIMMING_NUMBER_OF_BUCKETS * min(bit_floor(maxWorkGroupSize), bit_floor(static_cast<size_t>(INT16_MAX / 2))),
			
			// Trim edges step eleven kernel
			SLEAN_TRIMMING_NUMBER_OF_REMAINING_EDGES_BUCKETS * min(bit_floor(maxWorkGroupSize), bit_floor(static_cast<size_t>(INT16_MAX / 2))),
			
			// Trim edges step twelve kernel
			SLEAN_TRIMMING_NUMBER_OF_BUCKETS * min(bit_floor(maxWorkGroupSize), bit_floor(static_cast<size_t>(INT16_MAX / 2))),
			
			// Trim edges step thirteen kernel
			(NUMBER_OF_EDGES / SLEAN_TRIMMING_PARTS) / (sizeof(cl_ulong) * BITS_IN_A_BYTE),
			
			// Trim edges step fourteen kernel
			SLEAN_TRIMMING_NUMBER_OF_BUCKETS * min(bit_floor(maxWorkGroupSize), bit_floor(static_cast<size_t>(INT16_MAX / 2))),
			
			// Trim edges step fifteen kernel
			SLEAN_TRIMMING_NUMBER_OF_BUCKETS * min(bit_floor(maxWorkGroupSize), bit_floor(static_cast<size_t>(INT16_MAX / 2))),
			
			// Trim edges step sixteen kernel
			SLEAN_TRIMMING_NUMBER_OF_BUCKETS * min(bit_floor(maxWorkGroupSize), bit_floor(static_cast<size_t>(INT16_MAX / 2))),
			
			// Trim edges step seventeen kernel
			SLEAN_TRIMMING_NUMBER_OF_REMAINING_EDGES_BUCKETS * min(bit_floor(maxWorkGroupSize), bit_floor(static_cast<size_t>(INT16_MAX / 2))),
			
			// Trim edges step eighteen kernel
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
			min(bit_floor(maxWorkGroupSize), bit_floor(static_cast<size_t>(INT16_MAX / 2))),
			
			// Trim edges step seven kernel
			min(min(bit_floor(maxWorkGroupSize), bit_floor(static_cast<size_t>(INT16_MAX / 2))), totalNumberOfWorkItems[6]),
			
			// Trim edges step eight kernel
			min(bit_floor(maxWorkGroupSize), bit_floor(static_cast<size_t>(INT16_MAX / 2))),
			
			// Trim edges step nine kernel
			min(bit_floor(maxWorkGroupSize), bit_floor(static_cast<size_t>(INT16_MAX / 2))),
			
			// Trim edges step ten kernel
			min(bit_floor(maxWorkGroupSize), bit_floor(static_cast<size_t>(INT16_MAX / 2))),
			
			// Trim edges step eleven kernel
			min(bit_floor(maxWorkGroupSize), bit_floor(static_cast<size_t>(INT16_MAX / 2))),
			
			// Trim edges step twelve kernel
			min(bit_floor(maxWorkGroupSize), bit_floor(static_cast<size_t>(INT16_MAX / 2))),
			
			// Trim edges step thirteen kernel
			min(min(bit_floor(maxWorkGroupSize), bit_floor(static_cast<size_t>(INT16_MAX / 2))), totalNumberOfWorkItems[12]),
			
			// Trim edges step fourteen kernel
			min(bit_floor(maxWorkGroupSize), bit_floor(static_cast<size_t>(INT16_MAX / 2))),
			
			// Trim edges step fifteen kernel
			min(bit_floor(maxWorkGroupSize), bit_floor(static_cast<size_t>(INT16_MAX / 2))),
			
			// Trim edges step sixteen kernel
			min(bit_floor(maxWorkGroupSize), bit_floor(static_cast<size_t>(INT16_MAX / 2))),
			
			// Trim edges step seventeen kernel
			min(bit_floor(maxWorkGroupSize), bit_floor(static_cast<size_t>(INT16_MAX / 2))),
			
			// Trim edges step eighteen kernel
			min(bit_floor(maxWorkGroupSize), bit_floor(static_cast<size_t>(INT16_MAX / 2)))
		};
		
		// Free kernels
		stepOneKernel.reset();
		stepTwoKernel.reset();
		stepThreeKernel.reset();
		stepFourKernel.reset();
		stepFiveKernel.reset();
		stepSixKernel.reset();
		stepSevenKernel.reset();
		stepEightKernel.reset();
		stepNineKernel.reset();
		stepTenKernel.reset();
		stepElevenKernel.reset();
		stepTwelveKernel.reset();
		stepThirteenKernel.reset();
		stepFourteenKernel.reset();
		stepFifteenKernel.reset();
		stepSixteenKernel.reset();
		stepSeventeenKernel.reset();
		stepEighteenKernel.reset();
		
		// Check if recreating program for the device failed
		program = unique_ptr<remove_pointer<cl_program>::type, decltype(&clReleaseProgram)>(clCreateProgramWithSource(context, 1, &source, &sourceSize, nullptr), clReleaseProgram);
		if(!program) {
		
			// Display message
			cout << "Creating program for the GPU failed" << endl;
			
			// Return false
			return false;
		}
		
		// Check if rebuilding program for the device with hardcoded work items per work groups failed
		if(clBuildProgram(program.get(), 1, &device, ("-cl-std=CL1.2 -Werror -DEDGE_BITS=" TO_STRING(EDGE_BITS) " -DSLEAN_TRIMMING_PARTS=" TO_STRING(SLEAN_TRIMMING_PARTS) " -DNUMBER_OF_EDGES_PER_STEP_ONE_WORK_ITEM=" + to_string(SLEAN_TRIMMING_NUMBER_OF_EDGES_PER_STEP_ONE_WORK_ITEM) + " -DNUMBER_OF_BITMAP_BYTES=" + to_string(SLEAN_TRIMMING_NUMBER_OF_BITMAP_BYTES) + " -DNUMBER_OF_BUCKETS=" + to_string(SLEAN_TRIMMING_NUMBER_OF_BUCKETS) + " -DNUMBER_OF_LEAST_SIGNIFICANT_BITS_IGNORED_DURING_BUCKET_SORTING=" + to_string(SLEAN_TRIMMING_NUMBER_OF_LEAST_SIGNIFICANT_BITS_IGNORED_DURING_BUCKET_SORTING) + " -DINITIAL_MAX_NUMBER_OF_EDGES_PER_BUCKET=" + to_string(SLEAN_TRIMMING_INITIAL_MAX_NUMBER_OF_EDGES_PER_BUCKET) + " -DAFTER_TRIMMING_ROUND_MAX_NUMBER_OF_EDGES_PER_BUCKET=" + to_string(SLEAN_TRIMMING_AFTER_TRIMMING_ROUND_MAX_NUMBER_OF_EDGES_PER_BUCKET) + " -DAFTER_TRIMMING_ROUND_MAX_NUMBER_OF_EDGES_PER_REMAINING_EDGES_BUCKET=" + to_string(SLEAN_TRIMMING_AFTER_TRIMMING_ROUND_MAX_NUMBER_OF_EDGES_PER_REMAINING_EDGES_BUCKET) + " -DNUMBER_OF_LEAST_SIGNIFICANT_BITS_IGNORED_DURING_REMAINING_EDGES_BUCKET_SORTING=" + to_string(SLEAN_TRIMMING_NUMBER_OF_LEAST_SIGNIFICANT_BITS_IGNORED_DURING_REMAINING_EDGES_BUCKET_SORTING) + " -DNUMBER_OF_REMAINING_EDGES_BITMAP_BYTES=" + to_string(SLEAN_TRIMMING_NUMBER_OF_REMAINING_EDGES_BITMAP_BYTES) + " -DINITIAL_BUCKETS_NUMBER_OF_BUCKETS=" + to_string(bucketsOneNumberOfBuckets) + " -DLOCAL_BUCKETS_SIZE=" + to_string(localBucketsSize) + " -DTRIM_EDGES_STEP_ONE_WORK_ITEMS_PER_WORK_GROUP=" + to_string(workItemsPerWorkGroup[0]) + " -DTRIM_EDGES_STEP_TWO_WORK_ITEMS_PER_WORK_GROUP=" + to_string(workItemsPerWorkGroup[1]) + " -DTRIM_EDGES_STEP_THREE_WORK_ITEMS_PER_WORK_GROUP=" + to_string(workItemsPerWorkGroup[2]) + " -DTRIM_EDGES_STEP_FOUR_WORK_ITEMS_PER_WORK_GROUP=" + to_string(workItemsPerWorkGroup[3]) + " -DTRIM_EDGES_STEP_FIVE_WORK_ITEMS_PER_WORK_GROUP=" + to_string(workItemsPerWorkGroup[4]) + " -DTRIM_EDGES_STEP_SIX_WORK_ITEMS_PER_WORK_GROUP=" + to_string(workItemsPerWorkGroup[5]) + " -DTRIM_EDGES_STEP_SEVEN_WORK_ITEMS_PER_WORK_GROUP=" + to_string(workItemsPerWorkGroup[6]) + " -DTRIM_EDGES_STEP_EIGHT_WORK_ITEMS_PER_WORK_GROUP=" + to_string(workItemsPerWorkGroup[7]) + " -DTRIM_EDGES_STEP_NINE_WORK_ITEMS_PER_WORK_GROUP=" + to_string(workItemsPerWorkGroup[8]) + " -DTRIM_EDGES_STEP_TEN_WORK_ITEMS_PER_WORK_GROUP=" + to_string(workItemsPerWorkGroup[9]) + " -DTRIM_EDGES_STEP_ELEVEN_WORK_ITEMS_PER_WORK_GROUP=" + to_string(workItemsPerWorkGroup[10]) + " -DTRIM_EDGES_STEP_TWELVE_WORK_ITEMS_PER_WORK_GROUP=" + to_string(workItemsPerWorkGroup[11]) + " -DTRIM_EDGES_STEP_THIRTEEN_WORK_ITEMS_PER_WORK_GROUP=" + to_string(workItemsPerWorkGroup[12]) + " -DTRIM_EDGES_STEP_FOURTEEN_WORK_ITEMS_PER_WORK_GROUP=" + to_string(workItemsPerWorkGroup[13]) + " -DTRIM_EDGES_STEP_FIFTEEN_WORK_ITEMS_PER_WORK_GROUP=" + to_string(workItemsPerWorkGroup[14]) + " -DTRIM_EDGES_STEP_SIXTEEN_WORK_ITEMS_PER_WORK_GROUP=" + to_string(workItemsPerWorkGroup[15]) + " -DTRIM_EDGES_STEP_SEVENTEEN_WORK_ITEMS_PER_WORK_GROUP=" + to_string(workItemsPerWorkGroup[16]) + " -DTRIM_EDGES_STEP_EIGHTEEN_WORK_ITEMS_PER_WORK_GROUP=" + to_string(workItemsPerWorkGroup[17])).c_str(), nullptr, nullptr) != CL_SUCCESS) {
		
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
		stepSevenKernel = unique_ptr<remove_pointer<cl_kernel>::type, decltype(&clReleaseKernel)>(clCreateKernel(program.get(), "trimEdgesStepSeven", nullptr), clReleaseKernel);
		stepEightKernel = unique_ptr<remove_pointer<cl_kernel>::type, decltype(&clReleaseKernel)>(clCreateKernel(program.get(), "trimEdgesStepEight", nullptr), clReleaseKernel);
		stepNineKernel = unique_ptr<remove_pointer<cl_kernel>::type, decltype(&clReleaseKernel)>(clCreateKernel(program.get(), "trimEdgesStepNine", nullptr), clReleaseKernel);
		stepTenKernel = unique_ptr<remove_pointer<cl_kernel>::type, decltype(&clReleaseKernel)>(clCreateKernel(program.get(), "trimEdgesStepTen", nullptr), clReleaseKernel);
		stepElevenKernel = unique_ptr<remove_pointer<cl_kernel>::type, decltype(&clReleaseKernel)>(clCreateKernel(program.get(), "trimEdgesStepEleven", nullptr), clReleaseKernel);
		stepTwelveKernel = unique_ptr<remove_pointer<cl_kernel>::type, decltype(&clReleaseKernel)>(clCreateKernel(program.get(), "trimEdgesStepTwelve", nullptr), clReleaseKernel);
		stepThirteenKernel = unique_ptr<remove_pointer<cl_kernel>::type, decltype(&clReleaseKernel)>(clCreateKernel(program.get(), "trimEdgesStepThirteen", nullptr), clReleaseKernel);
		stepFourteenKernel = unique_ptr<remove_pointer<cl_kernel>::type, decltype(&clReleaseKernel)>(clCreateKernel(program.get(), "trimEdgesStepFourteen", nullptr), clReleaseKernel);
		stepFifteenKernel = unique_ptr<remove_pointer<cl_kernel>::type, decltype(&clReleaseKernel)>(clCreateKernel(program.get(), "trimEdgesStepFifteen", nullptr), clReleaseKernel);
		stepSixteenKernel = unique_ptr<remove_pointer<cl_kernel>::type, decltype(&clReleaseKernel)>(clCreateKernel(program.get(), "trimEdgesStepSixteen", nullptr), clReleaseKernel);
		stepSeventeenKernel = unique_ptr<remove_pointer<cl_kernel>::type, decltype(&clReleaseKernel)>(clCreateKernel(program.get(), "trimEdgesStepSeventeen", nullptr), clReleaseKernel);
		stepEighteenKernel = unique_ptr<remove_pointer<cl_kernel>::type, decltype(&clReleaseKernel)>(clCreateKernel(program.get(), "trimEdgesStepEighteen", nullptr), clReleaseKernel);
		if(!stepOneKernel || !stepTwoKernel || !stepThreeKernel || !stepFourKernel || !stepFiveKernel || !stepSixKernel || !stepSevenKernel || !stepEightKernel || !stepNineKernel || !stepTenKernel || !stepElevenKernel || !stepTwelveKernel || !stepThirteenKernel || !stepFourteenKernel || !stepFifteenKernel || !stepSixteenKernel || !stepSeventeenKernel || !stepEighteenKernel) {
		
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
		static const unique_ptr<remove_pointer<cl_command_queue>::type, void(*)(cl_command_queue)> commandQueue(clCreateCommandQueue(context, device, CL_QUEUE_PROFILING_ENABLE, nullptr), [](cl_command_queue commandQueue) noexcept {
		
			// Wait for all commands in the queue to finish
			clFinish(commandQueue);
			
			// Free command queue
			clReleaseCommandQueue(commandQueue);
		});
		if(!commandQueue) {
		
			// Display message
			cout << "Creating command queue for the GPU failed" << endl;
			
			// Return false
			return false;
		}
		
		// Check if setting program's unchanging arguments failed
		if(clSetKernelArg(stepOneKernel.get(), 0, sizeof(bucketsOne.get()), &unmove(bucketsOne.get())) != CL_SUCCESS || clSetKernelArg(stepOneKernel.get(), 1, sizeof(numberOfEdgesPerBucketOne.get()), &unmove(numberOfEdgesPerBucketOne.get())) != CL_SUCCESS || clSetKernelArg(stepTwoKernel.get(), 0, sizeof(bucketsOne.get()), &unmove(bucketsOne.get())) != CL_SUCCESS || clSetKernelArg(stepTwoKernel.get(), 1, sizeof(numberOfEdgesPerBucketOne.get()), &unmove(numberOfEdgesPerBucketOne.get())) != CL_SUCCESS || clSetKernelArg(stepTwoKernel.get(), 2, sizeof(nodesBitmap.get()), &unmove(nodesBitmap.get())) != CL_SUCCESS || clSetKernelArg(stepThreeKernel.get(), 0, sizeof(bucketsOne.get()), &unmove(bucketsOne.get())) != CL_SUCCESS || clSetKernelArg(stepThreeKernel.get(), 1, sizeof(numberOfEdgesPerBucketOne.get()), &unmove(numberOfEdgesPerBucketOne.get())) != CL_SUCCESS || clSetKernelArg(stepThreeKernel.get(), 2, sizeof(nodesBitmap.get()), &unmove(nodesBitmap.get())) != CL_SUCCESS || clSetKernelArg(stepFourKernel.get(), 0, sizeof(bucketsOne.get()), &unmove(bucketsOne.get())) != CL_SUCCESS || clSetKernelArg(stepFourKernel.get(), 1, sizeof(numberOfEdgesPerBucketOne.get()), &unmove(numberOfEdgesPerBucketOne.get())) != CL_SUCCESS || clSetKernelArg(stepFourKernel.get(), 2, sizeof(nodesBitmap.get()), &unmove(nodesBitmap.get())) != CL_SUCCESS || clSetKernelArg(stepFourKernel.get(), 3, sizeof(bucketsTwo.get()), &unmove(bucketsTwo.get())) != CL_SUCCESS || clSetKernelArg(stepFourKernel.get(), 4, sizeof(numberOfEdgesPerBucketTwo.get()), &unmove(numberOfEdgesPerBucketTwo.get())) != CL_SUCCESS || clSetKernelArg(stepFiveKernel.get(), 0, sizeof(bucketsTwo.get()), &unmove(bucketsTwo.get())) != CL_SUCCESS || clSetKernelArg(stepFiveKernel.get(), 1, sizeof(numberOfEdgesPerBucketTwo.get()), &unmove(numberOfEdgesPerBucketTwo.get())) != CL_SUCCESS || clSetKernelArg(stepSixKernel.get(), 0, sizeof(bucketsOne.get()), &unmove(bucketsOne.get())) != CL_SUCCESS || clSetKernelArg(stepSixKernel.get(), 1, sizeof(numberOfEdgesPerBucketOne.get()), &unmove(numberOfEdgesPerBucketOne.get())) != CL_SUCCESS || clSetKernelArg(stepSixKernel.get(), 2, sizeof(nodesBitmap.get()), &unmove(nodesBitmap.get())) != CL_SUCCESS || clSetKernelArg(stepSixKernel.get(), 3, sizeof(bucketsTwo.get()), &unmove(bucketsTwo.get())) != CL_SUCCESS || clSetKernelArg(stepSixKernel.get(), 4, sizeof(numberOfEdgesPerBucketTwo.get()), &unmove(numberOfEdgesPerBucketTwo.get())) != CL_SUCCESS || clSetKernelArg(stepSevenKernel.get(), 1, sizeof(bucketsOne.get()), &unmove(bucketsOne.get())) != CL_SUCCESS || clSetKernelArg(stepSevenKernel.get(), 2, sizeof(numberOfEdgesPerBucketOne.get()), &unmove(numberOfEdgesPerBucketOne.get())) != CL_SUCCESS || clSetKernelArg(stepEightKernel.get(), 0, sizeof(bucketsOne.get()), &unmove(bucketsOne.get())) != CL_SUCCESS || clSetKernelArg(stepEightKernel.get(), 1, sizeof(numberOfEdgesPerBucketOne.get()), &unmove(numberOfEdgesPerBucketOne.get())) != CL_SUCCESS || clSetKernelArg(stepEightKernel.get(), 2, sizeof(nodesBitmap.get()), &unmove(nodesBitmap.get())) != CL_SUCCESS || clSetKernelArg(stepNineKernel.get(), 0, sizeof(bucketsOne.get()), &unmove(bucketsOne.get())) != CL_SUCCESS || clSetKernelArg(stepNineKernel.get(), 1, sizeof(numberOfEdgesPerBucketOne.get()), &unmove(numberOfEdgesPerBucketOne.get())) != CL_SUCCESS || clSetKernelArg(stepNineKernel.get(), 2, sizeof(nodesBitmap.get()), &unmove(nodesBitmap.get())) != CL_SUCCESS || clSetKernelArg(stepTenKernel.get(), 0, sizeof(bucketsOne.get()), &unmove(bucketsOne.get())) != CL_SUCCESS || clSetKernelArg(stepTenKernel.get(), 1, sizeof(numberOfEdgesPerBucketOne.get()), &unmove(numberOfEdgesPerBucketOne.get())) != CL_SUCCESS || clSetKernelArg(stepTenKernel.get(), 2, sizeof(nodesBitmap.get()), &unmove(nodesBitmap.get())) != CL_SUCCESS || clSetKernelArg(stepTenKernel.get(), 3, sizeof(bucketsTwo.get()), &unmove(bucketsTwo.get())) != CL_SUCCESS || clSetKernelArg(stepTenKernel.get(), 4, sizeof(numberOfEdgesPerBucketTwo.get()), &unmove(numberOfEdgesPerBucketTwo.get())) != CL_SUCCESS || clSetKernelArg(stepElevenKernel.get(), 0, sizeof(bucketsTwo.get()), &unmove(bucketsTwo.get())) != CL_SUCCESS || clSetKernelArg(stepElevenKernel.get(), 1, sizeof(numberOfEdgesPerBucketTwo.get()), &unmove(numberOfEdgesPerBucketTwo.get())) != CL_SUCCESS || clSetKernelArg(stepTwelveKernel.get(), 0, sizeof(bucketsOne.get()), &unmove(bucketsOne.get())) != CL_SUCCESS || clSetKernelArg(stepTwelveKernel.get(), 1, sizeof(numberOfEdgesPerBucketOne.get()), &unmove(numberOfEdgesPerBucketOne.get())) != CL_SUCCESS || clSetKernelArg(stepTwelveKernel.get(), 2, sizeof(nodesBitmap.get()), &unmove(nodesBitmap.get())) != CL_SUCCESS || clSetKernelArg(stepTwelveKernel.get(), 3, sizeof(bucketsTwo.get()), &unmove(bucketsTwo.get())) != CL_SUCCESS || clSetKernelArg(stepTwelveKernel.get(), 4, sizeof(numberOfEdgesPerBucketTwo.get()), &unmove(numberOfEdgesPerBucketTwo.get())) != CL_SUCCESS || clSetKernelArg(stepThirteenKernel.get(), 1, sizeof(bucketsOne.get()), &unmove(bucketsOne.get())) != CL_SUCCESS || clSetKernelArg(stepThirteenKernel.get(), 2, sizeof(numberOfEdgesPerBucketOne.get()), &unmove(numberOfEdgesPerBucketOne.get())) != CL_SUCCESS || clSetKernelArg(stepFourteenKernel.get(), 0, sizeof(bucketsOne.get()), &unmove(bucketsOne.get())) != CL_SUCCESS || clSetKernelArg(stepFourteenKernel.get(), 1, sizeof(numberOfEdgesPerBucketOne.get()), &unmove(numberOfEdgesPerBucketOne.get())) != CL_SUCCESS || clSetKernelArg(stepFourteenKernel.get(), 2, sizeof(nodesBitmap.get()), &unmove(nodesBitmap.get())) != CL_SUCCESS || clSetKernelArg(stepFifteenKernel.get(), 0, sizeof(bucketsOne.get()), &unmove(bucketsOne.get())) != CL_SUCCESS || clSetKernelArg(stepFifteenKernel.get(), 1, sizeof(numberOfEdgesPerBucketOne.get()), &unmove(numberOfEdgesPerBucketOne.get())) != CL_SUCCESS || clSetKernelArg(stepFifteenKernel.get(), 2, sizeof(nodesBitmap.get()), &unmove(nodesBitmap.get())) != CL_SUCCESS || clSetKernelArg(stepSixteenKernel.get(), 0, sizeof(bucketsOne.get()), &unmove(bucketsOne.get())) != CL_SUCCESS || clSetKernelArg(stepSixteenKernel.get(), 1, sizeof(numberOfEdgesPerBucketOne.get()), &unmove(numberOfEdgesPerBucketOne.get())) != CL_SUCCESS || clSetKernelArg(stepSixteenKernel.get(), 2, sizeof(nodesBitmap.get()), &unmove(nodesBitmap.get())) != CL_SUCCESS || clSetKernelArg(stepSixteenKernel.get(), 3, sizeof(bucketsTwo.get()), &unmove(bucketsTwo.get())) != CL_SUCCESS || clSetKernelArg(stepSixteenKernel.get(), 4, sizeof(numberOfEdgesPerBucketTwo.get()), &unmove(numberOfEdgesPerBucketTwo.get())) != CL_SUCCESS || clSetKernelArg(stepSeventeenKernel.get(), 0, sizeof(bucketsTwo.get()), &unmove(bucketsTwo.get())) != CL_SUCCESS || clSetKernelArg(stepSeventeenKernel.get(), 1, sizeof(numberOfEdgesPerBucketTwo.get()), &unmove(numberOfEdgesPerBucketTwo.get())) != CL_SUCCESS || clSetKernelArg(stepEighteenKernel.get(), 0, sizeof(bucketsOne.get()), &unmove(bucketsOne.get())) != CL_SUCCESS || clSetKernelArg(stepEighteenKernel.get(), 1, sizeof(numberOfEdgesPerBucketOne.get()), &unmove(numberOfEdgesPerBucketOne.get())) != CL_SUCCESS || clSetKernelArg(stepEighteenKernel.get(), 2, sizeof(nodesBitmap.get()), &unmove(nodesBitmap.get())) != CL_SUCCESS || clSetKernelArg(stepEighteenKernel.get(), 3, sizeof(bucketsTwo.get()), &unmove(bucketsTwo.get())) != CL_SUCCESS || clSetKernelArg(stepEighteenKernel.get(), 4, sizeof(numberOfEdgesPerBucketTwo.get()), &unmove(numberOfEdgesPerBucketTwo.get())) != CL_SUCCESS) {
		
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
		
		// Display message
		cout << "Mining started" << endl << endl << "Mining info:" << endl << "\tMining rate:\t 0 graph(s)/second" << endl << "\tGraphs checked:\t 0" << endl;
		
		// Check if not tuning
		#ifndef TUNING
		
			// Display message
			cout << "\tSolutions found: 0" << endl;
		#endif
		
		// Display message
		cout << "Pipeline stages:" << endl;
		
		// Set previous graph processed time to now
		previousGraphProcessedTime = chrono::high_resolution_clock::now();
		
		// Check if queuing clearing number of edges per bucket one on the device failed
		static Event firstCommandEvent;
		if(clEnqueueFillBuffer(commandQueue.get(), numberOfEdgesPerBucketOne.get(), (const cl_ulong[]){0}, (SLEAN_TRIMMING_NUMBER_OF_BUCKETS == 1) ? sizeof(cl_uint) : sizeof(cl_ulong), 0, SLEAN_TRIMMING_NUMBER_OF_BUCKETS * sizeof(cl_uint), 0, nullptr, firstCommandEvent.getAddress()) != CL_SUCCESS) {
		
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
		
		// Check if setting program's part or SipHash keys arguments failed
		if(clSetKernelArg(stepOneKernel.get(), 2, sizeof(cl_uchar), &unmove(static_cast<cl_uchar>(0))) != CL_SUCCESS || clSetKernelArg(stepOneKernel.get(), 3, sizeof(sipHashKeysOne), &sipHashKeysOne) != CL_SUCCESS) {
		
			// Display message
			cout << "Setting program's arguments on the GPU failed" << endl;
			
			// Return false
			return false;
		}
		
		// Check if queuing running step one on the device failed
		if(clEnqueueNDRangeKernel(commandQueue.get(), stepOneKernel.get(), 1, nullptr, &totalNumberOfWorkItems[0], &workItemsPerWorkGroup[0], 0, nullptr, nullptr) != CL_SUCCESS) {
		
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
		if(clEnqueueNDRangeKernel(commandQueue.get(), stepTwoKernel.get(), 1, nullptr, &totalNumberOfWorkItems[1], &workItemsPerWorkGroup[1], 0, nullptr, nullptr) != CL_SUCCESS) {
		
			// Display message
			cout << "Running program on the GPU failed" << endl;
			
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
			if(clEnqueueFillBuffer(commandQueue.get(), numberOfEdgesPerBucketOne.get(), (const cl_ulong[]){0}, (SLEAN_TRIMMING_NUMBER_OF_BUCKETS == 1) ? sizeof(cl_uint) : sizeof(cl_ulong), 0, SLEAN_TRIMMING_NUMBER_OF_BUCKETS * sizeof(cl_uint), 0, nullptr, nullptr) != CL_SUCCESS) {
			
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
			if(clEnqueueNDRangeKernel(commandQueue.get(), stepOneKernel.get(), 1, nullptr, &totalNumberOfWorkItems[0], &workItemsPerWorkGroup[0], 0, nullptr, nullptr) != CL_SUCCESS) {
			
				// Display message
				cout << "Running program on the GPU failed" << endl;
				
				// Return false
				return false;
			}
			
			// Check if not the last trimming part
			if(i != SLEAN_TRIMMING_PARTS - 1) {
			
				// Check if queuing running step three on the device failed
				if(clEnqueueNDRangeKernel(commandQueue.get(), stepThreeKernel.get(), 1, nullptr, &totalNumberOfWorkItems[2], &workItemsPerWorkGroup[2], 0, nullptr, nullptr) != CL_SUCCESS) {
				
					// Display message
					cout << "Running program on the GPU failed" << endl;
					
					// Return false
					return false;
				}
			}
			
			// Otherwise
			else {
			
				// Check if queuing clearing number of edges per bucket two on the device failed
				if(clEnqueueFillBuffer(commandQueue.get(), numberOfEdgesPerBucketTwo.get(), (const cl_ulong[]){0}, (SLEAN_TRIMMING_NUMBER_OF_REMAINING_EDGES_BUCKETS == 1) ? sizeof(cl_uint) : sizeof(cl_ulong), 0, SLEAN_TRIMMING_NUMBER_OF_REMAINING_EDGES_BUCKETS * sizeof(cl_uint), 0, nullptr, nullptr) != CL_SUCCESS) {
				
					// Display message
					cout << "Preparing program's arguments on the GPU failed" << endl;
					
					// Return false
					return false;
				}
				
				// Check if setting program's SipHash keys argument failed
				if(clSetKernelArg(stepFourKernel.get(), 5, sizeof(sipHashKeysOne), &sipHashKeysOne) != CL_SUCCESS) {
				
					// Display message
					cout << "Setting program's arguments on the GPU failed" << endl;
					
					// Return false
					return false;
				}
				
				// Check if queuing running step four on the device failed
				if(clEnqueueNDRangeKernel(commandQueue.get(), stepFourKernel.get(), 1, nullptr, &totalNumberOfWorkItems[3], &workItemsPerWorkGroup[3], 0, nullptr, nullptr) != CL_SUCCESS) {
				
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
		if(clEnqueueNDRangeKernel(commandQueue.get(), stepFiveKernel.get(), 1, nullptr, &totalNumberOfWorkItems[4], &workItemsPerWorkGroup[4], 0, nullptr, nullptr) != CL_SUCCESS) {
		
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
			if(clEnqueueFillBuffer(commandQueue.get(), numberOfEdgesPerBucketOne.get(), (const cl_ulong[]){0}, (SLEAN_TRIMMING_NUMBER_OF_BUCKETS == 1) ? sizeof(cl_uint) : sizeof(cl_ulong), 0, SLEAN_TRIMMING_NUMBER_OF_BUCKETS * sizeof(cl_uint), 0, nullptr, nullptr) != CL_SUCCESS) {
			
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
			if(clEnqueueNDRangeKernel(commandQueue.get(), stepOneKernel.get(), 1, nullptr, &totalNumberOfWorkItems[0], &workItemsPerWorkGroup[0], 0, nullptr, nullptr) != CL_SUCCESS) {
			
				// Display message
				cout << "Running program on the GPU failed" << endl;
				
				// Return false
				return false;
			}
			
			// Check if queuing clearing number of edges per bucket two on the device failed
			if(clEnqueueFillBuffer(commandQueue.get(), numberOfEdgesPerBucketTwo.get(), (const cl_ulong[]){0}, (SLEAN_TRIMMING_NUMBER_OF_REMAINING_EDGES_BUCKETS == 1) ? sizeof(cl_uint) : sizeof(cl_ulong), 0, SLEAN_TRIMMING_NUMBER_OF_REMAINING_EDGES_BUCKETS * sizeof(cl_uint), 0, nullptr, nullptr) != CL_SUCCESS) {
			
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
			if(clEnqueueNDRangeKernel(commandQueue.get(), stepSixKernel.get(), 1, nullptr, &totalNumberOfWorkItems[5], &workItemsPerWorkGroup[5], 0, nullptr, nullptr) != CL_SUCCESS) {
			
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
			if(clEnqueueNDRangeKernel(commandQueue.get(), stepFiveKernel.get(), 1, nullptr, &totalNumberOfWorkItems[4], &workItemsPerWorkGroup[4], 0, nullptr, nullptr) != CL_SUCCESS) {
			
				// Display message
				cout << "Running program on the GPU failed" << endl;
				
				// Return false
				return false;
			}
		}
		
		// Check if trimming more than one round
		if(TRIMMING_ROUNDS > 1) {
		
			// Check if queuing clearing number of edges per bucket one on the device failed
			if(clEnqueueFillBuffer(commandQueue.get(), numberOfEdgesPerBucketOne.get(), (const cl_ulong[]){0}, (SLEAN_TRIMMING_NUMBER_OF_BUCKETS == 1) ? sizeof(cl_uint) : sizeof(cl_ulong), 0, SLEAN_TRIMMING_NUMBER_OF_BUCKETS * sizeof(cl_uint), 0, nullptr, nullptr) != CL_SUCCESS) {
			
				// Display message
				cout << "Preparing program's arguments on the GPU failed" << endl;
				
				// Return false
				return false;
			}
			
			// Check if setting program's edges bitmap, part, or SipHash keys arguments failed
			if(clSetKernelArg(stepSevenKernel.get(), 0, sizeof(edgesBitmapOne.get()), &unmove(edgesBitmapOne.get())) != CL_SUCCESS || clSetKernelArg(stepSevenKernel.get(), 3, sizeof(cl_uchar), &unmove(static_cast<cl_uchar>(0))) != CL_SUCCESS || clSetKernelArg(stepSevenKernel.get(), 4, sizeof(sipHashKeysOne), &sipHashKeysOne) != CL_SUCCESS) {
			
				// Display message
				cout << "Setting program's arguments on the GPU failed" << endl;
				
				// Return false
				return false;
			}
			
			// Check if queuing running step seven on the device failed
			if(clEnqueueNDRangeKernel(commandQueue.get(), stepSevenKernel.get(), 1, nullptr, &totalNumberOfWorkItems[6], &workItemsPerWorkGroup[6], 0, nullptr, nullptr) != CL_SUCCESS) {
			
				// Display message
				cout << "Running program on the GPU failed" << endl;
				
				// Return false
				return false;
			}
			
			// Check if setting program's SipHash keys argument failed
			if(clSetKernelArg(stepEightKernel.get(), 3, sizeof(sipHashKeysOne), &sipHashKeysOne) != CL_SUCCESS) {
			
				// Display message
				cout << "Setting program's arguments on the GPU failed" << endl;
				
				// Return false
				return false;
			}
			
			// Check if queuing running step eight on the device failed
			if(clEnqueueNDRangeKernel(commandQueue.get(), stepEightKernel.get(), 1, nullptr, &totalNumberOfWorkItems[7], &workItemsPerWorkGroup[7], 0, nullptr, nullptr) != CL_SUCCESS) {
			
				// Display message
				cout << "Running program on the GPU failed" << endl;
				
				// Return false
				return false;
			}
			
			// Check if setting program's SipHash keys argument failed
			if(clSetKernelArg(stepNineKernel.get(), 3, sizeof(sipHashKeysOne), &sipHashKeysOne) != CL_SUCCESS) {
			
				// Display message
				cout << "Setting program's arguments on the GPU failed" << endl;
				
				// Return false
				return false;
			}
			
			// Go through all remaining trimming parts
			for(unsigned int i = 1; i < SLEAN_TRIMMING_PARTS; ++i) {
			
				// Check if queuing clearing number of edges per bucket one on the device failed
				if(clEnqueueFillBuffer(commandQueue.get(), numberOfEdgesPerBucketOne.get(), (const cl_ulong[]){0}, (SLEAN_TRIMMING_NUMBER_OF_BUCKETS == 1) ? sizeof(cl_uint) : sizeof(cl_ulong), 0, SLEAN_TRIMMING_NUMBER_OF_BUCKETS * sizeof(cl_uint), 0, nullptr, nullptr) != CL_SUCCESS) {
				
					// Display message
					cout << "Preparing program's arguments on the GPU failed" << endl;
					
					// Return false
					return false;
				}
				
				// Check if setting program's part argument failed
				if(clSetKernelArg(stepSevenKernel.get(), 3, sizeof(cl_uchar), &unmove(static_cast<cl_uchar>(i))) != CL_SUCCESS) {
				
					// Display message
					cout << "Setting program's arguments on the GPU failed" << endl;
					
					// Return false
					return false;
				}
				
				// Check if queuing running step seven on the device failed
				if(clEnqueueNDRangeKernel(commandQueue.get(), stepSevenKernel.get(), 1, nullptr, &totalNumberOfWorkItems[6], &workItemsPerWorkGroup[6], 0, nullptr, nullptr) != CL_SUCCESS) {
				
					// Display message
					cout << "Running program on the GPU failed" << endl;
					
					// Return false
					return false;
				}
				
				// Check if not the last trimming part
				if(i != SLEAN_TRIMMING_PARTS - 1) {
				
					// Check if queuing running step nine on the device failed
					if(clEnqueueNDRangeKernel(commandQueue.get(), stepNineKernel.get(), 1, nullptr, &totalNumberOfWorkItems[8], &workItemsPerWorkGroup[8], 0, nullptr, nullptr) != CL_SUCCESS) {
					
						// Display message
						cout << "Running program on the GPU failed" << endl;
						
						// Return false
						return false;
					}
				}
				
				// Otherwise
				else {
				
					// Check if queuing clearing number of edges per bucket two on the device failed
					if(clEnqueueFillBuffer(commandQueue.get(), numberOfEdgesPerBucketTwo.get(), (const cl_ulong[]){0}, (SLEAN_TRIMMING_NUMBER_OF_REMAINING_EDGES_BUCKETS == 1) ? sizeof(cl_uint) : sizeof(cl_ulong), 0, SLEAN_TRIMMING_NUMBER_OF_REMAINING_EDGES_BUCKETS * sizeof(cl_uint), 0, nullptr, nullptr) != CL_SUCCESS) {
					
						// Display message
						cout << "Preparing program's arguments on the GPU failed" << endl;
						
						// Return false
						return false;
					}
					
					// Check if setting program's SipHash keys argument failed
					if(clSetKernelArg(stepTenKernel.get(), 5, sizeof(sipHashKeysOne), &sipHashKeysOne) != CL_SUCCESS) {
					
						// Display message
						cout << "Setting program's arguments on the GPU failed" << endl;
						
						// Return false
						return false;
					}
					
					// Check if queuing running step ten on the device failed
					if(clEnqueueNDRangeKernel(commandQueue.get(), stepTenKernel.get(), 1, nullptr, &totalNumberOfWorkItems[9], &workItemsPerWorkGroup[9], 0, nullptr, nullptr) != CL_SUCCESS) {
					
						// Display message
						cout << "Running program on the GPU failed" << endl;
						
						// Return false
						return false;
					}
				}
			}
			
			// Check if setting program's edges bitmap or part arguments failed
			if(clSetKernelArg(stepElevenKernel.get(), 2, sizeof(edgesBitmapOne.get()), &unmove(edgesBitmapOne.get())) != CL_SUCCESS || clSetKernelArg(stepElevenKernel.get(), 3, sizeof(cl_uchar), &unmove(static_cast<cl_uchar>(SLEAN_TRIMMING_PARTS - 1))) != CL_SUCCESS) {
			
				// Display message
				cout << "Setting program's arguments on the GPU failed" << endl;
				
				// Return false
				return false;
			}
			
			// Check if queuing running step eleven on the device failed
			if(clEnqueueNDRangeKernel(commandQueue.get(), stepElevenKernel.get(), 1, nullptr, &totalNumberOfWorkItems[10], &workItemsPerWorkGroup[10], 0, nullptr, nullptr) != CL_SUCCESS) {
			
				// Display message
				cout << "Running program on the GPU failed" << endl;
				
				// Return false
				return false;
			}
			
			// Check if setting program's SipHash keys argument failed
			if(clSetKernelArg(stepTwelveKernel.get(), 6, sizeof(sipHashKeysOne), &sipHashKeysOne) != CL_SUCCESS) {
			
				// Display message
				cout << "Setting program's arguments on the GPU failed" << endl;
				
				// Return false
				return false;
			}
			
			// Go through all remaining trimming parts except the last one
			for(unsigned int i = 0; i < SLEAN_TRIMMING_PARTS - 1; ++i) {
			
				// Check if queuing clearing number of edges per bucket one on the device failed
				if(clEnqueueFillBuffer(commandQueue.get(), numberOfEdgesPerBucketOne.get(), (const cl_ulong[]){0}, (SLEAN_TRIMMING_NUMBER_OF_BUCKETS == 1) ? sizeof(cl_uint) : sizeof(cl_ulong), 0, SLEAN_TRIMMING_NUMBER_OF_BUCKETS * sizeof(cl_uint), 0, nullptr, nullptr) != CL_SUCCESS) {
				
					// Display message
					cout << "Preparing program's arguments on the GPU failed" << endl;
					
					// Return false
					return false;
				}
				
				// Check if setting program's part argument failed
				if(clSetKernelArg(stepSevenKernel.get(), 3, sizeof(cl_uchar), &unmove(static_cast<cl_uchar>(i))) != CL_SUCCESS) {
				
					// Display message
					cout << "Setting program's arguments on the GPU failed" << endl;
					
					// Return false
					return false;
				}
				
				// Check if queuing running step seven on the device failed
				if(clEnqueueNDRangeKernel(commandQueue.get(), stepSevenKernel.get(), 1, nullptr, &totalNumberOfWorkItems[6], &workItemsPerWorkGroup[6], 0, nullptr, nullptr) != CL_SUCCESS) {
				
					// Display message
					cout << "Running program on the GPU failed" << endl;
					
					// Return false
					return false;
				}
				
				// Check if queuing clearing number of edges per bucket two on the device failed
				if(clEnqueueFillBuffer(commandQueue.get(), numberOfEdgesPerBucketTwo.get(), (const cl_ulong[]){0}, (SLEAN_TRIMMING_NUMBER_OF_REMAINING_EDGES_BUCKETS == 1) ? sizeof(cl_uint) : sizeof(cl_ulong), 0, SLEAN_TRIMMING_NUMBER_OF_REMAINING_EDGES_BUCKETS * sizeof(cl_uint), 0, nullptr, nullptr) != CL_SUCCESS) {
				
					// Display message
					cout << "Preparing program's arguments on the GPU failed" << endl;
					
					// Return false
					return false;
				}
				
				// Check if setting program's part argument failed
				if(clSetKernelArg(stepTwelveKernel.get(), 5, sizeof(cl_uchar), &unmove(static_cast<cl_uchar>(i))) != CL_SUCCESS) {
				
					// Display message
					cout << "Setting program's arguments on the GPU failed" << endl;
					
					// Return false
					return false;
				}
				
				// Check if queuing running step twelve on the device failed
				if(clEnqueueNDRangeKernel(commandQueue.get(), stepTwelveKernel.get(), 1, nullptr, &totalNumberOfWorkItems[11], &workItemsPerWorkGroup[11], 0, nullptr, nullptr) != CL_SUCCESS) {
				
					// Display message
					cout << "Running program on the GPU failed" << endl;
					
					// Return false
					return false;
				}
				
				// Check if setting program's part argument failed
				if(clSetKernelArg(stepElevenKernel.get(), 3, sizeof(cl_uchar), &unmove(static_cast<cl_uchar>(i))) != CL_SUCCESS) {
				
					// Display message
					cout << "Setting program's arguments on the GPU failed" << endl;
					
					// Return false
					return false;
				}
				
				// Check if queuing running step eleven on the device failed
				if(clEnqueueNDRangeKernel(commandQueue.get(), stepElevenKernel.get(), 1, nullptr, &totalNumberOfWorkItems[10], &workItemsPerWorkGroup[10], 0, nullptr, nullptr) != CL_SUCCESS) {
				
					// Display message
					cout << "Running program on the GPU failed" << endl;
					
					// Return false
					return false;
				}
			}
			
			// Go through all remaining trimming rounds
			for(unsigned int i = 2; i < TRIMMING_ROUNDS; ++i) {
			
				// Check if queuing clearing number of edges per bucket one on the device failed
				if(clEnqueueFillBuffer(commandQueue.get(), numberOfEdgesPerBucketOne.get(), (const cl_ulong[]){0}, (SLEAN_TRIMMING_NUMBER_OF_BUCKETS == 1) ? sizeof(cl_uint) : sizeof(cl_ulong), 0, SLEAN_TRIMMING_NUMBER_OF_BUCKETS * sizeof(cl_uint), 0, nullptr, nullptr) != CL_SUCCESS) {
				
					// Display message
					cout << "Preparing program's arguments on the GPU failed" << endl;
					
					// Return false
					return false;
				}
				
				// Check if setting program's edges bitmap, part, nodes in second partition, or SipHash keys arguments failed
				if(clSetKernelArg(stepThirteenKernel.get(), 0, sizeof(edgesBitmapOne.get()), &unmove(edgesBitmapOne.get())) != CL_SUCCESS || clSetKernelArg(stepThirteenKernel.get(), 3, sizeof(cl_uchar), &unmove(static_cast<cl_uchar>(0))) != CL_SUCCESS || clSetKernelArg(stepThirteenKernel.get(), 4, sizeof(cl_uchar), &unmove(static_cast<cl_uchar>(i % 2))) != CL_SUCCESS || clSetKernelArg(stepThirteenKernel.get(), 5, sizeof(sipHashKeysOne), &sipHashKeysOne) != CL_SUCCESS) {
				
					// Display message
					cout << "Setting program's arguments on the GPU failed" << endl;
					
					// Return false
					return false;
				}
				
				// Check if queuing running step thirteen on the device failed
				if(clEnqueueNDRangeKernel(commandQueue.get(), stepThirteenKernel.get(), 1, nullptr, &totalNumberOfWorkItems[12], &workItemsPerWorkGroup[12], 0, nullptr, nullptr) != CL_SUCCESS) {
				
					// Display message
					cout << "Running program on the GPU failed" << endl;
					
					// Return false
					return false;
				}
				
				// Check if queuing running step fourteen on the device failed
				if(clEnqueueNDRangeKernel(commandQueue.get(), stepFourteenKernel.get(), 1, nullptr, &totalNumberOfWorkItems[13], &workItemsPerWorkGroup[13], 0, nullptr, nullptr) != CL_SUCCESS) {
				
					// Display message
					cout << "Running program on the GPU failed" << endl;
					
					// Return false
					return false;
				}
				
				// Go through all remaining trimming parts
				for(unsigned int j = 1; j < SLEAN_TRIMMING_PARTS; ++j) {
				
					// Check if queuing clearing number of edges per bucket one on the device failed
					if(clEnqueueFillBuffer(commandQueue.get(), numberOfEdgesPerBucketOne.get(), (const cl_ulong[]){0}, (SLEAN_TRIMMING_NUMBER_OF_BUCKETS == 1) ? sizeof(cl_uint) : sizeof(cl_ulong), 0, SLEAN_TRIMMING_NUMBER_OF_BUCKETS * sizeof(cl_uint), 0, nullptr, nullptr) != CL_SUCCESS) {
					
						// Display message
						cout << "Preparing program's arguments on the GPU failed" << endl;
						
						// Return false
						return false;
					}
					
					// Check if setting program's part argument failed
					if(clSetKernelArg(stepThirteenKernel.get(), 3, sizeof(cl_uchar), &unmove(static_cast<cl_uchar>(j))) != CL_SUCCESS) {
					
						// Display message
						cout << "Setting program's arguments on the GPU failed" << endl;
						
						// Return false
						return false;
					}
					
					// Check if queuing running step thirteen on the device failed
					if(clEnqueueNDRangeKernel(commandQueue.get(), stepThirteenKernel.get(), 1, nullptr, &totalNumberOfWorkItems[12], &workItemsPerWorkGroup[12], 0, nullptr, nullptr) != CL_SUCCESS) {
					
						// Display message
						cout << "Running program on the GPU failed" << endl;
						
						// Return false
						return false;
					}
					
					// Check if not the last trimming part
					if(j != SLEAN_TRIMMING_PARTS - 1) {
					
						// Check if queuing running step fifteen on the device failed
						if(clEnqueueNDRangeKernel(commandQueue.get(), stepFifteenKernel.get(), 1, nullptr, &totalNumberOfWorkItems[14], &workItemsPerWorkGroup[14], 0, nullptr, nullptr) != CL_SUCCESS) {
						
							// Display message
							cout << "Running program on the GPU failed" << endl;
							
							// Return false
							return false;
						}
					}
					
					// Otherwise
					else {
					
						// Check if queuing clearing number of edges per bucket two on the device failed
						if(clEnqueueFillBuffer(commandQueue.get(), numberOfEdgesPerBucketTwo.get(), (const cl_ulong[]){0}, (SLEAN_TRIMMING_NUMBER_OF_REMAINING_EDGES_BUCKETS == 1) ? sizeof(cl_uint) : sizeof(cl_ulong), 0, SLEAN_TRIMMING_NUMBER_OF_REMAINING_EDGES_BUCKETS * sizeof(cl_uint), 0, nullptr, nullptr) != CL_SUCCESS) {
						
							// Display message
							cout << "Preparing program's arguments on the GPU failed" << endl;
							
							// Return false
							return false;
						}
						
						// Check if queuing running step sixteen on the device failed
						if(clEnqueueNDRangeKernel(commandQueue.get(), stepSixteenKernel.get(), 1, nullptr, &totalNumberOfWorkItems[15], &workItemsPerWorkGroup[15], 0, nullptr, nullptr) != CL_SUCCESS) {
						
							// Display message
							cout << "Running program on the GPU failed" << endl;
							
							// Return false
							return false;
						}
					}
				}
				
				// Check if setting program's edges bitmap or part arguments failed
				if(clSetKernelArg(stepSeventeenKernel.get(), 2, sizeof(edgesBitmapOne.get()), &unmove(edgesBitmapOne.get())) != CL_SUCCESS || clSetKernelArg(stepSeventeenKernel.get(), 3, sizeof(cl_uchar), &unmove(static_cast<cl_uchar>(SLEAN_TRIMMING_PARTS - 1))) != CL_SUCCESS) {
				
					// Display message
					cout << "Setting program's arguments on the GPU failed" << endl;
					
					// Return false
					return false;
				}
				
				// Check if queuing running step seventeen on the device failed
				if(clEnqueueNDRangeKernel(commandQueue.get(), stepSeventeenKernel.get(), 1, nullptr, &totalNumberOfWorkItems[16], &workItemsPerWorkGroup[16], 0, nullptr, nullptr) != CL_SUCCESS) {
				
					// Display message
					cout << "Running program on the GPU failed" << endl;
					
					// Return false
					return false;
				}
				
				// Go through all remaining trimming parts except the last one
				for(unsigned int j = 0; j < SLEAN_TRIMMING_PARTS - 1; ++j) {
				
					// Check if queuing clearing number of edges per bucket one on the device failed
					if(clEnqueueFillBuffer(commandQueue.get(), numberOfEdgesPerBucketOne.get(), (const cl_ulong[]){0}, (SLEAN_TRIMMING_NUMBER_OF_BUCKETS == 1) ? sizeof(cl_uint) : sizeof(cl_ulong), 0, SLEAN_TRIMMING_NUMBER_OF_BUCKETS * sizeof(cl_uint), 0, nullptr, nullptr) != CL_SUCCESS) {
					
						// Display message
						cout << "Preparing program's arguments on the GPU failed" << endl;
						
						// Return false
						return false;
					}
					
					// Check if setting program's part argument failed
					if(clSetKernelArg(stepThirteenKernel.get(), 3, sizeof(cl_uchar), &unmove(static_cast<cl_uchar>(j))) != CL_SUCCESS) {
					
						// Display message
						cout << "Setting program's arguments on the GPU failed" << endl;
						
						// Return false
						return false;
					}
					
					// Check if queuing running step thirteen on the device failed
					if(clEnqueueNDRangeKernel(commandQueue.get(), stepThirteenKernel.get(), 1, nullptr, &totalNumberOfWorkItems[12], &workItemsPerWorkGroup[12], 0, nullptr, nullptr) != CL_SUCCESS) {
					
						// Display message
						cout << "Running program on the GPU failed" << endl;
						
						// Return false
						return false;
					}
					
					// Check if queuing clearing number of edges per bucket two on the device failed
					if(clEnqueueFillBuffer(commandQueue.get(), numberOfEdgesPerBucketTwo.get(), (const cl_ulong[]){0}, (SLEAN_TRIMMING_NUMBER_OF_REMAINING_EDGES_BUCKETS == 1) ? sizeof(cl_uint) : sizeof(cl_ulong), 0, SLEAN_TRIMMING_NUMBER_OF_REMAINING_EDGES_BUCKETS * sizeof(cl_uint), 0, nullptr, nullptr) != CL_SUCCESS) {
					
						// Display message
						cout << "Preparing program's arguments on the GPU failed" << endl;
						
						// Return false
						return false;
					}
					
					// Check if setting program's part argument failed
					if(clSetKernelArg(stepEighteenKernel.get(), 5, sizeof(cl_uchar), &unmove(static_cast<cl_uchar>(j))) != CL_SUCCESS) {
					
						// Display message
						cout << "Setting program's arguments on the GPU failed" << endl;
						
						// Return false
						return false;
					}
					
					// Check if queuing running step eighteen on the device failed
					if(clEnqueueNDRangeKernel(commandQueue.get(), stepEighteenKernel.get(), 1, nullptr, &totalNumberOfWorkItems[17], &workItemsPerWorkGroup[17], 0, nullptr, nullptr) != CL_SUCCESS) {
					
						// Display message
						cout << "Running program on the GPU failed" << endl;
						
						// Return false
						return false;
					}
					
					// Check if setting program's part argument failed
					if(clSetKernelArg(stepSeventeenKernel.get(), 3, sizeof(cl_uchar), &unmove(static_cast<cl_uchar>(j))) != CL_SUCCESS) {
					
						// Display message
						cout << "Setting program's arguments on the GPU failed" << endl;
						
						// Return false
						return false;
					}
					
					// Check if queuing running step seventeen on the device failed
					if(clEnqueueNDRangeKernel(commandQueue.get(), stepSeventeenKernel.get(), 1, nullptr, &totalNumberOfWorkItems[16], &workItemsPerWorkGroup[16], 0, nullptr, nullptr) != CL_SUCCESS) {
					
						// Display message
						cout << "Running program on the GPU failed" << endl;
						
						// Return false
						return false;
					}
				}
			}
		}
		
		// Check if queuing map result failed
		static Event mapEvent;
		static uint64_t *resultOne = reinterpret_cast<uint64_t *>(clEnqueueMapBuffer(commandQueue.get(), edgesBitmapOne.get(), CL_FALSE, CL_MAP_READ, 0, NUMBER_OF_EDGES / BITS_IN_A_BYTE, 0, nullptr, mapEvent.getAddress(), nullptr));
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
				clEnqueueUnmapMemObject(commandQueue.get(), edgesBitmapOne.get(), reinterpret_cast<void *>(resultOne), mapEvent.get() ? 1 : 0, mapEvent.getAddress(), nullptr);
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
		if(clGetEventProfilingInfo(firstCommandEvent.get(), CL_PROFILING_COMMAND_QUEUED, sizeof(startTime), &startTime, nullptr) != CL_SUCCESS || clGetEventProfilingInfo(mapEvent.get(), CL_PROFILING_COMMAND_END, sizeof(endTime), &endTime, nullptr) != CL_SUCCESS) {
		
			// Display message
			cout << "Getting GPU trimming time failed" << endl;
			
			// Return false
			return false;
		}
		
		// Display message
		cout << "\tTrimming time:\t " << static_cast<chrono::duration<double>>(static_cast<chrono::nanoseconds>(endTime - startTime)).count() << " second(s)" << endl;
		
		// Check if queuing clearing number of edges per bucket one on the device failed
		firstCommandEvent.free();
		if(clEnqueueFillBuffer(commandQueue.get(), numberOfEdgesPerBucketOne.get(), (const cl_ulong[]){0}, (SLEAN_TRIMMING_NUMBER_OF_BUCKETS == 1) ? sizeof(cl_uint) : sizeof(cl_ulong), 0, SLEAN_TRIMMING_NUMBER_OF_BUCKETS * sizeof(cl_uint), 0, nullptr, firstCommandEvent.getAddress()) != CL_SUCCESS) {
		
			// Display message
			cout << "Preparing program's arguments on the GPU failed" << endl;
			
			// Return false
			return false;
		}
		
		// Get SipHash keys from job's header and nonce
		uint64_t __attribute__((vector_size(sizeof(uint64_t) * SIPHASH_KEYS_SIZE))) sipHashKeysTwo;
		uint64_t heightTwo = jobHeight;
		uint64_t idTwo = jobId;
		uint64_t nonceTwo = jobNonce++;
		blake2b(sipHashKeysTwo, jobHeader, nonceTwo);
		
		// Check if setting program's part or SipHash keys arguments failed
		if(clSetKernelArg(stepOneKernel.get(), 2, sizeof(cl_uchar), &unmove(static_cast<cl_uchar>(0))) != CL_SUCCESS || clSetKernelArg(stepOneKernel.get(), 3, sizeof(sipHashKeysTwo), &sipHashKeysTwo) != CL_SUCCESS) {
		
			// Display message
			cout << "Setting program's arguments on the GPU failed" << endl;
			
			// Return false
			return false;
		}
		
		// Check if queuing running step one on the device failed
		if(clEnqueueNDRangeKernel(commandQueue.get(), stepOneKernel.get(), 1, nullptr, &totalNumberOfWorkItems[0], &workItemsPerWorkGroup[0], 0, nullptr, nullptr) != CL_SUCCESS) {
		
			// Display message
			cout << "Running program on the GPU failed" << endl;
			
			// Return false
			return false;
		}
		
		// Check if setting program's SipHash keys argument failed
		if(clSetKernelArg(stepTwoKernel.get(), 3, sizeof(sipHashKeysTwo), &sipHashKeysTwo) != CL_SUCCESS) {
		
			// Display message
			cout << "Setting program's arguments on the GPU failed" << endl;
			
			// Return false
			return false;
		}
		
		// Check if queuing running step two on the device failed
		if(clEnqueueNDRangeKernel(commandQueue.get(), stepTwoKernel.get(), 1, nullptr, &totalNumberOfWorkItems[1], &workItemsPerWorkGroup[1], 0, nullptr, nullptr) != CL_SUCCESS) {
		
			// Display message
			cout << "Running program on the GPU failed" << endl;
			
			// Return false
			return false;
		}
		
		// Check if setting program's SipHash keys argument failed
		if(clSetKernelArg(stepThreeKernel.get(), 3, sizeof(sipHashKeysTwo), &sipHashKeysTwo) != CL_SUCCESS) {
		
			// Display message
			cout << "Setting program's arguments on the GPU failed" << endl;
			
			// Return false
			return false;
		}
		
		// Go through all remaining trimming parts
		for(unsigned int i = 1; i < SLEAN_TRIMMING_PARTS; ++i) {
		
			// Check if queuing clearing number of edges per bucket one on the device failed
			if(clEnqueueFillBuffer(commandQueue.get(), numberOfEdgesPerBucketOne.get(), (const cl_ulong[]){0}, (SLEAN_TRIMMING_NUMBER_OF_BUCKETS == 1) ? sizeof(cl_uint) : sizeof(cl_ulong), 0, SLEAN_TRIMMING_NUMBER_OF_BUCKETS * sizeof(cl_uint), 0, nullptr, nullptr) != CL_SUCCESS) {
			
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
			if(clEnqueueNDRangeKernel(commandQueue.get(), stepOneKernel.get(), 1, nullptr, &totalNumberOfWorkItems[0], &workItemsPerWorkGroup[0], 0, nullptr, nullptr) != CL_SUCCESS) {
			
				// Display message
				cout << "Running program on the GPU failed" << endl;
				
				// Return false
				return false;
			}
			
			// Check if not the last trimming part
			if(i != SLEAN_TRIMMING_PARTS - 1) {
			
				// Check if queuing running step three on the device failed
				if(clEnqueueNDRangeKernel(commandQueue.get(), stepThreeKernel.get(), 1, nullptr, &totalNumberOfWorkItems[2], &workItemsPerWorkGroup[2], 0, nullptr, nullptr) != CL_SUCCESS) {
				
					// Display message
					cout << "Running program on the GPU failed" << endl;
					
					// Return false
					return false;
				}
			}
			
			// Otherwise
			else {
			
				// Check if queuing clearing number of edges per bucket two on the device failed
				if(clEnqueueFillBuffer(commandQueue.get(), numberOfEdgesPerBucketTwo.get(), (const cl_ulong[]){0}, (SLEAN_TRIMMING_NUMBER_OF_REMAINING_EDGES_BUCKETS == 1) ? sizeof(cl_uint) : sizeof(cl_ulong), 0, SLEAN_TRIMMING_NUMBER_OF_REMAINING_EDGES_BUCKETS * sizeof(cl_uint), 0, nullptr, nullptr) != CL_SUCCESS) {
				
					// Display message
					cout << "Preparing program's arguments on the GPU failed" << endl;
					
					// Return false
					return false;
				}
				
				// Check if setting program's SipHash keys argument failed
				if(clSetKernelArg(stepFourKernel.get(), 5, sizeof(sipHashKeysTwo), &sipHashKeysTwo) != CL_SUCCESS) {
				
					// Display message
					cout << "Setting program's arguments on the GPU failed" << endl;
					
					// Return false
					return false;
				}
				
				// Check if queuing running step four on the device failed
				if(clEnqueueNDRangeKernel(commandQueue.get(), stepFourKernel.get(), 1, nullptr, &totalNumberOfWorkItems[3], &workItemsPerWorkGroup[3], 0, nullptr, nullptr) != CL_SUCCESS) {
				
					// Display message
					cout << "Running program on the GPU failed" << endl;
					
					// Return false
					return false;
				}
			}
		}
		
		// Check if setting program's edges bitmap or part arguments failed
		if(clSetKernelArg(stepFiveKernel.get(), 2, sizeof(edgesBitmapTwo.get()), &unmove(edgesBitmapTwo.get())) != CL_SUCCESS || clSetKernelArg(stepFiveKernel.get(), 3, sizeof(cl_uchar), &unmove(static_cast<cl_uchar>(SLEAN_TRIMMING_PARTS - 1))) != CL_SUCCESS) {
		
			// Display message
			cout << "Setting program's arguments on the GPU failed" << endl;
			
			// Return false
			return false;
		}
		
		// Check if queuing running step five on the device failed
		if(clEnqueueNDRangeKernel(commandQueue.get(), stepFiveKernel.get(), 1, nullptr, &totalNumberOfWorkItems[4], &workItemsPerWorkGroup[4], 0, nullptr, nullptr) != CL_SUCCESS) {
		
			// Display message
			cout << "Running program on the GPU failed" << endl;
			
			// Return false
			return false;
		}
		
		// Check if setting program's SipHash keys argument failed
		if(clSetKernelArg(stepSixKernel.get(), 6, sizeof(sipHashKeysTwo), &sipHashKeysTwo) != CL_SUCCESS) {
		
			// Display message
			cout << "Setting program's arguments on the GPU failed" << endl;
			
			// Return false
			return false;
		}
		
		// Go through all remaining trimming parts except the last one
		for(unsigned int i = 0; i < SLEAN_TRIMMING_PARTS - 1; ++i) {
		
			// Check if queuing clearing number of edges per bucket one on the device failed
			if(clEnqueueFillBuffer(commandQueue.get(), numberOfEdgesPerBucketOne.get(), (const cl_ulong[]){0}, (SLEAN_TRIMMING_NUMBER_OF_BUCKETS == 1) ? sizeof(cl_uint) : sizeof(cl_ulong), 0, SLEAN_TRIMMING_NUMBER_OF_BUCKETS * sizeof(cl_uint), 0, nullptr, nullptr) != CL_SUCCESS) {
			
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
			if(clEnqueueNDRangeKernel(commandQueue.get(), stepOneKernel.get(), 1, nullptr, &totalNumberOfWorkItems[0], &workItemsPerWorkGroup[0], 0, nullptr, nullptr) != CL_SUCCESS) {
			
				// Display message
				cout << "Running program on the GPU failed" << endl;
				
				// Return false
				return false;
			}
			
			// Check if queuing clearing number of edges per bucket two on the device failed
			if(clEnqueueFillBuffer(commandQueue.get(), numberOfEdgesPerBucketTwo.get(), (const cl_ulong[]){0}, (SLEAN_TRIMMING_NUMBER_OF_REMAINING_EDGES_BUCKETS == 1) ? sizeof(cl_uint) : sizeof(cl_ulong), 0, SLEAN_TRIMMING_NUMBER_OF_REMAINING_EDGES_BUCKETS * sizeof(cl_uint), 0, nullptr, nullptr) != CL_SUCCESS) {
			
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
			if(clEnqueueNDRangeKernel(commandQueue.get(), stepSixKernel.get(), 1, nullptr, &totalNumberOfWorkItems[5], &workItemsPerWorkGroup[5], 0, nullptr, nullptr) != CL_SUCCESS) {
			
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
			if(clEnqueueNDRangeKernel(commandQueue.get(), stepFiveKernel.get(), 1, nullptr, &totalNumberOfWorkItems[4], &workItemsPerWorkGroup[4], 0, nullptr, nullptr) != CL_SUCCESS) {
			
				// Display message
				cout << "Running program on the GPU failed" << endl;
				
				// Return false
				return false;
			}
		}
		
		// Check if trimming more than one round
		if(TRIMMING_ROUNDS > 1) {
		
			// Check if queuing clearing number of edges per bucket one on the device failed
			if(clEnqueueFillBuffer(commandQueue.get(), numberOfEdgesPerBucketOne.get(), (const cl_ulong[]){0}, (SLEAN_TRIMMING_NUMBER_OF_BUCKETS == 1) ? sizeof(cl_uint) : sizeof(cl_ulong), 0, SLEAN_TRIMMING_NUMBER_OF_BUCKETS * sizeof(cl_uint), 0, nullptr, nullptr) != CL_SUCCESS) {
			
				// Display message
				cout << "Preparing program's arguments on the GPU failed" << endl;
				
				// Return false
				return false;
			}
			
			// Check if setting program's edges bitmap, part, or SipHash keys arguments failed
			if(clSetKernelArg(stepSevenKernel.get(), 0, sizeof(edgesBitmapTwo.get()), &unmove(edgesBitmapTwo.get())) != CL_SUCCESS || clSetKernelArg(stepSevenKernel.get(), 3, sizeof(cl_uchar), &unmove(static_cast<cl_uchar>(0))) != CL_SUCCESS || clSetKernelArg(stepSevenKernel.get(), 4, sizeof(sipHashKeysTwo), &sipHashKeysTwo) != CL_SUCCESS) {
			
				// Display message
				cout << "Setting program's arguments on the GPU failed" << endl;
				
				// Return false
				return false;
			}
			
			// Check if queuing running step seven on the device failed
			if(clEnqueueNDRangeKernel(commandQueue.get(), stepSevenKernel.get(), 1, nullptr, &totalNumberOfWorkItems[6], &workItemsPerWorkGroup[6], 0, nullptr, nullptr) != CL_SUCCESS) {
			
				// Display message
				cout << "Running program on the GPU failed" << endl;
				
				// Return false
				return false;
			}
			
			// Check if setting program's SipHash keys argument failed
			if(clSetKernelArg(stepEightKernel.get(), 3, sizeof(sipHashKeysTwo), &sipHashKeysTwo) != CL_SUCCESS) {
			
				// Display message
				cout << "Setting program's arguments on the GPU failed" << endl;
				
				// Return false
				return false;
			}
			
			// Check if queuing running step eight on the device failed
			if(clEnqueueNDRangeKernel(commandQueue.get(), stepEightKernel.get(), 1, nullptr, &totalNumberOfWorkItems[7], &workItemsPerWorkGroup[7], 0, nullptr, nullptr) != CL_SUCCESS) {
			
				// Display message
				cout << "Running program on the GPU failed" << endl;
				
				// Return false
				return false;
			}
			
			// Check if setting program's SipHash keys argument failed
			if(clSetKernelArg(stepNineKernel.get(), 3, sizeof(sipHashKeysTwo), &sipHashKeysTwo) != CL_SUCCESS) {
			
				// Display message
				cout << "Setting program's arguments on the GPU failed" << endl;
				
				// Return false
				return false;
			}
			
			// Go through all remaining trimming parts
			for(unsigned int i = 1; i < SLEAN_TRIMMING_PARTS; ++i) {
			
				// Check if queuing clearing number of edges per bucket one on the device failed
				if(clEnqueueFillBuffer(commandQueue.get(), numberOfEdgesPerBucketOne.get(), (const cl_ulong[]){0}, (SLEAN_TRIMMING_NUMBER_OF_BUCKETS == 1) ? sizeof(cl_uint) : sizeof(cl_ulong), 0, SLEAN_TRIMMING_NUMBER_OF_BUCKETS * sizeof(cl_uint), 0, nullptr, nullptr) != CL_SUCCESS) {
				
					// Display message
					cout << "Preparing program's arguments on the GPU failed" << endl;
					
					// Return false
					return false;
				}
				
				// Check if setting program's part argument failed
				if(clSetKernelArg(stepSevenKernel.get(), 3, sizeof(cl_uchar), &unmove(static_cast<cl_uchar>(i))) != CL_SUCCESS) {
				
					// Display message
					cout << "Setting program's arguments on the GPU failed" << endl;
					
					// Return false
					return false;
				}
				
				// Check if queuing running step seven on the device failed
				if(clEnqueueNDRangeKernel(commandQueue.get(), stepSevenKernel.get(), 1, nullptr, &totalNumberOfWorkItems[6], &workItemsPerWorkGroup[6], 0, nullptr, nullptr) != CL_SUCCESS) {
				
					// Display message
					cout << "Running program on the GPU failed" << endl;
					
					// Return false
					return false;
				}
				
				// Check if not the last trimming part
				if(i != SLEAN_TRIMMING_PARTS - 1) {
				
					// Check if queuing running step nine on the device failed
					if(clEnqueueNDRangeKernel(commandQueue.get(), stepNineKernel.get(), 1, nullptr, &totalNumberOfWorkItems[8], &workItemsPerWorkGroup[8], 0, nullptr, nullptr) != CL_SUCCESS) {
					
						// Display message
						cout << "Running program on the GPU failed" << endl;
						
						// Return false
						return false;
					}
				}
				
				// Otherwise
				else {
				
					// Check if queuing clearing number of edges per bucket two on the device failed
					if(clEnqueueFillBuffer(commandQueue.get(), numberOfEdgesPerBucketTwo.get(), (const cl_ulong[]){0}, (SLEAN_TRIMMING_NUMBER_OF_REMAINING_EDGES_BUCKETS == 1) ? sizeof(cl_uint) : sizeof(cl_ulong), 0, SLEAN_TRIMMING_NUMBER_OF_REMAINING_EDGES_BUCKETS * sizeof(cl_uint), 0, nullptr, nullptr) != CL_SUCCESS) {
					
						// Display message
						cout << "Preparing program's arguments on the GPU failed" << endl;
						
						// Return false
						return false;
					}
					
					// Check if setting program's SipHash keys argument failed
					if(clSetKernelArg(stepTenKernel.get(), 5, sizeof(sipHashKeysTwo), &sipHashKeysTwo) != CL_SUCCESS) {
					
						// Display message
						cout << "Setting program's arguments on the GPU failed" << endl;
						
						// Return false
						return false;
					}
					
					// Check if queuing running step ten on the device failed
					if(clEnqueueNDRangeKernel(commandQueue.get(), stepTenKernel.get(), 1, nullptr, &totalNumberOfWorkItems[9], &workItemsPerWorkGroup[9], 0, nullptr, nullptr) != CL_SUCCESS) {
					
						// Display message
						cout << "Running program on the GPU failed" << endl;
						
						// Return false
						return false;
					}
				}
			}
			
			// Check if setting program's edges bitmap or part arguments failed
			if(clSetKernelArg(stepElevenKernel.get(), 2, sizeof(edgesBitmapTwo.get()), &unmove(edgesBitmapTwo.get())) != CL_SUCCESS || clSetKernelArg(stepElevenKernel.get(), 3, sizeof(cl_uchar), &unmove(static_cast<cl_uchar>(SLEAN_TRIMMING_PARTS - 1))) != CL_SUCCESS) {
			
				// Display message
				cout << "Setting program's arguments on the GPU failed" << endl;
				
				// Return false
				return false;
			}
			
			// Check if queuing running step eleven on the device failed
			if(clEnqueueNDRangeKernel(commandQueue.get(), stepElevenKernel.get(), 1, nullptr, &totalNumberOfWorkItems[10], &workItemsPerWorkGroup[10], 0, nullptr, nullptr) != CL_SUCCESS) {
			
				// Display message
				cout << "Running program on the GPU failed" << endl;
				
				// Return false
				return false;
			}
			
			// Check if setting program's SipHash keys argument failed
			if(clSetKernelArg(stepTwelveKernel.get(), 6, sizeof(sipHashKeysTwo), &sipHashKeysTwo) != CL_SUCCESS) {
			
				// Display message
				cout << "Setting program's arguments on the GPU failed" << endl;
				
				// Return false
				return false;
			}
			
			// Go through all remaining trimming parts except the last one
			for(unsigned int i = 0; i < SLEAN_TRIMMING_PARTS - 1; ++i) {
			
				// Check if queuing clearing number of edges per bucket one on the device failed
				if(clEnqueueFillBuffer(commandQueue.get(), numberOfEdgesPerBucketOne.get(), (const cl_ulong[]){0}, (SLEAN_TRIMMING_NUMBER_OF_BUCKETS == 1) ? sizeof(cl_uint) : sizeof(cl_ulong), 0, SLEAN_TRIMMING_NUMBER_OF_BUCKETS * sizeof(cl_uint), 0, nullptr, nullptr) != CL_SUCCESS) {
				
					// Display message
					cout << "Preparing program's arguments on the GPU failed" << endl;
					
					// Return false
					return false;
				}
				
				// Check if setting program's part argument failed
				if(clSetKernelArg(stepSevenKernel.get(), 3, sizeof(cl_uchar), &unmove(static_cast<cl_uchar>(i))) != CL_SUCCESS) {
				
					// Display message
					cout << "Setting program's arguments on the GPU failed" << endl;
					
					// Return false
					return false;
				}
				
				// Check if queuing running step seven on the device failed
				if(clEnqueueNDRangeKernel(commandQueue.get(), stepSevenKernel.get(), 1, nullptr, &totalNumberOfWorkItems[6], &workItemsPerWorkGroup[6], 0, nullptr, nullptr) != CL_SUCCESS) {
				
					// Display message
					cout << "Running program on the GPU failed" << endl;
					
					// Return false
					return false;
				}
				
				// Check if queuing clearing number of edges per bucket two on the device failed
				if(clEnqueueFillBuffer(commandQueue.get(), numberOfEdgesPerBucketTwo.get(), (const cl_ulong[]){0}, (SLEAN_TRIMMING_NUMBER_OF_REMAINING_EDGES_BUCKETS == 1) ? sizeof(cl_uint) : sizeof(cl_ulong), 0, SLEAN_TRIMMING_NUMBER_OF_REMAINING_EDGES_BUCKETS * sizeof(cl_uint), 0, nullptr, nullptr) != CL_SUCCESS) {
				
					// Display message
					cout << "Preparing program's arguments on the GPU failed" << endl;
					
					// Return false
					return false;
				}
				
				// Check if setting program's part argument failed
				if(clSetKernelArg(stepTwelveKernel.get(), 5, sizeof(cl_uchar), &unmove(static_cast<cl_uchar>(i))) != CL_SUCCESS) {
				
					// Display message
					cout << "Setting program's arguments on the GPU failed" << endl;
					
					// Return false
					return false;
				}
				
				// Check if queuing running step twelve on the device failed
				if(clEnqueueNDRangeKernel(commandQueue.get(), stepTwelveKernel.get(), 1, nullptr, &totalNumberOfWorkItems[11], &workItemsPerWorkGroup[11], 0, nullptr, nullptr) != CL_SUCCESS) {
				
					// Display message
					cout << "Running program on the GPU failed" << endl;
					
					// Return false
					return false;
				}
				
				// Check if setting program's part argument failed
				if(clSetKernelArg(stepElevenKernel.get(), 3, sizeof(cl_uchar), &unmove(static_cast<cl_uchar>(i))) != CL_SUCCESS) {
				
					// Display message
					cout << "Setting program's arguments on the GPU failed" << endl;
					
					// Return false
					return false;
				}
				
				// Check if queuing running step eleven on the device failed
				if(clEnqueueNDRangeKernel(commandQueue.get(), stepElevenKernel.get(), 1, nullptr, &totalNumberOfWorkItems[10], &workItemsPerWorkGroup[10], 0, nullptr, nullptr) != CL_SUCCESS) {
				
					// Display message
					cout << "Running program on the GPU failed" << endl;
					
					// Return false
					return false;
				}
			}
			
			// Go through all remaining trimming rounds
			for(unsigned int i = 2; i < TRIMMING_ROUNDS; ++i) {
			
				// Check if queuing clearing number of edges per bucket one on the device failed
				if(clEnqueueFillBuffer(commandQueue.get(), numberOfEdgesPerBucketOne.get(), (const cl_ulong[]){0}, (SLEAN_TRIMMING_NUMBER_OF_BUCKETS == 1) ? sizeof(cl_uint) : sizeof(cl_ulong), 0, SLEAN_TRIMMING_NUMBER_OF_BUCKETS * sizeof(cl_uint), 0, nullptr, nullptr) != CL_SUCCESS) {
				
					// Display message
					cout << "Preparing program's arguments on the GPU failed" << endl;
					
					// Return false
					return false;
				}
				
				// Check if setting program's edges bitmap, part, nodes in second partition, or SipHash keys arguments failed
				if(clSetKernelArg(stepThirteenKernel.get(), 0, sizeof(edgesBitmapTwo.get()), &unmove(edgesBitmapTwo.get())) != CL_SUCCESS || clSetKernelArg(stepThirteenKernel.get(), 3, sizeof(cl_uchar), &unmove(static_cast<cl_uchar>(0))) != CL_SUCCESS || clSetKernelArg(stepThirteenKernel.get(), 4, sizeof(cl_uchar), &unmove(static_cast<cl_uchar>(i % 2))) != CL_SUCCESS || clSetKernelArg(stepThirteenKernel.get(), 5, sizeof(sipHashKeysTwo), &sipHashKeysTwo) != CL_SUCCESS) {
				
					// Display message
					cout << "Setting program's arguments on the GPU failed" << endl;
					
					// Return false
					return false;
				}
				
				// Check if queuing running step thirteen on the device failed
				if(clEnqueueNDRangeKernel(commandQueue.get(), stepThirteenKernel.get(), 1, nullptr, &totalNumberOfWorkItems[12], &workItemsPerWorkGroup[12], 0, nullptr, nullptr) != CL_SUCCESS) {
				
					// Display message
					cout << "Running program on the GPU failed" << endl;
					
					// Return false
					return false;
				}
				
				// Check if queuing running step fourteen on the device failed
				if(clEnqueueNDRangeKernel(commandQueue.get(), stepFourteenKernel.get(), 1, nullptr, &totalNumberOfWorkItems[13], &workItemsPerWorkGroup[13], 0, nullptr, nullptr) != CL_SUCCESS) {
				
					// Display message
					cout << "Running program on the GPU failed" << endl;
					
					// Return false
					return false;
				}
				
				// Go through all remaining trimming parts
				for(unsigned int j = 1; j < SLEAN_TRIMMING_PARTS; ++j) {
				
					// Check if queuing clearing number of edges per bucket one on the device failed
					if(clEnqueueFillBuffer(commandQueue.get(), numberOfEdgesPerBucketOne.get(), (const cl_ulong[]){0}, (SLEAN_TRIMMING_NUMBER_OF_BUCKETS == 1) ? sizeof(cl_uint) : sizeof(cl_ulong), 0, SLEAN_TRIMMING_NUMBER_OF_BUCKETS * sizeof(cl_uint), 0, nullptr, nullptr) != CL_SUCCESS) {
					
						// Display message
						cout << "Preparing program's arguments on the GPU failed" << endl;
						
						// Return false
						return false;
					}
					
					// Check if setting program's part argument failed
					if(clSetKernelArg(stepThirteenKernel.get(), 3, sizeof(cl_uchar), &unmove(static_cast<cl_uchar>(j))) != CL_SUCCESS) {
					
						// Display message
						cout << "Setting program's arguments on the GPU failed" << endl;
						
						// Return false
						return false;
					}
					
					// Check if queuing running step thirteen on the device failed
					if(clEnqueueNDRangeKernel(commandQueue.get(), stepThirteenKernel.get(), 1, nullptr, &totalNumberOfWorkItems[12], &workItemsPerWorkGroup[12], 0, nullptr, nullptr) != CL_SUCCESS) {
					
						// Display message
						cout << "Running program on the GPU failed" << endl;
						
						// Return false
						return false;
					}
					
					// Check if not the last trimming part
					if(j != SLEAN_TRIMMING_PARTS - 1) {
					
						// Check if queuing running step fifteen on the device failed
						if(clEnqueueNDRangeKernel(commandQueue.get(), stepFifteenKernel.get(), 1, nullptr, &totalNumberOfWorkItems[14], &workItemsPerWorkGroup[14], 0, nullptr, nullptr) != CL_SUCCESS) {
						
							// Display message
							cout << "Running program on the GPU failed" << endl;
							
							// Return false
							return false;
						}
					}
					
					// Otherwise
					else {
					
						// Check if queuing clearing number of edges per bucket two on the device failed
						if(clEnqueueFillBuffer(commandQueue.get(), numberOfEdgesPerBucketTwo.get(), (const cl_ulong[]){0}, (SLEAN_TRIMMING_NUMBER_OF_REMAINING_EDGES_BUCKETS == 1) ? sizeof(cl_uint) : sizeof(cl_ulong), 0, SLEAN_TRIMMING_NUMBER_OF_REMAINING_EDGES_BUCKETS * sizeof(cl_uint), 0, nullptr, nullptr) != CL_SUCCESS) {
						
							// Display message
							cout << "Preparing program's arguments on the GPU failed" << endl;
							
							// Return false
							return false;
						}
						
						// Check if queuing running step sixteen on the device failed
						if(clEnqueueNDRangeKernel(commandQueue.get(), stepSixteenKernel.get(), 1, nullptr, &totalNumberOfWorkItems[15], &workItemsPerWorkGroup[15], 0, nullptr, nullptr) != CL_SUCCESS) {
						
							// Display message
							cout << "Running program on the GPU failed" << endl;
							
							// Return false
							return false;
						}
					}
				}
				
				// Check if setting program's edges bitmap or part arguments failed
				if(clSetKernelArg(stepSeventeenKernel.get(), 2, sizeof(edgesBitmapTwo.get()), &unmove(edgesBitmapTwo.get())) != CL_SUCCESS || clSetKernelArg(stepSeventeenKernel.get(), 3, sizeof(cl_uchar), &unmove(static_cast<cl_uchar>(SLEAN_TRIMMING_PARTS - 1))) != CL_SUCCESS) {
				
					// Display message
					cout << "Setting program's arguments on the GPU failed" << endl;
					
					// Return false
					return false;
				}
				
				// Check if queuing running step seventeen on the device failed
				if(clEnqueueNDRangeKernel(commandQueue.get(), stepSeventeenKernel.get(), 1, nullptr, &totalNumberOfWorkItems[16], &workItemsPerWorkGroup[16], 0, nullptr, nullptr) != CL_SUCCESS) {
				
					// Display message
					cout << "Running program on the GPU failed" << endl;
					
					// Return false
					return false;
				}
				
				// Go through all remaining trimming parts except the last one
				for(unsigned int j = 0; j < SLEAN_TRIMMING_PARTS - 1; ++j) {
				
					// Check if queuing clearing number of edges per bucket one on the device failed
					if(clEnqueueFillBuffer(commandQueue.get(), numberOfEdgesPerBucketOne.get(), (const cl_ulong[]){0}, (SLEAN_TRIMMING_NUMBER_OF_BUCKETS == 1) ? sizeof(cl_uint) : sizeof(cl_ulong), 0, SLEAN_TRIMMING_NUMBER_OF_BUCKETS * sizeof(cl_uint), 0, nullptr, nullptr) != CL_SUCCESS) {
					
						// Display message
						cout << "Preparing program's arguments on the GPU failed" << endl;
						
						// Return false
						return false;
					}
					
					// Check if setting program's part argument failed
					if(clSetKernelArg(stepThirteenKernel.get(), 3, sizeof(cl_uchar), &unmove(static_cast<cl_uchar>(j))) != CL_SUCCESS) {
					
						// Display message
						cout << "Setting program's arguments on the GPU failed" << endl;
						
						// Return false
						return false;
					}
					
					// Check if queuing running step thirteen on the device failed
					if(clEnqueueNDRangeKernel(commandQueue.get(), stepThirteenKernel.get(), 1, nullptr, &totalNumberOfWorkItems[12], &workItemsPerWorkGroup[12], 0, nullptr, nullptr) != CL_SUCCESS) {
					
						// Display message
						cout << "Running program on the GPU failed" << endl;
						
						// Return false
						return false;
					}
					
					// Check if queuing clearing number of edges per bucket two on the device failed
					if(clEnqueueFillBuffer(commandQueue.get(), numberOfEdgesPerBucketTwo.get(), (const cl_ulong[]){0}, (SLEAN_TRIMMING_NUMBER_OF_REMAINING_EDGES_BUCKETS == 1) ? sizeof(cl_uint) : sizeof(cl_ulong), 0, SLEAN_TRIMMING_NUMBER_OF_REMAINING_EDGES_BUCKETS * sizeof(cl_uint), 0, nullptr, nullptr) != CL_SUCCESS) {
					
						// Display message
						cout << "Preparing program's arguments on the GPU failed" << endl;
						
						// Return false
						return false;
					}
					
					// Check if setting program's part argument failed
					if(clSetKernelArg(stepEighteenKernel.get(), 5, sizeof(cl_uchar), &unmove(static_cast<cl_uchar>(j))) != CL_SUCCESS) {
					
						// Display message
						cout << "Setting program's arguments on the GPU failed" << endl;
						
						// Return false
						return false;
					}
					
					// Check if queuing running step eighteen on the device failed
					if(clEnqueueNDRangeKernel(commandQueue.get(), stepEighteenKernel.get(), 1, nullptr, &totalNumberOfWorkItems[17], &workItemsPerWorkGroup[17], 0, nullptr, nullptr) != CL_SUCCESS) {
					
						// Display message
						cout << "Running program on the GPU failed" << endl;
						
						// Return false
						return false;
					}
					
					// Check if setting program's part argument failed
					if(clSetKernelArg(stepSeventeenKernel.get(), 3, sizeof(cl_uchar), &unmove(static_cast<cl_uchar>(j))) != CL_SUCCESS) {
					
						// Display message
						cout << "Setting program's arguments on the GPU failed" << endl;
						
						// Return false
						return false;
					}
					
					// Check if queuing running step seventeen on the device failed
					if(clEnqueueNDRangeKernel(commandQueue.get(), stepSeventeenKernel.get(), 1, nullptr, &totalNumberOfWorkItems[16], &workItemsPerWorkGroup[16], 0, nullptr, nullptr) != CL_SUCCESS) {
					
						// Display message
						cout << "Running program on the GPU failed" << endl;
						
						// Return false
						return false;
					}
				}
			}
		}
		
		// Check if queuing map result failed
		mapEvent.free();
		static uint64_t *resultTwo = reinterpret_cast<uint64_t *>(clEnqueueMapBuffer(commandQueue.get(), edgesBitmapTwo.get(), CL_FALSE, CL_MAP_READ, 0, NUMBER_OF_EDGES / BITS_IN_A_BYTE, 0, nullptr, mapEvent.getAddress(), nullptr));
		if(!resultTwo) {
		
			// Display message
			cout << "Getting result from the GPU failed" << endl;
			
			// Return false
			return false;
		}
		
		// Automatically unmap result two when done
		static const unique_ptr<uint64_t, void(*)(uint64_t *)> resultTwoUniquePointer(resultTwo, [](__attribute__((unused)) uint64_t *unused) noexcept {
		
			// Check if result two exists
			if(resultTwo) {
			
				// Queue unmapping result two
				clEnqueueUnmapMemObject(commandQueue.get(), edgesBitmapTwo.get(), reinterpret_cast<void *>(resultTwo), mapEvent.get() ? 1 : 0, mapEvent.getAddress(), nullptr);
			}
		});
		
		// Trimming finished
		trimmingFinished(resultOne, sipHashKeysOne, heightOne, idOne, nonceOne);
		
		// Check if queuing unmap result failed
		if(clEnqueueUnmapMemObject(commandQueue.get(), edgesBitmapOne.get(), reinterpret_cast<void *>(resultOne), 0, nullptr, nullptr) != CL_SUCCESS) {
		
			// Display message
			cout << "Getting result from the GPU failed" << endl;
			
			// Return false
			return false;
		}
		
		// Set that result one doesn't exist
		resultOne = nullptr;
		
		// While not closing
		while(!closing) {
		
			// Check if waiting for map event to finish failed
			if(clWaitForEvents(1, mapEvent.getAddress()) != CL_SUCCESS) {
			
				// Display message
				cout << "Getting result from the GPU failed" << endl;
				
				// Return false
				return false;
			}
			
			// Check if getting trimming time failed
			if(clGetEventProfilingInfo(firstCommandEvent.get(), CL_PROFILING_COMMAND_QUEUED, sizeof(startTime), &startTime, nullptr) != CL_SUCCESS || clGetEventProfilingInfo(mapEvent.get(), CL_PROFILING_COMMAND_END, sizeof(endTime), &endTime, nullptr) != CL_SUCCESS) {
			
				// Display message
				cout << "Getting GPU trimming time failed" << endl;
				
				// Return false
				return false;
			}
			
			// Display message
			cout << "\tTrimming time:\t " << static_cast<chrono::duration<double>>(static_cast<chrono::nanoseconds>(endTime - startTime)).count() << " second(s)" << endl;
			
			// Check if queuing clearing number of edges per bucket one on the device failed
			firstCommandEvent.free();
			if(clEnqueueFillBuffer(commandQueue.get(), numberOfEdgesPerBucketOne.get(), (const cl_ulong[]){0}, (SLEAN_TRIMMING_NUMBER_OF_BUCKETS == 1) ? sizeof(cl_uint) : sizeof(cl_ulong), 0, SLEAN_TRIMMING_NUMBER_OF_BUCKETS * sizeof(cl_uint), 0, nullptr, firstCommandEvent.getAddress()) != CL_SUCCESS) {
			
				// Display message
				cout << "Preparing program's arguments on the GPU failed" << endl;
				
				// Return false
				return false;
			}
			
			// Get SipHash keys from job's header and nonce
			heightOne = jobHeight;
			idOne = jobId;
			nonceOne = jobNonce++;
			blake2b(sipHashKeysOne, jobHeader, nonceOne);
			
			// Check if setting program's part or SipHash keys arguments failed
			if(clSetKernelArg(stepOneKernel.get(), 2, sizeof(cl_uchar), &unmove(static_cast<cl_uchar>(0))) != CL_SUCCESS || clSetKernelArg(stepOneKernel.get(), 3, sizeof(sipHashKeysOne), &sipHashKeysOne) != CL_SUCCESS) {
			
				// Display message
				cout << "Setting program's arguments on the GPU failed" << endl;
				
				// Return false
				return false;
			}
			
			// Check if queuing running step one on the device failed
			if(clEnqueueNDRangeKernel(commandQueue.get(), stepOneKernel.get(), 1, nullptr, &totalNumberOfWorkItems[0], &workItemsPerWorkGroup[0], 0, nullptr, nullptr) != CL_SUCCESS) {
			
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
			if(clEnqueueNDRangeKernel(commandQueue.get(), stepTwoKernel.get(), 1, nullptr, &totalNumberOfWorkItems[1], &workItemsPerWorkGroup[1], 0, nullptr, nullptr) != CL_SUCCESS) {
			
				// Display message
				cout << "Running program on the GPU failed" << endl;
				
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
				if(clEnqueueFillBuffer(commandQueue.get(), numberOfEdgesPerBucketOne.get(), (const cl_ulong[]){0}, (SLEAN_TRIMMING_NUMBER_OF_BUCKETS == 1) ? sizeof(cl_uint) : sizeof(cl_ulong), 0, SLEAN_TRIMMING_NUMBER_OF_BUCKETS * sizeof(cl_uint), 0, nullptr, nullptr) != CL_SUCCESS) {
				
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
				if(clEnqueueNDRangeKernel(commandQueue.get(), stepOneKernel.get(), 1, nullptr, &totalNumberOfWorkItems[0], &workItemsPerWorkGroup[0], 0, nullptr, nullptr) != CL_SUCCESS) {
				
					// Display message
					cout << "Running program on the GPU failed" << endl;
					
					// Return false
					return false;
				}
				
				// Check if not the last trimming part
				if(i != SLEAN_TRIMMING_PARTS - 1) {
				
					// Check if queuing running step three on the device failed
					if(clEnqueueNDRangeKernel(commandQueue.get(), stepThreeKernel.get(), 1, nullptr, &totalNumberOfWorkItems[2], &workItemsPerWorkGroup[2], 0, nullptr, nullptr) != CL_SUCCESS) {
					
						// Display message
						cout << "Running program on the GPU failed" << endl;
						
						// Return false
						return false;
					}
				}
				
				// Otherwise
				else {
				
					// Check if queuing clearing number of edges per bucket two on the device failed
					if(clEnqueueFillBuffer(commandQueue.get(), numberOfEdgesPerBucketTwo.get(), (const cl_ulong[]){0}, (SLEAN_TRIMMING_NUMBER_OF_REMAINING_EDGES_BUCKETS == 1) ? sizeof(cl_uint) : sizeof(cl_ulong), 0, SLEAN_TRIMMING_NUMBER_OF_REMAINING_EDGES_BUCKETS * sizeof(cl_uint), 0, nullptr, nullptr) != CL_SUCCESS) {
					
						// Display message
						cout << "Preparing program's arguments on the GPU failed" << endl;
						
						// Return false
						return false;
					}
					
					// Check if setting program's SipHash keys argument failed
					if(clSetKernelArg(stepFourKernel.get(), 5, sizeof(sipHashKeysOne), &sipHashKeysOne) != CL_SUCCESS) {
					
						// Display message
						cout << "Setting program's arguments on the GPU failed" << endl;
						
						// Return false
						return false;
					}
					
					// Check if queuing running step four on the device failed
					if(clEnqueueNDRangeKernel(commandQueue.get(), stepFourKernel.get(), 1, nullptr, &totalNumberOfWorkItems[3], &workItemsPerWorkGroup[3], 0, nullptr, nullptr) != CL_SUCCESS) {
					
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
			if(clEnqueueNDRangeKernel(commandQueue.get(), stepFiveKernel.get(), 1, nullptr, &totalNumberOfWorkItems[4], &workItemsPerWorkGroup[4], 0, nullptr, nullptr) != CL_SUCCESS) {
			
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
				if(clEnqueueFillBuffer(commandQueue.get(), numberOfEdgesPerBucketOne.get(), (const cl_ulong[]){0}, (SLEAN_TRIMMING_NUMBER_OF_BUCKETS == 1) ? sizeof(cl_uint) : sizeof(cl_ulong), 0, SLEAN_TRIMMING_NUMBER_OF_BUCKETS * sizeof(cl_uint), 0, nullptr, nullptr) != CL_SUCCESS) {
				
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
				if(clEnqueueNDRangeKernel(commandQueue.get(), stepOneKernel.get(), 1, nullptr, &totalNumberOfWorkItems[0], &workItemsPerWorkGroup[0], 0, nullptr, nullptr) != CL_SUCCESS) {
				
					// Display message
					cout << "Running program on the GPU failed" << endl;
					
					// Return false
					return false;
				}
				
				// Check if queuing clearing number of edges per bucket two on the device failed
				if(clEnqueueFillBuffer(commandQueue.get(), numberOfEdgesPerBucketTwo.get(), (const cl_ulong[]){0}, (SLEAN_TRIMMING_NUMBER_OF_REMAINING_EDGES_BUCKETS == 1) ? sizeof(cl_uint) : sizeof(cl_ulong), 0, SLEAN_TRIMMING_NUMBER_OF_REMAINING_EDGES_BUCKETS * sizeof(cl_uint), 0, nullptr, nullptr) != CL_SUCCESS) {
				
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
				if(clEnqueueNDRangeKernel(commandQueue.get(), stepSixKernel.get(), 1, nullptr, &totalNumberOfWorkItems[5], &workItemsPerWorkGroup[5], 0, nullptr, nullptr) != CL_SUCCESS) {
				
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
				if(clEnqueueNDRangeKernel(commandQueue.get(), stepFiveKernel.get(), 1, nullptr, &totalNumberOfWorkItems[4], &workItemsPerWorkGroup[4], 0, nullptr, nullptr) != CL_SUCCESS) {
				
					// Display message
					cout << "Running program on the GPU failed" << endl;
					
					// Return false
					return false;
				}
			}
			
			// Check if trimming more than one round
			if(TRIMMING_ROUNDS > 1) {
			
				// Check if queuing clearing number of edges per bucket one on the device failed
				if(clEnqueueFillBuffer(commandQueue.get(), numberOfEdgesPerBucketOne.get(), (const cl_ulong[]){0}, (SLEAN_TRIMMING_NUMBER_OF_BUCKETS == 1) ? sizeof(cl_uint) : sizeof(cl_ulong), 0, SLEAN_TRIMMING_NUMBER_OF_BUCKETS * sizeof(cl_uint), 0, nullptr, nullptr) != CL_SUCCESS) {
				
					// Display message
					cout << "Preparing program's arguments on the GPU failed" << endl;
					
					// Return false
					return false;
				}
				
				// Check if setting program's edges bitmap, part, or SipHash keys arguments failed
				if(clSetKernelArg(stepSevenKernel.get(), 0, sizeof(edgesBitmapOne.get()), &unmove(edgesBitmapOne.get())) != CL_SUCCESS || clSetKernelArg(stepSevenKernel.get(), 3, sizeof(cl_uchar), &unmove(static_cast<cl_uchar>(0))) != CL_SUCCESS || clSetKernelArg(stepSevenKernel.get(), 4, sizeof(sipHashKeysOne), &sipHashKeysOne) != CL_SUCCESS) {
				
					// Display message
					cout << "Setting program's arguments on the GPU failed" << endl;
					
					// Return false
					return false;
				}
				
				// Check if queuing running step seven on the device failed
				if(clEnqueueNDRangeKernel(commandQueue.get(), stepSevenKernel.get(), 1, nullptr, &totalNumberOfWorkItems[6], &workItemsPerWorkGroup[6], 0, nullptr, nullptr) != CL_SUCCESS) {
				
					// Display message
					cout << "Running program on the GPU failed" << endl;
					
					// Return false
					return false;
				}
				
				// Check if setting program's SipHash keys argument failed
				if(clSetKernelArg(stepEightKernel.get(), 3, sizeof(sipHashKeysOne), &sipHashKeysOne) != CL_SUCCESS) {
				
					// Display message
					cout << "Setting program's arguments on the GPU failed" << endl;
					
					// Return false
					return false;
				}
				
				// Check if queuing running step eight on the device failed
				if(clEnqueueNDRangeKernel(commandQueue.get(), stepEightKernel.get(), 1, nullptr, &totalNumberOfWorkItems[7], &workItemsPerWorkGroup[7], 0, nullptr, nullptr) != CL_SUCCESS) {
				
					// Display message
					cout << "Running program on the GPU failed" << endl;
					
					// Return false
					return false;
				}
				
				// Check if setting program's SipHash keys argument failed
				if(clSetKernelArg(stepNineKernel.get(), 3, sizeof(sipHashKeysOne), &sipHashKeysOne) != CL_SUCCESS) {
				
					// Display message
					cout << "Setting program's arguments on the GPU failed" << endl;
					
					// Return false
					return false;
				}
				
				// Go through all remaining trimming parts
				for(unsigned int i = 1; i < SLEAN_TRIMMING_PARTS; ++i) {
				
					// Check if queuing clearing number of edges per bucket one on the device failed
					if(clEnqueueFillBuffer(commandQueue.get(), numberOfEdgesPerBucketOne.get(), (const cl_ulong[]){0}, (SLEAN_TRIMMING_NUMBER_OF_BUCKETS == 1) ? sizeof(cl_uint) : sizeof(cl_ulong), 0, SLEAN_TRIMMING_NUMBER_OF_BUCKETS * sizeof(cl_uint), 0, nullptr, nullptr) != CL_SUCCESS) {
					
						// Display message
						cout << "Preparing program's arguments on the GPU failed" << endl;
						
						// Return false
						return false;
					}
					
					// Check if setting program's part argument failed
					if(clSetKernelArg(stepSevenKernel.get(), 3, sizeof(cl_uchar), &unmove(static_cast<cl_uchar>(i))) != CL_SUCCESS) {
					
						// Display message
						cout << "Setting program's arguments on the GPU failed" << endl;
						
						// Return false
						return false;
					}
					
					// Check if queuing running step seven on the device failed
					if(clEnqueueNDRangeKernel(commandQueue.get(), stepSevenKernel.get(), 1, nullptr, &totalNumberOfWorkItems[6], &workItemsPerWorkGroup[6], 0, nullptr, nullptr) != CL_SUCCESS) {
					
						// Display message
						cout << "Running program on the GPU failed" << endl;
						
						// Return false
						return false;
					}
					
					// Check if not the last trimming part
					if(i != SLEAN_TRIMMING_PARTS - 1) {
					
						// Check if queuing running step nine on the device failed
						if(clEnqueueNDRangeKernel(commandQueue.get(), stepNineKernel.get(), 1, nullptr, &totalNumberOfWorkItems[8], &workItemsPerWorkGroup[8], 0, nullptr, nullptr) != CL_SUCCESS) {
						
							// Display message
							cout << "Running program on the GPU failed" << endl;
							
							// Return false
							return false;
						}
					}
					
					// Otherwise
					else {
					
						// Check if queuing clearing number of edges per bucket two on the device failed
						if(clEnqueueFillBuffer(commandQueue.get(), numberOfEdgesPerBucketTwo.get(), (const cl_ulong[]){0}, (SLEAN_TRIMMING_NUMBER_OF_REMAINING_EDGES_BUCKETS == 1) ? sizeof(cl_uint) : sizeof(cl_ulong), 0, SLEAN_TRIMMING_NUMBER_OF_REMAINING_EDGES_BUCKETS * sizeof(cl_uint), 0, nullptr, nullptr) != CL_SUCCESS) {
						
							// Display message
							cout << "Preparing program's arguments on the GPU failed" << endl;
							
							// Return false
							return false;
						}
						
						// Check if setting program's SipHash keys argument failed
						if(clSetKernelArg(stepTenKernel.get(), 5, sizeof(sipHashKeysOne), &sipHashKeysOne) != CL_SUCCESS) {
						
							// Display message
							cout << "Setting program's arguments on the GPU failed" << endl;
							
							// Return false
							return false;
						}
						
						// Check if queuing running step ten on the device failed
						if(clEnqueueNDRangeKernel(commandQueue.get(), stepTenKernel.get(), 1, nullptr, &totalNumberOfWorkItems[9], &workItemsPerWorkGroup[9], 0, nullptr, nullptr) != CL_SUCCESS) {
						
							// Display message
							cout << "Running program on the GPU failed" << endl;
							
							// Return false
							return false;
						}
					}
				}
				
				// Check if setting program's edges bitmap or part arguments failed
				if(clSetKernelArg(stepElevenKernel.get(), 2, sizeof(edgesBitmapOne.get()), &unmove(edgesBitmapOne.get())) != CL_SUCCESS || clSetKernelArg(stepElevenKernel.get(), 3, sizeof(cl_uchar), &unmove(static_cast<cl_uchar>(SLEAN_TRIMMING_PARTS - 1))) != CL_SUCCESS) {
				
					// Display message
					cout << "Setting program's arguments on the GPU failed" << endl;
					
					// Return false
					return false;
				}
				
				// Check if queuing running step eleven on the device failed
				if(clEnqueueNDRangeKernel(commandQueue.get(), stepElevenKernel.get(), 1, nullptr, &totalNumberOfWorkItems[10], &workItemsPerWorkGroup[10], 0, nullptr, nullptr) != CL_SUCCESS) {
				
					// Display message
					cout << "Running program on the GPU failed" << endl;
					
					// Return false
					return false;
				}
				
				// Check if setting program's SipHash keys argument failed
				if(clSetKernelArg(stepTwelveKernel.get(), 6, sizeof(sipHashKeysOne), &sipHashKeysOne) != CL_SUCCESS) {
				
					// Display message
					cout << "Setting program's arguments on the GPU failed" << endl;
					
					// Return false
					return false;
				}
				
				// Go through all remaining trimming parts except the last one
				for(unsigned int i = 0; i < SLEAN_TRIMMING_PARTS - 1; ++i) {
				
					// Check if queuing clearing number of edges per bucket one on the device failed
					if(clEnqueueFillBuffer(commandQueue.get(), numberOfEdgesPerBucketOne.get(), (const cl_ulong[]){0}, (SLEAN_TRIMMING_NUMBER_OF_BUCKETS == 1) ? sizeof(cl_uint) : sizeof(cl_ulong), 0, SLEAN_TRIMMING_NUMBER_OF_BUCKETS * sizeof(cl_uint), 0, nullptr, nullptr) != CL_SUCCESS) {
					
						// Display message
						cout << "Preparing program's arguments on the GPU failed" << endl;
						
						// Return false
						return false;
					}
					
					// Check if setting program's part argument failed
					if(clSetKernelArg(stepSevenKernel.get(), 3, sizeof(cl_uchar), &unmove(static_cast<cl_uchar>(i))) != CL_SUCCESS) {
					
						// Display message
						cout << "Setting program's arguments on the GPU failed" << endl;
						
						// Return false
						return false;
					}
					
					// Check if queuing running step seven on the device failed
					if(clEnqueueNDRangeKernel(commandQueue.get(), stepSevenKernel.get(), 1, nullptr, &totalNumberOfWorkItems[6], &workItemsPerWorkGroup[6], 0, nullptr, nullptr) != CL_SUCCESS) {
					
						// Display message
						cout << "Running program on the GPU failed" << endl;
						
						// Return false
						return false;
					}
					
					// Check if queuing clearing number of edges per bucket two on the device failed
					if(clEnqueueFillBuffer(commandQueue.get(), numberOfEdgesPerBucketTwo.get(), (const cl_ulong[]){0}, (SLEAN_TRIMMING_NUMBER_OF_REMAINING_EDGES_BUCKETS == 1) ? sizeof(cl_uint) : sizeof(cl_ulong), 0, SLEAN_TRIMMING_NUMBER_OF_REMAINING_EDGES_BUCKETS * sizeof(cl_uint), 0, nullptr, nullptr) != CL_SUCCESS) {
					
						// Display message
						cout << "Preparing program's arguments on the GPU failed" << endl;
						
						// Return false
						return false;
					}
					
					// Check if setting program's part argument failed
					if(clSetKernelArg(stepTwelveKernel.get(), 5, sizeof(cl_uchar), &unmove(static_cast<cl_uchar>(i))) != CL_SUCCESS) {
					
						// Display message
						cout << "Setting program's arguments on the GPU failed" << endl;
						
						// Return false
						return false;
					}
					
					// Check if queuing running step twelve on the device failed
					if(clEnqueueNDRangeKernel(commandQueue.get(), stepTwelveKernel.get(), 1, nullptr, &totalNumberOfWorkItems[11], &workItemsPerWorkGroup[11], 0, nullptr, nullptr) != CL_SUCCESS) {
					
						// Display message
						cout << "Running program on the GPU failed" << endl;
						
						// Return false
						return false;
					}
					
					// Check if setting program's part argument failed
					if(clSetKernelArg(stepElevenKernel.get(), 3, sizeof(cl_uchar), &unmove(static_cast<cl_uchar>(i))) != CL_SUCCESS) {
					
						// Display message
						cout << "Setting program's arguments on the GPU failed" << endl;
						
						// Return false
						return false;
					}
					
					// Check if queuing running step eleven on the device failed
					if(clEnqueueNDRangeKernel(commandQueue.get(), stepElevenKernel.get(), 1, nullptr, &totalNumberOfWorkItems[10], &workItemsPerWorkGroup[10], 0, nullptr, nullptr) != CL_SUCCESS) {
					
						// Display message
						cout << "Running program on the GPU failed" << endl;
						
						// Return false
						return false;
					}
				}
				
				// Go through all remaining trimming rounds
				for(unsigned int i = 2; i < TRIMMING_ROUNDS; ++i) {
				
					// Check if queuing clearing number of edges per bucket one on the device failed
					if(clEnqueueFillBuffer(commandQueue.get(), numberOfEdgesPerBucketOne.get(), (const cl_ulong[]){0}, (SLEAN_TRIMMING_NUMBER_OF_BUCKETS == 1) ? sizeof(cl_uint) : sizeof(cl_ulong), 0, SLEAN_TRIMMING_NUMBER_OF_BUCKETS * sizeof(cl_uint), 0, nullptr, nullptr) != CL_SUCCESS) {
					
						// Display message
						cout << "Preparing program's arguments on the GPU failed" << endl;
						
						// Return false
						return false;
					}
					
					// Check if setting program's edges bitmap, part, nodes in second partition, or SipHash keys arguments failed
					if(clSetKernelArg(stepThirteenKernel.get(), 0, sizeof(edgesBitmapOne.get()), &unmove(edgesBitmapOne.get())) != CL_SUCCESS || clSetKernelArg(stepThirteenKernel.get(), 3, sizeof(cl_uchar), &unmove(static_cast<cl_uchar>(0))) != CL_SUCCESS || clSetKernelArg(stepThirteenKernel.get(), 4, sizeof(cl_uchar), &unmove(static_cast<cl_uchar>(i % 2))) != CL_SUCCESS || clSetKernelArg(stepThirteenKernel.get(), 5, sizeof(sipHashKeysOne), &sipHashKeysOne) != CL_SUCCESS) {
					
						// Display message
						cout << "Setting program's arguments on the GPU failed" << endl;
						
						// Return false
						return false;
					}
					
					// Check if queuing running step thirteen on the device failed
					if(clEnqueueNDRangeKernel(commandQueue.get(), stepThirteenKernel.get(), 1, nullptr, &totalNumberOfWorkItems[12], &workItemsPerWorkGroup[12], 0, nullptr, nullptr) != CL_SUCCESS) {
					
						// Display message
						cout << "Running program on the GPU failed" << endl;
						
						// Return false
						return false;
					}
					
					// Check if queuing running step fourteen on the device failed
					if(clEnqueueNDRangeKernel(commandQueue.get(), stepFourteenKernel.get(), 1, nullptr, &totalNumberOfWorkItems[13], &workItemsPerWorkGroup[13], 0, nullptr, nullptr) != CL_SUCCESS) {
					
						// Display message
						cout << "Running program on the GPU failed" << endl;
						
						// Return false
						return false;
					}
					
					// Go through all remaining trimming parts
					for(unsigned int j = 1; j < SLEAN_TRIMMING_PARTS; ++j) {
					
						// Check if queuing clearing number of edges per bucket one on the device failed
						if(clEnqueueFillBuffer(commandQueue.get(), numberOfEdgesPerBucketOne.get(), (const cl_ulong[]){0}, (SLEAN_TRIMMING_NUMBER_OF_BUCKETS == 1) ? sizeof(cl_uint) : sizeof(cl_ulong), 0, SLEAN_TRIMMING_NUMBER_OF_BUCKETS * sizeof(cl_uint), 0, nullptr, nullptr) != CL_SUCCESS) {
						
							// Display message
							cout << "Preparing program's arguments on the GPU failed" << endl;
							
							// Return false
							return false;
						}
						
						// Check if setting program's part argument failed
						if(clSetKernelArg(stepThirteenKernel.get(), 3, sizeof(cl_uchar), &unmove(static_cast<cl_uchar>(j))) != CL_SUCCESS) {
						
							// Display message
							cout << "Setting program's arguments on the GPU failed" << endl;
							
							// Return false
							return false;
						}
						
						// Check if queuing running step thirteen on the device failed
						if(clEnqueueNDRangeKernel(commandQueue.get(), stepThirteenKernel.get(), 1, nullptr, &totalNumberOfWorkItems[12], &workItemsPerWorkGroup[12], 0, nullptr, nullptr) != CL_SUCCESS) {
						
							// Display message
							cout << "Running program on the GPU failed" << endl;
							
							// Return false
							return false;
						}
						
						// Check if not the last trimming part
						if(j != SLEAN_TRIMMING_PARTS - 1) {
						
							// Check if queuing running step fifteen on the device failed
							if(clEnqueueNDRangeKernel(commandQueue.get(), stepFifteenKernel.get(), 1, nullptr, &totalNumberOfWorkItems[14], &workItemsPerWorkGroup[14], 0, nullptr, nullptr) != CL_SUCCESS) {
							
								// Display message
								cout << "Running program on the GPU failed" << endl;
								
								// Return false
								return false;
							}
						}
						
						// Otherwise
						else {
						
							// Check if queuing clearing number of edges per bucket two on the device failed
							if(clEnqueueFillBuffer(commandQueue.get(), numberOfEdgesPerBucketTwo.get(), (const cl_ulong[]){0}, (SLEAN_TRIMMING_NUMBER_OF_REMAINING_EDGES_BUCKETS == 1) ? sizeof(cl_uint) : sizeof(cl_ulong), 0, SLEAN_TRIMMING_NUMBER_OF_REMAINING_EDGES_BUCKETS * sizeof(cl_uint), 0, nullptr, nullptr) != CL_SUCCESS) {
							
								// Display message
								cout << "Preparing program's arguments on the GPU failed" << endl;
								
								// Return false
								return false;
							}
							
							// Check if queuing running step sixteen on the device failed
							if(clEnqueueNDRangeKernel(commandQueue.get(), stepSixteenKernel.get(), 1, nullptr, &totalNumberOfWorkItems[15], &workItemsPerWorkGroup[15], 0, nullptr, nullptr) != CL_SUCCESS) {
							
								// Display message
								cout << "Running program on the GPU failed" << endl;
								
								// Return false
								return false;
							}
						}
					}
					
					// Check if setting program's edges bitmap or part arguments failed
					if(clSetKernelArg(stepSeventeenKernel.get(), 2, sizeof(edgesBitmapOne.get()), &unmove(edgesBitmapOne.get())) != CL_SUCCESS || clSetKernelArg(stepSeventeenKernel.get(), 3, sizeof(cl_uchar), &unmove(static_cast<cl_uchar>(SLEAN_TRIMMING_PARTS - 1))) != CL_SUCCESS) {
					
						// Display message
						cout << "Setting program's arguments on the GPU failed" << endl;
						
						// Return false
						return false;
					}
					
					// Check if queuing running step seventeen on the device failed
					if(clEnqueueNDRangeKernel(commandQueue.get(), stepSeventeenKernel.get(), 1, nullptr, &totalNumberOfWorkItems[16], &workItemsPerWorkGroup[16], 0, nullptr, nullptr) != CL_SUCCESS) {
					
						// Display message
						cout << "Running program on the GPU failed" << endl;
						
						// Return false
						return false;
					}
					
					// Go through all remaining trimming parts except the last one
					for(unsigned int j = 0; j < SLEAN_TRIMMING_PARTS - 1; ++j) {
					
						// Check if queuing clearing number of edges per bucket one on the device failed
						if(clEnqueueFillBuffer(commandQueue.get(), numberOfEdgesPerBucketOne.get(), (const cl_ulong[]){0}, (SLEAN_TRIMMING_NUMBER_OF_BUCKETS == 1) ? sizeof(cl_uint) : sizeof(cl_ulong), 0, SLEAN_TRIMMING_NUMBER_OF_BUCKETS * sizeof(cl_uint), 0, nullptr, nullptr) != CL_SUCCESS) {
						
							// Display message
							cout << "Preparing program's arguments on the GPU failed" << endl;
							
							// Return false
							return false;
						}
						
						// Check if setting program's part argument failed
						if(clSetKernelArg(stepThirteenKernel.get(), 3, sizeof(cl_uchar), &unmove(static_cast<cl_uchar>(j))) != CL_SUCCESS) {
						
							// Display message
							cout << "Setting program's arguments on the GPU failed" << endl;
							
							// Return false
							return false;
						}
						
						// Check if queuing running step thirteen on the device failed
						if(clEnqueueNDRangeKernel(commandQueue.get(), stepThirteenKernel.get(), 1, nullptr, &totalNumberOfWorkItems[12], &workItemsPerWorkGroup[12], 0, nullptr, nullptr) != CL_SUCCESS) {
						
							// Display message
							cout << "Running program on the GPU failed" << endl;
							
							// Return false
							return false;
						}
						
						// Check if queuing clearing number of edges per bucket two on the device failed
						if(clEnqueueFillBuffer(commandQueue.get(), numberOfEdgesPerBucketTwo.get(), (const cl_ulong[]){0}, (SLEAN_TRIMMING_NUMBER_OF_REMAINING_EDGES_BUCKETS == 1) ? sizeof(cl_uint) : sizeof(cl_ulong), 0, SLEAN_TRIMMING_NUMBER_OF_REMAINING_EDGES_BUCKETS * sizeof(cl_uint), 0, nullptr, nullptr) != CL_SUCCESS) {
						
							// Display message
							cout << "Preparing program's arguments on the GPU failed" << endl;
							
							// Return false
							return false;
						}
						
						// Check if setting program's part argument failed
						if(clSetKernelArg(stepEighteenKernel.get(), 5, sizeof(cl_uchar), &unmove(static_cast<cl_uchar>(j))) != CL_SUCCESS) {
						
							// Display message
							cout << "Setting program's arguments on the GPU failed" << endl;
							
							// Return false
							return false;
						}
						
						// Check if queuing running step eighteen on the device failed
						if(clEnqueueNDRangeKernel(commandQueue.get(), stepEighteenKernel.get(), 1, nullptr, &totalNumberOfWorkItems[17], &workItemsPerWorkGroup[17], 0, nullptr, nullptr) != CL_SUCCESS) {
						
							// Display message
							cout << "Running program on the GPU failed" << endl;
							
							// Return false
							return false;
						}
						
						// Check if setting program's part argument failed
						if(clSetKernelArg(stepSeventeenKernel.get(), 3, sizeof(cl_uchar), &unmove(static_cast<cl_uchar>(j))) != CL_SUCCESS) {
						
							// Display message
							cout << "Setting program's arguments on the GPU failed" << endl;
							
							// Return false
							return false;
						}
						
						// Check if queuing running step seventeen on the device failed
						if(clEnqueueNDRangeKernel(commandQueue.get(), stepSeventeenKernel.get(), 1, nullptr, &totalNumberOfWorkItems[16], &workItemsPerWorkGroup[16], 0, nullptr, nullptr) != CL_SUCCESS) {
						
							// Display message
							cout << "Running program on the GPU failed" << endl;
							
							// Return false
							return false;
						}
					}
				}
			}
			
			// Check if queuing map result failed
			mapEvent.free();
			resultOne = reinterpret_cast<uint64_t *>(clEnqueueMapBuffer(commandQueue.get(), edgesBitmapOne.get(), CL_FALSE, CL_MAP_READ, 0, NUMBER_OF_EDGES / BITS_IN_A_BYTE, 0, nullptr, mapEvent.getAddress(), nullptr));
			if(!resultOne) {
			
				// Display message
				cout << "Getting result from the GPU failed" << endl;
				
				// Return false
				return false;
			}
			
			// Trimming finished
			trimmingFinished(resultTwo, sipHashKeysTwo, heightTwo, idTwo, nonceTwo);
			
			// Check if queuing unmap result failed
			if(clEnqueueUnmapMemObject(commandQueue.get(), edgesBitmapTwo.get(), reinterpret_cast<void *>(resultTwo), 0, nullptr, nullptr) != CL_SUCCESS) {
			
				// Display message
				cout << "Getting result from the GPU failed" << endl;
				
				// Return false
				return false;
			}
			
			// Set that result two doesn't exist
			resultTwo = nullptr;
			
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
			if(clGetEventProfilingInfo(firstCommandEvent.get(), CL_PROFILING_COMMAND_QUEUED, sizeof(startTime), &startTime, nullptr) != CL_SUCCESS || clGetEventProfilingInfo(mapEvent.get(), CL_PROFILING_COMMAND_END, sizeof(endTime), &endTime, nullptr) != CL_SUCCESS) {
			
				// Display message
				cout << "Getting GPU trimming time failed" << endl;
				
				// Return false
				return false;
			}
			
			// Display message
			cout << "\tTrimming time:\t " << static_cast<chrono::duration<double>>(static_cast<chrono::nanoseconds>(endTime - startTime)).count() << " second(s)" << endl;
			
			// Check if queuing clearing number of edges per bucket one on the device failed
			firstCommandEvent.free();
			if(clEnqueueFillBuffer(commandQueue.get(), numberOfEdgesPerBucketOne.get(), (const cl_ulong[]){0}, (SLEAN_TRIMMING_NUMBER_OF_BUCKETS == 1) ? sizeof(cl_uint) : sizeof(cl_ulong), 0, SLEAN_TRIMMING_NUMBER_OF_BUCKETS * sizeof(cl_uint), 0, nullptr, firstCommandEvent.getAddress()) != CL_SUCCESS) {
			
				// Display message
				cout << "Preparing program's arguments on the GPU failed" << endl;
				
				// Return false
				return false;
			}
			
			// Get SipHash keys from job's header and nonce
			heightTwo = jobHeight;
			idTwo = jobId;
			nonceTwo = jobNonce++;
			blake2b(sipHashKeysTwo, jobHeader, nonceTwo);
			
			// Check if setting program's part or SipHash keys arguments failed
			if(clSetKernelArg(stepOneKernel.get(), 2, sizeof(cl_uchar), &unmove(static_cast<cl_uchar>(0))) != CL_SUCCESS || clSetKernelArg(stepOneKernel.get(), 3, sizeof(sipHashKeysTwo), &sipHashKeysTwo) != CL_SUCCESS) {
			
				// Display message
				cout << "Setting program's arguments on the GPU failed" << endl;
				
				// Return false
				return false;
			}
			
			// Check if queuing running step one on the device failed
			if(clEnqueueNDRangeKernel(commandQueue.get(), stepOneKernel.get(), 1, nullptr, &totalNumberOfWorkItems[0], &workItemsPerWorkGroup[0], 0, nullptr, nullptr) != CL_SUCCESS) {
			
				// Display message
				cout << "Running program on the GPU failed" << endl;
				
				// Return false
				return false;
			}
			
			// Check if setting program's SipHash keys argument failed
			if(clSetKernelArg(stepTwoKernel.get(), 3, sizeof(sipHashKeysTwo), &sipHashKeysTwo) != CL_SUCCESS) {
			
				// Display message
				cout << "Setting program's arguments on the GPU failed" << endl;
				
				// Return false
				return false;
			}
			
			// Check if queuing running step two on the device failed
			if(clEnqueueNDRangeKernel(commandQueue.get(), stepTwoKernel.get(), 1, nullptr, &totalNumberOfWorkItems[1], &workItemsPerWorkGroup[1], 0, nullptr, nullptr) != CL_SUCCESS) {
			
				// Display message
				cout << "Running program on the GPU failed" << endl;
				
				// Return false
				return false;
			}
			
			// Check if setting program's SipHash keys argument failed
			if(clSetKernelArg(stepThreeKernel.get(), 3, sizeof(sipHashKeysTwo), &sipHashKeysTwo) != CL_SUCCESS) {
			
				// Display message
				cout << "Setting program's arguments on the GPU failed" << endl;
				
				// Return false
				return false;
			}
			
			// Go through all remaining trimming parts
			for(unsigned int i = 1; i < SLEAN_TRIMMING_PARTS; ++i) {
			
				// Check if queuing clearing number of edges per bucket one on the device failed
				if(clEnqueueFillBuffer(commandQueue.get(), numberOfEdgesPerBucketOne.get(), (const cl_ulong[]){0}, (SLEAN_TRIMMING_NUMBER_OF_BUCKETS == 1) ? sizeof(cl_uint) : sizeof(cl_ulong), 0, SLEAN_TRIMMING_NUMBER_OF_BUCKETS * sizeof(cl_uint), 0, nullptr, nullptr) != CL_SUCCESS) {
				
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
				if(clEnqueueNDRangeKernel(commandQueue.get(), stepOneKernel.get(), 1, nullptr, &totalNumberOfWorkItems[0], &workItemsPerWorkGroup[0], 0, nullptr, nullptr) != CL_SUCCESS) {
				
					// Display message
					cout << "Running program on the GPU failed" << endl;
					
					// Return false
					return false;
				}
				
				// Check if not the last trimming part
				if(i != SLEAN_TRIMMING_PARTS - 1) {
				
					// Check if queuing running step three on the device failed
					if(clEnqueueNDRangeKernel(commandQueue.get(), stepThreeKernel.get(), 1, nullptr, &totalNumberOfWorkItems[2], &workItemsPerWorkGroup[2], 0, nullptr, nullptr) != CL_SUCCESS) {
					
						// Display message
						cout << "Running program on the GPU failed" << endl;
						
						// Return false
						return false;
					}
				}
				
				// Otherwise
				else {
				
					// Check if queuing clearing number of edges per bucket two on the device failed
					if(clEnqueueFillBuffer(commandQueue.get(), numberOfEdgesPerBucketTwo.get(), (const cl_ulong[]){0}, (SLEAN_TRIMMING_NUMBER_OF_REMAINING_EDGES_BUCKETS == 1) ? sizeof(cl_uint) : sizeof(cl_ulong), 0, SLEAN_TRIMMING_NUMBER_OF_REMAINING_EDGES_BUCKETS * sizeof(cl_uint), 0, nullptr, nullptr) != CL_SUCCESS) {
					
						// Display message
						cout << "Preparing program's arguments on the GPU failed" << endl;
						
						// Return false
						return false;
					}
					
					// Check if setting program's SipHash keys argument failed
					if(clSetKernelArg(stepFourKernel.get(), 5, sizeof(sipHashKeysTwo), &sipHashKeysTwo) != CL_SUCCESS) {
					
						// Display message
						cout << "Setting program's arguments on the GPU failed" << endl;
						
						// Return false
						return false;
					}
					
					// Check if queuing running step four on the device failed
					if(clEnqueueNDRangeKernel(commandQueue.get(), stepFourKernel.get(), 1, nullptr, &totalNumberOfWorkItems[3], &workItemsPerWorkGroup[3], 0, nullptr, nullptr) != CL_SUCCESS) {
					
						// Display message
						cout << "Running program on the GPU failed" << endl;
						
						// Return false
						return false;
					}
				}
			}
			
			// Check if setting program's edges bitmap or part arguments failed
			if(clSetKernelArg(stepFiveKernel.get(), 2, sizeof(edgesBitmapTwo.get()), &unmove(edgesBitmapTwo.get())) != CL_SUCCESS || clSetKernelArg(stepFiveKernel.get(), 3, sizeof(cl_uchar), &unmove(static_cast<cl_uchar>(SLEAN_TRIMMING_PARTS - 1))) != CL_SUCCESS) {
			
				// Display message
				cout << "Setting program's arguments on the GPU failed" << endl;
				
				// Return false
				return false;
			}
			
			// Check if queuing running step five on the device failed
			if(clEnqueueNDRangeKernel(commandQueue.get(), stepFiveKernel.get(), 1, nullptr, &totalNumberOfWorkItems[4], &workItemsPerWorkGroup[4], 0, nullptr, nullptr) != CL_SUCCESS) {
			
				// Display message
				cout << "Running program on the GPU failed" << endl;
				
				// Return false
				return false;
			}
			
			// Check if setting program's SipHash keys argument failed
			if(clSetKernelArg(stepSixKernel.get(), 6, sizeof(sipHashKeysTwo), &sipHashKeysTwo) != CL_SUCCESS) {
			
				// Display message
				cout << "Setting program's arguments on the GPU failed" << endl;
				
				// Return false
				return false;
			}
			
			// Go through all remaining trimming parts except the last one
			for(unsigned int i = 0; i < SLEAN_TRIMMING_PARTS - 1; ++i) {
			
				// Check if queuing clearing number of edges per bucket one on the device failed
				if(clEnqueueFillBuffer(commandQueue.get(), numberOfEdgesPerBucketOne.get(), (const cl_ulong[]){0}, (SLEAN_TRIMMING_NUMBER_OF_BUCKETS == 1) ? sizeof(cl_uint) : sizeof(cl_ulong), 0, SLEAN_TRIMMING_NUMBER_OF_BUCKETS * sizeof(cl_uint), 0, nullptr, nullptr) != CL_SUCCESS) {
				
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
				if(clEnqueueNDRangeKernel(commandQueue.get(), stepOneKernel.get(), 1, nullptr, &totalNumberOfWorkItems[0], &workItemsPerWorkGroup[0], 0, nullptr, nullptr) != CL_SUCCESS) {
				
					// Display message
					cout << "Running program on the GPU failed" << endl;
					
					// Return false
					return false;
				}
				
				// Check if queuing clearing number of edges per bucket two on the device failed
				if(clEnqueueFillBuffer(commandQueue.get(), numberOfEdgesPerBucketTwo.get(), (const cl_ulong[]){0}, (SLEAN_TRIMMING_NUMBER_OF_REMAINING_EDGES_BUCKETS == 1) ? sizeof(cl_uint) : sizeof(cl_ulong), 0, SLEAN_TRIMMING_NUMBER_OF_REMAINING_EDGES_BUCKETS * sizeof(cl_uint), 0, nullptr, nullptr) != CL_SUCCESS) {
				
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
				if(clEnqueueNDRangeKernel(commandQueue.get(), stepSixKernel.get(), 1, nullptr, &totalNumberOfWorkItems[5], &workItemsPerWorkGroup[5], 0, nullptr, nullptr) != CL_SUCCESS) {
				
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
				if(clEnqueueNDRangeKernel(commandQueue.get(), stepFiveKernel.get(), 1, nullptr, &totalNumberOfWorkItems[4], &workItemsPerWorkGroup[4], 0, nullptr, nullptr) != CL_SUCCESS) {
				
					// Display message
					cout << "Running program on the GPU failed" << endl;
					
					// Return false
					return false;
				}
			}
			
			// Check if trimming more than one round
			if(TRIMMING_ROUNDS > 1) {
			
				// Check if queuing clearing number of edges per bucket one on the device failed
				if(clEnqueueFillBuffer(commandQueue.get(), numberOfEdgesPerBucketOne.get(), (const cl_ulong[]){0}, (SLEAN_TRIMMING_NUMBER_OF_BUCKETS == 1) ? sizeof(cl_uint) : sizeof(cl_ulong), 0, SLEAN_TRIMMING_NUMBER_OF_BUCKETS * sizeof(cl_uint), 0, nullptr, nullptr) != CL_SUCCESS) {
				
					// Display message
					cout << "Preparing program's arguments on the GPU failed" << endl;
					
					// Return false
					return false;
				}
				
				// Check if setting program's edges bitmap, part, or SipHash keys arguments failed
				if(clSetKernelArg(stepSevenKernel.get(), 0, sizeof(edgesBitmapTwo.get()), &unmove(edgesBitmapTwo.get())) != CL_SUCCESS || clSetKernelArg(stepSevenKernel.get(), 3, sizeof(cl_uchar), &unmove(static_cast<cl_uchar>(0))) != CL_SUCCESS || clSetKernelArg(stepSevenKernel.get(), 4, sizeof(sipHashKeysTwo), &sipHashKeysTwo) != CL_SUCCESS) {
				
					// Display message
					cout << "Setting program's arguments on the GPU failed" << endl;
					
					// Return false
					return false;
				}
				
				// Check if queuing running step seven on the device failed
				if(clEnqueueNDRangeKernel(commandQueue.get(), stepSevenKernel.get(), 1, nullptr, &totalNumberOfWorkItems[6], &workItemsPerWorkGroup[6], 0, nullptr, nullptr) != CL_SUCCESS) {
				
					// Display message
					cout << "Running program on the GPU failed" << endl;
					
					// Return false
					return false;
				}
				
				// Check if setting program's SipHash keys argument failed
				if(clSetKernelArg(stepEightKernel.get(), 3, sizeof(sipHashKeysTwo), &sipHashKeysTwo) != CL_SUCCESS) {
				
					// Display message
					cout << "Setting program's arguments on the GPU failed" << endl;
					
					// Return false
					return false;
				}
				
				// Check if queuing running step eight on the device failed
				if(clEnqueueNDRangeKernel(commandQueue.get(), stepEightKernel.get(), 1, nullptr, &totalNumberOfWorkItems[7], &workItemsPerWorkGroup[7], 0, nullptr, nullptr) != CL_SUCCESS) {
				
					// Display message
					cout << "Running program on the GPU failed" << endl;
					
					// Return false
					return false;
				}
				
				// Check if setting program's SipHash keys argument failed
				if(clSetKernelArg(stepNineKernel.get(), 3, sizeof(sipHashKeysTwo), &sipHashKeysTwo) != CL_SUCCESS) {
				
					// Display message
					cout << "Setting program's arguments on the GPU failed" << endl;
					
					// Return false
					return false;
				}
				
				// Go through all remaining trimming parts
				for(unsigned int i = 1; i < SLEAN_TRIMMING_PARTS; ++i) {
				
					// Check if queuing clearing number of edges per bucket one on the device failed
					if(clEnqueueFillBuffer(commandQueue.get(), numberOfEdgesPerBucketOne.get(), (const cl_ulong[]){0}, (SLEAN_TRIMMING_NUMBER_OF_BUCKETS == 1) ? sizeof(cl_uint) : sizeof(cl_ulong), 0, SLEAN_TRIMMING_NUMBER_OF_BUCKETS * sizeof(cl_uint), 0, nullptr, nullptr) != CL_SUCCESS) {
					
						// Display message
						cout << "Preparing program's arguments on the GPU failed" << endl;
						
						// Return false
						return false;
					}
					
					// Check if setting program's part argument failed
					if(clSetKernelArg(stepSevenKernel.get(), 3, sizeof(cl_uchar), &unmove(static_cast<cl_uchar>(i))) != CL_SUCCESS) {
					
						// Display message
						cout << "Setting program's arguments on the GPU failed" << endl;
						
						// Return false
						return false;
					}
					
					// Check if queuing running step seven on the device failed
					if(clEnqueueNDRangeKernel(commandQueue.get(), stepSevenKernel.get(), 1, nullptr, &totalNumberOfWorkItems[6], &workItemsPerWorkGroup[6], 0, nullptr, nullptr) != CL_SUCCESS) {
					
						// Display message
						cout << "Running program on the GPU failed" << endl;
						
						// Return false
						return false;
					}
					
					// Check if not the last trimming part
					if(i != SLEAN_TRIMMING_PARTS - 1) {
					
						// Check if queuing running step nine on the device failed
						if(clEnqueueNDRangeKernel(commandQueue.get(), stepNineKernel.get(), 1, nullptr, &totalNumberOfWorkItems[8], &workItemsPerWorkGroup[8], 0, nullptr, nullptr) != CL_SUCCESS) {
						
							// Display message
							cout << "Running program on the GPU failed" << endl;
							
							// Return false
							return false;
						}
					}
					
					// Otherwise
					else {
					
						// Check if queuing clearing number of edges per bucket two on the device failed
						if(clEnqueueFillBuffer(commandQueue.get(), numberOfEdgesPerBucketTwo.get(), (const cl_ulong[]){0}, (SLEAN_TRIMMING_NUMBER_OF_REMAINING_EDGES_BUCKETS == 1) ? sizeof(cl_uint) : sizeof(cl_ulong), 0, SLEAN_TRIMMING_NUMBER_OF_REMAINING_EDGES_BUCKETS * sizeof(cl_uint), 0, nullptr, nullptr) != CL_SUCCESS) {
						
							// Display message
							cout << "Preparing program's arguments on the GPU failed" << endl;
							
							// Return false
							return false;
						}
						
						// Check if setting program's SipHash keys argument failed
						if(clSetKernelArg(stepTenKernel.get(), 5, sizeof(sipHashKeysTwo), &sipHashKeysTwo) != CL_SUCCESS) {
						
							// Display message
							cout << "Setting program's arguments on the GPU failed" << endl;
							
							// Return false
							return false;
						}
						
						// Check if queuing running step ten on the device failed
						if(clEnqueueNDRangeKernel(commandQueue.get(), stepTenKernel.get(), 1, nullptr, &totalNumberOfWorkItems[9], &workItemsPerWorkGroup[9], 0, nullptr, nullptr) != CL_SUCCESS) {
						
							// Display message
							cout << "Running program on the GPU failed" << endl;
							
							// Return false
							return false;
						}
					}
				}
				
				// Check if setting program's edges bitmap or part arguments failed
				if(clSetKernelArg(stepElevenKernel.get(), 2, sizeof(edgesBitmapTwo.get()), &unmove(edgesBitmapTwo.get())) != CL_SUCCESS || clSetKernelArg(stepElevenKernel.get(), 3, sizeof(cl_uchar), &unmove(static_cast<cl_uchar>(SLEAN_TRIMMING_PARTS - 1))) != CL_SUCCESS) {
				
					// Display message
					cout << "Setting program's arguments on the GPU failed" << endl;
					
					// Return false
					return false;
				}
				
				// Check if queuing running step eleven on the device failed
				if(clEnqueueNDRangeKernel(commandQueue.get(), stepElevenKernel.get(), 1, nullptr, &totalNumberOfWorkItems[10], &workItemsPerWorkGroup[10], 0, nullptr, nullptr) != CL_SUCCESS) {
				
					// Display message
					cout << "Running program on the GPU failed" << endl;
					
					// Return false
					return false;
				}
				
				// Check if setting program's SipHash keys argument failed
				if(clSetKernelArg(stepTwelveKernel.get(), 6, sizeof(sipHashKeysTwo), &sipHashKeysTwo) != CL_SUCCESS) {
				
					// Display message
					cout << "Setting program's arguments on the GPU failed" << endl;
					
					// Return false
					return false;
				}
				
				// Go through all remaining trimming parts except the last one
				for(unsigned int i = 0; i < SLEAN_TRIMMING_PARTS - 1; ++i) {
				
					// Check if queuing clearing number of edges per bucket one on the device failed
					if(clEnqueueFillBuffer(commandQueue.get(), numberOfEdgesPerBucketOne.get(), (const cl_ulong[]){0}, (SLEAN_TRIMMING_NUMBER_OF_BUCKETS == 1) ? sizeof(cl_uint) : sizeof(cl_ulong), 0, SLEAN_TRIMMING_NUMBER_OF_BUCKETS * sizeof(cl_uint), 0, nullptr, nullptr) != CL_SUCCESS) {
					
						// Display message
						cout << "Preparing program's arguments on the GPU failed" << endl;
						
						// Return false
						return false;
					}
					
					// Check if setting program's part argument failed
					if(clSetKernelArg(stepSevenKernel.get(), 3, sizeof(cl_uchar), &unmove(static_cast<cl_uchar>(i))) != CL_SUCCESS) {
					
						// Display message
						cout << "Setting program's arguments on the GPU failed" << endl;
						
						// Return false
						return false;
					}
					
					// Check if queuing running step seven on the device failed
					if(clEnqueueNDRangeKernel(commandQueue.get(), stepSevenKernel.get(), 1, nullptr, &totalNumberOfWorkItems[6], &workItemsPerWorkGroup[6], 0, nullptr, nullptr) != CL_SUCCESS) {
					
						// Display message
						cout << "Running program on the GPU failed" << endl;
						
						// Return false
						return false;
					}
					
					// Check if queuing clearing number of edges per bucket two on the device failed
					if(clEnqueueFillBuffer(commandQueue.get(), numberOfEdgesPerBucketTwo.get(), (const cl_ulong[]){0}, (SLEAN_TRIMMING_NUMBER_OF_REMAINING_EDGES_BUCKETS == 1) ? sizeof(cl_uint) : sizeof(cl_ulong), 0, SLEAN_TRIMMING_NUMBER_OF_REMAINING_EDGES_BUCKETS * sizeof(cl_uint), 0, nullptr, nullptr) != CL_SUCCESS) {
					
						// Display message
						cout << "Preparing program's arguments on the GPU failed" << endl;
						
						// Return false
						return false;
					}
					
					// Check if setting program's part argument failed
					if(clSetKernelArg(stepTwelveKernel.get(), 5, sizeof(cl_uchar), &unmove(static_cast<cl_uchar>(i))) != CL_SUCCESS) {
					
						// Display message
						cout << "Setting program's arguments on the GPU failed" << endl;
						
						// Return false
						return false;
					}
					
					// Check if queuing running step twelve on the device failed
					if(clEnqueueNDRangeKernel(commandQueue.get(), stepTwelveKernel.get(), 1, nullptr, &totalNumberOfWorkItems[11], &workItemsPerWorkGroup[11], 0, nullptr, nullptr) != CL_SUCCESS) {
					
						// Display message
						cout << "Running program on the GPU failed" << endl;
						
						// Return false
						return false;
					}
					
					// Check if setting program's part argument failed
					if(clSetKernelArg(stepElevenKernel.get(), 3, sizeof(cl_uchar), &unmove(static_cast<cl_uchar>(i))) != CL_SUCCESS) {
					
						// Display message
						cout << "Setting program's arguments on the GPU failed" << endl;
						
						// Return false
						return false;
					}
					
					// Check if queuing running step eleven on the device failed
					if(clEnqueueNDRangeKernel(commandQueue.get(), stepElevenKernel.get(), 1, nullptr, &totalNumberOfWorkItems[10], &workItemsPerWorkGroup[10], 0, nullptr, nullptr) != CL_SUCCESS) {
					
						// Display message
						cout << "Running program on the GPU failed" << endl;
						
						// Return false
						return false;
					}
				}
				
				// Go through all remaining trimming rounds
				for(unsigned int i = 2; i < TRIMMING_ROUNDS; ++i) {
				
					// Check if queuing clearing number of edges per bucket one on the device failed
					if(clEnqueueFillBuffer(commandQueue.get(), numberOfEdgesPerBucketOne.get(), (const cl_ulong[]){0}, (SLEAN_TRIMMING_NUMBER_OF_BUCKETS == 1) ? sizeof(cl_uint) : sizeof(cl_ulong), 0, SLEAN_TRIMMING_NUMBER_OF_BUCKETS * sizeof(cl_uint), 0, nullptr, nullptr) != CL_SUCCESS) {
					
						// Display message
						cout << "Preparing program's arguments on the GPU failed" << endl;
						
						// Return false
						return false;
					}
					
					// Check if setting program's edges bitmap, part, nodes in second partition, or SipHash keys arguments failed
					if(clSetKernelArg(stepThirteenKernel.get(), 0, sizeof(edgesBitmapTwo.get()), &unmove(edgesBitmapTwo.get())) != CL_SUCCESS || clSetKernelArg(stepThirteenKernel.get(), 3, sizeof(cl_uchar), &unmove(static_cast<cl_uchar>(0))) != CL_SUCCESS || clSetKernelArg(stepThirteenKernel.get(), 4, sizeof(cl_uchar), &unmove(static_cast<cl_uchar>(i % 2))) != CL_SUCCESS || clSetKernelArg(stepThirteenKernel.get(), 5, sizeof(sipHashKeysTwo), &sipHashKeysTwo) != CL_SUCCESS) {
					
						// Display message
						cout << "Setting program's arguments on the GPU failed" << endl;
						
						// Return false
						return false;
					}
					
					// Check if queuing running step thirteen on the device failed
					if(clEnqueueNDRangeKernel(commandQueue.get(), stepThirteenKernel.get(), 1, nullptr, &totalNumberOfWorkItems[12], &workItemsPerWorkGroup[12], 0, nullptr, nullptr) != CL_SUCCESS) {
					
						// Display message
						cout << "Running program on the GPU failed" << endl;
						
						// Return false
						return false;
					}
					
					// Check if queuing running step fourteen on the device failed
					if(clEnqueueNDRangeKernel(commandQueue.get(), stepFourteenKernel.get(), 1, nullptr, &totalNumberOfWorkItems[13], &workItemsPerWorkGroup[13], 0, nullptr, nullptr) != CL_SUCCESS) {
					
						// Display message
						cout << "Running program on the GPU failed" << endl;
						
						// Return false
						return false;
					}
					
					// Go through all remaining trimming parts
					for(unsigned int j = 1; j < SLEAN_TRIMMING_PARTS; ++j) {
					
						// Check if queuing clearing number of edges per bucket one on the device failed
						if(clEnqueueFillBuffer(commandQueue.get(), numberOfEdgesPerBucketOne.get(), (const cl_ulong[]){0}, (SLEAN_TRIMMING_NUMBER_OF_BUCKETS == 1) ? sizeof(cl_uint) : sizeof(cl_ulong), 0, SLEAN_TRIMMING_NUMBER_OF_BUCKETS * sizeof(cl_uint), 0, nullptr, nullptr) != CL_SUCCESS) {
						
							// Display message
							cout << "Preparing program's arguments on the GPU failed" << endl;
							
							// Return false
							return false;
						}
						
						// Check if setting program's part argument failed
						if(clSetKernelArg(stepThirteenKernel.get(), 3, sizeof(cl_uchar), &unmove(static_cast<cl_uchar>(j))) != CL_SUCCESS) {
						
							// Display message
							cout << "Setting program's arguments on the GPU failed" << endl;
							
							// Return false
							return false;
						}
						
						// Check if queuing running step thirteen on the device failed
						if(clEnqueueNDRangeKernel(commandQueue.get(), stepThirteenKernel.get(), 1, nullptr, &totalNumberOfWorkItems[12], &workItemsPerWorkGroup[12], 0, nullptr, nullptr) != CL_SUCCESS) {
						
							// Display message
							cout << "Running program on the GPU failed" << endl;
							
							// Return false
							return false;
						}
						
						// Check if not the last trimming part
						if(j != SLEAN_TRIMMING_PARTS - 1) {
						
							// Check if queuing running step fifteen on the device failed
							if(clEnqueueNDRangeKernel(commandQueue.get(), stepFifteenKernel.get(), 1, nullptr, &totalNumberOfWorkItems[14], &workItemsPerWorkGroup[14], 0, nullptr, nullptr) != CL_SUCCESS) {
							
								// Display message
								cout << "Running program on the GPU failed" << endl;
								
								// Return false
								return false;
							}
						}
						
						// Otherwise
						else {
						
							// Check if queuing clearing number of edges per bucket two on the device failed
							if(clEnqueueFillBuffer(commandQueue.get(), numberOfEdgesPerBucketTwo.get(), (const cl_ulong[]){0}, (SLEAN_TRIMMING_NUMBER_OF_REMAINING_EDGES_BUCKETS == 1) ? sizeof(cl_uint) : sizeof(cl_ulong), 0, SLEAN_TRIMMING_NUMBER_OF_REMAINING_EDGES_BUCKETS * sizeof(cl_uint), 0, nullptr, nullptr) != CL_SUCCESS) {
							
								// Display message
								cout << "Preparing program's arguments on the GPU failed" << endl;
								
								// Return false
								return false;
							}
							
							// Check if queuing running step sixteen on the device failed
							if(clEnqueueNDRangeKernel(commandQueue.get(), stepSixteenKernel.get(), 1, nullptr, &totalNumberOfWorkItems[15], &workItemsPerWorkGroup[15], 0, nullptr, nullptr) != CL_SUCCESS) {
							
								// Display message
								cout << "Running program on the GPU failed" << endl;
								
								// Return false
								return false;
							}
						}
					}
					
					// Check if setting program's edges bitmap or part arguments failed
					if(clSetKernelArg(stepSeventeenKernel.get(), 2, sizeof(edgesBitmapTwo.get()), &unmove(edgesBitmapTwo.get())) != CL_SUCCESS || clSetKernelArg(stepSeventeenKernel.get(), 3, sizeof(cl_uchar), &unmove(static_cast<cl_uchar>(SLEAN_TRIMMING_PARTS - 1))) != CL_SUCCESS) {
					
						// Display message
						cout << "Setting program's arguments on the GPU failed" << endl;
						
						// Return false
						return false;
					}
					
					// Check if queuing running step seventeen on the device failed
					if(clEnqueueNDRangeKernel(commandQueue.get(), stepSeventeenKernel.get(), 1, nullptr, &totalNumberOfWorkItems[16], &workItemsPerWorkGroup[16], 0, nullptr, nullptr) != CL_SUCCESS) {
					
						// Display message
						cout << "Running program on the GPU failed" << endl;
						
						// Return false
						return false;
					}
					
					// Go through all remaining trimming parts except the last one
					for(unsigned int j = 0; j < SLEAN_TRIMMING_PARTS - 1; ++j) {
					
						// Check if queuing clearing number of edges per bucket one on the device failed
						if(clEnqueueFillBuffer(commandQueue.get(), numberOfEdgesPerBucketOne.get(), (const cl_ulong[]){0}, (SLEAN_TRIMMING_NUMBER_OF_BUCKETS == 1) ? sizeof(cl_uint) : sizeof(cl_ulong), 0, SLEAN_TRIMMING_NUMBER_OF_BUCKETS * sizeof(cl_uint), 0, nullptr, nullptr) != CL_SUCCESS) {
						
							// Display message
							cout << "Preparing program's arguments on the GPU failed" << endl;
							
							// Return false
							return false;
						}
						
						// Check if setting program's part argument failed
						if(clSetKernelArg(stepThirteenKernel.get(), 3, sizeof(cl_uchar), &unmove(static_cast<cl_uchar>(j))) != CL_SUCCESS) {
						
							// Display message
							cout << "Setting program's arguments on the GPU failed" << endl;
							
							// Return false
							return false;
						}
						
						// Check if queuing running step thirteen on the device failed
						if(clEnqueueNDRangeKernel(commandQueue.get(), stepThirteenKernel.get(), 1, nullptr, &totalNumberOfWorkItems[12], &workItemsPerWorkGroup[12], 0, nullptr, nullptr) != CL_SUCCESS) {
						
							// Display message
							cout << "Running program on the GPU failed" << endl;
							
							// Return false
							return false;
						}
						
						// Check if queuing clearing number of edges per bucket two on the device failed
						if(clEnqueueFillBuffer(commandQueue.get(), numberOfEdgesPerBucketTwo.get(), (const cl_ulong[]){0}, (SLEAN_TRIMMING_NUMBER_OF_REMAINING_EDGES_BUCKETS == 1) ? sizeof(cl_uint) : sizeof(cl_ulong), 0, SLEAN_TRIMMING_NUMBER_OF_REMAINING_EDGES_BUCKETS * sizeof(cl_uint), 0, nullptr, nullptr) != CL_SUCCESS) {
						
							// Display message
							cout << "Preparing program's arguments on the GPU failed" << endl;
							
							// Return false
							return false;
						}
						
						// Check if setting program's part argument failed
						if(clSetKernelArg(stepEighteenKernel.get(), 5, sizeof(cl_uchar), &unmove(static_cast<cl_uchar>(j))) != CL_SUCCESS) {
						
							// Display message
							cout << "Setting program's arguments on the GPU failed" << endl;
							
							// Return false
							return false;
						}
						
						// Check if queuing running step eighteen on the device failed
						if(clEnqueueNDRangeKernel(commandQueue.get(), stepEighteenKernel.get(), 1, nullptr, &totalNumberOfWorkItems[17], &workItemsPerWorkGroup[17], 0, nullptr, nullptr) != CL_SUCCESS) {
						
							// Display message
							cout << "Running program on the GPU failed" << endl;
							
							// Return false
							return false;
						}
						
						// Check if setting program's part argument failed
						if(clSetKernelArg(stepSeventeenKernel.get(), 3, sizeof(cl_uchar), &unmove(static_cast<cl_uchar>(j))) != CL_SUCCESS) {
						
							// Display message
							cout << "Setting program's arguments on the GPU failed" << endl;
							
							// Return false
							return false;
						}
						
						// Check if queuing running step seventeen on the device failed
						if(clEnqueueNDRangeKernel(commandQueue.get(), stepSeventeenKernel.get(), 1, nullptr, &totalNumberOfWorkItems[16], &workItemsPerWorkGroup[16], 0, nullptr, nullptr) != CL_SUCCESS) {
						
							// Display message
							cout << "Running program on the GPU failed" << endl;
							
							// Return false
							return false;
						}
					}
				}
			}
			
			// Check if queuing map result failed
			mapEvent.free();
			resultTwo = reinterpret_cast<uint64_t *>(clEnqueueMapBuffer(commandQueue.get(), edgesBitmapTwo.get(), CL_FALSE, CL_MAP_READ, 0, NUMBER_OF_EDGES / BITS_IN_A_BYTE, 0, nullptr, mapEvent.getAddress(), nullptr));
			if(!resultTwo) {
			
				// Display message
				cout << "Getting result from the GPU failed" << endl;
				
				// Return false
				return false;
			}
			
			// Trimming finished
			trimmingFinished(resultOne, sipHashKeysOne, heightOne, idOne, nonceOne);
			
			// Check if queuing unmap result failed
			if(clEnqueueUnmapMemObject(commandQueue.get(), edgesBitmapOne.get(), reinterpret_cast<void *>(resultOne), 0, nullptr, nullptr) != CL_SUCCESS) {
			
				// Display message
				cout << "Getting result from the GPU failed" << endl;
				
				// Return false
				return false;
			}
			
			// Set that result one doesn't exist
			resultOne = nullptr;
		}
		
		// Return true
		return true;
	}
#endif


#endif

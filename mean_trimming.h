// Header guard
#ifndef MEAN_TRIMMING_H
#define MEAN_TRIMMING_H


// Header files
using namespace std;


// Configurable constants

// Mean trimming number of edges per step one work item
#define MEAN_TRIMMING_NUMBER_OF_EDGES_PER_STEP_ONE_WORK_ITEM min(static_cast<uint64_t>(1024), NUMBER_OF_EDGES)

// Mean trimming work items per work group for step one
#define MEAN_TRIMMING_WORK_ITEMS_PER_WORK_GROUP_STEP_ONE 512


// Constants

// Mean trimming number of bitmap bytes
#define MEAN_TRIMMING_NUMBER_OF_BITMAP_BYTES min(static_cast<uint64_t>(LOCAL_RAM_KILOBYTES * BYTES_IN_A_KILOBYTE), NUMBER_OF_EDGES / BITS_IN_A_BYTE)

// Mean trimming number of buckets
#define MEAN_TRIMMING_NUMBER_OF_BUCKETS ((NUMBER_OF_EDGES / BITS_IN_A_BYTE) / MEAN_TRIMMING_NUMBER_OF_BITMAP_BYTES)

// Mean trimming number of least significant bits ignored during bucket sorting
#define MEAN_TRIMMING_NUMBER_OF_LEAST_SIGNIFICANT_BITS_IGNORED_DURING_BUCKET_SORTING (EDGE_BITS - bit_width(MEAN_TRIMMING_NUMBER_OF_BUCKETS) + 1)

// Mean trimming number of items per bucket
#define MEAN_TRIMMING_NUMBER_OF_ITEMS_PER_BUCKET (NUMBER_OF_EDGES / MEAN_TRIMMING_NUMBER_OF_BUCKETS)

// Mean trimming initial max number of edges per bucket (Divide by 4 and multiply by 4 makes the result a product of 4)
#define MEAN_TRIMMING_INITIAL_MAX_NUMBER_OF_EDGES_PER_BUCKET ((static_cast<uint32_t>(MEAN_TRIMMING_NUMBER_OF_ITEMS_PER_BUCKET * 1.05) / 4) * 4)

// Mean trimming after trimming round max number of edges per bucket (Divide by 4 and multiply by 4 makes the result a product of 4)
#define MEAN_TRIMMING_AFTER_TRIMMING_ROUND_MAX_NUMBER_OF_EDGES_PER_BUCKET ((static_cast<uint32_t>(MEAN_TRIMMING_NUMBER_OF_ITEMS_PER_BUCKET * 0.73) / 4) * 4)

// Check if using an Apple device and not using OpenCL
#if defined __APPLE__ && !defined USE_OPENCL

	// Mean trimming required RAM bytes
	#define MEAN_TRIMMING_REQUIRED_RAM_BYTES (static_cast<uint64_t>(MEAN_TRIMMING_INITIAL_MAX_NUMBER_OF_EDGES_PER_BUCKET) * MEAN_TRIMMING_NUMBER_OF_BUCKETS * sizeof(uint32_t) + static_cast<uint64_t>(MEAN_TRIMMING_AFTER_TRIMMING_ROUND_MAX_NUMBER_OF_EDGES_PER_BUCKET) * MEAN_TRIMMING_NUMBER_OF_BUCKETS * sizeof(uint32_t) + MEAN_TRIMMING_NUMBER_OF_BUCKETS * sizeof(uint32_t) * 2 + (sizeof(uint32_t) + static_cast<uint64_t>(MAX_NUMBER_OF_EDGES_AFTER_TRIMMING) * EDGE_NUMBER_OF_COMPONENTS * sizeof(uint32_t)) * 2)
	
// Otherwise
#else

	// Mean trimming required RAM bytes
	#define MEAN_TRIMMING_REQUIRED_RAM_BYTES (static_cast<uint64_t>(MEAN_TRIMMING_INITIAL_MAX_NUMBER_OF_EDGES_PER_BUCKET) * MEAN_TRIMMING_NUMBER_OF_BUCKETS * sizeof(cl_uint) + static_cast<uint64_t>(MEAN_TRIMMING_AFTER_TRIMMING_ROUND_MAX_NUMBER_OF_EDGES_PER_BUCKET) * MEAN_TRIMMING_NUMBER_OF_BUCKETS * sizeof(cl_uint) + MEAN_TRIMMING_NUMBER_OF_BUCKETS * sizeof(cl_uint) * 2 + (sizeof(cl_uint) + static_cast<uint64_t>(MAX_NUMBER_OF_EDGES_AFTER_TRIMMING) * EDGE_NUMBER_OF_COMPONENTS * sizeof(cl_uint)) * 2)
#endif

// Mean trimming required work group RAM bytes
#define MEAN_TRIMMING_REQUIRED_WORK_GROUP_RAM_BYTES MEAN_TRIMMING_NUMBER_OF_BITMAP_BYTES

// Mean trimming max local buckets size
#define MEAN_TRIMMING_MAX_LOCAL_BUCKETS_SIZE min(static_cast<uint64_t>(4), (MEAN_TRIMMING_NUMBER_OF_BUCKETS + 256 - 1) / 256)


// Function prototypes

// Check if using an Apple device and not using OpenCL
#if defined __APPLE__ && !defined USE_OPENCL

	// Create mean trimming context
	static inline MTL::Device *createMeanTrimmingContext(const unsigned int deviceIndex) noexcept;
	
	// Perform mean trimming loop
	static inline bool performMeanTrimmingLoop(MTL::Device *device) noexcept;
	
// Otherwise
#else

	// Create mean trimming context
	static inline cl_context createMeanTrimmingContext(const cl_platform_id platforms[], const cl_uint numberOfPlatforms, const unsigned int deviceIndex) noexcept;
	
	// Perform mean trimming loop
	static inline bool performMeanTrimmingLoop(const cl_context context) noexcept;
#endif


// Supporting function implementation

// Check if using an Apple device and not using OpenCL
#if defined __APPLE__ && !defined USE_OPENCL

	// Create mean trimming context
	MTL::Device *createMeanTrimmingContext(const unsigned int deviceIndex) noexcept {
	
		// Set index to zero
		unsigned int index = 0;
		
		// Check if getting all devices failed
		unique_ptr<NS::Array, void(*)(NS::Array *)> devices(MTL::CopyAllDevices(), [](NS::Array *devices) noexcept {
		
			// Free devices
			devices->release();
		});
		if(!devices) {
		
			// Set devices to include just the default device
			devices = unique_ptr<NS::Array, void(*)(NS::Array *)>(NS::Array::alloc()->init((const NS::Object *[]){
			
				// Default device
				MTL::CreateSystemDefaultDevice()
				
			}, 1), [](NS::Array *devices) noexcept {
			
				// Free devices
				devices->release();
			});
		}
		
		// Check if getting devices was successful
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
								if(device->recommendedMaxWorkingSetSize() >= MEAN_TRIMMING_REQUIRED_RAM_BYTES && device->maxThreadgroupMemoryLength() >= MEAN_TRIMMING_REQUIRED_WORK_GROUP_RAM_BYTES) {
								
									// Check if device index is specified
									if(deviceIndex != ALL_DEVICES) {
									
										// Display message
										cout << "Using the GPU for mean trimming." << endl;
									}
									
									// Otherwise
									else {
									
										// Display message
										cout << "Using " << utf8String << " for mean trimming." << endl;
									}
									
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

	// Create mean trimming context
	cl_context createMeanTrimmingContext(const cl_platform_id platforms[], const cl_uint numberOfPlatforms, const unsigned int deviceIndex) noexcept {
	
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
											if(clGetDeviceInfo(devices[j], CL_DEVICE_GLOBAL_MEM_SIZE, sizeof(memorySize), &memorySize, nullptr) == CL_SUCCESS && clGetDeviceInfo(devices[j], CL_DEVICE_LOCAL_MEM_SIZE, sizeof(workGroupMemorySize), &workGroupMemorySize, nullptr) == CL_SUCCESS && memorySize >= MEAN_TRIMMING_REQUIRED_RAM_BYTES && workGroupMemorySize >= MEAN_TRIMMING_REQUIRED_WORK_GROUP_RAM_BYTES) {
											
												// Check if creating context for the device was successful
												cl_context context = clCreateContext(unmove((const cl_context_properties[]){CL_CONTEXT_PLATFORM, reinterpret_cast<cl_context_properties>(platforms[i]), 0}), 1, &devices[j], nullptr, nullptr, nullptr);
												if(context) {
												
													// Check if device index is specified
													if(deviceIndex != ALL_DEVICES) {
													
														// Display message
														cout << "Using the GPU for mean trimming." << endl;
													}
													
													// Otherwise
													else {
													
														// Display message
														cout << "Using " << name << " for mean trimming." << endl;
													}
													
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

// Check if using an Apple device and not using OpenCL
#if defined __APPLE__ && !defined USE_OPENCL

	// Perform mean trimming loop
	bool performMeanTrimmingLoop(MTL::Device *device) noexcept {
	
		// Check if device's work group memory isn't fully utilized
		if(bit_floor(device->maxThreadgroupMemoryLength()) / BYTES_IN_A_KILOBYTE > LOCAL_RAM_KILOBYTES) {
		
			// Display message
			cout << "GPU's local RAM won't be fully utilized. Build this program with LOCAL_RAM_KILOBYTES=" << (bit_floor(device->maxThreadgroupMemoryLength()) / BYTES_IN_A_KILOBYTE) << " for potentially better performance." << endl;
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
		const size_t bucketsOneNumberOfBuckets = (enforceMaxMemoryAllocationSize && static_cast<uint64_t>(MEAN_TRIMMING_INITIAL_MAX_NUMBER_OF_EDGES_PER_BUCKET) * MEAN_TRIMMING_NUMBER_OF_BUCKETS * sizeof(uint32_t) > device->maxBufferLength()) ? (MEAN_TRIMMING_AFTER_TRIMMING_ROUND_MAX_NUMBER_OF_EDGES_PER_BUCKET * MEAN_TRIMMING_NUMBER_OF_BUCKETS + MEAN_TRIMMING_INITIAL_MAX_NUMBER_OF_EDGES_PER_BUCKET - 1) / MEAN_TRIMMING_INITIAL_MAX_NUMBER_OF_EDGES_PER_BUCKET : MEAN_TRIMMING_NUMBER_OF_BUCKETS;
		
		// Set local buckets size based on the device's work group memory size
		const unsigned int localBucketsSize = min(bit_floor((device->maxThreadgroupMemoryLength() - (MEAN_TRIMMING_NUMBER_OF_BUCKETS + sizeof(uint32_t) - 1)) / (sizeof(uint32_t) * MEAN_TRIMMING_NUMBER_OF_BUCKETS) + 1), MEAN_TRIMMING_MAX_LOCAL_BUCKETS_SIZE);
		
		// Check if creating preprocessor macros failed
		const unique_ptr<NS::Dictionary, void(*)(NS::Dictionary *)> preprocessorMacros(NS::Dictionary::alloc()->init((const NS::Object *[]){
		
			// Edge bits value
			MTLSTR(TO_STRING(EDGE_BITS)),
			
			// Trimming rounds value
			MTLSTR(TO_STRING(TRIMMING_ROUNDS)),
			
			// Edge number of components value
			MTLSTR(TO_STRING(EDGE_NUMBER_OF_COMPONENTS)),
			
			// Number of edges per step one work item value
			unique_ptr<NS::Number, void(*)(NS::Number *)>(NS::Number::alloc()->init(MEAN_TRIMMING_NUMBER_OF_EDGES_PER_STEP_ONE_WORK_ITEM), [](NS::Number *numberOfEdgesPerStepOneWorkItemValue) noexcept {
			
				// Free number of edges per step one work item value
				numberOfEdgesPerStepOneWorkItemValue->release();
				
			}).get(),
			
			// Number of bitmap bytes value
			unique_ptr<NS::Number, void(*)(NS::Number *)>(NS::Number::alloc()->init(MEAN_TRIMMING_NUMBER_OF_BITMAP_BYTES), [](NS::Number *numberOfBitmapBytesValue) noexcept {
			
				// Free number of bitmap bytes value
				numberOfBitmapBytesValue->release();
				
			}).get(),
			
			// Number of buckets value
			unique_ptr<NS::Number, void(*)(NS::Number *)>(NS::Number::alloc()->init(MEAN_TRIMMING_NUMBER_OF_BUCKETS), [](NS::Number *numberOfBucketsValue) noexcept {
			
				// Free number of buckets value
				numberOfBucketsValue->release();
				
			}).get(),
			
			// Max number of edges after trimming value
			unique_ptr<NS::Number, void(*)(NS::Number *)>(NS::Number::alloc()->init(MAX_NUMBER_OF_EDGES_AFTER_TRIMMING), [](NS::Number *maxNumberOfEdgesAfterTrimmingValue) noexcept {
			
				// Free max number of edges after trimming value
				maxNumberOfEdgesAfterTrimmingValue->release();
				
			}).get(),
			
			// Number of least significant bits ignored during bucket sorting value
			unique_ptr<NS::Number, void(*)(NS::Number *)>(NS::Number::alloc()->init(MEAN_TRIMMING_NUMBER_OF_LEAST_SIGNIFICANT_BITS_IGNORED_DURING_BUCKET_SORTING), [](NS::Number *numberOfLeastSignificantBitsIgnoredDuringBucketSortingValue) noexcept {
			
				// Free number of least significant bits ignored during bucket sorting value
				numberOfLeastSignificantBitsIgnoredDuringBucketSortingValue->release();
				
			}).get(),
			
			// Initial max number of edges per bucket value
			unique_ptr<NS::Number, void(*)(NS::Number *)>(NS::Number::alloc()->init(MEAN_TRIMMING_INITIAL_MAX_NUMBER_OF_EDGES_PER_BUCKET), [](NS::Number *initialMaxNumberOfEdgesPerBucketValue) noexcept {
			
				// Free initial max number of edges per bucket value
				initialMaxNumberOfEdgesPerBucketValue->release();
				
			}).get(),
			
			// After trimming round max number of edges per bucket value
			unique_ptr<NS::Number, void(*)(NS::Number *)>(NS::Number::alloc()->init(MEAN_TRIMMING_AFTER_TRIMMING_ROUND_MAX_NUMBER_OF_EDGES_PER_BUCKET), [](NS::Number *afterTrimmingRoundMaxNumberOfEdgesPerBucketValue) noexcept {
			
				// Free after trimming round max number of edges per bucket value
				afterTrimmingRoundMaxNumberOfEdgesPerBucketValue->release();
				
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
			
			// Trimming rounds key
			MTLSTR("TRIMMING_ROUNDS"),
			
			// Edge number of components key
			MTLSTR("EDGE_NUMBER_OF_COMPONENTS"),
			
			// Number of edges per step one work item key
			MTLSTR("NUMBER_OF_EDGES_PER_STEP_ONE_WORK_ITEM"),
			
			// Number of bitmap bytes key
			MTLSTR("NUMBER_OF_BITMAP_BYTES"),
			
			// Number of buckets key
			MTLSTR("NUMBER_OF_BUCKETS"),
			
			// Max number of edges after trimming key
			MTLSTR("MAX_NUMBER_OF_EDGES_AFTER_TRIMMING"),
			
			// Number of least significant bits ignored during bucket sorting key
			MTLSTR("NUMBER_OF_LEAST_SIGNIFICANT_BITS_IGNORED_DURING_BUCKET_SORTING"),
			
			// Initial max number of edges per bucket key
			MTLSTR("INITIAL_MAX_NUMBER_OF_EDGES_PER_BUCKET"),
			
			// After trimming round max number of edges per bucket key
			MTLSTR("AFTER_TRIMMING_ROUND_MAX_NUMBER_OF_EDGES_PER_BUCKET"),
			
			// Initial buckets number of buckets key
			MTLSTR("INITIAL_BUCKETS_NUMBER_OF_BUCKETS"),
			
			// Local buckets size key
			MTLSTR("LOCAL_BUCKETS_SIZE")
		
		}, 12), [](NS::Dictionary *preprocessorMacros) noexcept {
		
			// Free preprocessor macros
			preprocessorMacros->release();
		});
		if(!preprocessorMacros) {
		
			// Display message
			cout << "Creating preprocessor macros for the kernels failed." << endl;
			
			// Return false
			return false;
		}
		
		// Check if creating compile options for the kernels failed
		const unique_ptr<MTL::CompileOptions, void(*)(MTL::CompileOptions *)> compileOptions(MTL::CompileOptions::alloc()->init(), [](MTL::CompileOptions *compileOptions) noexcept {
		
			// Free compile options
			compileOptions->release();
		});
		if(!compileOptions) {
		
			// Display message
			cout << "Creating compile options for the kernels failed." << endl;
			
			// Return false
			return false;
		}
		
		// Configure compiler options
		compileOptions->setPreprocessorMacros(preprocessorMacros.get());
		compileOptions->setLanguageVersion(METAL_TARGET_VERSION);
		
		// Check if creating library for the device failed
		const NS::String *source = (
			#include "./mean_trimming.metal"
		);
		NS::Error *createLibraryError;
		const unique_ptr<MTL::Library, void(*)(MTL::Library *)> library(device->newLibrary(source, compileOptions.get(), &createLibraryError), [](MTL::Library *library) noexcept {
		
			// Free library
			library->release();
		});
		if(!library) {
		
			// Display message
			cout << "Creating library for the GPU failed." << endl;
			
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
		const unique_ptr<MTL::Function, void(*)(MTL::Function *)> stepOneKernel(library->newFunction(MTLSTR("trimEdgesStepOne")), [](MTL::Function *stepOneKernel) noexcept {
		
			// Free step one kernel
			stepOneKernel->release();
		});
		const unique_ptr<MTL::Function, void(*)(MTL::Function *)> stepTwoKernel(library->newFunction(MTLSTR("trimEdgesStepTwo")), [](MTL::Function *stepTwoKernel) noexcept {
		
			// Free step two kernel
			stepTwoKernel->release();
		});
		const unique_ptr<MTL::Function, void(*)(MTL::Function *)> stepThreeKernel(library->newFunction(MTLSTR("trimEdgesStepThree")), [](MTL::Function *stepThreeKernel) noexcept {
		
			// Free step three kernel
			stepThreeKernel->release();
		});
		const unique_ptr<MTL::Function, void(*)(MTL::Function *)> stepFourKernel(library->newFunction(MTLSTR("trimEdgesStepFour")), [](MTL::Function *stepFourKernel) noexcept {
		
			// Free step four kernel
			stepFourKernel->release();
		});
		const unique_ptr<MTL::Function, void(*)(MTL::Function *)> stepFiveKernel(library->newFunction(MTLSTR("trimEdgesStepFive")), [](MTL::Function *stepFiveKernel) noexcept {
		
			// Free step five kernel
			stepFiveKernel->release();
		});
		const unique_ptr<MTL::Function, void(*)(MTL::Function *)> stepSixKernel(library->newFunction(MTLSTR("trimEdgesStepSix")), [](MTL::Function *stepSixKernel) noexcept {
		
			// Free step six kernel
			stepSixKernel->release();
		});
		const unique_ptr<MTL::Function, void(*)(MTL::Function *)> clearNumberOfEdgesPerBucketKernel(library->newFunction(MTLSTR("clearNumberOfEdgesPerBucket")), [](MTL::Function *clearNumberOfEdgesPerBucketKernel) noexcept {
		
			// Free clear number of edges per bucket kernel
			clearNumberOfEdgesPerBucketKernel->release();
		});
		if(!stepOneKernel || !stepTwoKernel || !stepThreeKernel || !stepFourKernel || !stepFiveKernel || !stepSixKernel || !clearNumberOfEdgesPerBucketKernel) {
		
			// Display message
			cout << "Getting kernels from the library failed." << endl;
			
			// Return false
			return false;
		}
		
		// Check if creating pipelines for the device failed
		NS::Error *createPipelineOneError;
		unique_ptr<MTL::ComputePipelineState, void(*)(MTL::ComputePipelineState *)> stepOnePipeline(device->newComputePipelineState(stepOneKernel.get(), &createPipelineOneError), [](MTL::ComputePipelineState *stepOnePipeline) noexcept {
		
			// Free step one pipeline
			stepOnePipeline->release();
		});
		NS::Error *createPipelineTwoError;
		unique_ptr<MTL::ComputePipelineState, void(*)(MTL::ComputePipelineState *)> stepTwoPipeline(device->newComputePipelineState(stepTwoKernel.get(), &createPipelineTwoError), [](MTL::ComputePipelineState *stepTwoPipeline) noexcept {
		
			// Free step two pipeline
			stepTwoPipeline->release();
		});
		NS::Error *createPipelineThreeError;
		unique_ptr<MTL::ComputePipelineState, void(*)(MTL::ComputePipelineState *)> stepThreePipeline(device->newComputePipelineState(stepThreeKernel.get(), &createPipelineThreeError), [](MTL::ComputePipelineState *stepThreePipeline) noexcept {
		
			// Free step three pipeline
			stepThreePipeline->release();
		});
		NS::Error *createPipelineFourError;
		unique_ptr<MTL::ComputePipelineState, void(*)(MTL::ComputePipelineState *)> stepFourPipeline(device->newComputePipelineState(stepFourKernel.get(), &createPipelineFourError), [](MTL::ComputePipelineState *stepFourPipeline) noexcept {
		
			// Free step four pipeline
			stepFourPipeline->release();
		});
		NS::Error *createPipelineFiveError;
		unique_ptr<MTL::ComputePipelineState, void(*)(MTL::ComputePipelineState *)> stepFivePipeline(device->newComputePipelineState(stepFiveKernel.get(), &createPipelineFiveError), [](MTL::ComputePipelineState *stepFivePipeline) noexcept {
		
			// Free step five pipeline
			stepFivePipeline->release();
		});
		NS::Error *createPipelineSixError;
		unique_ptr<MTL::ComputePipelineState, void(*)(MTL::ComputePipelineState *)> stepSixPipeline(device->newComputePipelineState(stepSixKernel.get(), &createPipelineSixError), [](MTL::ComputePipelineState *stepSixPipeline) noexcept {
		
			// Free step six pipeline
			stepSixPipeline->release();
		});
		NS::Error *createPipelineClearNumberOfEdgesPerBucketError;
		unique_ptr<MTL::ComputePipelineState, void(*)(MTL::ComputePipelineState *)> clearNumberOfEdgesPerBucketPipeline(device->newComputePipelineState(clearNumberOfEdgesPerBucketKernel.get(), &createPipelineClearNumberOfEdgesPerBucketError), [](MTL::ComputePipelineState *clearNumberOfEdgesPerBucketPipeline) noexcept {
		
			// Free clear number of edges per bucket pipeline
			clearNumberOfEdgesPerBucketPipeline->release();
		});
		if(!stepOnePipeline || !stepTwoPipeline || !stepThreePipeline || !stepFourPipeline || !stepFivePipeline || !stepSixPipeline || !clearNumberOfEdgesPerBucketPipeline) {
		
			// Display message
			cout << "Creating pipelines for the GPU failed." << endl;
			
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
			
			// Check if creating pipeline clear number of edges per bucket failed and an error exists
			if(!clearNumberOfEdgesPerBucketPipeline && createPipelineClearNumberOfEdgesPerBucketError) {
			
				// Check if error's localized description exists
				const NS::String *localizedDescription = createPipelineClearNumberOfEdgesPerBucketError->localizedDescription();
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
			{NUMBER_OF_EDGES / MEAN_TRIMMING_NUMBER_OF_EDGES_PER_STEP_ONE_WORK_ITEM, 1, 1},
			
			// Trim edges step two kernel
			{MEAN_TRIMMING_NUMBER_OF_BUCKETS * min(bit_floor(device->maxThreadsPerThreadgroup().width), bit_floor(static_cast<NS::UInteger>(INT16_MAX / 2))), 1, 1},
			
			// Trim edges step three kernel
			{MEAN_TRIMMING_NUMBER_OF_BUCKETS * min(bit_floor(device->maxThreadsPerThreadgroup().width), bit_floor(static_cast<NS::UInteger>(INT16_MAX / 2))), 1, 1},
			
			// Trim edges step four kernel
			{MEAN_TRIMMING_NUMBER_OF_BUCKETS * min(bit_floor(device->maxThreadsPerThreadgroup().width), bit_floor(static_cast<NS::UInteger>(INT16_MAX / 2))), 1, 1},
			
			// Trim edges step five kernel
			{MEAN_TRIMMING_NUMBER_OF_BUCKETS * min(bit_floor(device->maxThreadsPerThreadgroup().width), bit_floor(static_cast<NS::UInteger>(INT16_MAX / 2))), 1, 1},
			
			// Trim edges step six kernel
			{MEAN_TRIMMING_NUMBER_OF_BUCKETS * min(bit_floor(device->maxThreadsPerThreadgroup().width), static_cast<NS::UInteger>(UINT16_MAX)), 1, 1},
			
			// Clear number of edges per bucket kernel
			{(MEAN_TRIMMING_NUMBER_OF_BUCKETS * sizeof(uint32_t) + sizeof(uint64_t) - 1) / sizeof(uint64_t), 1, 1}
		};
		
		// Set work items per work group based on the total number of work items and max work group size
		const MTL::Size workItemsPerWorkGroup[] = {
		
			// Trim edges step one kernel
			{min(min(bit_floor(min(static_cast<NS::UInteger>(MEAN_TRIMMING_WORK_ITEMS_PER_WORK_GROUP_STEP_ONE), device->maxThreadsPerThreadgroup().width)), bit_floor(static_cast<NS::UInteger>(INT16_MAX / 2))), totalNumberOfWorkItems[0].width), 1, 1},
			
			// Trim edges step two kernel
			{min(bit_floor(device->maxThreadsPerThreadgroup().width), bit_floor(static_cast<NS::UInteger>(INT16_MAX / 2))), 1, 1},
			
			// Trim edges step three kernel
			{min(bit_floor(device->maxThreadsPerThreadgroup().width), bit_floor(static_cast<NS::UInteger>(INT16_MAX / 2))), 1, 1},
			
			// Trim edges step four kernel
			{min(bit_floor(device->maxThreadsPerThreadgroup().width), bit_floor(static_cast<NS::UInteger>(INT16_MAX / 2))), 1, 1},
			
			// Trim edges step five kernel
			{min(bit_floor(device->maxThreadsPerThreadgroup().width), bit_floor(static_cast<NS::UInteger>(INT16_MAX / 2))), 1, 1},
			
			// Trim edges step six kernel
			{min(bit_floor(device->maxThreadsPerThreadgroup().width), static_cast<NS::UInteger>(UINT16_MAX)), 1, 1},
			
			// Clear number of edges per bucket kernel
			{min(bit_floor(device->maxThreadsPerThreadgroup().width), totalNumberOfWorkItems[6].width), 1, 1}
		};
		
		// Check if creating pipeline descriptors failed
		const unique_ptr<MTL::ComputePipelineDescriptor, void(*)(MTL::ComputePipelineDescriptor *)> stepOnePipelineDescriptor(MTL::ComputePipelineDescriptor::alloc()->init(), [](MTL::ComputePipelineDescriptor *stepOnePipelineDescriptor) noexcept {
		
			// Free step one pipeline descriptor
			stepOnePipelineDescriptor->release();
		});
		const unique_ptr<MTL::ComputePipelineDescriptor, void(*)(MTL::ComputePipelineDescriptor *)> stepTwoPipelineDescriptor(MTL::ComputePipelineDescriptor::alloc()->init(), [](MTL::ComputePipelineDescriptor *stepTwoPipelineDescriptor) noexcept {
		
			// Free step two pipeline descriptor
			stepTwoPipelineDescriptor->release();
		});
		const unique_ptr<MTL::ComputePipelineDescriptor, void(*)(MTL::ComputePipelineDescriptor *)> stepThreePipelineDescriptor(MTL::ComputePipelineDescriptor::alloc()->init(), [](MTL::ComputePipelineDescriptor *stepThreePipelineDescriptor) noexcept {
		
			// Free step three pipeline descriptor
			stepThreePipelineDescriptor->release();
		});
		const unique_ptr<MTL::ComputePipelineDescriptor, void(*)(MTL::ComputePipelineDescriptor *)> stepFourPipelineDescriptor(MTL::ComputePipelineDescriptor::alloc()->init(), [](MTL::ComputePipelineDescriptor *stepFourPipelineDescriptor) noexcept {
		
			// Free step four pipeline descriptor
			stepFourPipelineDescriptor->release();
		});
		const unique_ptr<MTL::ComputePipelineDescriptor, void(*)(MTL::ComputePipelineDescriptor *)> stepFivePipelineDescriptor(MTL::ComputePipelineDescriptor::alloc()->init(), [](MTL::ComputePipelineDescriptor *stepFivePipelineDescriptor) noexcept {
		
			// Free step five pipeline descriptor
			stepFivePipelineDescriptor->release();
		});
		const unique_ptr<MTL::ComputePipelineDescriptor, void(*)(MTL::ComputePipelineDescriptor *)> stepSixPipelineDescriptor(MTL::ComputePipelineDescriptor::alloc()->init(), [](MTL::ComputePipelineDescriptor *stepSixPipelineDescriptor) noexcept {
		
			// Free step six pipeline descriptor
			stepSixPipelineDescriptor->release();
		});
		const unique_ptr<MTL::ComputePipelineDescriptor, void(*)(MTL::ComputePipelineDescriptor *)> clearNumberOfEdgesPerBucketPipelineDescriptor(MTL::ComputePipelineDescriptor::alloc()->init(), [](MTL::ComputePipelineDescriptor *clearNumberOfEdgesPerBucketPipelineDescriptor) noexcept {
		
			// Free clear number of edges per bucket pipeline descriptor
			clearNumberOfEdgesPerBucketPipelineDescriptor->release();
		});
		if(!stepOnePipelineDescriptor || !stepTwoPipelineDescriptor || !stepThreePipelineDescriptor || !stepFourPipelineDescriptor || !stepFivePipelineDescriptor || !stepSixPipelineDescriptor || !clearNumberOfEdgesPerBucketPipelineDescriptor) {
		
			// Display message
			cout << "Creating pipeline descriptors failed." << endl;
			
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
		
		clearNumberOfEdgesPerBucketPipelineDescriptor->setComputeFunction(clearNumberOfEdgesPerBucketKernel.get());
		clearNumberOfEdgesPerBucketPipelineDescriptor->setMaxTotalThreadsPerThreadgroup(workItemsPerWorkGroup[6].width);
		clearNumberOfEdgesPerBucketPipelineDescriptor->setThreadGroupSizeIsMultipleOfThreadExecutionWidth(workItemsPerWorkGroup[6].width % clearNumberOfEdgesPerBucketPipeline->threadExecutionWidth() == 0);
		
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
		clearNumberOfEdgesPerBucketPipeline = unique_ptr<MTL::ComputePipelineState, void(*)(MTL::ComputePipelineState *)>(device->newComputePipelineState(clearNumberOfEdgesPerBucketPipelineDescriptor.get(), MTL::PipelineOptionNone, nullptr, &createPipelineClearNumberOfEdgesPerBucketError), [](MTL::ComputePipelineState *clearNumberOfEdgesPerBucketPipeline) noexcept {
		
			// Free clear number of edges per bucket pipeline
			clearNumberOfEdgesPerBucketPipeline->release();
		});
		if(!stepOnePipeline || !stepTwoPipeline || !stepThreePipeline || !stepFourPipeline || !stepFivePipeline || !stepSixPipeline || !clearNumberOfEdgesPerBucketPipeline) {
		
			// Display message
			cout << "Creating pipelines for the GPU failed." << endl;
			
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
			
			// Check if recreating pipeline clear number of edges per bucket failed and an error exists
			if(!clearNumberOfEdgesPerBucketPipeline && createPipelineClearNumberOfEdgesPerBucketError) {
			
				// Check if error's localized description exists
				const NS::String *localizedDescription = createPipelineClearNumberOfEdgesPerBucketError->localizedDescription();
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
		const unique_ptr<MTL::Buffer, void(*)(MTL::Buffer *)> bucketsOne(device->newBuffer(static_cast<uint64_t>(MEAN_TRIMMING_INITIAL_MAX_NUMBER_OF_EDGES_PER_BUCKET) * bucketsOneNumberOfBuckets * sizeof(uint32_t), MTL::ResourceStorageModePrivate | MTL::ResourceHazardTrackingModeUntracked), [](MTL::Buffer *bucketsOne) noexcept {
		
			// Free buckets one
			bucketsOne->release();
		});
		const unique_ptr<MTL::Buffer, void(*)(MTL::Buffer *)> bucketsOneSecondPart(device->newBuffer(static_cast<uint64_t>(MEAN_TRIMMING_INITIAL_MAX_NUMBER_OF_EDGES_PER_BUCKET) * (MEAN_TRIMMING_NUMBER_OF_BUCKETS - bucketsOneNumberOfBuckets) * sizeof(uint32_t), MTL::ResourceStorageModePrivate | MTL::ResourceHazardTrackingModeUntracked), [](MTL::Buffer *bucketsOneSecondPart) noexcept {
		
			// Free buckets one second part
			bucketsOneSecondPart->release();
		});
		const unique_ptr<MTL::Buffer, void(*)(MTL::Buffer *)> bucketsTwo(device->newBuffer(static_cast<uint64_t>(MEAN_TRIMMING_AFTER_TRIMMING_ROUND_MAX_NUMBER_OF_EDGES_PER_BUCKET) * MEAN_TRIMMING_NUMBER_OF_BUCKETS * sizeof(uint32_t), MTL::ResourceStorageModePrivate | MTL::ResourceHazardTrackingModeUntracked), [](MTL::Buffer *bucketsTwo) noexcept {
		
			// Free buckets two
			bucketsTwo->release();
		});
		const unique_ptr<MTL::Buffer, void(*)(MTL::Buffer *)> numberOfEdgesPerBucketOne(device->newBuffer(MEAN_TRIMMING_NUMBER_OF_BUCKETS * sizeof(uint32_t), MTL::ResourceStorageModePrivate | MTL::ResourceHazardTrackingModeUntracked), [](MTL::Buffer *numberOfEdgesPerBucketOne) noexcept {
		
			// Free number of edges per bucket one
			numberOfEdgesPerBucketOne->release();
		});
		const unique_ptr<MTL::Buffer, void(*)(MTL::Buffer *)> numberOfEdgesPerBucketTwo(device->newBuffer(MEAN_TRIMMING_NUMBER_OF_BUCKETS * sizeof(uint32_t), MTL::ResourceStorageModePrivate | MTL::ResourceHazardTrackingModeUntracked), [](MTL::Buffer *numberOfEdgesPerBucketTwo) noexcept {
		
			// Free number of edges per bucket two
			numberOfEdgesPerBucketTwo->release();
		});
		const unique_ptr<MTL::Buffer, void(*)(MTL::Buffer *)> remainingEdgesOne(device->newBuffer(sizeof(uint32_t) + static_cast<uint64_t>(MAX_NUMBER_OF_EDGES_AFTER_TRIMMING) * EDGE_NUMBER_OF_COMPONENTS * sizeof(uint32_t), MTL::ResourceStorageModeShared | MTL::ResourceHazardTrackingModeUntracked), [](MTL::Buffer *remainingEdgesOne) noexcept {
		
			// Free remaining edges one
			remainingEdgesOne->release();
		});
		const unique_ptr<MTL::Buffer, void(*)(MTL::Buffer *)> remainingEdgesTwo(device->newBuffer(sizeof(uint32_t) + static_cast<uint64_t>(MAX_NUMBER_OF_EDGES_AFTER_TRIMMING) * EDGE_NUMBER_OF_COMPONENTS * sizeof(uint32_t), MTL::ResourceStorageModeShared | MTL::ResourceHazardTrackingModeUntracked), [](MTL::Buffer *remainingEdgesTwo) noexcept {
		
			// Free remaining edges two
			remainingEdgesTwo->release();
		});
		if(!bucketsOne || (bucketsOneNumberOfBuckets != MEAN_TRIMMING_NUMBER_OF_BUCKETS && !bucketsOneSecondPart) || !bucketsTwo || !numberOfEdgesPerBucketOne || !numberOfEdgesPerBucketTwo || !remainingEdgesOne || !remainingEdgesTwo) {
		
			// Display message
			cout << "Allocating memory on the GPU failed." << endl;
			
			// Return false
			return false;
		}
		
		// Check if creating command queue for the device failed
		const unique_ptr<MTL::CommandQueue, void(*)(MTL::CommandQueue *)> commandQueue(device->newCommandQueue(), [](MTL::CommandQueue *commandQueue) noexcept {
		
			// Free command queue
			commandQueue->release();
		});
		if(!commandQueue) {
		
			// Display message
			cout << "Creating command queue for the GPU failed." << endl;
			
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
		unique_ptr<NS::AutoreleasePool, void(*)(NS::AutoreleasePool *)> autoreleasePool(NS::AutoreleasePool::alloc()->init(), [](NS::AutoreleasePool *autoreleasePool) noexcept {
		
			// Free autorelease pool
			autoreleasePool->release();
		});
		if(!autoreleasePool) {
		
			// Display message
			cout << "Creating autorelease pool failed." << endl;
			
			// Return false
			return false;
		}
		
		// Check if creating command queue's command buffer failed
		MTL::CommandBuffer *commandBuffer = commandQueue->commandBuffer();
		if(!commandBuffer) {
		
			// Display message
			cout << "Creating command queue's command buffer failed." << endl;
			
			// Return false
			return false;
		}
		
		// Check if creating command buffer's compute pass encoder failed
		MTL::ComputeCommandEncoder *computePassEncoder = commandBuffer->computeCommandEncoder();
		if(!computePassEncoder) {
		
			// Display message
			cout << "Creating command buffer's compute pass encoder failed." << endl;
			
			// Return false
			return false;
		}
		
		// Get SipHash keys from job's header and nonce
		uint64_t __attribute__((vector_size(sizeof(uint64_t) * SIPHASH_KEYS_SIZE))) sipHashKeysOne;
		uint64_t heightOne = jobHeight;
		uint64_t idOne = jobId;
		uint64_t nonceOne = jobNonce++;
		blake2b(sipHashKeysOne, jobHeader, nonceOne);
		
		// Clear the beginning of remaining edges one
		reinterpret_cast<uint32_t *>(remainingEdgesOne->contents())[0] = 0;
		
		// Set compute pass's buckets, number of edges per bucket, SipHash keys, destination buckets, and remaining edges arguments
		computePassEncoder->setBuffer(bucketsOne.get(), 0, 0);
		computePassEncoder->setBuffer(numberOfEdgesPerBucketOne.get(), 0, 1);
		computePassEncoder->setBytes(&sipHashKeysOne, sizeof(sipHashKeysOne), 2);
		computePassEncoder->setBuffer(bucketsTwo.get(), 0, 4);
		computePassEncoder->setBuffer(remainingEdgesOne.get(), 0, 6);
		
		// Check if using buckets one second part
		if(bucketsOneNumberOfBuckets != MEAN_TRIMMING_NUMBER_OF_BUCKETS) {
		
			// Set compute pass's buckets second part argument
			computePassEncoder->setBuffer(bucketsOneSecondPart.get(), 0, 3);
		}
		
		// Set compute pass's number of edges per bucket argument
		computePassEncoder->setBuffer(numberOfEdgesPerBucketOne.get(), 0, 5);
		
		// Add clearing number of edges per bucket one to the compute pass
		computePassEncoder->setComputePipelineState(clearNumberOfEdgesPerBucketPipeline.get());
		computePassEncoder->dispatchThreads(totalNumberOfWorkItems[6], workItemsPerWorkGroup[6]);
		
		// Add running step one to the compute pass
		computePassEncoder->setComputePipelineState(stepOnePipeline.get());
		computePassEncoder->dispatchThreads(totalNumberOfWorkItems[0], workItemsPerWorkGroup[0]);
		
		// Set compute pass's number of edges per bucket argument
		computePassEncoder->setBuffer(numberOfEdgesPerBucketTwo.get(), 0, 5);
		
		// Add clearing number of edges per bucket two to the compute pass
		computePassEncoder->setComputePipelineState(clearNumberOfEdgesPerBucketPipeline.get());
		computePassEncoder->dispatchThreads(totalNumberOfWorkItems[6], workItemsPerWorkGroup[6]);
		
		// Add running step two to the compute pass
		computePassEncoder->setComputePipelineState(stepTwoPipeline.get());
		computePassEncoder->dispatchThreads(totalNumberOfWorkItems[1], workItemsPerWorkGroup[1]);
		
		// Check if trimming more than one round
		if(TRIMMING_ROUNDS > 1) {
		
			// Set compute pass's number of edges per bucket argument
			computePassEncoder->setBuffer(numberOfEdgesPerBucketOne.get(), 0, 5);
			
			// Add clearing number of edges per bucket one to the compute pass
			computePassEncoder->setComputePipelineState(clearNumberOfEdgesPerBucketPipeline.get());
			computePassEncoder->dispatchThreads(totalNumberOfWorkItems[6], workItemsPerWorkGroup[6]);
			
			// Set compute pass's number of edges per source bucket argument
			computePassEncoder->setBuffer(numberOfEdgesPerBucketTwo.get(), 0, 5);
			
			// Add running step three to the compute pass
			computePassEncoder->setComputePipelineState(stepThreePipeline.get());
			computePassEncoder->dispatchThreads(totalNumberOfWorkItems[2], workItemsPerWorkGroup[2]);
			
			// Check if trimming more than two rounds
			if(TRIMMING_ROUNDS > 2) {
			
				// Add clearing number of edges per bucket two to the compute pass
				computePassEncoder->setComputePipelineState(clearNumberOfEdgesPerBucketPipeline.get());
				computePassEncoder->dispatchThreads(totalNumberOfWorkItems[6], workItemsPerWorkGroup[6]);
				
				// Add running step four to the compute pass
				computePassEncoder->setComputePipelineState(stepFourPipeline.get());
				computePassEncoder->dispatchThreads(totalNumberOfWorkItems[3], workItemsPerWorkGroup[3]);
				
				// Go through all remaining trimming rounds
				for(unsigned int i = 3; i < TRIMMING_ROUNDS; ++i) {
					
					// Set compute pass's source buckets, number of edges per source bucket, destination buckets, and number of edges per destination bucket arguments
					computePassEncoder->setBuffer((i % 2) ? bucketsTwo.get() : bucketsOne.get(), 0, 0);
					computePassEncoder->setBuffer((i % 2) ? numberOfEdgesPerBucketTwo.get() : numberOfEdgesPerBucketOne.get(), 0, 1);
					computePassEncoder->setBuffer((i % 2) ? bucketsOne.get() : bucketsTwo.get(), 0, 4);
					computePassEncoder->setBuffer((i % 2) ? numberOfEdgesPerBucketOne.get() : numberOfEdgesPerBucketTwo.get(), 0, 5);
					
					// Add clearing number of edges per bucket to the compute pass
					computePassEncoder->setComputePipelineState(clearNumberOfEdgesPerBucketPipeline.get());
					computePassEncoder->dispatchThreads(totalNumberOfWorkItems[6], workItemsPerWorkGroup[6]);
					
					// Add running step five to the compute pass
					computePassEncoder->setComputePipelineState(stepFivePipeline.get());
					computePassEncoder->dispatchThreads(totalNumberOfWorkItems[4], workItemsPerWorkGroup[4]);
				}
			}
			
			// Otherwise
			else {
			
				// Set compute pass's buckets and number of edges per bucket arguments
				computePassEncoder->setBuffer(bucketsOne.get(), 0, 4);
				computePassEncoder->setBuffer(numberOfEdgesPerBucketOne.get(), 0, 5);
			}
		}
		
		// Add running step six to the compute pass
		computePassEncoder->setComputePipelineState(stepSixPipeline.get());
		computePassEncoder->dispatchThreads(totalNumberOfWorkItems[5], workItemsPerWorkGroup[5]);
		
		// Finish adding commands to the compute pass
		computePassEncoder->endEncoding();
		
		// Set end time to now
		chrono::high_resolution_clock::time_point endTime = chrono::high_resolution_clock::now();
		
		// Run the compute pass
		commandBuffer->commit();
		
		// Wait until the compute pass has finished
		commandBuffer->waitUntilCompleted();
		
		// Check if closing
		if(closing) {
			
			// Return true
			return true;
		}
		
		// Check if running compute pass failed
		if(commandBuffer->error()) {
		
			// Display message
			cout << "Running compute pass failed." << endl;
			
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
			cout << "Creating autorelease pool failed." << endl;
			
			// Return false
			return false;
		}
		
		// Check if creating command queue's command buffer failed
		commandBuffer = commandQueue->commandBuffer();
		if(!commandBuffer) {
		
			// Display message
			cout << "Creating command queue's command buffer failed." << endl;
			
			// Return false
			return false;
		}
		
		// Check if creating command buffer's compute pass encoder failed
		computePassEncoder = commandBuffer->computeCommandEncoder();
		if(!computePassEncoder) {
		
			// Display message
			cout << "Creating command buffer's compute pass encoder failed." << endl;
			
			// Return false
			return false;
		}
		
		// Get SipHash keys from job's header and nonce
		uint64_t __attribute__((vector_size(sizeof(uint64_t) * SIPHASH_KEYS_SIZE))) sipHashKeysTwo;
		uint64_t heightTwo = jobHeight;
		uint64_t idTwo = jobId;
		uint64_t nonceTwo = jobNonce++;
		blake2b(sipHashKeysTwo, jobHeader, nonceTwo);
		
		// Clear the beginning of remaining edges two
		reinterpret_cast<uint32_t *>(remainingEdgesTwo->contents())[0] = 0;
		
		// Set compute pass's buckets, number of edges per bucket, SipHash keys, destination buckets, and remaining edges arguments
		computePassEncoder->setBuffer(bucketsOne.get(), 0, 0);
		computePassEncoder->setBuffer(numberOfEdgesPerBucketOne.get(), 0, 1);
		computePassEncoder->setBytes(&sipHashKeysTwo, sizeof(sipHashKeysTwo), 2);
		computePassEncoder->setBuffer(bucketsTwo.get(), 0, 4);
		computePassEncoder->setBuffer(remainingEdgesTwo.get(), 0, 6);
		
		// Check if using buckets one second part
		if(bucketsOneNumberOfBuckets != MEAN_TRIMMING_NUMBER_OF_BUCKETS) {
		
			// Set compute pass's buckets second part argument
			computePassEncoder->setBuffer(bucketsOneSecondPart.get(), 0, 3);
		}
		
		// Set compute pass's number of edges per bucket argument
		computePassEncoder->setBuffer(numberOfEdgesPerBucketOne.get(), 0, 5);
		
		// Add clearing number of edges per bucket one to the compute pass
		computePassEncoder->setComputePipelineState(clearNumberOfEdgesPerBucketPipeline.get());
		computePassEncoder->dispatchThreads(totalNumberOfWorkItems[6], workItemsPerWorkGroup[6]);
		
		// Add running step one to the compute pass
		computePassEncoder->setComputePipelineState(stepOnePipeline.get());
		computePassEncoder->dispatchThreads(totalNumberOfWorkItems[0], workItemsPerWorkGroup[0]);
		
		// Set compute pass's number of edges per bucket argument
		computePassEncoder->setBuffer(numberOfEdgesPerBucketTwo.get(), 0, 5);
		
		// Add clearing number of edges per bucket two to the compute pass
		computePassEncoder->setComputePipelineState(clearNumberOfEdgesPerBucketPipeline.get());
		computePassEncoder->dispatchThreads(totalNumberOfWorkItems[6], workItemsPerWorkGroup[6]);
		
		// Add running step two to the compute pass
		computePassEncoder->setComputePipelineState(stepTwoPipeline.get());
		computePassEncoder->dispatchThreads(totalNumberOfWorkItems[1], workItemsPerWorkGroup[1]);
		
		// Check if trimming more than one round
		if(TRIMMING_ROUNDS > 1) {
		
			// Set compute pass's number of edges per bucket argument
			computePassEncoder->setBuffer(numberOfEdgesPerBucketOne.get(), 0, 5);
			
			// Add clearing number of edges per bucket one to the compute pass
			computePassEncoder->setComputePipelineState(clearNumberOfEdgesPerBucketPipeline.get());
			computePassEncoder->dispatchThreads(totalNumberOfWorkItems[6], workItemsPerWorkGroup[6]);
			
			// Set compute pass's number of edges per source bucket argument
			computePassEncoder->setBuffer(numberOfEdgesPerBucketTwo.get(), 0, 5);
			
			// Add running step three to the compute pass
			computePassEncoder->setComputePipelineState(stepThreePipeline.get());
			computePassEncoder->dispatchThreads(totalNumberOfWorkItems[2], workItemsPerWorkGroup[2]);
			
			// Check if trimming more than two rounds
			if(TRIMMING_ROUNDS > 2) {
			
				// Add clearing number of edges per bucket two to the compute pass
				computePassEncoder->setComputePipelineState(clearNumberOfEdgesPerBucketPipeline.get());
				computePassEncoder->dispatchThreads(totalNumberOfWorkItems[6], workItemsPerWorkGroup[6]);
				
				// Add running step four to the compute pass
				computePassEncoder->setComputePipelineState(stepFourPipeline.get());
				computePassEncoder->dispatchThreads(totalNumberOfWorkItems[3], workItemsPerWorkGroup[3]);
				
				// Go through all remaining trimming rounds
				for(unsigned int i = 3; i < TRIMMING_ROUNDS; ++i) {
					
					// Set compute pass's source buckets, number of edges per source bucket, destination buckets, and number of edges per destination bucket arguments
					computePassEncoder->setBuffer((i % 2) ? bucketsTwo.get() : bucketsOne.get(), 0, 0);
					computePassEncoder->setBuffer((i % 2) ? numberOfEdgesPerBucketTwo.get() : numberOfEdgesPerBucketOne.get(), 0, 1);
					computePassEncoder->setBuffer((i % 2) ? bucketsOne.get() : bucketsTwo.get(), 0, 4);
					computePassEncoder->setBuffer((i % 2) ? numberOfEdgesPerBucketOne.get() : numberOfEdgesPerBucketTwo.get(), 0, 5);
					
					// Add clearing number of edges per bucket to the compute pass
					computePassEncoder->setComputePipelineState(clearNumberOfEdgesPerBucketPipeline.get());
					computePassEncoder->dispatchThreads(totalNumberOfWorkItems[6], workItemsPerWorkGroup[6]);
					
					// Add running step five to the compute pass
					computePassEncoder->setComputePipelineState(stepFivePipeline.get());
					computePassEncoder->dispatchThreads(totalNumberOfWorkItems[4], workItemsPerWorkGroup[4]);
				}
			}
			
			// Otherwise
			else {
			
				// Set compute pass's buckets and number of edges per bucket arguments
				computePassEncoder->setBuffer(bucketsOne.get(), 0, 4);
				computePassEncoder->setBuffer(numberOfEdgesPerBucketOne.get(), 0, 5);
			}
		}
		
		// Add running step six to the compute pass
		computePassEncoder->setComputePipelineState(stepSixPipeline.get());
		computePassEncoder->dispatchThreads(totalNumberOfWorkItems[5], workItemsPerWorkGroup[5]);
		
		// Finish adding commands to the compute pass
		computePassEncoder->endEncoding();
		
		// Set end time to now
		endTime = chrono::high_resolution_clock::now();
		
		// Run the compute pass
		commandBuffer->commit();
		
		// Trimming finished
		trimmingFinished(remainingEdgesOne->contents(), sipHashKeysOne, heightOne, idOne, nonceOne);
		
		// Wait until the compute pass has finished
		commandBuffer->waitUntilCompleted();
		
		// While not closing
		while(!closing) {
		
			// Check if running compute pass failed
			if(commandBuffer->error()) {
			
				// Display message
				cout << "Running compute pass failed." << endl;
				
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
				cout << "Creating autorelease pool failed." << endl;
				
				// Return false
				return false;
			}
			
			// Check if creating command queue's command buffer failed
			commandBuffer = commandQueue->commandBuffer();
			if(!commandBuffer) {
			
				// Display message
				cout << "Creating command queue's command buffer failed." << endl;
				
				// Return false
				return false;
			}
			
			// Check if creating command buffer's compute pass encoder failed
			computePassEncoder = commandBuffer->computeCommandEncoder();
			if(!computePassEncoder) {
			
				// Display message
				cout << "Creating command buffer's compute pass encoder failed." << endl;
				
				// Return false
				return false;
			}
			
			// Get SipHash keys from job's header and nonce
			heightOne = jobHeight;
			idOne = jobId;
			nonceOne = jobNonce++;
			blake2b(sipHashKeysOne, jobHeader, nonceOne);
			
			// Clear the beginning of remaining edges one
			reinterpret_cast<uint32_t *>(remainingEdgesOne->contents())[0] = 0;
			
			// Set compute pass's buckets, number of edges per bucket, SipHash keys, destination buckets, and remaining edges arguments
			computePassEncoder->setBuffer(bucketsOne.get(), 0, 0);
			computePassEncoder->setBuffer(numberOfEdgesPerBucketOne.get(), 0, 1);
			computePassEncoder->setBytes(&sipHashKeysOne, sizeof(sipHashKeysOne), 2);
			computePassEncoder->setBuffer(bucketsTwo.get(), 0, 4);
			computePassEncoder->setBuffer(remainingEdgesOne.get(), 0, 6);
			
			// Check if using buckets one second part
			if(bucketsOneNumberOfBuckets != MEAN_TRIMMING_NUMBER_OF_BUCKETS) {
			
				// Set compute pass's buckets second part argument
				computePassEncoder->setBuffer(bucketsOneSecondPart.get(), 0, 3);
			}
			
			// Set compute pass's number of edges per bucket argument
			computePassEncoder->setBuffer(numberOfEdgesPerBucketOne.get(), 0, 5);
			
			// Add clearing number of edges per bucket one to the compute pass
			computePassEncoder->setComputePipelineState(clearNumberOfEdgesPerBucketPipeline.get());
			computePassEncoder->dispatchThreads(totalNumberOfWorkItems[6], workItemsPerWorkGroup[6]);
			
			// Add running step one to the compute pass
			computePassEncoder->setComputePipelineState(stepOnePipeline.get());
			computePassEncoder->dispatchThreads(totalNumberOfWorkItems[0], workItemsPerWorkGroup[0]);
			
			// Set compute pass's number of edges per bucket argument
			computePassEncoder->setBuffer(numberOfEdgesPerBucketTwo.get(), 0, 5);
			
			// Add clearing number of edges per bucket two to the compute pass
			computePassEncoder->setComputePipelineState(clearNumberOfEdgesPerBucketPipeline.get());
			computePassEncoder->dispatchThreads(totalNumberOfWorkItems[6], workItemsPerWorkGroup[6]);
			
			// Add running step two to the compute pass
			computePassEncoder->setComputePipelineState(stepTwoPipeline.get());
			computePassEncoder->dispatchThreads(totalNumberOfWorkItems[1], workItemsPerWorkGroup[1]);
			
			// Check if trimming more than one round
			if(TRIMMING_ROUNDS > 1) {
			
				// Set compute pass's number of edges per bucket argument
				computePassEncoder->setBuffer(numberOfEdgesPerBucketOne.get(), 0, 5);
				
				// Add clearing number of edges per bucket one to the compute pass
				computePassEncoder->setComputePipelineState(clearNumberOfEdgesPerBucketPipeline.get());
				computePassEncoder->dispatchThreads(totalNumberOfWorkItems[6], workItemsPerWorkGroup[6]);
				
				// Set compute pass's number of edges per source bucket argument
				computePassEncoder->setBuffer(numberOfEdgesPerBucketTwo.get(), 0, 5);
				
				// Add running step three to the compute pass
				computePassEncoder->setComputePipelineState(stepThreePipeline.get());
				computePassEncoder->dispatchThreads(totalNumberOfWorkItems[2], workItemsPerWorkGroup[2]);
				
				// Check if trimming more than two rounds
				if(TRIMMING_ROUNDS > 2) {
				
					// Add clearing number of edges per bucket two to the compute pass
					computePassEncoder->setComputePipelineState(clearNumberOfEdgesPerBucketPipeline.get());
					computePassEncoder->dispatchThreads(totalNumberOfWorkItems[6], workItemsPerWorkGroup[6]);
					
					// Add running step four to the compute pass
					computePassEncoder->setComputePipelineState(stepFourPipeline.get());
					computePassEncoder->dispatchThreads(totalNumberOfWorkItems[3], workItemsPerWorkGroup[3]);
					
					// Go through all remaining trimming rounds
					for(unsigned int i = 3; i < TRIMMING_ROUNDS; ++i) {
					
						// Set compute pass's source buckets, number of edges per source bucket, destination buckets, and number of edges per destination bucket arguments
						computePassEncoder->setBuffer((i % 2) ? bucketsTwo.get() : bucketsOne.get(), 0, 0);
						computePassEncoder->setBuffer((i % 2) ? numberOfEdgesPerBucketTwo.get() : numberOfEdgesPerBucketOne.get(), 0, 1);
						computePassEncoder->setBuffer((i % 2) ? bucketsOne.get() : bucketsTwo.get(), 0, 4);
						computePassEncoder->setBuffer((i % 2) ? numberOfEdgesPerBucketOne.get() : numberOfEdgesPerBucketTwo.get(), 0, 5);
						
						// Add clearing number of edges per bucket to the compute pass
						computePassEncoder->setComputePipelineState(clearNumberOfEdgesPerBucketPipeline.get());
						computePassEncoder->dispatchThreads(totalNumberOfWorkItems[6], workItemsPerWorkGroup[6]);
						
						// Add running step five to the compute pass
						computePassEncoder->setComputePipelineState(stepFivePipeline.get());
						computePassEncoder->dispatchThreads(totalNumberOfWorkItems[4], workItemsPerWorkGroup[4]);
					}
				}
				
				// Otherwise
				else {
				
					// Set compute pass's buckets and number of edges per bucket arguments
					computePassEncoder->setBuffer(bucketsOne.get(), 0, 4);
					computePassEncoder->setBuffer(numberOfEdgesPerBucketOne.get(), 0, 5);
				}
			}
			
			// Add running step six to the compute pass
			computePassEncoder->setComputePipelineState(stepSixPipeline.get());
			computePassEncoder->dispatchThreads(totalNumberOfWorkItems[5], workItemsPerWorkGroup[5]);
			
			// Finish adding commands to the compute pass
			computePassEncoder->endEncoding();
			
			// Set end time to now
			endTime = chrono::high_resolution_clock::now();
			
			// Run the compute pass
			commandBuffer->commit();
			
			// Trimming finished
			trimmingFinished(remainingEdgesTwo->contents(), sipHashKeysTwo, heightTwo, idTwo, nonceTwo);
			
			// Wait until the compute pass has finished
			commandBuffer->waitUntilCompleted();
			
			// Check if closing
			if(closing) {
			
				// Return true
				return true;
			}
			
			// Check if running compute pass failed
			if(commandBuffer->error()) {
			
				// Display message
				cout << "Running compute pass failed." << endl;
				
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
				cout << "Creating autorelease pool failed." << endl;
				
				// Return false
				return false;
			}
			
			// Check if creating command queue's command buffer failed
			commandBuffer = commandQueue->commandBuffer();
			if(!commandBuffer) {
			
				// Display message
				cout << "Creating command queue's command buffer failed." << endl;
				
				// Return false
				return false;
			}
			
			// Check if creating command buffer's compute pass encoder failed
			computePassEncoder = commandBuffer->computeCommandEncoder();
			if(!computePassEncoder) {
			
				// Display message
				cout << "Creating command buffer's compute pass encoder failed." << endl;
				
				// Return false
				return false;
			}
			
			// Get SipHash keys from job's header and nonce
			heightTwo = jobHeight;
			idTwo = jobId;
			nonceTwo = jobNonce++;
			blake2b(sipHashKeysTwo, jobHeader, nonceTwo);
			
			// Clear the beginning of remaining edges two
			reinterpret_cast<uint32_t *>(remainingEdgesTwo->contents())[0] = 0;
			
			// Set compute pass's buckets, number of edges per bucket, SipHash keys, destination buckets, and remaining edges arguments
			computePassEncoder->setBuffer(bucketsOne.get(), 0, 0);
			computePassEncoder->setBuffer(numberOfEdgesPerBucketOne.get(), 0, 1);
			computePassEncoder->setBytes(&sipHashKeysTwo, sizeof(sipHashKeysTwo), 2);
			computePassEncoder->setBuffer(bucketsTwo.get(), 0, 4);
			computePassEncoder->setBuffer(remainingEdgesTwo.get(), 0, 6);
			
			// Check if using buckets one second part
			if(bucketsOneNumberOfBuckets != MEAN_TRIMMING_NUMBER_OF_BUCKETS) {
			
				// Set compute pass's buckets second part argument
				computePassEncoder->setBuffer(bucketsOneSecondPart.get(), 0, 3);
			}
			
			// Set compute pass's number of edges per bucket argument
			computePassEncoder->setBuffer(numberOfEdgesPerBucketOne.get(), 0, 5);
			
			// Add clearing number of edges per bucket one to the compute pass
			computePassEncoder->setComputePipelineState(clearNumberOfEdgesPerBucketPipeline.get());
			computePassEncoder->dispatchThreads(totalNumberOfWorkItems[6], workItemsPerWorkGroup[6]);
			
			// Add running step one to the compute pass
			computePassEncoder->setComputePipelineState(stepOnePipeline.get());
			computePassEncoder->dispatchThreads(totalNumberOfWorkItems[0], workItemsPerWorkGroup[0]);
			
			// Set compute pass's number of edges per bucket argument
			computePassEncoder->setBuffer(numberOfEdgesPerBucketTwo.get(), 0, 5);
			
			// Add clearing number of edges per bucket two to the compute pass
			computePassEncoder->setComputePipelineState(clearNumberOfEdgesPerBucketPipeline.get());
			computePassEncoder->dispatchThreads(totalNumberOfWorkItems[6], workItemsPerWorkGroup[6]);
			
			// Add running step two to the compute pass
			computePassEncoder->setComputePipelineState(stepTwoPipeline.get());
			computePassEncoder->dispatchThreads(totalNumberOfWorkItems[1], workItemsPerWorkGroup[1]);
			
			// Check if trimming more than one round
			if(TRIMMING_ROUNDS > 1) {
			
				// Set compute pass's number of edges per bucket argument
				computePassEncoder->setBuffer(numberOfEdgesPerBucketOne.get(), 0, 5);
				
				// Add clearing number of edges per bucket one to the compute pass
				computePassEncoder->setComputePipelineState(clearNumberOfEdgesPerBucketPipeline.get());
				computePassEncoder->dispatchThreads(totalNumberOfWorkItems[6], workItemsPerWorkGroup[6]);
				
				// Set compute pass's number of edges per source bucket argument
				computePassEncoder->setBuffer(numberOfEdgesPerBucketTwo.get(), 0, 5);
				
				// Add running step three to the compute pass
				computePassEncoder->setComputePipelineState(stepThreePipeline.get());
				computePassEncoder->dispatchThreads(totalNumberOfWorkItems[2], workItemsPerWorkGroup[2]);
				
				// Check if trimming more than two rounds
				if(TRIMMING_ROUNDS > 2) {
				
					// Add clearing number of edges per bucket two to the compute pass
					computePassEncoder->setComputePipelineState(clearNumberOfEdgesPerBucketPipeline.get());
					computePassEncoder->dispatchThreads(totalNumberOfWorkItems[6], workItemsPerWorkGroup[6]);
					
					// Add running step four to the compute pass
					computePassEncoder->setComputePipelineState(stepFourPipeline.get());
					computePassEncoder->dispatchThreads(totalNumberOfWorkItems[3], workItemsPerWorkGroup[3]);
					
					// Go through all remaining trimming rounds
					for(unsigned int i = 3; i < TRIMMING_ROUNDS; ++i) {
					
						// Set compute pass's source buckets, number of edges per source bucket, destination buckets, and number of edges per destination bucket arguments
						computePassEncoder->setBuffer((i % 2) ? bucketsTwo.get() : bucketsOne.get(), 0, 0);
						computePassEncoder->setBuffer((i % 2) ? numberOfEdgesPerBucketTwo.get() : numberOfEdgesPerBucketOne.get(), 0, 1);
						computePassEncoder->setBuffer((i % 2) ? bucketsOne.get() : bucketsTwo.get(), 0, 4);
						computePassEncoder->setBuffer((i % 2) ? numberOfEdgesPerBucketOne.get() : numberOfEdgesPerBucketTwo.get(), 0, 5);
						
						// Add clearing number of edges per bucket to the compute pass
						computePassEncoder->setComputePipelineState(clearNumberOfEdgesPerBucketPipeline.get());
						computePassEncoder->dispatchThreads(totalNumberOfWorkItems[6], workItemsPerWorkGroup[6]);
						
						// Add running step five to the compute pass
						computePassEncoder->setComputePipelineState(stepFivePipeline.get());
						computePassEncoder->dispatchThreads(totalNumberOfWorkItems[4], workItemsPerWorkGroup[4]);
					}
				}
				
				// Otherwise
				else {
				
					// Set compute pass's buckets and number of edges per bucket arguments
					computePassEncoder->setBuffer(bucketsOne.get(), 0, 4);
					computePassEncoder->setBuffer(numberOfEdgesPerBucketOne.get(), 0, 5);
				}
			}
			
			// Add running step six to the compute pass
			computePassEncoder->setComputePipelineState(stepSixPipeline.get());
			computePassEncoder->dispatchThreads(totalNumberOfWorkItems[5], workItemsPerWorkGroup[5]);
			
			// Finish adding commands to the compute pass
			computePassEncoder->endEncoding();
			
			// Set end time to now
			endTime = chrono::high_resolution_clock::now();
			
			// Run the compute pass
			commandBuffer->commit();
			
			// Trimming finished
			trimmingFinished(remainingEdgesOne->contents(), sipHashKeysOne, heightOne, idOne, nonceOne);
			
			// Wait until the compute pass has finished
			commandBuffer->waitUntilCompleted();
		}
		
		// Return true
		return true;
	}
	
// Otherwise
#else

	// Perform mean trimming loop
	bool performMeanTrimmingLoop(const cl_context context) noexcept {
	
		// Check if getting context's device failed
		cl_device_id device;
		if(clGetContextInfo(context, CL_CONTEXT_DEVICES, sizeof(device), &device, nullptr) != CL_SUCCESS) {
		
			// Display message
			cout << "Getting OpenCL context's GPU failed." << endl;
			
			// Return false
			return false;
		}
		
		// Check if getting device's max work group size, max memory allocation size, or work group memory size failed, or its max work group size don't exists
		size_t maxWorkGroupSize;
		cl_ulong maxMemoryAllocationSize;
		cl_ulong workGroupMemorySize;
		if(clGetDeviceInfo(device, CL_DEVICE_MAX_WORK_GROUP_SIZE, sizeof(maxWorkGroupSize), &maxWorkGroupSize, nullptr) != CL_SUCCESS || clGetDeviceInfo(device, CL_DEVICE_MAX_MEM_ALLOC_SIZE, sizeof(maxMemoryAllocationSize), &maxMemoryAllocationSize, nullptr) != CL_SUCCESS || clGetDeviceInfo(device, CL_DEVICE_LOCAL_MEM_SIZE, sizeof(workGroupMemorySize), &workGroupMemorySize, nullptr) != CL_SUCCESS || !maxWorkGroupSize) {
		
			// Display message
			cout << "Getting GPU's info failed." << endl;
			
			// Return false
			return false;
		}
		
		// Check if device's work group memory isn't fully utilized
		if(bit_floor(workGroupMemorySize) / BYTES_IN_A_KILOBYTE > LOCAL_RAM_KILOBYTES) {
		
			// Display message
			cout << "GPU's local RAM won't be fully utilized. Build this program with LOCAL_RAM_KILOBYTES=" << (bit_floor(workGroupMemorySize) / BYTES_IN_A_KILOBYTE) << " for potentially better performance." << endl;
		}
		
		// Try to allocate more than the max memory allocation size
		cl_int errorCode;
		unique_ptr<remove_pointer<cl_mem>::type, decltype(&clReleaseMemObject)> moreThanMaxMemoryAllocation(clCreateBuffer(context, CL_MEM_READ_WRITE | CL_MEM_HOST_NO_ACCESS, maxMemoryAllocationSize + 1, nullptr, &errorCode), clReleaseMemObject);
		
		// Set enforce max memory allocation size to if the allocation failed because of an invalid buffer size
		const bool enforceMaxMemoryAllocationSize = errorCode == CL_INVALID_BUFFER_SIZE;
		moreThanMaxMemoryAllocation.reset();
		
		// Check if creating program for the device failed
		const char *source = (
			#include "./mean_trimming.cl"
		);
		const size_t sourceSize = strlen(source);
		unique_ptr<remove_pointer<cl_program>::type, decltype(&clReleaseProgram)> program(clCreateProgramWithSource(context, 1, &source, &sourceSize, nullptr), clReleaseProgram);
		if(!program) {
		
			// Display message
			cout << "Creating program for the GPU failed." << endl;
			
			// Return false
			return false;
		}
		
		// Set buckets one number of buckets based on if enforcing max memory allocation size
		const size_t bucketsOneNumberOfBuckets = (enforceMaxMemoryAllocationSize && static_cast<uint64_t>(MEAN_TRIMMING_INITIAL_MAX_NUMBER_OF_EDGES_PER_BUCKET) * MEAN_TRIMMING_NUMBER_OF_BUCKETS * sizeof(cl_uint) > maxMemoryAllocationSize) ? (MEAN_TRIMMING_AFTER_TRIMMING_ROUND_MAX_NUMBER_OF_EDGES_PER_BUCKET * MEAN_TRIMMING_NUMBER_OF_BUCKETS + MEAN_TRIMMING_INITIAL_MAX_NUMBER_OF_EDGES_PER_BUCKET - 1) / MEAN_TRIMMING_INITIAL_MAX_NUMBER_OF_EDGES_PER_BUCKET : MEAN_TRIMMING_NUMBER_OF_BUCKETS;
		
		// Set local buckets size based on the device's work group memory size
		const unsigned int localBucketsSize = min(bit_floor((workGroupMemorySize - (MEAN_TRIMMING_NUMBER_OF_BUCKETS + sizeof(cl_uint) - 1)) / (sizeof(cl_uint) * MEAN_TRIMMING_NUMBER_OF_BUCKETS) + 1), MEAN_TRIMMING_MAX_LOCAL_BUCKETS_SIZE);
		
		// Check if building program for the device failed
		if(clBuildProgram(program.get(), 1, &device, ("-cl-std=CL1.2 -Werror -DEDGE_BITS=" TO_STRING(EDGE_BITS) " -DTRIMMING_ROUNDS=" TO_STRING(TRIMMING_ROUNDS) " -DEDGE_NUMBER_OF_COMPONENTS=" TO_STRING(EDGE_NUMBER_OF_COMPONENTS) " -DNUMBER_OF_EDGES_PER_STEP_ONE_WORK_ITEM=" + to_string(MEAN_TRIMMING_NUMBER_OF_EDGES_PER_STEP_ONE_WORK_ITEM) + " -DNUMBER_OF_BITMAP_BYTES=" + to_string(MEAN_TRIMMING_NUMBER_OF_BITMAP_BYTES) + " -DNUMBER_OF_BUCKETS=" + to_string(MEAN_TRIMMING_NUMBER_OF_BUCKETS) + " -DMAX_NUMBER_OF_EDGES_AFTER_TRIMMING=" + to_string(MAX_NUMBER_OF_EDGES_AFTER_TRIMMING) + " -DNUMBER_OF_LEAST_SIGNIFICANT_BITS_IGNORED_DURING_BUCKET_SORTING=" + to_string(MEAN_TRIMMING_NUMBER_OF_LEAST_SIGNIFICANT_BITS_IGNORED_DURING_BUCKET_SORTING) + " -DINITIAL_MAX_NUMBER_OF_EDGES_PER_BUCKET=" + to_string(MEAN_TRIMMING_INITIAL_MAX_NUMBER_OF_EDGES_PER_BUCKET) + " -DAFTER_TRIMMING_ROUND_MAX_NUMBER_OF_EDGES_PER_BUCKET=" + to_string(MEAN_TRIMMING_AFTER_TRIMMING_ROUND_MAX_NUMBER_OF_EDGES_PER_BUCKET) + " -DINITIAL_BUCKETS_NUMBER_OF_BUCKETS=" + to_string(bucketsOneNumberOfBuckets) + " -DLOCAL_BUCKETS_SIZE=" + to_string(localBucketsSize)).c_str(), nullptr, nullptr) != CL_SUCCESS) {
		
			// Display message
			cout << "Building program for the GPU failed." << endl;
			
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
		unique_ptr<remove_pointer<cl_kernel>::type, decltype(&clReleaseKernel)> stepOneKernel(clCreateKernel(program.get(), "trimEdgesStepOne", nullptr), clReleaseKernel);
		unique_ptr<remove_pointer<cl_kernel>::type, decltype(&clReleaseKernel)> stepTwoKernel(clCreateKernel(program.get(), "trimEdgesStepTwo", nullptr), clReleaseKernel);
		unique_ptr<remove_pointer<cl_kernel>::type, decltype(&clReleaseKernel)> stepThreeKernel(clCreateKernel(program.get(), "trimEdgesStepThree", nullptr), clReleaseKernel);
		unique_ptr<remove_pointer<cl_kernel>::type, decltype(&clReleaseKernel)> stepFourKernel(clCreateKernel(program.get(), "trimEdgesStepFour", nullptr), clReleaseKernel);
		unique_ptr<remove_pointer<cl_kernel>::type, decltype(&clReleaseKernel)> stepFiveKernel(clCreateKernel(program.get(), "trimEdgesStepFive", nullptr), clReleaseKernel);
		unique_ptr<remove_pointer<cl_kernel>::type, decltype(&clReleaseKernel)> stepSixKernel(clCreateKernel(program.get(), "trimEdgesStepSix", nullptr), clReleaseKernel);
		if(!stepOneKernel || !stepTwoKernel || !stepThreeKernel || !stepFourKernel || !stepFiveKernel || !stepSixKernel) {
		
			// Display message
			cout << "Creating kernels for the GPU failed." << endl;
			
			// Return false
			return false;
		}
		
		// Set total number of work items based on the kernels and work items per work group
		const size_t totalNumberOfWorkItems[] = {
		
			// Trim edges step one kernel
			NUMBER_OF_EDGES / MEAN_TRIMMING_NUMBER_OF_EDGES_PER_STEP_ONE_WORK_ITEM,
			
			// Trim edges step two kernel
			MEAN_TRIMMING_NUMBER_OF_BUCKETS * min(bit_floor(maxWorkGroupSize), bit_floor(static_cast<size_t>(INT16_MAX / 2))),
			
			// Trim edges step three kernel
			MEAN_TRIMMING_NUMBER_OF_BUCKETS * min(bit_floor(maxWorkGroupSize), bit_floor(static_cast<size_t>(INT16_MAX / 2))),
			
			// Trim edges step four kernel
			MEAN_TRIMMING_NUMBER_OF_BUCKETS * min(bit_floor(maxWorkGroupSize), bit_floor(static_cast<size_t>(INT16_MAX / 2))),
			
			// Trim edges step five kernel
			MEAN_TRIMMING_NUMBER_OF_BUCKETS * min(bit_floor(maxWorkGroupSize), bit_floor(static_cast<size_t>(INT16_MAX / 2))),
			
			// Trim edges step six kernel
			MEAN_TRIMMING_NUMBER_OF_BUCKETS * min(bit_floor(maxWorkGroupSize), static_cast<size_t>(UINT16_MAX))
		};
		
		// Set work items per work group based on the total number of work items and max work group size
		const size_t workItemsPerWorkGroup[] = {
		
			// Trim edges step one kernel
			min(min(bit_floor(min(static_cast<size_t>(MEAN_TRIMMING_WORK_ITEMS_PER_WORK_GROUP_STEP_ONE), maxWorkGroupSize)), bit_floor(static_cast<size_t>(INT16_MAX / 2))), totalNumberOfWorkItems[0]),
			
			// Trim edges step two kernel
			min(bit_floor(maxWorkGroupSize), bit_floor(static_cast<size_t>(INT16_MAX / 2))),
			
			// Trim edges step three kernel
			min(bit_floor(maxWorkGroupSize), bit_floor(static_cast<size_t>(INT16_MAX / 2))),
			
			// Trim edges step four kernel
			min(bit_floor(maxWorkGroupSize), bit_floor(static_cast<size_t>(INT16_MAX / 2))),
			
			// Trim edges step five kernel
			min(bit_floor(maxWorkGroupSize), bit_floor(static_cast<size_t>(INT16_MAX / 2))),
			
			// Trim edges step six kernel
			min(bit_floor(maxWorkGroupSize), static_cast<size_t>(UINT16_MAX))
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
			cout << "Creating program for the GPU failed." << endl;
			
			// Return false
			return false;
		}
		
		// Check if rebuilding program for the device with hardcoded work items per work groups failed
		if(clBuildProgram(program.get(), 1, &device, ("-cl-std=CL1.2 -Werror -DEDGE_BITS=" TO_STRING(EDGE_BITS) " -DTRIMMING_ROUNDS=" TO_STRING(TRIMMING_ROUNDS) " -DEDGE_NUMBER_OF_COMPONENTS=" TO_STRING(EDGE_NUMBER_OF_COMPONENTS) " -DNUMBER_OF_EDGES_PER_STEP_ONE_WORK_ITEM=" + to_string(MEAN_TRIMMING_NUMBER_OF_EDGES_PER_STEP_ONE_WORK_ITEM) + " -DNUMBER_OF_BITMAP_BYTES=" + to_string(MEAN_TRIMMING_NUMBER_OF_BITMAP_BYTES) + " -DNUMBER_OF_BUCKETS=" + to_string(MEAN_TRIMMING_NUMBER_OF_BUCKETS) + " -DMAX_NUMBER_OF_EDGES_AFTER_TRIMMING=" + to_string(MAX_NUMBER_OF_EDGES_AFTER_TRIMMING) + " -DNUMBER_OF_LEAST_SIGNIFICANT_BITS_IGNORED_DURING_BUCKET_SORTING=" + to_string(MEAN_TRIMMING_NUMBER_OF_LEAST_SIGNIFICANT_BITS_IGNORED_DURING_BUCKET_SORTING) + " -DINITIAL_MAX_NUMBER_OF_EDGES_PER_BUCKET=" + to_string(MEAN_TRIMMING_INITIAL_MAX_NUMBER_OF_EDGES_PER_BUCKET) + " -DAFTER_TRIMMING_ROUND_MAX_NUMBER_OF_EDGES_PER_BUCKET=" + to_string(MEAN_TRIMMING_AFTER_TRIMMING_ROUND_MAX_NUMBER_OF_EDGES_PER_BUCKET) + " -DINITIAL_BUCKETS_NUMBER_OF_BUCKETS=" + to_string(bucketsOneNumberOfBuckets) + " -DLOCAL_BUCKETS_SIZE=" + to_string(localBucketsSize) + " -DTRIM_EDGES_STEP_ONE_WORK_ITEMS_PER_WORK_GROUP=" + to_string(workItemsPerWorkGroup[0]) + " -DTRIM_EDGES_STEP_TWO_WORK_ITEMS_PER_WORK_GROUP=" + to_string(workItemsPerWorkGroup[1]) + " -DTRIM_EDGES_STEP_THREE_WORK_ITEMS_PER_WORK_GROUP=" + to_string(workItemsPerWorkGroup[2]) + " -DTRIM_EDGES_STEP_FOUR_WORK_ITEMS_PER_WORK_GROUP=" + to_string(workItemsPerWorkGroup[3]) + " -DTRIM_EDGES_STEP_FIVE_WORK_ITEMS_PER_WORK_GROUP=" + to_string(workItemsPerWorkGroup[4]) + " -DTRIM_EDGES_STEP_SIX_WORK_ITEMS_PER_WORK_GROUP=" + to_string(workItemsPerWorkGroup[5])).c_str(), nullptr, nullptr) != CL_SUCCESS) {
		
			// Display message
			cout << "Building program for the GPU failed." << endl;
			
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
			cout << "Creating kernels for the GPU failed." << endl;
			
			// Return false
			return false;
		}
		
		// Check if allocating memory on the device failed
		const unique_ptr<remove_pointer<cl_mem>::type, decltype(&clReleaseMemObject)> bucketsOne(clCreateBuffer(context, CL_MEM_READ_WRITE | CL_MEM_HOST_NO_ACCESS, static_cast<uint64_t>(MEAN_TRIMMING_INITIAL_MAX_NUMBER_OF_EDGES_PER_BUCKET) * bucketsOneNumberOfBuckets * sizeof(cl_uint), nullptr, nullptr), clReleaseMemObject);
		const unique_ptr<remove_pointer<cl_mem>::type, decltype(&clReleaseMemObject)> bucketsOneSecondPart(clCreateBuffer(context, CL_MEM_READ_WRITE | CL_MEM_HOST_NO_ACCESS, static_cast<uint64_t>(MEAN_TRIMMING_INITIAL_MAX_NUMBER_OF_EDGES_PER_BUCKET) * (MEAN_TRIMMING_NUMBER_OF_BUCKETS - bucketsOneNumberOfBuckets) * sizeof(cl_uint), nullptr, nullptr), clReleaseMemObject);
		const unique_ptr<remove_pointer<cl_mem>::type, decltype(&clReleaseMemObject)> bucketsTwo(clCreateBuffer(context, CL_MEM_READ_WRITE | CL_MEM_HOST_NO_ACCESS, static_cast<uint64_t>(MEAN_TRIMMING_AFTER_TRIMMING_ROUND_MAX_NUMBER_OF_EDGES_PER_BUCKET) * MEAN_TRIMMING_NUMBER_OF_BUCKETS * sizeof(cl_uint), nullptr, nullptr), clReleaseMemObject);
		const unique_ptr<remove_pointer<cl_mem>::type, decltype(&clReleaseMemObject)> numberOfEdgesPerBucketOne(clCreateBuffer(context, CL_MEM_READ_WRITE | CL_MEM_HOST_NO_ACCESS, MEAN_TRIMMING_NUMBER_OF_BUCKETS * sizeof(cl_uint), nullptr, nullptr), clReleaseMemObject);
		const unique_ptr<remove_pointer<cl_mem>::type, decltype(&clReleaseMemObject)> numberOfEdgesPerBucketTwo(clCreateBuffer(context, CL_MEM_READ_WRITE | CL_MEM_HOST_NO_ACCESS, MEAN_TRIMMING_NUMBER_OF_BUCKETS * sizeof(cl_uint), nullptr, nullptr), clReleaseMemObject);
		static unique_ptr<remove_pointer<cl_mem>::type, decltype(&clReleaseMemObject)> remainingEdgesOne(nullptr, clReleaseMemObject);
		remainingEdgesOne = unique_ptr<remove_pointer<cl_mem>::type, decltype(&clReleaseMemObject)>(clCreateBuffer(context, CL_MEM_READ_WRITE | CL_MEM_HOST_READ_ONLY | CL_MEM_ALLOC_HOST_PTR, sizeof(cl_uint) + static_cast<uint64_t>(MAX_NUMBER_OF_EDGES_AFTER_TRIMMING) * EDGE_NUMBER_OF_COMPONENTS * sizeof(cl_uint), nullptr, nullptr), clReleaseMemObject);
		static unique_ptr<remove_pointer<cl_mem>::type, decltype(&clReleaseMemObject)> remainingEdgesTwo(nullptr, clReleaseMemObject);
		remainingEdgesTwo = unique_ptr<remove_pointer<cl_mem>::type, decltype(&clReleaseMemObject)>(clCreateBuffer(context, CL_MEM_READ_WRITE | CL_MEM_HOST_READ_ONLY | CL_MEM_ALLOC_HOST_PTR, sizeof(cl_uint) + static_cast<uint64_t>(MAX_NUMBER_OF_EDGES_AFTER_TRIMMING) * EDGE_NUMBER_OF_COMPONENTS * sizeof(cl_uint), nullptr, nullptr), clReleaseMemObject);
		if(!bucketsOne || (bucketsOneNumberOfBuckets != MEAN_TRIMMING_NUMBER_OF_BUCKETS && !bucketsOneSecondPart) || !bucketsTwo || !numberOfEdgesPerBucketOne || !numberOfEdgesPerBucketTwo || !remainingEdgesOne || !remainingEdgesTwo) {
		
			// Display message
			cout << "Allocating memory on the GPU failed." << endl;
			
			// Free remaining edges one and two
			remainingEdgesOne.reset();
			remainingEdgesTwo.reset();
			
			// Return false
			return false;
		}
		
		// Check if creating command queue for the device failed
		static uint32_t *resultOne = nullptr;
		static uint32_t *resultTwo = nullptr;
		const unique_ptr<remove_pointer<cl_command_queue>::type, void(*)(cl_command_queue)> commandQueue(clCreateCommandQueue(context, device, CL_QUEUE_PROFILING_ENABLE, nullptr), [](cl_command_queue commandQueue) noexcept {
		
			// Wait for all commands in the queue to finish
			clFinish(commandQueue);
			
			// Check if result one exists
			if(resultOne) {
			
				// Queue unmapping result one
				clEnqueueUnmapMemObject(commandQueue, remainingEdgesOne.get(), reinterpret_cast<void *>(resultOne), 0, nullptr, nullptr);
			}
			
			// Check if result two exists
			if(resultTwo) {
			
				// Queue unmapping result two
				clEnqueueUnmapMemObject(commandQueue, remainingEdgesTwo.get(), reinterpret_cast<void *>(resultTwo), 0, nullptr, nullptr);
			}
			
			// Wait for all commands in the queue to finish
			clFinish(commandQueue);
			
			// Free command queue
			clReleaseCommandQueue(commandQueue);
			
			// Reset result one and two
			resultOne = nullptr;
			resultTwo = nullptr;
			
			// Free remaining edges one and two
			remainingEdgesOne.reset();
			remainingEdgesTwo.reset();
		});
		if(!commandQueue) {
		
			// Display message
			cout << "Creating command queue for the GPU failed." << endl;
			
			// Free remaining edges one and two
			remainingEdgesOne.reset();
			remainingEdgesTwo.reset();
			
			// Return false
			return false;
		}
		
		// Check if setting program's unchanging arguments failed
		if(clSetKernelArg(stepOneKernel.get(), 0, sizeof(bucketsOne.get()), &unmove(bucketsOne.get())) != CL_SUCCESS || clSetKernelArg(stepOneKernel.get(), 1, sizeof(numberOfEdgesPerBucketOne.get()), &unmove(numberOfEdgesPerBucketOne.get())) != CL_SUCCESS || clSetKernelArg(stepTwoKernel.get(), 0, sizeof(bucketsOne.get()), &unmove(bucketsOne.get())) != CL_SUCCESS || clSetKernelArg(stepTwoKernel.get(), 1, sizeof(numberOfEdgesPerBucketOne.get()), &unmove(numberOfEdgesPerBucketOne.get())) != CL_SUCCESS || clSetKernelArg(stepTwoKernel.get(), 2, sizeof(bucketsTwo.get()), &unmove(bucketsTwo.get())) != CL_SUCCESS || clSetKernelArg(stepTwoKernel.get(), 3, sizeof(numberOfEdgesPerBucketTwo.get()), &unmove(numberOfEdgesPerBucketTwo.get())) != CL_SUCCESS || clSetKernelArg(stepThreeKernel.get(), 0, sizeof(bucketsTwo.get()), &unmove(bucketsTwo.get())) != CL_SUCCESS || clSetKernelArg(stepThreeKernel.get(), 1, sizeof(numberOfEdgesPerBucketTwo.get()), &unmove(numberOfEdgesPerBucketTwo.get())) != CL_SUCCESS || clSetKernelArg(stepThreeKernel.get(), 2, sizeof(bucketsOne.get()), &unmove(bucketsOne.get())) != CL_SUCCESS || clSetKernelArg(stepThreeKernel.get(), 3, sizeof(numberOfEdgesPerBucketOne.get()), &unmove(numberOfEdgesPerBucketOne.get())) != CL_SUCCESS || clSetKernelArg(stepFourKernel.get(), 0, sizeof(bucketsOne.get()), &unmove(bucketsOne.get())) != CL_SUCCESS || clSetKernelArg(stepFourKernel.get(), 1, sizeof(numberOfEdgesPerBucketOne.get()), &unmove(numberOfEdgesPerBucketOne.get())) != CL_SUCCESS || clSetKernelArg(stepFourKernel.get(), 2, sizeof(bucketsTwo.get()), &unmove(bucketsTwo.get())) != CL_SUCCESS || clSetKernelArg(stepFourKernel.get(), 3, sizeof(numberOfEdgesPerBucketTwo.get()), &unmove(numberOfEdgesPerBucketTwo.get())) != CL_SUCCESS || clSetKernelArg(stepSixKernel.get(), 0, (TRIMMING_ROUNDS % 2) ? sizeof(bucketsTwo.get()) : sizeof(bucketsOne.get()), &unmove((TRIMMING_ROUNDS % 2) ? bucketsTwo.get() : bucketsOne.get())) != CL_SUCCESS || clSetKernelArg(stepSixKernel.get(), 1, (TRIMMING_ROUNDS % 2) ? sizeof(numberOfEdgesPerBucketTwo.get()) : sizeof(numberOfEdgesPerBucketOne.get()), &unmove((TRIMMING_ROUNDS % 2) ? numberOfEdgesPerBucketTwo.get() : numberOfEdgesPerBucketOne.get())) != CL_SUCCESS) {
		
			// Display message
			cout << "Setting program's arguments on the GPU failed." << endl;
			
			// Return false
			return false;
		}
		
		// Check if using buckets one second part
		if(bucketsOneNumberOfBuckets != MEAN_TRIMMING_NUMBER_OF_BUCKETS) {
		
			// Check if setting program's unchanging arguments failed
			if(clSetKernelArg(stepOneKernel.get(), 3, sizeof(bucketsOneSecondPart.get()), &unmove(bucketsOneSecondPart.get())) != CL_SUCCESS || clSetKernelArg(stepTwoKernel.get(), 5, sizeof(bucketsOneSecondPart.get()), &unmove(bucketsOneSecondPart.get())) != CL_SUCCESS) {
			
				// Display message
				cout << "Setting program's arguments on the GPU failed." << endl;
				
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
		Event firstCommandEvent;
		if(clEnqueueFillBuffer(commandQueue.get(), numberOfEdgesPerBucketOne.get(), (const cl_ulong[]){0}, (MEAN_TRIMMING_NUMBER_OF_BUCKETS == 1) ? sizeof(cl_uint) : sizeof(cl_ulong), 0, MEAN_TRIMMING_NUMBER_OF_BUCKETS * sizeof(cl_uint), 0, nullptr, firstCommandEvent.getAddress()) != CL_SUCCESS) {
		
			// Display message
			cout << "Preparing program's arguments on the GPU failed." << endl;
			
			// Return false
			return false;
		}
		
		// Get SipHash keys from job's header and nonce
		uint64_t __attribute__((vector_size(sizeof(uint64_t) * SIPHASH_KEYS_SIZE))) sipHashKeysOne;
		uint64_t heightOne = jobHeight;
		uint64_t idOne = jobId;
		uint64_t nonceOne = jobNonce++;
		blake2b(sipHashKeysOne, jobHeader, nonceOne);
		
		// Check if setting program's SipHash keys argument failed
		if(clSetKernelArg(stepOneKernel.get(), 2, sizeof(sipHashKeysOne), &sipHashKeysOne) != CL_SUCCESS) {
		
			// Display message
			cout << "Setting program's arguments on the GPU failed." << endl;
			
			// Return false
			return false;
		}
		
		// Check if queuing running step one on the device failed
		if(clEnqueueNDRangeKernel(commandQueue.get(), stepOneKernel.get(), 1, nullptr, &totalNumberOfWorkItems[0], &workItemsPerWorkGroup[0], 0, nullptr, nullptr) != CL_SUCCESS) {
		
			// Display message
			cout << "Running program on the GPU failed." << endl;
			
			// Return false
			return false;
		}
		
		// Check if queuing clearing number of edges per bucket two on the device failed
		if(clEnqueueFillBuffer(commandQueue.get(), numberOfEdgesPerBucketTwo.get(), (const cl_ulong[]){0}, (MEAN_TRIMMING_NUMBER_OF_BUCKETS == 1) ? sizeof(cl_uint) : sizeof(cl_ulong), 0, MEAN_TRIMMING_NUMBER_OF_BUCKETS * sizeof(cl_uint), 0, nullptr, nullptr) != CL_SUCCESS) {
		
			// Display message
			cout << "Preparing program's arguments on the GPU failed." << endl;
			
			// Return false
			return false;
		}
		
		// Check if setting program's SipHash keys argument failed
		if(clSetKernelArg(stepTwoKernel.get(), 4, sizeof(sipHashKeysOne), &sipHashKeysOne) != CL_SUCCESS) {
		
			// Display message
			cout << "Setting program's arguments on the GPU failed." << endl;
			
			// Return false
			return false;
		}
		
		// Check if queuing running step two on the device failed
		if(clEnqueueNDRangeKernel(commandQueue.get(), stepTwoKernel.get(), 1, nullptr, &totalNumberOfWorkItems[1], &workItemsPerWorkGroup[1], 0, nullptr, nullptr) != CL_SUCCESS) {
		
			// Display message
			cout << "Running program on the GPU failed." << endl;
			
			// Return false
			return false;
		}
		
		// Check if trimming more than one round
		if(TRIMMING_ROUNDS > 1) {
		
			// Check if queuing clearing number of edges per bucket one on the device failed
			if(clEnqueueFillBuffer(commandQueue.get(), numberOfEdgesPerBucketOne.get(), (const cl_ulong[]){0}, (MEAN_TRIMMING_NUMBER_OF_BUCKETS == 1) ? sizeof(cl_uint) : sizeof(cl_ulong), 0, MEAN_TRIMMING_NUMBER_OF_BUCKETS * sizeof(cl_uint), 0, nullptr, nullptr) != CL_SUCCESS) {
			
				// Display message
				cout << "Preparing program's arguments on the GPU failed." << endl;
				
				// Return false
				return false;
			}
			
			// Check if setting program's SipHash keys argument failed
			if(clSetKernelArg(stepThreeKernel.get(), 4, sizeof(sipHashKeysOne), &sipHashKeysOne) != CL_SUCCESS) {
			
				// Display message
				cout << "Setting program's arguments on the GPU failed." << endl;
				
				// Return false
				return false;
			}
			
			// Check if queuing running step three on the device failed
			if(clEnqueueNDRangeKernel(commandQueue.get(), stepThreeKernel.get(), 1, nullptr, &totalNumberOfWorkItems[2], &workItemsPerWorkGroup[2], 0, nullptr, nullptr) != CL_SUCCESS) {
			
				// Display message
				cout << "Running program on the GPU failed." << endl;
				
				// Return false
				return false;
			}
			
			// Check if trimming more than two rounds
			if(TRIMMING_ROUNDS > 2) {
			
				// Check if queuing clearing number of edges per bucket two on the device failed
				if(clEnqueueFillBuffer(commandQueue.get(), numberOfEdgesPerBucketTwo.get(), (const cl_ulong[]){0}, (MEAN_TRIMMING_NUMBER_OF_BUCKETS == 1) ? sizeof(cl_uint) : sizeof(cl_ulong), 0, MEAN_TRIMMING_NUMBER_OF_BUCKETS * sizeof(cl_uint), 0, nullptr, nullptr) != CL_SUCCESS) {
				
					// Display message
					cout << "Preparing program's arguments on the GPU failed." << endl;
					
					// Return false
					return false;
				}
				
				// Check if setting program's SipHash keys argument failed
				if(clSetKernelArg(stepFourKernel.get(), 4, sizeof(sipHashKeysOne), &sipHashKeysOne) != CL_SUCCESS) {
				
					// Display message
					cout << "Setting program's arguments on the GPU failed." << endl;
					
					// Return false
					return false;
				}
				
				// Check if queuing running step four on the device failed
				if(clEnqueueNDRangeKernel(commandQueue.get(), stepFourKernel.get(), 1, nullptr, &totalNumberOfWorkItems[3], &workItemsPerWorkGroup[3], 0, nullptr, nullptr) != CL_SUCCESS) {
				
					// Display message
					cout << "Running program on the GPU failed." << endl;
					
					// Return false
					return false;
				}
				
				// Go through all remaining trimming rounds
				for(unsigned int i = 3; i < TRIMMING_ROUNDS; ++i) {
				
					// Check if queuing clearing number of edges per bucket on the device failed
					if(clEnqueueFillBuffer(commandQueue.get(), (i % 2) ? numberOfEdgesPerBucketOne.get() : numberOfEdgesPerBucketTwo.get(), (const cl_ulong[]){0}, (MEAN_TRIMMING_NUMBER_OF_BUCKETS == 1) ? sizeof(cl_uint) : sizeof(cl_ulong), 0, MEAN_TRIMMING_NUMBER_OF_BUCKETS * sizeof(cl_uint), 0, nullptr, nullptr) != CL_SUCCESS) {
					
						// Display message
						cout << "Preparing program's arguments on the GPU failed." << endl;
						
						// Return false
						return false;
					}
					
					// Check if setting program's source buckets, number of edges per source bucket, destination buckets, or number of edges per destination bucket arguments failed
					if(clSetKernelArg(stepFiveKernel.get(), 0, (i % 2) ? sizeof(bucketsTwo.get()) : sizeof(bucketsOne.get()), &unmove((i % 2) ? bucketsTwo.get() : bucketsOne.get())) != CL_SUCCESS || clSetKernelArg(stepFiveKernel.get(), 1, (i % 2) ? sizeof(numberOfEdgesPerBucketTwo.get()) : sizeof(numberOfEdgesPerBucketOne.get()), &unmove((i % 2) ? numberOfEdgesPerBucketTwo.get() : numberOfEdgesPerBucketOne.get())) != CL_SUCCESS || clSetKernelArg(stepFiveKernel.get(), 2, (i % 2) ? sizeof(bucketsOne.get()) : sizeof(bucketsTwo.get()), &unmove((i % 2) ? bucketsOne.get() : bucketsTwo.get())) != CL_SUCCESS || clSetKernelArg(stepFiveKernel.get(), 3, (i % 2) ? sizeof(numberOfEdgesPerBucketOne.get()) : sizeof(numberOfEdgesPerBucketTwo.get()), &unmove((i % 2) ? numberOfEdgesPerBucketOne.get() : numberOfEdgesPerBucketTwo.get())) != CL_SUCCESS) {
					
						// Display message
						cout << "Setting program's arguments on the GPU failed." << endl;
						
						// Return false
						return false;
					}
					
					// Check if queuing running step five on the device failed
					if(clEnqueueNDRangeKernel(commandQueue.get(), stepFiveKernel.get(), 1, nullptr, &totalNumberOfWorkItems[4], &workItemsPerWorkGroup[4], 0, nullptr, nullptr) != CL_SUCCESS) {
					
						// Display message
						cout << "Running program on the GPU failed." << endl;
						
						// Return false
						return false;
					}
				}
			}
		}
		
		// Check if queuing clearing the beginning of remaining edges one on the device failed
		if(clEnqueueFillBuffer(commandQueue.get(), remainingEdgesOne.get(), (const cl_uint[]){0}, sizeof(cl_uint), 0, sizeof(cl_uint), 0, nullptr, nullptr) != CL_SUCCESS) {
		
			// Display message
			cout << "Preparing program's arguments on the GPU failed." << endl;
			
			// Return false
			return false;
		}
		
		// Check if setting program's remaining edges argument failed
		if(clSetKernelArg(stepSixKernel.get(), 2, sizeof(remainingEdgesOne.get()), &unmove(remainingEdgesOne.get())) != CL_SUCCESS) {
		
			// Display message
			cout << "Setting program's arguments on the GPU failed." << endl;
			
			// Return false
			return false;
		}
		
		// Check if trimming rounds is less than three
		if(TRIMMING_ROUNDS < 3) {
		
			// Check if setting program's SipHash keys argument failed
			if(clSetKernelArg(stepSixKernel.get(), 3, sizeof(sipHashKeysOne), &sipHashKeysOne) != CL_SUCCESS) {
			
				// Display message
				cout << "Setting program's arguments on the GPU failed." << endl;
				
				// Return false
				return false;
			}
		}
		
		// Check if queuing running step six on the device failed
		if(clEnqueueNDRangeKernel(commandQueue.get(), stepSixKernel.get(), 1, nullptr, &totalNumberOfWorkItems[5], &workItemsPerWorkGroup[5], 0, nullptr, nullptr) != CL_SUCCESS) {
		
			// Display message
			cout << "Running program on the GPU failed." << endl;
			
			// Return false
			return false;
		}
		
		// Check if queuing map result failed
		Event mapEvent;
		resultOne = reinterpret_cast<uint32_t *>(clEnqueueMapBuffer(commandQueue.get(), remainingEdgesOne.get(), CL_FALSE, CL_MAP_READ, 0, sizeof(cl_uint) + static_cast<uint64_t>(MAX_NUMBER_OF_EDGES_AFTER_TRIMMING) * EDGE_NUMBER_OF_COMPONENTS * sizeof(cl_uint), 0, nullptr, mapEvent.getAddress(), nullptr));
		if(!resultOne) {
		
			// Display message
			cout << "Getting result from the GPU failed." << endl;
			
			// Return false
			return false;
		}
		
		// Check if closing
		if(closing) {
			
			// Return true
			return true;
		}
		
		// Check if waiting for map event to finish failed
		if(clWaitForEvents(1, mapEvent.getAddress()) != CL_SUCCESS) {
		
			// Display message
			cout << "Getting result from the GPU failed." << endl;
			
			// Return false
			return false;
		}
		
		// Check if getting trimming time failed
		cl_ulong startTime;
		cl_ulong endTime;
		if(clGetEventProfilingInfo(firstCommandEvent.get(), CL_PROFILING_COMMAND_QUEUED, sizeof(startTime), &startTime, nullptr) != CL_SUCCESS || clGetEventProfilingInfo(mapEvent.get(), CL_PROFILING_COMMAND_END, sizeof(endTime), &endTime, nullptr) != CL_SUCCESS) {
		
			// Display message
			cout << "Getting GPU trimming time failed." << endl;
			
			// Return false
			return false;
		}
		
		// Display message
		cout << "\tTrimming time:\t " << static_cast<chrono::duration<double>>(static_cast<chrono::nanoseconds>(endTime - startTime)).count() << " second(s)" << endl;
		
		// Check if queuing clearing number of edges per bucket one on the device failed
		firstCommandEvent.free();
		if(clEnqueueFillBuffer(commandQueue.get(), numberOfEdgesPerBucketOne.get(), (const cl_ulong[]){0}, (MEAN_TRIMMING_NUMBER_OF_BUCKETS == 1) ? sizeof(cl_uint) : sizeof(cl_ulong), 0, MEAN_TRIMMING_NUMBER_OF_BUCKETS * sizeof(cl_uint), 0, nullptr, firstCommandEvent.getAddress()) != CL_SUCCESS) {
		
			// Display message
			cout << "Preparing program's arguments on the GPU failed." << endl;
			
			// Return false
			return false;
		}
		
		// Get SipHash keys from job's header and nonce
		uint64_t __attribute__((vector_size(sizeof(uint64_t) * SIPHASH_KEYS_SIZE))) sipHashKeysTwo;
		uint64_t heightTwo = jobHeight;
		uint64_t idTwo = jobId;
		uint64_t nonceTwo = jobNonce++;
		blake2b(sipHashKeysTwo, jobHeader, nonceTwo);
		
		// Check if setting program's SipHash keys argument failed
		if(clSetKernelArg(stepOneKernel.get(), 2, sizeof(sipHashKeysTwo), &sipHashKeysTwo) != CL_SUCCESS) {
		
			// Display message
			cout << "Setting program's arguments on the GPU failed." << endl;
			
			// Return false
			return false;
		}
		
		// Check if queuing running step one on the device failed
		if(clEnqueueNDRangeKernel(commandQueue.get(), stepOneKernel.get(), 1, nullptr, &totalNumberOfWorkItems[0], &workItemsPerWorkGroup[0], 0, nullptr, nullptr) != CL_SUCCESS) {
		
			// Display message
			cout << "Running program on the GPU failed." << endl;
			
			// Return false
			return false;
		}
		
		// Check if queuing clearing number of edges per bucket two on the device failed
		if(clEnqueueFillBuffer(commandQueue.get(), numberOfEdgesPerBucketTwo.get(), (const cl_ulong[]){0}, (MEAN_TRIMMING_NUMBER_OF_BUCKETS == 1) ? sizeof(cl_uint) : sizeof(cl_ulong), 0, MEAN_TRIMMING_NUMBER_OF_BUCKETS * sizeof(cl_uint), 0, nullptr, nullptr) != CL_SUCCESS) {
		
			// Display message
			cout << "Preparing program's arguments on the GPU failed." << endl;
			
			// Return false
			return false;
		}
		
		// Check if setting program's SipHash keys argument failed
		if(clSetKernelArg(stepTwoKernel.get(), 4, sizeof(sipHashKeysTwo), &sipHashKeysTwo) != CL_SUCCESS) {
		
			// Display message
			cout << "Setting program's arguments on the GPU failed." << endl;
			
			// Return false
			return false;
		}
		
		// Check if queuing running step two on the device failed
		if(clEnqueueNDRangeKernel(commandQueue.get(), stepTwoKernel.get(), 1, nullptr, &totalNumberOfWorkItems[1], &workItemsPerWorkGroup[1], 0, nullptr, nullptr) != CL_SUCCESS) {
		
			// Display message
			cout << "Running program on the GPU failed." << endl;
			
			// Return false
			return false;
		}
		
		// Check if trimming more than one round
		if(TRIMMING_ROUNDS > 1) {
		
			// Check if queuing clearing number of edges per bucket one on the device failed
			if(clEnqueueFillBuffer(commandQueue.get(), numberOfEdgesPerBucketOne.get(), (const cl_ulong[]){0}, (MEAN_TRIMMING_NUMBER_OF_BUCKETS == 1) ? sizeof(cl_uint) : sizeof(cl_ulong), 0, MEAN_TRIMMING_NUMBER_OF_BUCKETS * sizeof(cl_uint), 0, nullptr, nullptr) != CL_SUCCESS) {
			
				// Display message
				cout << "Preparing program's arguments on the GPU failed." << endl;
				
				// Return false
				return false;
			}
			
			// Check if setting program's SipHash keys argument failed
			if(clSetKernelArg(stepThreeKernel.get(), 4, sizeof(sipHashKeysTwo), &sipHashKeysTwo) != CL_SUCCESS) {
			
				// Display message
				cout << "Setting program's arguments on the GPU failed." << endl;
				
				// Return false
				return false;
			}
			
			// Check if queuing running step three on the device failed
			if(clEnqueueNDRangeKernel(commandQueue.get(), stepThreeKernel.get(), 1, nullptr, &totalNumberOfWorkItems[2], &workItemsPerWorkGroup[2], 0, nullptr, nullptr) != CL_SUCCESS) {
			
				// Display message
				cout << "Running program on the GPU failed." << endl;
				
				// Return false
				return false;
			}
			
			// Check if trimming more than two rounds
			if(TRIMMING_ROUNDS > 2) {
			
				// Check if queuing clearing number of edges per bucket two on the device failed
				if(clEnqueueFillBuffer(commandQueue.get(), numberOfEdgesPerBucketTwo.get(), (const cl_ulong[]){0}, (MEAN_TRIMMING_NUMBER_OF_BUCKETS == 1) ? sizeof(cl_uint) : sizeof(cl_ulong), 0, MEAN_TRIMMING_NUMBER_OF_BUCKETS * sizeof(cl_uint), 0, nullptr, nullptr) != CL_SUCCESS) {
				
					// Display message
					cout << "Preparing program's arguments on the GPU failed." << endl;
					
					// Return false
					return false;
				}
				
				// Check if setting program's SipHash keys argument failed
				if(clSetKernelArg(stepFourKernel.get(), 4, sizeof(sipHashKeysTwo), &sipHashKeysTwo) != CL_SUCCESS) {
				
					// Display message
					cout << "Setting program's arguments on the GPU failed." << endl;
					
					// Return false
					return false;
				}
				
				// Check if queuing running step four on the device failed
				if(clEnqueueNDRangeKernel(commandQueue.get(), stepFourKernel.get(), 1, nullptr, &totalNumberOfWorkItems[3], &workItemsPerWorkGroup[3], 0, nullptr, nullptr) != CL_SUCCESS) {
				
					// Display message
					cout << "Running program on the GPU failed." << endl;
					
					// Return false
					return false;
				}
				
				// Go through all remaining trimming rounds
				for(unsigned int i = 3; i < TRIMMING_ROUNDS; ++i) {
				
					// Check if queuing clearing number of edges per bucket on the device failed
					if(clEnqueueFillBuffer(commandQueue.get(), (i % 2) ? numberOfEdgesPerBucketOne.get() : numberOfEdgesPerBucketTwo.get(), (const cl_ulong[]){0}, (MEAN_TRIMMING_NUMBER_OF_BUCKETS == 1) ? sizeof(cl_uint) : sizeof(cl_ulong), 0, MEAN_TRIMMING_NUMBER_OF_BUCKETS * sizeof(cl_uint), 0, nullptr, nullptr) != CL_SUCCESS) {
					
						// Display message
						cout << "Preparing program's arguments on the GPU failed." << endl;
						
						// Return false
						return false;
					}
					
					// Check if setting program's source buckets, number of edges per source bucket, destination buckets, or number of edges per destination bucket arguments failed
					if(clSetKernelArg(stepFiveKernel.get(), 0, (i % 2) ? sizeof(bucketsTwo.get()) : sizeof(bucketsOne.get()), &unmove((i % 2) ? bucketsTwo.get() : bucketsOne.get())) != CL_SUCCESS || clSetKernelArg(stepFiveKernel.get(), 1, (i % 2) ? sizeof(numberOfEdgesPerBucketTwo.get()) : sizeof(numberOfEdgesPerBucketOne.get()), &unmove((i % 2) ? numberOfEdgesPerBucketTwo.get() : numberOfEdgesPerBucketOne.get())) != CL_SUCCESS || clSetKernelArg(stepFiveKernel.get(), 2, (i % 2) ? sizeof(bucketsOne.get()) : sizeof(bucketsTwo.get()), &unmove((i % 2) ? bucketsOne.get() : bucketsTwo.get())) != CL_SUCCESS || clSetKernelArg(stepFiveKernel.get(), 3, (i % 2) ? sizeof(numberOfEdgesPerBucketOne.get()) : sizeof(numberOfEdgesPerBucketTwo.get()), &unmove((i % 2) ? numberOfEdgesPerBucketOne.get() : numberOfEdgesPerBucketTwo.get())) != CL_SUCCESS) {
					
						// Display message
						cout << "Setting program's arguments on the GPU failed." << endl;
						
						// Return false
						return false;
					}
					
					// Check if queuing running step five on the device failed
					if(clEnqueueNDRangeKernel(commandQueue.get(), stepFiveKernel.get(), 1, nullptr, &totalNumberOfWorkItems[4], &workItemsPerWorkGroup[4], 0, nullptr, nullptr) != CL_SUCCESS) {
					
						// Display message
						cout << "Running program on the GPU failed." << endl;
						
						// Return false
						return false;
					}
				}
			}
		}
		
		// Check if queuing clearing the beginning of remaining edges two on the device failed
		if(clEnqueueFillBuffer(commandQueue.get(), remainingEdgesTwo.get(), (const cl_uint[]){0}, sizeof(cl_uint), 0, sizeof(cl_uint), 0, nullptr, nullptr) != CL_SUCCESS) {
		
			// Display message
			cout << "Preparing program's arguments on the GPU failed." << endl;
			
			// Return false
			return false;
		}
		
		// Check if setting program's remaining edge argument failed
		if(clSetKernelArg(stepSixKernel.get(), 2, sizeof(remainingEdgesTwo.get()), &unmove(remainingEdgesTwo.get())) != CL_SUCCESS) {
		
			// Display message
			cout << "Setting program's arguments on the GPU failed." << endl;
			
			// Return false
			return false;
		}
		
		// Check if trimming rounds is less than three
		if(TRIMMING_ROUNDS < 3) {
		
			// Check if setting program's SipHash keys argument failed
			if(clSetKernelArg(stepSixKernel.get(), 3, sizeof(sipHashKeysTwo), &sipHashKeysTwo) != CL_SUCCESS) {
			
				// Display message
				cout << "Setting program's arguments on the GPU failed." << endl;
				
				// Return false
				return false;
			}
		}
		
		// Check if queuing running step six on the device failed
		if(clEnqueueNDRangeKernel(commandQueue.get(), stepSixKernel.get(), 1, nullptr, &totalNumberOfWorkItems[5], &workItemsPerWorkGroup[5], 0, nullptr, nullptr) != CL_SUCCESS) {
		
			// Display message
			cout << "Running program on the GPU failed." << endl;
			
			// Return false
			return false;
		}
		
		// Check if queuing map result failed
		mapEvent.free();
		resultTwo = reinterpret_cast<uint32_t *>(clEnqueueMapBuffer(commandQueue.get(), remainingEdgesTwo.get(), CL_FALSE, CL_MAP_READ, 0, sizeof(cl_uint) + static_cast<uint64_t>(MAX_NUMBER_OF_EDGES_AFTER_TRIMMING) * EDGE_NUMBER_OF_COMPONENTS * sizeof(cl_uint), 0, nullptr, mapEvent.getAddress(), nullptr));
		if(!resultTwo) {
		
			// Display message
			cout << "Getting result from the GPU failed." << endl;
			
			// Return false
			return false;
		}
		
		// Perform commands in queue
		clFlush(commandQueue.get());
		
		// Trimming finished
		trimmingFinished(resultOne, sipHashKeysOne, heightOne, idOne, nonceOne);
		
		// Check if queuing unmap result failed
		if(clEnqueueUnmapMemObject(commandQueue.get(), remainingEdgesOne.get(), reinterpret_cast<void *>(resultOne), 0, nullptr, nullptr) != CL_SUCCESS) {
		
			// Display message
			cout << "Getting result from the GPU failed." << endl;
			
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
				cout << "Getting result from the GPU failed." << endl;
				
				// Return false
				return false;
			}
			
			// Check if getting trimming time failed
			if(clGetEventProfilingInfo(firstCommandEvent.get(), CL_PROFILING_COMMAND_QUEUED, sizeof(startTime), &startTime, nullptr) != CL_SUCCESS || clGetEventProfilingInfo(mapEvent.get(), CL_PROFILING_COMMAND_END, sizeof(endTime), &endTime, nullptr) != CL_SUCCESS) {
			
				// Display message
				cout << "Getting GPU trimming time failed." << endl;
				
				// Return false
				return false;
			}
			
			// Display message
			cout << "\tTrimming time:\t " << static_cast<chrono::duration<double>>(static_cast<chrono::nanoseconds>(endTime - startTime)).count() << " second(s)" << endl;
			
			// Check if queuing clearing number of edges per bucket one on the device failed
			firstCommandEvent.free();
			if(clEnqueueFillBuffer(commandQueue.get(), numberOfEdgesPerBucketOne.get(), (const cl_ulong[]){0}, (MEAN_TRIMMING_NUMBER_OF_BUCKETS == 1) ? sizeof(cl_uint) : sizeof(cl_ulong), 0, MEAN_TRIMMING_NUMBER_OF_BUCKETS * sizeof(cl_uint), 0, nullptr, firstCommandEvent.getAddress()) != CL_SUCCESS) {
			
				// Display message
				cout << "Preparing program's arguments on the GPU failed." << endl;
				
				// Return false
				return false;
			}
			
			// Get SipHash keys from job's header and nonce
			heightOne = jobHeight;
			idOne = jobId;
			nonceOne = jobNonce++;
			blake2b(sipHashKeysOne, jobHeader, nonceOne);
			
			// Check if setting program's SipHash keys argument failed
			if(clSetKernelArg(stepOneKernel.get(), 2, sizeof(sipHashKeysOne), &sipHashKeysOne) != CL_SUCCESS) {
			
				// Display message
				cout << "Setting program's arguments on the GPU failed." << endl;
				
				// Return false
				return false;
			}
			
			// Check if queuing running step one on the device failed
			if(clEnqueueNDRangeKernel(commandQueue.get(), stepOneKernel.get(), 1, nullptr, &totalNumberOfWorkItems[0], &workItemsPerWorkGroup[0], 0, nullptr, nullptr) != CL_SUCCESS) {
			
				// Display message
				cout << "Running program on the GPU failed." << endl;
				
				// Return false
				return false;
			}
			
			// Check if queuing clearing number of edges per bucket two on the device failed
			if(clEnqueueFillBuffer(commandQueue.get(), numberOfEdgesPerBucketTwo.get(), (const cl_ulong[]){0}, (MEAN_TRIMMING_NUMBER_OF_BUCKETS == 1) ? sizeof(cl_uint) : sizeof(cl_ulong), 0, MEAN_TRIMMING_NUMBER_OF_BUCKETS * sizeof(cl_uint), 0, nullptr, nullptr) != CL_SUCCESS) {
			
				// Display message
				cout << "Preparing program's arguments on the GPU failed." << endl;
				
				// Return false
				return false;
			}
			
			// Check if setting program's SipHash keys argument failed
			if(clSetKernelArg(stepTwoKernel.get(), 4, sizeof(sipHashKeysOne), &sipHashKeysOne) != CL_SUCCESS) {
			
				// Display message
				cout << "Setting program's arguments on the GPU failed." << endl;
				
				// Return false
				return false;
			}
			
			// Check if queuing running step two on the device failed
			if(clEnqueueNDRangeKernel(commandQueue.get(), stepTwoKernel.get(), 1, nullptr, &totalNumberOfWorkItems[1], &workItemsPerWorkGroup[1], 0, nullptr, nullptr) != CL_SUCCESS) {
			
				// Display message
				cout << "Running program on the GPU failed." << endl;
				
				// Return false
				return false;
			}
			
			// Check if trimming more than one round
			if(TRIMMING_ROUNDS > 1) {
			
				// Check if queuing clearing number of edges per bucket one on the device failed
				if(clEnqueueFillBuffer(commandQueue.get(), numberOfEdgesPerBucketOne.get(), (const cl_ulong[]){0}, (MEAN_TRIMMING_NUMBER_OF_BUCKETS == 1) ? sizeof(cl_uint) : sizeof(cl_ulong), 0, MEAN_TRIMMING_NUMBER_OF_BUCKETS * sizeof(cl_uint), 0, nullptr, nullptr) != CL_SUCCESS) {
				
					// Display message
					cout << "Preparing program's arguments on the GPU failed." << endl;
					
					// Return false
					return false;
				}
				
				// Check if setting program's SipHash keys argument failed
				if(clSetKernelArg(stepThreeKernel.get(), 4, sizeof(sipHashKeysOne), &sipHashKeysOne) != CL_SUCCESS) {
				
					// Display message
					cout << "Setting program's arguments on the GPU failed." << endl;
					
					// Return false
					return false;
				}
				
				// Check if queuing running step three on the device failed
				if(clEnqueueNDRangeKernel(commandQueue.get(), stepThreeKernel.get(), 1, nullptr, &totalNumberOfWorkItems[2], &workItemsPerWorkGroup[2], 0, nullptr, nullptr) != CL_SUCCESS) {
				
					// Display message
					cout << "Running program on the GPU failed." << endl;
					
					// Return false
					return false;
				}
				
				// Check if trimming more than two rounds
				if(TRIMMING_ROUNDS > 2) {
				
					// Check if queuing clearing number of edges per bucket two on the device failed
					if(clEnqueueFillBuffer(commandQueue.get(), numberOfEdgesPerBucketTwo.get(), (const cl_ulong[]){0}, (MEAN_TRIMMING_NUMBER_OF_BUCKETS == 1) ? sizeof(cl_uint) : sizeof(cl_ulong), 0, MEAN_TRIMMING_NUMBER_OF_BUCKETS * sizeof(cl_uint), 0, nullptr, nullptr) != CL_SUCCESS) {
					
						// Display message
						cout << "Preparing program's arguments on the GPU failed." << endl;
						
						// Return false
						return false;
					}
					
					// Check if setting program's SipHash keys argument failed
					if(clSetKernelArg(stepFourKernel.get(), 4, sizeof(sipHashKeysOne), &sipHashKeysOne) != CL_SUCCESS) {
					
						// Display message
						cout << "Setting program's arguments on the GPU failed." << endl;
						
						// Return false
						return false;
					}
					
					// Check if queuing running step four on the device failed
					if(clEnqueueNDRangeKernel(commandQueue.get(), stepFourKernel.get(), 1, nullptr, &totalNumberOfWorkItems[3], &workItemsPerWorkGroup[3], 0, nullptr, nullptr) != CL_SUCCESS) {
					
						// Display message
						cout << "Running program on the GPU failed." << endl;
						
						// Return false
						return false;
					}
					
					// Go through all remaining trimming rounds
					for(unsigned int i = 3; i < TRIMMING_ROUNDS; ++i) {
					
						// Check if queuing clearing number of edges per bucket on the device failed
						if(clEnqueueFillBuffer(commandQueue.get(), (i % 2) ? numberOfEdgesPerBucketOne.get() : numberOfEdgesPerBucketTwo.get(), (const cl_ulong[]){0}, (MEAN_TRIMMING_NUMBER_OF_BUCKETS == 1) ? sizeof(cl_uint) : sizeof(cl_ulong), 0, MEAN_TRIMMING_NUMBER_OF_BUCKETS * sizeof(cl_uint), 0, nullptr, nullptr) != CL_SUCCESS) {
						
							// Display message
							cout << "Preparing program's arguments on the GPU failed." << endl;
							
							// Return false
							return false;
						}
						
						// Check if setting program's source buckets, number of edges per source bucket, destination buckets, or number of edges per destination bucket arguments failed
						if(clSetKernelArg(stepFiveKernel.get(), 0, (i % 2) ? sizeof(bucketsTwo.get()) : sizeof(bucketsOne.get()), &unmove((i % 2) ? bucketsTwo.get() : bucketsOne.get())) != CL_SUCCESS || clSetKernelArg(stepFiveKernel.get(), 1, (i % 2) ? sizeof(numberOfEdgesPerBucketTwo.get()) : sizeof(numberOfEdgesPerBucketOne.get()), &unmove((i % 2) ? numberOfEdgesPerBucketTwo.get() : numberOfEdgesPerBucketOne.get())) != CL_SUCCESS || clSetKernelArg(stepFiveKernel.get(), 2, (i % 2) ? sizeof(bucketsOne.get()) : sizeof(bucketsTwo.get()), &unmove((i % 2) ? bucketsOne.get() : bucketsTwo.get())) != CL_SUCCESS || clSetKernelArg(stepFiveKernel.get(), 3, (i % 2) ? sizeof(numberOfEdgesPerBucketOne.get()) : sizeof(numberOfEdgesPerBucketTwo.get()), &unmove((i % 2) ? numberOfEdgesPerBucketOne.get() : numberOfEdgesPerBucketTwo.get())) != CL_SUCCESS) {
						
							// Display message
							cout << "Setting program's arguments on the GPU failed." << endl;
							
							// Return false
							return false;
						}
						
						// Check if queuing running step five on the device failed
						if(clEnqueueNDRangeKernel(commandQueue.get(), stepFiveKernel.get(), 1, nullptr, &totalNumberOfWorkItems[4], &workItemsPerWorkGroup[4], 0, nullptr, nullptr) != CL_SUCCESS) {
						
							// Display message
							cout << "Running program on the GPU failed." << endl;
							
							// Return false
							return false;
						}
					}
				}
			}
			
			// Check if queuing clearing the beginning of remaining edges one on the device failed
			if(clEnqueueFillBuffer(commandQueue.get(), remainingEdgesOne.get(), (const cl_uint[]){0}, sizeof(cl_uint), 0, sizeof(cl_uint), 0, nullptr, nullptr) != CL_SUCCESS) {
			
				// Display message
				cout << "Preparing program's arguments on the GPU failed." << endl;
				
				// Return false
				return false;
			}
			
			// Check if setting program's remaining edge argument failed
			if(clSetKernelArg(stepSixKernel.get(), 2, sizeof(remainingEdgesOne.get()), &unmove(remainingEdgesOne.get())) != CL_SUCCESS) {
			
				// Display message
				cout << "Setting program's arguments on the GPU failed." << endl;
				
				// Return false
				return false;
			}
			
			// Check if trimming rounds is less than three
			if(TRIMMING_ROUNDS < 3) {
			
				// Check if setting program's SipHash keys argument failed
				if(clSetKernelArg(stepSixKernel.get(), 3, sizeof(sipHashKeysOne), &sipHashKeysOne) != CL_SUCCESS) {
				
					// Display message
					cout << "Setting program's arguments on the GPU failed." << endl;
					
					// Return false
					return false;
				}
			}
			
			// Check if queuing running step six on the device failed
			if(clEnqueueNDRangeKernel(commandQueue.get(), stepSixKernel.get(), 1, nullptr, &totalNumberOfWorkItems[5], &workItemsPerWorkGroup[5], 0, nullptr, nullptr) != CL_SUCCESS) {
			
				// Display message
				cout << "Running program on the GPU failed." << endl;
				
				// Return false
				return false;
			}
			
			// Check if queuing map result failed
			mapEvent.free();
			resultOne = reinterpret_cast<uint32_t *>(clEnqueueMapBuffer(commandQueue.get(), remainingEdgesOne.get(), CL_FALSE, CL_MAP_READ, 0, sizeof(cl_uint) + static_cast<uint64_t>(MAX_NUMBER_OF_EDGES_AFTER_TRIMMING) * EDGE_NUMBER_OF_COMPONENTS * sizeof(cl_uint), 0, nullptr, mapEvent.getAddress(), nullptr));
			if(!resultOne) {
			
				// Display message
				cout << "Getting result from the GPU failed." << endl;
				
				// Return false
				return false;
			}
			
			// Perform commands in queue
			clFlush(commandQueue.get());
			
			// Trimming finished
			trimmingFinished(resultTwo, sipHashKeysTwo, heightTwo, idTwo, nonceTwo);
			
			// Check if queuing unmap result failed
			if(clEnqueueUnmapMemObject(commandQueue.get(), remainingEdgesTwo.get(), reinterpret_cast<void *>(resultTwo), 0, nullptr, nullptr) != CL_SUCCESS) {
			
				// Display message
				cout << "Getting result from the GPU failed." << endl;
				
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
				cout << "Getting result from the GPU failed." << endl;
				
				// Return false
				return false;
			}
			
			// Check if getting trimming time failed
			if(clGetEventProfilingInfo(firstCommandEvent.get(), CL_PROFILING_COMMAND_QUEUED, sizeof(startTime), &startTime, nullptr) != CL_SUCCESS || clGetEventProfilingInfo(mapEvent.get(), CL_PROFILING_COMMAND_END, sizeof(endTime), &endTime, nullptr) != CL_SUCCESS) {
			
				// Display message
				cout << "Getting GPU trimming time failed." << endl;
				
				// Return false
				return false;
			}
			
			// Display message
			cout << "\tTrimming time:\t " << static_cast<chrono::duration<double>>(static_cast<chrono::nanoseconds>(endTime - startTime)).count() << " second(s)" << endl;
			
			// Check if queuing clearing number of edges per bucket one on the device failed
			firstCommandEvent.free();
			if(clEnqueueFillBuffer(commandQueue.get(), numberOfEdgesPerBucketOne.get(), (const cl_ulong[]){0}, (MEAN_TRIMMING_NUMBER_OF_BUCKETS == 1) ? sizeof(cl_uint) : sizeof(cl_ulong), 0, MEAN_TRIMMING_NUMBER_OF_BUCKETS * sizeof(cl_uint), 0, nullptr, firstCommandEvent.getAddress()) != CL_SUCCESS) {
			
				// Display message
				cout << "Preparing program's arguments on the GPU failed." << endl;
				
				// Return false
				return false;
			}
			
			// Get SipHash keys from job's header and nonce
			heightTwo = jobHeight;
			idTwo = jobId;
			nonceTwo = jobNonce++;
			blake2b(sipHashKeysTwo, jobHeader, nonceTwo);
			
			// Check if setting program's SipHash keys argument failed
			if(clSetKernelArg(stepOneKernel.get(), 2, sizeof(sipHashKeysTwo), &sipHashKeysTwo) != CL_SUCCESS) {
			
				// Display message
				cout << "Setting program's arguments on the GPU failed." << endl;
				
				// Return false
				return false;
			}
			
			// Check if queuing running step one on the device failed
			if(clEnqueueNDRangeKernel(commandQueue.get(), stepOneKernel.get(), 1, nullptr, &totalNumberOfWorkItems[0], &workItemsPerWorkGroup[0], 0, nullptr, nullptr) != CL_SUCCESS) {
			
				// Display message
				cout << "Running program on the GPU failed." << endl;
				
				// Return false
				return false;
			}
			
			// Check if queuing clearing number of edges per bucket two on the device failed
			if(clEnqueueFillBuffer(commandQueue.get(), numberOfEdgesPerBucketTwo.get(), (const cl_ulong[]){0}, (MEAN_TRIMMING_NUMBER_OF_BUCKETS == 1) ? sizeof(cl_uint) : sizeof(cl_ulong), 0, MEAN_TRIMMING_NUMBER_OF_BUCKETS * sizeof(cl_uint), 0, nullptr, nullptr) != CL_SUCCESS) {
			
				// Display message
				cout << "Preparing program's arguments on the GPU failed." << endl;
				
				// Return false
				return false;
			}
			
			// Check if setting program's SipHash keys argument failed
			if(clSetKernelArg(stepTwoKernel.get(), 4, sizeof(sipHashKeysTwo), &sipHashKeysTwo) != CL_SUCCESS) {
			
				// Display message
				cout << "Setting program's arguments on the GPU failed." << endl;
				
				// Return false
				return false;
			}
			
			// Check if queuing running step two on the device failed
			if(clEnqueueNDRangeKernel(commandQueue.get(), stepTwoKernel.get(), 1, nullptr, &totalNumberOfWorkItems[1], &workItemsPerWorkGroup[1], 0, nullptr, nullptr) != CL_SUCCESS) {
			
				// Display message
				cout << "Running program on the GPU failed." << endl;
				
				// Return false
				return false;
			}
			
			// Check if trimming more than one round
			if(TRIMMING_ROUNDS > 1) {
			
				// Check if queuing clearing number of edges per bucket one on the device failed
				if(clEnqueueFillBuffer(commandQueue.get(), numberOfEdgesPerBucketOne.get(), (const cl_ulong[]){0}, (MEAN_TRIMMING_NUMBER_OF_BUCKETS == 1) ? sizeof(cl_uint) : sizeof(cl_ulong), 0, MEAN_TRIMMING_NUMBER_OF_BUCKETS * sizeof(cl_uint), 0, nullptr, nullptr) != CL_SUCCESS) {
				
					// Display message
					cout << "Preparing program's arguments on the GPU failed." << endl;
					
					// Return false
					return false;
				}
				
				// Check if setting program's SipHash keys argument failed
				if(clSetKernelArg(stepThreeKernel.get(), 4, sizeof(sipHashKeysTwo), &sipHashKeysTwo) != CL_SUCCESS) {
				
					// Display message
					cout << "Setting program's arguments on the GPU failed." << endl;
					
					// Return false
					return false;
				}
				
				// Check if queuing running step three on the device failed
				if(clEnqueueNDRangeKernel(commandQueue.get(), stepThreeKernel.get(), 1, nullptr, &totalNumberOfWorkItems[2], &workItemsPerWorkGroup[2], 0, nullptr, nullptr) != CL_SUCCESS) {
				
					// Display message
					cout << "Running program on the GPU failed." << endl;
					
					// Return false
					return false;
				}
				
				// Check if trimming more than two rounds
				if(TRIMMING_ROUNDS > 2) {
				
					// Check if queuing clearing number of edges per bucket two on the device failed
					if(clEnqueueFillBuffer(commandQueue.get(), numberOfEdgesPerBucketTwo.get(), (const cl_ulong[]){0}, (MEAN_TRIMMING_NUMBER_OF_BUCKETS == 1) ? sizeof(cl_uint) : sizeof(cl_ulong), 0, MEAN_TRIMMING_NUMBER_OF_BUCKETS * sizeof(cl_uint), 0, nullptr, nullptr) != CL_SUCCESS) {
					
						// Display message
						cout << "Preparing program's arguments on the GPU failed." << endl;
						
						// Return false
						return false;
					}
					
					// Check if setting program's SipHash keys argument failed
					if(clSetKernelArg(stepFourKernel.get(), 4, sizeof(sipHashKeysTwo), &sipHashKeysTwo) != CL_SUCCESS) {
					
						// Display message
						cout << "Setting program's arguments on the GPU failed." << endl;
						
						// Return false
						return false;
					}
					
					// Check if queuing running step four on the device failed
					if(clEnqueueNDRangeKernel(commandQueue.get(), stepFourKernel.get(), 1, nullptr, &totalNumberOfWorkItems[3], &workItemsPerWorkGroup[3], 0, nullptr, nullptr) != CL_SUCCESS) {
					
						// Display message
						cout << "Running program on the GPU failed." << endl;
						
						// Return false
						return false;
					}
					
					// Go through all remaining trimming rounds
					for(unsigned int i = 3; i < TRIMMING_ROUNDS; ++i) {
					
						// Check if queuing clearing number of edges per bucket on the device failed
						if(clEnqueueFillBuffer(commandQueue.get(), (i % 2) ? numberOfEdgesPerBucketOne.get() : numberOfEdgesPerBucketTwo.get(), (const cl_ulong[]){0}, (MEAN_TRIMMING_NUMBER_OF_BUCKETS == 1) ? sizeof(cl_uint) : sizeof(cl_ulong), 0, MEAN_TRIMMING_NUMBER_OF_BUCKETS * sizeof(cl_uint), 0, nullptr, nullptr) != CL_SUCCESS) {
						
							// Display message
							cout << "Preparing program's arguments on the GPU failed." << endl;
							
							// Return false
							return false;
						}
						
						// Check if setting program's source buckets, number of edges per source bucket, destination buckets, or number of edges per destination bucket arguments failed
						if(clSetKernelArg(stepFiveKernel.get(), 0, (i % 2) ? sizeof(bucketsTwo.get()) : sizeof(bucketsOne.get()), &unmove((i % 2) ? bucketsTwo.get() : bucketsOne.get())) != CL_SUCCESS || clSetKernelArg(stepFiveKernel.get(), 1, (i % 2) ? sizeof(numberOfEdgesPerBucketTwo.get()) : sizeof(numberOfEdgesPerBucketOne.get()), &unmove((i % 2) ? numberOfEdgesPerBucketTwo.get() : numberOfEdgesPerBucketOne.get())) != CL_SUCCESS || clSetKernelArg(stepFiveKernel.get(), 2, (i % 2) ? sizeof(bucketsOne.get()) : sizeof(bucketsTwo.get()), &unmove((i % 2) ? bucketsOne.get() : bucketsTwo.get())) != CL_SUCCESS || clSetKernelArg(stepFiveKernel.get(), 3, (i % 2) ? sizeof(numberOfEdgesPerBucketOne.get()) : sizeof(numberOfEdgesPerBucketTwo.get()), &unmove((i % 2) ? numberOfEdgesPerBucketOne.get() : numberOfEdgesPerBucketTwo.get())) != CL_SUCCESS) {
						
							// Display message
							cout << "Setting program's arguments on the GPU failed." << endl;
							
							// Return false
							return false;
						}
						
						// Check if queuing running step five on the device failed
						if(clEnqueueNDRangeKernel(commandQueue.get(), stepFiveKernel.get(), 1, nullptr, &totalNumberOfWorkItems[4], &workItemsPerWorkGroup[4], 0, nullptr, nullptr) != CL_SUCCESS) {
						
							// Display message
							cout << "Running program on the GPU failed." << endl;
							
							// Return false
							return false;
						}
					}
				}
			}
			
			// Check if queuing clearing the beginning of remaining edges two on the device failed
			if(clEnqueueFillBuffer(commandQueue.get(), remainingEdgesTwo.get(), (const cl_uint[]){0}, sizeof(cl_uint), 0, sizeof(cl_uint), 0, nullptr, nullptr) != CL_SUCCESS) {
			
				// Display message
				cout << "Preparing program's arguments on the GPU failed." << endl;
				
				// Return false
				return false;
			}
			
			// Check if setting program's remaining edge argument failed
			if(clSetKernelArg(stepSixKernel.get(), 2, sizeof(remainingEdgesTwo.get()), &unmove(remainingEdgesTwo.get())) != CL_SUCCESS) {
			
				// Display message
				cout << "Setting program's arguments on the GPU failed." << endl;
				
				// Return false
				return false;
			}
			
			// Check if trimming rounds is less than three
			if(TRIMMING_ROUNDS < 3) {
			
				// Check if setting program's SipHash keys argument failed
				if(clSetKernelArg(stepSixKernel.get(), 3, sizeof(sipHashKeysTwo), &sipHashKeysTwo) != CL_SUCCESS) {
				
					// Display message
					cout << "Setting program's arguments on the GPU failed." << endl;
					
					// Return false
					return false;
				}
			}
			
			// Check if queuing running step six on the device failed
			if(clEnqueueNDRangeKernel(commandQueue.get(), stepSixKernel.get(), 1, nullptr, &totalNumberOfWorkItems[5], &workItemsPerWorkGroup[5], 0, nullptr, nullptr) != CL_SUCCESS) {
			
				// Display message
				cout << "Running program on the GPU failed." << endl;
				
				// Return false
				return false;
			}
			
			// Check if queuing map result failed
			mapEvent.free();
			resultTwo = reinterpret_cast<uint32_t *>(clEnqueueMapBuffer(commandQueue.get(), remainingEdgesTwo.get(), CL_FALSE, CL_MAP_READ, 0, sizeof(cl_uint) + static_cast<uint64_t>(MAX_NUMBER_OF_EDGES_AFTER_TRIMMING) * EDGE_NUMBER_OF_COMPONENTS * sizeof(cl_uint), 0, nullptr, mapEvent.getAddress(), nullptr));
			if(!resultTwo) {
			
				// Display message
				cout << "Getting result from the GPU failed." << endl;
				
				// Return false
				return false;
			}
			
			// Perform commands in queue
			clFlush(commandQueue.get());
			
			// Trimming finished
			trimmingFinished(resultOne, sipHashKeysOne, heightOne, idOne, nonceOne);
			
			// Check if queuing unmap result failed
			if(clEnqueueUnmapMemObject(commandQueue.get(), remainingEdgesOne.get(), reinterpret_cast<void *>(resultOne), 0, nullptr, nullptr) != CL_SUCCESS) {
			
				// Display message
				cout << "Getting result from the GPU failed." << endl;
				
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

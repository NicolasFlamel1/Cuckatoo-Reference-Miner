// Header guard
#ifndef LEAN_TRIMMING_H
#define LEAN_TRIMMING_H


// Header files
using namespace std;


// Configurable constants

// Lean trimming number of edges per step one work item
#define LEAN_TRIMMING_NUMBER_OF_EDGES_PER_STEP_ONE_WORK_ITEM min(static_cast<uint64_t>(64), NUMBER_OF_EDGES)


// Constants

// Lean trimming required RAM bytes
#define LEAN_TRIMMING_REQUIRED_RAM_BYTES (NUMBER_OF_EDGES / BITS_IN_A_BYTE * 3)


// Function prototypes

// Check if using macOS
#ifdef __APPLE__

	// Create lean trimming context
	static inline MTL::Device *createLeanTrimmingContext(const unsigned int deviceIndex) noexcept;
	
	// Perform lean trimming loop
	static inline bool performLeanTrimmingLoop(MTL::Device *device, const unsigned int deviceIndex) noexcept;
	
// Otherwise
#else

	// Create lean trimming context
	static inline cl_context createLeanTrimmingContext(const cl_platform_id platforms[], const cl_uint numberOfPlatforms, const unsigned int deviceIndex) noexcept;
	
	// Perform lean trimming loop
	static inline bool performLeanTrimmingLoop(const cl_context context, const unsigned int deviceIndex) noexcept;
#endif


// Supporting function implementation

// Check if using macOS
#ifdef __APPLE__

	// Create lean trimming context
	MTL::Device *createLeanTrimmingContext(const unsigned int deviceIndex) noexcept {
	
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
							
								// Check if device's memory size is large enough
								if(device->recommendedMaxWorkingSetSize() >= LEAN_TRIMMING_REQUIRED_RAM_BYTES) {
							
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

	// Create lean trimming context
	cl_context createLeanTrimmingContext(const cl_platform_id platforms[], const cl_uint numberOfPlatforms, const unsigned int deviceIndex) noexcept {
	
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
										
											// Check if getting device's memory size was successful and memory size is large enough
											cl_ulong memorySize;
											if(clGetDeviceInfo(devices[j], CL_DEVICE_GLOBAL_MEM_SIZE, sizeof(memorySize), &memorySize, nullptr) == CL_SUCCESS && memorySize >= LEAN_TRIMMING_REQUIRED_RAM_BYTES) {
											
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

	// Perform lean trimming loop
	bool performLeanTrimmingLoop(MTL::Device *device, const unsigned int deviceIndex) noexcept {
	
		// Check if device index is specified
		if(deviceIndex != ALL_DEVICES) {
		
			// Display message
			cout << "Using GPU for lean trimming" << endl;
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
			cout << "Using " << utf8String << " for lean trimming" << endl;
		}
		
		// Check if creating preprocessor macros failed
		static const unique_ptr<NS::Dictionary, void(*)(NS::Dictionary *)> preprocessorMacros(NS::Dictionary::alloc()->init((const NS::Object *[]){
		
			// Edge bits value
			MTLSTR(TO_STRING(EDGE_BITS)),
			
			// Number of edges per step one work item value
			unique_ptr<NS::Number, void(*)(NS::Number *)>(NS::Number::alloc()->init(LEAN_TRIMMING_NUMBER_OF_EDGES_PER_STEP_ONE_WORK_ITEM), [](NS::Number *numberOfEdgesPerStepOneWorkItemValue) noexcept {
			
				// Free number of edges per step one work item value
				numberOfEdgesPerStepOneWorkItemValue->release();
				
			}).get(),
		
		}, (const NS::Object *[]){
		
			// Edge bits key
			MTLSTR("EDGE_BITS"),
			
			// Number of edges per step one work item key
			MTLSTR("NUMBER_OF_EDGES_PER_STEP_ONE_WORK_ITEM")
		
		}, 2), [](NS::Dictionary *preprocessorMacros) noexcept {
		
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
			#include "./lean_trimming.metal"
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
		static const unique_ptr<MTL::Function, void(*)(MTL::Function *)> clearNodesBitmapKernel(library->newFunction(MTLSTR("clearNodesBitmap")), [](MTL::Function *clearNodesBitmapKernel) noexcept {
		
			// Free clear nodes bitmap kernel
			clearNodesBitmapKernel->release();
		});
		if(!stepOneKernel || !stepTwoKernel || !stepThreeKernel || !stepFourKernel || !clearNodesBitmapKernel) {
		
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
		NS::Error *createPipelineClearNodesBitmapError;
		static unique_ptr<MTL::ComputePipelineState, void(*)(MTL::ComputePipelineState *)> clearNodesBitmapPipeline(device->newComputePipelineState(clearNodesBitmapKernel.get(), &createPipelineClearNodesBitmapError), [](MTL::ComputePipelineState *clearNodesBitmapPipeline) noexcept {
		
			// Free clear nodes bitmap pipeline
			clearNodesBitmapPipeline->release();
		});
		if(!stepOnePipeline || !stepTwoPipeline || !stepThreePipeline || !stepFourPipeline || !clearNodesBitmapPipeline) {
		
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
			
			// Check if creating pipeline clear nodes bitmap failed and an error exists
			if(!clearNodesBitmapPipeline && createPipelineClearNodesBitmapError) {
			
				// Check if error's localized description exists
				const NS::String *localizedDescription = createPipelineClearNodesBitmapError->localizedDescription();
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
		
		// Set total number of work items based on the kernels
		const MTL::Size totalNumberOfWorkItems[] = {
		
			// Trim edges step one kernel
			{NUMBER_OF_EDGES / LEAN_TRIMMING_NUMBER_OF_EDGES_PER_STEP_ONE_WORK_ITEM, 1, 1},
			
			// Trim edges step two kernel
			{NUMBER_OF_EDGES / (sizeof(uint64_t) * BITS_IN_A_BYTE), 1, 1},
			
			// Trim edges step three kernel
			{NUMBER_OF_EDGES / (sizeof(uint64_t) * BITS_IN_A_BYTE), 1, 1},
			
			// Trim edges step four kernel
			{NUMBER_OF_EDGES / (sizeof(uint64_t) * BITS_IN_A_BYTE), 1, 1},
			
			// Clear nodes bitmap kernel
			{(NUMBER_OF_EDGES / BITS_IN_A_BYTE) / sizeof(uint64_t), 1, 1}
		};
		
		// Set work items per work group based on the total number of work items and max work group size
		const MTL::Size workItemsPerWorkGroup[] = {
		
			// Trim edges step one kernel
			{min(bit_floor(device->maxThreadsPerThreadgroup().width), totalNumberOfWorkItems[0].width), 1, 1},
			
			// Trim edges step two kernel
			{min(bit_floor(device->maxThreadsPerThreadgroup().width), totalNumberOfWorkItems[1].width), 1, 1},
			
			// Trim edges step three kernel
			{min(bit_floor(device->maxThreadsPerThreadgroup().width), totalNumberOfWorkItems[2].width), 1, 1},
			
			// Trim edges step four kernel
			{min(bit_floor(device->maxThreadsPerThreadgroup().width), totalNumberOfWorkItems[3].width), 1, 1},
			
			// Clear nodes bitmap kernel
			{min(bit_floor(device->maxThreadsPerThreadgroup().width), totalNumberOfWorkItems[4].width), 1, 1}
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
		static const unique_ptr<MTL::ComputePipelineDescriptor, void(*)(MTL::ComputePipelineDescriptor *)> clearNodesBitmapPipelineDescriptor(MTL::ComputePipelineDescriptor::alloc()->init(), [](MTL::ComputePipelineDescriptor *clearNodesBitmapPipelineDescriptor) noexcept {
		
			// Free clear nodes bitmap pipeline descriptor
			clearNodesBitmapPipelineDescriptor->release();
		});
		if(!stepOnePipelineDescriptor || !stepTwoPipelineDescriptor || !stepThreePipelineDescriptor || !stepFourPipelineDescriptor || !clearNodesBitmapPipelineDescriptor) {
		
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
		
		clearNodesBitmapPipelineDescriptor->setComputeFunction(clearNodesBitmapKernel.get());
		clearNodesBitmapPipelineDescriptor->setMaxTotalThreadsPerThreadgroup(workItemsPerWorkGroup[4].width);
		clearNodesBitmapPipelineDescriptor->setThreadGroupSizeIsMultipleOfThreadExecutionWidth(workItemsPerWorkGroup[4].width % clearNodesBitmapPipeline->threadExecutionWidth() == 0);
		
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
		clearNodesBitmapPipeline = unique_ptr<MTL::ComputePipelineState, void(*)(MTL::ComputePipelineState *)>(device->newComputePipelineState(clearNodesBitmapPipelineDescriptor.get(), MTL::PipelineOptionNone, nullptr, &createPipelineClearNodesBitmapError), [](MTL::ComputePipelineState *clearNodesBitmapPipeline) noexcept {
		
			// Free clear nodes bitmap pipeline
			clearNodesBitmapPipeline->release();
		});
		if(!stepOnePipeline || !stepTwoPipeline || !stepThreePipeline || !stepFourPipeline || !clearNodesBitmapPipeline) {
		
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
			
			// Check if recreating pipeline clear nodes bitmap failed and an error exists
			if(!clearNodesBitmapPipeline && createPipelineClearNodesBitmapError) {
			
				// Check if error's localized description exists
				const NS::String *localizedDescription = createPipelineClearNodesBitmapError->localizedDescription();
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
		if(!edgesBitmapOne || !edgesBitmapTwo || !nodesBitmap) {
		
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
		
		// Set compute pass's nodes bitmap, SipHash keys, and edges bitmap arguments
		computePassEncoder->setBuffer(nodesBitmap.get(), 0, 0);
		computePassEncoder->setBytes(&sipHashKeysOne, sizeof(sipHashKeysOne), 1);
		computePassEncoder->setBuffer(edgesBitmapOne.get(), 0, 2);
		
		// Add clearing nodes bitmap to the compute pass
		computePassEncoder->setComputePipelineState(clearNodesBitmapPipeline.get());
		computePassEncoder->dispatchThreads(totalNumberOfWorkItems[4], workItemsPerWorkGroup[4]);
		
		// Add running step one to the compute pass
		computePassEncoder->setComputePipelineState(stepOnePipeline.get());
		computePassEncoder->dispatchThreads(totalNumberOfWorkItems[0], workItemsPerWorkGroup[0]);
		
		// Add running step two to the compute pass
		computePassEncoder->setComputePipelineState(stepTwoPipeline.get());
		computePassEncoder->dispatchThreads(totalNumberOfWorkItems[1], workItemsPerWorkGroup[1]);
		
		// Go through all remaining trimming rounds
		for(unsigned int i = 1; i < TRIMMING_ROUNDS; ++i) {
		
			// Add clearing nodes bitmap to the compute pass
			computePassEncoder->setComputePipelineState(clearNodesBitmapPipeline.get());
			computePassEncoder->dispatchThreads(totalNumberOfWorkItems[4], workItemsPerWorkGroup[4]);
			
			// Set compute pass's nodes in second partition argument
			computePassEncoder->setBytes(&unmove(static_cast<uint8_t>(i % 2)), sizeof(uint8_t), 3);
			
			// Add running step three to the compute pass
			computePassEncoder->setComputePipelineState(stepThreePipeline.get());
			computePassEncoder->dispatchThreads(totalNumberOfWorkItems[2], workItemsPerWorkGroup[2]);
			
			// Add running step four to the compute pass
			computePassEncoder->setComputePipelineState(stepFourPipeline.get());
			computePassEncoder->dispatchThreads(totalNumberOfWorkItems[3], workItemsPerWorkGroup[3]);
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
		
		// Set compute pass's nodes bitmap, SipHash keys, and edges bitmap arguments
		computePassEncoder->setBuffer(nodesBitmap.get(), 0, 0);
		computePassEncoder->setBytes(&sipHashKeysTwo, sizeof(sipHashKeysTwo), 1);
		computePassEncoder->setBuffer(edgesBitmapTwo.get(), 0, 2);
		
		// Add clearing nodes bitmap to the compute pass
		computePassEncoder->setComputePipelineState(clearNodesBitmapPipeline.get());
		computePassEncoder->dispatchThreads(totalNumberOfWorkItems[4], workItemsPerWorkGroup[4]);
		
		// Add running step one to the compute pass
		computePassEncoder->setComputePipelineState(stepOnePipeline.get());
		computePassEncoder->dispatchThreads(totalNumberOfWorkItems[0], workItemsPerWorkGroup[0]);
		
		// Add running step two to the compute pass
		computePassEncoder->setComputePipelineState(stepTwoPipeline.get());
		computePassEncoder->dispatchThreads(totalNumberOfWorkItems[1], workItemsPerWorkGroup[1]);
		
		// Go through all remaining trimming rounds
		for(unsigned int i = 1; i < TRIMMING_ROUNDS; ++i) {
		
			// Add clearing nodes bitmap to the compute pass
			computePassEncoder->setComputePipelineState(clearNodesBitmapPipeline.get());
			computePassEncoder->dispatchThreads(totalNumberOfWorkItems[4], workItemsPerWorkGroup[4]);
			
			// Set compute pass's nodes in second partition argument
			computePassEncoder->setBytes(&unmove(static_cast<uint8_t>(i % 2)), sizeof(uint8_t), 3);
			
			// Add running step three to the compute pass
			computePassEncoder->setComputePipelineState(stepThreePipeline.get());
			computePassEncoder->dispatchThreads(totalNumberOfWorkItems[2], workItemsPerWorkGroup[2]);
			
			// Add running step four to the compute pass
			computePassEncoder->setComputePipelineState(stepFourPipeline.get());
			computePassEncoder->dispatchThreads(totalNumberOfWorkItems[3], workItemsPerWorkGroup[3]);
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
			
			// Set compute pass's nodes bitmap, SipHash keys, and edges bitmap arguments
			computePassEncoder->setBuffer(nodesBitmap.get(), 0, 0);
			computePassEncoder->setBytes(&sipHashKeysOne, sizeof(sipHashKeysOne), 1);
			computePassEncoder->setBuffer(edgesBitmapOne.get(), 0, 2);
			
			// Add clearing nodes bitmap to the compute pass
			computePassEncoder->setComputePipelineState(clearNodesBitmapPipeline.get());
			computePassEncoder->dispatchThreads(totalNumberOfWorkItems[4], workItemsPerWorkGroup[4]);
			
			// Add running step one to the compute pass
			computePassEncoder->setComputePipelineState(stepOnePipeline.get());
			computePassEncoder->dispatchThreads(totalNumberOfWorkItems[0], workItemsPerWorkGroup[0]);
			
			// Add running step two to the compute pass
			computePassEncoder->setComputePipelineState(stepTwoPipeline.get());
			computePassEncoder->dispatchThreads(totalNumberOfWorkItems[1], workItemsPerWorkGroup[1]);
			
			// Go through all remaining trimming rounds
			for(unsigned int i = 1; i < TRIMMING_ROUNDS; ++i) {
			
				// Add clearing nodes bitmap to the compute pass
				computePassEncoder->setComputePipelineState(clearNodesBitmapPipeline.get());
				computePassEncoder->dispatchThreads(totalNumberOfWorkItems[4], workItemsPerWorkGroup[4]);
				
				// Set compute pass's nodes in second partition argument
				computePassEncoder->setBytes(&unmove(static_cast<uint8_t>(i % 2)), sizeof(uint8_t), 3);
				
				// Add running step three to the compute pass
				computePassEncoder->setComputePipelineState(stepThreePipeline.get());
				computePassEncoder->dispatchThreads(totalNumberOfWorkItems[2], workItemsPerWorkGroup[2]);
				
				// Add running step four to the compute pass
				computePassEncoder->setComputePipelineState(stepFourPipeline.get());
				computePassEncoder->dispatchThreads(totalNumberOfWorkItems[3], workItemsPerWorkGroup[3]);
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
			
			// Set compute pass's nodes bitmap, SipHash keys, and edges bitmap arguments
			computePassEncoder->setBuffer(nodesBitmap.get(), 0, 0);
			computePassEncoder->setBytes(&sipHashKeysTwo, sizeof(sipHashKeysTwo), 1);
			computePassEncoder->setBuffer(edgesBitmapTwo.get(), 0, 2);
			
			// Add clearing nodes bitmap to the compute pass
			computePassEncoder->setComputePipelineState(clearNodesBitmapPipeline.get());
			computePassEncoder->dispatchThreads(totalNumberOfWorkItems[4], workItemsPerWorkGroup[4]);
			
			// Add running step one to the compute pass
			computePassEncoder->setComputePipelineState(stepOnePipeline.get());
			computePassEncoder->dispatchThreads(totalNumberOfWorkItems[0], workItemsPerWorkGroup[0]);
			
			// Add running step two to the compute pass
			computePassEncoder->setComputePipelineState(stepTwoPipeline.get());
			computePassEncoder->dispatchThreads(totalNumberOfWorkItems[1], workItemsPerWorkGroup[1]);
			
			// Go through all remaining trimming rounds
			for(unsigned int i = 1; i < TRIMMING_ROUNDS; ++i) {
			
				// Add clearing nodes bitmap to the compute pass
				computePassEncoder->setComputePipelineState(clearNodesBitmapPipeline.get());
				computePassEncoder->dispatchThreads(totalNumberOfWorkItems[4], workItemsPerWorkGroup[4]);
				
				// Set compute pass's nodes in second partition argument
				computePassEncoder->setBytes(&unmove(static_cast<uint8_t>(i % 2)), sizeof(uint8_t), 3);
				
				// Add running step three to the compute pass
				computePassEncoder->setComputePipelineState(stepThreePipeline.get());
				computePassEncoder->dispatchThreads(totalNumberOfWorkItems[2], workItemsPerWorkGroup[2]);
				
				// Add running step four to the compute pass
				computePassEncoder->setComputePipelineState(stepFourPipeline.get());
				computePassEncoder->dispatchThreads(totalNumberOfWorkItems[3], workItemsPerWorkGroup[3]);
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

	// Perform lean trimming loop
	bool performLeanTrimmingLoop(const cl_context context, const unsigned int deviceIndex) noexcept {
	
		// Check if getting context's device failed
		cl_device_id device;
		if(clGetContextInfo(context, CL_CONTEXT_DEVICES, sizeof(device), &device, nullptr) != CL_SUCCESS) {
		
			// Display message
			cout << "Getting OpenCL context's GPU failed" << endl;
			
			// Return false
			return false;
		}
		
		// Check if getting device's max work group size failed or its max work group size don't exists
		size_t maxWorkGroupSize;
		if(clGetDeviceInfo(device, CL_DEVICE_MAX_WORK_GROUP_SIZE, sizeof(maxWorkGroupSize), &maxWorkGroupSize, nullptr) != CL_SUCCESS || !maxWorkGroupSize) {
		
			// Display message
			cout << "Getting GPU's info failed" << endl;
			
			// Return false
			return false;
		}
		
		// Check if device index is specified
		if(deviceIndex != ALL_DEVICES) {
		
			// Display message
			cout << "Using GPU for lean trimming" << endl;
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
			cout << "Using " << name << " for lean trimming" << endl;
		}
		
		// Check if creating program for the device failed
		const char *source = (
			#include "./lean_trimming.cl"
		);
		const size_t sourceSize = strlen(source);
		static unique_ptr<remove_pointer<cl_program>::type, decltype(&clReleaseProgram)> program(clCreateProgramWithSource(context, 1, &source, &sourceSize, nullptr), clReleaseProgram);
		if(!program) {
		
			// Display message
			cout << "Creating program for the GPU failed" << endl;
			
			// Return false
			return false;
		}
		
		// Check if building program for the device failed
		if(clBuildProgram(program.get(), 1, &device, ("-cl-std=CL1.2 -Werror -DEDGE_BITS=" TO_STRING(EDGE_BITS) " -DNUMBER_OF_EDGES_PER_STEP_ONE_WORK_ITEM=" + to_string(LEAN_TRIMMING_NUMBER_OF_EDGES_PER_STEP_ONE_WORK_ITEM)).c_str(), nullptr, nullptr) != CL_SUCCESS) {
		
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
		if(!stepOneKernel || !stepTwoKernel || !stepThreeKernel || !stepFourKernel) {
		
			// Display message
			cout << "Creating kernels for the GPU failed" << endl;
			
			// Return false
			return false;
		}
		
		// Set total number of work items based on the kernels
		const size_t totalNumberOfWorkItems[] = {
		
			// Trim edges step one kernel
			NUMBER_OF_EDGES / LEAN_TRIMMING_NUMBER_OF_EDGES_PER_STEP_ONE_WORK_ITEM,
			
			// Trim edges step two kernel
			NUMBER_OF_EDGES / (sizeof(cl_ulong) * BITS_IN_A_BYTE),
			
			// Trim edges step three kernel
			NUMBER_OF_EDGES / (sizeof(cl_ulong) * BITS_IN_A_BYTE),
			
			// Trim edges step four kernel
			NUMBER_OF_EDGES / (sizeof(cl_ulong) * BITS_IN_A_BYTE)
		};
		
		// Set work items per work group based on the total number of work items and max work group size
		const size_t workItemsPerWorkGroup[] = {
		
			// Trim edges step one kernel
			min(bit_floor(maxWorkGroupSize), totalNumberOfWorkItems[0]),
			
			// Trim edges step two kernel
			min(bit_floor(maxWorkGroupSize), totalNumberOfWorkItems[1]),
			
			// Trim edges step three kernel
			min(bit_floor(maxWorkGroupSize), totalNumberOfWorkItems[2]),
			
			// Trim edges step four kernel
			min(bit_floor(maxWorkGroupSize), totalNumberOfWorkItems[3])
		};
		
		// Free kernels
		stepOneKernel.reset();
		stepTwoKernel.reset();
		stepThreeKernel.reset();
		stepFourKernel.reset();
		
		// Check if recreating program for the device failed
		program = unique_ptr<remove_pointer<cl_program>::type, decltype(&clReleaseProgram)>(clCreateProgramWithSource(context, 1, &source, &sourceSize, nullptr), clReleaseProgram);
		if(!program) {
		
			// Display message
			cout << "Creating program for the GPU failed" << endl;
			
			// Return false
			return false;
		}
		
		// Check if rebuilding program for the device with hardcoded work items per work groups failed
		if(clBuildProgram(program.get(), 1, &device, ("-cl-std=CL1.2 -Werror -DEDGE_BITS=" TO_STRING(EDGE_BITS) " -DNUMBER_OF_EDGES_PER_STEP_ONE_WORK_ITEM=" + to_string(LEAN_TRIMMING_NUMBER_OF_EDGES_PER_STEP_ONE_WORK_ITEM) + " -DTRIM_EDGES_STEP_ONE_WORK_ITEMS_PER_WORK_GROUP=" + to_string(workItemsPerWorkGroup[0]) + " -DTRIM_EDGES_STEP_TWO_WORK_ITEMS_PER_WORK_GROUP=" + to_string(workItemsPerWorkGroup[1]) + " -DTRIM_EDGES_STEP_THREE_WORK_ITEMS_PER_WORK_GROUP=" + to_string(workItemsPerWorkGroup[2]) + " -DTRIM_EDGES_STEP_FOUR_WORK_ITEMS_PER_WORK_GROUP=" + to_string(workItemsPerWorkGroup[3])).c_str(), nullptr, nullptr) != CL_SUCCESS) {
		
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
		if(!stepOneKernel || !stepTwoKernel || !stepThreeKernel || !stepFourKernel) {
		
			// Display message
			cout << "Creating kernels for the GPU failed" << endl;
			
			// Return false
			return false;
		}
		
		// Check if allocating memory on the device failed
		static const unique_ptr<remove_pointer<cl_mem>::type, decltype(&clReleaseMemObject)> edgesBitmapOne(clCreateBuffer(context, CL_MEM_READ_WRITE | CL_MEM_HOST_READ_ONLY | CL_MEM_ALLOC_HOST_PTR, NUMBER_OF_EDGES / BITS_IN_A_BYTE, nullptr, nullptr), clReleaseMemObject);
		static const unique_ptr<remove_pointer<cl_mem>::type, decltype(&clReleaseMemObject)> edgesBitmapTwo(clCreateBuffer(context, CL_MEM_READ_WRITE | CL_MEM_HOST_READ_ONLY | CL_MEM_ALLOC_HOST_PTR, NUMBER_OF_EDGES / BITS_IN_A_BYTE, nullptr, nullptr), clReleaseMemObject);
		static const unique_ptr<remove_pointer<cl_mem>::type, decltype(&clReleaseMemObject)> nodesBitmap(clCreateBuffer(context, CL_MEM_READ_WRITE | CL_MEM_HOST_NO_ACCESS, NUMBER_OF_EDGES / BITS_IN_A_BYTE, nullptr, nullptr), clReleaseMemObject);
		if(!edgesBitmapOne || !edgesBitmapTwo || !nodesBitmap) {
		
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
		if(clSetKernelArg(stepOneKernel.get(), 0, sizeof(nodesBitmap.get()), &unmove(nodesBitmap.get())) != CL_SUCCESS || clSetKernelArg(stepTwoKernel.get(), 1, sizeof(nodesBitmap.get()), &unmove(nodesBitmap.get())) != CL_SUCCESS || clSetKernelArg(stepThreeKernel.get(), 1, sizeof(nodesBitmap.get()), &unmove(nodesBitmap.get())) != CL_SUCCESS || clSetKernelArg(stepFourKernel.get(), 1, sizeof(nodesBitmap.get()), &unmove(nodesBitmap.get())) != CL_SUCCESS) {
		
			// Display message
			cout << "Setting program's arguments on the GPU failed" << endl;
			
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
		
		// Check if queuing clearing nodes bitmap on the device failed
		static Event firstCommandEvent;
		if(clEnqueueFillBuffer(commandQueue.get(), nodesBitmap.get(), (const cl_ulong[]){0}, sizeof(cl_ulong), 0, NUMBER_OF_EDGES / BITS_IN_A_BYTE, 0, nullptr, firstCommandEvent.getAddress()) != CL_SUCCESS) {
		
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
		
		// Check if setting program's SipHash keys argument failed
		if(clSetKernelArg(stepOneKernel.get(), 1, sizeof(sipHashKeysOne), &sipHashKeysOne) != CL_SUCCESS) {
		
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
		
		// Check if setting program's edges bitmap or SipHash keys arguments failed
		if(clSetKernelArg(stepTwoKernel.get(), 0, sizeof(edgesBitmapOne.get()), &unmove(edgesBitmapOne.get())) != CL_SUCCESS || clSetKernelArg(stepTwoKernel.get(), 2, sizeof(sipHashKeysOne), &sipHashKeysOne) != CL_SUCCESS) {
		
			// Display message
			cout << "Setting program's arguments on the GPU failed" << endl;
			
			// Return false
			return false;
		}
		
		// Check if queuing running step Two on the device failed
		if(clEnqueueNDRangeKernel(commandQueue.get(), stepTwoKernel.get(), 1, nullptr, &totalNumberOfWorkItems[1], &workItemsPerWorkGroup[1], 0, nullptr, nullptr) != CL_SUCCESS) {
		
			// Display message
			cout << "Running program on the GPU failed" << endl;
			
			// Return false
			return false;
		}
		
		// Check if setting program's edges bitmap or SipHash keys arguments failed
		if(clSetKernelArg(stepThreeKernel.get(), 0, sizeof(edgesBitmapOne.get()), &unmove(edgesBitmapOne.get())) != CL_SUCCESS || clSetKernelArg(stepThreeKernel.get(), 3, sizeof(sipHashKeysOne), &sipHashKeysOne) != CL_SUCCESS || clSetKernelArg(stepFourKernel.get(), 0, sizeof(edgesBitmapOne.get()), &unmove(edgesBitmapOne.get())) != CL_SUCCESS || clSetKernelArg(stepFourKernel.get(), 3, sizeof(sipHashKeysOne), &sipHashKeysOne) != CL_SUCCESS) {
		
			// Display message
			cout << "Setting program's arguments on the GPU failed" << endl;
			
			// Return false
			return false;
		}
		
		// Go through all remaining trimming rounds
		for(unsigned int i = 1; i < TRIMMING_ROUNDS; ++i) {
		
			// Check if queuing clearing nodes bitmap on the device failed
			if(clEnqueueFillBuffer(commandQueue.get(), nodesBitmap.get(), (const cl_ulong[]){0}, sizeof(cl_ulong), 0, NUMBER_OF_EDGES / BITS_IN_A_BYTE, 0, nullptr, nullptr) != CL_SUCCESS) {
			
				// Display message
				cout << "Preparing program's arguments on the GPU failed" << endl;
				
				// Return false
				return false;
			}
			
			// Check if setting program's nodes in second partition argument failed
			if(clSetKernelArg(stepThreeKernel.get(), 2, sizeof(cl_uchar), &unmove(static_cast<cl_uchar>(i % 2))) != CL_SUCCESS) {
			
				// Display message
				cout << "Setting program's arguments on the GPU failed" << endl;
				
				// Return false
				return false;
			}
			
			// Check if queuing running step three on the device failed
			if(clEnqueueNDRangeKernel(commandQueue.get(), stepThreeKernel.get(), 1, nullptr, &totalNumberOfWorkItems[2], &workItemsPerWorkGroup[2], 0, nullptr, nullptr) != CL_SUCCESS) {
			
				// Display message
				cout << "Running program on the GPU failed" << endl;
				
				// Return false
				return false;
			}
			
			// Check if setting program's nodes in second partition argument failed
			if(clSetKernelArg(stepFourKernel.get(), 2, sizeof(cl_uchar), &unmove(static_cast<cl_uchar>(i % 2))) != CL_SUCCESS) {
			
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
		
		// Check if queuing clearing nodes bitmap on the device failed
		firstCommandEvent.free();
		if(clEnqueueFillBuffer(commandQueue.get(), nodesBitmap.get(), (const cl_ulong[]){0}, sizeof(cl_ulong), 0, NUMBER_OF_EDGES / BITS_IN_A_BYTE, 0, nullptr, firstCommandEvent.getAddress()) != CL_SUCCESS) {
		
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
		
		// Check if setting program's SipHash keys argument failed
		if(clSetKernelArg(stepOneKernel.get(), 1, sizeof(sipHashKeysTwo), &sipHashKeysTwo) != CL_SUCCESS) {
		
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
		
		// Check if setting program's edges bitmap or SipHash keys arguments failed
		if(clSetKernelArg(stepTwoKernel.get(), 0, sizeof(edgesBitmapTwo.get()), &unmove(edgesBitmapTwo.get())) != CL_SUCCESS || clSetKernelArg(stepTwoKernel.get(), 2, sizeof(sipHashKeysTwo), &sipHashKeysTwo) != CL_SUCCESS) {
		
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
		
		// Check if setting program's edges bitmap or SipHash keys arguments failed
		if(clSetKernelArg(stepThreeKernel.get(), 0, sizeof(edgesBitmapTwo.get()), &unmove(edgesBitmapTwo.get())) != CL_SUCCESS || clSetKernelArg(stepThreeKernel.get(), 3, sizeof(sipHashKeysTwo), &sipHashKeysTwo) != CL_SUCCESS || clSetKernelArg(stepFourKernel.get(), 0, sizeof(edgesBitmapTwo.get()), &unmove(edgesBitmapTwo.get())) != CL_SUCCESS || clSetKernelArg(stepFourKernel.get(), 3, sizeof(sipHashKeysTwo), &sipHashKeysTwo) != CL_SUCCESS) {
		
			// Display message
			cout << "Setting program's arguments on the GPU failed" << endl;
			
			// Return false
			return false;
		}
		
		// Go through all remaining trimming rounds
		for(unsigned int i = 1; i < TRIMMING_ROUNDS; ++i) {
		
			// Check if queuing clearing nodes bitmap on the device failed
			if(clEnqueueFillBuffer(commandQueue.get(), nodesBitmap.get(), (const cl_ulong[]){0}, sizeof(cl_ulong), 0, NUMBER_OF_EDGES / BITS_IN_A_BYTE, 0, nullptr, nullptr) != CL_SUCCESS) {
			
				// Display message
				cout << "Preparing program's arguments on the GPU failed" << endl;
				
				// Return false
				return false;
			}
			
			// Check if setting program's nodes in second partition argument failed
			if(clSetKernelArg(stepThreeKernel.get(), 2, sizeof(cl_uchar), &unmove(static_cast<cl_uchar>(i % 2))) != CL_SUCCESS) {
			
				// Display message
				cout << "Setting program's arguments on the GPU failed" << endl;
				
				// Return false
				return false;
			}
			
			// Check if queuing running step three on the device failed
			if(clEnqueueNDRangeKernel(commandQueue.get(), stepThreeKernel.get(), 1, nullptr, &totalNumberOfWorkItems[2], &workItemsPerWorkGroup[2], 0, nullptr, nullptr) != CL_SUCCESS) {
			
				// Display message
				cout << "Running program on the GPU failed" << endl;
				
				// Return false
				return false;
			}
			
			// Check if setting program's nodes in second partition argument failed
			if(clSetKernelArg(stepFourKernel.get(), 2, sizeof(cl_uchar), &unmove(static_cast<cl_uchar>(i % 2))) != CL_SUCCESS) {
			
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
			
			// Check if queuing clearing nodes bitmap on the device failed
			firstCommandEvent.free();
			if(clEnqueueFillBuffer(commandQueue.get(), nodesBitmap.get(), (const cl_ulong[]){0}, sizeof(cl_ulong), 0, NUMBER_OF_EDGES / BITS_IN_A_BYTE, 0, nullptr, firstCommandEvent.getAddress()) != CL_SUCCESS) {
			
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
			
			// Check if setting program's SipHash keys argument failed
			if(clSetKernelArg(stepOneKernel.get(), 1, sizeof(sipHashKeysOne), &sipHashKeysOne) != CL_SUCCESS) {
			
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
			
			// Check if setting program's edges bitmap or SipHash keys arguments failed
			if(clSetKernelArg(stepTwoKernel.get(), 0, sizeof(edgesBitmapOne.get()), &unmove(edgesBitmapOne.get())) != CL_SUCCESS || clSetKernelArg(stepTwoKernel.get(), 2, sizeof(sipHashKeysOne), &sipHashKeysOne) != CL_SUCCESS) {
			
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
			
			// Check if setting program's edges bitmap or SipHash keys arguments failed
			if(clSetKernelArg(stepThreeKernel.get(), 0, sizeof(edgesBitmapOne.get()), &unmove(edgesBitmapOne.get())) != CL_SUCCESS || clSetKernelArg(stepThreeKernel.get(), 3, sizeof(sipHashKeysOne), &sipHashKeysOne) != CL_SUCCESS || clSetKernelArg(stepFourKernel.get(), 0, sizeof(edgesBitmapOne.get()), &unmove(edgesBitmapOne.get())) != CL_SUCCESS || clSetKernelArg(stepFourKernel.get(), 3, sizeof(sipHashKeysOne), &sipHashKeysOne) != CL_SUCCESS) {
			
				// Display message
				cout << "Setting program's arguments on the GPU failed" << endl;
				
				// Return false
				return false;
			}
			
			// Go through all remaining trimming rounds
			for(unsigned int i = 1; i < TRIMMING_ROUNDS; ++i) {
			
				// Check if queuing clearing nodes bitmap on the device failed
				if(clEnqueueFillBuffer(commandQueue.get(), nodesBitmap.get(), (const cl_ulong[]){0}, sizeof(cl_ulong), 0, NUMBER_OF_EDGES / BITS_IN_A_BYTE, 0, nullptr, nullptr) != CL_SUCCESS) {
				
					// Display message
					cout << "Preparing program's arguments on the GPU failed" << endl;
					
					// Return false
					return false;
				}
				
				// Check if setting program's nodes in second partition argument failed
				if(clSetKernelArg(stepThreeKernel.get(), 2, sizeof(cl_uchar), &unmove(static_cast<cl_uchar>(i % 2))) != CL_SUCCESS) {
				
					// Display message
					cout << "Setting program's arguments on the GPU failed" << endl;
					
					// Return false
					return false;
				}
				
				// Check if queuing running step three on the device failed
				if(clEnqueueNDRangeKernel(commandQueue.get(), stepThreeKernel.get(), 1, nullptr, &totalNumberOfWorkItems[2], &workItemsPerWorkGroup[2], 0, nullptr, nullptr) != CL_SUCCESS) {
				
					// Display message
					cout << "Running program on the GPU failed" << endl;
					
					// Return false
					return false;
				}
				
				// Check if setting program's nodes in second partition argument failed
				if(clSetKernelArg(stepFourKernel.get(), 2, sizeof(cl_uchar), &unmove(static_cast<cl_uchar>(i % 2))) != CL_SUCCESS) {
				
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
			
			// Check if queuing clearing nodes bitmap on the device failed
			firstCommandEvent.free();
			if(clEnqueueFillBuffer(commandQueue.get(), nodesBitmap.get(), (const cl_ulong[]){0}, sizeof(cl_ulong), 0, NUMBER_OF_EDGES / BITS_IN_A_BYTE, 0, nullptr, firstCommandEvent.getAddress()) != CL_SUCCESS) {
			
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
			
			// Check if setting program's SipHash keys argument failed
			if(clSetKernelArg(stepOneKernel.get(), 1, sizeof(sipHashKeysTwo), &sipHashKeysTwo) != CL_SUCCESS) {
			
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
			
			// Check if setting program's edges bitmap or SipHash keys arguments failed
			if(clSetKernelArg(stepTwoKernel.get(), 0, sizeof(edgesBitmapTwo.get()), &unmove(edgesBitmapTwo.get())) != CL_SUCCESS || clSetKernelArg(stepTwoKernel.get(), 2, sizeof(sipHashKeysTwo), &sipHashKeysTwo) != CL_SUCCESS) {
			
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
			
			// Check if setting program's edges bitmap or SipHash keys arguments failed
			if(clSetKernelArg(stepThreeKernel.get(), 0, sizeof(edgesBitmapTwo.get()), &unmove(edgesBitmapTwo.get())) != CL_SUCCESS || clSetKernelArg(stepThreeKernel.get(), 3, sizeof(sipHashKeysTwo), &sipHashKeysTwo) != CL_SUCCESS || clSetKernelArg(stepFourKernel.get(), 0, sizeof(edgesBitmapTwo.get()), &unmove(edgesBitmapTwo.get())) != CL_SUCCESS || clSetKernelArg(stepFourKernel.get(), 3, sizeof(sipHashKeysTwo), &sipHashKeysTwo) != CL_SUCCESS) {
			
				// Display message
				cout << "Setting program's arguments on the GPU failed" << endl;
				
				// Return false
				return false;
			}
			
			// Go through all remaining trimming rounds
			for(unsigned int i = 1; i < TRIMMING_ROUNDS; ++i) {
			
				// Check if queuing clearing nodes bitmap on the device failed
				if(clEnqueueFillBuffer(commandQueue.get(), nodesBitmap.get(), (const cl_ulong[]){0}, sizeof(cl_ulong), 0, NUMBER_OF_EDGES / BITS_IN_A_BYTE, 0, nullptr, nullptr) != CL_SUCCESS) {
				
					// Display message
					cout << "Preparing program's arguments on the GPU failed" << endl;
					
					// Return false
					return false;
				}
				
				// Check if setting program's nodes in second partition argument failed
				if(clSetKernelArg(stepThreeKernel.get(), 2, sizeof(cl_uchar), &unmove(static_cast<cl_uchar>(i % 2))) != CL_SUCCESS) {
				
					// Display message
					cout << "Setting program's arguments on the GPU failed" << endl;
					
					// Return false
					return false;
				}
				
				// Check if queuing running step three on the device failed
				if(clEnqueueNDRangeKernel(commandQueue.get(), stepThreeKernel.get(), 1, nullptr, &totalNumberOfWorkItems[2], &workItemsPerWorkGroup[2], 0, nullptr, nullptr) != CL_SUCCESS) {
				
					// Display message
					cout << "Running program on the GPU failed" << endl;
					
					// Return false
					return false;
				}
				
				// Check if setting program's nodes in second partition argument failed
				if(clSetKernelArg(stepFourKernel.get(), 2, sizeof(cl_uchar), &unmove(static_cast<cl_uchar>(i % 2))) != CL_SUCCESS) {
				
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

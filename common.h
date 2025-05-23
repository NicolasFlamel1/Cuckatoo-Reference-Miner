// Header guard
#ifndef COMMON_H
#define COMMON_H


// Header files
using namespace std;


// Constants

// Solution size
#define SOLUTION_SIZE 42

// Number of edges
#define NUMBER_OF_EDGES (static_cast<uint64_t>(1) << EDGE_BITS)

// Max slean trimming parts
#define MAX_SLEAN_TRIMMING_PARTS 16

// Min local RAM kilobytes
#define MIN_LOCAL_RAM_KILOBYTES 32


// Checks

// Throw error if host's byte order isn't little endian
static_assert(endian::native == endian::little, "Host's byte order isn't little endian");

// Throw error if edge bits is invalid
static_assert(EDGE_BITS >= 10 && EDGE_BITS <= 32, "Edge bits is outside of the accepted range");

// Throw error if trimming rounds is invalid (Number of edges after a pair of trimming rounds ≈ NUMBER_OF_EDGES / (pow(roundNumber / 2 + 1, 2) + 2))
static_assert(TRIMMING_ROUNDS >= 0 && TRIMMING_ROUNDS <= 1000 && static_cast<double>(NUMBER_OF_EDGES) / (((TRIMMING_ROUNDS + 1) / 2 + 1) * ((TRIMMING_ROUNDS + 1) / 2 + 1) + 2) >= SOLUTION_SIZE, "Trimming rounds is outside of the accepted range");

// Throw error if slean trimming parts is invalid
static_assert(SLEAN_TRIMMING_PARTS >= 2 && SLEAN_TRIMMING_PARTS <= MAX_SLEAN_TRIMMING_PARTS, "Slean trimming parts is outside of the accepted range");
static_assert(has_single_bit(static_cast<unsigned int>(SLEAN_TRIMMING_PARTS)), "Slean trimming parts isn't a power of two");

// Throw error if slean then mean slean trimming rounds is invalid
static_assert(SLEAN_THEN_MEAN_SLEAN_TRIMMING_ROUNDS >= 1 && (SLEAN_THEN_MEAN_SLEAN_TRIMMING_ROUNDS < TRIMMING_ROUNDS || !TRIMMING_ROUNDS), "Slean then mean slean trimming rounds is outside of the accepted range");

// Throw error if local RAM kilobytes is invalid
static_assert(LOCAL_RAM_KILOBYTES >= MIN_LOCAL_RAM_KILOBYTES && LOCAL_RAM_KILOBYTES <= 256, "Local RAM kilobytes is outside of the accepted range");
static_assert(has_single_bit(static_cast<unsigned int>(LOCAL_RAM_KILOBYTES)), "Local RAM kilobytes isn't a power of two");

// Throw error if the size of a vector of a type isn't the same as the size of an array of that type
static_assert(sizeof(uint64_t __attribute__((vector_size(8)))) == sizeof(uint64_t[1]), "Vector vs array size mismatch");

// Check if using an Apple device and not using OpenCL
#if defined __APPLE__ && !defined USE_OPENCL

	// Throw error if the size of a vector of a type isn't the same as the size of a Metal vector of that type
	static_assert(sizeof(uint64_t __attribute__((vector_size(8 * 4)))) == sizeof(uint64_t[4]), "Vector vs Metal vector size mismatch");
	
	// Throw error if the size of a Metal vector type isn't double the size of a Metal vector of twice that type
	static_assert(sizeof(uint32_t) == sizeof(uint32_t[2]) / 2 && sizeof(uint32_t[2]) == sizeof(uint32_t[4]) / 2, "Metal vector size mismatch");
	
// Otherwise
#else

	// Throw error if the size of a vector of a type isn't the same as the size of an OpenCL vector of that type
	static_assert(sizeof(uint64_t __attribute__((vector_size(8 * 4)))) == sizeof(cl_ulong4), "Vector vs OpenCL vector size mismatch");
	
	// Throw error if the size of an OpenCL vector type isn't double the size of an OpenCL vector of twice that type
	static_assert(sizeof(cl_uint) == sizeof(cl_uint2) / 2 && sizeof(cl_uint2) == sizeof(cl_uint4) / 2, "OpenCL vector size mismatch");
#endif


// Constants

// Bits in a byte
#define BITS_IN_A_BYTE 8

// Bytes in a kilobyte
#define BYTES_IN_A_KILOBYTE 1024

// Kilobytes in a megabyte
#define KILOBYTES_IN_A_MEGABYTE 1024

// Megabytes in a gigabyte
#define MEGABYTES_IN_A_GIGABYTE 1024

// Decimal number base
#define DECIMAL_NUMBER_BASE 10

// Edges bitmap size
#define EDGES_BITMAP_SIZE (NUMBER_OF_EDGES / BITMAP_UNIT_WIDTH)

// Node mask
#define NODE_MASK (NUMBER_OF_EDGES - 1)

// BLAKE2b hash size
#define BLAKE2B_HASH_SIZE 32

// Secp256k1 private key size
#define SECP256K1_PRIVATE_KEY_SIZE 32

// Header size
#define HEADER_SIZE (sizeof(uint16_t) + sizeof(uint64_t) + sizeof(int64_t) + BLAKE2B_HASH_SIZE + BLAKE2B_HASH_SIZE + BLAKE2B_HASH_SIZE + BLAKE2B_HASH_SIZE + BLAKE2B_HASH_SIZE + SECP256K1_PRIVATE_KEY_SIZE + sizeof(uint64_t) + sizeof(uint64_t) + sizeof(uint64_t) + sizeof(uint32_t))

// Check if there's no trimming rounds
#if TRIMMING_ROUNDS == 0

	// Max number of edges after trimming
	#define MAX_NUMBER_OF_EDGES_AFTER_TRIMMING NUMBER_OF_EDGES
	
// Otherwise
#else

	// Max number of edges after trimming
	#define MAX_NUMBER_OF_EDGES_AFTER_TRIMMING ceilingConstantExpression(static_cast<double>(NUMBER_OF_EDGES) / (((TRIMMING_ROUNDS + 1) / 2) * ((TRIMMING_ROUNDS + 1) / 2) + 2) - 1)
#endif

// Edge number of components
#define EDGE_NUMBER_OF_COMPONENTS 3

// Milliseconds in a second
#define MILLISECONDS_IN_A_SECOND 1000

// All devices
#define ALL_DEVICES 0

// GNOME inhibit suspending session
#define GNOME_INHIBIT_SUSPENDING_SESSION (1 << 2)

// To string
#define STRINGIFY(a) #a
#define TO_STRING(a) STRINGIFY(a)


// Classes

// Prevent sleep
class PreventSleep final {

	// Public
	public:
	
		// Constructor
		inline explicit PreventSleep() noexcept;
		
		// Destructor
		inline ~PreventSleep() noexcept;
		
	// Private
	private:
	
		// Check if using an Apple device
		#ifdef __APPLE__
		
			// Check if using macOS
			#if TARGET_OS_OSX == 1
			
				// Assertion ID
				IOPMAssertionID assertionID;
			#endif
			
		// Otherwise check if not using Windows
		#elif !defined _WIN32
		
			// Allow sleep message
			unique_ptr<DBusMessage, decltype(&dbus_message_unref)> allowSleepMessage;
		#endif
};

// Check if not using an Apple device or using OpenCL
#if !defined __APPLE__ || defined USE_OPENCL

	// Event class
	class Event final {
	
		// Public
		public:
		
			// Constructor
			inline explicit Event() noexcept;
			
			// Destructor
			inline ~Event() noexcept;
			
			// Get address
			inline cl_event *getAddress() noexcept;
			
			// Get
			inline cl_event &get() noexcept;
			
			// Free
			inline void free() noexcept;
			
		// Private
		private:
		
			// Event
			cl_event event;
	};
#endif

// Check if Windows
#ifdef _WIN32

	// Windows socket class
	class WindowsSocket final {
	
		// Public
		public:
		
			// Constructor
			inline explicit WindowsSocket() noexcept;
			
			// Destructor
			inline ~WindowsSocket() noexcept;
		
		// Private
		private:
		
			// Major version
			static const BYTE MAJOR_VERSION;
			
			// Minor version
			static const BYTE MINOR_VERSION;
	};
#endif


// Constants

// Check if Windows
#ifdef _WIN32

	// Windows socket major version
	const BYTE WindowsSocket::MAJOR_VERSION = 2;
	
	// Windows socket minor version
	const BYTE WindowsSocket::MINOR_VERSION = 2;
#endif


// Function prototypes

// Ceiling constant expression
static inline constexpr unsigned int ceilingConstantExpression(const double value) noexcept;

// Bit ceiling constant expression
static inline constexpr unsigned int bitCeilingConstantExpression(const unsigned int value) noexcept;

// Unmove
template<typename ValueType> static inline ValueType &unmove(ValueType &&value) noexcept;

// Set thread priority and affinity
static inline bool setThreadPriorityAndAffinity(const unsigned int threadIndex) noexcept;

// Securely clear
static inline void securelyClear(void *data, const size_t length) noexcept;

// Get number of CPU cores
static inline unsigned int getNumberOfCpuCores() noexcept;

// Get number of high performance CPU cores
static inline unsigned int getNumberOfHighPerformanceCpuCores() noexcept;


// Supporting function implementation

// Prevent sleep constructor
PreventSleep::PreventSleep() noexcept

	// Check if using Windows
	#ifdef _WIN32
	
		{
		
		// Check if preventing sleep failed
		if(!SetThreadExecutionState(ES_CONTINUOUS | ES_SYSTEM_REQUIRED)) {
		
			// Display message
			cout << "Preventing sleep failed" << endl;
			
			// Exit failure
			exit(EXIT_FAILURE);
		}
		
	// Otherwise check if using an Apple device
	#elif defined __APPLE__
	
		{
		
		// Check if using macOS
		#if TARGET_OS_OSX == 1
		
			// Check if preventing sleep failed
			if(IOPMAssertionCreateWithName(kIOPMAssertionTypePreventUserIdleSystemSleep, kIOPMAssertionLevelOn, CFSTR(TO_STRING(NAME) " is running"), &assertionID) != kIOReturnSuccess) {
			
				// Display message
				cout << "Preventing sleep failed" << endl;
				
				// Exit failure
				exit(EXIT_FAILURE);
			}
			
		// Otherwise
		#else
		
			// Display message
			cout << "Preventing sleep failed" << endl;
		#endif
		
	// Otherwise
	#else
	
		:
		
			// Set allow sleep to nothing
			allowSleepMessage(nullptr, dbus_message_unref)
		{
		
		// Check if connecting to the session bus was successful
		DBusConnection *connection = dbus_bus_get(DBUS_BUS_SESSION, nullptr);
		if(connection) {
		
			// Set application identifier and reason
			const char *applicationIdentifier = TO_STRING(NAME);
			const char *reason = TO_STRING(NAME) " is running";
				
			// Check if creating message to prevent sleep for GNOME-compliant environments was successful
			unique_ptr<DBusMessage, decltype(&dbus_message_unref)> message(dbus_message_new_method_call("org.gnome.SessionManager", "/org/gnome/SessionManager", "org.gnome.SessionManager", "Inhibit"), dbus_message_unref);
			if(message) {
			
				// Check if setting message's application identifier, toplevel X window identifier, reason, and flags arguments was successful
				if(dbus_message_append_args(message.get(), DBUS_TYPE_STRING, &applicationIdentifier, DBUS_TYPE_UINT32, &unmove(static_cast<dbus_uint32_t>(0)), DBUS_TYPE_STRING, &reason, DBUS_TYPE_UINT32, &unmove(static_cast<dbus_uint32_t>(GNOME_INHIBIT_SUSPENDING_SESSION)), DBUS_TYPE_INVALID)) {
				
					// Check if getting message's reply was successful
					const unique_ptr<DBusMessage, decltype(&dbus_message_unref)> reply(dbus_connection_send_with_reply_and_block(connection, message.get(), DBUS_TIMEOUT_USE_DEFAULT, nullptr), dbus_message_unref);
					if(reply) {
					
						// Check if getting reply's inhibit cookie was successful
						dbus_uint32_t inhibitCookie;
						if(dbus_message_get_args(reply.get(), nullptr, DBUS_TYPE_UINT32, &inhibitCookie, DBUS_TYPE_INVALID)) {
						
							// Check if creating message to allow sleep was successful
							allowSleepMessage = unique_ptr<DBusMessage, decltype(&dbus_message_unref)>(dbus_message_new_method_call("org.gnome.SessionManager", "/org/gnome/SessionManager", "org.gnome.SessionManager", "Uninhibit"), dbus_message_unref);
							if(allowSleepMessage) {
							
								// Check if setting allow sleep message's inhibit cookit argument failed
								if(!dbus_message_append_args(allowSleepMessage.get(), DBUS_TYPE_UINT32, &inhibitCookie, DBUS_TYPE_INVALID)) {
								
									// Free allow sleep message
									allowSleepMessage.reset();
								}
								
								// Otherwise
								else {
								
									// Return
									return;
								}
							}
						}
					}
				}
			}
			
			// Check if creating message to prevent sleep for Freedesktop-compliant environments was successful
			message = unique_ptr<DBusMessage, decltype(&dbus_message_unref)>(dbus_message_new_method_call("org.freedesktop.PowerManagement", "/org/freedesktop/PowerManagement/Inhibit", "org.freedesktop.PowerManagement", "Inhibit"), dbus_message_unref);
			if(message) {
			
				// Check if setting message's application identifier and reason arguments was successful
				if(dbus_message_append_args(message.get(), DBUS_TYPE_STRING, &applicationIdentifier, DBUS_TYPE_STRING, &reason, DBUS_TYPE_INVALID)) {
				
					// Check if getting message's reply was successful
					const unique_ptr<DBusMessage, decltype(&dbus_message_unref)> reply(dbus_connection_send_with_reply_and_block(connection, message.get(), DBUS_TIMEOUT_USE_DEFAULT, nullptr), dbus_message_unref);
					if(reply) {
					
						// Check if getting reply's inhibit cookie was successful
						dbus_uint32_t inhibitCookie;
						if(dbus_message_get_args(reply.get(), nullptr, DBUS_TYPE_UINT32, &inhibitCookie, DBUS_TYPE_INVALID)) {
						
							// Check if creating message to allow sleep was successful
							allowSleepMessage = unique_ptr<DBusMessage, decltype(&dbus_message_unref)>(dbus_message_new_method_call("org.freedesktop.PowerManagement", "/org/freedesktop/PowerManagement/Inhibit", "org.freedesktop.PowerManagement", "Uninhibit"), dbus_message_unref);
							if(allowSleepMessage) {
							
								// Check if setting allow sleep message's inhibit cookit argument failed
								if(!dbus_message_append_args(allowSleepMessage.get(), DBUS_TYPE_UINT32, &inhibitCookie, DBUS_TYPE_INVALID)) {
								
									// Free allow sleep message
									allowSleepMessage.reset();
								}
								
								// Otherwise
								else {
								
									// Return
									return;
								}
							}
						}
					}
				}
			}
		}
		
		// Display message
		cout << "Preventing sleep failed" << endl;
	#endif
}

// Prevent sleep destructor
PreventSleep::~PreventSleep() noexcept {

	// Check if using Windows
	#ifdef _WIN32
	
		// Allow sleep
		SetThreadExecutionState(ES_CONTINUOUS);
		
	// Otherwise check if using an Apple device
	#elif defined __APPLE__
	
		// Check if using macOS
		#if TARGET_OS_OSX == 1
		
			// Allow sleep
			IOPMAssertionRelease(assertionID);
		#endif
		
	// Otherwise
	#else
	
		// Check if allow sleep message exists
		if(allowSleepMessage) {
		
			// Check if connecting to the session bus was successful
			DBusConnection *connection = dbus_bus_get(DBUS_BUS_SESSION, nullptr);
			if(connection) {
			
				// Get allow sleep message's reply
				unique_ptr<DBusMessage, decltype(&dbus_message_unref)>(dbus_connection_send_with_reply_and_block(connection, allowSleepMessage.get(), DBUS_TIMEOUT_USE_DEFAULT, nullptr), dbus_message_unref);
			}
		}
	#endif
}

// Check if not using an Apple device or using OpenCL
#if !defined __APPLE__ || defined USE_OPENCL

	// Event constructor
	Event::Event() noexcept :
	
		// Set event to nothing
		event(nullptr)
	{
	}
	
	// Event destructor
	Event::~Event() noexcept {
	
		// Wait for event to finish
		clWaitForEvents(1, &event);
		
		// Free event
		clReleaseEvent(event);
	}
	
	// Event get address
	cl_event *Event::getAddress() noexcept {
	
		// Return event's address
		return &event;
	}
	
	// Event get
	cl_event &Event::get() noexcept {
	
		// Return event
		return event;
	}
	
	// Event free
	void Event::free() noexcept {
	
		// Free event
		clReleaseEvent(event);
		
		// Set event to nothing
		event = nullptr;
	}
#endif

// Check if Windows
#ifdef _WIN32

	// Windows socket constructor
	WindowsSocket::WindowsSocket() noexcept {
	
		// Check if initializing Windows socket failed
		WSAData windowsSocketData;
		if(WSAStartup(MAKEWORD(WindowsSocket::MAJOR_VERSION, WindowsSocket::MINOR_VERSION), &windowsSocketData)) {
		
			// Display message
			cout << "Initializing Windows socket failed" << endl;
			
			// Exit failure
			exit(EXIT_FAILURE);
		}
	}
	
	// Windows socket destructor
	WindowsSocket::~WindowsSocket() noexcept {
	
		// Clean up Windows socket
		WSACleanup();
	}
#endif

// Ceiling constant expression
constexpr unsigned int ceilingConstantExpression(const double value) noexcept {

	// Get value as an integer
	const unsigned int integerValue = static_cast<unsigned int>(value);
	
	// Return one more than the integer if it's less than the value
	return (integerValue < value) ? integerValue + 1 : integerValue;
}

// Bit ceiling constant expression
constexpr unsigned int bitCeilingConstantExpression(const unsigned int value) noexcept {

	// Return power of two not less than value
	return 1 << (sizeof(value) * BITS_IN_A_BYTE - __builtin_clz(value - 1));
}

// Unmove
template<typename ValueType> ValueType &unmove(ValueType &&value) noexcept {

	// Return value as an lvalue
	return static_cast<ValueType &>(value);
}

// Check if using Windows
#ifdef _WIN32

	// Set thread priority and affinity
	bool setThreadPriorityAndAffinity(__attribute__((unused)) const unsigned int threadIndex) noexcept {
	
// Otherwise
#else

	// Set thread priority and affinity
	bool setThreadPriorityAndAffinity(const unsigned int threadIndex) noexcept {
#endif

	// Check if using Windows
	#ifdef _WIN32
	
		// Check if setting thread's scheduling priority to max failed
		if(!SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_TIME_CRITICAL)) {
		
			// Return false
			return false;
		}
		
		// Check if setting thread's CPU affinity to its own core failed (This makes the program slower on Windows, so commenting it out)
		/*if(!SetThreadAffinityMask(GetCurrentThread(), static_cast<uint64_t>(1) << threadIndex)) {
		
			// Return false
			return false;
		}*/
		
	// Otherwise check if using an Apple device
	#elif defined __APPLE__
	
		// Check if setting thread's scheduling priority to max failed
		if(pthread_set_qos_class_self_np(QOS_CLASS_USER_INTERACTIVE, 0)) {
		
			// Return false
			return false;
		}
		
		// Check if setting thread's CPU affinity to its own core failed
		thread_affinity_policy_data_t affinityPolicy = {
		
			// Affinity tag
			.affinity_tag = static_cast<integer_t>(threadIndex + 1)
		};
		
		const kern_return_t result = thread_policy_set(pthread_mach_thread_np(pthread_self()), THREAD_AFFINITY_POLICY, reinterpret_cast<thread_policy_t>(&affinityPolicy), THREAD_AFFINITY_POLICY_COUNT);
		if(result != KERN_SUCCESS && result != KERN_NOT_SUPPORTED) {
		
			// Return false
			return false;
		}
		
	// Otherwise
	#else
	
		// Check if getting thread's scheduling policy failed
		int schedulingPolicy;
		sched_param schedulingParameters;
		if(pthread_getschedparam(pthread_self(), &schedulingPolicy, &schedulingParameters)) {
		
			// Return false
			return false;
		}
		
		// Check if getting the max priority for the thread's scheduling policy failed
		schedulingParameters.sched_priority = sched_get_priority_max(schedulingPolicy);
		if(schedulingParameters.sched_priority == -1) {
		
			// Return false
			return false;
		}
		
		// Check if setting thread's scheduling priority to max failed
		if(pthread_setschedparam(pthread_self(), schedulingPolicy, &schedulingParameters)) {
		
			// Return false
			return false;
		}
		
		// Check if setting thread's CPU affinity to its own core failed
		cpu_set_t cpuSet;
		CPU_ZERO(&cpuSet);
		CPU_SET(threadIndex, &cpuSet);
		if(pthread_setaffinity_np(pthread_self(), sizeof(cpuSet), &cpuSet)) {
		
			// Return false
			return false;
		}
	#endif
	
	// Return true
	return true;
}

// Securely clear
void securelyClear(void *data, const size_t length) noexcept {

	// Check if using Windows
	#ifdef _WIN32
	
		// Securely clear data
		SecureZeroMemory(data, length);
	
	// Otherwise check if using an Apple device
	#elif defined __APPLE__
	
		// Securely clear data
		memset_s(data, length, 0, length);
		
	// Otherwise
	#else
	
		// Securely clear data
		explicit_bzero(data, length);
	#endif
}

// Get number of CPU cores
unsigned int getNumberOfCpuCores() noexcept {

	// Check if using an Apple device
	#if __APPLE__
	
		// Check if getting the number of CPU cores was successful
		int32_t numberOfCpuCores;
		size_t numberOfCpuCoresSize = sizeof(numberOfCpuCores);
		if(!sysctlbyname("hw.logicalcpu", &numberOfCpuCores, &numberOfCpuCoresSize, nullptr, 0)) {
		
			// Return number of CPU cores
			return max(numberOfCpuCores, 1);
		}
	#endif
	
	// Return number of CPU cores
	return max(thread::hardware_concurrency(), static_cast<unsigned int>(1));
}

// Get number of high performance CPU cores
unsigned int getNumberOfHighPerformanceCpuCores() noexcept {

	// Check if using an Apple device
	#if __APPLE__
	
		// Check if getting the number of high performance CPU cores was successful
		int32_t numberOfHighPerformanceCpuCores;
		size_t numberOfHighPerformanceCpuCoresSize = sizeof(numberOfHighPerformanceCpuCores);
		if(!sysctlbyname("hw.perflevel0.logicalcpu", &numberOfHighPerformanceCpuCores, &numberOfHighPerformanceCpuCoresSize, nullptr, 0)) {
		
			// Return number of high performance CPU cores
			return max(numberOfHighPerformanceCpuCores, 1);
		}
	#endif
	
	// Return number of CPU cores
	return max(thread::hardware_concurrency(), static_cast<unsigned int>(1));
}


#endif

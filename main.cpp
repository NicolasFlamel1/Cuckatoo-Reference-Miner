// Constants

// OpenCL target version (OpenCL v1.2)
#define CL_TARGET_OPENCL_VERSION 120

// Metal target version (Metal v3.0)
#define METAL_TARGET_VERSION MTL::LanguageVersion3_0

// Check if using Windows
#ifdef _WIN32

	// System version
	#define _WIN32_WINNT _WIN32_WINNT_VISTA
	
	// Use Unicode
	#define UNICODE
	#define _UNICODE

// Otherwise check if using macOS
#elif defined __APPLE__

	// Use Metal
	#define MTL_PRIVATE_IMPLEMENTATION
	
	// Use NS
	#define NS_PRIVATE_IMPLEMENTATION
#endif


// Header files

// Check if using Windows
#ifdef _WIN32

	// Header files
	#include <ws2tcpip.h>
	#include <CL/cl.h>
	
// Otherwise check if using macOS
#elif defined __APPLE__

	// Header files
	#include <netdb.h>
	#include <poll.h>
	#include "./metal.h"
	
// Otherwise
#else

	// Header files
	#include <CL/cl.h>
	#include <netdb.h>
	#include <poll.h>
#endif

// Header files
#include <algorithm>
#include <barrier>
#include <cinttypes>
#include <condition_variable>
#include <csignal>
#include <cstring>
#include <functional>
#include <getopt.h>
#include <iostream>
#include <random>
#include <thread>
#include "./common.h"
#include "./bitmap.h"
#include "./blake2b.h"
#include "./hash_table.h"
#include "./siphash.h"
#include "./cuckatoo.h"

using namespace std;


// Configurable constants

// Max number of searching threads
#define MAX_NUMBER_OF_SEARCHING_THREADS 8

// Max number of searching threads searching edges
#define MAX_NUMBER_OF_SEARCHING_THREADS_SEARCHING_EDGES 4

// First searching thread search edges percent
#define FIRST_SEARCHING_THREAD_SEARCH_EDGES_PERCENT 0.86

// Default stratum server address
#define DEFAULT_STRATUM_SERVER_ADDRESS "localhost"

// Default stratum server port
#define DEFAULT_STRATUM_SERVER_PORT "3416"

// Server response size
#define SERVER_RESPONSE_SIZE (10 * BYTES_IN_A_KILOBYTE)

// Read timeout
#define READ_TIMEOUT_SECONDS 60

// Write timeout seconds
#define WRITE_TIMEOUT_SECONDS 60


// Global variables

// Closing
static volatile sig_atomic_t closing = false;

// Searching threads mutex
static mutex searchingThreadsMutex;

// Searching threads lock
static unique_lock searchingThreadsLock(searchingThreadsMutex);

// Searching threads data
static const void *searchingThreadsData;

// Searching threads SipHash keys
static const uint64_t __attribute__((vector_size(sizeof(uint64_t) * SIPHASH_KEYS_SIZE))) *searchingThreadsSipHashKeys;

// Searching threads solution
static uint32_t searchingThreadsSolution[SOLUTION_SIZE];

// Searching threads finished
static bool searchingThreadsFinished;

// Start searching threads trigger toggle
static bool startSearchingThreadsTriggerToggle = false;

// Start searching threads conditional variable
static condition_variable *startSearchingThreadsConditionalVariable;

// Searching threads finished conditional variable
static condition_variable *searchingThreadsFinishedConditionalVariable;

// Check if not tuning
#ifndef TUNING

	// Random number generator
	static mt19937_64 randomNumberGenerator((random_device())());
	
	// Check if using Windows
	#ifdef _WIN32
	
		// Socket descriptor
		static SOCKET socketDescriptor = INVALID_SOCKET;
		
	// Otherwise
	#else
	
		// Socket descriptor
		static int socketDescriptor = -1;
	#endif
	
	// Server response
	static char serverResponse[SERVER_RESPONSE_SIZE];
#endif

// Job height
static uint64_t jobHeight;

// Job ID
static uint64_t jobId;

// Job header
static uint8_t jobHeader[HEADER_SIZE] = {};

// Job nonce
static uint64_t jobNonce = 0;


// Function prototypes

// Trimming finished
static inline void trimmingFinished(const void *data, const uint64_t __attribute__((vector_size(sizeof(uint64_t) * SIPHASH_KEYS_SIZE))) &sipHashKeys, const uint64_t height, const uint64_t id, const uint64_t nonce) noexcept;

// Check if not tuning
#ifndef TUNING

	// Process server response
	static inline bool processServerResponse() noexcept;
	
	// Send full
	static inline bool sendFull(const char *data, size_t size) noexcept;
	
	// Receive full
	static inline bool receiveFull(char *data, const size_t size) noexcept;
#endif


// Header files
#include "./lean_trimming.h"
#include "./mean_trimming.h"
#include "./slean_trimming.h"


// Check if not tuning
#ifndef TUNING

	// Main function
	int main(int argc, char *argv[]) noexcept {
	
// Otherwise
#else

	// Main function
	int main() noexcept {
#endif

	// Display message
	cout << TO_STRING(NAME) " v" TO_STRING(VERSION) " (Cuckatoo" TO_STRING(EDGE_BITS) ", " TO_STRING(TRIMMING_ROUNDS) " trimming round(s), " TO_STRING(SLEAN_TRIMMING_PARTS) " slean trimming part(s), targeting " TO_STRING(LOCAL_RAM_KILOBYTES) " KB of GPU local memory)" << endl;
	
	// Check if not tuning
	#ifndef TUNING
	
		// Set stratum server address to the default stratum server address
		const char *stratumServerAddress = DEFAULT_STRATUM_SERVER_ADDRESS;
		
		// Set stratum server port to the default stratum server port
		const char *stratumServerPort = DEFAULT_STRATUM_SERVER_PORT;
		
		// Set stratum server username to nothing
		const char *stratumServerUsername = nullptr;
		
		// Set options
		const option options[] = {
		
			// Version
			{"version", no_argument, nullptr, 'v'},
			
			// Stratum server address
			{"stratum_server_address", required_argument, nullptr, 'a'},
			
			// Stratum server port
			{"stratum_server_port", required_argument, nullptr, 'p'},
			
			// Stratum server username
			{"stratum_server_username", required_argument, nullptr, 'u'},
			
			// Help
			{"help", no_argument, nullptr, 'h'},
			
			// End
			{}
		};
		
		// Set display help to false
		bool displayHelp = false;
		
		// Go through all options
		int option;
		while((option = getopt_long(argc, argv, "va:p:u:h", options, nullptr)) != -1) {
		
			// Check option
			switch(option) {
			
				// Version
				case 'v':
				
					// Return success
					exit(EXIT_SUCCESS);
					
					// Break
					break;
				
				// Stratum server address
				case 'a':
				
					// Check if option is invalid
					if(!optarg || !*optarg) {
					
						// Display message
						cout << argv[0] << ": invalid stratum server address -- '" << (optarg ? optarg : "") << '\'' << endl;
						
						// Set display help to true
						displayHelp = true;
					}
					
					// Otherwise
					else {
					
						// Set stratum server address to the option
						stratumServerAddress = optarg;
					}
					
					// Break
					break;
				
				// Stratum server port
				case 'p': {
				
					// Check if option is invalid
					char *end;
					errno = 0;
					const unsigned long port = optarg ? strtoul(optarg, &end, DECIMAL_NUMBER_BASE) : 0;
					if(!optarg || end == optarg || *end || !isdigit(optarg[0]) || (optarg[0] == '0' && isdigit(optarg[1])) || errno || !port || port > UINT16_MAX) {
					
						// Display message
						cout << argv[0] << ": invalid stratum server port -- '" << (optarg ? optarg : "") << '\'' << endl;
						
						// Set display help to true
						displayHelp = true;
					}
					
					// Otherwise
					else {
					
						// Set stratum server port to the option
						stratumServerPort = optarg;
					}
					
					// Break
					break;
				}
				
				// Stratum server username
				case 'u':
				
					// Check if option is invalid
					if(!optarg || !*optarg) {
					
						// Display message
						cout << argv[0] << ": invalid stratum server username -- '" << (optarg ? optarg : "") << '\'' << endl;
						
						// Set display help to true
						displayHelp = true;
					}
					
					// Otherwise
					else {
					
						// Go through all characters in the option
						for(const char *character = optarg; *character; ++character) {
						
							// Check if character is invalid
							if(!isprint(*character) || *character == '"' || *character == '\\') {
							
								// Display message
								cout << argv[0] << ": invalid stratum server username -- '" << optarg << '\'' << endl;
								
								// Set display help to true
								displayHelp = true;
								
								// Break
								break;
							}
						}
						
						// Set stratum server username to the option
						stratumServerUsername = optarg;
					}
					
					// Break
					break;
				
				// Help or default
				case 'h':
				default:
				
					// Set display help to true
					displayHelp = true;
					
					// Break
					break;
			}
		}
		
		// Check if displaying help
		if(displayHelp) {
		
			// Display message
			cout << endl << "Usage:" << endl << '\t' << argv[0] << " [options]" << endl << endl;
			cout << "Options:" << endl;
			cout << "\t-v, --version\t\t\tDisplay version information" << endl;
			cout << "\t-a, --stratum_server_address\tThe address of the stratum server to connect to (default: " << DEFAULT_STRATUM_SERVER_ADDRESS << ')' << endl;
			cout << "\t-p, --stratum_server_port\tThe port of the stratum server to connect to (default: " << DEFAULT_STRATUM_SERVER_PORT << ')' << endl;
			cout << "\t-u, --stratum_server_username\tThe username to use when logging into the stratum server" << endl;
			cout << "\t-h, --help\t\t\tDisplay help information" << endl;
			
			// Return failure
			exit(EXIT_FAILURE);
		}
	#endif
	
	// Check if setting interrupt signal handler failed
	if(signal(SIGINT, [](const int signal) noexcept {
	
		// Check if interrupt signal occurred
		if(signal == SIGINT) {
		
			// Set closing to true
			closing = true;
		}
		
	}) == SIG_ERR) {
	
		// Display message
		cout << "Setting interrupt signal handler failed" << endl;
		
		// Return failure
		exit(EXIT_FAILURE);
	}
	
	// Check if using Windows
	#ifdef _WIN32
	
		// Create Windows socket
		static const WindowsSocket windowsSocket;
		
	// Otherwise check if using macOS
	#elif defined __APPLE__
	
		// Check if creating autorelease pool failed
		static const unique_ptr<NS::AutoreleasePool, void(*)(NS::AutoreleasePool *)> autoreleasePool(NS::AutoreleasePool::alloc()->init(), [](NS::AutoreleasePool *autoreleasePool) noexcept {
		
			// Free autorelease pool
			autoreleasePool->release();
		});
		if(!autoreleasePool) {
		
			// Display message
			cout << "Creating autorelease pool failed" << endl;
			
			// Return failure
			exit(EXIT_FAILURE);
		}
	#endif
	
	// Check if not tuning
	#ifndef TUNING
	
		// Display message
		cout << "Connecting to the stratum server at " << stratumServerAddress << ':' << stratumServerPort << endl;
		
		// Check if getting address info for the stratum server failed
		const addrinfo addressInfoHints = {
		
			// Port provided
			.ai_flags = AI_NUMERICSERV,
			
			// IPv4 or IPv6
			.ai_family = AF_UNSPEC,
			
			// TCP
			.ai_socktype = SOCK_STREAM,
		};
		static addrinfo *addressInfo;
		if(getaddrinfo(stratumServerAddress, stratumServerPort, &addressInfoHints, &addressInfo)) {
		
			// Display message
			cout << "Getting address info for the stratum server failed" << endl;
			
			// Return failure
			exit(EXIT_FAILURE);
		}
		
		// Automatically free address info when done
		static const unique_ptr<addrinfo, decltype(&freeaddrinfo)> addressInfoUniquePointer(addressInfo, freeaddrinfo);
		
		// Go through all addresses for the stratum server
		for(const addrinfo *address = addressInfo; address; address = address->ai_next) {
		
			// Create socket descriptor
			socketDescriptor = socket(address->ai_family, address->ai_socktype, address->ai_protocol);
			
			// Check if using Windows
			#ifdef _WIN32
			
				// Check if creating socket descriptor was successful
				if(socketDescriptor != INVALID_SOCKET) {
				
			// Otherwise
			#else
			
				// Check if creating socket descriptor was successful
				if(socketDescriptor != -1) {
			#endif
			
				// Check if Windows
				#ifdef _WIN32
					
					// Set read timeout
					const DWORD readTimeout = READ_TIMEOUT_SECONDS * MILLISECONDS_IN_A_SECOND;
					
					// Set write timeout
					const DWORD writeTimeout = WRITE_TIMEOUT_SECONDS * MILLISECONDS_IN_A_SECOND;
				
				// Otherwise
				#else
				
					// Set read timeout
					const timeval readTimeout = {
					
						// Seconds
						.tv_sec = READ_TIMEOUT_SECONDS
					};
					
					// Set write timeout
					const timeval writeTimeout = {
					
						// Seconds
						.tv_sec = WRITE_TIMEOUT_SECONDS
					};
				#endif
				
				// Check if setting socket descriptor's read and write timeouts was successful and connecting to the address was successful
				if(!setsockopt(socketDescriptor, SOL_SOCKET, SO_RCVTIMEO, reinterpret_cast<const char *>(&readTimeout), sizeof(readTimeout)) && !setsockopt(socketDescriptor, SOL_SOCKET, SO_SNDTIMEO, reinterpret_cast<const char *>(&writeTimeout), sizeof(writeTimeout)) && !connect(socketDescriptor, address->ai_addr, address->ai_addrlen)) {
				
					// Break
					break;
				}
				
				// Otherwise
				else {
				
					// Check if using Windows
					#ifdef _WIN32
					
						// Close socket descriptor
						closesocket(socketDescriptor);
						
						// Reset socket descriptor
						socketDescriptor = INVALID_SOCKET;
						
					// Otherwise
					#else
					
						// Close socket descriptor
						close(socketDescriptor);
						
						// Reset socket descriptor
						socketDescriptor = -1;
					#endif
				}
			}
		}
		
		// Check if using Windows
		#ifdef _WIN32
		
			// Check if connecting to the stratum server failed
			if(socketDescriptor == INVALID_SOCKET) {
			
		// Otherwise
		#else
		
			// Check if connecting to the stratum server failed
			if(socketDescriptor == -1) {
		#endif
		
			// Display message
			cout << "Connecting to the stratum server failed" << endl;
			
			// Return failure
			exit(EXIT_FAILURE);
		}
		
		// Automatically free socket descriptor when done
		static const unique_ptr<decltype(socketDescriptor), void(*)(decltype(socketDescriptor) *)> socketDescriptorUniquePointer(&socketDescriptor, [](decltype(socketDescriptor) *socketDescriptorPointer) noexcept {
		
			// Check if using Windows
			#ifdef _WIN32
			
				// Shutdown socket descriptor receive and send
				shutdown(*socketDescriptorPointer, SD_BOTH);
				
				// Close socket descriptor
				closesocket(*socketDescriptorPointer);
				
			// Otherwise
			#else
			
				// Shutdown socket descriptor receive and send
				shutdown(*socketDescriptorPointer, SHUT_RDWR);
				
				// Close socket descriptor
				close(*socketDescriptorPointer);
			#endif
		});
		
		// Check if stratum server username exists
		if(stratumServerUsername) {
		
			// Display message
			cout << "Logging into the stratum server with username " << stratumServerUsername << endl;
		}
		
		// Check if sending login request to the stratum server failed
		if(!sendFull("{\"id\":\"1\",\"jsonrpc\":\"2.0\",\"method\":\"login\",\"params\":{\"login\":\"", sizeof("{\"id\":\"1\",\"jsonrpc\":\"2.0\",\"method\":\"login\",\"params\":{\"login\":\"") - sizeof('\0')) || (stratumServerUsername && !sendFull(stratumServerUsername, strlen(stratumServerUsername))) || !sendFull("\",\"pass\":\"\",\"agent\":\"" TO_STRING(NAME) "\"}}\n", sizeof("\",\"pass\":\"\",\"agent\":\"" TO_STRING(NAME) "\"}}\n") - sizeof('\0'))) {
		
			// Display message
			cout << "Sending login request to the stratum server failed" << endl;
			
			// Return failure
			exit(EXIT_FAILURE);
		}
		
		// Check if receiving response from the stratum server failed
		if(!receiveFull(serverResponse, sizeof(serverResponse))) {
		
			// Display message
			cout << "Receiving response from the stratum server failed" << endl;
			
			// Return failure
			exit(EXIT_FAILURE);
		}
		
		// Check if logging into the stratum server failed
		if(!strstr(serverResponse, "\"error\":null") && !strstr(serverResponse, "\"error\": null")) {
		
			// Display message
			cout << "Logging into the stratum server failed" << endl;
			
			// Return failure
			exit(EXIT_FAILURE);
		}
		
		// Check if sending get job template request to the stratum server failed
		if(!sendFull("{\"id\":\"1\",\"jsonrpc\":\"2.0\",\"method\":\"getjobtemplate\",\"params\":null}\n", sizeof("{\"id\":\"1\",\"jsonrpc\":\"2.0\",\"method\":\"getjobtemplate\",\"params\":null}\n") - sizeof('\0'))) {
		
			// Display message
			cout << "Sending get job template request to the stratum server failed" << endl;
			
			// Return failure
			exit(EXIT_FAILURE);
		}
		
		// Check if receiving response from the stratum server failed
		if(!receiveFull(serverResponse, sizeof(serverResponse))) {
		
			// Display message
			cout << "Receiving response from the stratum server failed" << endl;
			
			// Return failure
			exit(EXIT_FAILURE);
		}
		
		// Check if getting job from the stratum server failed
		if(!processServerResponse()) {
		
			// Display message
			cout << "Getting job from the stratum server failed" << endl;
			
			// Return failure
			exit(EXIT_FAILURE);
		}
	#endif
	
	// Create start searching threads conditional variable
	condition_variable startSearchingThreadsConditionalVariableAutomaticDuration;
	startSearchingThreadsConditionalVariable = &startSearchingThreadsConditionalVariableAutomaticDuration;
	
	// Create searching threads finished conditional variable
	condition_variable searchingThreadsFinishedConditionalVariableAutomaticDuration;
	searchingThreadsFinishedConditionalVariable = &searchingThreadsFinishedConditionalVariableAutomaticDuration;
	
	// Check if there's no trimming rounds
	#if TRIMMING_ROUNDS == 0
	
		// Get number of searching threads
		const unsigned int numberOfSearchingThreads = min(min(max(thread::hardware_concurrency(), static_cast<unsigned int>(1)), static_cast<unsigned int>(MAX_NUMBER_OF_SEARCHING_THREADS)), static_cast<unsigned int>(EDGES_BITMAP_SIZE));
		
		// Get number of searching threads searching edges
		const unsigned int numberOfSearchingThreadsSearchingEdges = min(min(numberOfSearchingThreads, static_cast<unsigned int>(MAX_NUMBER_OF_SEARCHING_THREADS_SEARCHING_EDGES)), static_cast<unsigned int>(1 + ceil(log2((1 - FIRST_SEARCHING_THREAD_SEARCH_EDGES_PERCENT) * MAX_NUMBER_OF_EDGES_AFTER_TRIMMING))));
		
		// Go through all searching threads
		uint64_t numberOfEdges[numberOfSearchingThreads];
		barrier searchingThreadsBarrier(numberOfSearchingThreads);
		const unique_ptr edges = make_unique<uint32_t[]>(MAX_NUMBER_OF_EDGES_AFTER_TRIMMING * EDGE_NUMBER_OF_COMPONENTS);
		unique_ptr<CuckatooNodeConnectionsLink[]> nodeConnections[numberOfSearchingThreadsSearchingEdges];
		unsigned int numberOfSearchingThreadsFinished = 0;
		
		for(unsigned int i = 0; i < numberOfSearchingThreads; ++i) {
		
			// Check if searching thread is searching edges
			if(i < numberOfSearchingThreadsSearchingEdges) {
			
				// Create searching thread's node connections
				nodeConnections[i] = make_unique<CuckatooNodeConnectionsLink[]>(MAX_NUMBER_OF_EDGES_AFTER_TRIMMING * 2);
			}
			
			// Create searching thread
			thread([numberOfSearchingThreads, numberOfSearchingThreadsSearchingEdges, &numberOfEdges, &searchingThreadsBarrier, edges = edges.get(), nodeConnections = nodeConnections[min(i, numberOfSearchingThreadsSearchingEdges - 1)].get(), &numberOfSearchingThreadsFinished, searchingThreadIndex = i]() noexcept {
			
				// Check if setting searching thread's priority and affinity failed
				if(!setThreadPriorityAndAffinity(searchingThreadIndex)) {
				
					// Display message
					cout << "Setting searching thread's priority and affinity failed" << endl;
					
					// Return failure
					exit(EXIT_FAILURE);
				}
				
				// Check if searching thread is searching edges
				if(searchingThreadIndex < numberOfSearchingThreadsSearchingEdges) {
				
					// Initialize thread local global variables
					initializeCuckatooThreadLocalGlobalVariables();
				}
				
				// Set searching thread's bitmap start and end
				const uint_fast32_t bitmapStart = (EDGES_BITMAP_SIZE + numberOfSearchingThreads - 1) / numberOfSearchingThreads * searchingThreadIndex;
				const uint_fast32_t bitmapEnd = min((EDGES_BITMAP_SIZE + numberOfSearchingThreads - 1) / numberOfSearchingThreads * (searchingThreadIndex + 1), EDGES_BITMAP_SIZE);
				
				// Loop forever
				unique_lock lock(searchingThreadsMutex, defer_lock);
				for(bool startTriggerTrue = true;; startTriggerTrue = !startTriggerTrue) {
				
					// Wait until starting searching threads
					lock.lock();
					startSearchingThreadsConditionalVariable->wait(lock, [startTriggerTrue]() noexcept -> bool {
					
						// Return if starting searching threads
						return startSearchingThreadsTriggerToggle == startTriggerTrue;
					});
					lock.unlock();
					
					// Go through all of the searching thread's units in the edges bitmap
					numberOfEdges[searchingThreadIndex] = 0;
					for(uint_fast32_t bitmapIndex = bitmapStart; bitmapIndex < bitmapEnd; ++bitmapIndex) {
					
						// Add number of set bits in the unit to the searching thread's number of edges
						numberOfEdges[searchingThreadIndex] += __builtin_popcountll(reinterpret_cast<const uint64_t *>(searchingThreadsData)[bitmapIndex]);
					}
					
					// Wait for all searching threads to finish counting the number of edges in their units
					searchingThreadsBarrier.arrive_and_wait();
					
					// Check if not the first searching thread
					uint64_t firstEdge = 0;
					if(searchingThreadIndex) {
					
						// Go through all previous searching threads
						for(unsigned int previousSearchingThreadIndex = searchingThreadIndex; previousSearchingThreadIndex; --previousSearchingThreadIndex) {
						
							// Add previous searching thread's number of edges to first edge
							firstEdge += numberOfEdges[previousSearchingThreadIndex - 1];
						}
					}
					
					// Go through all of the searching thread's units in the edges bitmap
					uint64_t edgeIndex = firstEdge * EDGE_NUMBER_OF_COMPONENTS;
					for(uint_fast32_t bitmapIndex = bitmapStart; bitmapIndex < bitmapEnd; ++bitmapIndex) {
					
						// Go through all set bits in the unit
						uint64_t unit = reinterpret_cast<const uint64_t *>(searchingThreadsData)[bitmapIndex];
						for(uint_fast8_t unitCurrentBitIndex = __builtin_ffsll(unit), unitPreviousBitIndex = 0; unitCurrentBitIndex; unit >>= unitCurrentBitIndex, unitPreviousBitIndex += unitCurrentBitIndex, unitCurrentBitIndex = __builtin_ffsll(unit)) {
						
							// Set edge's index
							edges[edgeIndex] = bitmapIndex * BITMAP_UNIT_WIDTH + (unitCurrentBitIndex - 1) + unitPreviousBitIndex;
							
							// Set edge's nodes
							const uint64_t __attribute__((vector_size(sizeof(uint64_t) * 2))) nonces = {static_cast<uint64_t>(edges[edgeIndex]) * 2, (static_cast<uint64_t>(edges[edgeIndex]) * 2) | 1};
							uint64_t __attribute__((vector_size(sizeof(uint64_t) * 2))) nodes;
							sipHash24<2>(&nodes, *searchingThreadsSipHashKeys, &nonces);
							edges[edgeIndex + 1] = nodes[0];
							edges[edgeIndex + 2] = nodes[1];
							
							// Go to next edge
							edgeIndex += EDGE_NUMBER_OF_COMPONENTS;
							
							// Check if shifting by the entire unit
							if(unitCurrentBitIndex == BITMAP_UNIT_WIDTH) {
							
								// Break
								break;
							}
						}
					}
					
					// Go through all next searching threads
					uint64_t totalNumberOfEdges = firstEdge + numberOfEdges[searchingThreadIndex];
					for(unsigned int nextSearchingThreadIndex = searchingThreadIndex + 1; nextSearchingThreadIndex < numberOfSearchingThreads; ++nextSearchingThreadIndex) {
					
						// Add next searching thread's number of edges to total number of edges
						totalNumberOfEdges += numberOfEdges[nextSearchingThreadIndex];
					}
					
					// Wait for all searching threads to finish getting the edges in their units
					searchingThreadsBarrier.arrive_and_wait();
					
					// Check if searching thread is searching edges
					if(searchingThreadIndex < numberOfSearchingThreadsSearchingEdges) {
					
						// Check if not the first searching thread
						if(searchingThreadIndex) {
						
							// Set first searching edges
							const uint64_t firstSearchingEdge = (1 - 1 / pow(2, searchingThreadIndex - 1)) * totalNumberOfEdges * (1 - FIRST_SEARCHING_THREAD_SEARCH_EDGES_PERCENT) + ceil(totalNumberOfEdges * FIRST_SEARCHING_THREAD_SEARCH_EDGES_PERCENT + totalNumberOfEdges * (1 - FIRST_SEARCHING_THREAD_SEARCH_EDGES_PERCENT) - (1 - 1 / pow(2, numberOfSearchingThreadsSearchingEdges - 1)) * totalNumberOfEdges * (1 - FIRST_SEARCHING_THREAD_SEARCH_EDGES_PERCENT));
							
							// Go through all previous edges
							for(uint64_t nodeConnectionsIndex = 0, edgesIndex = 0; nodeConnectionsIndex < firstSearchingEdge * 2; nodeConnectionsIndex += 2, edgesIndex += EDGE_NUMBER_OF_COMPONENTS) {
							
								// Replace newest node connection for the node on the first partition and add node connection to list
								nodeConnections[nodeConnectionsIndex] = {cuckatooUNewestNodeConnections.replace(edges[edgesIndex + 1], &nodeConnections[nodeConnectionsIndex]), edges[edgesIndex + 1], edges[edgesIndex]};
								
								// Replace newest node connection for the node on the second partition and add node connection to list
								nodeConnections[nodeConnectionsIndex + 1] = {cuckatooVNewestNodeConnections.replace(edges[edgesIndex + 2], &nodeConnections[nodeConnectionsIndex + 1]), edges[edgesIndex + 2], edges[edgesIndex]};
							}
							
							// Check if getting solution was successful
							uint32_t solution[SOLUTION_SIZE];
							if(getCuckatooSolution(solution, &nodeConnections[firstSearchingEdge * 2], &edges[firstSearchingEdge * EDGE_NUMBER_OF_COMPONENTS], (1 - 1 / pow(2, searchingThreadIndex)) * totalNumberOfEdges * (1 - FIRST_SEARCHING_THREAD_SEARCH_EDGES_PERCENT) + ceil(totalNumberOfEdges * FIRST_SEARCHING_THREAD_SEARCH_EDGES_PERCENT + totalNumberOfEdges * (1 - FIRST_SEARCHING_THREAD_SEARCH_EDGES_PERCENT) - (1 - 1 / pow(2, numberOfSearchingThreadsSearchingEdges - 1)) * totalNumberOfEdges * (1 - FIRST_SEARCHING_THREAD_SEARCH_EDGES_PERCENT)) - firstSearchingEdge)) {
							
								// Lock
								lock.lock();
								
								// Set searching threads solution to the solution
								memcpy(searchingThreadsSolution, solution, sizeof(solution));
								
								// Unlock
								lock.unlock();
							}
						}
						
						// Otherwise
						else {
						
							// Check if getting solution was successful
							uint32_t solution[SOLUTION_SIZE];
							if(getCuckatooSolution(solution, nodeConnections, edges, ceil(totalNumberOfEdges * FIRST_SEARCHING_THREAD_SEARCH_EDGES_PERCENT + totalNumberOfEdges * (1 - FIRST_SEARCHING_THREAD_SEARCH_EDGES_PERCENT) - (1 - 1 / pow(2, numberOfSearchingThreadsSearchingEdges - 1)) * totalNumberOfEdges * (1 - FIRST_SEARCHING_THREAD_SEARCH_EDGES_PERCENT)))) {
							
								// Lock
								lock.lock();
								
								// Set searching threads solution to the solution
								memcpy(searchingThreadsSolution, solution, sizeof(solution));
								
								// Unlock
								lock.unlock();
							}
						}
						
						// Reset node connections
						cuckatooUNewestNodeConnections.clear();
						cuckatooVNewestNodeConnections.clear();
					}
					
					// Check if all searching threads have finished
					lock.lock();
					if(++numberOfSearchingThreadsFinished == numberOfSearchingThreads) {
					
						// Reset number of searching threads finished
						numberOfSearchingThreadsFinished = 0;
						
						// Notify that searching threads have finished
						searchingThreadsFinished = true;
						lock.unlock();
						searchingThreadsFinishedConditionalVariable->notify_one();
					}
					
					// Otherwise
					else {
					
						// Unlock
						lock.unlock();
					}
				}
				
			}).detach();
		}
		
		// Enable all edges in edges bitmap
		Bitmap<NUMBER_OF_EDGES> edgesBitmap;
		edgesBitmap.setAllBits();
		
		// While not closing
		while(!closing) {
		
			// Get SipHash keys from job's header and nonce
			uint64_t __attribute__((vector_size(sizeof(uint64_t) * SIPHASH_KEYS_SIZE))) sipHashKeys;
			blake2b(sipHashKeys, jobHeader, jobNonce);
			
			// Trimming finished
			trimmingFinished(edgesBitmap.getBuffer(), sipHashKeys, jobHeight, jobId, jobNonce++);
		}
		
	// Otherwise
	#else
	
		// Check if using macOS
		#ifdef __APPLE__
		
			// Create mean trimming context
			static unique_ptr<MTL::Device, void(*)(MTL::Device *)> context(createMeanTrimmingContext(), [](MTL::Device *context) noexcept {
			
				// Free context
				context->release();
			});
			
		// Otherwise
		#else
		
			// Check if getting number of platforms failed or no platforms exist
			cl_uint numberOfPlatforms;
			if(clGetPlatformIDs(0, nullptr, &numberOfPlatforms) != CL_SUCCESS || !numberOfPlatforms) {
			
				// Display message
				cout << "No OpenCL platforms found" << endl;
				
				// Return failure
				exit(EXIT_FAILURE);
			}
			
			// Check if getting platforms failed
			cl_platform_id platforms[numberOfPlatforms];
			if(clGetPlatformIDs(numberOfPlatforms, platforms, nullptr) != CL_SUCCESS) {
			
				// Display message
				cout << "Getting OpenCL platforms failed" << endl;
				
				// Return failure
				exit(EXIT_FAILURE);
			}
			
			// Create mean trimming context
			static unique_ptr<remove_pointer<cl_context>::type, decltype(&clReleaseContext)> context(createMeanTrimmingContext(platforms, numberOfPlatforms), clReleaseContext);
		#endif
		
		// Check if creating mean trimming context was successful
		if(context) {
			
			// Get number of searching threads
			const unsigned int numberOfSearchingThreads = min(min(max(thread::hardware_concurrency(), static_cast<unsigned int>(1)), static_cast<unsigned int>(MAX_NUMBER_OF_SEARCHING_THREADS_SEARCHING_EDGES)), static_cast<unsigned int>(1 + ceil(log2((1 - FIRST_SEARCHING_THREAD_SEARCH_EDGES_PERCENT) * MAX_NUMBER_OF_EDGES_AFTER_TRIMMING))));
			
			// Go through all searching threads
			unique_ptr<CuckatooNodeConnectionsLink[]> nodeConnections[numberOfSearchingThreads];
			unsigned int numberOfSearchingThreadsFinished = 0;
			
			for(unsigned int i = 0; i < numberOfSearchingThreads; ++i) {
			
				// Create searching thread's node connections
				nodeConnections[i] = make_unique<CuckatooNodeConnectionsLink[]>(MAX_NUMBER_OF_EDGES_AFTER_TRIMMING * 2);
				
				// Create searching thread
				thread([numberOfSearchingThreads, nodeConnections = nodeConnections[i].get(), &numberOfSearchingThreadsFinished, searchingThreadIndex = i]() noexcept {
				
					// Check if setting searching thread's priority and affinity failed
					if(!setThreadPriorityAndAffinity(searchingThreadIndex)) {
					
						// Display message
						cout << "Setting searching thread's priority and affinity failed" << endl;
						
						// Return failure
						exit(EXIT_FAILURE);
					}
					
					// Initialize thread local global variables
					initializeCuckatooThreadLocalGlobalVariables();
					
					// Loop forever
					unique_lock lock(searchingThreadsMutex, defer_lock);
					for(bool startTriggerTrue = true;; startTriggerTrue = !startTriggerTrue) {
					
						// Wait until starting searching threads
						lock.lock();
						startSearchingThreadsConditionalVariable->wait(lock, [startTriggerTrue]() noexcept -> bool {
						
							// Return if starting searching threads
							return startSearchingThreadsTriggerToggle == startTriggerTrue;
						});
						lock.unlock();
						
						// Get number of edges
						const uint32_t &numberOfEdges = reinterpret_cast<const uint32_t *>(searchingThreadsData)[0];
						
						// Get total number of edges
						const uint32_t totalNumberOfEdges = min(numberOfEdges, MAX_NUMBER_OF_EDGES_AFTER_TRIMMING);
						
						// Get edges
						const uint32_t *edges = &reinterpret_cast<const uint32_t *>(searchingThreadsData)[1];
						
						// Check if not the first searching thread
						if(searchingThreadIndex) {
						
							// Set first searching edges
							const uint64_t firstSearchingEdge = (1 - 1 / pow(2, searchingThreadIndex - 1)) * totalNumberOfEdges * (1 - FIRST_SEARCHING_THREAD_SEARCH_EDGES_PERCENT) + ceil(totalNumberOfEdges * FIRST_SEARCHING_THREAD_SEARCH_EDGES_PERCENT + totalNumberOfEdges * (1 - FIRST_SEARCHING_THREAD_SEARCH_EDGES_PERCENT) - (1 - 1 / pow(2, numberOfSearchingThreads - 1)) * totalNumberOfEdges * (1 - FIRST_SEARCHING_THREAD_SEARCH_EDGES_PERCENT));
							
							// Go through all previous edges
							for(uint64_t nodeConnectionsIndex = 0, edgesIndex = 0; nodeConnectionsIndex < firstSearchingEdge * 2; nodeConnectionsIndex += 2, edgesIndex += EDGE_NUMBER_OF_COMPONENTS) {
							
								// Replace newest node connection for the node on the first partition and add node connection to list
								nodeConnections[nodeConnectionsIndex] = {cuckatooUNewestNodeConnections.replace(edges[edgesIndex + 1], &nodeConnections[nodeConnectionsIndex]), edges[edgesIndex + 1], edges[edgesIndex]};
								
								// Replace newest node connection for the node on the second partition and add node connection to list
								nodeConnections[nodeConnectionsIndex + 1] = {cuckatooVNewestNodeConnections.replace(edges[edgesIndex + 2], &nodeConnections[nodeConnectionsIndex + 1]), edges[edgesIndex + 2], edges[edgesIndex]};
							}
							
							// Check if getting solution was successful
							uint32_t solution[SOLUTION_SIZE];
							if(getCuckatooSolution(solution, &nodeConnections[firstSearchingEdge * 2], &edges[firstSearchingEdge * EDGE_NUMBER_OF_COMPONENTS], (1 - 1 / pow(2, searchingThreadIndex)) * totalNumberOfEdges * (1 - FIRST_SEARCHING_THREAD_SEARCH_EDGES_PERCENT) + ceil(totalNumberOfEdges * FIRST_SEARCHING_THREAD_SEARCH_EDGES_PERCENT + totalNumberOfEdges * (1 - FIRST_SEARCHING_THREAD_SEARCH_EDGES_PERCENT) - (1 - 1 / pow(2, numberOfSearchingThreads - 1)) * totalNumberOfEdges * (1 - FIRST_SEARCHING_THREAD_SEARCH_EDGES_PERCENT)) - firstSearchingEdge)) {
							
								// Lock
								lock.lock();
								
								// Set searching threads solution to the solution
								memcpy(searchingThreadsSolution, solution, sizeof(solution));
								
								// Unlock
								lock.unlock();
							}
						}
						
						// Otherwise
						else {
						
							// Check if too may edges exist
							if(numberOfEdges > MAX_NUMBER_OF_EDGES_AFTER_TRIMMING) {
							
								// Check if there's too few trimming rounds
								#if TRIMMING_ROUNDS <= 10
								
									// Display message
									cout << "Too many edges exist after trimming, so some edges won't be searched. Increase the number of trimming rounds by building this program with TRIMMING_ROUNDS=" << (TRIMMING_ROUNDS + 1) << " if this happens frequently" << endl;
									
								// Otherwise
								#else
								
									// Display message
									cout << "Too many edges exist after trimming, so some edges won't be searched. Decrease the number of trimming rounds by building this program with TRIMMING_ROUNDS=" << (TRIMMING_ROUNDS - 1) << " if this happens frequently" << endl;
								#endif
							}
							
							// Check if getting solution was successful
							uint32_t solution[SOLUTION_SIZE];
							if(getCuckatooSolution(solution, nodeConnections, edges, ceil(totalNumberOfEdges * FIRST_SEARCHING_THREAD_SEARCH_EDGES_PERCENT + totalNumberOfEdges * (1 - FIRST_SEARCHING_THREAD_SEARCH_EDGES_PERCENT) - (1 - 1 / pow(2, numberOfSearchingThreads - 1)) * totalNumberOfEdges * (1 - FIRST_SEARCHING_THREAD_SEARCH_EDGES_PERCENT)))) {
							
								// Lock
								lock.lock();
								
								// Set searching threads solution to the solution
								memcpy(searchingThreadsSolution, solution, sizeof(solution));
								
								// Unlock
								lock.unlock();
							}
						}
						
						// Reset node connections
						cuckatooUNewestNodeConnections.clear();
						cuckatooVNewestNodeConnections.clear();
						
						// Check if all searching threads have finished
						lock.lock();
						if(++numberOfSearchingThreadsFinished == numberOfSearchingThreads) {
						
							// Reset number of searching threads finished
							numberOfSearchingThreadsFinished = 0;
							
							// Notify that searching threads have finished
							searchingThreadsFinished = true;
							lock.unlock();
							searchingThreadsFinishedConditionalVariable->notify_one();
						}
						
						// Otherwise
						else {
						
							// Unlock
							lock.unlock();
						}
					}
					
				}).detach();
			}
			
			// Check if performing mean trimming loop failed
			if(!performMeanTrimmingLoop(context.get())) {
			
				// Return failure
				exit(EXIT_FAILURE);
			}
		}
		
		// Otherwise
		else {
		
			// Display message
			cout << "No applicable GPU found for mean trimming. Mean trimming requires ";
			
			// Check if RAM requirement can be expressed in bytes
			if(MEAN_TRIMMING_REQUIRED_RAM_BYTES < BYTES_IN_A_KILOBYTE / 2) {
			
				// Display message
				cout << MEAN_TRIMMING_REQUIRED_RAM_BYTES << " bytes";
			}
			
			// Otherwise check if RAM requirement can be expressed in kilobytes
			else if(MEAN_TRIMMING_REQUIRED_RAM_BYTES < BYTES_IN_A_KILOBYTE * KILOBYTES_IN_A_MEGABYTE / 2) {
			
				// Display message
				cout << (ceil(static_cast<double>(MEAN_TRIMMING_REQUIRED_RAM_BYTES) / BYTES_IN_A_KILOBYTE * 100) / 100) << " KB";
			}
			
			// Otherwise check if RAM requirement can be expressed in megabytes
			else if(MEAN_TRIMMING_REQUIRED_RAM_BYTES < BYTES_IN_A_KILOBYTE * KILOBYTES_IN_A_MEGABYTE * MEGABYTES_IN_A_GIGABYTE / 2) {
			
				// Display message
				cout << (ceil(static_cast<double>(MEAN_TRIMMING_REQUIRED_RAM_BYTES) / BYTES_IN_A_KILOBYTE / KILOBYTES_IN_A_MEGABYTE * 100) / 100) << " MB";
			}
			
			// Otherwise
			else {
			
				// Display message
				cout << (ceil(static_cast<double>(MEAN_TRIMMING_REQUIRED_RAM_BYTES) / BYTES_IN_A_KILOBYTE / KILOBYTES_IN_A_MEGABYTE / MEGABYTES_IN_A_GIGABYTE * 100) / 100) << " GB";
			}
			
			// Display message
			cout << " of RAM and " << (MEAN_TRIMMING_REQUIRED_WORK_GROUP_RAM_BYTES / BYTES_IN_A_KILOBYTE) << " KB of local memory" << endl;
			
			// Check if local RAM kilobytes is greater than its min value
			if(LOCAL_RAM_KILOBYTES > MIN_LOCAL_RAM_KILOBYTES) {
			
				// Display message
				cout << "Build this program with LOCAL_RAM_KILOBYTES=" << (LOCAL_RAM_KILOBYTES / 2) << " to reduce mean trimming's GPU local memory requirement by half" << endl;
			}
			
			// Check if using macOS
			#ifdef __APPLE__
			
				// Create slean trimming context
				context = unique_ptr<MTL::Device, void(*)(MTL::Device *)>(createSleanTrimmingContext(), [](MTL::Device *context) noexcept {
				
					// Free context
					context->release();
				});
				
			// Otherwise
			#else
			
				// Create slean trimming context
				context = unique_ptr<remove_pointer<cl_context>::type, decltype(&clReleaseContext)>(createSleanTrimmingContext(platforms, numberOfPlatforms), clReleaseContext);
			#endif
			
			// Check if creating slean trimming context was successful
			if(context) {
				
				// Get number of searching threads
				const unsigned int numberOfSearchingThreads = min(min(max(thread::hardware_concurrency(), static_cast<unsigned int>(1)), static_cast<unsigned int>(MAX_NUMBER_OF_SEARCHING_THREADS)), static_cast<unsigned int>(EDGES_BITMAP_SIZE));
				
				// Get number of searching threads searching edges
				const unsigned int numberOfSearchingThreadsSearchingEdges = min(min(numberOfSearchingThreads, static_cast<unsigned int>(MAX_NUMBER_OF_SEARCHING_THREADS_SEARCHING_EDGES)), static_cast<unsigned int>(1 + ceil(log2((1 - FIRST_SEARCHING_THREAD_SEARCH_EDGES_PERCENT) * MAX_NUMBER_OF_EDGES_AFTER_TRIMMING))));
				
				// Go through all searching threads
				uint64_t numberOfEdges[numberOfSearchingThreads];
				barrier searchingThreadsBarrier(numberOfSearchingThreads);
				const unique_ptr edges = make_unique<uint32_t[]>(MAX_NUMBER_OF_EDGES_AFTER_TRIMMING * EDGE_NUMBER_OF_COMPONENTS);
				unique_ptr<CuckatooNodeConnectionsLink[]> nodeConnections[numberOfSearchingThreadsSearchingEdges];
				unsigned int numberOfSearchingThreadsFinished = 0;
				
				for(unsigned int i = 0; i < numberOfSearchingThreads; ++i) {
				
					// Check if searching thread is searching edges
					if(i < numberOfSearchingThreadsSearchingEdges) {
					
						// Create searching thread's node connections
						nodeConnections[i] = make_unique<CuckatooNodeConnectionsLink[]>(MAX_NUMBER_OF_EDGES_AFTER_TRIMMING * 2);
					}
					
					// Create searching thread
					thread([numberOfSearchingThreads, numberOfSearchingThreadsSearchingEdges, &numberOfEdges, &searchingThreadsBarrier, edges = edges.get(), nodeConnections = nodeConnections[min(i, numberOfSearchingThreadsSearchingEdges - 1)].get(), &numberOfSearchingThreadsFinished, searchingThreadIndex = i]() noexcept {
					
						// Check if setting searching thread's priority and affinity failed
						if(!setThreadPriorityAndAffinity(searchingThreadIndex)) {
						
							// Display message
							cout << "Setting searching thread's priority and affinity failed" << endl;
							
							// Return failure
							exit(EXIT_FAILURE);
						}
						
						// Check if searching thread is searching edges
						if(searchingThreadIndex < numberOfSearchingThreadsSearchingEdges) {
						
							// Initialize thread local global variables
							initializeCuckatooThreadLocalGlobalVariables();
						}
						
						// Set searching thread's bitmap start and end
						const uint_fast32_t bitmapStart = (EDGES_BITMAP_SIZE + numberOfSearchingThreads - 1) / numberOfSearchingThreads * searchingThreadIndex;
						const uint_fast32_t bitmapEnd = min((EDGES_BITMAP_SIZE + numberOfSearchingThreads - 1) / numberOfSearchingThreads * (searchingThreadIndex + 1), EDGES_BITMAP_SIZE);
						
						// Loop forever
						unique_lock lock(searchingThreadsMutex, defer_lock);
						for(bool startTriggerTrue = true;; startTriggerTrue = !startTriggerTrue) {
						
							// Wait until starting searching threads
							lock.lock();
							startSearchingThreadsConditionalVariable->wait(lock, [startTriggerTrue]() noexcept -> bool {
							
								// Return if starting searching threads
								return startSearchingThreadsTriggerToggle == startTriggerTrue;
							});
							lock.unlock();
							
							// Go through all of the searching thread's units in the edges bitmap
							numberOfEdges[searchingThreadIndex] = 0;
							for(uint_fast32_t bitmapIndex = bitmapStart; bitmapIndex < bitmapEnd; ++bitmapIndex) {
							
								// Add number of set bits in the unit to the searching thread's number of edges
								numberOfEdges[searchingThreadIndex] += __builtin_popcountll(reinterpret_cast<const uint64_t *>(searchingThreadsData)[bitmapIndex]);
							}
							
							// Wait for all searching threads to finish counting the number of edges in their units
							searchingThreadsBarrier.arrive_and_wait();
							
							// Check if not the first searching thread
							uint64_t firstEdge = 0;
							if(searchingThreadIndex) {
							
								// Go through all previous searching threads
								for(unsigned int previousSearchingThreadIndex = searchingThreadIndex; previousSearchingThreadIndex; --previousSearchingThreadIndex) {
								
									// Add previous searching thread's number of edges to first edge
									firstEdge += numberOfEdges[previousSearchingThreadIndex - 1];
								}
							}
							
							// Check if not too many edges exist for the searching thread
							uint64_t totalNumberOfEdges;
							if(firstEdge + numberOfEdges[searchingThreadIndex] <= MAX_NUMBER_OF_EDGES_AFTER_TRIMMING) {
							
								// Go through all of the searching thread's units in the edges bitmap
								uint64_t edgeIndex = firstEdge * EDGE_NUMBER_OF_COMPONENTS;
								for(uint_fast32_t bitmapIndex = bitmapStart; bitmapIndex < bitmapEnd; ++bitmapIndex) {
								
									// Go through all set bits in the unit
									uint64_t unit = reinterpret_cast<const uint64_t *>(searchingThreadsData)[bitmapIndex];
									for(uint_fast8_t unitCurrentBitIndex = __builtin_ffsll(unit), unitPreviousBitIndex = 0; unitCurrentBitIndex; unit >>= unitCurrentBitIndex, unitPreviousBitIndex += unitCurrentBitIndex, unitCurrentBitIndex = __builtin_ffsll(unit)) {
									
										// Set edge's index
										edges[edgeIndex] = bitmapIndex * BITMAP_UNIT_WIDTH + (unitCurrentBitIndex - 1) + unitPreviousBitIndex;
										
										// Set edge's nodes
										const uint64_t __attribute__((vector_size(sizeof(uint64_t) * 2))) nonces = {static_cast<uint64_t>(edges[edgeIndex]) * 2, (static_cast<uint64_t>(edges[edgeIndex]) * 2) | 1};
										uint64_t __attribute__((vector_size(sizeof(uint64_t) * 2))) nodes;
										sipHash24<2>(&nodes, *searchingThreadsSipHashKeys, &nonces);
										edges[edgeIndex + 1] = nodes[0];
										edges[edgeIndex + 2] = nodes[1];
										
										// Go to next edge
										edgeIndex += EDGE_NUMBER_OF_COMPONENTS;
										
										// Check if shifting by the entire unit
										if(unitCurrentBitIndex == BITMAP_UNIT_WIDTH) {
										
											// Break
											break;
										}
									}
								}
								
								// Go through all next searching threads
								totalNumberOfEdges = firstEdge + numberOfEdges[searchingThreadIndex];
								for(unsigned int nextSearchingThreadIndex = searchingThreadIndex + 1; nextSearchingThreadIndex < numberOfSearchingThreads; ++nextSearchingThreadIndex) {
								
									// Check if not too many edges exist for the next searching thread
									if(totalNumberOfEdges + numberOfEdges[nextSearchingThreadIndex] <= MAX_NUMBER_OF_EDGES_AFTER_TRIMMING) {
									
										// Add next searching thread's number of edges to total number of edges
										totalNumberOfEdges += numberOfEdges[nextSearchingThreadIndex];
									}
									
									// Otherwise
									else {
									
										// Check if first searching thread
										if(!searchingThreadIndex) {
										
											// Check if there's too few trimming rounds
											#if TRIMMING_ROUNDS <= 10
											
												// Display message
												cout << "Too many edges exist after trimming, so some edges won't be searched. Increase the number of trimming rounds by building this program with TRIMMING_ROUNDS=" << (TRIMMING_ROUNDS + 1) << " if this happens frequently" << endl;
												
											// Otherwise
											#else
											
												// Display message
												cout << "Too many edges exist after trimming, so some edges won't be searched. Decrease the number of trimming rounds by building this program with TRIMMING_ROUNDS=" << (TRIMMING_ROUNDS - 1) << " if this happens frequently" << endl;
											#endif
										}
										
										// Break
										break;
									}
								}
							}
							
							// Otherwise
							else {
							
								// Check if first searching thread
								if(!searchingThreadIndex) {
								
									// Check if there's too few trimming rounds
									#if TRIMMING_ROUNDS <= 10
									
										// Display message
										cout << "Too many edges exist after trimming, so some edges won't be searched. Increase the number of trimming rounds by building this program with TRIMMING_ROUNDS=" << (TRIMMING_ROUNDS + 1) << " if this happens frequently" << endl;
										
									// Otherwise
									#else
									
										// Display message
										cout << "Too many edges exist after trimming, so some edges won't be searched. Decrease the number of trimming rounds by building this program with TRIMMING_ROUNDS=" << (TRIMMING_ROUNDS - 1) << " if this happens frequently" << endl;
									#endif
								}
								
								// Go through all searching threads
								totalNumberOfEdges = 0;
								for(unsigned int nextSearchingThreadIndex = 0; nextSearchingThreadIndex < numberOfSearchingThreads; ++nextSearchingThreadIndex) {
								
									// Check if not too many edges exist for the next searching thread
									if(totalNumberOfEdges + numberOfEdges[nextSearchingThreadIndex] <= MAX_NUMBER_OF_EDGES_AFTER_TRIMMING) {
									
										// Add next searching thread's number of edges to total number of edges
										totalNumberOfEdges += numberOfEdges[nextSearchingThreadIndex];
									}
									
									// Otherwise
									else {
									
										// Break
										break;
									}
								}
							}
							
							// Wait for all searching threads to finish getting the edges in their units
							searchingThreadsBarrier.arrive_and_wait();
							
							// Check if searching thread is searching edges
							if(searchingThreadIndex < numberOfSearchingThreadsSearchingEdges) {
							
								// Check if not the first searching thread
								if(searchingThreadIndex) {
								
									// Set first searching edges
									const uint64_t firstSearchingEdge = (1 - 1 / pow(2, searchingThreadIndex - 1)) * totalNumberOfEdges * (1 - FIRST_SEARCHING_THREAD_SEARCH_EDGES_PERCENT) + ceil(totalNumberOfEdges * FIRST_SEARCHING_THREAD_SEARCH_EDGES_PERCENT + totalNumberOfEdges * (1 - FIRST_SEARCHING_THREAD_SEARCH_EDGES_PERCENT) - (1 - 1 / pow(2, numberOfSearchingThreadsSearchingEdges - 1)) * totalNumberOfEdges * (1 - FIRST_SEARCHING_THREAD_SEARCH_EDGES_PERCENT));
									
									// Go through all previous edges
									for(uint64_t nodeConnectionsIndex = 0, edgesIndex = 0; nodeConnectionsIndex < firstSearchingEdge * 2; nodeConnectionsIndex += 2, edgesIndex += EDGE_NUMBER_OF_COMPONENTS) {
									
										// Replace newest node connection for the node on the first partition and add node connection to list
										nodeConnections[nodeConnectionsIndex] = {cuckatooUNewestNodeConnections.replace(edges[edgesIndex + 1], &nodeConnections[nodeConnectionsIndex]), edges[edgesIndex + 1], edges[edgesIndex]};
										
										// Replace newest node connection for the node on the second partition and add node connection to list
										nodeConnections[nodeConnectionsIndex + 1] = {cuckatooVNewestNodeConnections.replace(edges[edgesIndex + 2], &nodeConnections[nodeConnectionsIndex + 1]), edges[edgesIndex + 2], edges[edgesIndex]};
									}
									
									// Check if getting solution was successful
									uint32_t solution[SOLUTION_SIZE];
									if(getCuckatooSolution(solution, &nodeConnections[firstSearchingEdge * 2], &edges[firstSearchingEdge * EDGE_NUMBER_OF_COMPONENTS], (1 - 1 / pow(2, searchingThreadIndex)) * totalNumberOfEdges * (1 - FIRST_SEARCHING_THREAD_SEARCH_EDGES_PERCENT) + ceil(totalNumberOfEdges * FIRST_SEARCHING_THREAD_SEARCH_EDGES_PERCENT + totalNumberOfEdges * (1 - FIRST_SEARCHING_THREAD_SEARCH_EDGES_PERCENT) - (1 - 1 / pow(2, numberOfSearchingThreadsSearchingEdges - 1)) * totalNumberOfEdges * (1 - FIRST_SEARCHING_THREAD_SEARCH_EDGES_PERCENT)) - firstSearchingEdge)) {
									
										// Lock
										lock.lock();
										
										// Set searching threads solution to the solution
										memcpy(searchingThreadsSolution, solution, sizeof(solution));
										
										// Unlock
										lock.unlock();
									}
								}
								
								// Otherwise
								else {
								
									// Check if getting solution was successful
									uint32_t solution[SOLUTION_SIZE];
									if(getCuckatooSolution(solution, nodeConnections, edges, ceil(totalNumberOfEdges * FIRST_SEARCHING_THREAD_SEARCH_EDGES_PERCENT + totalNumberOfEdges * (1 - FIRST_SEARCHING_THREAD_SEARCH_EDGES_PERCENT) - (1 - 1 / pow(2, numberOfSearchingThreadsSearchingEdges - 1)) * totalNumberOfEdges * (1 - FIRST_SEARCHING_THREAD_SEARCH_EDGES_PERCENT)))) {
									
										// Lock
										lock.lock();
										
										// Set searching threads solution to the solution
										memcpy(searchingThreadsSolution, solution, sizeof(solution));
										
										// Unlock
										lock.unlock();
									}
								}
								
								// Reset node connections
								cuckatooUNewestNodeConnections.clear();
								cuckatooVNewestNodeConnections.clear();
							}
							
							// Check if all searching threads have finished
							lock.lock();
							if(++numberOfSearchingThreadsFinished == numberOfSearchingThreads) {
							
								// Reset number of searching threads finished
								numberOfSearchingThreadsFinished = 0;
								
								// Notify that searching threads have finished
								searchingThreadsFinished = true;
								lock.unlock();
								searchingThreadsFinishedConditionalVariable->notify_one();
							}
							
							// Otherwise
							else {
							
								// Unlock
								lock.unlock();
							}
						}
						
					}).detach();
				}
				
				// Check if performing slean trimming loop failed
				if(!performSleanTrimmingLoop(context.get())) {
				
					// Return failure
					exit(EXIT_FAILURE);
				}
			}
			
			// Otherwise
			else {
			
				// Display message
				cout << "No applicable GPU found for slean trimming. Slean trimming requires ";
				
				// Check if RAM requirement can be expressed in bytes
				if(SLEAN_TRIMMING_REQUIRED_RAM_BYTES < BYTES_IN_A_KILOBYTE / 2) {
				
					// Display message
					cout << SLEAN_TRIMMING_REQUIRED_RAM_BYTES << " bytes";
				}
				
				// Otherwise check if RAM requirement can be expressed in kilobytes
				else if(SLEAN_TRIMMING_REQUIRED_RAM_BYTES < BYTES_IN_A_KILOBYTE * KILOBYTES_IN_A_MEGABYTE / 2) {
				
					// Display message
					cout << (ceil(static_cast<double>(SLEAN_TRIMMING_REQUIRED_RAM_BYTES) / BYTES_IN_A_KILOBYTE * 100) / 100) << " KB";
				}
				
				// Otherwise check if RAM requirement can be expressed in megabytes
				else if(SLEAN_TRIMMING_REQUIRED_RAM_BYTES < BYTES_IN_A_KILOBYTE * KILOBYTES_IN_A_MEGABYTE * MEGABYTES_IN_A_GIGABYTE / 2) {
				
					// Display message
					cout << (ceil(static_cast<double>(SLEAN_TRIMMING_REQUIRED_RAM_BYTES) / BYTES_IN_A_KILOBYTE / KILOBYTES_IN_A_MEGABYTE * 100) / 100) << " MB";
				}
				
				// Otherwise
				else {
				
					// Display message
					cout << (ceil(static_cast<double>(SLEAN_TRIMMING_REQUIRED_RAM_BYTES) / BYTES_IN_A_KILOBYTE / KILOBYTES_IN_A_MEGABYTE / MEGABYTES_IN_A_GIGABYTE * 100) / 100) << " GB";
				}
				
				// Display message
				cout << " of RAM and " << (SLEAN_TRIMMING_REQUIRED_WORK_GROUP_RAM_BYTES / BYTES_IN_A_KILOBYTE) << " KB of local memory" << endl;
				
				// Check if slean trimming parts is less than its max value
				if(SLEAN_TRIMMING_PARTS < MAX_SLEAN_TRIMMING_PARTS) {
				
					// Display message
					cout << "Build this program with SLEAN_TRIMMING_PARTS=" << (SLEAN_TRIMMING_PARTS * 2) << " to reduce slean trimming's GPU RAM requirement by about half" << endl;
				}
				
				// Check if local RAM kilobytes is greater than its min value
				if(LOCAL_RAM_KILOBYTES > MIN_LOCAL_RAM_KILOBYTES) {
				
					// Display message
					cout << "Build this program with LOCAL_RAM_KILOBYTES=" << (LOCAL_RAM_KILOBYTES / 2) << " to reduce slean trimming's GPU local memory requirement by half" << endl;
				}
				
				// Check if using macOS
				#ifdef __APPLE__
				
					// Create lean trimming context
					context = unique_ptr<MTL::Device, void(*)(MTL::Device *)>(createLeanTrimmingContext(), [](MTL::Device *context) noexcept {
					
						// Free context
						context->release();
					});
					
				// Otherwise
				#else
				
					// Create lean trimming context
					context = unique_ptr<remove_pointer<cl_context>::type, decltype(&clReleaseContext)>(createLeanTrimmingContext(platforms, numberOfPlatforms), clReleaseContext);
				#endif
				
				// Check if creating lean trimming context was successful
				if(context) {
					
					// Get number of searching threads
					const unsigned int numberOfSearchingThreads = min(min(max(thread::hardware_concurrency(), static_cast<unsigned int>(1)), static_cast<unsigned int>(MAX_NUMBER_OF_SEARCHING_THREADS)), static_cast<unsigned int>(EDGES_BITMAP_SIZE));
					
					// Get number of searching threads searching edges
					const unsigned int numberOfSearchingThreadsSearchingEdges = min(min(numberOfSearchingThreads, static_cast<unsigned int>(MAX_NUMBER_OF_SEARCHING_THREADS_SEARCHING_EDGES)), static_cast<unsigned int>(1 + ceil(log2((1 - FIRST_SEARCHING_THREAD_SEARCH_EDGES_PERCENT) * MAX_NUMBER_OF_EDGES_AFTER_TRIMMING))));
					
					// Go through all searching threads
					uint64_t numberOfEdges[numberOfSearchingThreads];
					barrier searchingThreadsBarrier(numberOfSearchingThreads);
					const unique_ptr edges = make_unique<uint32_t[]>(MAX_NUMBER_OF_EDGES_AFTER_TRIMMING * EDGE_NUMBER_OF_COMPONENTS);
					unique_ptr<CuckatooNodeConnectionsLink[]> nodeConnections[numberOfSearchingThreadsSearchingEdges];
					unsigned int numberOfSearchingThreadsFinished = 0;
					
					for(unsigned int i = 0; i < numberOfSearchingThreads; ++i) {
					
						// Check if searching thread is searching edges
						if(i < numberOfSearchingThreadsSearchingEdges) {
						
							// Create searching thread's node connections
							nodeConnections[i] = make_unique<CuckatooNodeConnectionsLink[]>(MAX_NUMBER_OF_EDGES_AFTER_TRIMMING * 2);
						}
						
						// Create searching thread
						thread([numberOfSearchingThreads, numberOfSearchingThreadsSearchingEdges, &numberOfEdges, &searchingThreadsBarrier, edges = edges.get(), nodeConnections = nodeConnections[min(i, numberOfSearchingThreadsSearchingEdges - 1)].get(), &numberOfSearchingThreadsFinished, searchingThreadIndex = i]() noexcept {
						
							// Check if setting searching thread's priority and affinity failed
							if(!setThreadPriorityAndAffinity(searchingThreadIndex)) {
							
								// Display message
								cout << "Setting searching thread's priority and affinity failed" << endl;
								
								// Return failure
								exit(EXIT_FAILURE);
							}
							
							// Check if searching thread is searching edges
							if(searchingThreadIndex < numberOfSearchingThreadsSearchingEdges) {
							
								// Initialize thread local global variables
								initializeCuckatooThreadLocalGlobalVariables();
							}
							
							// Set searching thread's bitmap start and end
							const uint_fast32_t bitmapStart = (EDGES_BITMAP_SIZE + numberOfSearchingThreads - 1) / numberOfSearchingThreads * searchingThreadIndex;
							const uint_fast32_t bitmapEnd = min((EDGES_BITMAP_SIZE + numberOfSearchingThreads - 1) / numberOfSearchingThreads * (searchingThreadIndex + 1), EDGES_BITMAP_SIZE);
							
							// Loop forever
							unique_lock lock(searchingThreadsMutex, defer_lock);
							for(bool startTriggerTrue = true;; startTriggerTrue = !startTriggerTrue) {
							
								// Wait until starting searching threads
								lock.lock();
								startSearchingThreadsConditionalVariable->wait(lock, [startTriggerTrue]() noexcept -> bool {
								
									// Return if starting searching threads
									return startSearchingThreadsTriggerToggle == startTriggerTrue;
								});
								lock.unlock();
								
								// Go through all of the searching thread's units in the edges bitmap
								numberOfEdges[searchingThreadIndex] = 0;
								for(uint_fast32_t bitmapIndex = bitmapStart; bitmapIndex < bitmapEnd; ++bitmapIndex) {
								
									// Add number of set bits in the unit to the searching thread's number of edges
									numberOfEdges[searchingThreadIndex] += __builtin_popcountll(reinterpret_cast<const uint64_t *>(searchingThreadsData)[bitmapIndex]);
								}
								
								// Wait for all searching threads to finish counting the number of edges in their units
								searchingThreadsBarrier.arrive_and_wait();
								
								// Check if not the first searching thread
								uint64_t firstEdge = 0;
								if(searchingThreadIndex) {
								
									// Go through all previous searching threads
									for(unsigned int previousSearchingThreadIndex = searchingThreadIndex; previousSearchingThreadIndex; --previousSearchingThreadIndex) {
									
										// Add previous searching thread's number of edges to first edge
										firstEdge += numberOfEdges[previousSearchingThreadIndex - 1];
									}
								}
								
								// Check if not too many edges exist for the searching thread
								uint64_t totalNumberOfEdges;
								if(firstEdge + numberOfEdges[searchingThreadIndex] <= MAX_NUMBER_OF_EDGES_AFTER_TRIMMING) {
								
									// Go through all of the searching thread's units in the edges bitmap
									uint64_t edgeIndex = firstEdge * EDGE_NUMBER_OF_COMPONENTS;
									for(uint_fast32_t bitmapIndex = bitmapStart; bitmapIndex < bitmapEnd; ++bitmapIndex) {
									
										// Go through all set bits in the unit
										uint64_t unit = reinterpret_cast<const uint64_t *>(searchingThreadsData)[bitmapIndex];
										for(uint_fast8_t unitCurrentBitIndex = __builtin_ffsll(unit), unitPreviousBitIndex = 0; unitCurrentBitIndex; unit >>= unitCurrentBitIndex, unitPreviousBitIndex += unitCurrentBitIndex, unitCurrentBitIndex = __builtin_ffsll(unit)) {
										
											// Set edge's index
											edges[edgeIndex] = bitmapIndex * BITMAP_UNIT_WIDTH + (unitCurrentBitIndex - 1) + unitPreviousBitIndex;
											
											// Set edge's nodes
											const uint64_t __attribute__((vector_size(sizeof(uint64_t) * 2))) nonces = {static_cast<uint64_t>(edges[edgeIndex]) * 2, (static_cast<uint64_t>(edges[edgeIndex]) * 2) | 1};
											uint64_t __attribute__((vector_size(sizeof(uint64_t) * 2))) nodes;
											sipHash24<2>(&nodes, *searchingThreadsSipHashKeys, &nonces);
											edges[edgeIndex + 1] = nodes[0];
											edges[edgeIndex + 2] = nodes[1];
											
											// Go to next edge
											edgeIndex += EDGE_NUMBER_OF_COMPONENTS;
											
											// Check if shifting by the entire unit
											if(unitCurrentBitIndex == BITMAP_UNIT_WIDTH) {
											
												// Break
												break;
											}
										}
									}
									
									// Go through all next searching threads
									totalNumberOfEdges = firstEdge + numberOfEdges[searchingThreadIndex];
									for(unsigned int nextSearchingThreadIndex = searchingThreadIndex + 1; nextSearchingThreadIndex < numberOfSearchingThreads; ++nextSearchingThreadIndex) {
									
										// Check if not too many edges exist for the next searching thread
										if(totalNumberOfEdges + numberOfEdges[nextSearchingThreadIndex] <= MAX_NUMBER_OF_EDGES_AFTER_TRIMMING) {
										
											// Add next searching thread's number of edges to total number of edges
											totalNumberOfEdges += numberOfEdges[nextSearchingThreadIndex];
										}
										
										// Otherwise
										else {
										
											// Check if first searching thread
											if(!searchingThreadIndex) {
											
												// Check if there's too few trimming rounds
												#if TRIMMING_ROUNDS <= 10
												
													// Display message
													cout << "Too many edges exist after trimming, so some edges won't be searched. Increase the number of trimming rounds by building this program with TRIMMING_ROUNDS=" << (TRIMMING_ROUNDS + 1) << " if this happens frequently" << endl;
													
												// Otherwise
												#else
												
													// Display message
													cout << "Too many edges exist after trimming, so some edges won't be searched. Decrease the number of trimming rounds by building this program with TRIMMING_ROUNDS=" << (TRIMMING_ROUNDS - 1) << " if this happens frequently" << endl;
												#endif
											}
											
											// Break
											break;
										}
									}
								}
								
								// Otherwise
								else {
								
									// Check if first searching thread
									if(!searchingThreadIndex) {
									
										// Check if there's too few trimming rounds
										#if TRIMMING_ROUNDS <= 10
										
											// Display message
											cout << "Too many edges exist after trimming, so some edges won't be searched. Increase the number of trimming rounds by building this program with TRIMMING_ROUNDS=" << (TRIMMING_ROUNDS + 1) << " if this happens frequently" << endl;
											
										// Otherwise
										#else
										
											// Display message
											cout << "Too many edges exist after trimming, so some edges won't be searched. Decrease the number of trimming rounds by building this program with TRIMMING_ROUNDS=" << (TRIMMING_ROUNDS - 1) << " if this happens frequently" << endl;
										#endif
									}
									
									// Go through all searching threads
									totalNumberOfEdges = 0;
									for(unsigned int nextSearchingThreadIndex = 0; nextSearchingThreadIndex < numberOfSearchingThreads; ++nextSearchingThreadIndex) {
									
										// Check if not too many edges exist for the next searching thread
										if(totalNumberOfEdges + numberOfEdges[nextSearchingThreadIndex] <= MAX_NUMBER_OF_EDGES_AFTER_TRIMMING) {
										
											// Add next searching thread's number of edges to total number of edges
											totalNumberOfEdges += numberOfEdges[nextSearchingThreadIndex];
										}
										
										// Otherwise
										else {
										
											// Break
											break;
										}
									}
								}
								
								// Wait for all searching threads to finish getting the edges in their units
								searchingThreadsBarrier.arrive_and_wait();
								
								// Check if searching thread is searching edges
								if(searchingThreadIndex < numberOfSearchingThreadsSearchingEdges) {
								
									// Check if not the first searching thread
									if(searchingThreadIndex) {
									
										// Set first searching edges
										const uint64_t firstSearchingEdge = (1 - 1 / pow(2, searchingThreadIndex - 1)) * totalNumberOfEdges * (1 - FIRST_SEARCHING_THREAD_SEARCH_EDGES_PERCENT) + ceil(totalNumberOfEdges * FIRST_SEARCHING_THREAD_SEARCH_EDGES_PERCENT + totalNumberOfEdges * (1 - FIRST_SEARCHING_THREAD_SEARCH_EDGES_PERCENT) - (1 - 1 / pow(2, numberOfSearchingThreadsSearchingEdges - 1)) * totalNumberOfEdges * (1 - FIRST_SEARCHING_THREAD_SEARCH_EDGES_PERCENT));
										
										// Go through all previous edges
										for(uint64_t nodeConnectionsIndex = 0, edgesIndex = 0; nodeConnectionsIndex < firstSearchingEdge * 2; nodeConnectionsIndex += 2, edgesIndex += EDGE_NUMBER_OF_COMPONENTS) {
										
											// Replace newest node connection for the node on the first partition and add node connection to list
											nodeConnections[nodeConnectionsIndex] = {cuckatooUNewestNodeConnections.replace(edges[edgesIndex + 1], &nodeConnections[nodeConnectionsIndex]), edges[edgesIndex + 1], edges[edgesIndex]};
											
											// Replace newest node connection for the node on the second partition and add node connection to list
											nodeConnections[nodeConnectionsIndex + 1] = {cuckatooVNewestNodeConnections.replace(edges[edgesIndex + 2], &nodeConnections[nodeConnectionsIndex + 1]), edges[edgesIndex + 2], edges[edgesIndex]};
										}
										
										// Check if getting solution was successful
										uint32_t solution[SOLUTION_SIZE];
										if(getCuckatooSolution(solution, &nodeConnections[firstSearchingEdge * 2], &edges[firstSearchingEdge * EDGE_NUMBER_OF_COMPONENTS], (1 - 1 / pow(2, searchingThreadIndex)) * totalNumberOfEdges * (1 - FIRST_SEARCHING_THREAD_SEARCH_EDGES_PERCENT) + ceil(totalNumberOfEdges * FIRST_SEARCHING_THREAD_SEARCH_EDGES_PERCENT + totalNumberOfEdges * (1 - FIRST_SEARCHING_THREAD_SEARCH_EDGES_PERCENT) - (1 - 1 / pow(2, numberOfSearchingThreadsSearchingEdges - 1)) * totalNumberOfEdges * (1 - FIRST_SEARCHING_THREAD_SEARCH_EDGES_PERCENT)) - firstSearchingEdge)) {
										
											// Lock
											lock.lock();
											
											// Set searching threads solution to the solution
											memcpy(searchingThreadsSolution, solution, sizeof(solution));
											
											// Unlock
											lock.unlock();
										}
									}
									
									// Otherwise
									else {
									
										// Check if getting solution was successful
										uint32_t solution[SOLUTION_SIZE];
										if(getCuckatooSolution(solution, nodeConnections, edges, ceil(totalNumberOfEdges * FIRST_SEARCHING_THREAD_SEARCH_EDGES_PERCENT + totalNumberOfEdges * (1 - FIRST_SEARCHING_THREAD_SEARCH_EDGES_PERCENT) - (1 - 1 / pow(2, numberOfSearchingThreadsSearchingEdges - 1)) * totalNumberOfEdges * (1 - FIRST_SEARCHING_THREAD_SEARCH_EDGES_PERCENT)))) {
										
											// Lock
											lock.lock();
											
											// Set searching threads solution to the solution
											memcpy(searchingThreadsSolution, solution, sizeof(solution));
											
											// Unlock
											lock.unlock();
										}
									}
									
									// Reset node connections
									cuckatooUNewestNodeConnections.clear();
									cuckatooVNewestNodeConnections.clear();
								}
								
								// Check if all searching threads have finished
								lock.lock();
								if(++numberOfSearchingThreadsFinished == numberOfSearchingThreads) {
								
									// Reset number of searching threads finished
									numberOfSearchingThreadsFinished = 0;
									
									// Notify that searching threads have finished
									searchingThreadsFinished = true;
									lock.unlock();
									searchingThreadsFinishedConditionalVariable->notify_one();
								}
								
								// Otherwise
								else {
								
									// Unlock
									lock.unlock();
								}
							}
							
						}).detach();
					}
					
					// Check if performing lean trimming loop failed
					if(!performLeanTrimmingLoop(context.get())) {
					
						// Return failure
						exit(EXIT_FAILURE);
					}
				}
				
				// Otherwise
				else {
				
					// Display message
					cout << "No applicable GPU found for lean trimming. Lean trimming requires ";
					
					// Check if RAM requirement can be expressed in bytes
					if(LEAN_TRIMMING_REQUIRED_RAM_BYTES < BYTES_IN_A_KILOBYTE / 2) {
					
						// Display message
						cout << LEAN_TRIMMING_REQUIRED_RAM_BYTES << " bytes";
					}
					
					// Otherwise check if RAM requirement can be expressed in kilobytes
					else if(LEAN_TRIMMING_REQUIRED_RAM_BYTES < BYTES_IN_A_KILOBYTE * KILOBYTES_IN_A_MEGABYTE / 2) {
					
						// Display message
						cout << (ceil(static_cast<double>(LEAN_TRIMMING_REQUIRED_RAM_BYTES) / BYTES_IN_A_KILOBYTE * 100) / 100) << " KB";
					}
					
					// Otherwise check if RAM requirement can be expressed in megabytes
					else if(LEAN_TRIMMING_REQUIRED_RAM_BYTES < BYTES_IN_A_KILOBYTE * KILOBYTES_IN_A_MEGABYTE * MEGABYTES_IN_A_GIGABYTE / 2) {
					
						// Display message
						cout << (ceil(static_cast<double>(LEAN_TRIMMING_REQUIRED_RAM_BYTES) / BYTES_IN_A_KILOBYTE / KILOBYTES_IN_A_MEGABYTE * 100) / 100) << " MB";
					}
					
					// Otherwise
					else {
					
						// Display message
						cout << (ceil(static_cast<double>(LEAN_TRIMMING_REQUIRED_RAM_BYTES) / BYTES_IN_A_KILOBYTE / KILOBYTES_IN_A_MEGABYTE / MEGABYTES_IN_A_GIGABYTE * 100) / 100) << " GB";
					}
					
					// Display message
					cout << " of RAM" << endl;
					
					// Return failure
					exit(EXIT_FAILURE);
				}
			}
		}
	#endif
	
	// Return success
	exit(EXIT_SUCCESS);
}


// Supporting function implementation

// Check if not tuning
#ifndef TUNING

	// Trimming finished
	void trimmingFinished(const void *data, const uint64_t __attribute__((vector_size(sizeof(uint64_t) * SIPHASH_KEYS_SIZE))) &sipHashKeys, const uint64_t height, const uint64_t id, const uint64_t nonce) noexcept {
	
// Otherwise
#else

	// Trimming finished
	void trimmingFinished(const void *data, const uint64_t __attribute__((vector_size(sizeof(uint64_t) * SIPHASH_KEYS_SIZE))) &sipHashKeys, __attribute__((unused)) const uint64_t height, __attribute__((unused)) const uint64_t id, __attribute__((unused)) const uint64_t nonce) noexcept {
#endif

	// Record start time
	const chrono::high_resolution_clock::time_point startTime = chrono::high_resolution_clock::now();
	
	// Start searching threads
	searchingThreadsData = data;
	searchingThreadsSipHashKeys = &sipHashKeys;
	searchingThreadsSolution[1] = 0;
	searchingThreadsFinished = false;
	startSearchingThreadsTriggerToggle = !startSearchingThreadsTriggerToggle;
	searchingThreadsLock.unlock();
	startSearchingThreadsConditionalVariable->notify_all();
	
	// Wait until searching threads have finished
	searchingThreadsLock.lock();
	searchingThreadsFinishedConditionalVariable->wait(searchingThreadsLock, []() noexcept -> bool {
	
		// Return if searching threads have finished
		return searchingThreadsFinished;
	});
	
	// Check if not tuning
	#ifndef TUNING
	
		// Initialize solutions found
		static uint64_t solutionsFound = 0;
		
		// Display message
		cout << "Solutions:\t" << solutionsFound << endl;
		
		// Check if searching threads found a solution
		if(searchingThreadsSolution[1]) {
		
			// Increment solutions found
			++solutionsFound;
			
			// Check if creating submit request failed
			static char submitRequest[sizeof("{\"id\":\"1\",\"jsonrpc\":\"2.0\",\"method\":\"submit\",\"params\":{\"edge_bits\":" TO_STRING(EDGE_BITS) ",\"height\":") - sizeof('\0') + sizeof("18446744073709551615") - sizeof('\0') + sizeof(",\"job_id\":") - sizeof('\0') + sizeof("18446744073709551615") - sizeof('\0') + sizeof(",\"nonce\":") - sizeof('\0') + sizeof("18446744073709551615") - sizeof('\0') + sizeof(",\"pow\":[") - sizeof('\0') + (sizeof("4294967295,") - sizeof('\0')) * SOLUTION_SIZE - sizeof(',') + sizeof("]}}\n")];
			const int requestSize = snprintf(submitRequest, sizeof(submitRequest), "{\"id\":\"1\",\"jsonrpc\":\"2.0\",\"method\":\"submit\",\"params\":{\"edge_bits\":" TO_STRING(EDGE_BITS) ",\"height\":%" PRIu64 ",\"job_id\":%" PRIu64 ",\"nonce\":%" PRIu64 ",\"pow\":[%" PRIu32 ",%" PRIu32 ",%" PRIu32 ",%" PRIu32 ",%" PRIu32 ",%" PRIu32 ",%" PRIu32 ",%" PRIu32 ",%" PRIu32 ",%" PRIu32 ",%" PRIu32 ",%" PRIu32 ",%" PRIu32 ",%" PRIu32 ",%" PRIu32 ",%" PRIu32 ",%" PRIu32 ",%" PRIu32 ",%" PRIu32 ",%" PRIu32 ",%" PRIu32 ",%" PRIu32 ",%" PRIu32 ",%" PRIu32 ",%" PRIu32 ",%" PRIu32 ",%" PRIu32 ",%" PRIu32 ",%" PRIu32 ",%" PRIu32 ",%" PRIu32 ",%" PRIu32 ",%" PRIu32 ",%" PRIu32 ",%" PRIu32 ",%" PRIu32 ",%" PRIu32 ",%" PRIu32 ",%" PRIu32 ",%" PRIu32 ",%" PRIu32 ",%" PRIu32 "]}}\n", height, id, nonce, searchingThreadsSolution[0], searchingThreadsSolution[1], searchingThreadsSolution[2], searchingThreadsSolution[3], searchingThreadsSolution[4], searchingThreadsSolution[5], searchingThreadsSolution[6], searchingThreadsSolution[7], searchingThreadsSolution[8], searchingThreadsSolution[9], searchingThreadsSolution[10], searchingThreadsSolution[11], searchingThreadsSolution[12], searchingThreadsSolution[13], searchingThreadsSolution[14], searchingThreadsSolution[15], searchingThreadsSolution[16], searchingThreadsSolution[17], searchingThreadsSolution[18], searchingThreadsSolution[19], searchingThreadsSolution[20], searchingThreadsSolution[21], searchingThreadsSolution[22], searchingThreadsSolution[23], searchingThreadsSolution[24], searchingThreadsSolution[25], searchingThreadsSolution[26], searchingThreadsSolution[27], searchingThreadsSolution[28], searchingThreadsSolution[29], searchingThreadsSolution[30], searchingThreadsSolution[31], searchingThreadsSolution[32], searchingThreadsSolution[33], searchingThreadsSolution[34], searchingThreadsSolution[35], searchingThreadsSolution[36], searchingThreadsSolution[37], searchingThreadsSolution[38], searchingThreadsSolution[39], searchingThreadsSolution[40], searchingThreadsSolution[41]);
			if(requestSize < 0) {
			
				// Display message
				cout << "Creating submit request failed" << endl;
				
				// Return failure
				exit(EXIT_FAILURE);
			}
			
			// Check if sending submit request to the stratum server failed
			if(!sendFull(submitRequest, requestSize)) {
			
				// Display message
				cout << "Sending submit request to the stratum server failed" << endl;
				
				// Return failure
				exit(EXIT_FAILURE);
			}
		}
		
		// Otherwise
		else {
		
			// Check if sending keep alive request to the stratum server failed
			if(!sendFull("{\"id\":\"1\",\"jsonrpc\":\"2.0\",\"method\":\"keepalive\",\"params\":null}\n", sizeof("{\"id\":\"1\",\"jsonrpc\":\"2.0\",\"method\":\"keepalive\",\"params\":null}\n") - sizeof('\0'))) {
			
				// Display message
				cout << "Sending get keep alive request to the stratum server failed" << endl;
				
				// Return failure
				exit(EXIT_FAILURE);
			}
		}
		
		// Loop while a response from stratum server is available
		int responseAvailable;
		do {
		
			// Check if using Windows
			#ifdef _WIN32
			
				// Check if getting if a response from the stratum server exists failed
				pollfd pollInfo = {
				
					// Socket descriptor
					.fd = socketDescriptor,
					
					// Events
					.events = POLLIN
				};
				responseAvailable = WSAPoll(&pollInfo, 1, 0);
				if(responseAvailable == SOCKET_ERROR) {
				
			// Otherwise
			#else
			
				// Check if getting if a response from the stratum server exists failed
				pollfd pollInfo = {
				
					// Socket descriptor
					.fd = socketDescriptor,
					
					// Events
					.events = POLLIN
				};
				responseAvailable = poll(&pollInfo, 1, 0);
				if(responseAvailable == -1) {
			#endif
			
				// Display message
				cout << "Getting if a response from the stratum server exists failed" << endl;
				
				// Return failure
				exit(EXIT_FAILURE);
			}
			
			// Check if a response from the stratum server exists
			if(responseAvailable) {
			
				// Check if receiving response from the stratum server failed
				decltype(function(recv))::result_type responseSize = recv(socketDescriptor, serverResponse, sizeof(serverResponse) - sizeof('\0'), MSG_PEEK);
				if(responseSize <= 0) {
				
					// Check if disconnected from the stratum server
					if(!responseSize) {
					
						// Display message
						cout << "Disconnected from the stratum server" << endl;
					}
					
					// Otherwise
					else {
					
						// Display message
						cout << "Receiving response from the stratum server failed. Error " << errno << endl;
					}
					
					// Return failure
					exit(EXIT_FAILURE);
				}
				
				// Check if full response wasn't received
				if(!memchr(serverResponse, '\n', responseSize)) {
				
					// Check if server response buffer is full
					if(responseSize == sizeof(serverResponse) - sizeof('\0')) {
					
						// Display message
						cout << "Receiving response from the stratum server failed. Buffer full" << endl;
						
						// Return failure
						exit(EXIT_FAILURE);
					}
					
					// Break
					break;
				}
				
				// Check if receiving response from the stratum server failed
				responseSize = recv(socketDescriptor, serverResponse, sizeof(serverResponse) - sizeof('\0'), 0);
				if(responseSize <= 0) {
				
					// Check if disconnected from the stratum server
					if(!responseSize) {
					
						// Display message
						cout << "Disconnected from the stratum server" << endl;
					}
					
					// Otherwise
					else {
					
						// Display message
						cout << "Receiving response from the stratum server failed. Error " << errno << endl;
					}
					
					// Return failure
					exit(EXIT_FAILURE);
				}
				
				// Null terminate server response
				serverResponse[responseSize] = '\0';
				
				// Process server response
				processServerResponse();
			}
			
		} while(responseAvailable);
	#endif
	
	// Display message
	const chrono::high_resolution_clock::time_point endTime = chrono::high_resolution_clock::now();
	cout << endl << "Pipeline stages:" << endl << "Searching:\t" << static_cast<chrono::duration<double>>(endTime - startTime).count() << " second(s)" << endl;
}

// Check if not tuning
#ifndef TUNING

	// Process server response
	bool processServerResponse() noexcept {
	
		// Initialize new job height
		unsigned long long newJobHeight;
		
		// Initialize new job ID
		unsigned long long newJobId;
		
		// Initialize new job header
		uint8_t newJobHeader[HEADER_SIZE];
		
		// Set new job found to false
		bool newJobFound = false;
		
		// Go through all complete parts of the server response
		for(char *partStart = serverResponse, *partEnd = strchr(partStart, '\n'); partStart && partEnd; partStart = &partEnd[sizeof('\n')], partEnd = strchr(partStart, '\n')) {
		
			// Null terminate the part
			*partEnd = '\0';
			
			// Check if getting part's method was successful
			const char *method = strstr(partStart, "\"method\":");
			if(method) {
			
				// Check if method is get job template or job
				if(!strncmp(&method[sizeof("\"method\":") - sizeof('\0')], "\"getjobtemplate\"", sizeof("\"getjobtemplate\"") - sizeof('\0')) || !strncmp(&method[sizeof("\"method\":") - sizeof('\0')], " \"getjobtemplate\"", sizeof(" \"getjobtemplate\"") - sizeof('\0')) || !strncmp(&method[sizeof("\"method\":") - sizeof('\0')], "\"job\"", sizeof("\"job\"") - sizeof('\0')) || !strncmp(&method[sizeof("\"method\":") - sizeof('\0')], " \"job\"", sizeof(" \"job\"") - sizeof('\0'))) {
				
					// Set new job found to false
					newJobFound = false;
					
					// Check if getting job was successful
					if(strstr(partStart, "\"error\":null") || strstr(partStart, "\"error\": null") || !strncmp(&method[sizeof("\"method\":") - sizeof('\0')], "\"job\"", sizeof("\"job\"") - sizeof('\0')) || !strncmp(&method[sizeof("\"method\":") - sizeof('\0')], " \"job\"", sizeof(" \"job\"") - sizeof('\0'))) {
					
						// Check if getting job's height was successful
						const char *height = strstr(partStart, "\"height\":");
						if(height) {
						
							// Check if getting job's height was successful
							const size_t heightValueOffset = height[sizeof("\"height\":") - sizeof('\0')] == ' ';
							char *end;
							errno = 0;
							newJobHeight = strtoull(&height[sizeof("\"height\":") - sizeof('\0') + heightValueOffset], &end, DECIMAL_NUMBER_BASE);
							if(end != &height[sizeof("\"height\":") - sizeof('\0') + heightValueOffset] && isdigit(height[sizeof("\"height\":") - sizeof('\0') + heightValueOffset]) && (height[sizeof("\"height\":") - sizeof('\0') + heightValueOffset] != '0' || !isdigit(height[sizeof("\"height\":") - sizeof('\0') + heightValueOffset + sizeof('0')])) && !errno && newJobHeight && newJobHeight <= UINT64_MAX) {
							
								// Check if getting job's ID was successful
								const char *id = strstr(partStart, "\"job_id\":");
								if(id) {
								
									// Check if getting job's ID was successful
									const size_t idValueOffset = id[sizeof("\"job_id\":") - sizeof('\0')] == ' ';
									errno = 0;
									newJobId = strtoull(&id[sizeof("\"job_id\":") - sizeof('\0') + idValueOffset], &end, DECIMAL_NUMBER_BASE);
									if(end != &id[sizeof("\"job_id\":") - sizeof('\0') + idValueOffset] && isdigit(id[sizeof("\"job_id\":") - sizeof('\0') + idValueOffset]) && (id[sizeof("\"job_id\":") - sizeof('\0') + idValueOffset] != '0' || !isdigit(id[sizeof("\"job_id\":") - sizeof('\0') + idValueOffset + sizeof('0')])) && !errno && newJobId <= UINT64_MAX) {
									
										// Check if getting job's pre-proof of work was successful
										const char *preProofOfWork = strstr(partStart, "\"pre_pow\":\"");
										if(preProofOfWork) {
										
											// Reset new job's header
											memset(newJobHeader, 0, sizeof(newJobHeader));
											
											// Go through all hex characters in the job's pre-proof of work
											const size_t preProofOfWorkValueOffset = preProofOfWork[sizeof("\"pre_pow\":\"") - sizeof('\0')] == ' ';
											for(const char *i = &preProofOfWork[sizeof("\"pre_pow\":\"") - sizeof('\0') + preProofOfWorkValueOffset]; isxdigit(*i) && !isupper(*i); ++i) {
											
												// Check if job's pre-proof of work is too long
												if((i - &preProofOfWork[sizeof("\"pre_pow\":\"") - sizeof('\0') + preProofOfWorkValueOffset]) / 2 == static_cast<ssize_t>(sizeof(newJobHeader))) {
												
													// Break
													break;
												}
												
												// Set character in new job's header
												newJobHeader[(i - &preProofOfWork[sizeof("\"pre_pow\":\"") - sizeof('\0') + preProofOfWorkValueOffset]) / 2] |= ((i - &preProofOfWork[sizeof("\"pre_pow\":\"") - sizeof('\0') + preProofOfWorkValueOffset]) % 2) ? ((*i - ((*i > '9') ? '0' + 'a' - '9' - 1 : '0')) & 0xF) : ((*i - ((*i > '9') ? '0' + 'a' - '9' - 1 : '0')) << 4);
												
												// Check if next character terminates the job's pre-proof of work and the job's pre-proof of work is the correct size
												if(i[1] == '"' && i - &preProofOfWork[sizeof("\"pre_pow\":\"") - sizeof('\0') + preProofOfWorkValueOffset] == sizeof(newJobHeader) * 2 - 1) {
												
													// Set new job found to true
													newJobFound = true;
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
		
		// Check if a new job was found
		if(newJobFound) {
		
			// Set job's height to new job's height
			jobHeight = newJobHeight;
			
			// Set job's ID to new job's ID
			jobId = newJobId;
			
			// Set job's header to new job's header
			memcpy(jobHeader, newJobHeader, sizeof(newJobHeader));
			
			// Create random job's nonce
			jobNonce = randomNumberGenerator();
			
			// Return true
			return true;
		}
		
		// Return false
		return false;
	}
	
	// Send full
	bool sendFull(const char *data, size_t size) noexcept {
	
		// Loop while there's data to send
		while(size) {
		
			// Check if using Windows
			#ifdef _WIN32
			
				// Send remaining data to the stratum server
				const int sent = send(socketDescriptor, data, size, 0);
				
			// Otherwise
			#else
			
				// Send remaining data to the stratum server
				const ssize_t sent = send(socketDescriptor, data, size, MSG_NOSIGNAL);
			#endif
			
			// Check if sending remaining data to the stratum server failed
			if(sent <= 0) {
			
				// Return false
				return false;
			}
			
			// Otherwise
			else {
			
				// Update remaining data
				data += sent;
				size -= sent;
			}
		}
		
		// Return true
		return true;
	}
	
	// Receive full
	bool receiveFull(char *data, const size_t size) noexcept {
	
		// Loop until full message is received
		decltype(function(recv))::result_type received;
		do {
		
			// Check if receiving data from the stratum server failed
			received = recv(socketDescriptor, data, size - sizeof('\0'), MSG_PEEK);
			if(received <= 0 || (static_cast<size_t>(received) == size - sizeof('\0') && !memchr(data, '\n', received))) {
			
				// Return false
				return false;
			}
		
		} while(!memchr(data, '\n', received));
		
		// Check if receiving data from the stratum server failed
		received = recv(socketDescriptor, data, size - sizeof('\0'), 0);
		if(received <= 0) {
		
			// Return false
			return false;
		}
		
		// Null terminate the data
		data[received] = '\0';
		
		// Return true
		return true;
	}
#endif

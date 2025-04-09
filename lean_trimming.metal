MTLSTR(R"(

// A pair of steps is one trimming round and the steps are called in this order: clear nodes bitmap, step one, step two, clear nodes bitmap, step three, step four, clear nodes bitmap, step three, step four, ..., clear nodes bitmap, step three, step four, get result from edges bitmap


// Header files
#include <metal_stdlib>

using namespace metal;


// Constants

// Bits in a byte
#define BITS_IN_A_BYTE 8

// Number of edges
#define NUMBER_OF_EDGES (static_cast<ulong>(1) << EDGE_BITS)

// Node mask
#define NODE_MASK (NUMBER_OF_EDGES - 1)

// SipRound rotation
#define SIP_ROUND_ROTATION 21


// Function prototypes

// Trim edges step one
[[kernel]] void trimEdgesStepOne(device atomic_uint *nodesBitmap, constant const ulong4 &sipHashKeys, const uint globalId);

// Trim edges step two
[[kernel]] void trimEdgesStepTwo(device ulong *edgesBitmap, device const uint *nodesBitmap, constant const ulong4 &sipHashKeys, const uint globalId);

// Trim edges step three
[[kernel]] void trimEdgesStepThree(device const ulong *edgesBitmap, device atomic_uint *nodesBitmap, constant const uchar &nodesInSecondPartition, constant const ulong4 &sipHashKeys, const uint globalId);

// Trim edges step four
[[kernel]] void trimEdgesStepFour(device ulong *edgesBitmap, device const uint *nodesBitmap, constant const uchar &nodesInSecondPartition, constant const ulong4 &sipHashKeys, const uint globalId);

// Clear nodes bitmap
[[kernel]] void clearNodesBitmap(device ulong *nodesBitmap, const uint globalId);

// SipHash-2-4
static inline uint sipHash24(ulong4 keys, const ulong nonce);

// SipRound
static inline void sipRound(thread ulong4 &keys);

// Set bit in bitmap
static inline void setBitInBitmap(device atomic_uint *bitmap, const uint index);

// Is bit set in bitmap
static inline bool isBitSetInBitmap(device const uint *bitmap, const uint index);


// Supporting function implementation

// Trim edges step one
[[kernel]] void trimEdgesStepOne(device atomic_uint *nodesBitmap [[buffer(0)]], constant const ulong4 &sipHashKeys [[buffer(1)]], const uint globalId [[thread_position_in_grid]]) {

	// Get work item's edge indices
	const uint indices = globalId * NUMBER_OF_EDGES_PER_STEP_ONE_WORK_ITEM;
	
	// Go through all of this work item's edges
	for(char i = 0; i < NUMBER_OF_EDGES_PER_STEP_ONE_WORK_ITEM; ++i) {
	
		// Get edge's index
		const uint edgeIndex = indices + i;
		
		// Get edge's node
		const uint node = sipHash24(sipHashKeys, static_cast<ulong>(edgeIndex) * 2);
		
		// Enable node in nodes bitmap
		setBitInBitmap(nodesBitmap, node);
	}
}

// Trim edges step two
[[kernel]] void trimEdgesStepTwo(device ulong *edgesBitmap [[buffer(2)]], device const uint *nodesBitmap [[buffer(0)]], constant const ulong4 &sipHashKeys [[buffer(1)]], const uint globalId [[thread_position_in_grid]]) {

	// Get work item's edge indices
	const uint indices = globalId * static_cast<char>(sizeof(ulong) * BITS_IN_A_BYTE);
	
	// Set edges to zero
	ulong edges = 0;
	
	// Go through all of this work item's edges
	for(char i = 0; i < static_cast<char>(sizeof(ulong) * BITS_IN_A_BYTE); ++i) {
	
		// Get edge's index
		const uint edgeIndex = indices + i;
		
		// Get edge's node
		const uint node = sipHash24(sipHashKeys, static_cast<ulong>(edgeIndex) * 2);
		
		// Check if node has a pair in the nodes bitmap
		if(isBitSetInBitmap(nodesBitmap, node ^ 1)) {
		
			// Enable edge
			edges |= static_cast<ulong>(1) << i;
		}
	}
	
	// Set edges in edges bitmap
	edgesBitmap[globalId] = edges;
}

// Trim edges step three
[[kernel]] void trimEdgesStepThree(device const ulong *edgesBitmap [[buffer(2)]], device atomic_uint *nodesBitmap [[buffer(0)]], constant const uchar &nodesInSecondPartition [[buffer(3)]], constant const ulong4 &sipHashKeys [[buffer(1)]], const uint globalId [[thread_position_in_grid]]) {

	// Get work item's edge indices
	const uint indices = globalId * static_cast<char>(sizeof(ulong) * BITS_IN_A_BYTE);
	
	// Get this work item's edges
	ulong edges = edgesBitmap[globalId];
	
	// Go through all of this work item's enabled edges
	for(char currentBitIndex = clz(edges) + 1, previousBitIndex = 0; edges && previousBitIndex != static_cast<char>(sizeof(ulong) * BITS_IN_A_BYTE); edges <<= min(currentBitIndex, static_cast<char>(sizeof(ulong) * BITS_IN_A_BYTE - 1)), previousBitIndex += currentBitIndex, currentBitIndex = clz(edges) + 1) {
	
		// Get edge's index
		const uint edgeIndex = indices + (static_cast<char>(sizeof(ulong) * BITS_IN_A_BYTE) - currentBitIndex - previousBitIndex);
		
		// Get edge's node
		const uint node = sipHash24(sipHashKeys, (static_cast<ulong>(edgeIndex) * 2) | nodesInSecondPartition);
		
		// Enable node in nodes bitmap
		setBitInBitmap(nodesBitmap, node);
	}
}

// Trim edges step four
[[kernel]] void trimEdgesStepFour(device ulong *edgesBitmap [[buffer(2)]], device const uint *nodesBitmap [[buffer(0)]], constant const uchar &nodesInSecondPartition [[buffer(3)]], constant const ulong4 &sipHashKeys [[buffer(1)]], const uint globalId [[thread_position_in_grid]]) {

	// Get work item's edge indices
	const uint indices = globalId * static_cast<char>(sizeof(ulong) * BITS_IN_A_BYTE);
	
	// Get this work item's edges
	ulong edges = edgesBitmap[globalId];
	
	// Go through all of this work item's enabled edges
	ulong remainingEdges = edges;
	for(char currentBitIndex = clz(remainingEdges) + 1, previousBitIndex = 0; remainingEdges && previousBitIndex != static_cast<char>(sizeof(ulong) * BITS_IN_A_BYTE); remainingEdges <<= min(currentBitIndex, static_cast<char>(sizeof(ulong) * BITS_IN_A_BYTE - 1)), previousBitIndex += currentBitIndex, currentBitIndex = clz(remainingEdges) + 1) {
	
		// Get edge's index
		const uint edgeIndex = indices + (static_cast<char>(sizeof(ulong) * BITS_IN_A_BYTE) - currentBitIndex - previousBitIndex);
		
		// Get edge's node
		const uint node = sipHash24(sipHashKeys, (static_cast<ulong>(edgeIndex) * 2) | nodesInSecondPartition);
		
		// Check if node doesn't have a pair in the nodes bitmap
		if(!isBitSetInBitmap(nodesBitmap, node ^ 1)) {
		
			// Disable edge
			edges ^= static_cast<ulong>(1) << (edgeIndex % static_cast<char>(sizeof(ulong) * BITS_IN_A_BYTE));
		}
	}
	
	// Set edges in edges bitmap
	edgesBitmap[globalId] = edges;
}

// Clear nodes bitmap
[[kernel]] void clearNodesBitmap(device ulong *nodesBitmap [[buffer(0)]], const uint globalId [[thread_position_in_grid]]) {

	// Set this work item's value in nodes bitmap to zero
	nodesBitmap[globalId] = 0;
}

// SipHash-2-4
uint sipHash24(ulong4 keys, const ulong nonce) {

	// Perform hash on keys
	keys.w ^= nonce;
	sipRound(keys);
	sipRound(keys);
	keys.even ^= ulong2(nonce, 255);
	sipRound(keys);
	sipRound(keys);
	sipRound(keys);
	sipRound(keys);
	keys.lo ^= keys.hi;
	
	// Check if edge bits is 32
	#if EDGE_BITS == 32
	
		// Return node from keys
		return keys.x ^ keys.y;
		
	// Otherwise
	#else
	
		// Return node from keys
		return (keys.x ^ keys.y) & NODE_MASK;
	#endif
}

// SipRound
void sipRound(thread ulong4 &keys) {

	// Perform SipRound on keys
	keys.even += keys.odd;
	keys.odd = rotate(keys.odd, ulong2(13, 16));
	keys.odd ^= keys.even;
	keys.x = rotate(keys.x, static_cast<ulong>(32));
	keys.even += keys.wy;
	keys.odd = rotate(keys.odd, ulong2(17, SIP_ROUND_ROTATION));
	keys.odd ^= keys.zx;
	keys.z = rotate(keys.z, static_cast<ulong>(32));
}

// Set bit in bitmap
void setBitInBitmap(device atomic_uint *bitmap [[buffer(0)]], const uint index) {

	// Set bit in bitmap
	atomic_fetch_or_explicit(&bitmap[index / static_cast<char>(sizeof(uint) * BITS_IN_A_BYTE)], 1 << (index % static_cast<char>(sizeof(uint) * BITS_IN_A_BYTE)), memory_order_relaxed);
}

// Is bit set in bitmap
bool isBitSetInBitmap(device const uint *bitmap [[buffer(0)]], const uint index) {

	// Return if bit is set in bitmap
	return bitmap[index / static_cast<char>(sizeof(uint) * BITS_IN_A_BYTE)] & (1 << (index % static_cast<char>(sizeof(uint) * BITS_IN_A_BYTE)));
}


)")

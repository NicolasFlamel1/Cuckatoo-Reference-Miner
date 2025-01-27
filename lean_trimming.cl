R"(

// A pair of steps is one trimming round and the steps are called in this order: clear nodes bitmap, step one, step two, clear nodes bitmap, step three, step four, clear nodes bitmap, step three, step four, ..., clear nodes bitmap, step three, step four, get result from edges bitmap


// Constants

// Bits in a byte
#define BITS_IN_A_BYTE 8

// Number of edges
#define NUMBER_OF_EDGES ((ulong)1 << EDGE_BITS)

// Edge mask
#define EDGE_MASK (NUMBER_OF_EDGES - 1)

// SipRound rotation
#define SIP_ROUND_ROTATION 21


// Function prototypes

// Check if work items per work group exists
#ifdef TRIM_EDGES_STEP_ONE_WORK_ITEMS_PER_WORK_GROUP

	// Required work group size
	__attribute__((reqd_work_group_size(TRIM_EDGES_STEP_ONE_WORK_ITEMS_PER_WORK_GROUP, 1, 1)))
#endif

// Trim edges step one
__kernel void trimEdgesStepOne(__global uint *nodesBitmap, const ulong4 sipHashKeys);

// Check if work items per work group exists
#ifdef TRIM_EDGES_STEP_TWO_WORK_ITEMS_PER_WORK_GROUP

	// Required work group size
	__attribute__((reqd_work_group_size(TRIM_EDGES_STEP_TWO_WORK_ITEMS_PER_WORK_GROUP, 1, 1)))
#endif

// Trim edges step two
__kernel void trimEdgesStepTwo(__global ulong *edgesBitmap, __global const uint *nodesBitmap, const ulong4 sipHashKeys);

// Check if work items per work group exists
#ifdef TRIM_EDGES_STEP_THREE_WORK_ITEMS_PER_WORK_GROUP

	// Required work group size
	__attribute__((reqd_work_group_size(TRIM_EDGES_STEP_THREE_WORK_ITEMS_PER_WORK_GROUP, 1, 1)))
#endif

// Trim edges step three
__kernel void trimEdgesStepThree(__global const ulong *edgesBitmap, __global uint *nodesBitmap, const uchar nodesInSecondPartition, const ulong4 sipHashKeys);

// Check if work items per work group exists
#ifdef TRIM_EDGES_STEP_FOUR_WORK_ITEMS_PER_WORK_GROUP

	// Required work group size
	__attribute__((reqd_work_group_size(TRIM_EDGES_STEP_FOUR_WORK_ITEMS_PER_WORK_GROUP, 1, 1)))
#endif

// Trim edges step four
__kernel void trimEdgesStepFour(__global ulong *edgesBitmap, __global const uint *nodesBitmap, const uchar nodesInSecondPartition, const ulong4 sipHashKeys);

// SipHash-2-4
static inline uint sipHash24(ulong4 keys, const ulong nonce);

// SipRound
static inline void sipRound(ulong4 *keys);

// Set bit in bitmap
static inline void setBitInBitmap(__global uint *bitmap, const uint index);

// Is bit set in bitmap
static inline bool isBitSetInBitmap(__global const uint *bitmap, const uint index);


// Supporting function implementation

// Trim edges step one
void trimEdgesStepOne(__global uint *nodesBitmap, const ulong4 sipHashKeys) {

	// Get global ID
	const uint globalId = get_global_id(0);
	
	// Get work item's edge indices
	const uint indices = globalId * NUMBER_OF_EDGES_PER_STEP_ONE_WORK_ITEM;
	
	// Go through all of this work item's edges
	for(char i = 0; i < NUMBER_OF_EDGES_PER_STEP_ONE_WORK_ITEM; ++i) {
	
		// Get edge's index
		const uint edgeIndex = indices + i;
		
		// Get edge's node
		const uint node = sipHash24(sipHashKeys, (ulong)edgeIndex * 2);
		
		// Enable node in nodes bitmap
		setBitInBitmap(nodesBitmap, node);
	}
}

// Trim edges step two
void trimEdgesStepTwo(__global ulong *edgesBitmap, __global const uint *nodesBitmap, const ulong4 sipHashKeys) {

	// Get global ID
	const uint globalId = get_global_id(0);
	
	// Get work item's edge indices
	const uint indices = globalId * (char)(sizeof(ulong) * BITS_IN_A_BYTE);
	
	// Set edges to zero
	ulong edges = 0;
	
	// Go through all of this work item's edges
	for(char i = 0; i < (char)(sizeof(ulong) * BITS_IN_A_BYTE); ++i) {
	
		// Get edge's index
		const uint edgeIndex = indices + i;
		
		// Get edge's node
		const uint node = sipHash24(sipHashKeys, (ulong)edgeIndex * 2);
		
		// Check if node has a pair in the nodes bitmap
		if(isBitSetInBitmap(nodesBitmap, node ^ 1)) {
		
			// Enable edge
			edges |= (ulong)1 << i;
		}
	}
	
	// Set edges in edges bitmap
	edgesBitmap[globalId] = edges;
}

// Trim edges step three
void trimEdgesStepThree(__global const ulong *edgesBitmap, __global uint *nodesBitmap, const uchar nodesInSecondPartition, const ulong4 sipHashKeys) {

	// Get global ID
	const uint globalId = get_global_id(0);
	
	// Get work item's edge indices
	const uint indices = globalId * (char)(sizeof(ulong) * BITS_IN_A_BYTE);
	
	// Get this work item's edges
	ulong edges = edgesBitmap[globalId];
	
	// Go through all of this work item's enabled edges
	for(char currentBitIndex = clz(edges) + 1, previousBitIndex = 0; edges && previousBitIndex != (char)(sizeof(ulong) * BITS_IN_A_BYTE); edges <<= min(currentBitIndex, (char)(sizeof(ulong) * BITS_IN_A_BYTE - 1)), previousBitIndex += currentBitIndex, currentBitIndex = clz(edges) + 1) {
	
		// Get edge's index
		const uint edgeIndex = indices + ((char)(sizeof(ulong) * BITS_IN_A_BYTE) - currentBitIndex - previousBitIndex);
		
		// Get edge's node
		const uint node = sipHash24(sipHashKeys, ((ulong)edgeIndex * 2) | nodesInSecondPartition);
		
		// Enable node in nodes bitmap
		setBitInBitmap(nodesBitmap, node);
	}
}

// Trim edges step four
void trimEdgesStepFour(__global ulong *edgesBitmap, __global const uint *nodesBitmap, const uchar nodesInSecondPartition, const ulong4 sipHashKeys) {

	// Get global ID
	const uint globalId = get_global_id(0);
	
	// Get work item's edge indices
	const uint indices = globalId * (char)(sizeof(ulong) * BITS_IN_A_BYTE);
	
	// Get this work item's edges
	ulong edges = edgesBitmap[globalId];
	
	// Go through all of this work item's enabled edges
	ulong remainingEdges = edges;
	for(char currentBitIndex = clz(remainingEdges) + 1, previousBitIndex = 0; remainingEdges && previousBitIndex != (char)(sizeof(ulong) * BITS_IN_A_BYTE); remainingEdges <<= min(currentBitIndex, (char)(sizeof(ulong) * BITS_IN_A_BYTE - 1)), previousBitIndex += currentBitIndex, currentBitIndex = clz(remainingEdges) + 1) {
	
		// Get edge's index
		const uint edgeIndex = indices + ((char)(sizeof(ulong) * BITS_IN_A_BYTE) - currentBitIndex - previousBitIndex);
		
		// Get edge's node
		const uint node = sipHash24(sipHashKeys, ((ulong)edgeIndex * 2) | nodesInSecondPartition);
		
		// Check if node doesn't have a pair in the nodes bitmap
		if(!isBitSetInBitmap(nodesBitmap, node ^ 1)) {
		
			// Disable edge
			edges ^= (ulong)1 << (edgeIndex % (char)(sizeof(ulong) * BITS_IN_A_BYTE));
		}
	}
	
	// Set edges in edges bitmap
	edgesBitmap[globalId] = edges;
}

// SipHash-2-4
uint sipHash24(ulong4 keys, const ulong nonce) {

	// Perform hash on keys
	keys.w ^= nonce;
	sipRound(&keys);
	sipRound(&keys);
	keys.even ^= (ulong2)(nonce, 255);
	sipRound(&keys);
	sipRound(&keys);
	sipRound(&keys);
	sipRound(&keys);
	
	// Return node from keys
	keys.lo ^= keys.hi;
	return (keys.x ^ keys.y) & EDGE_MASK;
}

// SipRound
void sipRound(ulong4 *keys) {

	// Perform SipRound on keys
	keys->even += keys->odd;
	keys->odd = rotate(keys->odd, (ulong2)(13, 16));
	keys->odd ^= keys->even;
	keys->x = rotate(keys->x, (ulong)32);
	keys->even += keys->wy;
	keys->odd = rotate(keys->odd, (ulong2)(17, SIP_ROUND_ROTATION));
	keys->odd ^= keys->zx;
	keys->z = rotate(keys->z, (ulong)32);
}

// Set bit in bitmap
void setBitInBitmap(__global uint *bitmap, const uint index) {

	// Set bit in bitmap
	atomic_or(&bitmap[index / (char)(sizeof(uint) * BITS_IN_A_BYTE)], 1 << (index % (char)(sizeof(uint) * BITS_IN_A_BYTE)));
}

// Is bit set in bitmap
bool isBitSetInBitmap(__global const uint *bitmap, const uint index) {

	// Return if bit is set in bitmap
	return bitmap[index / (char)(sizeof(uint) * BITS_IN_A_BYTE)] & (1 << (index % (char)(sizeof(uint) * BITS_IN_A_BYTE)));
}


)"

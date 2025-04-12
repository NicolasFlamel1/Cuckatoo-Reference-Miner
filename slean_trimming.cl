R"(

// Steps one through six are one trimming round and the steps are called in this order: clear number of edges per bucket one, step one, step two, clear number of edges per bucket one, step one, step three, ..., clear number of edges per bucket one, step one, step three, clear number of edges per bucket one, step one, clear number of edges per bucket two, step four, step five, clear number of edges per bucket one, step one, clear number of edges per bucket two, step six, step five, ..., , clear number of edges per bucket one, step one, clear number of edges per bucket two, step six, step five, get result from edges bitmap


// Constants

// Bits in a byte
#define BITS_IN_A_BYTE 8

// Number of edges
#define NUMBER_OF_EDGES ((ulong)1 << EDGE_BITS)

// Node mask
#define NODE_MASK (NUMBER_OF_EDGES - 1)

// SipRound rotation
#define SIP_ROUND_ROTATION 21

// Bitmap mask
#define BITMAP_MASK (NUMBER_OF_BITMAP_BYTES * BITS_IN_A_BYTE - 1)

// Remaining edges bitmap mask
#define REMAINING_EDGES_BITMAP_MASK (NUMBER_OF_REMAINING_EDGES_BITMAP_BYTES * BITS_IN_A_BYTE - 1)


// Function prototypes

// Check if work items per work group exists
#ifdef TRIM_EDGES_STEP_ONE_WORK_ITEMS_PER_WORK_GROUP

	// Required work group size
	__attribute__((reqd_work_group_size(TRIM_EDGES_STEP_ONE_WORK_ITEMS_PER_WORK_GROUP, 1, 1)))
#endif

// Check if local buckets size is one
#if LOCAL_BUCKETS_SIZE == 1

	// Check if initial buckets isn't disjointed
	#if INITIAL_BUCKETS_NUMBER_OF_BUCKETS == NUMBER_OF_BUCKETS
	
		// Trim edges step one
		__kernel void trimEdgesStepOne(__global uint *buckets, __global uint *numberOfEdgesPerBucket, const uchar part, const ulong4 sipHashKeys);
	
	// Otherwise
	#else
	
		// Trim edges step one
		__kernel void trimEdgesStepOne(__global uint *buckets, __global uint *numberOfEdgesPerBucket, const uchar part, const ulong4 sipHashKeys, __global uint *bucketsSecondPart);
	#endif

// Otherwise check if local buckets size is two
#elif LOCAL_BUCKETS_SIZE == 2

	// Check if initial buckets isn't disjointed
	#if INITIAL_BUCKETS_NUMBER_OF_BUCKETS == NUMBER_OF_BUCKETS
	
		// Trim edges step one
		__kernel void trimEdgesStepOne(__global uint2 *buckets, __global uint *numberOfEdgesPerBucket, const uchar part, const ulong4 sipHashKeys);
	
	// Otherwise
	#else
	
		// Trim edges step one
		__kernel void trimEdgesStepOne(__global uint2 *buckets, __global uint *numberOfEdgesPerBucket, const uchar part, const ulong4 sipHashKeys, __global uint2 *bucketsSecondPart);
	#endif

// Otherwise
#else

	// Check if initial buckets isn't disjointed
	#if INITIAL_BUCKETS_NUMBER_OF_BUCKETS == NUMBER_OF_BUCKETS
	
		// Trim edges step one
		__kernel void trimEdgesStepOne(__global uint4 *buckets, __global uint *numberOfEdgesPerBucket, const uchar part, const ulong4 sipHashKeys);
	
	// Otherwise
	#else
	
		// Trim edges step one
		__kernel void trimEdgesStepOne(__global uint4 *buckets, __global uint *numberOfEdgesPerBucket, const uchar part, const ulong4 sipHashKeys, __global uint4 *bucketsSecondPart);
	#endif
#endif

// Check if work items per work group exists
#ifdef TRIM_EDGES_STEP_TWO_WORK_ITEMS_PER_WORK_GROUP

	// Required work group size
	__attribute__((reqd_work_group_size(TRIM_EDGES_STEP_TWO_WORK_ITEMS_PER_WORK_GROUP, 1, 1)))
#endif

// Check if initial buckets isn't disjointed
#if INITIAL_BUCKETS_NUMBER_OF_BUCKETS == NUMBER_OF_BUCKETS

	// Trim edges step two
	__kernel void trimEdgesStepTwo(__global const uint *buckets, __global const uint *numberOfEdgesPerBucket, __global uint *nodesBitmap, const ulong4 sipHashKeys);

// Otherwise
#else

	// Trim edges step two
	__kernel void trimEdgesStepTwo(__global const uint *buckets, __global const uint *numberOfEdgesPerBucket, __global uint *nodesBitmap, const ulong4 sipHashKeys, __global const uint *bucketsSecondPart);
#endif

// Check if work items per work group exists
#ifdef TRIM_EDGES_STEP_THREE_WORK_ITEMS_PER_WORK_GROUP

	// Required work group size
	__attribute__((reqd_work_group_size(TRIM_EDGES_STEP_THREE_WORK_ITEMS_PER_WORK_GROUP, 1, 1)))
#endif

// Check if initial buckets isn't disjointed
#if INITIAL_BUCKETS_NUMBER_OF_BUCKETS == NUMBER_OF_BUCKETS

	// Trim edges step three
	__kernel void trimEdgesStepThree(__global const uint *buckets, __global const uint *numberOfEdgesPerBucket, __global uint *nodesBitmap, const ulong4 sipHashKeys);

// Otherwise
#else

	// Trim edges step three
	__kernel void trimEdgesStepThree(__global const uint *buckets, __global const uint *numberOfEdgesPerBucket, __global uint *nodesBitmap, const ulong4 sipHashKeys, __global const uint *bucketsSecondPart);
#endif

// Check if work items per work group exists
#ifdef TRIM_EDGES_STEP_FOUR_WORK_ITEMS_PER_WORK_GROUP

	// Required work group size
	__attribute__((reqd_work_group_size(TRIM_EDGES_STEP_FOUR_WORK_ITEMS_PER_WORK_GROUP, 1, 1)))
#endif

// Check if initial buckets isn't disjointed
#if INITIAL_BUCKETS_NUMBER_OF_BUCKETS == NUMBER_OF_BUCKETS

	// Trim edges step four
	__kernel void trimEdgesStepFour(__global const uint *sourceBuckets, __global const uint *numberOfEdgesPerSourceBucket, __global uint *nodesBitmap, __global uint *destinationBuckets, __global uint *numberOfEdgesPerDestinationBucket, const ulong4 sipHashKeys);

// Otherwise
#else

	// Trim edges step four
	__kernel void trimEdgesStepFour(__global const uint *sourceBuckets, __global const uint *numberOfEdgesPerSourceBucket, __global uint *nodesBitmap, __global uint *destinationBuckets, __global uint *numberOfEdgesPerDestinationBucket, const ulong4 sipHashKeys, __global const uint *sourceBucketsSecondPart);
#endif

// Check if work items per work group exists
#ifdef TRIM_EDGES_STEP_FIVE_WORK_ITEMS_PER_WORK_GROUP

	// Required work group size
	__attribute__((reqd_work_group_size(TRIM_EDGES_STEP_FIVE_WORK_ITEMS_PER_WORK_GROUP, 1, 1)))
#endif

// Trim edges step five
__kernel void trimEdgesStepFive(__global const uint *buckets, __global const uint *numberOfEdgesPerBucket, __global uint *edgesBitmap, const uchar part);

// Check if work items per work group exists
#ifdef TRIM_EDGES_STEP_SIX_WORK_ITEMS_PER_WORK_GROUP

	// Required work group size
	__attribute__((reqd_work_group_size(TRIM_EDGES_STEP_SIX_WORK_ITEMS_PER_WORK_GROUP, 1, 1)))
#endif

// Check if initial buckets isn't disjointed
#if INITIAL_BUCKETS_NUMBER_OF_BUCKETS == NUMBER_OF_BUCKETS

	// Trim edges step six
	__kernel void trimEdgesStepSix(__global const uint *sourceBuckets, __global const uint *numberOfEdgesPerSourceBucket, __global const uint *nodesBitmap, __global uint *destinationBuckets, __global uint *numberOfEdgesPerDestinationBucket, const uchar part, const ulong4 sipHashKeys);

// Otherwise
#else

	// Trim edges step six
	__kernel void trimEdgesStepSix(__global const uint *sourceBuckets, __global const uint *numberOfEdgesPerSourceBucket, __global const uint *nodesBitmap, __global uint *destinationBuckets, __global uint *numberOfEdgesPerDestinationBucket, const uchar part, const ulong4 sipHashKeys, __global const uint *sourceBucketsSecondPart);
#endif

// SipHash-2-4
static inline uint sipHash24(ulong4 keys, const ulong nonce);

// SipRound
static inline void sipRound(ulong4 *keys);

// Set bit in bitmap
static inline void setBitInBitmap(__local uint *bitmap, const uint index);

// Clear bit in bitmap
static inline void clearBitInBitmap(__local uint *bitmap, const uint index);

// Is bit set in bitmap
static inline bool isBitSetInBitmap(__local const uint *bitmap, const uint index);


// Supporting function implementation

// Check if local buckets size is one
#if LOCAL_BUCKETS_SIZE == 1

	// Check if initial buckets isn't disjointed
	#if INITIAL_BUCKETS_NUMBER_OF_BUCKETS == NUMBER_OF_BUCKETS
	
		// Trim edges step one
		void trimEdgesStepOne(__global uint *buckets, __global uint *numberOfEdgesPerBucket, const uchar part, const ulong4 sipHashKeys) {
	
	// Otherwise
	#else
	
		// Trim edges step one
		void trimEdgesStepOne(__global uint *buckets, __global uint *numberOfEdgesPerBucket, const uchar part, const ulong4 sipHashKeys, __global uint *bucketsSecondPart) {
	#endif

// Otherwise check if local buckets size is two
#elif LOCAL_BUCKETS_SIZE == 2

	// Check if initial buckets isn't disjointed
	#if INITIAL_BUCKETS_NUMBER_OF_BUCKETS == NUMBER_OF_BUCKETS
	
		// Trim edges step one
		void trimEdgesStepOne(__global uint2 *buckets, __global uint *numberOfEdgesPerBucket, const uchar part, const ulong4 sipHashKeys) {
	
	// Otherwise
	#else
	
		// Trim edges step one
		void trimEdgesStepOne(__global uint2 *buckets, __global uint *numberOfEdgesPerBucket, const uchar part, const ulong4 sipHashKeys, __global uint2 *bucketsSecondPart) {
	#endif

// Otherwise
#else

	// Check if initial buckets isn't disjointed
	#if INITIAL_BUCKETS_NUMBER_OF_BUCKETS == NUMBER_OF_BUCKETS
	
		// Trim edges step one
		void trimEdgesStepOne(__global uint4 *buckets, __global uint *numberOfEdgesPerBucket, const uchar part, const ulong4 sipHashKeys) {
	
	// Otherwise
	#else
	
		// Trim edges step one
		void trimEdgesStepOne(__global uint4 *buckets, __global uint *numberOfEdgesPerBucket, const uchar part, const ulong4 sipHashKeys, __global uint4 *bucketsSecondPart) {
	#endif
#endif

	// Check if local buckets size is one
	#if LOCAL_BUCKETS_SIZE == 1
	
		// Get global ID
		const uint globalId = get_global_id(0);
		
		// Get work item's edge indices in the part
		const uint indices = globalId * NUMBER_OF_EDGES_PER_STEP_ONE_WORK_ITEM + part * (NUMBER_OF_EDGES / SLEAN_TRIMMING_PARTS);
		
		// Go through all of this work item's edges
		for(short i = 0; i < NUMBER_OF_EDGES_PER_STEP_ONE_WORK_ITEM; ++i) {
		
			// Get edge's index
			const uint edgeIndex = indices + i;
			
			// Get edge's node's bucket index
			const uint bucketIndex = sipHash24(sipHashKeys, (ulong)edgeIndex * 2) >> NUMBER_OF_LEAST_SIGNIFICANT_BITS_IGNORED_DURING_BUCKET_SORTING;
			
			// Get bucket's next edge index
			const uint nextEdgeIndex = min(atomic_inc(&numberOfEdgesPerBucket[bucketIndex]), (uint)(INITIAL_MAX_NUMBER_OF_EDGES_PER_BUCKET - 1));
			
			// Check if initial buckets isn't disjointed
			#if INITIAL_BUCKETS_NUMBER_OF_BUCKETS == NUMBER_OF_BUCKETS
			
				// Get bucket's next indices
				__global uint *bucketNextIndices = &buckets[(ulong)INITIAL_MAX_NUMBER_OF_EDGES_PER_BUCKET * bucketIndex + nextEdgeIndex];
			
			// Otherwise
			#else
			
				// Get bucket's next indices
				__global uint *bucketNextIndices = (bucketIndex < INITIAL_BUCKETS_NUMBER_OF_BUCKETS) ? &buckets[(ulong)INITIAL_MAX_NUMBER_OF_EDGES_PER_BUCKET * bucketIndex + nextEdgeIndex] : &bucketsSecondPart[(ulong)INITIAL_MAX_NUMBER_OF_EDGES_PER_BUCKET * (bucketIndex - INITIAL_BUCKETS_NUMBER_OF_BUCKETS) + nextEdgeIndex];
			#endif
			
			// Set bucket's next edge to the edge
			*bucketNextIndices = edgeIndex;
		}
		
	// Otherwise
	#else
	
		// Declare local buckets
		__local uint localBuckets[NUMBER_OF_BUCKETS][LOCAL_BUCKETS_SIZE - 1];
		
		// Declare number of edges per local bucket
		__local uint numberOfEdgesPerLocalBucket[(short)((NUMBER_OF_BUCKETS + sizeof(uint) - 1) / sizeof(uint))];
		
		// Get global ID
		const uint globalId = get_global_id(0);
		
		// Get local ID
		const ushort localId = get_local_id(0);
		
		// Get local size
		const ushort localSize = get_local_size(0);
		
		// Go through all groups of local buckets as a work group
		for(short i = localId; i < (short)((NUMBER_OF_BUCKETS + sizeof(uint) - 1) / sizeof(uint)); i += localSize) {
		
			// Set group of local bucket's number of edges to zero
			numberOfEdgesPerLocalBucket[i] = 0;
		}
		
		// Get work item's edge indices in the part
		const uint indices = globalId * NUMBER_OF_EDGES_PER_STEP_ONE_WORK_ITEM + part * (NUMBER_OF_EDGES / SLEAN_TRIMMING_PARTS);
		
		// Go through all of this work item's edges
		for(short i = 0; i < NUMBER_OF_EDGES_PER_STEP_ONE_WORK_ITEM; ++i) {
		
			// Get edge's index
			const uint edgeIndex = indices + i;
			
			// Get edge's node's bucket index
			const uint bucketIndex = sipHash24(sipHashKeys, (ulong)edgeIndex * 2) >> NUMBER_OF_LEAST_SIGNIFICANT_BITS_IGNORED_DURING_BUCKET_SORTING;
			
			// Synchronize work group
			barrier(CLK_LOCAL_MEM_FENCE);
			
			// Increment local bucket's number of edges
			uchar numberOfEdges = atomic_add(&numberOfEdgesPerLocalBucket[bucketIndex / (char)sizeof(uint)], 1 << ((bucketIndex % (char)sizeof(uint)) * BITS_IN_A_BYTE)) >> ((bucketIndex % (char)sizeof(uint)) * BITS_IN_A_BYTE);
			
			// Check if local bucket isn't full with this edge
			if(numberOfEdges < LOCAL_BUCKETS_SIZE - 1) {
			
				// Append edge index to local bucket
				localBuckets[bucketIndex][numberOfEdges] = edgeIndex;
			}
			
			// Loop until edges have been added to a local bucket
			for(char j = 0; j < 8 / LOCAL_BUCKETS_SIZE; ++j) {
			
				// Synchronize work group
				barrier(CLK_LOCAL_MEM_FENCE);
				
				// Check if local bucket is full with this edge
				if(numberOfEdges == LOCAL_BUCKETS_SIZE - 1) {
				
					// Get bucket's next edge index
					const uint nextEdgeIndex = min(atomic_add(&numberOfEdgesPerBucket[bucketIndex], LOCAL_BUCKETS_SIZE), (uint)(INITIAL_MAX_NUMBER_OF_EDGES_PER_BUCKET - LOCAL_BUCKETS_SIZE));
					
					// Check if local buckets size is two
					#if LOCAL_BUCKETS_SIZE == 2
					
						// Check if initial buckets isn't disjointed
						#if INITIAL_BUCKETS_NUMBER_OF_BUCKETS == NUMBER_OF_BUCKETS
						
							// Get bucket's next indices
							__global uint2 *bucketNextIndices = &buckets[((ulong)INITIAL_MAX_NUMBER_OF_EDGES_PER_BUCKET * bucketIndex + nextEdgeIndex) / 2];
						
						// Otherwise
						#else
						
							// Get bucket's next indices
							__global uint2 *bucketNextIndices = (bucketIndex < INITIAL_BUCKETS_NUMBER_OF_BUCKETS) ? &buckets[((ulong)INITIAL_MAX_NUMBER_OF_EDGES_PER_BUCKET * bucketIndex + nextEdgeIndex) / 2] : &bucketsSecondPart[((ulong)INITIAL_MAX_NUMBER_OF_EDGES_PER_BUCKET * (bucketIndex - INITIAL_BUCKETS_NUMBER_OF_BUCKETS) + nextEdgeIndex) / 2];
						#endif
						
						// Set bucket's next edges to the local bucket's edges
						*bucketNextIndices = (uint2)(localBuckets[bucketIndex][0], edgeIndex);
						
					// Otherwise
					#else
					
						// Check if initial buckets isn't disjointed
						#if INITIAL_BUCKETS_NUMBER_OF_BUCKETS == NUMBER_OF_BUCKETS
						
							// Get bucket's next indices
							__global uint4 *bucketNextIndices = &buckets[((ulong)INITIAL_MAX_NUMBER_OF_EDGES_PER_BUCKET * bucketIndex + nextEdgeIndex) / 4];
						
						// Otherwise
						#else
						
							// Get bucket's next indices
							__global uint4 *bucketNextIndices = (bucketIndex < INITIAL_BUCKETS_NUMBER_OF_BUCKETS) ? &buckets[((ulong)INITIAL_MAX_NUMBER_OF_EDGES_PER_BUCKET * bucketIndex + nextEdgeIndex) / 4] : &bucketsSecondPart[((ulong)INITIAL_MAX_NUMBER_OF_EDGES_PER_BUCKET * (bucketIndex - INITIAL_BUCKETS_NUMBER_OF_BUCKETS) + nextEdgeIndex) / 4];
						#endif
						
						// Set bucket's next edges to the local bucket's edges
						*bucketNextIndices = (uint4)(localBuckets[bucketIndex][0], localBuckets[bucketIndex][1], localBuckets[bucketIndex][2], edgeIndex);
					#endif
					
					// Update local bucket's number of edges
					atomic_sub(&numberOfEdgesPerLocalBucket[bucketIndex / (char)sizeof(uint)], LOCAL_BUCKETS_SIZE << ((bucketIndex % (char)sizeof(uint)) * BITS_IN_A_BYTE));
				}
				
				// Update number of edges
				numberOfEdges -= LOCAL_BUCKETS_SIZE;
				
				// Synchronize work group
				barrier(CLK_LOCAL_MEM_FENCE);
				
				// Check if local bucket isn't full with this edge
				if(numberOfEdges < LOCAL_BUCKETS_SIZE - 1) {
				
					// Append edge index to local bucket
					localBuckets[bucketIndex][numberOfEdges] = edgeIndex;
				}
			}
		}
		
		// Synchronize work group
		barrier(CLK_LOCAL_MEM_FENCE);
		
		// Go through all local buckets as a work group
		for(int i = localId; i < NUMBER_OF_BUCKETS; i += localSize) {
		
			// Get local bucket's number of edges
			const uchar numberOfEdges = numberOfEdgesPerLocalBucket[i / (char)sizeof(uint)] >> ((i % (char)sizeof(uint)) * BITS_IN_A_BYTE);
			
			// Check if local bucket isn't empty
			if(numberOfEdges) {
			
				// Get bucket's next edge index
				const uint nextEdgeIndex = min(atomic_add(&numberOfEdgesPerBucket[i], LOCAL_BUCKETS_SIZE), (uint)(INITIAL_MAX_NUMBER_OF_EDGES_PER_BUCKET - LOCAL_BUCKETS_SIZE));
				
				// Check if local buckets size is two
				#if LOCAL_BUCKETS_SIZE == 2
				
					// Check if initial buckets isn't disjointed
					#if INITIAL_BUCKETS_NUMBER_OF_BUCKETS == NUMBER_OF_BUCKETS
					
						// Get bucket's next indices
						__global uint2 *bucketNextIndices = &buckets[((ulong)INITIAL_MAX_NUMBER_OF_EDGES_PER_BUCKET * i + nextEdgeIndex) / 2];
					
					// Otherwise
					#else
					
						// Get bucket's next indices
						__global uint2 *bucketNextIndices = (i < INITIAL_BUCKETS_NUMBER_OF_BUCKETS) ? &buckets[((ulong)INITIAL_MAX_NUMBER_OF_EDGES_PER_BUCKET * i + nextEdgeIndex) / 2] : &bucketsSecondPart[((ulong)INITIAL_MAX_NUMBER_OF_EDGES_PER_BUCKET * (i - INITIAL_BUCKETS_NUMBER_OF_BUCKETS) + nextEdgeIndex) / 2];
					#endif
					
					// Set bucket's next edges to the local bucket's edges
					*bucketNextIndices = (uint2)(localBuckets[i][0]);
					
				// Otherwise
				#else
				
					// Check if initial buckets isn't disjointed
					#if INITIAL_BUCKETS_NUMBER_OF_BUCKETS == NUMBER_OF_BUCKETS
					
						// Get bucket's next indices
						__global uint4 *bucketNextIndices = &buckets[((ulong)INITIAL_MAX_NUMBER_OF_EDGES_PER_BUCKET * i + nextEdgeIndex) / 4];
					
					// Otherwise
					#else
					
						// Get bucket's next indices
						__global uint4 *bucketNextIndices = (i < INITIAL_BUCKETS_NUMBER_OF_BUCKETS) ? &buckets[((ulong)INITIAL_MAX_NUMBER_OF_EDGES_PER_BUCKET * i + nextEdgeIndex) / 4] : &bucketsSecondPart[((ulong)INITIAL_MAX_NUMBER_OF_EDGES_PER_BUCKET * (i - INITIAL_BUCKETS_NUMBER_OF_BUCKETS) + nextEdgeIndex) / 4];
					#endif
					
					// Check number of edges
					switch(numberOfEdges) {
					
						// One
						case 1:
						
							// Set bucket's next edges to the local bucket's edges
							*bucketNextIndices = (uint4)(localBuckets[i][0]);
							
							// Break
							break;
						
						// Two
						case 2:
						
							// Set bucket's next edges to the local bucket's edges
							*bucketNextIndices = (uint4)((uint3)(localBuckets[i][0]), localBuckets[i][1]);
							
							// Break
							break;
						
						// Default
						default:
						
							// Set bucket's next edges to the local bucket's edges
							*bucketNextIndices = (uint4)((uint2)(localBuckets[i][0]), localBuckets[i][1], localBuckets[i][2]);
					}
				#endif
			}
		}
	#endif
}

// Check if initial buckets isn't disjointed
#if INITIAL_BUCKETS_NUMBER_OF_BUCKETS == NUMBER_OF_BUCKETS

	// Trim edges step two
	void trimEdgesStepTwo(__global const uint *buckets, __global const uint *numberOfEdgesPerBucket, __global uint *nodesBitmap, const ulong4 sipHashKeys) {

// Otherwise
#else

	// Trim edges step two
	void trimEdgesStepTwo(__global const uint *buckets, __global const uint *numberOfEdgesPerBucket, __global uint *nodesBitmap, const ulong4 sipHashKeys, __global const uint *bucketsSecondPart) {
#endif

	// Declare bitmap
	__local uint bitmap[(short)(NUMBER_OF_BITMAP_BYTES / sizeof(uint))];
	
	// Get local ID
	const ushort localId = get_local_id(0);
	
	// Get local size
	const ushort localSize = get_local_size(0);
	
	// Get group ID
	const uint groupId = get_group_id(0);
	
	// Go through all groups of bytes in the bitmap as a work group
	for(short i = localId; i < (short)(NUMBER_OF_BITMAP_BYTES / sizeof(uint)); i += localSize) {
	
		// Set group of bytes in the bitmap to zero
		bitmap[i] = 0;
	}
	
	// Get number of edges in this work group's bucket
	const uint numberOfEdges = min(numberOfEdgesPerBucket[groupId], (uint)INITIAL_MAX_NUMBER_OF_EDGES_PER_BUCKET);
	
	// Check if initial buckets isn't disjointed
	#if INITIAL_BUCKETS_NUMBER_OF_BUCKETS == NUMBER_OF_BUCKETS
	
		// Get work group's bucket's indices
		__global const uint *indices = &buckets[(ulong)INITIAL_MAX_NUMBER_OF_EDGES_PER_BUCKET * groupId];
	
	// Otherwise
	#else
	
		// Get work group's bucket's indices
		__global const uint *indices = (groupId < INITIAL_BUCKETS_NUMBER_OF_BUCKETS) ? &buckets[(ulong)INITIAL_MAX_NUMBER_OF_EDGES_PER_BUCKET * groupId] : &bucketsSecondPart[(ulong)INITIAL_MAX_NUMBER_OF_EDGES_PER_BUCKET * (groupId - INITIAL_BUCKETS_NUMBER_OF_BUCKETS)];
	#endif
	
	// Synchronize work group
	barrier(CLK_LOCAL_MEM_FENCE);
	
	// Go through all edges in this work group's bucket as a work group
	for(uint i = localId; i < numberOfEdges; i += localSize) {
	
		// Get edge's index
		const uint edgeIndex = indices[i];
		
		// Enable edge's node in the bitmap
		setBitInBitmap(bitmap, sipHash24(sipHashKeys, (ulong)edgeIndex * 2) & BITMAP_MASK);
	}
	
	// Get work group's nodes bitmap's index
	__global uint *nodesBitmapIndex = &nodesBitmap[groupId * (short)(NUMBER_OF_BITMAP_BYTES / sizeof(uint))];
	
	// Synchronize work group
	barrier(CLK_LOCAL_MEM_FENCE);
	
	// Go through all groups of bytes in the bitmap as a work group
	for(short i = localId; i < (short)(NUMBER_OF_BITMAP_BYTES / sizeof(uint)); i += localSize) {
	
		// Set group of bytes in nodes bitmap to the bitmap's
		nodesBitmapIndex[i] = bitmap[i];
	}
}

// Check if initial buckets isn't disjointed
#if INITIAL_BUCKETS_NUMBER_OF_BUCKETS == NUMBER_OF_BUCKETS

	// Trim edges step three
	void trimEdgesStepThree(__global const uint *buckets, __global const uint *numberOfEdgesPerBucket, __global uint *nodesBitmap, const ulong4 sipHashKeys) {

// Otherwise
#else

	// Trim edges step three
	void trimEdgesStepThree(__global const uint *buckets, __global const uint *numberOfEdgesPerBucket, __global uint *nodesBitmap, const ulong4 sipHashKeys, __global const uint *bucketsSecondPart) {
#endif

	// Declare bitmap
	__local uint bitmap[(short)(NUMBER_OF_BITMAP_BYTES / sizeof(uint))];
	
	// Get local ID
	const ushort localId = get_local_id(0);
	
	// Get local size
	const ushort localSize = get_local_size(0);
	
	// Get group ID
	const uint groupId = get_group_id(0);
	
	// Get work group's nodes bitmap's index
	__global uint *nodesBitmapIndex = &nodesBitmap[groupId * (short)(NUMBER_OF_BITMAP_BYTES / sizeof(uint))];
	
	// Go through all groups of bytes in the bitmap as a work group
	for(short i = localId; i < (short)(NUMBER_OF_BITMAP_BYTES / sizeof(uint)); i += localSize) {
	
		// Set group of bytes in the bitmap to the nodes bitmap's
		bitmap[i] = nodesBitmapIndex[i];
	}
	
	// Get number of edges in this work group's bucket
	const uint numberOfEdges = min(numberOfEdgesPerBucket[groupId], (uint)INITIAL_MAX_NUMBER_OF_EDGES_PER_BUCKET);
	
	// Check if initial buckets isn't disjointed
	#if INITIAL_BUCKETS_NUMBER_OF_BUCKETS == NUMBER_OF_BUCKETS
	
		// Get work group's bucket's indices
		__global const uint *indices = &buckets[(ulong)INITIAL_MAX_NUMBER_OF_EDGES_PER_BUCKET * groupId];
	
	// Otherwise
	#else
	
		// Get work group's bucket's indices
		__global const uint *indices = (groupId < INITIAL_BUCKETS_NUMBER_OF_BUCKETS) ? &buckets[(ulong)INITIAL_MAX_NUMBER_OF_EDGES_PER_BUCKET * groupId] : &bucketsSecondPart[(ulong)INITIAL_MAX_NUMBER_OF_EDGES_PER_BUCKET * (groupId - INITIAL_BUCKETS_NUMBER_OF_BUCKETS)];
	#endif
	
	// Synchronize work group
	barrier(CLK_LOCAL_MEM_FENCE);
	
	// Go through all edges in this work group's bucket as a work group
	for(uint i = localId; i < numberOfEdges; i += localSize) {
	
		// Get edge's index
		const uint edgeIndex = indices[i];
		
		// Enable edge's node in the bitmap
		setBitInBitmap(bitmap, sipHash24(sipHashKeys, (ulong)edgeIndex * 2) & BITMAP_MASK);
	}
	
	// Synchronize work group
	barrier(CLK_LOCAL_MEM_FENCE);
	
	// Go through all groups of bytes in the bitmap as a work group
	for(short i = localId; i < (short)(NUMBER_OF_BITMAP_BYTES / sizeof(uint)); i += localSize) {
	
		// Set group of bytes in nodes bitmap to the bitmap's
		nodesBitmapIndex[i] = bitmap[i];
	}
}

// Check if initial buckets isn't disjointed
#if INITIAL_BUCKETS_NUMBER_OF_BUCKETS == NUMBER_OF_BUCKETS

	// Trim edges step four
	void trimEdgesStepFour(__global const uint *sourceBuckets, __global const uint *numberOfEdgesPerSourceBucket, __global uint *nodesBitmap, __global uint *destinationBuckets, __global uint *numberOfEdgesPerDestinationBucket, const ulong4 sipHashKeys) {

// Otherwise
#else

	// Trim edges step four
	void trimEdgesStepFour(__global const uint *sourceBuckets, __global const uint *numberOfEdgesPerSourceBucket, __global uint *nodesBitmap, __global uint *destinationBuckets, __global uint *numberOfEdgesPerDestinationBucket, const ulong4 sipHashKeys, __global const uint *sourceBucketsSecondPart) {
#endif

	// Declare bitmap
	__local uint bitmap[(short)(NUMBER_OF_BITMAP_BYTES / sizeof(uint))];
	
	// Get local ID
	const ushort localId = get_local_id(0);
	
	// Get local size
	const ushort localSize = get_local_size(0);
	
	// Get group ID
	const uint groupId = get_group_id(0);
	
	// Get work group's nodes bitmap's index
	__global uint *nodesBitmapIndex = &nodesBitmap[groupId * (short)(NUMBER_OF_BITMAP_BYTES / sizeof(uint))];
	
	// Go through all groups of bytes in the bitmap as a work group
	for(short i = localId; i < (short)(NUMBER_OF_BITMAP_BYTES / sizeof(uint)); i += localSize) {
	
		// Set group of bytes in the bitmap to the nodes bitmap's
		bitmap[i] = nodesBitmapIndex[i];
	}
	
	// Get number of edges in this work group's bucket
	const uint numberOfEdges = min(numberOfEdgesPerSourceBucket[groupId], (uint)INITIAL_MAX_NUMBER_OF_EDGES_PER_BUCKET);
	
	// Check if initial buckets isn't disjointed
	#if INITIAL_BUCKETS_NUMBER_OF_BUCKETS == NUMBER_OF_BUCKETS
	
		// Get work group's bucket's indices
		__global const uint *indices = &sourceBuckets[(ulong)INITIAL_MAX_NUMBER_OF_EDGES_PER_BUCKET * groupId];
	
	// Otherwise
	#else
	
		// Get work group's bucket's indices
		__global const uint *indices = (groupId < INITIAL_BUCKETS_NUMBER_OF_BUCKETS) ? &sourceBuckets[(ulong)INITIAL_MAX_NUMBER_OF_EDGES_PER_BUCKET * groupId] : &sourceBucketsSecondPart[(ulong)INITIAL_MAX_NUMBER_OF_EDGES_PER_BUCKET * (groupId - INITIAL_BUCKETS_NUMBER_OF_BUCKETS)];
	#endif
	
	// Synchronize work group
	barrier(CLK_LOCAL_MEM_FENCE);
	
	// Go through all edges in this work group's bucket as a work group
	for(uint i = localId; i < numberOfEdges; i += localSize) {
	
		// Get edge's index
		const uint edgeIndex = indices[i];
		
		// Enable edge's node in the bitmap
		setBitInBitmap(bitmap, sipHash24(sipHashKeys, (ulong)edgeIndex * 2) & BITMAP_MASK);
	}
	
	// Synchronize work group
	barrier(CLK_LOCAL_MEM_FENCE);
	
	// Go through all groups of bytes in the bitmap as a work group
	for(short i = localId; i < (short)(NUMBER_OF_BITMAP_BYTES / sizeof(uint)); i += localSize) {
	
		// Set group of bytes in nodes bitmap to the bitmap's
		nodesBitmapIndex[i] = bitmap[i];
	}
	
	// Go through all edges in this work group's bucket as a work group
	for(uint i = localId; i < numberOfEdges; i += localSize) {
	
		// Get edge's index
		const uint edgeIndex = indices[i];
		
		// Check if local buckets size isn't one
		#if LOCAL_BUCKETS_SIZE != 1
		
			// Check if edge isn't a duplicate
			if(edgeIndex != indices[min(i - 1, numberOfEdges - 1)]) {
		#endif
		
			// Check if edge's node doesn't have a pair in the bitmap
			if(!isBitSetInBitmap(bitmap, (sipHash24(sipHashKeys, (ulong)edgeIndex * 2) & BITMAP_MASK) ^ 1)) {
			
				// Get edge's index's bucket index
				const uint bucketIndex = ((edgeIndex - (NUMBER_OF_EDGES / SLEAN_TRIMMING_PARTS) * (SLEAN_TRIMMING_PARTS - 1)) * SLEAN_TRIMMING_PARTS) >> NUMBER_OF_LEAST_SIGNIFICANT_BITS_IGNORED_DURING_REMAINING_EDGES_BUCKET_SORTING;
				
				// Get bucket's next edge index
				const uint nextEdgeIndex = min(atomic_inc(&numberOfEdgesPerDestinationBucket[bucketIndex]), (uint)(AFTER_TRIMMING_ROUND_MAX_NUMBER_OF_EDGES_PER_REMAINING_EDGES_BUCKET - 1));
				
				// Get destination bucket's next indices
				__global uint *bucketNextIndices = &destinationBuckets[AFTER_TRIMMING_ROUND_MAX_NUMBER_OF_EDGES_PER_REMAINING_EDGES_BUCKET * bucketIndex + nextEdgeIndex];
				
				// Set destination bucket's next edge to the edge
				*bucketNextIndices = edgeIndex;
			}
			
		// Check if local buckets size isn't one
		#if LOCAL_BUCKETS_SIZE != 1
		
			}
		#endif
	}
}

// Trim edges step five
void trimEdgesStepFive(__global const uint *buckets, __global const uint *numberOfEdgesPerBucket, __global uint *edgesBitmap, const uchar part) {

	// Declare bitmap
	__local uint bitmap[(short)(NUMBER_OF_REMAINING_EDGES_BITMAP_BYTES / sizeof(uint))];
	
	// Get local ID
	const ushort localId = get_local_id(0);
	
	// Get local size
	const ushort localSize = get_local_size(0);
	
	// Get group ID
	const uint groupId = get_group_id(0);
	
	// Go through all groups of bytes in the bitmap as a work group
	for(short i = localId; i < (short)(NUMBER_OF_REMAINING_EDGES_BITMAP_BYTES / sizeof(uint)); i += localSize) {
	
		// Set group of bytes in the bitmap to max
		bitmap[i] = UINT_MAX;
	}
	
	// Get number of edges in this work group's bucket
	const uint numberOfEdges = min(numberOfEdgesPerBucket[groupId], (uint)AFTER_TRIMMING_ROUND_MAX_NUMBER_OF_EDGES_PER_REMAINING_EDGES_BUCKET);
	
	// Get work group's bucket's indices
	__global const uint *indices = &buckets[AFTER_TRIMMING_ROUND_MAX_NUMBER_OF_EDGES_PER_REMAINING_EDGES_BUCKET * groupId];
	
	// Synchronize work group
	barrier(CLK_LOCAL_MEM_FENCE);
	
	// Go through all edges in this work group's bucket as a work group
	for(uint i = localId; i < numberOfEdges; i += localSize) {
	
		// Disable edge's index in the bitmap
		clearBitInBitmap(bitmap, indices[i] & REMAINING_EDGES_BITMAP_MASK);
	}
	
	// Get work group's edges bitmap's index
	__global uint *edgesBitmapIndex = &edgesBitmap[groupId * (short)(NUMBER_OF_REMAINING_EDGES_BITMAP_BYTES / sizeof(uint)) + part * (int)(((NUMBER_OF_EDGES / BITS_IN_A_BYTE) / SLEAN_TRIMMING_PARTS) / sizeof(uint))];
	
	// Synchronize work group
	barrier(CLK_LOCAL_MEM_FENCE);
	
	// Go through all groups of bytes in the bitmap as a work group
	for(short i = localId; i < (short)(NUMBER_OF_REMAINING_EDGES_BITMAP_BYTES / sizeof(uint)); i += localSize) {
	
		// Set group of bytes in edges bitmap to the bitmap's
		edgesBitmapIndex[i] = bitmap[i];
	}
}

// Check if initial buckets isn't disjointed
#if INITIAL_BUCKETS_NUMBER_OF_BUCKETS == NUMBER_OF_BUCKETS

	// Trim edges step six
	void trimEdgesStepSix(__global const uint *sourceBuckets, __global const uint *numberOfEdgesPerSourceBucket, __global const uint *nodesBitmap, __global uint *destinationBuckets, __global uint *numberOfEdgesPerDestinationBucket, const uchar part, const ulong4 sipHashKeys) {

// Otherwise
#else

	// Trim edges step six
	void trimEdgesStepSix(__global const uint *sourceBuckets, __global const uint *numberOfEdgesPerSourceBucket, __global const uint *nodesBitmap, __global uint *destinationBuckets, __global uint *numberOfEdgesPerDestinationBucket, const uchar part, const ulong4 sipHashKeys, __global const uint *sourceBucketsSecondPart) {
#endif

	// Declare bitmap
	__local uint bitmap[(short)(NUMBER_OF_BITMAP_BYTES / sizeof(uint))];
	
	// Get local ID
	const ushort localId = get_local_id(0);
	
	// Get local size
	const ushort localSize = get_local_size(0);
	
	// Get group ID
	const uint groupId = get_group_id(0);
	
	// Get work group's nodes bitmap's index
	__global const uint *nodesBitmapIndex = &nodesBitmap[groupId * (short)(NUMBER_OF_BITMAP_BYTES / sizeof(uint))];
	
	// Go through all groups of bytes in the bitmap as a work group
	for(short i = localId; i < (short)(NUMBER_OF_BITMAP_BYTES / sizeof(uint)); i += localSize) {
	
		// Set group of bytes in the bitmap to the nodes bitmap's
		bitmap[i] = nodesBitmapIndex[i];
	}
	
	// Get number of edges in this work group's bucket
	const uint numberOfEdges = min(numberOfEdgesPerSourceBucket[groupId], (uint)INITIAL_MAX_NUMBER_OF_EDGES_PER_BUCKET);
	
	// Check if initial buckets isn't disjointed
	#if INITIAL_BUCKETS_NUMBER_OF_BUCKETS == NUMBER_OF_BUCKETS
	
		// Get work group's bucket's indices
		__global const uint *indices = &sourceBuckets[(ulong)INITIAL_MAX_NUMBER_OF_EDGES_PER_BUCKET * groupId];
	
	// Otherwise
	#else
	
		// Get work group's bucket's indices
		__global const uint *indices = (groupId < INITIAL_BUCKETS_NUMBER_OF_BUCKETS) ? &sourceBuckets[(ulong)INITIAL_MAX_NUMBER_OF_EDGES_PER_BUCKET * groupId] : &sourceBucketsSecondPart[(ulong)INITIAL_MAX_NUMBER_OF_EDGES_PER_BUCKET * (groupId - INITIAL_BUCKETS_NUMBER_OF_BUCKETS)];
	#endif
	
	// Synchronize work group
	barrier(CLK_LOCAL_MEM_FENCE);
	
	// Go through all edges in this work group's bucket as a work group
	for(uint i = localId; i < numberOfEdges; i += localSize) {
	
		// Get edge's index
		const uint edgeIndex = indices[i];
		
		// Check if local buckets size isn't one
		#if LOCAL_BUCKETS_SIZE != 1
		
			// Check if edge isn't a duplicate
			if(edgeIndex != indices[min(i - 1, numberOfEdges - 1)]) {
		#endif
		
			// Check if edge's node doesn't have a pair in the bitmap
			if(!isBitSetInBitmap(bitmap, (sipHash24(sipHashKeys, (ulong)edgeIndex * 2) & BITMAP_MASK) ^ 1)) {
			
				// Get edge's index's bucket index
				const uint bucketIndex = ((edgeIndex - (NUMBER_OF_EDGES / SLEAN_TRIMMING_PARTS) * part) * SLEAN_TRIMMING_PARTS) >> NUMBER_OF_LEAST_SIGNIFICANT_BITS_IGNORED_DURING_REMAINING_EDGES_BUCKET_SORTING;
				
				// Get bucket's next edge index
				const uint nextEdgeIndex = min(atomic_inc(&numberOfEdgesPerDestinationBucket[bucketIndex]), (uint)(AFTER_TRIMMING_ROUND_MAX_NUMBER_OF_EDGES_PER_REMAINING_EDGES_BUCKET - 1));
				
				// Get destination bucket's next indices
				__global uint *bucketNextIndices = &destinationBuckets[AFTER_TRIMMING_ROUND_MAX_NUMBER_OF_EDGES_PER_REMAINING_EDGES_BUCKET * bucketIndex + nextEdgeIndex];
				
				// Set destination bucket's next edge to the edge
				*bucketNextIndices = edgeIndex;
			}
			
		// Check if local buckets size isn't one
		#if LOCAL_BUCKETS_SIZE != 1
		
			}
		#endif
	}
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
void setBitInBitmap(__local uint *bitmap, const uint index) {

	// Set bit in bitmap
	atomic_or(&bitmap[index / (char)(sizeof(uint) * BITS_IN_A_BYTE)], 1 << (index % (char)(sizeof(uint) * BITS_IN_A_BYTE)));
}

// Clear bit in bitmap
void clearBitInBitmap(__local uint *bitmap, const uint index) {

	// Set bit in bitmap
	atomic_xor(&bitmap[index / (char)(sizeof(uint) * BITS_IN_A_BYTE)], 1 << (index % (char)(sizeof(uint) * BITS_IN_A_BYTE)));
}

// Is bit set in bitmap
bool isBitSetInBitmap(__local const uint *bitmap, const uint index) {

	// Return if bit is set in bitmap
	return bitmap[index / (char)(sizeof(uint) * BITS_IN_A_BYTE)] & (1 << (index % (char)(sizeof(uint) * BITS_IN_A_BYTE)));
}


)"

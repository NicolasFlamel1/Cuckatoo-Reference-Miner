R"(

/*
N trimming rounds can be performed with the following:
Trimming round 1: clear number of edges per bucket one, step one, clear number of edges per bucket two, step two
Trimming round 2: clear number of edges per bucket one, step three
Trimming round 3: clear number of edges per bucket two, step four
Trimming round 4: clear number of edges per bucket one, step five
Trimming round 5: clear number of edges per bucket two, step five
...
Trimming round n - 1: clear number of edges per bucket one, step five
Trimming round n: clear number of edges per bucket two, step five, clear first remaining edge, step six
Get result from remaining edges
*/


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
		__kernel void trimEdgesStepOne(__global uint *buckets, __global uint *numberOfEdgesPerBucket, const ulong4 sipHashKeys);
	
	// Otherwise
	#else
	
		// Trim edges step one
		__kernel void trimEdgesStepOne(__global uint *buckets, __global uint *numberOfEdgesPerBucket, const ulong4 sipHashKeys, __global uint *bucketsSecondPart);
	#endif

// Otherwise check if local buckets size is two
#elif LOCAL_BUCKETS_SIZE == 2

	// Check if initial buckets isn't disjointed
	#if INITIAL_BUCKETS_NUMBER_OF_BUCKETS == NUMBER_OF_BUCKETS
	
		// Trim edges step one
		__kernel void trimEdgesStepOne(__global uint2 *buckets, __global uint *numberOfEdgesPerBucket, const ulong4 sipHashKeys);
	
	// Otherwise
	#else
	
		// Trim edges step one
		__kernel void trimEdgesStepOne(__global uint2 *buckets, __global uint *numberOfEdgesPerBucket, const ulong4 sipHashKeys, __global uint2 *bucketsSecondPart);
	#endif

// Otherwise
#else

	// Check if initial buckets isn't disjointed
	#if INITIAL_BUCKETS_NUMBER_OF_BUCKETS == NUMBER_OF_BUCKETS
	
		// Trim edges step one
		__kernel void trimEdgesStepOne(__global uint4 *buckets, __global uint *numberOfEdgesPerBucket, const ulong4 sipHashKeys);
	
	// Otherwise
	#else
	
		// Trim edges step one
		__kernel void trimEdgesStepOne(__global uint4 *buckets, __global uint *numberOfEdgesPerBucket, const ulong4 sipHashKeys, __global uint4 *bucketsSecondPart);
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
	__kernel void trimEdgesStepTwo(__global const uint *sourceBuckets, __global const uint *numberOfEdgesPerSourceBucket, __global uint *destinationBuckets, __global uint *numberOfEdgesPerDestinationBucket, const ulong4 sipHashKeys);

// Otherwise
#else

	// Trim edges step two
	__kernel void trimEdgesStepTwo(__global const uint *sourceBuckets, __global const uint *numberOfEdgesPerSourceBucket, __global uint *destinationBuckets, __global uint *numberOfEdgesPerDestinationBucket, const ulong4 sipHashKeys, __global const uint *sourceBucketsSecondPart);
#endif

// Check if work items per work group exists
#ifdef TRIM_EDGES_STEP_THREE_WORK_ITEMS_PER_WORK_GROUP

	// Required work group size
	__attribute__((reqd_work_group_size(TRIM_EDGES_STEP_THREE_WORK_ITEMS_PER_WORK_GROUP, 1, 1)))
#endif

// Trim edges step three
__kernel void trimEdgesStepThree(__global const uint *sourceBuckets, __global const uint *numberOfEdgesPerSourceBucket, __global uint2 *destinationBuckets, __global uint *numberOfEdgesPerDestinationBucket, const ulong4 sipHashKeys);

// Check if work items per work group exists
#ifdef TRIM_EDGES_STEP_FOUR_WORK_ITEMS_PER_WORK_GROUP

	// Required work group size
	__attribute__((reqd_work_group_size(TRIM_EDGES_STEP_FOUR_WORK_ITEMS_PER_WORK_GROUP, 1, 1)))
#endif

// Trim edges step four
__kernel void trimEdgesStepFour(__global const uint2 *sourceBuckets, __global const uint *numberOfEdgesPerSourceBucket, __global uint4 *destinationBuckets, __global uint *numberOfEdgesPerDestinationBucket, const ulong4 sipHashKeys);

// Check if work items per work group exists
#ifdef TRIM_EDGES_STEP_FIVE_WORK_ITEMS_PER_WORK_GROUP

	// Required work group size
	__attribute__((reqd_work_group_size(TRIM_EDGES_STEP_FIVE_WORK_ITEMS_PER_WORK_GROUP, 1, 1)))
#endif

// Trim edges step five
__kernel void trimEdgesStepFive(__global const uint4 *sourceBuckets, __global const uint *numberOfEdgesPerSourceBucket, __global uint4 *destinationBuckets, __global uint *numberOfEdgesPerDestinationBucket);

// Check if work items per work group exists
#ifdef TRIM_EDGES_STEP_SIX_WORK_ITEMS_PER_WORK_GROUP

	// Required work group size
	__attribute__((reqd_work_group_size(TRIM_EDGES_STEP_SIX_WORK_ITEMS_PER_WORK_GROUP, 1, 1)))
#endif

// Check if trimming rounds is one
#if TRIMMING_ROUNDS == 1

	// Trim edges step six
	__kernel void trimEdgesStepSix(__global const uint *buckets, __global const uint *numberOfEdgesPerBucket, __global uint *remainingEdges, const ulong4 sipHashKeys);

// Otherwise check if trimming rounds is two
#elif TRIMMING_ROUNDS == 2

	// Trim edges step six
	__kernel void trimEdgesStepSix(__global const uint2 *buckets, __global const uint *numberOfEdgesPerBucket, __global uint *remainingEdges, const ulong4 sipHashKeys);

// Otherwise
#else

	// Trim edges step six
	__kernel void trimEdgesStepSix(__global const uint4 *buckets, __global const uint *numberOfEdgesPerBucket, __global uint *remainingEdges);
#endif

// SipHash-2-4
static inline uint sipHash24(ulong4 keys, const ulong nonce);

// SipRound
static inline void sipRound(ulong4 *keys);

// Set bit in bitmap
static inline void setBitInBitmap(__local uint *bitmap, const uint index);

// Is bit set in bitmap
static inline bool isBitSetInBitmap(__local const uint *bitmap, const uint index);


// Supporting function implementation

// Check if local buckets size is one
#if LOCAL_BUCKETS_SIZE == 1

	// Check if initial buckets isn't disjointed
	#if INITIAL_BUCKETS_NUMBER_OF_BUCKETS == NUMBER_OF_BUCKETS
	
		// Trim edges step one
		__kernel void trimEdgesStepOne(__global uint *buckets, __global uint *numberOfEdgesPerBucket, const ulong4 sipHashKeys) {
	
	// Otherwise
	#else
	
		// Trim edges step one
		__kernel void trimEdgesStepOne(__global uint *buckets, __global uint *numberOfEdgesPerBucket, const ulong4 sipHashKeys, __global uint *bucketsSecondPart) {
	#endif

// Otherwise check if local buckets size is two
#elif LOCAL_BUCKETS_SIZE == 2

	// Check if initial buckets isn't disjointed
	#if INITIAL_BUCKETS_NUMBER_OF_BUCKETS == NUMBER_OF_BUCKETS
	
		// Trim edges step one
		__kernel void trimEdgesStepOne(__global uint2 *buckets, __global uint *numberOfEdgesPerBucket, const ulong4 sipHashKeys) {
	
	// Otherwise
	#else
	
		// Trim edges step one
		__kernel void trimEdgesStepOne(__global uint2 *buckets, __global uint *numberOfEdgesPerBucket, const ulong4 sipHashKeys, __global uint2 *bucketsSecondPart) {
	#endif

// Otherwise
#else

	// Check if initial buckets isn't disjointed
	#if INITIAL_BUCKETS_NUMBER_OF_BUCKETS == NUMBER_OF_BUCKETS
	
		// Trim edges step one
		__kernel void trimEdgesStepOne(__global uint4 *buckets, __global uint *numberOfEdgesPerBucket, const ulong4 sipHashKeys) {
	
	// Otherwise
	#else
	
		// Trim edges step one
		__kernel void trimEdgesStepOne(__global uint4 *buckets, __global uint *numberOfEdgesPerBucket, const ulong4 sipHashKeys, __global uint4 *bucketsSecondPart) {
	#endif
#endif

	// Check if local buckets size is one
	#if LOCAL_BUCKETS_SIZE == 1
	
		// Get global ID
		const uint globalId = get_global_id(0);
		
		// Get work item's edge indices
		const uint indices = globalId * NUMBER_OF_EDGES_PER_STEP_ONE_WORK_ITEM;
		
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
		
		// Get work item's edge indices
		const uint indices = globalId * NUMBER_OF_EDGES_PER_STEP_ONE_WORK_ITEM;
		
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
	__kernel void trimEdgesStepTwo(__global const uint *sourceBuckets, __global const uint *numberOfEdgesPerSourceBucket, __global uint *destinationBuckets, __global uint *numberOfEdgesPerDestinationBucket, const ulong4 sipHashKeys) {

// Otherwise
#else

	// Trim edges step two
	__kernel void trimEdgesStepTwo(__global const uint *sourceBuckets, __global const uint *numberOfEdgesPerSourceBucket, __global uint *destinationBuckets, __global uint *numberOfEdgesPerDestinationBucket, const ulong4 sipHashKeys, __global const uint *sourceBucketsSecondPart) {
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
	
	// Go through all edges in this work group's bucket as a work group
	for(uint i = localId; i < numberOfEdges; i += localSize) {
	
		// Get edge's index
		const uint edgeIndex = indices[i];
		
		// Check if local buckets size isn't one
		#if LOCAL_BUCKETS_SIZE != 1
		
			// Check if edge isn't a duplicate
			if(edgeIndex != indices[min(i - 1, numberOfEdges - 1)]) {
		#endif
		
			// Get edge's node
			const uint node = sipHash24(sipHashKeys, (ulong)edgeIndex * 2);
			
			// Check if edge's node has a pair in the bitmap
			if(isBitSetInBitmap(bitmap, (node & BITMAP_MASK) ^ 1)) {
			
				// Get edge's other node
				const uint otherNode = sipHash24(sipHashKeys, ((ulong)edgeIndex * 2) | 1);
				
				// Get edge's other node's bucket index
				const uint bucketIndex = otherNode >> NUMBER_OF_LEAST_SIGNIFICANT_BITS_IGNORED_DURING_BUCKET_SORTING;
				
				// Get bucket's next edge index
				const uint nextEdgeIndex = min(atomic_inc(&numberOfEdgesPerDestinationBucket[bucketIndex]), (uint)(AFTER_TRIMMING_ROUND_MAX_NUMBER_OF_EDGES_PER_BUCKET - 1));
				
				// Get destination bucket's next indices
				__global uint *bucketNextIndices = &destinationBuckets[AFTER_TRIMMING_ROUND_MAX_NUMBER_OF_EDGES_PER_BUCKET * bucketIndex + nextEdgeIndex];
				
				// Set destination bucket's next edge to the edge
				*bucketNextIndices = edgeIndex;
			}
			
		// Check if local buckets size isn't one
		#if LOCAL_BUCKETS_SIZE != 1
		
			}
		#endif
	}
}

// Trim edges step three
__kernel void trimEdgesStepThree(__global const uint *sourceBuckets, __global const uint *numberOfEdgesPerSourceBucket, __global uint2 *destinationBuckets, __global uint *numberOfEdgesPerDestinationBucket, const ulong4 sipHashKeys) {

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
	const uint numberOfEdges = min(numberOfEdgesPerSourceBucket[groupId], (uint)AFTER_TRIMMING_ROUND_MAX_NUMBER_OF_EDGES_PER_BUCKET);
	
	// Get work group's bucket's indices
	__global const uint *indices = &sourceBuckets[AFTER_TRIMMING_ROUND_MAX_NUMBER_OF_EDGES_PER_BUCKET * groupId];
	
	// Synchronize work group
	barrier(CLK_LOCAL_MEM_FENCE);
	
	// Go through all edges in this work group's bucket as a work group
	for(uint i = localId; i < numberOfEdges; i += localSize) {
	
		// Get edge's index
		const uint edgeIndex = indices[i];
		
		// Enable edge's node in the bitmap
		setBitInBitmap(bitmap, sipHash24(sipHashKeys, ((ulong)edgeIndex * 2) | 1) & BITMAP_MASK);
	}
	
	// Synchronize work group
	barrier(CLK_LOCAL_MEM_FENCE);
	
	// Go through all edges in this work group's bucket as a work group
	for(uint i = localId; i < numberOfEdges; i += localSize) {
	
		// Get edge's index
		const uint edgeIndex = indices[i];
		
		// Get edge's node
		const uint node = sipHash24(sipHashKeys, ((ulong)edgeIndex * 2) | 1);
		
		// Check if edge's node has a pair in the bitmap
		if(isBitSetInBitmap(bitmap, (node & BITMAP_MASK) ^ 1)) {
		
			// Get edge's other node
			const uint otherNode = sipHash24(sipHashKeys, (ulong)edgeIndex * 2);
			
			// Get edge's other node's bucket index
			const uint bucketIndex = otherNode >> NUMBER_OF_LEAST_SIGNIFICANT_BITS_IGNORED_DURING_BUCKET_SORTING;
			
			// Get bucket's next edge index
			const uint nextEdgeIndex = min(atomic_inc(&numberOfEdgesPerDestinationBucket[bucketIndex]), (uint)(AFTER_TRIMMING_ROUND_MAX_NUMBER_OF_EDGES_PER_BUCKET / 2 - 1));
			
			// Get destination bucket's next indices
			__global uint2 *bucketNextIndices = &destinationBuckets[AFTER_TRIMMING_ROUND_MAX_NUMBER_OF_EDGES_PER_BUCKET / 2 * bucketIndex + nextEdgeIndex];
			
			// Set destination bucket's next edge to the edge and its other node
			*bucketNextIndices = (uint2)(edgeIndex, otherNode);
		}
	}
}

// Trim edges step four
__kernel void trimEdgesStepFour(__global const uint2 *sourceBuckets, __global const uint *numberOfEdgesPerSourceBucket, __global uint4 *destinationBuckets, __global uint *numberOfEdgesPerDestinationBucket, const ulong4 sipHashKeys) {

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
	const uint numberOfEdges = min(numberOfEdgesPerSourceBucket[groupId], (uint)(AFTER_TRIMMING_ROUND_MAX_NUMBER_OF_EDGES_PER_BUCKET / 2));
	
	// Get work group's bucket's indices
	__global const uint2 *indices = &sourceBuckets[AFTER_TRIMMING_ROUND_MAX_NUMBER_OF_EDGES_PER_BUCKET / 2 * groupId];
	
	// Synchronize work group
	barrier(CLK_LOCAL_MEM_FENCE);
	
	// Go through all edges in this work group's bucket as a work group
	for(uint i = localId; i < numberOfEdges; i += localSize) {
	
		// Enable edge's node in the bitmap
		setBitInBitmap(bitmap, indices[i].y & BITMAP_MASK);
	}
	
	// Synchronize work group
	barrier(CLK_LOCAL_MEM_FENCE);
	
	// Go through all edges in this work group's bucket as a work group
	for(uint i = localId; i < numberOfEdges; i += localSize) {
	
		// Get edge's index and node
		const uint2 edgeIndexAndNode = indices[i];
		
		// Check if edge's node has a pair in the bitmap
		if(isBitSetInBitmap(bitmap, (edgeIndexAndNode.y & BITMAP_MASK) ^ 1)) {
		
			// Get edge's other node
			const uint otherNode = sipHash24(sipHashKeys, ((ulong)edgeIndexAndNode.x * 2) | 1);
			
			// Get edge's other node's bucket index
			const uint bucketIndex = otherNode >> NUMBER_OF_LEAST_SIGNIFICANT_BITS_IGNORED_DURING_BUCKET_SORTING;
			
			// Get bucket's next edge index
			const uint nextEdgeIndex = min(atomic_inc(&numberOfEdgesPerDestinationBucket[bucketIndex]), (uint)(AFTER_TRIMMING_ROUND_MAX_NUMBER_OF_EDGES_PER_BUCKET / 4 - 1));
			
			// Get destination bucket's next indices
			__global uint4 *bucketNextIndices = &destinationBuckets[AFTER_TRIMMING_ROUND_MAX_NUMBER_OF_EDGES_PER_BUCKET / 4 * bucketIndex + nextEdgeIndex];
			
			// Set destination bucket's next edge to the edge and its nodes
			*bucketNextIndices = (uint4)(edgeIndexAndNode.x, otherNode, edgeIndexAndNode.y, otherNode & BITMAP_MASK);
		}
	}
}

// Trim edges step five
__kernel void trimEdgesStepFive(__global const uint4 *sourceBuckets, __global const uint *numberOfEdgesPerSourceBucket, __global uint4 *destinationBuckets, __global uint *numberOfEdgesPerDestinationBucket) {

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
	const uint numberOfEdges = min(numberOfEdgesPerSourceBucket[groupId], (uint)(AFTER_TRIMMING_ROUND_MAX_NUMBER_OF_EDGES_PER_BUCKET / 4));
	
	// Get work group's bucket's indices
	__global const uint4 *indices = &sourceBuckets[AFTER_TRIMMING_ROUND_MAX_NUMBER_OF_EDGES_PER_BUCKET / 4 * groupId];
	
	// Synchronize work group
	barrier(CLK_LOCAL_MEM_FENCE);
	
	// Go through all edges in this work group's bucket as a work group
	for(uint i = localId; i < numberOfEdges; i += localSize) {
	
		// Enable edge's node in the bitmap
		setBitInBitmap(bitmap, indices[i].w);
	}
	
	// Synchronize work group
	barrier(CLK_LOCAL_MEM_FENCE);
	
	// Go through all edges in this work group's bucket as a work group
	for(uint i = localId; i < numberOfEdges; i += localSize) {
	
		// Get edge's index and nodes
		const uint4 edgeIndexAndNodes = indices[i];
		
		// Check if edge's node has a pair in the bitmap
		if(isBitSetInBitmap(bitmap, edgeIndexAndNodes.w ^ 1)) {
		
			// Get edge's other node's bucket index
			const uint bucketIndex = edgeIndexAndNodes.z >> NUMBER_OF_LEAST_SIGNIFICANT_BITS_IGNORED_DURING_BUCKET_SORTING;
			
			// Get bucket's next edge index
			const uint nextEdgeIndex = min(atomic_inc(&numberOfEdgesPerDestinationBucket[bucketIndex]), (uint)(AFTER_TRIMMING_ROUND_MAX_NUMBER_OF_EDGES_PER_BUCKET / 4 - 1));
			
			// Get destination bucket's next indices
			__global uint4 *bucketNextIndices = &destinationBuckets[AFTER_TRIMMING_ROUND_MAX_NUMBER_OF_EDGES_PER_BUCKET / 4 * bucketIndex + nextEdgeIndex];
			
			// Set destination bucket's next edge to the edge and its nodes
			*bucketNextIndices = (uint4)(edgeIndexAndNodes.xzy, edgeIndexAndNodes.z & BITMAP_MASK);
		}
	}
}

// Check if trimming rounds is one
#if TRIMMING_ROUNDS == 1

	// Trim edges step six
	__kernel void trimEdgesStepSix(__global const uint *buckets, __global const uint *numberOfEdgesPerBucket, __global uint *remainingEdges, const ulong4 sipHashKeys) {

// Otherwise check if trimming rounds is two
#elif TRIMMING_ROUNDS == 2

	// Trim edges step six
	__kernel void trimEdgesStepSix(__global const uint2 *buckets, __global const uint *numberOfEdgesPerBucket, __global uint *remainingEdges, const ulong4 sipHashKeys) {

// Otherwise
#else

	// Trim edges step six
	__kernel void trimEdgesStepSix(__global const uint4 *buckets, __global const uint *numberOfEdgesPerBucket, __global uint *remainingEdges) {
#endif

	// Declare index
	__local uint index;
	
	// Get local ID
	const ushort localId = get_local_id(0);
	
	// Get local size
	const ushort localSize = get_local_size(0);
	
	// Get group ID
	const uint groupId = get_group_id(0);
	
	// Check if trimming rounds is one
	#if TRIMMING_ROUNDS == 1
	
		// Get number of edges in this work group's bucket
		const uint numberOfEdges = min(numberOfEdgesPerBucket[groupId], (uint)AFTER_TRIMMING_ROUND_MAX_NUMBER_OF_EDGES_PER_BUCKET);
		
		// Get work group's bucket's indices
		__global const uint *indices = &buckets[AFTER_TRIMMING_ROUND_MAX_NUMBER_OF_EDGES_PER_BUCKET * groupId];
		
	// Otherwise check if trimming rounds is two
	#elif TRIMMING_ROUNDS == 2
	
		// Get number of edges in this work group's bucket
		const uint numberOfEdges = min(numberOfEdgesPerBucket[groupId], (uint)(AFTER_TRIMMING_ROUND_MAX_NUMBER_OF_EDGES_PER_BUCKET / 2));
		
		// Get work group's bucket's indices
		__global const uint2 *indices = &buckets[AFTER_TRIMMING_ROUND_MAX_NUMBER_OF_EDGES_PER_BUCKET / 2 * groupId];
		
	// Otherwise
	#else
	
		// Get number of edges in this work group's bucket
		const uint numberOfEdges = min(numberOfEdgesPerBucket[groupId], (uint)(AFTER_TRIMMING_ROUND_MAX_NUMBER_OF_EDGES_PER_BUCKET / 4));
		
		// Get work group's bucket's indices
		__global const uint4 *indices = &buckets[AFTER_TRIMMING_ROUND_MAX_NUMBER_OF_EDGES_PER_BUCKET / 4 * groupId];
	#endif
	
	// Check if this work item is the first in the work group
	if(localId == 0) {
	
		// Add number of edges to the number of remaining edges
		index = atomic_add(remainingEdges, numberOfEdges);
	}
	
	// Synchronize work group
	barrier(CLK_LOCAL_MEM_FENCE);
	
	// Go through all edges in this work group's bucket as a work group
	for(uint i = localId; i < numberOfEdges; i += localSize) {
	
		// Get next remaining edge
		__global uint *nextRemainingEdge = &remainingEdges[min(index + i, (uint)(MAX_NUMBER_OF_EDGES_AFTER_TRIMMING - 1)) * EDGE_NUMBER_OF_COMPONENTS + 1];
		
		// Check if trimming rounds is one
		#if TRIMMING_ROUNDS == 1
		
			// Get edge's index
			const uint edgeIndex = indices[i];
			
			// Get edge's node
			const uint node = sipHash24(sipHashKeys, (ulong)edgeIndex * 2);
			
			// Get edge's other node
			const uint otherNode = sipHash24(sipHashKeys, ((ulong)edgeIndex * 2) | 1);
			
			// Set next remaining edge to the edge and its nodes
			nextRemainingEdge[0] = edgeIndex;
			nextRemainingEdge[1] = node;
			nextRemainingEdge[2] = otherNode;
			
		// Otherwise check if trimming rounds is two
		#elif TRIMMING_ROUNDS == 2
		
			// Get edge's other node
			const uint otherNode = sipHash24(sipHashKeys, ((ulong)indices[i].x * 2) | 1);
			
			// Set next remaining edge to the edge and its nodes
			nextRemainingEdge[0] = indices[i].x;
			nextRemainingEdge[1] = indices[i].y;
			nextRemainingEdge[2] = otherNode;
			
		// Otherwise
		#else
		
			// Check if trimming rounds is even
			#if TRIMMING_ROUNDS % 2 == 0
			
				// Set next remaining edge to the edge and its nodes
				nextRemainingEdge[0] = indices[i].x;
				nextRemainingEdge[1] = indices[i].y;
				nextRemainingEdge[2] = indices[i].z;
			
			// Otherwise
			#else
			
				// Set next remaining edge to the edge and its nodes
				nextRemainingEdge[0] = indices[i].x;
				nextRemainingEdge[1] = indices[i].z;
				nextRemainingEdge[2] = indices[i].y;
			#endif
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

// Is bit set in bitmap
bool isBitSetInBitmap(__local const uint *bitmap, const uint index) {

	// Return if bit is set in bitmap
	return bitmap[index / (char)(sizeof(uint) * BITS_IN_A_BYTE)] & (1 << (index % (char)(sizeof(uint) * BITS_IN_A_BYTE)));
}


)"

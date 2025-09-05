MTLSTR(R"(

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

This trimming algorithm can be made faster by storing the edge's index and both its nodes in step one instead of just storing the edge's index. This would require 3x more memory though, but then steps two, three, and four could be skipped.
*/


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

// Bitmap mask
#define BITMAP_MASK (NUMBER_OF_BITMAP_BYTES * BITS_IN_A_BYTE - 1)


// Function prototypes

// Check if local buckets size is one
#if LOCAL_BUCKETS_SIZE == 1

	// Check if initial buckets isn't disjointed
	#if INITIAL_BUCKETS_NUMBER_OF_BUCKETS == NUMBER_OF_BUCKETS
	
		// Trim edges step one
		[[kernel]] void trimEdgesStepOne(device uint *__restrict buckets, device atomic_uint *__restrict numberOfEdgesPerBucket, constant const ulong4 &__restrict sipHashKeys, const uint globalId);
	
	// Otherwise
	#else
	
		// Trim edges step one
		[[kernel]] void trimEdgesStepOne(device uint *__restrict buckets, device atomic_uint *__restrict numberOfEdgesPerBucket, constant const ulong4 &__restrict sipHashKeys, device uint *__restrict bucketsSecondPart, const uint globalId);
	#endif

// Otherwise check if local buckets size is two
#elif LOCAL_BUCKETS_SIZE == 2

	// Check if initial buckets isn't disjointed
	#if INITIAL_BUCKETS_NUMBER_OF_BUCKETS == NUMBER_OF_BUCKETS
	
		// Trim edges step one
		[[kernel]] void trimEdgesStepOne(device uint2 *__restrict buckets, device atomic_uint *__restrict numberOfEdgesPerBucket, constant const ulong4 &__restrict sipHashKeys, const uint globalId, const ushort localId, const ushort localSize);
	
	// Otherwise
	#else
	
		// Trim edges step one
		[[kernel]] void trimEdgesStepOne(device uint2 *__restrict buckets, device atomic_uint *__restrict numberOfEdgesPerBucket, constant const ulong4 &__restrict sipHashKeys, device uint2 *__restrict bucketsSecondPart, const uint globalId, const ushort localId, const ushort localSize);
	#endif

// Otherwise
#else

	// Check if initial buckets isn't disjointed
	#if INITIAL_BUCKETS_NUMBER_OF_BUCKETS == NUMBER_OF_BUCKETS
	
		// Trim edges step one
		[[kernel]] void trimEdgesStepOne(device uint4 *__restrict buckets, device atomic_uint *__restrict numberOfEdgesPerBucket, constant const ulong4 &__restrict sipHashKeys, const uint globalId, const ushort localId, const ushort localSize);
	
	// Otherwise
	#else
	
		// Trim edges step one
		[[kernel]] void trimEdgesStepOne(device uint4 *__restrict buckets, device atomic_uint *__restrict numberOfEdgesPerBucket, constant const ulong4 &__restrict sipHashKeys, device uint4 *__restrict bucketsSecondPart, const uint globalId, const ushort localId, const ushort localSize);
	#endif
#endif

// Check if initial buckets isn't disjointed
#if INITIAL_BUCKETS_NUMBER_OF_BUCKETS == NUMBER_OF_BUCKETS

	// Trim edges step two
	[[kernel]] void trimEdgesStepTwo(device const uint *__restrict sourceBuckets, device const uint *__restrict numberOfEdgesPerSourceBucket, device uint *__restrict destinationBuckets, device atomic_uint *__restrict numberOfEdgesPerDestinationBucket, constant const ulong4 &__restrict sipHashKeys, const ushort localId, const ushort localSize, const uint groupId);

// Otherwise
#else

	// Trim edges step two
	[[kernel]] void trimEdgesStepTwo(device const uint *__restrict sourceBuckets, device const uint *__restrict numberOfEdgesPerSourceBucket, device uint *__restrict destinationBuckets, device atomic_uint *__restrict numberOfEdgesPerDestinationBucket, constant const ulong4 &__restrict sipHashKeys, device const uint *__restrict sourceBucketsSecondPart, const ushort localId, const ushort localSize, const uint groupId);
#endif

// Trim edges step three
[[kernel]] void trimEdgesStepThree(device const uint *__restrict sourceBuckets, device const uint *__restrict numberOfEdgesPerSourceBucket, device uint2 *__restrict destinationBuckets, device atomic_uint *__restrict numberOfEdgesPerDestinationBucket, constant const ulong4 &__restrict sipHashKeys, const ushort localId, const ushort localSize, const uint groupId);

// Trim edges step four
[[kernel]] void trimEdgesStepFour(device const uint2 *__restrict sourceBuckets, device const uint *__restrict numberOfEdgesPerSourceBucket, device uint4 *__restrict destinationBuckets, device atomic_uint *__restrict numberOfEdgesPerDestinationBucket, constant const ulong4 &__restrict sipHashKeys, const ushort localId, const ushort localSize, const uint groupId);

// Trim edges step five
[[kernel]] void trimEdgesStepFive(device const uint4 *__restrict sourceBuckets, device const uint *__restrict numberOfEdgesPerSourceBucket, device uint4 *__restrict destinationBuckets, device atomic_uint *__restrict numberOfEdgesPerDestinationBucket, const ushort localId, const ushort localSize, const uint groupId);

// Check if trimming rounds is one
#if TRIMMING_ROUNDS == 1

	// Trim edges step six
	[[kernel]] void trimEdgesStepSix(device const uint *__restrict buckets, device const uint *__restrict numberOfEdgesPerBucket, device atomic_uint *__restrict remainingEdges, constant const ulong4 &__restrict sipHashKeys, const ushort localId, const ushort localSize, const uint groupId);

// Otherwise check if trimming rounds is two
#elif TRIMMING_ROUNDS == 2

	// Trim edges step six
	[[kernel]] void trimEdgesStepSix(device const uint2 *__restrict buckets, device const uint *__restrict numberOfEdgesPerBucket, device atomic_uint *__restrict remainingEdges, constant const ulong4 &__restrict sipHashKeys, const ushort localId, const ushort localSize, const uint groupId);

// Otherwise
#else

	// Trim edges step six
	[[kernel]] void trimEdgesStepSix(device const uint4 *__restrict buckets, device const uint *__restrict numberOfEdgesPerBucket, device atomic_uint *__restrict remainingEdges, const ushort localId, const ushort localSize, const uint groupId);
#endif

// Check if number of buckets is one
#if NUMBER_OF_BUCKETS == 1

	// Clear number of edges per bucket
	[[kernel]] void clearNumberOfEdgesPerBucket(device uint *numberOfEdgesPerBucket, const uint globalId);
	
// Otherwise
#else

	// Clear number of edges per bucket
	[[kernel]] void clearNumberOfEdgesPerBucket(device ulong *numberOfEdgesPerBucket, const uint globalId);
#endif

// SipHash-2-4
static inline uint sipHash24(ulong4 keys, const ulong nonce);

// SipRound
static inline void sipRound(thread ulong4 &keys);

// Set bit in bitmap
static inline void setBitInBitmap(threadgroup atomic_uint *bitmap, const uint index);

// Is bit set in bitmap
static inline bool isBitSetInBitmap(threadgroup const atomic_uint *bitmap, const uint index);


// Supporting function implementation

// Check if local buckets size is one
#if LOCAL_BUCKETS_SIZE == 1

	// Check if initial buckets isn't disjointed
	#if INITIAL_BUCKETS_NUMBER_OF_BUCKETS == NUMBER_OF_BUCKETS
	
		// Trim edges step one
		[[kernel]] void trimEdgesStepOne(device uint *__restrict buckets [[buffer(0)]], device atomic_uint *__restrict numberOfEdgesPerBucket [[buffer(1)]], constant const ulong4 &__restrict sipHashKeys [[buffer(2)]], const uint globalId [[thread_position_in_grid]]) {
		
	// Otherwise
	#else
	
		// Trim edges step one
		[[kernel]] void trimEdgesStepOne(device uint *__restrict buckets [[buffer(0)]], device atomic_uint *__restrict numberOfEdgesPerBucket [[buffer(1)]], constant const ulong4 &__restrict sipHashKeys [[buffer(2)]], device uint *__restrict bucketsSecondPart [[buffer(3)]], const uint globalId [[thread_position_in_grid]]) {
	#endif

// Otherwise check if local buckets size is two
#elif LOCAL_BUCKETS_SIZE == 2

	// Check if initial buckets isn't disjointed
	#if INITIAL_BUCKETS_NUMBER_OF_BUCKETS == NUMBER_OF_BUCKETS
	
		// Trim edges step one
		[[kernel]] void trimEdgesStepOne(device uint2 *__restrict buckets [[buffer(0)]], device atomic_uint *__restrict numberOfEdgesPerBucket [[buffer(1)]], constant const ulong4 &__restrict sipHashKeys [[buffer(2)]], const uint globalId [[thread_position_in_grid]], const ushort localId [[thread_position_in_threadgroup]], const ushort localSize [[threads_per_threadgroup]]) {
		
	// Otherwise
	#else
	
		// Trim edges step one
		[[kernel]] void trimEdgesStepOne(device uint2 *__restrict buckets [[buffer(0)]], device atomic_uint *__restrict numberOfEdgesPerBucket [[buffer(1)]], constant const ulong4 &__restrict sipHashKeys [[buffer(2)]], device uint2 *__restrict bucketsSecondPart [[buffer(3)]], const uint globalId [[thread_position_in_grid]], const ushort localId [[thread_position_in_threadgroup]], const ushort localSize [[threads_per_threadgroup]]) {
	#endif

// Otherwise
#else

	// Check if initial buckets isn't disjointed
	#if INITIAL_BUCKETS_NUMBER_OF_BUCKETS == NUMBER_OF_BUCKETS
	
		// Trim edges step one
		[[kernel]] void trimEdgesStepOne(device uint4 *__restrict buckets [[buffer(0)]], device atomic_uint *__restrict numberOfEdgesPerBucket [[buffer(1)]], constant const ulong4 &__restrict sipHashKeys [[buffer(2)]], const uint globalId [[thread_position_in_grid]], const ushort localId [[thread_position_in_threadgroup]], const ushort localSize [[threads_per_threadgroup]]) {
		
	// Otherwise
	#else
	
		// Trim edges step one
		[[kernel]] void trimEdgesStepOne(device uint4 *__restrict buckets [[buffer(0)]], device atomic_uint *__restrict numberOfEdgesPerBucket [[buffer(1)]], constant const ulong4 &__restrict sipHashKeys [[buffer(2)]], device uint4 *__restrict bucketsSecondPart [[buffer(3)]], const uint globalId [[thread_position_in_grid]], const ushort localId [[thread_position_in_threadgroup]], const ushort localSize [[threads_per_threadgroup]]) {
	#endif
#endif

	// Check if local buckets size is one
	#if LOCAL_BUCKETS_SIZE == 1
	
		// Get work item's edge indices
		const uint indices = globalId * NUMBER_OF_EDGES_PER_STEP_ONE_WORK_ITEM;
		
		// Go through all of this work item's edges
		for(short i = 0; i < NUMBER_OF_EDGES_PER_STEP_ONE_WORK_ITEM; ++i) {
		
			// Get edge's index
			const uint edgeIndex = indices + i;
			
			// Get edge's node's bucket index
			const uint bucketIndex = sipHash24(sipHashKeys, static_cast<ulong>(edgeIndex) * 2) >> NUMBER_OF_LEAST_SIGNIFICANT_BITS_IGNORED_DURING_BUCKET_SORTING;
			
			// Get bucket's next edge index
			const uint nextEdgeIndex = min(atomic_fetch_add_explicit(&numberOfEdgesPerBucket[bucketIndex], 1, memory_order_relaxed), static_cast<uint>(INITIAL_MAX_NUMBER_OF_EDGES_PER_BUCKET - 1));
			
			// Check if initial buckets isn't disjointed
			#if INITIAL_BUCKETS_NUMBER_OF_BUCKETS == NUMBER_OF_BUCKETS
			
				// Get bucket's next indices
				device uint *bucketNextIndices = &buckets[static_cast<ulong>(INITIAL_MAX_NUMBER_OF_EDGES_PER_BUCKET) * bucketIndex + nextEdgeIndex];
			
			// Otherwise
			#else
			
				// Get bucket's next indices
				device uint *bucketNextIndices = (bucketIndex < INITIAL_BUCKETS_NUMBER_OF_BUCKETS) ? &buckets[static_cast<ulong>(INITIAL_MAX_NUMBER_OF_EDGES_PER_BUCKET) * bucketIndex + nextEdgeIndex] : &bucketsSecondPart[static_cast<ulong>(INITIAL_MAX_NUMBER_OF_EDGES_PER_BUCKET) * (bucketIndex - INITIAL_BUCKETS_NUMBER_OF_BUCKETS) + nextEdgeIndex];
			#endif
			
			// Set bucket's next edge to the edge
			*bucketNextIndices = edgeIndex;
		}
		
	// Otherwise
	#else
	
		// Declare local buckets
		threadgroup uint localBuckets[NUMBER_OF_BUCKETS][LOCAL_BUCKETS_SIZE - 1];
		
		// Declare number of edges per local bucket
		threadgroup atomic_uint numberOfEdgesPerLocalBucket[static_cast<short>((NUMBER_OF_BUCKETS + sizeof(uint) - 1) / sizeof(uint))];
		
		// Go through all groups of local buckets as a work group
		for(short i = localId; i < static_cast<short>((NUMBER_OF_BUCKETS + sizeof(uint) - 1) / sizeof(uint)); i += localSize) {
		
			// Set group of local bucket's number of edges to zero
			atomic_store_explicit(&numberOfEdgesPerLocalBucket[i], 0, memory_order_relaxed);
		}
		
		// Get work item's edge indices
		const uint indices = globalId * NUMBER_OF_EDGES_PER_STEP_ONE_WORK_ITEM;
		
		// Go through all of this work item's edges
		for(short i = 0; i < NUMBER_OF_EDGES_PER_STEP_ONE_WORK_ITEM; ++i) {
		
			// Get edge's index
			const uint edgeIndex = indices + i;
			
			// Get edge's node's bucket index
			const uint bucketIndex = sipHash24(sipHashKeys, static_cast<ulong>(edgeIndex) * 2) >> NUMBER_OF_LEAST_SIGNIFICANT_BITS_IGNORED_DURING_BUCKET_SORTING;
			
			// Synchronize work group
			threadgroup_barrier(mem_flags::mem_threadgroup);
			
			// Increment local bucket's number of edges
			uchar numberOfEdges = atomic_fetch_add_explicit(&numberOfEdgesPerLocalBucket[bucketIndex / static_cast<char>(sizeof(uint))], 1 << ((bucketIndex % static_cast<char>(sizeof(uint))) * BITS_IN_A_BYTE), memory_order_relaxed) >> ((bucketIndex % static_cast<char>(sizeof(uint))) * BITS_IN_A_BYTE);
			
			// Check if local bucket isn't full with this edge
			if(numberOfEdges < LOCAL_BUCKETS_SIZE - 1) {
			
				// Append edge index to local bucket
				localBuckets[bucketIndex][numberOfEdges] = edgeIndex;
			}
			
			// Loop until edges have been added to a local bucket
			for(char j = 0; j < 8 / LOCAL_BUCKETS_SIZE; ++j) {
			
				// Synchronize work group
				threadgroup_barrier(mem_flags::mem_threadgroup);
				
				// Check if local bucket is full with this edge
				if(numberOfEdges == LOCAL_BUCKETS_SIZE - 1) {
				
					// Get bucket's next edge index
					const uint nextEdgeIndex = min(atomic_fetch_add_explicit(&numberOfEdgesPerBucket[bucketIndex], LOCAL_BUCKETS_SIZE, memory_order_relaxed), static_cast<uint>(INITIAL_MAX_NUMBER_OF_EDGES_PER_BUCKET - LOCAL_BUCKETS_SIZE));
					
					// Check if local buckets size is two
					#if LOCAL_BUCKETS_SIZE == 2
					
						// Check if initial buckets isn't disjointed
						#if INITIAL_BUCKETS_NUMBER_OF_BUCKETS == NUMBER_OF_BUCKETS
						
							// Get bucket's next indices
							device uint2 *bucketNextIndices = &buckets[(static_cast<ulong>(INITIAL_MAX_NUMBER_OF_EDGES_PER_BUCKET) * bucketIndex + nextEdgeIndex) / 2];
						
						// Otherwise
						#else
						
							// Get bucket's next indices
							device uint2 *bucketNextIndices = (bucketIndex < INITIAL_BUCKETS_NUMBER_OF_BUCKETS) ? &buckets[(static_cast<ulong>(INITIAL_MAX_NUMBER_OF_EDGES_PER_BUCKET) * bucketIndex + nextEdgeIndex) / 2] : &bucketsSecondPart[(static_cast<ulong>(INITIAL_MAX_NUMBER_OF_EDGES_PER_BUCKET) * (bucketIndex - INITIAL_BUCKETS_NUMBER_OF_BUCKETS) + nextEdgeIndex) / 2];
						#endif
						
						// Set bucket's next edges to the local bucket's edges
						*bucketNextIndices = uint2(localBuckets[bucketIndex][0], edgeIndex);
						
					// Otherwise
					#else
					
						// Check if initial buckets isn't disjointed
						#if INITIAL_BUCKETS_NUMBER_OF_BUCKETS == NUMBER_OF_BUCKETS
						
							// Get bucket's next indices
							device uint4 *bucketNextIndices = &buckets[(static_cast<ulong>(INITIAL_MAX_NUMBER_OF_EDGES_PER_BUCKET) * bucketIndex + nextEdgeIndex) / 4];
						
						// Otherwise
						#else
						
							// Get bucket's next indices
							device uint4 *bucketNextIndices = (bucketIndex < INITIAL_BUCKETS_NUMBER_OF_BUCKETS) ? &buckets[(static_cast<ulong>(INITIAL_MAX_NUMBER_OF_EDGES_PER_BUCKET) * bucketIndex + nextEdgeIndex) / 4] : &bucketsSecondPart[(static_cast<ulong>(INITIAL_MAX_NUMBER_OF_EDGES_PER_BUCKET) * (bucketIndex - INITIAL_BUCKETS_NUMBER_OF_BUCKETS) + nextEdgeIndex) / 4];
						#endif
						
						// Set bucket's next edges to the local bucket's edges
						*bucketNextIndices = uint4(localBuckets[bucketIndex][0], localBuckets[bucketIndex][1], localBuckets[bucketIndex][2], edgeIndex);
					#endif
					
					// Update local bucket's number of edges
					atomic_fetch_sub_explicit(&numberOfEdgesPerLocalBucket[bucketIndex / static_cast<char>(sizeof(uint))], LOCAL_BUCKETS_SIZE << ((bucketIndex % static_cast<char>(sizeof(uint))) * BITS_IN_A_BYTE), memory_order_relaxed);
				}
				
				// Update number of edges
				numberOfEdges -= LOCAL_BUCKETS_SIZE;
				
				// Synchronize work group
				threadgroup_barrier(mem_flags::mem_threadgroup);
				
				// Check if local bucket isn't full with this edge
				if(numberOfEdges < LOCAL_BUCKETS_SIZE - 1) {
				
					// Append edge index to local bucket
					localBuckets[bucketIndex][numberOfEdges] = edgeIndex;
				}
			}
		}
		
		// Synchronize work group
		threadgroup_barrier(mem_flags::mem_threadgroup);
		
		// Go through all local buckets as a work group
		for(int i = localId; i < NUMBER_OF_BUCKETS; i += localSize) {
		
			// Get local bucket's number of edges
			const uchar numberOfEdges = atomic_load_explicit(&numberOfEdgesPerLocalBucket[i / static_cast<char>(sizeof(uint))], memory_order_relaxed) >> ((i % static_cast<char>(sizeof(uint))) * BITS_IN_A_BYTE);
			
			// Check if local bucket isn't empty
			if(numberOfEdges) {
			
				// Get bucket's next edge index
				const uint nextEdgeIndex = min(atomic_fetch_add_explicit(&numberOfEdgesPerBucket[i], LOCAL_BUCKETS_SIZE, memory_order_relaxed), static_cast<uint>(INITIAL_MAX_NUMBER_OF_EDGES_PER_BUCKET - LOCAL_BUCKETS_SIZE));
				
				// Check if local buckets size is two
				#if LOCAL_BUCKETS_SIZE == 2
				
					// Check if initial buckets isn't disjointed
					#if INITIAL_BUCKETS_NUMBER_OF_BUCKETS == NUMBER_OF_BUCKETS
					
						// Get bucket's next indices
						device uint2 *bucketNextIndices = &buckets[(static_cast<ulong>(INITIAL_MAX_NUMBER_OF_EDGES_PER_BUCKET) * i + nextEdgeIndex) / 2];
					
					// Otherwise
					#else
					
						// Get bucket's next indices
						device uint2 *bucketNextIndices = (i < INITIAL_BUCKETS_NUMBER_OF_BUCKETS) ? &buckets[(static_cast<ulong>(INITIAL_MAX_NUMBER_OF_EDGES_PER_BUCKET) * i + nextEdgeIndex) / 2] : &bucketsSecondPart[(static_cast<ulong>(INITIAL_MAX_NUMBER_OF_EDGES_PER_BUCKET) * (i - INITIAL_BUCKETS_NUMBER_OF_BUCKETS) + nextEdgeIndex) / 2];
					#endif
					
					// Set bucket's next edges to the local bucket's edges
					*bucketNextIndices = uint2(localBuckets[i][0]);
					
				// Otherwise
				#else
				
					// Check if initial buckets isn't disjointed
					#if INITIAL_BUCKETS_NUMBER_OF_BUCKETS == NUMBER_OF_BUCKETS
					
						// Get bucket's next indices
						device uint4 *bucketNextIndices = &buckets[(static_cast<ulong>(INITIAL_MAX_NUMBER_OF_EDGES_PER_BUCKET) * i + nextEdgeIndex) / 4];
					
					// Otherwise
					#else
					
						// Get bucket's next indices
						device uint4 *bucketNextIndices = (i < INITIAL_BUCKETS_NUMBER_OF_BUCKETS) ? &buckets[(static_cast<ulong>(INITIAL_MAX_NUMBER_OF_EDGES_PER_BUCKET) * i + nextEdgeIndex) / 4] : &bucketsSecondPart[(static_cast<ulong>(INITIAL_MAX_NUMBER_OF_EDGES_PER_BUCKET) * (i - INITIAL_BUCKETS_NUMBER_OF_BUCKETS) + nextEdgeIndex) / 4];
					#endif
					
					// Check number of edges
					switch(numberOfEdges) {
					
						// One
						case 1:
						
							// Set bucket's next edges to the local bucket's edges
							*bucketNextIndices = uint4(localBuckets[i][0]);
							
							// Break
							break;
						
						// Two
						case 2:
						
							// Set bucket's next edges to the local bucket's edges
							*bucketNextIndices = uint4(uint3(localBuckets[i][0]), localBuckets[i][1]);
							
							// Break
							break;
						
						// Default
						default:
						
							// Set bucket's next edges to the local bucket's edges
							*bucketNextIndices = uint4(uint2(localBuckets[i][0]), localBuckets[i][1], localBuckets[i][2]);
					}
				#endif
			}
		}
	#endif
}

// Check if initial buckets isn't disjointed
#if INITIAL_BUCKETS_NUMBER_OF_BUCKETS == NUMBER_OF_BUCKETS

	// Trim edges step two
	[[kernel]] void trimEdgesStepTwo(device const uint *__restrict sourceBuckets [[buffer(0)]], device const uint *__restrict numberOfEdgesPerSourceBucket [[buffer(1)]], device uint *__restrict destinationBuckets [[buffer(4)]], device atomic_uint *__restrict numberOfEdgesPerDestinationBucket [[buffer(5)]], constant const ulong4 &__restrict sipHashKeys [[buffer(2)]], const ushort localId [[thread_position_in_threadgroup]], const ushort localSize [[threads_per_threadgroup]], const uint groupId [[threadgroup_position_in_grid]]) {

// Otherwise
#else

	// Trim edges step two
	[[kernel]] void trimEdgesStepTwo(device const uint *__restrict sourceBuckets [[buffer(0)]], device const uint *__restrict numberOfEdgesPerSourceBucket [[buffer(1)]], device uint *__restrict destinationBuckets [[buffer(4)]], device atomic_uint *__restrict numberOfEdgesPerDestinationBucket [[buffer(5)]], constant const ulong4 &__restrict sipHashKeys [[buffer(2)]], device const uint *__restrict sourceBucketsSecondPart [[buffer(3)]], const ushort localId [[thread_position_in_threadgroup]], const ushort localSize [[threads_per_threadgroup]], const uint groupId [[threadgroup_position_in_grid]]) {
#endif

	// Declare bitmap
	threadgroup atomic_uint bitmap[static_cast<short>(NUMBER_OF_BITMAP_BYTES / sizeof(uint))];
	
	// Go through all groups of bytes in the bitmap as a work group
	for(short i = localId; i < static_cast<short>(NUMBER_OF_BITMAP_BYTES / sizeof(uint)); i += localSize) {
	
		// Set group of bytes in the bitmap to zero
		atomic_store_explicit(&bitmap[i], 0, memory_order_relaxed);
	}
	
	// Get number of edges in this work group's bucket
	const uint numberOfEdges = min(numberOfEdgesPerSourceBucket[groupId], static_cast<uint>(INITIAL_MAX_NUMBER_OF_EDGES_PER_BUCKET));
	
	// Check if initial buckets isn't disjointed
	#if INITIAL_BUCKETS_NUMBER_OF_BUCKETS == NUMBER_OF_BUCKETS
	
		// Get work group's bucket's indices
		device const uint *indices = &sourceBuckets[static_cast<ulong>(INITIAL_MAX_NUMBER_OF_EDGES_PER_BUCKET) * groupId];
	
	// Otherwise
	#else
	
		// Get work group's bucket's indices
		device const uint *indices = (groupId < INITIAL_BUCKETS_NUMBER_OF_BUCKETS) ? &sourceBuckets[static_cast<ulong>(INITIAL_MAX_NUMBER_OF_EDGES_PER_BUCKET) * groupId] : &sourceBucketsSecondPart[static_cast<ulong>(INITIAL_MAX_NUMBER_OF_EDGES_PER_BUCKET) * (groupId - INITIAL_BUCKETS_NUMBER_OF_BUCKETS)];
	#endif
	
	// Synchronize work group
	threadgroup_barrier(mem_flags::mem_threadgroup);
	
	// Go through all edges in this work group's bucket as a work group
	for(uint i = localId; i < numberOfEdges; i += localSize) {
	
		// Get edge's index
		const uint edgeIndex = indices[i];
		
		// Enable edge's node in the bitmap
		setBitInBitmap(bitmap, sipHash24(sipHashKeys, static_cast<ulong>(edgeIndex) * 2) & BITMAP_MASK);
	}
	
	// Synchronize work group
	threadgroup_barrier(mem_flags::mem_threadgroup);
	
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
			const uint node = sipHash24(sipHashKeys, static_cast<ulong>(edgeIndex) * 2);
			
			// Check if edge's node has a pair in the bitmap
			if(isBitSetInBitmap(bitmap, (node & BITMAP_MASK) ^ 1)) {
			
				// Get edge's other node
				const uint otherNode = sipHash24(sipHashKeys, (static_cast<ulong>(edgeIndex) * 2) | 1);
				
				// Get edge's other node's bucket index
				const uint bucketIndex = otherNode >> NUMBER_OF_LEAST_SIGNIFICANT_BITS_IGNORED_DURING_BUCKET_SORTING;
				
				// Get bucket's next edge index
				const uint nextEdgeIndex = min(atomic_fetch_add_explicit(&numberOfEdgesPerDestinationBucket[bucketIndex], 1, memory_order_relaxed), static_cast<uint>(AFTER_TRIMMING_ROUND_MAX_NUMBER_OF_EDGES_PER_BUCKET - 1));
				
				// Get destination bucket's next indices
				device uint *bucketNextIndices = &destinationBuckets[AFTER_TRIMMING_ROUND_MAX_NUMBER_OF_EDGES_PER_BUCKET * bucketIndex + nextEdgeIndex];
				
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
[[kernel]] void trimEdgesStepThree(device const uint *__restrict sourceBuckets [[buffer(4)]], device const uint *__restrict numberOfEdgesPerSourceBucket [[buffer(5)]], device uint2 *__restrict destinationBuckets [[buffer(0)]], device atomic_uint *__restrict numberOfEdgesPerDestinationBucket [[buffer(1)]], constant const ulong4 &__restrict sipHashKeys [[buffer(2)]], const ushort localId [[thread_position_in_threadgroup]], const ushort localSize [[threads_per_threadgroup]], const uint groupId [[threadgroup_position_in_grid]]) {

	// Declare bitmap
	threadgroup atomic_uint bitmap[static_cast<short>(NUMBER_OF_BITMAP_BYTES / sizeof(uint))];
	
	// Go through all groups of bytes in the bitmap as a work group
	for(short i = localId; i < static_cast<short>(NUMBER_OF_BITMAP_BYTES / sizeof(uint)); i += localSize) {
	
		// Set group of bytes in the bitmap to zero
		atomic_store_explicit(&bitmap[i], 0, memory_order_relaxed);
	}
	
	// Get number of edges in this work group's bucket
	const uint numberOfEdges = min(numberOfEdgesPerSourceBucket[groupId], static_cast<uint>(AFTER_TRIMMING_ROUND_MAX_NUMBER_OF_EDGES_PER_BUCKET));
	
	// Get work group's bucket's indices
	device const uint *indices = &sourceBuckets[AFTER_TRIMMING_ROUND_MAX_NUMBER_OF_EDGES_PER_BUCKET * groupId];
	
	// Synchronize work group
	threadgroup_barrier(mem_flags::mem_threadgroup);
	
	// Go through all edges in this work group's bucket as a work group
	for(uint i = localId; i < numberOfEdges; i += localSize) {
	
		// Get edge's index
		const uint edgeIndex = indices[i];
		
		// Enable edge's node in the bitmap
		setBitInBitmap(bitmap, sipHash24(sipHashKeys, (static_cast<ulong>(edgeIndex) * 2) | 1) & BITMAP_MASK);
	}
	
	// Synchronize work group
	threadgroup_barrier(mem_flags::mem_threadgroup);
	
	// Go through all edges in this work group's bucket as a work group
	for(uint i = localId; i < numberOfEdges; i += localSize) {
	
		// Get edge's index
		const uint edgeIndex = indices[i];
		
		// Get edge's node
		const uint node = sipHash24(sipHashKeys, (static_cast<ulong>(edgeIndex) * 2) | 1);
		
		// Check if edge's node has a pair in the bitmap
		if(isBitSetInBitmap(bitmap, (node & BITMAP_MASK) ^ 1)) {
		
			// Get edge's other node
			const uint otherNode = sipHash24(sipHashKeys, static_cast<ulong>(edgeIndex) * 2);
			
			// Get edge's other node's bucket index
			const uint bucketIndex = otherNode >> NUMBER_OF_LEAST_SIGNIFICANT_BITS_IGNORED_DURING_BUCKET_SORTING;
			
			// Get bucket's next edge index
			const uint nextEdgeIndex = min(atomic_fetch_add_explicit(&numberOfEdgesPerDestinationBucket[bucketIndex], 1, memory_order_relaxed), static_cast<uint>(AFTER_TRIMMING_ROUND_MAX_NUMBER_OF_EDGES_PER_BUCKET / 2 - 1));
			
			// Get destination bucket's next indices
			device uint2 *bucketNextIndices = &destinationBuckets[AFTER_TRIMMING_ROUND_MAX_NUMBER_OF_EDGES_PER_BUCKET / 2 * bucketIndex + nextEdgeIndex];
			
			// Set destination bucket's next edge to the edge and its other node
			*bucketNextIndices = uint2(edgeIndex, otherNode);
		}
	}
}

// Trim edges step four
[[kernel]] void trimEdgesStepFour(device const uint2 *__restrict sourceBuckets [[buffer(0)]], device const uint *__restrict numberOfEdgesPerSourceBucket [[buffer(1)]], device uint4 *__restrict destinationBuckets [[buffer(4)]], device atomic_uint *__restrict numberOfEdgesPerDestinationBucket [[buffer(5)]], constant const ulong4 &__restrict sipHashKeys [[buffer(2)]], const ushort localId [[thread_position_in_threadgroup]], const ushort localSize [[threads_per_threadgroup]], const uint groupId [[threadgroup_position_in_grid]]) {

	// Declare bitmap
	threadgroup atomic_uint bitmap[static_cast<short>(NUMBER_OF_BITMAP_BYTES / sizeof(uint))];
	
	// Go through all groups of bytes in the bitmap as a work group
	for(short i = localId; i < static_cast<short>(NUMBER_OF_BITMAP_BYTES / sizeof(uint)); i += localSize) {
	
		// Set group of bytes in the bitmap to zero
		atomic_store_explicit(&bitmap[i], 0, memory_order_relaxed);
	}
	
	// Get number of edges in this work group's bucket
	const uint numberOfEdges = min(numberOfEdgesPerSourceBucket[groupId], static_cast<uint>(AFTER_TRIMMING_ROUND_MAX_NUMBER_OF_EDGES_PER_BUCKET / 2));
	
	// Get work group's bucket's indices
	device const uint2 *indices = &sourceBuckets[AFTER_TRIMMING_ROUND_MAX_NUMBER_OF_EDGES_PER_BUCKET / 2 * groupId];
	
	// Synchronize work group
	threadgroup_barrier(mem_flags::mem_threadgroup);
	
	// Go through all edges in this work group's bucket as a work group
	for(uint i = localId; i < numberOfEdges; i += localSize) {
	
		// Enable edge's node in the bitmap
		setBitInBitmap(bitmap, indices[i].y & BITMAP_MASK);
	}
	
	// Synchronize work group
	threadgroup_barrier(mem_flags::mem_threadgroup);
	
	// Go through all edges in this work group's bucket as a work group
	for(uint i = localId; i < numberOfEdges; i += localSize) {
	
		// Get edge's index and node
		const uint2 edgeIndexAndNode = indices[i];
		
		// Check if edge's node has a pair in the bitmap
		if(isBitSetInBitmap(bitmap, (edgeIndexAndNode.y & BITMAP_MASK) ^ 1)) {
		
			// Get edge's other node
			const uint otherNode = sipHash24(sipHashKeys, (static_cast<ulong>(edgeIndexAndNode.x) * 2) | 1);
			
			// Get edge's other node's bucket index
			const uint bucketIndex = otherNode >> NUMBER_OF_LEAST_SIGNIFICANT_BITS_IGNORED_DURING_BUCKET_SORTING;
			
			// Get bucket's next edge index
			const uint nextEdgeIndex = min(atomic_fetch_add_explicit(&numberOfEdgesPerDestinationBucket[bucketIndex], 1, memory_order_relaxed), static_cast<uint>(AFTER_TRIMMING_ROUND_MAX_NUMBER_OF_EDGES_PER_BUCKET / 4 - 1));
			
			// Get destination bucket's next indices
			device uint4 *bucketNextIndices = &destinationBuckets[AFTER_TRIMMING_ROUND_MAX_NUMBER_OF_EDGES_PER_BUCKET / 4 * bucketIndex + nextEdgeIndex];
			
			// Set destination bucket's next edge to the edge and its nodes
			*bucketNextIndices = uint4(edgeIndexAndNode.x, otherNode, edgeIndexAndNode.y, otherNode & BITMAP_MASK);
		}
	}
}

// Trim edges step five
[[kernel]] void trimEdgesStepFive(device const uint4 *__restrict sourceBuckets [[buffer(0)]], device const uint *__restrict numberOfEdgesPerSourceBucket [[buffer(1)]], device uint4 *__restrict destinationBuckets [[buffer(4)]], device atomic_uint *__restrict numberOfEdgesPerDestinationBucket [[buffer(5)]], const ushort localId [[thread_position_in_threadgroup]], const ushort localSize [[threads_per_threadgroup]], const uint groupId [[threadgroup_position_in_grid]]) {

	// Declare bitmap
	threadgroup atomic_uint bitmap[static_cast<short>(NUMBER_OF_BITMAP_BYTES / sizeof(uint))];
	
	// Go through all groups of bytes in the bitmap as a work group
	for(short i = localId; i < static_cast<short>(NUMBER_OF_BITMAP_BYTES / sizeof(uint)); i += localSize) {
	
		// Set group of bytes in the bitmap to zero
		atomic_store_explicit(&bitmap[i], 0, memory_order_relaxed);
	}
	
	// Get number of edges in this work group's bucket
	const uint numberOfEdges = min(numberOfEdgesPerSourceBucket[groupId], static_cast<uint>(AFTER_TRIMMING_ROUND_MAX_NUMBER_OF_EDGES_PER_BUCKET / 4));
	
	// Get work group's bucket's indices
	device const uint4 *indices = &sourceBuckets[AFTER_TRIMMING_ROUND_MAX_NUMBER_OF_EDGES_PER_BUCKET / 4 * groupId];
	
	// Synchronize work group
	threadgroup_barrier(mem_flags::mem_threadgroup);
	
	// Go through all edges in this work group's bucket as a work group
	for(uint i = localId; i < numberOfEdges; i += localSize) {
	
		// Enable edge's node in the bitmap
		setBitInBitmap(bitmap, indices[i].w);
	}
	
	// Synchronize work group
	threadgroup_barrier(mem_flags::mem_threadgroup);
	
	// Go through all edges in this work group's bucket as a work group
	for(uint i = localId; i < numberOfEdges; i += localSize) {
	
		// Get edge's index and nodes
		const uint4 edgeIndexAndNodes = indices[i];
		
		// Check if edge's node has a pair in the bitmap
		if(isBitSetInBitmap(bitmap, edgeIndexAndNodes.w ^ 1)) {
		
			// Get edge's other node's bucket index
			const uint bucketIndex = edgeIndexAndNodes.z >> NUMBER_OF_LEAST_SIGNIFICANT_BITS_IGNORED_DURING_BUCKET_SORTING;
			
			// Get bucket's next edge index
			const uint nextEdgeIndex = min(atomic_fetch_add_explicit(&numberOfEdgesPerDestinationBucket[bucketIndex], 1, memory_order_relaxed), static_cast<uint>(AFTER_TRIMMING_ROUND_MAX_NUMBER_OF_EDGES_PER_BUCKET / 4 - 1));
			
			// Get destination bucket's next indices
			device uint4 *bucketNextIndices = &destinationBuckets[AFTER_TRIMMING_ROUND_MAX_NUMBER_OF_EDGES_PER_BUCKET / 4 * bucketIndex + nextEdgeIndex];
			
			// Set destination bucket's next edge to the edge and its nodes
			*bucketNextIndices = uint4(edgeIndexAndNodes.xzy, edgeIndexAndNodes.z & BITMAP_MASK);
		}
	}
}

// Check if trimming rounds is one
#if TRIMMING_ROUNDS == 1

	// Trim edges step six
	[[kernel]] void trimEdgesStepSix(device const uint *__restrict buckets [[buffer(4)]], device const uint *__restrict numberOfEdgesPerBucket [[buffer(5)]], device atomic_uint *__restrict remainingEdges [[buffer(6)]], constant const ulong4 &__restrict sipHashKeys [[buffer(2)]], const ushort localId [[thread_position_in_threadgroup]], const ushort localSize [[threads_per_threadgroup]], const uint groupId [[threadgroup_position_in_grid]]) {

// Otherwise check if trimming rounds is two
#elif TRIMMING_ROUNDS == 2

	// Trim edges step six
	[[kernel]] void trimEdgesStepSix(device const uint2 *__restrict buckets [[buffer(4)]], device const uint *__restrict numberOfEdgesPerBucket [[buffer(5)]], device atomic_uint *__restrict remainingEdges [[buffer(6)]], constant const ulong4 &__restrict sipHashKeys [[buffer(2)]], const ushort localId [[thread_position_in_threadgroup]], const ushort localSize [[threads_per_threadgroup]], const uint groupId [[threadgroup_position_in_grid]]) {

// Otherwise
#else

	// Trim edges step six
	[[kernel]] void trimEdgesStepSix(device const uint4 *__restrict buckets [[buffer(4)]], device const uint *__restrict numberOfEdgesPerBucket [[buffer(5)]], device atomic_uint *__restrict remainingEdges [[buffer(6)]], const ushort localId [[thread_position_in_threadgroup]], const ushort localSize [[threads_per_threadgroup]], const uint groupId [[threadgroup_position_in_grid]]) {
#endif

	// Declare index
	threadgroup uint index;
	
	// Check if trimming rounds is one
	#if TRIMMING_ROUNDS == 1
	
		// Get number of edges in this work group's bucket
		const uint numberOfEdges = min(numberOfEdgesPerBucket[groupId], static_cast<uint>(AFTER_TRIMMING_ROUND_MAX_NUMBER_OF_EDGES_PER_BUCKET));
		
		// Get work group's bucket's indices
		device const uint *indices = &buckets[AFTER_TRIMMING_ROUND_MAX_NUMBER_OF_EDGES_PER_BUCKET * groupId];
		
	// Otherwise check if trimming rounds is two
	#elif TRIMMING_ROUNDS == 2
	
		// Get number of edges in this work group's bucket
		const uint numberOfEdges = min(numberOfEdgesPerBucket[groupId], static_cast<uint>(AFTER_TRIMMING_ROUND_MAX_NUMBER_OF_EDGES_PER_BUCKET / 2));
		
		// Get work group's bucket's indices
		device const uint2 *indices = &buckets[AFTER_TRIMMING_ROUND_MAX_NUMBER_OF_EDGES_PER_BUCKET / 2 * groupId];
		
	// Otherwise
	#else
	
		// Get number of edges in this work group's bucket
		const uint numberOfEdges = min(numberOfEdgesPerBucket[groupId], static_cast<uint>(AFTER_TRIMMING_ROUND_MAX_NUMBER_OF_EDGES_PER_BUCKET / 4));
		
		// Get work group's bucket's indices
		device const uint4 *indices = &buckets[AFTER_TRIMMING_ROUND_MAX_NUMBER_OF_EDGES_PER_BUCKET / 4 * groupId];
	#endif
	
	// Check if this work item is the first in the work group
	if(localId == 0) {
	
		// Add number of edges to the number of remaining edges
		index = atomic_fetch_add_explicit(remainingEdges, numberOfEdges, memory_order_relaxed);
	}
	
	// Synchronize work group
	threadgroup_barrier(mem_flags::mem_threadgroup);
	
	// Go through all edges in this work group's bucket as a work group
	for(uint i = localId; i < numberOfEdges; i += localSize) {
	
		// Get next remaining edge
		device atomic_uint *nextRemainingEdge = &remainingEdges[min(index + i, static_cast<uint>(MAX_NUMBER_OF_EDGES_AFTER_TRIMMING - 1)) * EDGE_NUMBER_OF_COMPONENTS + 1];
		
		// Check if trimming rounds is one
		#if TRIMMING_ROUNDS == 1
		
			// Get edge's index
			const uint edgeIndex = indices[i];
			
			// Get edge's node
			const uint node = sipHash24(sipHashKeys, static_cast<ulong>(edgeIndex) * 2);
			
			// Get edge's other node
			const uint otherNode = sipHash24(sipHashKeys, (static_cast<ulong>(edgeIndex) * 2) | 1);
			
			// Set next remaining edge to the edge and its nodes
			atomic_store_explicit(&nextRemainingEdge[0], edgeIndex, memory_order_relaxed);
			atomic_store_explicit(&nextRemainingEdge[1], node, memory_order_relaxed);
			atomic_store_explicit(&nextRemainingEdge[2], otherNode, memory_order_relaxed);
			
		// Otherwise check if trimming rounds is two
		#elif TRIMMING_ROUNDS == 2
		
			// Get edge's other node
			const uint otherNode = sipHash24(sipHashKeys, (static_cast<ulong>(indices[i].x) * 2) | 1);
			
			// Set next remaining edge to the edge and its nodes
			atomic_store_explicit(&nextRemainingEdge[0], indices[i].x, memory_order_relaxed);
			atomic_store_explicit(&nextRemainingEdge[1], indices[i].y, memory_order_relaxed);
			atomic_store_explicit(&nextRemainingEdge[2], otherNode, memory_order_relaxed);
			
		// Otherwise
		#else
		
			// Check if trimming rounds is even
			#if TRIMMING_ROUNDS % 2 == 0
			
				// Set next remaining edge to the edge and its nodes
				atomic_store_explicit(&nextRemainingEdge[0], indices[i].x, memory_order_relaxed);
				atomic_store_explicit(&nextRemainingEdge[1], indices[i].y, memory_order_relaxed);
				atomic_store_explicit(&nextRemainingEdge[2], indices[i].z, memory_order_relaxed);
			
			// Otherwise
			#else
			
				// Set next remaining edge to the edge and its nodes
				atomic_store_explicit(&nextRemainingEdge[0], indices[i].x, memory_order_relaxed);
				atomic_store_explicit(&nextRemainingEdge[1], indices[i].z, memory_order_relaxed);
				atomic_store_explicit(&nextRemainingEdge[2], indices[i].y, memory_order_relaxed);
			#endif
		#endif
	}
}

// Check if number of buckets is one
#if NUMBER_OF_BUCKETS == 1

	// Clear number of edges per bucket
	[[kernel]] void clearNumberOfEdgesPerBucket(device uint *numberOfEdgesPerBucket [[buffer(5)]], const uint globalId [[thread_position_in_grid]]) {
	
// Otherwise
#else

	// Clear number of edges per bucket
	[[kernel]] void clearNumberOfEdgesPerBucket(device ulong *numberOfEdgesPerBucket [[buffer(5)]], const uint globalId [[thread_position_in_grid]]) {
#endif

	// Set this work item's value in number of edges per bucket to zero
	numberOfEdgesPerBucket[globalId] = 0;
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
void setBitInBitmap(threadgroup atomic_uint *bitmap, const uint index) {

	// Set bit in bitmap
	atomic_fetch_or_explicit(&bitmap[index / static_cast<char>(sizeof(uint) * BITS_IN_A_BYTE)], 1 << (index % static_cast<char>(sizeof(uint) * BITS_IN_A_BYTE)), memory_order_relaxed);
}

// Is bit set in bitmap
bool isBitSetInBitmap(threadgroup const atomic_uint *bitmap, const uint index) {

	// Return if bit is set in bitmap
	return atomic_load_explicit(&bitmap[index / static_cast<char>(sizeof(uint) * BITS_IN_A_BYTE)], memory_order_relaxed) & (1 << (index % static_cast<char>(sizeof(uint) * BITS_IN_A_BYTE)));
}


)")

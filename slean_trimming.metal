MTLSTR(R"(

/*
N trimming rounds can be performed with the following:
Trimming round 1: clear number of edges per bucket one, step one, step two, clear number of edges per bucket one, step one, step three, clear number of edges per bucket one, step one, step three, ..., clear number of edges per bucket one, step one, step three, clear number of edges per bucket one, step one, clear number of edges per bucket two, step four, step five, clear number of edges per bucket one, step one, clear number of edges per bucket two, step six, step five, clear number of edges per bucket one, step one, clear number of edges per bucket two, step six, step five, ..., clear number of edges per bucket one, step one, clear number of edges per bucket two, step six, step five
Trimming round 2: clear number of edges per bucket one, step seven, step eight, clear number of edges per bucket one, step seven, step nine, clear number of edges per bucket one, step seven, step nine, ..., clear number of edges per bucket one, step seven, step nine, clear number of edges per bucket one, step seven, clear number of edges per bucket two, step ten, step eleven, clear number of edges per bucket one, step seven, clear number of edges per bucket two, step twelve, step eleven, clear number of edges per bucket one, step seven, clear number of edges per bucket two, step twelve, step eleven, ..., clear number of edges per bucket one, step seven, clear number of edges per bucket two, step twelve, step eleven
Trimming round 3: clear number of edges per bucket one, step thirteen, step fourteen, clear number of edges per bucket one, step thirteen, step fifteen, clear number of edges per bucket one, step thirteen, step fifteen, ..., clear number of edges per bucket one, step thirteen, step fifteen, clear number of edges per bucket one, step thirteen, clear number of edges per bucket two, step sixteen, step seventeen, clear number of edges per bucket one, step thirteen, clear number of edges per bucket two, step eighteen, step seventeen, clear number of edges per bucket one, step thirteen, clear number of edges per bucket two, step eighteen, step seventeen, ..., clear number of edges per bucket one, step thirteen, clear number of edges per bucket two, step eighteen, step seventeen
Trimming round 4: clear number of edges per bucket one, step nineteen, step twenty, clear number of edges per bucket one, step nineteen, step twenty-one, clear number of edges per bucket one, step nineteen, step twenty-one, ..., clear number of edges per bucket one, step nineteen, step twenty-one, clear number of edges per bucket one, step nineteen, clear number of edges per bucket two, step twenty-two, step seventeen, clear number of edges per bucket one, step nineteen, clear number of edges per bucket two, step twenty-three, step seventeen, clear number of edges per bucket one, step nineteen, clear number of edges per bucket two, step twenty-three, step seventeen, ..., clear number of edges per bucket one, step nineteen, clear number of edges per bucket two, step twenty-three, step seventeen
Repeat trimming round 4 if 2 trimming parts or until at most 25% of edges remain
Trimming round 5: clear number of edges per bucket one, step twenty-four, step twenty, clear number of edges per bucket one, step twenty-four, step twenty-one, clear number of edges per bucket one, step twenty-four, step twenty-one, ..., clear number of edges per bucket one, step twenty-four, step twenty-one, clear number of edges per bucket one, step twenty-four, clear number of edges per bucket two, step twenty-five, step twenty-six, clear number of edges per bucket one, step twenty-four, clear number of edges per bucket two, step twenty-seven, step twenty-six, clear number of edges per bucket one, step twenty-four, clear number of edges per bucket two, step twenty-seven, step twenty-six, ..., clear number of edges per bucket one, step twenty-four, clear number of edges per bucket two, step twenty-seven, step twenty-six
Repeat trimming round 5 if 4 trimming parts or until at most 12.5% of edges remain
Trimming round 6: clear number of edges per bucket one, step twenty-eight, step twenty, clear number of edges per bucket one, step twenty-eight, step twenty-one, clear number of edges per bucket one, step twenty-eight, step twenty-one, ..., clear number of edges per bucket one, step twenty-eight, step twenty-one, clear number of edges per bucket one, step twenty-eight, clear number of edges per bucket two, step twenty-nine, step thirty, clear number of edges per bucket one, step twenty-eight, clear number of edges per bucket two, step thirty-one, step thirty, clear number of edges per bucket one, step twenty-eight, clear number of edges per bucket two, step thirty-one, step thirty, ..., clear number of edges per bucket one, step twenty-eight, clear number of edges per bucket two, step thirty-one, step thirty
Repeat trimming round 6 if 8 trimming parts or until at most 6.25% of edges remain
Trimming round 7: clear number of edges per bucket one, step thirty-two, clear number of edges per bucket two, step thirty-three, step thirty-four
...
Trimming round n - 1: clear number of edges per bucket one, step thirty-two, clear number of edges per bucket two, step thirty-three, step thirty-four
Trimming round n: clear number of edges per bucket one, step thirty-two, clear number of edges per bucket two, step thirty-three, step thirty-four
Get result from edges bitmap

This trimming algorithm can be made faster by switching to mean trimming once the number of trimming parts has been reduced to one.
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

// Remaining edges bitmap mask
#define REMAINING_EDGES_BITMAP_MASK (NUMBER_OF_REMAINING_EDGES_BITMAP_BYTES * BITS_IN_A_BYTE - 1)


// Function prototypes

// Check if local buckets size is one
#if LOCAL_BUCKETS_SIZE == 1

	// Check if initial buckets isn't disjointed
	#if INITIAL_BUCKETS_NUMBER_OF_BUCKETS == NUMBER_OF_BUCKETS
	
		// Trim edges step one
		[[kernel]] void trimEdgesStepOne(device uint *buckets, device atomic_uint *numberOfEdgesPerBucket, constant const ulong4 &sipHashKeys, constant const uchar &part, const uint globalId);
	
	// Otherwise
	#else
	
		// Trim edges step one
		[[kernel]] void trimEdgesStepOne(device uint *buckets, device atomic_uint *numberOfEdgesPerBucket, constant const ulong4 &sipHashKeys, constant const uchar &part, device uint *bucketsSecondPart, const uint globalId);
	#endif

// Otherwise check if local buckets size is two
#elif LOCAL_BUCKETS_SIZE == 2

	// Check if initial buckets isn't disjointed
	#if INITIAL_BUCKETS_NUMBER_OF_BUCKETS == NUMBER_OF_BUCKETS
	
		// Trim edges step one
		[[kernel]] void trimEdgesStepOne(device uint2 *buckets, device atomic_uint *numberOfEdgesPerBucket, constant const ulong4 &sipHashKeys, constant const uchar &part, const uint globalId, const ushort localId, const ushort localSize);
	
	// Otherwise
	#else
	
		// Trim edges step one
		[[kernel]] void trimEdgesStepOne(device uint2 *buckets, device atomic_uint *numberOfEdgesPerBucket, constant const ulong4 &sipHashKeys, constant const uchar &part, device uint2 *bucketsSecondPart, const uint globalId, const ushort localId, const ushort localSize);
	#endif

// Otherwise
#else

	// Check if initial buckets isn't disjointed
	#if INITIAL_BUCKETS_NUMBER_OF_BUCKETS == NUMBER_OF_BUCKETS
	
		// Trim edges step one
		[[kernel]] void trimEdgesStepOne(device uint4 *buckets, device atomic_uint *numberOfEdgesPerBucket, constant const ulong4 &sipHashKeys, constant const uchar &part, const uint globalId, const ushort localId, const ushort localSize);
	
	// Otherwise
	#else
	
		// Trim edges step one
		[[kernel]] void trimEdgesStepOne(device uint4 *buckets, device atomic_uint *numberOfEdgesPerBucket, constant const ulong4 &sipHashKeys, constant const uchar &part, device uint4 *bucketsSecondPart, const uint globalId, const ushort localId, const ushort localSize);
	#endif
#endif

// Check if initial buckets isn't disjointed
#if INITIAL_BUCKETS_NUMBER_OF_BUCKETS == NUMBER_OF_BUCKETS

	// Trim edges step two
	[[kernel]] void trimEdgesStepTwo(device const uint *buckets, device const uint *numberOfEdgesPerBucket, device uint *nodesBitmap, constant const ulong4 &sipHashKeys, const ushort localId, const ushort localSize, const uint groupId);

// Otherwise
#else

	// Trim edges step two
	[[kernel]] void trimEdgesStepTwo(device const uint *buckets, device const uint *numberOfEdgesPerBucket, device uint *nodesBitmap, constant const ulong4 &sipHashKeys, device const uint *bucketsSecondPart, const ushort localId, const ushort localSize, const uint groupId);
#endif

// Check if initial buckets isn't disjointed
#if INITIAL_BUCKETS_NUMBER_OF_BUCKETS == NUMBER_OF_BUCKETS

	// Trim edges step three
	[[kernel]] void trimEdgesStepThree(device const uint *buckets, device const uint *numberOfEdgesPerBucket, device uint *nodesBitmap, constant const ulong4 &sipHashKeys, const ushort localId, const ushort localSize, const uint groupId);

// Otherwise
#else

	// Trim edges step three
	[[kernel]] void trimEdgesStepThree(device const uint *buckets, device const uint *numberOfEdgesPerBucket, device uint *nodesBitmap, constant const ulong4 &sipHashKeys, device const uint *bucketsSecondPart, const ushort localId, const ushort localSize, const uint groupId);
#endif

// Check if initial buckets isn't disjointed
#if INITIAL_BUCKETS_NUMBER_OF_BUCKETS == NUMBER_OF_BUCKETS

	// Trim edges step four
	[[kernel]] void trimEdgesStepFour(device const uint *sourceBuckets, device const uint *numberOfEdgesPerSourceBucket, device uint *nodesBitmap, device uint *destinationBuckets, device atomic_uint *numberOfEdgesPerDestinationBucket, constant const ulong4 &sipHashKeys, const ushort localId, const ushort localSize, const uint groupId);

// Otherwise
#else

	// Trim edges step four
	[[kernel]] void trimEdgesStepFour(device const uint *sourceBuckets, device const uint *numberOfEdgesPerSourceBucket, device uint *nodesBitmap, device uint *destinationBuckets, device atomic_uint *numberOfEdgesPerDestinationBucket, constant const ulong4 &sipHashKeys, device const uint *sourceBucketsSecondPart, const ushort localId, const ushort localSize, const uint groupId);
#endif

// Trim edges step five
[[kernel]] void trimEdgesStepFive(device const uint *buckets, device const uint *numberOfEdgesPerBucket, device uint *edgesBitmap, constant const uchar &part, const ushort localId, const ushort localSize, const uint groupId);

// Check if initial buckets isn't disjointed
#if INITIAL_BUCKETS_NUMBER_OF_BUCKETS == NUMBER_OF_BUCKETS

	// Trim edges step six
	[[kernel]] void trimEdgesStepSix(device const uint *sourceBuckets, device const uint *numberOfEdgesPerSourceBucket, device const uint *nodesBitmap, device uint *destinationBuckets, device atomic_uint *numberOfEdgesPerDestinationBucket, constant const ulong4 &sipHashKeys, constant const uchar &part, const ushort localId, const ushort localSize, const uint groupId);

// Otherwise
#else

	// Trim edges step six
	[[kernel]] void trimEdgesStepSix(device const uint *sourceBuckets, device const uint *numberOfEdgesPerSourceBucket, device const uint *nodesBitmap, device uint *destinationBuckets, device atomic_uint *numberOfEdgesPerDestinationBucket, constant const ulong4 &sipHashKeys, constant const uchar &part, device const uint *sourceBucketsSecondPart, const ushort localId, const ushort localSize, const uint groupId);
#endif

// Check if local buckets size is one
#if LOCAL_BUCKETS_SIZE == 1

	// Trim edges step seven
	[[kernel]] void trimEdgesStepSeven(device const ulong *edgesBitmap, device uint *buckets, device atomic_uint *numberOfEdgesPerBucket, constant const ulong4 &sipHashKeys, constant const uchar &part, const uint globalId);

// Otherwise check if local buckets size is two
#elif LOCAL_BUCKETS_SIZE == 2

	// Trim edges step seven
	[[kernel]] void trimEdgesStepSeven(device const ulong *edgesBitmap, device uint2 *buckets, device atomic_uint *numberOfEdgesPerBucket, constant const ulong4 &sipHashKeys, constant const uchar &part, const uint globalId, const ushort localId, const ushort localSize);

// Otherwise
#else

	// Trim edges step seven
	[[kernel]] void trimEdgesStepSeven(device const ulong *edgesBitmap, device uint4 *buckets, device atomic_uint *numberOfEdgesPerBucket, constant const ulong4 &sipHashKeys, constant const uchar &part, const uint globalId, const ushort localId, const ushort localSize);
#endif

// Trim edges step eight
[[kernel]] void trimEdgesStepEight(device const uint *buckets, device const uint *numberOfEdgesPerBucket, device uint *nodesBitmap, constant const ulong4 &sipHashKeys, const ushort localId, const ushort localSize, const uint groupId);

// Trim edges step nine
[[kernel]] void trimEdgesStepNine(device const uint *buckets, device const uint *numberOfEdgesPerBucket, device uint *nodesBitmap, constant const ulong4 &sipHashKeys, const ushort localId, const ushort localSize, const uint groupId);

// Trim edges step ten
[[kernel]] void trimEdgesStepTen(device const uint *sourceBuckets, device const uint *numberOfEdgesPerSourceBucket, device uint *nodesBitmap, device uint *destinationBuckets, device atomic_uint *numberOfEdgesPerDestinationBucket, constant const ulong4 &sipHashKeys, const ushort localId, const ushort localSize, const uint groupId);

// Trim edges step eleven
[[kernel]] void trimEdgesStepEleven(device const uint *buckets, device const uint *numberOfEdgesPerBucket, device uint *edgesBitmap, constant const uchar &part, const ushort localId, const ushort localSize, const uint groupId);

// Trim edges step twelve
[[kernel]] void trimEdgesStepTwelve(device const uint *sourceBuckets, device const uint *numberOfEdgesPerSourceBucket, device const uint *nodesBitmap, device uint *destinationBuckets, device atomic_uint *numberOfEdgesPerDestinationBucket, constant const ulong4 &sipHashKeys, constant const uchar &part, const ushort localId, const ushort localSize, const uint groupId);

// Check is slean trimming parts is two
#if SLEAN_TRIMMING_PARTS == 2

	// Trim edges step thirteen
	[[kernel]] void trimEdgesStepThirteen(device const ulong *edgesBitmap, device uint *buckets, device atomic_uint *numberOfEdgesPerBucket, constant const ulong4 &sipHashKeys, const uint globalId);

// Otherwise
#else

	// Trim edges step thirteen
	[[kernel]] void trimEdgesStepThirteen(device const ulong *edgesBitmap, device uint *buckets, device atomic_uint *numberOfEdgesPerBucket, constant const ulong4 &sipHashKeys, constant const uchar &part, const uint globalId);
#endif

// Trim edges step fourteen
[[kernel]] void trimEdgesStepFourteen(device const uint *buckets, device const uint *numberOfEdgesPerBucket, device uint *nodesBitmap, constant const ulong4 &sipHashKeys, const ushort localId, const ushort localSize, const uint groupId);

// Trim edges step fifteen
[[kernel]] void trimEdgesStepFifteen(device const uint *buckets, device const uint *numberOfEdgesPerBucket, device uint *nodesBitmap, constant const ulong4 &sipHashKeys, const ushort localId, const ushort localSize, const uint groupId);

// Check is slean trimming parts is two
#if SLEAN_TRIMMING_PARTS == 2

	// Trim edges step sixteen
	[[kernel]] void trimEdgesStepSixteen(device const uint *sourceBuckets, device const uint *numberOfEdgesPerSourceBucket, device uint *destinationBuckets, device atomic_uint *numberOfEdgesPerDestinationBucket, constant const ulong4 &sipHashKeys, const ushort localId, const ushort localSize, const uint groupId);

// Otherwise
#else

	// Trim edges step sixteen
	[[kernel]] void trimEdgesStepSixteen(device const uint *sourceBuckets, device const uint *numberOfEdgesPerSourceBucket, device uint *destinationBuckets, device atomic_uint *numberOfEdgesPerDestinationBucket, constant const ulong4 &sipHashKeys, device uint *nodesBitmap, const ushort localId, const ushort localSize, const uint groupId);
#endif

// Check is slean trimming parts is two
#if SLEAN_TRIMMING_PARTS == 2

	// Trim edges step seventeen
	[[kernel]] void trimEdgesStepSeventeen(device const uint *buckets, device const uint *numberOfEdgesPerBucket, device uint *edgesBitmap, const ushort localId, const ushort localSize, const uint groupId);

// Otherwise
#else

	// Trim edges step seventeen
	[[kernel]] void trimEdgesStepSeventeen(device const uint *buckets, device const uint *numberOfEdgesPerBucket, device uint *edgesBitmap, constant const uchar &part, const ushort localId, const ushort localSize, const uint groupId);
#endif

// Check is slean trimming parts is two
#if SLEAN_TRIMMING_PARTS == 2

	// Trim edges step eighteen
	[[kernel]] void trimEdgesStepEighteen(device const uint *sourceBuckets, device const uint *numberOfEdgesPerSourceBucket, device const uint *nodesBitmap, device uint *destinationBuckets, device atomic_uint *numberOfEdgesPerDestinationBucket, constant const ulong4 &sipHashKeys, const ushort localId, const ushort localSize, const uint groupId);

// Otherwise
#else

	// Trim edges step eighteen
	[[kernel]] void trimEdgesStepEighteen(device const uint *sourceBuckets, device const uint *numberOfEdgesPerSourceBucket, device const uint *nodesBitmap, device uint *destinationBuckets, device atomic_uint *numberOfEdgesPerDestinationBucket, constant const ulong4 &sipHashKeys, constant const uchar &part, const ushort localId, const ushort localSize, const uint groupId);
#endif

// Check is slean trimming parts is two
#if SLEAN_TRIMMING_PARTS == 2

	// Trim edges step nineteen
	[[kernel]] void trimEdgesStepNineteen(device const ulong *edgesBitmap, device uint2 *buckets, device atomic_uint *numberOfEdgesPerBucket, constant const uchar &nodesInSecondPartition, constant const ulong4 &sipHashKeys, const uint globalId);

// Otherwise
#else

	// Trim edges step nineteen
	[[kernel]] void trimEdgesStepNineteen(device const ulong *edgesBitmap, device uint2 *buckets, device atomic_uint *numberOfEdgesPerBucket, constant const uchar &nodesInSecondPartition, constant const ulong4 &sipHashKeys, constant const uchar &part, const uint globalId);
#endif

// Trim edges step twenty
[[kernel]] void trimEdgesStepTwenty(device const uint2 *buckets, device const uint *numberOfEdgesPerBucket, device uint *nodesBitmap, const ushort localId, const ushort localSize, const uint groupId);

// Trim edges step twenty-one
[[kernel]] void trimEdgesStepTwentyOne(device const uint2 *buckets, device const uint *numberOfEdgesPerBucket, device uint *nodesBitmap, const ushort localId, const ushort localSize, const uint groupId);

// Check is slean trimming parts is two
#if SLEAN_TRIMMING_PARTS == 2

	// Trim edges step twenty-two
	[[kernel]] void trimEdgesStepTwentyTwo(device const uint2 *sourceBuckets, device const uint *numberOfEdgesPerSourceBucket, device uint *destinationBuckets, device atomic_uint *numberOfEdgesPerDestinationBucket, const ushort localId, const ushort localSize, const uint groupId);

// Otherwise
#else

	// Trim edges step twenty-two
	[[kernel]] void trimEdgesStepTwentyTwo(device const uint2 *sourceBuckets, device const uint *numberOfEdgesPerSourceBucket, device uint *destinationBuckets, device atomic_uint *numberOfEdgesPerDestinationBucket, device uint *nodesBitmap, const ushort localId, const ushort localSize, const uint groupId);
#endif

// Check is slean trimming parts is two
#if SLEAN_TRIMMING_PARTS == 2

	// Trim edges step twenty-three
	[[kernel]] void trimEdgesStepTwentyThree(device const uint2 *sourceBuckets, device const uint *numberOfEdgesPerSourceBucket, device const uint *nodesBitmap, device uint *destinationBuckets, device atomic_uint *numberOfEdgesPerDestinationBucket, const ushort localId, const ushort localSize, const uint groupId);

// Otherwise
#else

	// Trim edges step twenty-three
	[[kernel]] void trimEdgesStepTwentyThree(device const uint2 *sourceBuckets, device const uint *numberOfEdgesPerSourceBucket, device const uint *nodesBitmap, device uint *destinationBuckets, device atomic_uint *numberOfEdgesPerDestinationBucket, constant const uchar &part, const ushort localId, const ushort localSize, const uint groupId);
#endif

// Check is slean trimming parts is four
#if SLEAN_TRIMMING_PARTS == 4

	// Trim edges step twenty-four
	[[kernel]] void trimEdgesStepTwentyFour(device const ulong *edgesBitmap, device uint2 *buckets, device atomic_uint *numberOfEdgesPerBucket, constant const uchar &nodesInSecondPartition, constant const ulong4 &sipHashKeys, const uint globalId);

// Otherwise
#else

	// Trim edges step twenty-four
	[[kernel]] void trimEdgesStepTwentyFour(device const ulong *edgesBitmap, device uint2 *buckets, device atomic_uint *numberOfEdgesPerBucket, constant const uchar &nodesInSecondPartition, constant const ulong4 &sipHashKeys, constant const uchar &part, const uint globalId);
#endif

// Check is slean trimming parts is four
#if SLEAN_TRIMMING_PARTS == 4

	// Trim edges step twenty-five
	[[kernel]] void trimEdgesStepTwentyFive(device const uint2 *sourceBuckets, device const uint *numberOfEdgesPerSourceBucket, device uint *destinationBuckets, device atomic_uint *numberOfEdgesPerDestinationBucket, const ushort localId, const ushort localSize, const uint groupId);

// Otherwise
#else

	// Trim edges step twenty-five
	[[kernel]] void trimEdgesStepTwentyFive(device const uint2 *sourceBuckets, device const uint *numberOfEdgesPerSourceBucket, device uint *destinationBuckets, device atomic_uint *numberOfEdgesPerDestinationBucket, device uint *nodesBitmap, const ushort localId, const ushort localSize, const uint groupId);
#endif

// Check is slean trimming parts is four
#if SLEAN_TRIMMING_PARTS == 4

	// Trim edges step twenty-six
	[[kernel]] void trimEdgesStepTwentySix(device const uint *buckets, device const uint *numberOfEdgesPerBucket, device uint *edgesBitmap, const ushort localId, const ushort localSize, const uint groupId);

// Otherwise
#else

	// Trim edges step twenty-six
	[[kernel]] void trimEdgesStepTwentySix(device const uint *buckets, device const uint *numberOfEdgesPerBucket, device uint *edgesBitmap, constant const uchar &part, const ushort localId, const ushort localSize, const uint groupId);
#endif

// Check is slean trimming parts is four
#if SLEAN_TRIMMING_PARTS == 4

	// Trim edges step twenty-seven
	[[kernel]] void trimEdgesStepTwentySeven(device const uint2 *sourceBuckets, device const uint *numberOfEdgesPerSourceBucket, device const uint *nodesBitmap, device uint *destinationBuckets, device atomic_uint *numberOfEdgesPerDestinationBucket, const ushort localId, const ushort localSize, const uint groupId);

// Otherwise
#else

	// Trim edges step twenty-seven
	[[kernel]] void trimEdgesStepTwentySeven(device const uint2 *sourceBuckets, device const uint *numberOfEdgesPerSourceBucket, device const uint *nodesBitmap, device uint *destinationBuckets, device atomic_uint *numberOfEdgesPerDestinationBucket, constant const uchar &part, const ushort localId, const ushort localSize, const uint groupId);
#endif

// Check is slean trimming parts is eight
#if SLEAN_TRIMMING_PARTS == 8

	// Trim edges step twenty-eight
	[[kernel]] void trimEdgesStepTwentyEight(device const ulong *edgesBitmap, device uint2 *buckets, device atomic_uint *numberOfEdgesPerBucket, constant const uchar &nodesInSecondPartition, constant const ulong4 &sipHashKeys, const uint globalId);

// Otherwise
#else

	// Trim edges step twenty-eight
	[[kernel]] void trimEdgesStepTwentyEight(device const ulong *edgesBitmap, device uint2 *buckets, device atomic_uint *numberOfEdgesPerBucket, constant const uchar &nodesInSecondPartition, constant const ulong4 &sipHashKeys, constant const uchar &part, const uint globalId);
#endif

// Check is slean trimming parts is eight
#if SLEAN_TRIMMING_PARTS == 8

	// Trim edges step twenty-nine
	[[kernel]] void trimEdgesStepTwentyNine(device const uint2 *sourceBuckets, device const uint *numberOfEdgesPerSourceBucket, device uint *destinationBuckets, device atomic_uint *numberOfEdgesPerDestinationBucket, const ushort localId, const ushort localSize, const uint groupId);

// Otherwise
#else

	// Trim edges step twenty-nine
	[[kernel]] void trimEdgesStepTwentyNine(device const uint2 *sourceBuckets, device const uint *numberOfEdgesPerSourceBucket, device uint *destinationBuckets, device atomic_uint *numberOfEdgesPerDestinationBucket, device uint *nodesBitmap, const ushort localId, const ushort localSize, const uint groupId);
#endif

// Check is slean trimming parts is eight
#if SLEAN_TRIMMING_PARTS == 8

	// Trim edges step thirty
	[[kernel]] void trimEdgesStepThirty(device const uint *buckets, device const uint *numberOfEdgesPerBucket, device uint *edgesBitmap, const ushort localId, const ushort localSize, const uint groupId);

// Otherwise
#else

	// Trim edges step thirty
	[[kernel]] void trimEdgesStepThirty(device const uint *buckets, device const uint *numberOfEdgesPerBucket, device uint *edgesBitmap, constant const uchar &part, const ushort localId, const ushort localSize, const uint groupId);
#endif

// Check is slean trimming parts is eight
#if SLEAN_TRIMMING_PARTS == 8

	// Trim edges step thirty-one
	[[kernel]] void trimEdgesStepThirtyOne(device const uint2 *sourceBuckets, device const uint *numberOfEdgesPerSourceBucket, device const uint *nodesBitmap, device uint *destinationBuckets, device atomic_uint *numberOfEdgesPerDestinationBucket, const ushort localId, const ushort localSize, const uint groupId);

// Otherwise
#else

	// Trim edges step thirty-one
	[[kernel]] void trimEdgesStepThirtyOne(device const uint2 *sourceBuckets, device const uint *numberOfEdgesPerSourceBucket, device const uint *nodesBitmap, device uint *destinationBuckets, device atomic_uint *numberOfEdgesPerDestinationBucket, constant const uchar &part, const ushort localId, const ushort localSize, const uint groupId);
#endif

// Trim edges step thirty-two
[[kernel]] void trimEdgesStepThirtyTwo(device const ulong *edgesBitmap, device uint2 *buckets, device atomic_uint *numberOfEdgesPerBucket, constant const uchar &nodesInSecondPartition, constant const ulong4 &sipHashKeys, const uint globalId);

// Trim edges step thirty-three
[[kernel]] void trimEdgesStepThirtyThree(device const uint2 *sourceBuckets, device const uint *numberOfEdgesPerSourceBucket, device uint *destinationBuckets, device atomic_uint *numberOfEdgesPerDestinationBucket, const ushort localId, const ushort localSize, const uint groupId);

// Trim edges step thirty-four
[[kernel]] void trimEdgesStepThirtyFour(device const uint *buckets, device const uint *numberOfEdgesPerBucket, device uint *edgesBitmap, const ushort localId, const ushort localSize, const uint groupId);

// Check if number of buckets is one
#if NUMBER_OF_BUCKETS == 1

	// Clear number of edges per source bucket
	[[kernel]] void clearNumberOfEdgesPerSourceBucket(device uint *numberOfEdgesPerSourceBucket, const uint globalId);
	
// Otherwise
#else

	// Clear number of edges per source bucket
	[[kernel]] void clearNumberOfEdgesPerSourceBucket(device ulong *numberOfEdgesPerSourceBucket, const uint globalId);
#endif

// Check if number of remaining edges buckets is one
#if NUMBER_OF_REMAINING_EDGES_BUCKETS == 1

	// Clear number of edges per destination bucket one
	[[kernel]] void clearNumberOfEdgesPerDestinationBucketOne(device uint *numberOfEdgesPerDestinationBucket, const uint globalId);

// Otherwise
#else

	// Clear number of edges per destination bucket one
	[[kernel]] void clearNumberOfEdgesPerDestinationBucketOne(device ulong *numberOfEdgesPerDestinationBucket, const uint globalId);
#endif

// Clear number of edges per destination bucket two
[[kernel]] void clearNumberOfEdgesPerDestinationBucketTwo(device ulong *numberOfEdgesPerDestinationBucket, const uint globalId);

// Clear number of edges per destination bucket four
[[kernel]] void clearNumberOfEdgesPerDestinationBucketFour(device ulong *numberOfEdgesPerDestinationBucket, const uint globalId);

// Clear number of edges per destination bucket eight
[[kernel]] void clearNumberOfEdgesPerDestinationBucketEight(device ulong *numberOfEdgesPerDestinationBucket, const uint globalId);

// Clear number of edges per destination bucket sixteen
[[kernel]] void clearNumberOfEdgesPerDestinationBucketSixteen(device ulong *numberOfEdgesPerDestinationBucket, const uint globalId);

// SipHash-2-4
static inline uint sipHash24(ulong4 keys, const ulong nonce);

// SipRound
static inline void sipRound(thread ulong4 &keys);

// Set bit in bitmap
static inline void setBitInBitmap(threadgroup atomic_uint *bitmap, const uint index);

// Clear bit in bitmap
static inline void clearBitInBitmap(threadgroup atomic_uint *bitmap, const uint index);

// Is bit set in bitmap
static inline bool isBitSetInBitmap(threadgroup const atomic_uint *bitmap, const uint index);

// Is bit set in bitmap
static inline bool isBitSetInBitmap(threadgroup const uint *bitmap, const uint index);


// Supporting function implementation

// Check if local buckets size is one
#if LOCAL_BUCKETS_SIZE == 1

	// Check if initial buckets isn't disjointed
	#if INITIAL_BUCKETS_NUMBER_OF_BUCKETS == NUMBER_OF_BUCKETS
	
		// Trim edges step one
		[[kernel]] void trimEdgesStepOne(device uint *buckets [[buffer(0)]], device atomic_uint *numberOfEdgesPerBucket [[buffer(1)]], constant const ulong4 &sipHashKeys [[buffer(2)]], constant const uchar &part [[buffer(3)]], const uint globalId [[thread_position_in_grid]]) {
	
	// Otherwise
	#else
	
		// Trim edges step one
		[[kernel]] void trimEdgesStepOne(device uint *buckets [[buffer(0)]], device atomic_uint *numberOfEdgesPerBucket [[buffer(1)]], constant const ulong4 &sipHashKeys [[buffer(2)]], constant const uchar &part [[buffer(3)]], device uint *bucketsSecondPart [[buffer(4)]], const uint globalId [[thread_position_in_grid]]) {
	#endif

// Otherwise check if local buckets size is two
#elif LOCAL_BUCKETS_SIZE == 2

	// Check if initial buckets isn't disjointed
	#if INITIAL_BUCKETS_NUMBER_OF_BUCKETS == NUMBER_OF_BUCKETS
	
		// Trim edges step one
		[[kernel]] void trimEdgesStepOne(device uint2 *buckets [[buffer(0)]], device atomic_uint *numberOfEdgesPerBucket [[buffer(1)]], constant const ulong4 &sipHashKeys [[buffer(2)]], constant const uchar &part [[buffer(3)]], const uint globalId [[thread_position_in_grid]], const ushort localId [[thread_position_in_threadgroup]], const ushort localSize [[threads_per_threadgroup]]) {
	
	// Otherwise
	#else
	
		// Trim edges step one
		[[kernel]] void trimEdgesStepOne(device uint2 *buckets [[buffer(0)]], device atomic_uint *numberOfEdgesPerBucket [[buffer(1)]], constant const ulong4 &sipHashKeys [[buffer(2)]], constant const uchar &part [[buffer(3)]], device uint2 *bucketsSecondPart [[buffer(4)]], const uint globalId [[thread_position_in_grid]], const ushort localId [[thread_position_in_threadgroup]], const ushort localSize [[threads_per_threadgroup]]) {
	#endif

// Otherwise
#else

	// Check if initial buckets isn't disjointed
	#if INITIAL_BUCKETS_NUMBER_OF_BUCKETS == NUMBER_OF_BUCKETS
	
		// Trim edges step one
		[[kernel]] void trimEdgesStepOne(device uint4 *buckets [[buffer(0)]], device atomic_uint *numberOfEdgesPerBucket [[buffer(1)]], constant const ulong4 &sipHashKeys [[buffer(2)]], constant const uchar &part [[buffer(3)]], const uint globalId [[thread_position_in_grid]], const ushort localId [[thread_position_in_threadgroup]], const ushort localSize [[threads_per_threadgroup]]) {
	
	// Otherwise
	#else
	
		// Trim edges step one
		[[kernel]] void trimEdgesStepOne(device uint4 *buckets [[buffer(0)]], device atomic_uint *numberOfEdgesPerBucket [[buffer(1)]], constant const ulong4 &sipHashKeys [[buffer(2)]], constant const uchar &part [[buffer(3)]], device uint4 *bucketsSecondPart [[buffer(4)]], const uint globalId [[thread_position_in_grid]], const ushort localId [[thread_position_in_threadgroup]], const ushort localSize [[threads_per_threadgroup]]) {
	#endif
#endif

	// Check if local buckets size is one
	#if LOCAL_BUCKETS_SIZE == 1
	
		// Get work item's edge indices in the part
		const uint indices = globalId * NUMBER_OF_EDGES_PER_STEP_ONE_WORK_ITEM + part * static_cast<uint>(NUMBER_OF_EDGES / SLEAN_TRIMMING_PARTS);
		
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
				device uint *bucketNextIndices = &buckets[INITIAL_MAX_NUMBER_OF_EDGES_PER_BUCKET * bucketIndex + nextEdgeIndex];
			
			// Otherwise
			#else
			
				// Get bucket's next indices
				device uint *bucketNextIndices = (bucketIndex < INITIAL_BUCKETS_NUMBER_OF_BUCKETS) ? &buckets[INITIAL_MAX_NUMBER_OF_EDGES_PER_BUCKET * bucketIndex + nextEdgeIndex] : &bucketsSecondPart[INITIAL_MAX_NUMBER_OF_EDGES_PER_BUCKET * (bucketIndex - INITIAL_BUCKETS_NUMBER_OF_BUCKETS) + nextEdgeIndex];
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
		
		// Get work item's edge indices in the part
		const uint indices = globalId * NUMBER_OF_EDGES_PER_STEP_ONE_WORK_ITEM + part * static_cast<uint>(NUMBER_OF_EDGES / SLEAN_TRIMMING_PARTS);
		
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
							device uint2 *bucketNextIndices = &buckets[(INITIAL_MAX_NUMBER_OF_EDGES_PER_BUCKET * bucketIndex + nextEdgeIndex) / 2];
						
						// Otherwise
						#else
						
							// Get bucket's next indices
							device uint2 *bucketNextIndices = (bucketIndex < INITIAL_BUCKETS_NUMBER_OF_BUCKETS) ? &buckets[(INITIAL_MAX_NUMBER_OF_EDGES_PER_BUCKET * bucketIndex + nextEdgeIndex) / 2] : &bucketsSecondPart[(INITIAL_MAX_NUMBER_OF_EDGES_PER_BUCKET * (bucketIndex - INITIAL_BUCKETS_NUMBER_OF_BUCKETS) + nextEdgeIndex) / 2];
						#endif
						
						// Set bucket's next edges to the local bucket's edges
						*bucketNextIndices = uint2(localBuckets[bucketIndex][0], edgeIndex);
						
					// Otherwise
					#else
					
						// Check if initial buckets isn't disjointed
						#if INITIAL_BUCKETS_NUMBER_OF_BUCKETS == NUMBER_OF_BUCKETS
						
							// Get bucket's next indices
							device uint4 *bucketNextIndices = &buckets[(INITIAL_MAX_NUMBER_OF_EDGES_PER_BUCKET * bucketIndex + nextEdgeIndex) / 4];
						
						// Otherwise
						#else
						
							// Get bucket's next indices
							device uint4 *bucketNextIndices = (bucketIndex < INITIAL_BUCKETS_NUMBER_OF_BUCKETS) ? &buckets[(INITIAL_MAX_NUMBER_OF_EDGES_PER_BUCKET * bucketIndex + nextEdgeIndex) / 4] : &bucketsSecondPart[(INITIAL_MAX_NUMBER_OF_EDGES_PER_BUCKET * (bucketIndex - INITIAL_BUCKETS_NUMBER_OF_BUCKETS) + nextEdgeIndex) / 4];
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
						device uint2 *bucketNextIndices = &buckets[(static_cast<uint>(INITIAL_MAX_NUMBER_OF_EDGES_PER_BUCKET) * i + nextEdgeIndex) / 2];
					
					// Otherwise
					#else
					
						// Get bucket's next indices
						device uint2 *bucketNextIndices = (i < INITIAL_BUCKETS_NUMBER_OF_BUCKETS) ? &buckets[(static_cast<uint>(INITIAL_MAX_NUMBER_OF_EDGES_PER_BUCKET) * i + nextEdgeIndex) / 2] : &bucketsSecondPart[(static_cast<uint>(INITIAL_MAX_NUMBER_OF_EDGES_PER_BUCKET) * (i - INITIAL_BUCKETS_NUMBER_OF_BUCKETS) + nextEdgeIndex) / 2];
					#endif
					
					// Set bucket's next edges to the local bucket's edges
					*bucketNextIndices = uint2(localBuckets[i][0]);
					
				// Otherwise
				#else
				
					// Check if initial buckets isn't disjointed
					#if INITIAL_BUCKETS_NUMBER_OF_BUCKETS == NUMBER_OF_BUCKETS
					
						// Get bucket's next indices
						device uint4 *bucketNextIndices = &buckets[(static_cast<uint>(INITIAL_MAX_NUMBER_OF_EDGES_PER_BUCKET) * i + nextEdgeIndex) / 4];
					
					// Otherwise
					#else
					
						// Get bucket's next indices
						device uint4 *bucketNextIndices = (i < INITIAL_BUCKETS_NUMBER_OF_BUCKETS) ? &buckets[(static_cast<uint>(INITIAL_MAX_NUMBER_OF_EDGES_PER_BUCKET) * i + nextEdgeIndex) / 4] : &bucketsSecondPart[(static_cast<uint>(INITIAL_MAX_NUMBER_OF_EDGES_PER_BUCKET) * (i - INITIAL_BUCKETS_NUMBER_OF_BUCKETS) + nextEdgeIndex) / 4];
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
	[[kernel]] void trimEdgesStepTwo(device const uint *buckets [[buffer(0)]], device const uint *numberOfEdgesPerBucket [[buffer(1)]], device uint *nodesBitmap [[buffer(5)]], constant const ulong4 &sipHashKeys [[buffer(2)]], const ushort localId [[thread_position_in_threadgroup]], const ushort localSize [[threads_per_threadgroup]], const uint groupId [[threadgroup_position_in_grid]]) {

// Otherwise
#else

	// Trim edges step two
	[[kernel]] void trimEdgesStepTwo(device const uint *buckets [[buffer(0)]], device const uint *numberOfEdgesPerBucket [[buffer(1)]], device uint *nodesBitmap [[buffer(5)]], constant const ulong4 &sipHashKeys [[buffer(2)]], device const uint *bucketsSecondPart [[buffer(4)]], const ushort localId [[thread_position_in_threadgroup]], const ushort localSize [[threads_per_threadgroup]], const uint groupId [[threadgroup_position_in_grid]]) {
#endif

	// Declare bitmap
	threadgroup atomic_uint bitmap[static_cast<short>(NUMBER_OF_BITMAP_BYTES / sizeof(uint))];
	
	// Go through all groups of bytes in the bitmap as a work group
	for(short i = localId; i < static_cast<short>(NUMBER_OF_BITMAP_BYTES / sizeof(uint)); i += localSize) {
	
		// Set group of bytes in the bitmap to zero
		atomic_store_explicit(&bitmap[i], 0, memory_order_relaxed);
	}
	
	// Get number of edges in this work group's bucket
	const uint numberOfEdges = min(numberOfEdgesPerBucket[groupId], static_cast<uint>(INITIAL_MAX_NUMBER_OF_EDGES_PER_BUCKET));
	
	// Check if initial buckets isn't disjointed
	#if INITIAL_BUCKETS_NUMBER_OF_BUCKETS == NUMBER_OF_BUCKETS
	
		// Get work group's bucket's indices
		device const uint *indices = &buckets[INITIAL_MAX_NUMBER_OF_EDGES_PER_BUCKET * groupId];
	
	// Otherwise
	#else
	
		// Get work group's bucket's indices
		device const uint *indices = (groupId < INITIAL_BUCKETS_NUMBER_OF_BUCKETS) ? &buckets[INITIAL_MAX_NUMBER_OF_EDGES_PER_BUCKET * groupId] : &bucketsSecondPart[INITIAL_MAX_NUMBER_OF_EDGES_PER_BUCKET * (groupId - INITIAL_BUCKETS_NUMBER_OF_BUCKETS)];
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
	
	// Get work group's nodes bitmap's index
	device uint *nodesBitmapIndex = &nodesBitmap[groupId * static_cast<short>(NUMBER_OF_BITMAP_BYTES / sizeof(uint))];
	
	// Synchronize work group
	threadgroup_barrier(mem_flags::mem_threadgroup);
	
	// Go through all groups of bytes in the bitmap as a work group
	for(short i = localId; i < static_cast<short>(NUMBER_OF_BITMAP_BYTES / sizeof(uint)); i += localSize) {
	
		// Set group of bytes in nodes bitmap to the bitmap's
		nodesBitmapIndex[i] = atomic_load_explicit(&bitmap[i], memory_order_relaxed);
	}
}

// Check if initial buckets isn't disjointed
#if INITIAL_BUCKETS_NUMBER_OF_BUCKETS == NUMBER_OF_BUCKETS

	// Trim edges step three
	[[kernel]] void trimEdgesStepThree(device const uint *buckets [[buffer(0)]], device const uint *numberOfEdgesPerBucket [[buffer(1)]], device uint *nodesBitmap [[buffer(5)]], constant const ulong4 &sipHashKeys [[buffer(2)]], const ushort localId [[thread_position_in_threadgroup]], const ushort localSize [[threads_per_threadgroup]], const uint groupId [[threadgroup_position_in_grid]]) {

// Otherwise
#else

	// Trim edges step three
	[[kernel]] void trimEdgesStepThree(device const uint *buckets [[buffer(0)]], device const uint *numberOfEdgesPerBucket [[buffer(1)]], device uint *nodesBitmap [[buffer(5)]], constant const ulong4 &sipHashKeys [[buffer(2)]], device const uint *bucketsSecondPart [[buffer(4)]], const ushort localId [[thread_position_in_threadgroup]], const ushort localSize [[threads_per_threadgroup]], const uint groupId [[threadgroup_position_in_grid]]) {
#endif

	// Declare bitmap
	threadgroup atomic_uint bitmap[static_cast<short>(NUMBER_OF_BITMAP_BYTES / sizeof(uint))];
		
	// Get work group's nodes bitmap's index
	device uint *nodesBitmapIndex = &nodesBitmap[groupId * static_cast<short>(NUMBER_OF_BITMAP_BYTES / sizeof(uint))];
	
	// Go through all groups of bytes in the bitmap as a work group
	for(short i = localId; i < static_cast<short>(NUMBER_OF_BITMAP_BYTES / sizeof(uint)); i += localSize) {
	
		// Set group of bytes in the bitmap to the nodes bitmap's
		atomic_store_explicit(&bitmap[i], nodesBitmapIndex[i], memory_order_relaxed);
	}
	
	// Get number of edges in this work group's bucket
	const uint numberOfEdges = min(numberOfEdgesPerBucket[groupId], static_cast<uint>(INITIAL_MAX_NUMBER_OF_EDGES_PER_BUCKET));
	
	// Check if initial buckets isn't disjointed
	#if INITIAL_BUCKETS_NUMBER_OF_BUCKETS == NUMBER_OF_BUCKETS
	
		// Get work group's bucket's indices
		device const uint *indices = &buckets[INITIAL_MAX_NUMBER_OF_EDGES_PER_BUCKET * groupId];
	
	// Otherwise
	#else
	
		// Get work group's bucket's indices
		device const uint *indices = (groupId < INITIAL_BUCKETS_NUMBER_OF_BUCKETS) ? &buckets[INITIAL_MAX_NUMBER_OF_EDGES_PER_BUCKET * groupId] : &bucketsSecondPart[INITIAL_MAX_NUMBER_OF_EDGES_PER_BUCKET * (groupId - INITIAL_BUCKETS_NUMBER_OF_BUCKETS)];
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
	
	// Go through all groups of bytes in the bitmap as a work group
	for(short i = localId; i < static_cast<short>(NUMBER_OF_BITMAP_BYTES / sizeof(uint)); i += localSize) {
	
		// Set group of bytes in nodes bitmap to the bitmap's
		nodesBitmapIndex[i] = atomic_load_explicit(&bitmap[i], memory_order_relaxed);
	}
}

// Check if initial buckets isn't disjointed
#if INITIAL_BUCKETS_NUMBER_OF_BUCKETS == NUMBER_OF_BUCKETS

	// Trim edges step four
	[[kernel]] void trimEdgesStepFour(device const uint *sourceBuckets [[buffer(0)]], device const uint *numberOfEdgesPerSourceBucket [[buffer(1)]], device uint *nodesBitmap [[buffer(5)]], device uint *destinationBuckets [[buffer(6)]], device atomic_uint *numberOfEdgesPerDestinationBucket [[buffer(7)]], constant const ulong4 &sipHashKeys [[buffer(2)]], const ushort localId [[thread_position_in_threadgroup]], const ushort localSize [[threads_per_threadgroup]], const uint groupId [[threadgroup_position_in_grid]]) {

// Otherwise
#else

	// Trim edges step four
	[[kernel]] void trimEdgesStepFour(device const uint *sourceBuckets [[buffer(0)]], device const uint *numberOfEdgesPerSourceBucket [[buffer(1)]], device uint *nodesBitmap [[buffer(5)]], device uint *destinationBuckets [[buffer(6)]], device atomic_uint *numberOfEdgesPerDestinationBucket [[buffer(7)]], constant const ulong4 &sipHashKeys [[buffer(2)]], device const uint *sourceBucketsSecondPart [[buffer(4)]], const ushort localId [[thread_position_in_threadgroup]], const ushort localSize [[threads_per_threadgroup]], const uint groupId [[threadgroup_position_in_grid]]) {
#endif

	// Declare bitmap
	threadgroup atomic_uint bitmap[static_cast<short>(NUMBER_OF_BITMAP_BYTES / sizeof(uint))];
		
	// Get work group's nodes bitmap's index
	device uint *nodesBitmapIndex = &nodesBitmap[groupId * static_cast<short>(NUMBER_OF_BITMAP_BYTES / sizeof(uint))];
	
	// Go through all groups of bytes in the bitmap as a work group
	for(short i = localId; i < static_cast<short>(NUMBER_OF_BITMAP_BYTES / sizeof(uint)); i += localSize) {
	
		// Set group of bytes in the bitmap to the nodes bitmap's
		atomic_store_explicit(&bitmap[i], nodesBitmapIndex[i], memory_order_relaxed);
	}
	
	// Get number of edges in this work group's bucket
	const uint numberOfEdges = min(numberOfEdgesPerSourceBucket[groupId], static_cast<uint>(INITIAL_MAX_NUMBER_OF_EDGES_PER_BUCKET));
	
	// Check if initial buckets isn't disjointed
	#if INITIAL_BUCKETS_NUMBER_OF_BUCKETS == NUMBER_OF_BUCKETS
	
		// Get work group's bucket's indices
		device const uint *indices = &sourceBuckets[INITIAL_MAX_NUMBER_OF_EDGES_PER_BUCKET * groupId];
	
	// Otherwise
	#else
	
		// Get work group's bucket's indices
		device const uint *indices = (groupId < INITIAL_BUCKETS_NUMBER_OF_BUCKETS) ? &sourceBuckets[INITIAL_MAX_NUMBER_OF_EDGES_PER_BUCKET * groupId] : &sourceBucketsSecondPart[INITIAL_MAX_NUMBER_OF_EDGES_PER_BUCKET * (groupId - INITIAL_BUCKETS_NUMBER_OF_BUCKETS)];
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
	
	// Go through all groups of bytes in the bitmap as a work group
	for(short i = localId; i < static_cast<short>(NUMBER_OF_BITMAP_BYTES / sizeof(uint)); i += localSize) {
	
		// Set group of bytes in nodes bitmap to the bitmap's
		nodesBitmapIndex[i] = atomic_load_explicit(&bitmap[i], memory_order_relaxed);
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
			if(!isBitSetInBitmap(bitmap, (sipHash24(sipHashKeys, static_cast<ulong>(edgeIndex) * 2) & BITMAP_MASK) ^ 1)) {
			
				// Get edge's index's bucket index
				const uint bucketIndex = ((edgeIndex - static_cast<uint>(NUMBER_OF_EDGES / SLEAN_TRIMMING_PARTS) * (SLEAN_TRIMMING_PARTS - 1)) * SLEAN_TRIMMING_PARTS) >> NUMBER_OF_LEAST_SIGNIFICANT_BITS_IGNORED_DURING_REMAINING_EDGES_BUCKET_SORTING;
				
				// Get bucket's next edge index
				const uint nextEdgeIndex = min(atomic_fetch_add_explicit(&numberOfEdgesPerDestinationBucket[bucketIndex], 1, memory_order_relaxed), static_cast<uint>(AFTER_TRIMMING_ROUND_MAX_NUMBER_OF_EDGES_PER_REMAINING_EDGES_BUCKET - 1));
				
				// Get destination bucket's next indices
				device uint *bucketNextIndices = &destinationBuckets[AFTER_TRIMMING_ROUND_MAX_NUMBER_OF_EDGES_PER_REMAINING_EDGES_BUCKET * bucketIndex + nextEdgeIndex];
				
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
[[kernel]] void trimEdgesStepFive(device const uint *buckets [[buffer(6)]], device const uint *numberOfEdgesPerBucket [[buffer(7)]], device uint *edgesBitmap [[buffer(8)]], constant const uchar &part [[buffer(3)]], const ushort localId [[thread_position_in_threadgroup]], const ushort localSize [[threads_per_threadgroup]], const uint groupId [[threadgroup_position_in_grid]]) {

	// Declare bitmap
	threadgroup atomic_uint bitmap[static_cast<short>(NUMBER_OF_REMAINING_EDGES_BITMAP_BYTES / sizeof(uint))];
	
	// Go through all groups of bytes in the bitmap as a work group
	for(short i = localId; i < static_cast<short>(NUMBER_OF_REMAINING_EDGES_BITMAP_BYTES / sizeof(uint)); i += localSize) {
	
		// Set group of bytes in the bitmap to max
		atomic_store_explicit(&bitmap[i], UINT_MAX, memory_order_relaxed);
	}
	
	// Get number of edges in this work group's bucket
	const uint numberOfEdges = min(numberOfEdgesPerBucket[groupId], static_cast<uint>(AFTER_TRIMMING_ROUND_MAX_NUMBER_OF_EDGES_PER_REMAINING_EDGES_BUCKET));
	
	// Get work group's bucket's indices
	device const uint *indices = &buckets[AFTER_TRIMMING_ROUND_MAX_NUMBER_OF_EDGES_PER_REMAINING_EDGES_BUCKET * groupId];
	
	// Synchronize work group
	threadgroup_barrier(mem_flags::mem_threadgroup);
	
	// Go through all edges in this work group's bucket as a work group
	for(uint i = localId; i < numberOfEdges; i += localSize) {
	
		// Disable edge's index in the bitmap
		clearBitInBitmap(bitmap, indices[i] & REMAINING_EDGES_BITMAP_MASK);
	}
	
	// Get work group's edges bitmap's index
	device uint *edgesBitmapIndex = &edgesBitmap[groupId * static_cast<short>(NUMBER_OF_REMAINING_EDGES_BITMAP_BYTES / sizeof(uint)) + part * static_cast<int>((static_cast<uint>(NUMBER_OF_EDGES / BITS_IN_A_BYTE) / SLEAN_TRIMMING_PARTS) / sizeof(uint))];
	
	// Synchronize work group
	threadgroup_barrier(mem_flags::mem_threadgroup);
	
	// Go through all groups of bytes in the bitmap as a work group
	for(short i = localId; i < static_cast<short>(NUMBER_OF_REMAINING_EDGES_BITMAP_BYTES / sizeof(uint)); i += localSize) {
	
		// Set group of bytes in edges bitmap to the bitmap's
		edgesBitmapIndex[i] = atomic_load_explicit(&bitmap[i], memory_order_relaxed);
	}
}

// Check if initial buckets isn't disjointed
#if INITIAL_BUCKETS_NUMBER_OF_BUCKETS == NUMBER_OF_BUCKETS

	// Trim edges step six
	[[kernel]] void trimEdgesStepSix(device const uint *sourceBuckets [[buffer(0)]], device const uint *numberOfEdgesPerSourceBucket [[buffer(1)]], device const uint *nodesBitmap [[buffer(5)]], device uint *destinationBuckets [[buffer(6)]], device atomic_uint *numberOfEdgesPerDestinationBucket [[buffer(7)]], constant const ulong4 &sipHashKeys [[buffer(2)]], constant const uchar &part [[buffer(3)]], const ushort localId [[thread_position_in_threadgroup]], const ushort localSize [[threads_per_threadgroup]], const uint groupId [[threadgroup_position_in_grid]]) {

// Otherwise
#else

	// Trim edges step six
	[[kernel]] void trimEdgesStepSix(device const uint *sourceBuckets [[buffer(0)]], device const uint *numberOfEdgesPerSourceBucket [[buffer(1)]], device const uint *nodesBitmap [[buffer(5)]], device uint *destinationBuckets [[buffer(6)]], device atomic_uint *numberOfEdgesPerDestinationBucket [[buffer(7)]], constant const ulong4 &sipHashKeys [[buffer(2)]], constant const uchar &part [[buffer(3)]], device const uint *sourceBucketsSecondPart [[buffer(4)]], const ushort localId [[thread_position_in_threadgroup]], const ushort localSize [[threads_per_threadgroup]], const uint groupId [[threadgroup_position_in_grid]]) {
#endif

	// Declare bitmap
	threadgroup uint bitmap[static_cast<short>(NUMBER_OF_BITMAP_BYTES / sizeof(uint))];
		
	// Get work group's nodes bitmap's index
	device const uint *nodesBitmapIndex = &nodesBitmap[groupId * static_cast<short>(NUMBER_OF_BITMAP_BYTES / sizeof(uint))];
	
	// Go through all groups of bytes in the bitmap as a work group
	for(short i = localId; i < static_cast<short>(NUMBER_OF_BITMAP_BYTES / sizeof(uint)); i += localSize) {
	
		// Set group of bytes in the bitmap to the nodes bitmap's
		bitmap[i] = nodesBitmapIndex[i];
	}
	
	// Get number of edges in this work group's bucket
	const uint numberOfEdges = min(numberOfEdgesPerSourceBucket[groupId], static_cast<uint>(INITIAL_MAX_NUMBER_OF_EDGES_PER_BUCKET));
	
	// Check if initial buckets isn't disjointed
	#if INITIAL_BUCKETS_NUMBER_OF_BUCKETS == NUMBER_OF_BUCKETS
	
		// Get work group's bucket's indices
		device const uint *indices = &sourceBuckets[INITIAL_MAX_NUMBER_OF_EDGES_PER_BUCKET * groupId];
	
	// Otherwise
	#else
	
		// Get work group's bucket's indices
		device const uint *indices = (groupId < INITIAL_BUCKETS_NUMBER_OF_BUCKETS) ? &sourceBuckets[INITIAL_MAX_NUMBER_OF_EDGES_PER_BUCKET * groupId] : &sourceBucketsSecondPart[INITIAL_MAX_NUMBER_OF_EDGES_PER_BUCKET * (groupId - INITIAL_BUCKETS_NUMBER_OF_BUCKETS)];
	#endif
	
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
		
			// Check if edge's node doesn't have a pair in the bitmap
			if(!isBitSetInBitmap(bitmap, (sipHash24(sipHashKeys, static_cast<ulong>(edgeIndex) * 2) & BITMAP_MASK) ^ 1)) {
			
				// Get edge's index's bucket index
				const uint bucketIndex = ((edgeIndex - static_cast<uint>(NUMBER_OF_EDGES / SLEAN_TRIMMING_PARTS) * part) * SLEAN_TRIMMING_PARTS) >> NUMBER_OF_LEAST_SIGNIFICANT_BITS_IGNORED_DURING_REMAINING_EDGES_BUCKET_SORTING;
				
				// Get bucket's next edge index
				const uint nextEdgeIndex = min(atomic_fetch_add_explicit(&numberOfEdgesPerDestinationBucket[bucketIndex], 1, memory_order_relaxed), static_cast<uint>(AFTER_TRIMMING_ROUND_MAX_NUMBER_OF_EDGES_PER_REMAINING_EDGES_BUCKET - 1));
				
				// Get destination bucket's next indices
				device uint *bucketNextIndices = &destinationBuckets[AFTER_TRIMMING_ROUND_MAX_NUMBER_OF_EDGES_PER_REMAINING_EDGES_BUCKET * bucketIndex + nextEdgeIndex];
				
				// Set destination bucket's next edge to the edge
				*bucketNextIndices = edgeIndex;
			}
			
		// Check if local buckets size isn't one
		#if LOCAL_BUCKETS_SIZE != 1
		
			}
		#endif
	}
}

// Check if local buckets size is one
#if LOCAL_BUCKETS_SIZE == 1

	// Trim edges step seven
	[[kernel]] void trimEdgesStepSeven(device const ulong *edgesBitmap [[buffer(8)]], device uint *buckets [[buffer(0)]], device atomic_uint *numberOfEdgesPerBucket [[buffer(1)]], constant const ulong4 &sipHashKeys [[buffer(2)]], constant const uchar &part [[buffer(3)]], const uint globalId [[thread_position_in_grid]]) {

// Otherwise check if local buckets size is two
#elif LOCAL_BUCKETS_SIZE == 2

	// Trim edges step seven
	[[kernel]] void trimEdgesStepSeven(device const ulong *edgesBitmap [[buffer(8)]], device uint2 *buckets [[buffer(0)]], device atomic_uint *numberOfEdgesPerBucket [[buffer(1)]], constant const ulong4 &sipHashKeys [[buffer(2)]], constant const uchar &part [[buffer(3)]], const uint globalId [[thread_position_in_grid]], const ushort localId [[thread_position_in_threadgroup]], const ushort localSize [[threads_per_threadgroup]]) {

// Otherwise
#else

	// Trim edges step seven
	[[kernel]] void trimEdgesStepSeven(device const ulong *edgesBitmap [[buffer(8)]], device uint4 *buckets [[buffer(0)]], device atomic_uint *numberOfEdgesPerBucket [[buffer(1)]], constant const ulong4 &sipHashKeys [[buffer(2)]], constant const uchar &part [[buffer(3)]], const uint globalId [[thread_position_in_grid]], const ushort localId [[thread_position_in_threadgroup]], const ushort localSize [[threads_per_threadgroup]]) {
#endif

	// Check if local buckets size is one
	#if LOCAL_BUCKETS_SIZE == 1
	
		// Get work item's edge indices in the part
		const uint indices = globalId * static_cast<char>(sizeof(ulong) * BITS_IN_A_BYTE) + part * static_cast<uint>(NUMBER_OF_EDGES / SLEAN_TRIMMING_PARTS);
		
		// Get this work item's edges in the part
		ulong edges = edgesBitmap[globalId + part * static_cast<int>((static_cast<uint>(NUMBER_OF_EDGES / BITS_IN_A_BYTE) / SLEAN_TRIMMING_PARTS) / sizeof(ulong))];
		
		// Go through all of this work item's enabled edges
		for(char currentBitIndex = clz(edges) + 1, previousBitIndex = 0; edges && previousBitIndex != static_cast<char>(sizeof(ulong) * BITS_IN_A_BYTE); edges <<= min(currentBitIndex, static_cast<char>(sizeof(ulong) * BITS_IN_A_BYTE - 1)), previousBitIndex += currentBitIndex, currentBitIndex = clz(edges) + 1) {
		
			// Get edge's index
			const uint edgeIndex = indices + (static_cast<char>(sizeof(ulong) * BITS_IN_A_BYTE) - currentBitIndex - previousBitIndex);
			
			// Get edge's node's bucket index
			const uint bucketIndex = sipHash24(sipHashKeys, (static_cast<ulong>(edgeIndex) * 2) | 1) >> NUMBER_OF_LEAST_SIGNIFICANT_BITS_IGNORED_DURING_BUCKET_SORTING;
			
			// Get bucket's next edge index
			const uint nextEdgeIndex = min(atomic_fetch_add_explicit(&numberOfEdgesPerBucket[bucketIndex], 1, memory_order_relaxed), static_cast<uint>(AFTER_TRIMMING_ROUND_MAX_NUMBER_OF_EDGES_PER_BUCKET - 1));
			
			// Get bucket's next indices
			device uint *bucketNextIndices = &buckets[AFTER_TRIMMING_ROUND_MAX_NUMBER_OF_EDGES_PER_BUCKET * bucketIndex + nextEdgeIndex];
			
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
		
		// Get work item's edge indices in the part
		const uint indices = globalId * static_cast<char>(sizeof(ulong) * BITS_IN_A_BYTE) + part * static_cast<uint>(NUMBER_OF_EDGES / SLEAN_TRIMMING_PARTS);
		
		// Get this work item's edges in the part
		ulong edges = edgesBitmap[globalId + part * static_cast<int>((static_cast<uint>(NUMBER_OF_EDGES / BITS_IN_A_BYTE) / SLEAN_TRIMMING_PARTS) / sizeof(ulong))];
		
		// Go through all of this work item's edges
		for(char i = 0; i < static_cast<char>(sizeof(ulong) * BITS_IN_A_BYTE); ++i, edges >>= 1) {
		
			// Get edge's index
			const uint edgeIndex = indices + i;
			
			// Get edge's node's bucket index
			const uint bucketIndex = sipHash24(sipHashKeys, (static_cast<ulong>(edgeIndex) * 2) | 1) >> NUMBER_OF_LEAST_SIGNIFICANT_BITS_IGNORED_DURING_BUCKET_SORTING;
			
			// Synchronize work group
			threadgroup_barrier(mem_flags::mem_threadgroup);
			
			// Increment local bucket's number of edges if the edge is enabled
			uchar numberOfEdges = (edges & 1) ? atomic_fetch_add_explicit(&numberOfEdgesPerLocalBucket[bucketIndex / static_cast<char>(sizeof(uint))], 1 << ((bucketIndex % static_cast<char>(sizeof(uint))) * BITS_IN_A_BYTE), memory_order_relaxed) >> ((bucketIndex % static_cast<char>(sizeof(uint))) * BITS_IN_A_BYTE) : UCHAR_MAX;
			
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
					const uint nextEdgeIndex = min(atomic_fetch_add_explicit(&numberOfEdgesPerBucket[bucketIndex], LOCAL_BUCKETS_SIZE, memory_order_relaxed), static_cast<uint>(AFTER_TRIMMING_ROUND_MAX_NUMBER_OF_EDGES_PER_BUCKET - LOCAL_BUCKETS_SIZE));
					
					// Check if local buckets size is two
					#if LOCAL_BUCKETS_SIZE == 2
					
						// Get bucket's next indices
						device uint2 *bucketNextIndices = &buckets[(AFTER_TRIMMING_ROUND_MAX_NUMBER_OF_EDGES_PER_BUCKET * bucketIndex + nextEdgeIndex) / 2];
						
						// Set bucket's next edges to the local bucket's edges
						*bucketNextIndices = uint2(localBuckets[bucketIndex][0], edgeIndex);
						
					// Otherwise
					#else
					
						// Get bucket's next indices
						device uint4 *bucketNextIndices = &buckets[(AFTER_TRIMMING_ROUND_MAX_NUMBER_OF_EDGES_PER_BUCKET * bucketIndex + nextEdgeIndex) / 4];
						
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
				const uint nextEdgeIndex = min(atomic_fetch_add_explicit(&numberOfEdgesPerBucket[i], LOCAL_BUCKETS_SIZE, memory_order_relaxed), static_cast<uint>(AFTER_TRIMMING_ROUND_MAX_NUMBER_OF_EDGES_PER_BUCKET - LOCAL_BUCKETS_SIZE));
				
				// Check if local buckets size is two
				#if LOCAL_BUCKETS_SIZE == 2
				
					// Get bucket's next indices
					device uint2 *bucketNextIndices = &buckets[(AFTER_TRIMMING_ROUND_MAX_NUMBER_OF_EDGES_PER_BUCKET * i + nextEdgeIndex) / 2];
					
					// Set bucket's next edges to the local bucket's edges
					*bucketNextIndices = uint2(localBuckets[i][0]);
					
				// Otherwise
				#else
				
					// Get bucket's next indices
					device uint4 *bucketNextIndices = &buckets[(AFTER_TRIMMING_ROUND_MAX_NUMBER_OF_EDGES_PER_BUCKET * i + nextEdgeIndex) / 4];
					
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

// Trim edges step eight
[[kernel]] void trimEdgesStepEight(device const uint *buckets [[buffer(0)]], device const uint *numberOfEdgesPerBucket [[buffer(1)]], device uint *nodesBitmap [[buffer(5)]], constant const ulong4 &sipHashKeys [[buffer(2)]], const ushort localId [[thread_position_in_threadgroup]], const ushort localSize [[threads_per_threadgroup]], const uint groupId [[threadgroup_position_in_grid]]) {

	// Declare bitmap
	threadgroup atomic_uint bitmap[static_cast<short>(NUMBER_OF_BITMAP_BYTES / sizeof(uint))];
	
	// Go through all groups of bytes in the bitmap as a work group
	for(short i = localId; i < static_cast<short>(NUMBER_OF_BITMAP_BYTES / sizeof(uint)); i += localSize) {
	
		// Set group of bytes in the bitmap to zero
		atomic_store_explicit(&bitmap[i], 0, memory_order_relaxed);
	}
	
	// Get number of edges in this work group's bucket
	const uint numberOfEdges = min(numberOfEdgesPerBucket[groupId], static_cast<uint>(AFTER_TRIMMING_ROUND_MAX_NUMBER_OF_EDGES_PER_BUCKET));
	
	// Get work group's bucket's indices
	device const uint *indices = &buckets[AFTER_TRIMMING_ROUND_MAX_NUMBER_OF_EDGES_PER_BUCKET * groupId];
	
	// Synchronize work group
	threadgroup_barrier(mem_flags::mem_threadgroup);
	
	// Go through all edges in this work group's bucket as a work group
	for(uint i = localId; i < numberOfEdges; i += localSize) {
	
		// Get edge's index
		const uint edgeIndex = indices[i];
		
		// Enable edge's node in the bitmap
		setBitInBitmap(bitmap, sipHash24(sipHashKeys, (static_cast<ulong>(edgeIndex) * 2) | 1) & BITMAP_MASK);
	}
	
	// Get work group's nodes bitmap's index
	device uint *nodesBitmapIndex = &nodesBitmap[groupId * static_cast<short>(NUMBER_OF_BITMAP_BYTES / sizeof(uint))];
	
	// Synchronize work group
	threadgroup_barrier(mem_flags::mem_threadgroup);
	
	// Go through all groups of bytes in the bitmap as a work group
	for(short i = localId; i < static_cast<short>(NUMBER_OF_BITMAP_BYTES / sizeof(uint)); i += localSize) {
	
		// Set group of bytes in nodes bitmap to the bitmap's
		nodesBitmapIndex[i] = atomic_load_explicit(&bitmap[i], memory_order_relaxed);
	}
}

// Trim edges step nine
[[kernel]] void trimEdgesStepNine(device const uint *buckets [[buffer(0)]], device const uint *numberOfEdgesPerBucket [[buffer(1)]], device uint *nodesBitmap [[buffer(5)]], constant const ulong4 &sipHashKeys [[buffer(2)]], const ushort localId [[thread_position_in_threadgroup]], const ushort localSize [[threads_per_threadgroup]], const uint groupId [[threadgroup_position_in_grid]]) {

	// Declare bitmap
	threadgroup atomic_uint bitmap[static_cast<short>(NUMBER_OF_BITMAP_BYTES / sizeof(uint))];
		
	// Get work group's nodes bitmap's index
	device uint *nodesBitmapIndex = &nodesBitmap[groupId * static_cast<short>(NUMBER_OF_BITMAP_BYTES / sizeof(uint))];
	
	// Go through all groups of bytes in the bitmap as a work group
	for(short i = localId; i < static_cast<short>(NUMBER_OF_BITMAP_BYTES / sizeof(uint)); i += localSize) {
	
		// Set group of bytes in the bitmap to the nodes bitmap's
		atomic_store_explicit(&bitmap[i], nodesBitmapIndex[i], memory_order_relaxed);
	}
	
	// Get number of edges in this work group's bucket
	const uint numberOfEdges = min(numberOfEdgesPerBucket[groupId], static_cast<uint>(AFTER_TRIMMING_ROUND_MAX_NUMBER_OF_EDGES_PER_BUCKET));
	
	// Get work group's bucket's indices
	device const uint *indices = &buckets[AFTER_TRIMMING_ROUND_MAX_NUMBER_OF_EDGES_PER_BUCKET * groupId];
	
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
	
	// Go through all groups of bytes in the bitmap as a work group
	for(short i = localId; i < static_cast<short>(NUMBER_OF_BITMAP_BYTES / sizeof(uint)); i += localSize) {
	
		// Set group of bytes in nodes bitmap to the bitmap's
		nodesBitmapIndex[i] = atomic_load_explicit(&bitmap[i], memory_order_relaxed);
	}
}

// Trim edges step ten
[[kernel]] void trimEdgesStepTen(device const uint *sourceBuckets [[buffer(0)]], device const uint *numberOfEdgesPerSourceBucket [[buffer(1)]], device uint *nodesBitmap [[buffer(5)]], device uint *destinationBuckets [[buffer(6)]], device atomic_uint *numberOfEdgesPerDestinationBucket [[buffer(7)]], constant const ulong4 &sipHashKeys [[buffer(2)]], const ushort localId [[thread_position_in_threadgroup]], const ushort localSize [[threads_per_threadgroup]], const uint groupId [[threadgroup_position_in_grid]]) {

	// Declare bitmap
	threadgroup atomic_uint bitmap[static_cast<short>(NUMBER_OF_BITMAP_BYTES / sizeof(uint))];
		
	// Get work group's nodes bitmap's index
	device uint *nodesBitmapIndex = &nodesBitmap[groupId * static_cast<short>(NUMBER_OF_BITMAP_BYTES / sizeof(uint))];
	
	// Go through all groups of bytes in the bitmap as a work group
	for(short i = localId; i < static_cast<short>(NUMBER_OF_BITMAP_BYTES / sizeof(uint)); i += localSize) {
	
		// Set group of bytes in the bitmap to the nodes bitmap's
		atomic_store_explicit(&bitmap[i], nodesBitmapIndex[i], memory_order_relaxed);
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
	
	// Go through all groups of bytes in the bitmap as a work group
	for(short i = localId; i < static_cast<short>(NUMBER_OF_BITMAP_BYTES / sizeof(uint)); i += localSize) {
	
		// Set group of bytes in nodes bitmap to the bitmap's
		nodesBitmapIndex[i] = atomic_load_explicit(&bitmap[i], memory_order_relaxed);
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
		
			// Check if edge's node has a pair in the bitmap
			if(isBitSetInBitmap(bitmap, (sipHash24(sipHashKeys, (static_cast<ulong>(edgeIndex) * 2) | 1) & BITMAP_MASK) ^ 1)) {
			
				// Get edge's index's bucket index
				const uint bucketIndex = ((edgeIndex - static_cast<uint>(NUMBER_OF_EDGES / SLEAN_TRIMMING_PARTS) * (SLEAN_TRIMMING_PARTS - 1)) * SLEAN_TRIMMING_PARTS) >> NUMBER_OF_LEAST_SIGNIFICANT_BITS_IGNORED_DURING_REMAINING_EDGES_BUCKET_SORTING;
				
				// Get bucket's next edge index
				const uint nextEdgeIndex = min(atomic_fetch_add_explicit(&numberOfEdgesPerDestinationBucket[bucketIndex], 1, memory_order_relaxed), static_cast<uint>(AFTER_TRIMMING_ROUND_MAX_NUMBER_OF_EDGES_PER_REMAINING_EDGES_BUCKET - 1));
				
				// Get destination bucket's next indices
				device uint *bucketNextIndices = &destinationBuckets[AFTER_TRIMMING_ROUND_MAX_NUMBER_OF_EDGES_PER_REMAINING_EDGES_BUCKET * bucketIndex + nextEdgeIndex];
				
				// Set destination bucket's next edge to the edge
				*bucketNextIndices = edgeIndex;
			}
			
		// Check if local buckets size isn't one
		#if LOCAL_BUCKETS_SIZE != 1
		
			}
		#endif
	}
}

// Trim edges step eleven
[[kernel]] void trimEdgesStepEleven(device const uint *buckets [[buffer(6)]], device const uint *numberOfEdgesPerBucket [[buffer(7)]], device uint *edgesBitmap [[buffer(8)]], constant const uchar &part [[buffer(3)]], const ushort localId [[thread_position_in_threadgroup]], const ushort localSize [[threads_per_threadgroup]], const uint groupId [[threadgroup_position_in_grid]]) {

	// Declare bitmap
	threadgroup atomic_uint bitmap[static_cast<short>(NUMBER_OF_REMAINING_EDGES_BITMAP_BYTES / sizeof(uint))];
	
	// Go through all groups of bytes in the bitmap as a work group
	for(short i = localId; i < static_cast<short>(NUMBER_OF_REMAINING_EDGES_BITMAP_BYTES / sizeof(uint)); i += localSize) {
	
		// Set group of bytes in the bitmap to zero
		atomic_store_explicit(&bitmap[i], 0, memory_order_relaxed);
	}
	
	// Get number of edges in this work group's bucket
	const uint numberOfEdges = min(numberOfEdgesPerBucket[groupId], static_cast<uint>(AFTER_TRIMMING_ROUND_MAX_NUMBER_OF_EDGES_PER_REMAINING_EDGES_BUCKET));
	
	// Get work group's bucket's indices
	device const uint *indices = &buckets[AFTER_TRIMMING_ROUND_MAX_NUMBER_OF_EDGES_PER_REMAINING_EDGES_BUCKET * groupId];
	
	// Synchronize work group
	threadgroup_barrier(mem_flags::mem_threadgroup);
	
	// Go through all edges in this work group's bucket as a work group
	for(uint i = localId; i < numberOfEdges; i += localSize) {
	
		// Enable edge's index in the bitmap
		setBitInBitmap(bitmap, indices[i] & REMAINING_EDGES_BITMAP_MASK);
	}
	
	// Get work group's edges bitmap's index
	device uint *edgesBitmapIndex = &edgesBitmap[groupId * static_cast<short>(NUMBER_OF_REMAINING_EDGES_BITMAP_BYTES / sizeof(uint)) + part * static_cast<int>((static_cast<uint>(NUMBER_OF_EDGES / BITS_IN_A_BYTE) / SLEAN_TRIMMING_PARTS) / sizeof(uint))];
	
	// Synchronize work group
	threadgroup_barrier(mem_flags::mem_threadgroup);
	
	// Go through all groups of bytes in the bitmap as a work group
	for(short i = localId; i < static_cast<short>(NUMBER_OF_REMAINING_EDGES_BITMAP_BYTES / sizeof(uint)); i += localSize) {
	
		// Set group of bytes in edges bitmap to the bitmap's
		edgesBitmapIndex[i] = atomic_load_explicit(&bitmap[i], memory_order_relaxed);
	}
}

// Trim edges step twelve
[[kernel]] void trimEdgesStepTwelve(device const uint *sourceBuckets [[buffer(0)]], device const uint *numberOfEdgesPerSourceBucket [[buffer(1)]], device const uint *nodesBitmap [[buffer(5)]], device uint *destinationBuckets [[buffer(6)]], device atomic_uint *numberOfEdgesPerDestinationBucket [[buffer(7)]], constant const ulong4 &sipHashKeys [[buffer(2)]], constant const uchar &part [[buffer(3)]], const ushort localId [[thread_position_in_threadgroup]], const ushort localSize [[threads_per_threadgroup]], const uint groupId [[threadgroup_position_in_grid]]) {

	// Declare bitmap
	threadgroup uint bitmap[static_cast<short>(NUMBER_OF_BITMAP_BYTES / sizeof(uint))];
		
	// Get work group's nodes bitmap's index
	device const uint *nodesBitmapIndex = &nodesBitmap[groupId * static_cast<short>(NUMBER_OF_BITMAP_BYTES / sizeof(uint))];
	
	// Go through all groups of bytes in the bitmap as a work group
	for(short i = localId; i < static_cast<short>(NUMBER_OF_BITMAP_BYTES / sizeof(uint)); i += localSize) {
	
		// Set group of bytes in the bitmap to the nodes bitmap's
		bitmap[i] = nodesBitmapIndex[i];
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
		
		// Check if local buckets size isn't one
		#if LOCAL_BUCKETS_SIZE != 1
		
			// Check if edge isn't a duplicate
			if(edgeIndex != indices[min(i - 1, numberOfEdges - 1)]) {
		#endif
		
			// Check if edge's node has a pair in the bitmap
			if(isBitSetInBitmap(bitmap, (sipHash24(sipHashKeys, (static_cast<ulong>(edgeIndex) * 2) | 1) & BITMAP_MASK) ^ 1)) {
			
				// Get edge's index's bucket index
				const uint bucketIndex = ((edgeIndex - static_cast<uint>(NUMBER_OF_EDGES / SLEAN_TRIMMING_PARTS) * part) * SLEAN_TRIMMING_PARTS) >> NUMBER_OF_LEAST_SIGNIFICANT_BITS_IGNORED_DURING_REMAINING_EDGES_BUCKET_SORTING;
				
				// Get bucket's next edge index
				const uint nextEdgeIndex = min(atomic_fetch_add_explicit(&numberOfEdgesPerDestinationBucket[bucketIndex], 1, memory_order_relaxed), static_cast<uint>(AFTER_TRIMMING_ROUND_MAX_NUMBER_OF_EDGES_PER_REMAINING_EDGES_BUCKET - 1));
				
				// Get destination bucket's next indices
				device uint *bucketNextIndices = &destinationBuckets[AFTER_TRIMMING_ROUND_MAX_NUMBER_OF_EDGES_PER_REMAINING_EDGES_BUCKET * bucketIndex + nextEdgeIndex];
				
				// Set destination bucket's next edge to the edge
				*bucketNextIndices = edgeIndex;
			}
			
		// Check if local buckets size isn't one
		#if LOCAL_BUCKETS_SIZE != 1
		
			}
		#endif
	}
}

// Check is slean trimming parts is two
#if SLEAN_TRIMMING_PARTS == 2

	// Trim edges step thirteen
	[[kernel]] void trimEdgesStepThirteen(device const ulong *edgesBitmap [[buffer(8)]], device uint *buckets [[buffer(0)]], device atomic_uint *numberOfEdgesPerBucket [[buffer(1)]], constant const ulong4 &sipHashKeys [[buffer(2)]], const uint globalId [[thread_position_in_grid]]) {

// Otherwise
#else

	// Trim edges step thirteen
	[[kernel]] void trimEdgesStepThirteen(device const ulong *edgesBitmap [[buffer(8)]], device uint *buckets [[buffer(0)]], device atomic_uint *numberOfEdgesPerBucket [[buffer(1)]], constant const ulong4 &sipHashKeys [[buffer(2)]], constant const uchar &part [[buffer(3)]], const uint globalId [[thread_position_in_grid]]) {
#endif

	// Check is slean trimming parts is two
	#if SLEAN_TRIMMING_PARTS == 2
	
		// Get work item's edge indices in the part
		const uint indices = globalId * static_cast<char>(sizeof(ulong) * BITS_IN_A_BYTE);
		
		// Get this work item's edges in the part
		ulong edges = edgesBitmap[globalId];
		
	// Otherwise
	#else
	
		// Get work item's edge indices in the part
		const uint indices = globalId * static_cast<char>(sizeof(ulong) * BITS_IN_A_BYTE) + part * static_cast<uint>(NUMBER_OF_EDGES / (SLEAN_TRIMMING_PARTS / 2));
		
		// Get this work item's edges in the part
		ulong edges = edgesBitmap[globalId + part * static_cast<int>((static_cast<uint>(NUMBER_OF_EDGES / BITS_IN_A_BYTE) / (SLEAN_TRIMMING_PARTS / 2)) / sizeof(ulong))];
	#endif
	
	// Go through all of this work item's enabled edges
	for(char currentBitIndex = clz(edges) + 1, previousBitIndex = 0; edges && previousBitIndex != static_cast<char>(sizeof(ulong) * BITS_IN_A_BYTE); edges <<= min(currentBitIndex, static_cast<char>(sizeof(ulong) * BITS_IN_A_BYTE - 1)), previousBitIndex += currentBitIndex, currentBitIndex = clz(edges) + 1) {
	
		// Get edge's index
		const uint edgeIndex = indices + (static_cast<char>(sizeof(ulong) * BITS_IN_A_BYTE) - currentBitIndex - previousBitIndex);
		
		// Get edge's node's bucket index
		const uint bucketIndex = sipHash24(sipHashKeys, static_cast<ulong>(edgeIndex) * 2) >> NUMBER_OF_LEAST_SIGNIFICANT_BITS_IGNORED_DURING_BUCKET_SORTING;
		
		// Get bucket's next edge index
		const uint nextEdgeIndex = min(atomic_fetch_add_explicit(&numberOfEdgesPerBucket[bucketIndex], 1, memory_order_relaxed), static_cast<uint>(AFTER_TRIMMING_ROUND_MAX_NUMBER_OF_EDGES_PER_BUCKET - 1));
		
		// Get bucket's next indices
		device uint *bucketNextIndices = &buckets[AFTER_TRIMMING_ROUND_MAX_NUMBER_OF_EDGES_PER_BUCKET * bucketIndex + nextEdgeIndex];
		
		// Set bucket's next edge to the edge
		*bucketNextIndices = edgeIndex;
	}
}

// Trim edges step fourteen
[[kernel]] void trimEdgesStepFourteen(device const uint *buckets [[buffer(0)]], device const uint *numberOfEdgesPerBucket [[buffer(1)]], device uint *nodesBitmap [[buffer(5)]], constant const ulong4 &sipHashKeys [[buffer(2)]], const ushort localId [[thread_position_in_threadgroup]], const ushort localSize [[threads_per_threadgroup]], const uint groupId [[threadgroup_position_in_grid]]) {

	// Declare bitmap
	threadgroup atomic_uint bitmap[static_cast<short>(NUMBER_OF_BITMAP_BYTES / sizeof(uint))];
	
	// Go through all groups of bytes in the bitmap as a work group
	for(short i = localId; i < static_cast<short>(NUMBER_OF_BITMAP_BYTES / sizeof(uint)); i += localSize) {
	
		// Set group of bytes in the bitmap to zero
		atomic_store_explicit(&bitmap[i], 0, memory_order_relaxed);
	}
	
	// Get number of edges in this work group's bucket
	const uint numberOfEdges = min(numberOfEdgesPerBucket[groupId], static_cast<uint>(AFTER_TRIMMING_ROUND_MAX_NUMBER_OF_EDGES_PER_BUCKET));
	
	// Get work group's bucket's indices
	device const uint *indices = &buckets[AFTER_TRIMMING_ROUND_MAX_NUMBER_OF_EDGES_PER_BUCKET * groupId];
	
	// Synchronize work group
	threadgroup_barrier(mem_flags::mem_threadgroup);
	
	// Go through all edges in this work group's bucket as a work group
	for(uint i = localId; i < numberOfEdges; i += localSize) {
	
		// Get edge's index
		const uint edgeIndex = indices[i];
		
		// Enable edge's node in the bitmap
		setBitInBitmap(bitmap, sipHash24(sipHashKeys, static_cast<ulong>(edgeIndex) * 2) & BITMAP_MASK);
	}
	
	// Get work group's nodes bitmap's index
	device uint *nodesBitmapIndex = &nodesBitmap[groupId * static_cast<short>(NUMBER_OF_BITMAP_BYTES / sizeof(uint))];
	
	// Synchronize work group
	threadgroup_barrier(mem_flags::mem_threadgroup);
	
	// Go through all groups of bytes in the bitmap as a work group
	for(short i = localId; i < static_cast<short>(NUMBER_OF_BITMAP_BYTES / sizeof(uint)); i += localSize) {
	
		// Set group of bytes in nodes bitmap to the bitmap's
		nodesBitmapIndex[i] = atomic_load_explicit(&bitmap[i], memory_order_relaxed);
	}
}

// Trim edges step fifteen
[[kernel]] void trimEdgesStepFifteen(device const uint *buckets [[buffer(0)]], device const uint *numberOfEdgesPerBucket [[buffer(1)]], device uint *nodesBitmap [[buffer(5)]], constant const ulong4 &sipHashKeys [[buffer(2)]], const ushort localId [[thread_position_in_threadgroup]], const ushort localSize [[threads_per_threadgroup]], const uint groupId [[threadgroup_position_in_grid]]) {

	// Declare bitmap
	threadgroup atomic_uint bitmap[static_cast<short>(NUMBER_OF_BITMAP_BYTES / sizeof(uint))];
		
	// Get work group's nodes bitmap's index
	device uint *nodesBitmapIndex = &nodesBitmap[groupId * static_cast<short>(NUMBER_OF_BITMAP_BYTES / sizeof(uint))];
	
	// Go through all groups of bytes in the bitmap as a work group
	for(short i = localId; i < static_cast<short>(NUMBER_OF_BITMAP_BYTES / sizeof(uint)); i += localSize) {
	
		// Set group of bytes in the bitmap to the nodes bitmap's
		atomic_store_explicit(&bitmap[i], nodesBitmapIndex[i], memory_order_relaxed);
	}
	
	// Get number of edges in this work group's bucket
	const uint numberOfEdges = min(numberOfEdgesPerBucket[groupId], static_cast<uint>(AFTER_TRIMMING_ROUND_MAX_NUMBER_OF_EDGES_PER_BUCKET));
	
	// Get work group's bucket's indices
	device const uint *indices = &buckets[AFTER_TRIMMING_ROUND_MAX_NUMBER_OF_EDGES_PER_BUCKET * groupId];
	
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
	
	// Go through all groups of bytes in the bitmap as a work group
	for(short i = localId; i < static_cast<short>(NUMBER_OF_BITMAP_BYTES / sizeof(uint)); i += localSize) {
	
		// Set group of bytes in nodes bitmap to the bitmap's
		nodesBitmapIndex[i] = atomic_load_explicit(&bitmap[i], memory_order_relaxed);
	}
}

// Check is slean trimming parts is two
#if SLEAN_TRIMMING_PARTS == 2

	// Trim edges step sixteen
	[[kernel]] void trimEdgesStepSixteen(device const uint *sourceBuckets [[buffer(0)]], device const uint *numberOfEdgesPerSourceBucket [[buffer(1)]], device uint *destinationBuckets [[buffer(6)]], device atomic_uint *numberOfEdgesPerDestinationBucket [[buffer(7)]], constant const ulong4 &sipHashKeys [[buffer(2)]], const ushort localId [[thread_position_in_threadgroup]], const ushort localSize [[threads_per_threadgroup]], const uint groupId [[threadgroup_position_in_grid]]) {

// Otherwise
#else

	// Trim edges step sixteen
	[[kernel]] void trimEdgesStepSixteen(device const uint *sourceBuckets [[buffer(0)]], device const uint *numberOfEdgesPerSourceBucket [[buffer(1)]], device uint *destinationBuckets [[buffer(6)]], device atomic_uint *numberOfEdgesPerDestinationBucket [[buffer(7)]], constant const ulong4 &sipHashKeys [[buffer(2)]], device uint *nodesBitmap [[buffer(5)]], const ushort localId [[thread_position_in_threadgroup]], const ushort localSize [[threads_per_threadgroup]], const uint groupId [[threadgroup_position_in_grid]]) {
#endif

	// Declare bitmap
	threadgroup atomic_uint bitmap[static_cast<short>(NUMBER_OF_BITMAP_BYTES / sizeof(uint))];
		
	// Check is slean trimming parts isn't two
	#if SLEAN_TRIMMING_PARTS != 2
	
		// Get work group's nodes bitmap's index
		device uint *nodesBitmapIndex = &nodesBitmap[groupId * static_cast<short>(NUMBER_OF_BITMAP_BYTES / sizeof(uint))];
		
		// Go through all groups of bytes in the bitmap as a work group
		for(short i = localId; i < static_cast<short>(NUMBER_OF_BITMAP_BYTES / sizeof(uint)); i += localSize) {
		
			// Set group of bytes in the bitmap to the nodes bitmap's
			atomic_store_explicit(&bitmap[i], nodesBitmapIndex[i], memory_order_relaxed);
		}
		
	// Otherwise
	#else
	
		// Go through all groups of bytes in the bitmap as a work group
		for(short i = localId; i < static_cast<short>(NUMBER_OF_BITMAP_BYTES / sizeof(uint)); i += localSize) {
		
			// Set group of bytes in the bitmap to zero
			atomic_store_explicit(&bitmap[i], 0, memory_order_relaxed);
		}
	#endif
	
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
		setBitInBitmap(bitmap, sipHash24(sipHashKeys, static_cast<ulong>(edgeIndex) * 2) & BITMAP_MASK);
	}
	
	// Synchronize work group
	threadgroup_barrier(mem_flags::mem_threadgroup);
	
	// Check is slean trimming parts isn't two
	#if SLEAN_TRIMMING_PARTS != 2
	
		// Go through all groups of bytes in the bitmap as a work group
		for(short i = localId; i < static_cast<short>(NUMBER_OF_BITMAP_BYTES / sizeof(uint)); i += localSize) {
		
			// Set group of bytes in nodes bitmap to the bitmap's
			nodesBitmapIndex[i] = atomic_load_explicit(&bitmap[i], memory_order_relaxed);
		}
	#endif
	
	// Go through all edges in this work group's bucket as a work group
	for(uint i = localId; i < numberOfEdges; i += localSize) {
	
		// Get edge's index
		const uint edgeIndex = indices[i];
		
		// Check if edge's node doesn't have a pair in the bitmap
		if(!isBitSetInBitmap(bitmap, (sipHash24(sipHashKeys, static_cast<ulong>(edgeIndex) * 2) & BITMAP_MASK) ^ 1)) {
		
			// Check is slean trimming parts is two
			#if SLEAN_TRIMMING_PARTS == 2
			
				// Get edge's index's bucket index
				const uint bucketIndex = edgeIndex >> (NUMBER_OF_LEAST_SIGNIFICANT_BITS_IGNORED_DURING_REMAINING_EDGES_BUCKET_SORTING - 1);
				
			// Otherwise
			#else
			
				// Get edge's index's bucket index
				const uint bucketIndex = ((edgeIndex - static_cast<uint>(NUMBER_OF_EDGES / (SLEAN_TRIMMING_PARTS / 2)) * (SLEAN_TRIMMING_PARTS / 2 - 1)) * (SLEAN_TRIMMING_PARTS / 2)) >> (NUMBER_OF_LEAST_SIGNIFICANT_BITS_IGNORED_DURING_REMAINING_EDGES_BUCKET_SORTING - 1);
			#endif
			
			// Get bucket's next edge index
			const uint nextEdgeIndex = min(atomic_fetch_add_explicit(&numberOfEdgesPerDestinationBucket[bucketIndex], 1, memory_order_relaxed), static_cast<uint>(AFTER_TRIMMING_ROUND_MAX_NUMBER_OF_EDGES_PER_REMAINING_EDGES_BUCKET / 2 - 1));
			
			// Get destination bucket's next indices
			device uint *bucketNextIndices = &destinationBuckets[AFTER_TRIMMING_ROUND_MAX_NUMBER_OF_EDGES_PER_REMAINING_EDGES_BUCKET / 2 * bucketIndex + nextEdgeIndex];
			
			// Set destination bucket's next edge to the edge
			*bucketNextIndices = edgeIndex;
		}
	}
}

// Check is slean trimming parts is two
#if SLEAN_TRIMMING_PARTS == 2

	// Trim edges step seventeen
	[[kernel]] void trimEdgesStepSeventeen(device const uint *buckets [[buffer(6)]], device const uint *numberOfEdgesPerBucket [[buffer(7)]], device uint *edgesBitmap [[buffer(8)]], const ushort localId [[thread_position_in_threadgroup]], const ushort localSize [[threads_per_threadgroup]], const uint groupId [[threadgroup_position_in_grid]]) {

// Otherwise
#else

	// Trim edges step seventeen
	[[kernel]] void trimEdgesStepSeventeen(device const uint *buckets [[buffer(6)]], device const uint *numberOfEdgesPerBucket [[buffer(7)]], device uint *edgesBitmap [[buffer(8)]], constant const uchar &part [[buffer(3)]], const ushort localId [[thread_position_in_threadgroup]], const ushort localSize [[threads_per_threadgroup]], const uint groupId [[threadgroup_position_in_grid]]) {
#endif

	// Declare bitmap
	threadgroup atomic_uint bitmap[static_cast<short>(NUMBER_OF_REMAINING_EDGES_BITMAP_BYTES / sizeof(uint))];
		
	// Check is slean trimming parts is two
	#if SLEAN_TRIMMING_PARTS == 2
	
		// Get work group's edges bitmap's index
		device uint *edgesBitmapIndex = &edgesBitmap[groupId * static_cast<short>(NUMBER_OF_REMAINING_EDGES_BITMAP_BYTES / sizeof(uint))];
		
	// Otherwise
	#else
	
		// Get work group's edges bitmap's index
		device uint *edgesBitmapIndex = &edgesBitmap[groupId * static_cast<short>(NUMBER_OF_REMAINING_EDGES_BITMAP_BYTES / sizeof(uint)) + part * static_cast<int>((static_cast<uint>(NUMBER_OF_EDGES / BITS_IN_A_BYTE) / (SLEAN_TRIMMING_PARTS / 2)) / sizeof(uint))];
	#endif
	
	// Go through all groups of bytes in the bitmap as a work group
	for(short i = localId; i < static_cast<short>(NUMBER_OF_REMAINING_EDGES_BITMAP_BYTES / sizeof(uint)); i += localSize) {
	
		// Set group of bytes in the bitmap to the edges bitmap's
		atomic_store_explicit(&bitmap[i], edgesBitmapIndex[i], memory_order_relaxed);
	}
	
	// Get number of edges in this work group's bucket
	const uint numberOfEdges = min(numberOfEdgesPerBucket[groupId], static_cast<uint>(AFTER_TRIMMING_ROUND_MAX_NUMBER_OF_EDGES_PER_REMAINING_EDGES_BUCKET / 2));
	
	// Get work group's bucket's indices
	device const uint *indices = &buckets[AFTER_TRIMMING_ROUND_MAX_NUMBER_OF_EDGES_PER_REMAINING_EDGES_BUCKET / 2 * groupId];
	
	// Synchronize work group
	threadgroup_barrier(mem_flags::mem_threadgroup);
	
	// Go through all edges in this work group's bucket as a work group
	for(uint i = localId; i < numberOfEdges; i += localSize) {
	
		// Disable edge's index in the bitmap
		clearBitInBitmap(bitmap, indices[i] & REMAINING_EDGES_BITMAP_MASK);
	}
	
	// Synchronize work group
	threadgroup_barrier(mem_flags::mem_threadgroup);
	
	// Go through all groups of bytes in the bitmap as a work group
	for(short i = localId; i < static_cast<short>(NUMBER_OF_REMAINING_EDGES_BITMAP_BYTES / sizeof(uint)); i += localSize) {
	
		// Set group of bytes in edges bitmap to the bitmap's
		edgesBitmapIndex[i] = atomic_load_explicit(&bitmap[i], memory_order_relaxed);
	}
}

// Check is slean trimming parts is two
#if SLEAN_TRIMMING_PARTS == 2

	// Trim edges step eighteen
	[[kernel]] void trimEdgesStepEighteen(device const uint *sourceBuckets [[buffer(0)]], device const uint *numberOfEdgesPerSourceBucket [[buffer(1)]], device const uint *nodesBitmap [[buffer(5)]], device uint *destinationBuckets [[buffer(6)]], device atomic_uint *numberOfEdgesPerDestinationBucket [[buffer(7)]], constant const ulong4 &sipHashKeys [[buffer(2)]], const ushort localId [[thread_position_in_threadgroup]], const ushort localSize [[threads_per_threadgroup]], const uint groupId [[threadgroup_position_in_grid]]) {

// Otherwise
#else

	// Trim edges step eighteen
	[[kernel]] void trimEdgesStepEighteen(device const uint *sourceBuckets [[buffer(0)]], device const uint *numberOfEdgesPerSourceBucket [[buffer(1)]], device const uint *nodesBitmap [[buffer(5)]], device uint *destinationBuckets [[buffer(6)]], device atomic_uint *numberOfEdgesPerDestinationBucket [[buffer(7)]], constant const ulong4 &sipHashKeys [[buffer(2)]], constant const uchar &part [[buffer(3)]], const ushort localId [[thread_position_in_threadgroup]], const ushort localSize [[threads_per_threadgroup]], const uint groupId [[threadgroup_position_in_grid]]) {
#endif

	// Declare bitmap
	threadgroup uint bitmap[static_cast<short>(NUMBER_OF_BITMAP_BYTES / sizeof(uint))];
		
	// Get work group's nodes bitmap's index
	device const uint *nodesBitmapIndex = &nodesBitmap[groupId * static_cast<short>(NUMBER_OF_BITMAP_BYTES / sizeof(uint))];
	
	// Go through all groups of bytes in the bitmap as a work group
	for(short i = localId; i < static_cast<short>(NUMBER_OF_BITMAP_BYTES / sizeof(uint)); i += localSize) {
	
		// Set group of bytes in the bitmap to the nodes bitmap's
		bitmap[i] = nodesBitmapIndex[i];
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
		
		// Check if edge's node doesn't have a pair in the bitmap
		if(!isBitSetInBitmap(bitmap, (sipHash24(sipHashKeys, static_cast<ulong>(edgeIndex) * 2) & BITMAP_MASK) ^ 1)) {
		
			// Check is slean trimming parts is two
			#if SLEAN_TRIMMING_PARTS == 2
			
				// Get edge's index's bucket index
				const uint bucketIndex = edgeIndex >> (NUMBER_OF_LEAST_SIGNIFICANT_BITS_IGNORED_DURING_REMAINING_EDGES_BUCKET_SORTING - 1);
				
			// Otherwise
			#else
			
				// Get edge's index's bucket index
				const uint bucketIndex = ((edgeIndex - static_cast<uint>(NUMBER_OF_EDGES / (SLEAN_TRIMMING_PARTS / 2)) * part) * (SLEAN_TRIMMING_PARTS / 2)) >> (NUMBER_OF_LEAST_SIGNIFICANT_BITS_IGNORED_DURING_REMAINING_EDGES_BUCKET_SORTING - 1);
			#endif
			
			// Get bucket's next edge index
			const uint nextEdgeIndex = min(atomic_fetch_add_explicit(&numberOfEdgesPerDestinationBucket[bucketIndex], 1, memory_order_relaxed), static_cast<uint>(AFTER_TRIMMING_ROUND_MAX_NUMBER_OF_EDGES_PER_REMAINING_EDGES_BUCKET / 2 - 1));
			
			// Get destination bucket's next indices
			device uint *bucketNextIndices = &destinationBuckets[AFTER_TRIMMING_ROUND_MAX_NUMBER_OF_EDGES_PER_REMAINING_EDGES_BUCKET / 2 * bucketIndex + nextEdgeIndex];
			
			// Set destination bucket's next edge to the edge
			*bucketNextIndices = edgeIndex;
		}
	}
}

// Check is slean trimming parts is two
#if SLEAN_TRIMMING_PARTS == 2

	// Trim edges step nineteen
	[[kernel]] void trimEdgesStepNineteen(device const ulong *edgesBitmap [[buffer(8)]], device uint2 *buckets [[buffer(0)]], device atomic_uint *numberOfEdgesPerBucket [[buffer(1)]], constant const uchar &nodesInSecondPartition [[buffer(9)]], constant const ulong4 &sipHashKeys [[buffer(2)]], const uint globalId [[thread_position_in_grid]]) {

// Otherwise
#else

	// Trim edges step nineteen
	[[kernel]] void trimEdgesStepNineteen(device const ulong *edgesBitmap [[buffer(8)]], device uint2 *buckets [[buffer(0)]], device atomic_uint *numberOfEdgesPerBucket [[buffer(1)]], constant const uchar &nodesInSecondPartition [[buffer(9)]], constant const ulong4 &sipHashKeys [[buffer(2)]], constant const uchar &part [[buffer(3)]], const uint globalId [[thread_position_in_grid]]) {
#endif

	// Check is slean trimming parts is two
	#if SLEAN_TRIMMING_PARTS == 2
	
		// Get work item's edge indices in the part
		const uint indices = globalId * static_cast<char>(sizeof(ulong) * BITS_IN_A_BYTE);
		
		// Get this work item's edges in the part
		ulong edges = edgesBitmap[globalId];
		
	// Otherwise
	#else
	
		// Get work item's edge indices in the part
		const uint indices = globalId * static_cast<char>(sizeof(ulong) * BITS_IN_A_BYTE) + part * static_cast<uint>(NUMBER_OF_EDGES / (SLEAN_TRIMMING_PARTS / 2));
		
		// Get this work item's edges in the part
		ulong edges = edgesBitmap[globalId + part * static_cast<int>((static_cast<uint>(NUMBER_OF_EDGES / BITS_IN_A_BYTE) / (SLEAN_TRIMMING_PARTS / 2)) / sizeof(ulong))];
	#endif
	
	// Go through all of this work item's enabled edges
	for(char currentBitIndex = clz(edges) + 1, previousBitIndex = 0; edges && previousBitIndex != static_cast<char>(sizeof(ulong) * BITS_IN_A_BYTE); edges <<= min(currentBitIndex, static_cast<char>(sizeof(ulong) * BITS_IN_A_BYTE - 1)), previousBitIndex += currentBitIndex, currentBitIndex = clz(edges) + 1) {
	
		// Get edge's index
		const uint edgeIndex = indices + (static_cast<char>(sizeof(ulong) * BITS_IN_A_BYTE) - currentBitIndex - previousBitIndex);
		
		// Get edge's node
		const uint node = sipHash24(sipHashKeys, (static_cast<ulong>(edgeIndex) * 2) | nodesInSecondPartition);
		
		// Get edge's node's bucket index
		const uint bucketIndex = node >> NUMBER_OF_LEAST_SIGNIFICANT_BITS_IGNORED_DURING_BUCKET_SORTING;
		
		// Get bucket's next edge index
		const uint nextEdgeIndex = min(atomic_fetch_add_explicit(&numberOfEdgesPerBucket[bucketIndex], 1, memory_order_relaxed), static_cast<uint>(AFTER_TRIMMING_ROUND_MAX_NUMBER_OF_EDGES_PER_BUCKET / 2 - 1));
		
		// Get bucket's next indices
		device uint2 *bucketNextIndices = &buckets[AFTER_TRIMMING_ROUND_MAX_NUMBER_OF_EDGES_PER_BUCKET / 2 * bucketIndex + nextEdgeIndex];
		
		// Set bucket's next edge to the edge and its node
		*bucketNextIndices = uint2(edgeIndex, node & BITMAP_MASK);
	}
}

// Trim edges step twenty
[[kernel]] void trimEdgesStepTwenty(device const uint2 *buckets [[buffer(0)]], device const uint *numberOfEdgesPerBucket [[buffer(1)]], device uint *nodesBitmap [[buffer(5)]], const ushort localId [[thread_position_in_threadgroup]], const ushort localSize [[threads_per_threadgroup]], const uint groupId [[threadgroup_position_in_grid]]) {

	// Declare bitmap
	threadgroup atomic_uint bitmap[static_cast<short>(NUMBER_OF_BITMAP_BYTES / sizeof(uint))];
	
	// Go through all groups of bytes in the bitmap as a work group
	for(short i = localId; i < static_cast<short>(NUMBER_OF_BITMAP_BYTES / sizeof(uint)); i += localSize) {
	
		// Set group of bytes in the bitmap to zero
		atomic_store_explicit(&bitmap[i], 0, memory_order_relaxed);
	}
	
	// Get number of edges in this work group's bucket
	const uint numberOfEdges = min(numberOfEdgesPerBucket[groupId], static_cast<uint>(AFTER_TRIMMING_ROUND_MAX_NUMBER_OF_EDGES_PER_BUCKET / 2));
	
	// Get work group's bucket's indices
	device const uint2 *indices = &buckets[AFTER_TRIMMING_ROUND_MAX_NUMBER_OF_EDGES_PER_BUCKET / 2 * groupId];
	
	// Synchronize work group
	threadgroup_barrier(mem_flags::mem_threadgroup);
	
	// Go through all edges in this work group's bucket as a work group
	for(uint i = localId; i < numberOfEdges; i += localSize) {
	
		// Enable edge's node in the bitmap
		setBitInBitmap(bitmap, indices[i].y);
	}
	
	// Get work group's nodes bitmap's index
	device uint *nodesBitmapIndex = &nodesBitmap[groupId * static_cast<short>(NUMBER_OF_BITMAP_BYTES / sizeof(uint))];
	
	// Synchronize work group
	threadgroup_barrier(mem_flags::mem_threadgroup);
	
	// Go through all groups of bytes in the bitmap as a work group
	for(short i = localId; i < static_cast<short>(NUMBER_OF_BITMAP_BYTES / sizeof(uint)); i += localSize) {
	
		// Set group of bytes in nodes bitmap to the bitmap's
		nodesBitmapIndex[i] = atomic_load_explicit(&bitmap[i], memory_order_relaxed);
	}
}

// Trim edges step twenty-one
[[kernel]] void trimEdgesStepTwentyOne(device const uint2 *buckets [[buffer(0)]], device const uint *numberOfEdgesPerBucket [[buffer(1)]], device uint *nodesBitmap [[buffer(5)]], const ushort localId [[thread_position_in_threadgroup]], const ushort localSize [[threads_per_threadgroup]], const uint groupId [[threadgroup_position_in_grid]]) {

	// Declare bitmap
	threadgroup atomic_uint bitmap[static_cast<short>(NUMBER_OF_BITMAP_BYTES / sizeof(uint))];
		
	// Get work group's nodes bitmap's index
	device uint *nodesBitmapIndex = &nodesBitmap[groupId * static_cast<short>(NUMBER_OF_BITMAP_BYTES / sizeof(uint))];
	
	// Go through all groups of bytes in the bitmap as a work group
	for(short i = localId; i < static_cast<short>(NUMBER_OF_BITMAP_BYTES / sizeof(uint)); i += localSize) {
	
		// Set group of bytes in the bitmap to the nodes bitmap's
		atomic_store_explicit(&bitmap[i], nodesBitmapIndex[i], memory_order_relaxed);
	}
	
	// Get number of edges in this work group's bucket
	const uint numberOfEdges = min(numberOfEdgesPerBucket[groupId], static_cast<uint>(AFTER_TRIMMING_ROUND_MAX_NUMBER_OF_EDGES_PER_BUCKET / 2));
	
	// Get work group's bucket's indices
	device const uint2 *indices = &buckets[AFTER_TRIMMING_ROUND_MAX_NUMBER_OF_EDGES_PER_BUCKET / 2 * groupId];
	
	// Synchronize work group
	threadgroup_barrier(mem_flags::mem_threadgroup);
	
	// Go through all edges in this work group's bucket as a work group
	for(uint i = localId; i < numberOfEdges; i += localSize) {
	
		// Enable edge's node in the bitmap
		setBitInBitmap(bitmap, indices[i].y);
	}
	
	// Synchronize work group
	threadgroup_barrier(mem_flags::mem_threadgroup);
	
	// Go through all groups of bytes in the bitmap as a work group
	for(short i = localId; i < static_cast<short>(NUMBER_OF_BITMAP_BYTES / sizeof(uint)); i += localSize) {
	
		// Set group of bytes in nodes bitmap to the bitmap's
		nodesBitmapIndex[i] = atomic_load_explicit(&bitmap[i], memory_order_relaxed);
	}
}

// Check is slean trimming parts is two
#if SLEAN_TRIMMING_PARTS == 2

	// Trim edges step twenty-two
	[[kernel]] void trimEdgesStepTwentyTwo(device const uint2 *sourceBuckets [[buffer(0)]], device const uint *numberOfEdgesPerSourceBucket [[buffer(1)]], device uint *destinationBuckets [[buffer(6)]], device atomic_uint *numberOfEdgesPerDestinationBucket [[buffer(7)]], const ushort localId [[thread_position_in_threadgroup]], const ushort localSize [[threads_per_threadgroup]], const uint groupId [[threadgroup_position_in_grid]]) {

// Otherwise
#else

	// Trim edges step twenty-two
	[[kernel]] void trimEdgesStepTwentyTwo(device const uint2 *sourceBuckets [[buffer(0)]], device const uint *numberOfEdgesPerSourceBucket [[buffer(1)]], device uint *destinationBuckets [[buffer(6)]], device atomic_uint *numberOfEdgesPerDestinationBucket [[buffer(7)]], device uint *nodesBitmap [[buffer(5)]], const ushort localId [[thread_position_in_threadgroup]], const ushort localSize [[threads_per_threadgroup]], const uint groupId [[threadgroup_position_in_grid]]) {
#endif

	// Declare bitmap
	threadgroup atomic_uint bitmap[static_cast<short>(NUMBER_OF_BITMAP_BYTES / sizeof(uint))];
		
	// Check is slean trimming parts isn't two
	#if SLEAN_TRIMMING_PARTS != 2
	
		// Get work group's nodes bitmap's index
		device uint *nodesBitmapIndex = &nodesBitmap[groupId * static_cast<short>(NUMBER_OF_BITMAP_BYTES / sizeof(uint))];
		
		// Go through all groups of bytes in the bitmap as a work group
		for(short i = localId; i < static_cast<short>(NUMBER_OF_BITMAP_BYTES / sizeof(uint)); i += localSize) {
		
			// Set group of bytes in the bitmap to the nodes bitmap's
			atomic_store_explicit(&bitmap[i], nodesBitmapIndex[i], memory_order_relaxed);
		}
		
	// Otherwise
	#else
	
		// Go through all groups of bytes in the bitmap as a work group
		for(short i = localId; i < static_cast<short>(NUMBER_OF_BITMAP_BYTES / sizeof(uint)); i += localSize) {
		
			// Set group of bytes in the bitmap to zero
			atomic_store_explicit(&bitmap[i], 0, memory_order_relaxed);
		}
	#endif
	
	// Get number of edges in this work group's bucket
	const uint numberOfEdges = min(numberOfEdgesPerSourceBucket[groupId], static_cast<uint>(AFTER_TRIMMING_ROUND_MAX_NUMBER_OF_EDGES_PER_BUCKET / 2));
	
	// Get work group's bucket's indices
	device const uint2 *indices = &sourceBuckets[AFTER_TRIMMING_ROUND_MAX_NUMBER_OF_EDGES_PER_BUCKET / 2 * groupId];
	
	// Synchronize work group
	threadgroup_barrier(mem_flags::mem_threadgroup);
	
	// Go through all edges in this work group's bucket as a work group
	for(uint i = localId; i < numberOfEdges; i += localSize) {
	
		// Enable edge's node in the bitmap
		setBitInBitmap(bitmap, indices[i].y);
	}
	
	// Synchronize work group
	threadgroup_barrier(mem_flags::mem_threadgroup);
	
	// Check is slean trimming parts isn't two
	#if SLEAN_TRIMMING_PARTS != 2
	
		// Go through all groups of bytes in the bitmap as a work group
		for(short i = localId; i < static_cast<short>(NUMBER_OF_BITMAP_BYTES / sizeof(uint)); i += localSize) {
		
			// Set group of bytes in nodes bitmap to the bitmap's
			nodesBitmapIndex[i] = atomic_load_explicit(&bitmap[i], memory_order_relaxed);
		}
	#endif
	
	// Go through all edges in this work group's bucket as a work group
	for(uint i = localId; i < numberOfEdges; i += localSize) {
	
		// Get edge's index and node
		const uint2 edgeIndexAndNode = indices[i];
		
		// Check if edge's node doesn't have a pair in the bitmap
		if(!isBitSetInBitmap(bitmap, edgeIndexAndNode.y ^ 1)) {
		
			// Check is slean trimming parts is two
			#if SLEAN_TRIMMING_PARTS == 2
			
				// Get edge's index's bucket index
				const uint bucketIndex = edgeIndexAndNode.x >> (NUMBER_OF_LEAST_SIGNIFICANT_BITS_IGNORED_DURING_REMAINING_EDGES_BUCKET_SORTING - 1);
				
			// Otherwise
			#else
			
				// Get edge's index's bucket index
				const uint bucketIndex = ((edgeIndexAndNode.x - static_cast<uint>(NUMBER_OF_EDGES / (SLEAN_TRIMMING_PARTS / 2)) * (SLEAN_TRIMMING_PARTS / 2 - 1)) * (SLEAN_TRIMMING_PARTS / 2)) >> (NUMBER_OF_LEAST_SIGNIFICANT_BITS_IGNORED_DURING_REMAINING_EDGES_BUCKET_SORTING - 1);
			#endif
			
			// Get bucket's next edge index
			const uint nextEdgeIndex = min(atomic_fetch_add_explicit(&numberOfEdgesPerDestinationBucket[bucketIndex], 1, memory_order_relaxed), static_cast<uint>(AFTER_TRIMMING_ROUND_MAX_NUMBER_OF_EDGES_PER_REMAINING_EDGES_BUCKET / 2 - 1));
			
			// Get destination bucket's next indices
			device uint *bucketNextIndices = &destinationBuckets[AFTER_TRIMMING_ROUND_MAX_NUMBER_OF_EDGES_PER_REMAINING_EDGES_BUCKET / 2 * bucketIndex + nextEdgeIndex];
			
			// Set destination bucket's next edge to the edge
			*bucketNextIndices = edgeIndexAndNode.x;
		}
	}
}

// Check is slean trimming parts is two
#if SLEAN_TRIMMING_PARTS == 2

	// Trim edges step twenty-three
	[[kernel]] void trimEdgesStepTwentyThree(device const uint2 *sourceBuckets [[buffer(0)]], device const uint *numberOfEdgesPerSourceBucket [[buffer(1)]], device const uint *nodesBitmap [[buffer(5)]], device uint *destinationBuckets [[buffer(6)]], device atomic_uint *numberOfEdgesPerDestinationBucket [[buffer(7)]], const ushort localId [[thread_position_in_threadgroup]], const ushort localSize [[threads_per_threadgroup]], const uint groupId [[threadgroup_position_in_grid]]) {

// Otherwise
#else

	// Trim edges step twenty-three
	[[kernel]] void trimEdgesStepTwentyThree(device const uint2 *sourceBuckets [[buffer(0)]], device const uint *numberOfEdgesPerSourceBucket [[buffer(1)]], device const uint *nodesBitmap [[buffer(5)]], device uint *destinationBuckets [[buffer(6)]], device atomic_uint *numberOfEdgesPerDestinationBucket [[buffer(7)]], constant const uchar &part [[buffer(3)]], const ushort localId [[thread_position_in_threadgroup]], const ushort localSize [[threads_per_threadgroup]], const uint groupId [[threadgroup_position_in_grid]]) {
#endif

	// Declare bitmap
	threadgroup uint bitmap[static_cast<short>(NUMBER_OF_BITMAP_BYTES / sizeof(uint))];
		
	// Get work group's nodes bitmap's index
	device const uint *nodesBitmapIndex = &nodesBitmap[groupId * static_cast<short>(NUMBER_OF_BITMAP_BYTES / sizeof(uint))];
	
	// Go through all groups of bytes in the bitmap as a work group
	for(short i = localId; i < static_cast<short>(NUMBER_OF_BITMAP_BYTES / sizeof(uint)); i += localSize) {
	
		// Set group of bytes in the bitmap to the nodes bitmap's
		bitmap[i] = nodesBitmapIndex[i];
	}
	
	// Get number of edges in this work group's bucket
	const uint numberOfEdges = min(numberOfEdgesPerSourceBucket[groupId], static_cast<uint>(AFTER_TRIMMING_ROUND_MAX_NUMBER_OF_EDGES_PER_BUCKET / 2));
	
	// Get work group's bucket's indices
	device const uint2 *indices = &sourceBuckets[AFTER_TRIMMING_ROUND_MAX_NUMBER_OF_EDGES_PER_BUCKET / 2 * groupId];
	
	// Synchronize work group
	threadgroup_barrier(mem_flags::mem_threadgroup);
	
	// Go through all edges in this work group's bucket as a work group
	for(uint i = localId; i < numberOfEdges; i += localSize) {
	
		// Get edge's index and node
		const uint2 edgeIndexAndNode = indices[i];
		
		// Check if edge's node doesn't have a pair in the bitmap
		if(!isBitSetInBitmap(bitmap, edgeIndexAndNode.y ^ 1)) {
		
			// Check is slean trimming parts is two
			#if SLEAN_TRIMMING_PARTS == 2
			
				// Get edge's index's bucket index
				const uint bucketIndex = edgeIndexAndNode.x >> (NUMBER_OF_LEAST_SIGNIFICANT_BITS_IGNORED_DURING_REMAINING_EDGES_BUCKET_SORTING - 1);
				
			// Otherwise
			#else
			
				// Get edge's index's bucket index
				const uint bucketIndex = ((edgeIndexAndNode.x - static_cast<uint>(NUMBER_OF_EDGES / (SLEAN_TRIMMING_PARTS / 2)) * part) * (SLEAN_TRIMMING_PARTS / 2)) >> (NUMBER_OF_LEAST_SIGNIFICANT_BITS_IGNORED_DURING_REMAINING_EDGES_BUCKET_SORTING - 1);
			#endif
			
			// Get bucket's next edge index
			const uint nextEdgeIndex = min(atomic_fetch_add_explicit(&numberOfEdgesPerDestinationBucket[bucketIndex], 1, memory_order_relaxed), static_cast<uint>(AFTER_TRIMMING_ROUND_MAX_NUMBER_OF_EDGES_PER_REMAINING_EDGES_BUCKET / 2 - 1));
			
			// Get destination bucket's next indices
			device uint *bucketNextIndices = &destinationBuckets[AFTER_TRIMMING_ROUND_MAX_NUMBER_OF_EDGES_PER_REMAINING_EDGES_BUCKET / 2 * bucketIndex + nextEdgeIndex];
			
			// Set destination bucket's next edge to the edge
			*bucketNextIndices = edgeIndexAndNode.x;
		}
	}
}

// Check is slean trimming parts is four
#if SLEAN_TRIMMING_PARTS == 4

	// Trim edges step twenty-four
	[[kernel]] void trimEdgesStepTwentyFour(device const ulong *edgesBitmap [[buffer(8)]], device uint2 *buckets [[buffer(0)]], device atomic_uint *numberOfEdgesPerBucket [[buffer(1)]], constant const uchar &nodesInSecondPartition [[buffer(9)]], constant const ulong4 &sipHashKeys [[buffer(2)]], const uint globalId [[thread_position_in_grid]]) {

// Otherwise
#else

	// Trim edges step twenty-four
	[[kernel]] void trimEdgesStepTwentyFour(device const ulong *edgesBitmap [[buffer(8)]], device uint2 *buckets [[buffer(0)]], device atomic_uint *numberOfEdgesPerBucket [[buffer(1)]], constant const uchar &nodesInSecondPartition [[buffer(9)]], constant const ulong4 &sipHashKeys [[buffer(2)]], constant const uchar &part [[buffer(3)]], const uint globalId [[thread_position_in_grid]]) {
#endif

	// Check is slean trimming parts is four
	#if SLEAN_TRIMMING_PARTS == 4
	
		// Get work item's edge indices in the part
		const uint indices = globalId * static_cast<char>(sizeof(ulong) * BITS_IN_A_BYTE);
		
		// Get this work item's edges in the part
		ulong edges = edgesBitmap[globalId];
		
	// Otherwise
	#else
	
		// Get work item's edge indices in the part
		const uint indices = globalId * static_cast<char>(sizeof(ulong) * BITS_IN_A_BYTE) + part * static_cast<uint>(NUMBER_OF_EDGES / ((SLEAN_TRIMMING_PARTS + 4 - 1) / 4));
		
		// Get this work item's edges in the part
		ulong edges = edgesBitmap[globalId + part * static_cast<int>((static_cast<uint>(NUMBER_OF_EDGES / BITS_IN_A_BYTE) / ((SLEAN_TRIMMING_PARTS + 4 - 1) / 4)) / sizeof(ulong))];
	#endif
	
	// Go through all of this work item's enabled edges
	for(char currentBitIndex = clz(edges) + 1, previousBitIndex = 0; edges && previousBitIndex != static_cast<char>(sizeof(ulong) * BITS_IN_A_BYTE); edges <<= min(currentBitIndex, static_cast<char>(sizeof(ulong) * BITS_IN_A_BYTE - 1)), previousBitIndex += currentBitIndex, currentBitIndex = clz(edges) + 1) {
	
		// Get edge's index
		const uint edgeIndex = indices + (static_cast<char>(sizeof(ulong) * BITS_IN_A_BYTE) - currentBitIndex - previousBitIndex);
		
		// Get edge's node
		const uint node = sipHash24(sipHashKeys, (static_cast<ulong>(edgeIndex) * 2) | nodesInSecondPartition);
		
		// Get edge's node's bucket index
		const uint bucketIndex = node >> NUMBER_OF_LEAST_SIGNIFICANT_BITS_IGNORED_DURING_BUCKET_SORTING;
		
		// Get bucket's next edge index
		const uint nextEdgeIndex = min(atomic_fetch_add_explicit(&numberOfEdgesPerBucket[bucketIndex], 1, memory_order_relaxed), static_cast<uint>(AFTER_TRIMMING_ROUND_MAX_NUMBER_OF_EDGES_PER_BUCKET / 2 - 1));
		
		// Get bucket's next indices
		device uint2 *bucketNextIndices = &buckets[AFTER_TRIMMING_ROUND_MAX_NUMBER_OF_EDGES_PER_BUCKET / 2 * bucketIndex + nextEdgeIndex];
		
		// Set bucket's next edge to the edge and its node
		*bucketNextIndices = uint2(edgeIndex, node & BITMAP_MASK);
	}
}

// Check is slean trimming parts is four
#if SLEAN_TRIMMING_PARTS == 4

	// Trim edges step twenty-five
	[[kernel]] void trimEdgesStepTwentyFive(device const uint2 *sourceBuckets [[buffer(0)]], device const uint *numberOfEdgesPerSourceBucket [[buffer(1)]], device uint *destinationBuckets [[buffer(6)]], device atomic_uint *numberOfEdgesPerDestinationBucket [[buffer(7)]], const ushort localId [[thread_position_in_threadgroup]], const ushort localSize [[threads_per_threadgroup]], const uint groupId [[threadgroup_position_in_grid]]) {

// Otherwise
#else

	// Trim edges step twenty-five
	[[kernel]] void trimEdgesStepTwentyFive(device const uint2 *sourceBuckets [[buffer(0)]], device const uint *numberOfEdgesPerSourceBucket [[buffer(1)]], device uint *destinationBuckets [[buffer(6)]], device atomic_uint *numberOfEdgesPerDestinationBucket [[buffer(7)]], device uint *nodesBitmap [[buffer(5)]], const ushort localId [[thread_position_in_threadgroup]], const ushort localSize [[threads_per_threadgroup]], const uint groupId [[threadgroup_position_in_grid]]) {
#endif

	// Declare bitmap
	threadgroup atomic_uint bitmap[static_cast<short>(NUMBER_OF_BITMAP_BYTES / sizeof(uint))];
		
	// Check is slean trimming parts isn't four
	#if SLEAN_TRIMMING_PARTS != 4
	
		// Get work group's nodes bitmap's index
		device uint *nodesBitmapIndex = &nodesBitmap[groupId * static_cast<short>(NUMBER_OF_BITMAP_BYTES / sizeof(uint))];
		
		// Go through all groups of bytes in the bitmap as a work group
		for(short i = localId; i < static_cast<short>(NUMBER_OF_BITMAP_BYTES / sizeof(uint)); i += localSize) {
		
			// Set group of bytes in the bitmap to the nodes bitmap's
			atomic_store_explicit(&bitmap[i], nodesBitmapIndex[i], memory_order_relaxed);
		}
		
	// Otherwise
	#else
	
		// Go through all groups of bytes in the bitmap as a work group
		for(short i = localId; i < static_cast<short>(NUMBER_OF_BITMAP_BYTES / sizeof(uint)); i += localSize) {
		
			// Set group of bytes in the bitmap to zero
			atomic_store_explicit(&bitmap[i], 0, memory_order_relaxed);
		}
	#endif
	
	// Get number of edges in this work group's bucket
	const uint numberOfEdges = min(numberOfEdgesPerSourceBucket[groupId], static_cast<uint>(AFTER_TRIMMING_ROUND_MAX_NUMBER_OF_EDGES_PER_BUCKET / 2));
	
	// Get work group's bucket's indices
	device const uint2 *indices = &sourceBuckets[AFTER_TRIMMING_ROUND_MAX_NUMBER_OF_EDGES_PER_BUCKET / 2 * groupId];
	
	// Synchronize work group
	threadgroup_barrier(mem_flags::mem_threadgroup);
	
	// Go through all edges in this work group's bucket as a work group
	for(uint i = localId; i < numberOfEdges; i += localSize) {
	
		// Enable edge's node in the bitmap
		setBitInBitmap(bitmap, indices[i].y);
	}
	
	// Synchronize work group
	threadgroup_barrier(mem_flags::mem_threadgroup);
	
	// Check is slean trimming parts isn't four
	#if SLEAN_TRIMMING_PARTS != 4
	
		// Go through all groups of bytes in the bitmap as a work group
		for(short i = localId; i < static_cast<short>(NUMBER_OF_BITMAP_BYTES / sizeof(uint)); i += localSize) {
		
			// Set group of bytes in nodes bitmap to the bitmap's
			nodesBitmapIndex[i] = atomic_load_explicit(&bitmap[i], memory_order_relaxed);
		}
	#endif
	
	// Go through all edges in this work group's bucket as a work group
	for(uint i = localId; i < numberOfEdges; i += localSize) {
	
		// Get edge's index and node
		const uint2 edgeIndexAndNode = indices[i];
		
		// Check if edge's node doesn't have a pair in the bitmap
		if(!isBitSetInBitmap(bitmap, edgeIndexAndNode.y ^ 1)) {
		
			// Check is slean trimming parts is four
			#if SLEAN_TRIMMING_PARTS == 4
			
				// Get edge's index's bucket index
				const uint bucketIndex = edgeIndexAndNode.x >> (NUMBER_OF_LEAST_SIGNIFICANT_BITS_IGNORED_DURING_REMAINING_EDGES_BUCKET_SORTING - 2);
				
			// Otherwise
			#else
			
				// Get edge's index's bucket index
				const uint bucketIndex = ((edgeIndexAndNode.x - static_cast<uint>(NUMBER_OF_EDGES / ((SLEAN_TRIMMING_PARTS + 4 - 1) / 4)) * (SLEAN_TRIMMING_PARTS / 4 - 1)) * (SLEAN_TRIMMING_PARTS / 4)) >> (NUMBER_OF_LEAST_SIGNIFICANT_BITS_IGNORED_DURING_REMAINING_EDGES_BUCKET_SORTING - 2);
			#endif
			
			// Get bucket's next edge index
			const uint nextEdgeIndex = min(atomic_fetch_add_explicit(&numberOfEdgesPerDestinationBucket[bucketIndex], 1, memory_order_relaxed), static_cast<uint>(AFTER_TRIMMING_ROUND_MAX_NUMBER_OF_EDGES_PER_REMAINING_EDGES_BUCKET / 4 - 1));
			
			// Get destination bucket's next indices
			device uint *bucketNextIndices = &destinationBuckets[AFTER_TRIMMING_ROUND_MAX_NUMBER_OF_EDGES_PER_REMAINING_EDGES_BUCKET / 4 * bucketIndex + nextEdgeIndex];
			
			// Set destination bucket's next edge to the edge
			*bucketNextIndices = edgeIndexAndNode.x;
		}
	}
}

// Check is slean trimming parts is four
#if SLEAN_TRIMMING_PARTS == 4

	// Trim edges step twenty-six
	[[kernel]] void trimEdgesStepTwentySix(device const uint *buckets [[buffer(6)]], device const uint *numberOfEdgesPerBucket [[buffer(7)]], device uint *edgesBitmap [[buffer(8)]], const ushort localId [[thread_position_in_threadgroup]], const ushort localSize [[threads_per_threadgroup]], const uint groupId [[threadgroup_position_in_grid]]) {

// Otherwise
#else

	// Trim edges step twenty-six
	[[kernel]] void trimEdgesStepTwentySix(device const uint *buckets [[buffer(6)]], device const uint *numberOfEdgesPerBucket [[buffer(7)]], device uint *edgesBitmap [[buffer(8)]], constant const uchar &part [[buffer(3)]], const ushort localId [[thread_position_in_threadgroup]], const ushort localSize [[threads_per_threadgroup]], const uint groupId [[threadgroup_position_in_grid]]) {
#endif

	// Declare bitmap
	threadgroup atomic_uint bitmap[static_cast<short>(NUMBER_OF_REMAINING_EDGES_BITMAP_BYTES / sizeof(uint))];
		
	// Check is slean trimming parts is four
	#if SLEAN_TRIMMING_PARTS == 4
	
		// Get work group's edges bitmap's index
		device uint *edgesBitmapIndex = &edgesBitmap[groupId * static_cast<short>(NUMBER_OF_REMAINING_EDGES_BITMAP_BYTES / sizeof(uint))];
		
	// Otherwise
	#else
	
		// Get work group's edges bitmap's index
		device uint *edgesBitmapIndex = &edgesBitmap[groupId * static_cast<short>(NUMBER_OF_REMAINING_EDGES_BITMAP_BYTES / sizeof(uint)) + part * static_cast<int>((static_cast<uint>(NUMBER_OF_EDGES / BITS_IN_A_BYTE) / ((SLEAN_TRIMMING_PARTS + 4 - 1) / 4)) / sizeof(uint))];
	#endif
	
	// Go through all groups of bytes in the bitmap as a work group
	for(short i = localId; i < static_cast<short>(NUMBER_OF_REMAINING_EDGES_BITMAP_BYTES / sizeof(uint)); i += localSize) {
	
		// Set group of bytes in the bitmap to the edges bitmap's
		atomic_store_explicit(&bitmap[i], edgesBitmapIndex[i], memory_order_relaxed);
	}
	
	// Get number of edges in this work group's bucket
	const uint numberOfEdges = min(numberOfEdgesPerBucket[groupId], static_cast<uint>(AFTER_TRIMMING_ROUND_MAX_NUMBER_OF_EDGES_PER_REMAINING_EDGES_BUCKET / 4));
	
	// Get work group's bucket's indices
	device const uint *indices = &buckets[AFTER_TRIMMING_ROUND_MAX_NUMBER_OF_EDGES_PER_REMAINING_EDGES_BUCKET / 4 * groupId];
	
	// Synchronize work group
	threadgroup_barrier(mem_flags::mem_threadgroup);
	
	// Go through all edges in this work group's bucket as a work group
	for(uint i = localId; i < numberOfEdges; i += localSize) {
	
		// Disable edge's index in the bitmap
		clearBitInBitmap(bitmap, indices[i] & REMAINING_EDGES_BITMAP_MASK);
	}
	
	// Synchronize work group
	threadgroup_barrier(mem_flags::mem_threadgroup);
	
	// Go through all groups of bytes in the bitmap as a work group
	for(short i = localId; i < static_cast<short>(NUMBER_OF_REMAINING_EDGES_BITMAP_BYTES / sizeof(uint)); i += localSize) {
	
		// Set group of bytes in edges bitmap to the bitmap's
		edgesBitmapIndex[i] = atomic_load_explicit(&bitmap[i], memory_order_relaxed);
	}
}

// Check is slean trimming parts is four
#if SLEAN_TRIMMING_PARTS == 4

	// Trim edges step twenty-seven
	[[kernel]] void trimEdgesStepTwentySeven(device const uint2 *sourceBuckets [[buffer(0)]], device const uint *numberOfEdgesPerSourceBucket [[buffer(1)]], device const uint *nodesBitmap [[buffer(5)]], device uint *destinationBuckets [[buffer(6)]], device atomic_uint *numberOfEdgesPerDestinationBucket [[buffer(7)]], const ushort localId [[thread_position_in_threadgroup]], const ushort localSize [[threads_per_threadgroup]], const uint groupId [[threadgroup_position_in_grid]]) {

// Otherwise
#else

	// Trim edges step twenty-seven
	[[kernel]] void trimEdgesStepTwentySeven(device const uint2 *sourceBuckets [[buffer(0)]], device const uint *numberOfEdgesPerSourceBucket [[buffer(1)]], device const uint *nodesBitmap [[buffer(5)]], device uint *destinationBuckets [[buffer(6)]], device atomic_uint *numberOfEdgesPerDestinationBucket [[buffer(7)]], constant const uchar &part [[buffer(3)]], const ushort localId [[thread_position_in_threadgroup]], const ushort localSize [[threads_per_threadgroup]], const uint groupId [[threadgroup_position_in_grid]]) {
#endif

	// Declare bitmap
	threadgroup uint bitmap[static_cast<short>(NUMBER_OF_BITMAP_BYTES / sizeof(uint))];
		
	// Get work group's nodes bitmap's index
	device const uint *nodesBitmapIndex = &nodesBitmap[groupId * static_cast<short>(NUMBER_OF_BITMAP_BYTES / sizeof(uint))];
	
	// Go through all groups of bytes in the bitmap as a work group
	for(short i = localId; i < static_cast<short>(NUMBER_OF_BITMAP_BYTES / sizeof(uint)); i += localSize) {
	
		// Set group of bytes in the bitmap to the nodes bitmap's
		bitmap[i] = nodesBitmapIndex[i];
	}
	
	// Get number of edges in this work group's bucket
	const uint numberOfEdges = min(numberOfEdgesPerSourceBucket[groupId], static_cast<uint>(AFTER_TRIMMING_ROUND_MAX_NUMBER_OF_EDGES_PER_BUCKET / 2));
	
	// Get work group's bucket's indices
	device const uint2 *indices = &sourceBuckets[AFTER_TRIMMING_ROUND_MAX_NUMBER_OF_EDGES_PER_BUCKET / 2 * groupId];
	
	// Synchronize work group
	threadgroup_barrier(mem_flags::mem_threadgroup);
	
	// Go through all edges in this work group's bucket as a work group
	for(uint i = localId; i < numberOfEdges; i += localSize) {
	
		// Get edge's index and node
		const uint2 edgeIndexAndNode = indices[i];
		
		// Check if edge's node doesn't have a pair in the bitmap
		if(!isBitSetInBitmap(bitmap, edgeIndexAndNode.y ^ 1)) {
		
			// Check is slean trimming parts is four
			#if SLEAN_TRIMMING_PARTS == 4
			
				// Get edge's index's bucket index
				const uint bucketIndex = edgeIndexAndNode.x >> (NUMBER_OF_LEAST_SIGNIFICANT_BITS_IGNORED_DURING_REMAINING_EDGES_BUCKET_SORTING - 2);
				
			// Otherwise
			#else
			
				// Get edge's index's bucket index
				const uint bucketIndex = ((edgeIndexAndNode.x - static_cast<uint>(NUMBER_OF_EDGES / ((SLEAN_TRIMMING_PARTS + 4 - 1) / 4)) * part) * (SLEAN_TRIMMING_PARTS / 4)) >> (NUMBER_OF_LEAST_SIGNIFICANT_BITS_IGNORED_DURING_REMAINING_EDGES_BUCKET_SORTING - 2);
			#endif
			
			// Get bucket's next edge index
			const uint nextEdgeIndex = min(atomic_fetch_add_explicit(&numberOfEdgesPerDestinationBucket[bucketIndex], 1, memory_order_relaxed), static_cast<uint>(AFTER_TRIMMING_ROUND_MAX_NUMBER_OF_EDGES_PER_REMAINING_EDGES_BUCKET / 4 - 1));
			
			// Get destination bucket's next indices
			device uint *bucketNextIndices = &destinationBuckets[AFTER_TRIMMING_ROUND_MAX_NUMBER_OF_EDGES_PER_REMAINING_EDGES_BUCKET / 4 * bucketIndex + nextEdgeIndex];
			
			// Set destination bucket's next edge to the edge
			*bucketNextIndices = edgeIndexAndNode.x;
		}
	}
}

// Check is slean trimming parts is eight
#if SLEAN_TRIMMING_PARTS == 8

	// Trim edges step twenty-eight
	[[kernel]] void trimEdgesStepTwentyEight(device const ulong *edgesBitmap [[buffer(8)]], device uint2 *buckets [[buffer(0)]], device atomic_uint *numberOfEdgesPerBucket [[buffer(1)]], constant const uchar &nodesInSecondPartition [[buffer(9)]], constant const ulong4 &sipHashKeys [[buffer(2)]], const uint globalId [[thread_position_in_grid]]) {

// Otherwise
#else

	// Trim edges step twenty-eight
	[[kernel]] void trimEdgesStepTwentyEight(device const ulong *edgesBitmap [[buffer(8)]], device uint2 *buckets [[buffer(0)]], device atomic_uint *numberOfEdgesPerBucket [[buffer(1)]], constant const uchar &nodesInSecondPartition [[buffer(9)]], constant const ulong4 &sipHashKeys [[buffer(2)]], constant const uchar &part [[buffer(3)]], const uint globalId [[thread_position_in_grid]]) {
#endif

	// Check is slean trimming parts is eight
	#if SLEAN_TRIMMING_PARTS == 8
	
		// Get work item's edge indices in the part
		const uint indices = globalId * static_cast<char>(sizeof(ulong) * BITS_IN_A_BYTE);
		
		// Get this work item's edges in the part
		ulong edges = edgesBitmap[globalId];
		
	// Otherwise
	#else
	
		// Get work item's edge indices in the part
		const uint indices = globalId * static_cast<char>(sizeof(ulong) * BITS_IN_A_BYTE) + part * static_cast<uint>(NUMBER_OF_EDGES / ((SLEAN_TRIMMING_PARTS + 8 - 1) / 8));
		
		// Get this work item's edges in the part
		ulong edges = edgesBitmap[globalId + part * static_cast<int>((static_cast<uint>(NUMBER_OF_EDGES / BITS_IN_A_BYTE) / ((SLEAN_TRIMMING_PARTS + 8 - 1) / 8)) / sizeof(ulong))];
	#endif
	
	// Go through all of this work item's enabled edges
	for(char currentBitIndex = clz(edges) + 1, previousBitIndex = 0; edges && previousBitIndex != static_cast<char>(sizeof(ulong) * BITS_IN_A_BYTE); edges <<= min(currentBitIndex, static_cast<char>(sizeof(ulong) * BITS_IN_A_BYTE - 1)), previousBitIndex += currentBitIndex, currentBitIndex = clz(edges) + 1) {
	
		// Get edge's index
		const uint edgeIndex = indices + (static_cast<char>(sizeof(ulong) * BITS_IN_A_BYTE) - currentBitIndex - previousBitIndex);
		
		// Get edge's node
		const uint node = sipHash24(sipHashKeys, (static_cast<ulong>(edgeIndex) * 2) | nodesInSecondPartition);
		
		// Get edge's node's bucket index
		const uint bucketIndex = node >> NUMBER_OF_LEAST_SIGNIFICANT_BITS_IGNORED_DURING_BUCKET_SORTING;
		
		// Get bucket's next edge index
		const uint nextEdgeIndex = min(atomic_fetch_add_explicit(&numberOfEdgesPerBucket[bucketIndex], 1, memory_order_relaxed), static_cast<uint>(AFTER_TRIMMING_ROUND_MAX_NUMBER_OF_EDGES_PER_BUCKET / 2 - 1));
		
		// Get bucket's next indices
		device uint2 *bucketNextIndices = &buckets[AFTER_TRIMMING_ROUND_MAX_NUMBER_OF_EDGES_PER_BUCKET / 2 * bucketIndex + nextEdgeIndex];
		
		// Set bucket's next edge to the edge and its node
		*bucketNextIndices = uint2(edgeIndex, node & BITMAP_MASK);
	}
}

// Check is slean trimming parts is eight
#if SLEAN_TRIMMING_PARTS == 8

	// Trim edges step twenty-nine
	[[kernel]] void trimEdgesStepTwentyNine(device const uint2 *sourceBuckets [[buffer(0)]], device const uint *numberOfEdgesPerSourceBucket [[buffer(1)]], device uint *destinationBuckets [[buffer(6)]], device atomic_uint *numberOfEdgesPerDestinationBucket [[buffer(7)]], const ushort localId [[thread_position_in_threadgroup]], const ushort localSize [[threads_per_threadgroup]], const uint groupId [[threadgroup_position_in_grid]]) {

// Otherwise
#else

	// Trim edges step twenty-nine
	[[kernel]] void trimEdgesStepTwentyNine(device const uint2 *sourceBuckets [[buffer(0)]], device const uint *numberOfEdgesPerSourceBucket [[buffer(1)]], device uint *destinationBuckets [[buffer(6)]], device atomic_uint *numberOfEdgesPerDestinationBucket [[buffer(7)]], device uint *nodesBitmap [[buffer(5)]], const ushort localId [[thread_position_in_threadgroup]], const ushort localSize [[threads_per_threadgroup]], const uint groupId [[threadgroup_position_in_grid]]) {
#endif

	// Declare bitmap
	threadgroup atomic_uint bitmap[static_cast<short>(NUMBER_OF_BITMAP_BYTES / sizeof(uint))];
		
	// Check is slean trimming parts isn't eight
	#if SLEAN_TRIMMING_PARTS != 8
	
		// Get work group's nodes bitmap's index
		device uint *nodesBitmapIndex = &nodesBitmap[groupId * static_cast<short>(NUMBER_OF_BITMAP_BYTES / sizeof(uint))];
		
		// Go through all groups of bytes in the bitmap as a work group
		for(short i = localId; i < static_cast<short>(NUMBER_OF_BITMAP_BYTES / sizeof(uint)); i += localSize) {
		
			// Set group of bytes in the bitmap to the nodes bitmap's
			atomic_store_explicit(&bitmap[i], nodesBitmapIndex[i], memory_order_relaxed);
		}
		
	// Otherwise
	#else
	
		// Go through all groups of bytes in the bitmap as a work group
		for(short i = localId; i < static_cast<short>(NUMBER_OF_BITMAP_BYTES / sizeof(uint)); i += localSize) {
		
			// Set group of bytes in the bitmap to zero
			atomic_store_explicit(&bitmap[i], 0, memory_order_relaxed);
		}
	#endif
	
	// Get number of edges in this work group's bucket
	const uint numberOfEdges = min(numberOfEdgesPerSourceBucket[groupId], static_cast<uint>(AFTER_TRIMMING_ROUND_MAX_NUMBER_OF_EDGES_PER_BUCKET / 2));
	
	// Get work group's bucket's indices
	device const uint2 *indices = &sourceBuckets[AFTER_TRIMMING_ROUND_MAX_NUMBER_OF_EDGES_PER_BUCKET / 2 * groupId];
	
	// Synchronize work group
	threadgroup_barrier(mem_flags::mem_threadgroup);
	
	// Go through all edges in this work group's bucket as a work group
	for(uint i = localId; i < numberOfEdges; i += localSize) {
	
		// Enable edge's node in the bitmap
		setBitInBitmap(bitmap, indices[i].y);
	}
	
	// Synchronize work group
	threadgroup_barrier(mem_flags::mem_threadgroup);
	
	// Check is slean trimming parts isn't eight
	#if SLEAN_TRIMMING_PARTS != 8
	
		// Go through all groups of bytes in the bitmap as a work group
		for(short i = localId; i < static_cast<short>(NUMBER_OF_BITMAP_BYTES / sizeof(uint)); i += localSize) {
		
			// Set group of bytes in nodes bitmap to the bitmap's
			nodesBitmapIndex[i] = atomic_load_explicit(&bitmap[i], memory_order_relaxed);
		}
	#endif
	
	// Go through all edges in this work group's bucket as a work group
	for(uint i = localId; i < numberOfEdges; i += localSize) {
	
		// Get edge's index and node
		const uint2 edgeIndexAndNode = indices[i];
		
		// Check if edge's node doesn't have a pair in the bitmap
		if(!isBitSetInBitmap(bitmap, edgeIndexAndNode.y ^ 1)) {
		
			// Check is slean trimming parts is eight
			#if SLEAN_TRIMMING_PARTS == 8
			
				// Get edge's index's bucket index
				const uint bucketIndex = edgeIndexAndNode.x >> (NUMBER_OF_LEAST_SIGNIFICANT_BITS_IGNORED_DURING_REMAINING_EDGES_BUCKET_SORTING - 3);
				
			// Otherwise
			#else
			
				// Get edge's index's bucket index
				const uint bucketIndex = ((edgeIndexAndNode.x - static_cast<uint>(NUMBER_OF_EDGES / ((SLEAN_TRIMMING_PARTS + 8 - 1) / 8)) * (SLEAN_TRIMMING_PARTS / 8 - 1)) * (SLEAN_TRIMMING_PARTS / 8)) >> (NUMBER_OF_LEAST_SIGNIFICANT_BITS_IGNORED_DURING_REMAINING_EDGES_BUCKET_SORTING - 3);
			#endif
			
			// Get bucket's next edge index
			const uint nextEdgeIndex = min(atomic_fetch_add_explicit(&numberOfEdgesPerDestinationBucket[bucketIndex], 1, memory_order_relaxed), static_cast<uint>(AFTER_TRIMMING_ROUND_MAX_NUMBER_OF_EDGES_PER_REMAINING_EDGES_BUCKET / 8 - 1));
			
			// Get destination bucket's next indices
			device uint *bucketNextIndices = &destinationBuckets[AFTER_TRIMMING_ROUND_MAX_NUMBER_OF_EDGES_PER_REMAINING_EDGES_BUCKET / 8 * bucketIndex + nextEdgeIndex];
			
			// Set destination bucket's next edge to the edge
			*bucketNextIndices = edgeIndexAndNode.x;
		}
	}
}

// Check is slean trimming parts is eight
#if SLEAN_TRIMMING_PARTS == 8

	// Trim edges step thirty
	[[kernel]] void trimEdgesStepThirty(device const uint *buckets [[buffer(6)]], device const uint *numberOfEdgesPerBucket [[buffer(7)]], device uint *edgesBitmap [[buffer(8)]], const ushort localId [[thread_position_in_threadgroup]], const ushort localSize [[threads_per_threadgroup]], const uint groupId [[threadgroup_position_in_grid]]) {

// Otherwise
#else

	// Trim edges step thirty
	[[kernel]] void trimEdgesStepThirty(device const uint *buckets [[buffer(6)]], device const uint *numberOfEdgesPerBucket [[buffer(7)]], device uint *edgesBitmap [[buffer(8)]], constant const uchar &part [[buffer(3)]], const ushort localId [[thread_position_in_threadgroup]], const ushort localSize [[threads_per_threadgroup]], const uint groupId [[threadgroup_position_in_grid]]) {
#endif

	// Declare bitmap
	threadgroup atomic_uint bitmap[static_cast<short>(NUMBER_OF_REMAINING_EDGES_BITMAP_BYTES / sizeof(uint))];
		
	// Check is slean trimming parts is eight
	#if SLEAN_TRIMMING_PARTS == 8
	
		// Get work group's edges bitmap's index
		device uint *edgesBitmapIndex = &edgesBitmap[groupId * static_cast<short>(NUMBER_OF_REMAINING_EDGES_BITMAP_BYTES / sizeof(uint))];
		
	// Otherwise
	#else
	
		// Get work group's edges bitmap's index
		device uint *edgesBitmapIndex = &edgesBitmap[groupId * static_cast<short>(NUMBER_OF_REMAINING_EDGES_BITMAP_BYTES / sizeof(uint)) + part * static_cast<int>((static_cast<uint>(NUMBER_OF_EDGES / BITS_IN_A_BYTE) / ((SLEAN_TRIMMING_PARTS + 8 - 1) / 8)) / sizeof(uint))];
	#endif
	
	// Go through all groups of bytes in the bitmap as a work group
	for(short i = localId; i < static_cast<short>(NUMBER_OF_REMAINING_EDGES_BITMAP_BYTES / sizeof(uint)); i += localSize) {
	
		// Set group of bytes in the bitmap to the edges bitmap's
		atomic_store_explicit(&bitmap[i], edgesBitmapIndex[i], memory_order_relaxed);
	}
	
	// Get number of edges in this work group's bucket
	const uint numberOfEdges = min(numberOfEdgesPerBucket[groupId], static_cast<uint>(AFTER_TRIMMING_ROUND_MAX_NUMBER_OF_EDGES_PER_REMAINING_EDGES_BUCKET / 8));
	
	// Get work group's bucket's indices
	device const uint *indices = &buckets[AFTER_TRIMMING_ROUND_MAX_NUMBER_OF_EDGES_PER_REMAINING_EDGES_BUCKET / 8 * groupId];
	
	// Synchronize work group
	threadgroup_barrier(mem_flags::mem_threadgroup);
	
	// Go through all edges in this work group's bucket as a work group
	for(uint i = localId; i < numberOfEdges; i += localSize) {
	
		// Disable edge's index in the bitmap
		clearBitInBitmap(bitmap, indices[i] & REMAINING_EDGES_BITMAP_MASK);
	}
	
	// Synchronize work group
	threadgroup_barrier(mem_flags::mem_threadgroup);
	
	// Go through all groups of bytes in the bitmap as a work group
	for(short i = localId; i < static_cast<short>(NUMBER_OF_REMAINING_EDGES_BITMAP_BYTES / sizeof(uint)); i += localSize) {
	
		// Set group of bytes in edges bitmap to the bitmap's
		edgesBitmapIndex[i] = atomic_load_explicit(&bitmap[i], memory_order_relaxed);
	}
}

// Check is slean trimming parts is eight
#if SLEAN_TRIMMING_PARTS == 8

	// Trim edges step thirty-one
	[[kernel]] void trimEdgesStepThirtyOne(device const uint2 *sourceBuckets [[buffer(0)]], device const uint *numberOfEdgesPerSourceBucket [[buffer(1)]], device const uint *nodesBitmap [[buffer(5)]], device uint *destinationBuckets [[buffer(6)]], device atomic_uint *numberOfEdgesPerDestinationBucket [[buffer(7)]], const ushort localId [[thread_position_in_threadgroup]], const ushort localSize [[threads_per_threadgroup]], const uint groupId [[threadgroup_position_in_grid]]) {

// Otherwise
#else

	// Trim edges step thirty-one
	[[kernel]] void trimEdgesStepThirtyOne(device const uint2 *sourceBuckets [[buffer(0)]], device const uint *numberOfEdgesPerSourceBucket [[buffer(1)]], device const uint *nodesBitmap [[buffer(5)]], device uint *destinationBuckets [[buffer(6)]], device atomic_uint *numberOfEdgesPerDestinationBucket [[buffer(7)]], constant const uchar &part [[buffer(3)]], const ushort localId [[thread_position_in_threadgroup]], const ushort localSize [[threads_per_threadgroup]], const uint groupId [[threadgroup_position_in_grid]]) {
#endif

	// Declare bitmap
	threadgroup uint bitmap[static_cast<short>(NUMBER_OF_BITMAP_BYTES / sizeof(uint))];
		
	// Get work group's nodes bitmap's index
	device const uint *nodesBitmapIndex = &nodesBitmap[groupId * static_cast<short>(NUMBER_OF_BITMAP_BYTES / sizeof(uint))];
	
	// Go through all groups of bytes in the bitmap as a work group
	for(short i = localId; i < static_cast<short>(NUMBER_OF_BITMAP_BYTES / sizeof(uint)); i += localSize) {
	
		// Set group of bytes in the bitmap to the nodes bitmap's
		bitmap[i] = nodesBitmapIndex[i];
	}
	
	// Get number of edges in this work group's bucket
	const uint numberOfEdges = min(numberOfEdgesPerSourceBucket[groupId], static_cast<uint>(AFTER_TRIMMING_ROUND_MAX_NUMBER_OF_EDGES_PER_BUCKET / 2));
	
	// Get work group's bucket's indices
	device const uint2 *indices = &sourceBuckets[AFTER_TRIMMING_ROUND_MAX_NUMBER_OF_EDGES_PER_BUCKET / 2 * groupId];
	
	// Synchronize work group
	threadgroup_barrier(mem_flags::mem_threadgroup);
	
	// Go through all edges in this work group's bucket as a work group
	for(uint i = localId; i < numberOfEdges; i += localSize) {
	
		// Get edge's index and node
		const uint2 edgeIndexAndNode = indices[i];
		
		// Check if edge's node doesn't have a pair in the bitmap
		if(!isBitSetInBitmap(bitmap, edgeIndexAndNode.y ^ 1)) {
		
			// Check is slean trimming parts is eight
			#if SLEAN_TRIMMING_PARTS == 8
			
				// Get edge's index's bucket index
				const uint bucketIndex = edgeIndexAndNode.x >> (NUMBER_OF_LEAST_SIGNIFICANT_BITS_IGNORED_DURING_REMAINING_EDGES_BUCKET_SORTING - 3);
				
			// Otherwise
			#else
			
				// Get edge's index's bucket index
				const uint bucketIndex = ((edgeIndexAndNode.x - static_cast<uint>(NUMBER_OF_EDGES / ((SLEAN_TRIMMING_PARTS + 8 - 1) / 8)) * part) * (SLEAN_TRIMMING_PARTS / 8)) >> (NUMBER_OF_LEAST_SIGNIFICANT_BITS_IGNORED_DURING_REMAINING_EDGES_BUCKET_SORTING - 3);
			#endif
			
			// Get bucket's next edge index
			const uint nextEdgeIndex = min(atomic_fetch_add_explicit(&numberOfEdgesPerDestinationBucket[bucketIndex], 1, memory_order_relaxed), static_cast<uint>(AFTER_TRIMMING_ROUND_MAX_NUMBER_OF_EDGES_PER_REMAINING_EDGES_BUCKET / 8 - 1));
			
			// Get destination bucket's next indices
			device uint *bucketNextIndices = &destinationBuckets[AFTER_TRIMMING_ROUND_MAX_NUMBER_OF_EDGES_PER_REMAINING_EDGES_BUCKET / 8 * bucketIndex + nextEdgeIndex];
			
			// Set destination bucket's next edge to the edge
			*bucketNextIndices = edgeIndexAndNode.x;
		}
	}
}

// Trim edges step thirty-two
[[kernel]] void trimEdgesStepThirtyTwo(device const ulong *edgesBitmap [[buffer(8)]], device uint2 *buckets [[buffer(0)]], device atomic_uint *numberOfEdgesPerBucket [[buffer(1)]], constant const uchar &nodesInSecondPartition [[buffer(9)]], constant const ulong4 &sipHashKeys [[buffer(2)]], const uint globalId [[thread_position_in_grid]]) {

	// Get work item's edge indices in the part
	const uint indices = globalId * static_cast<char>(sizeof(ulong) * BITS_IN_A_BYTE);
	
	// Get this work item's edges in the part
	ulong edges = edgesBitmap[globalId];
	
	// Go through all of this work item's enabled edges
	for(char currentBitIndex = clz(edges) + 1, previousBitIndex = 0; edges && previousBitIndex != static_cast<char>(sizeof(ulong) * BITS_IN_A_BYTE); edges <<= min(currentBitIndex, static_cast<char>(sizeof(ulong) * BITS_IN_A_BYTE - 1)), previousBitIndex += currentBitIndex, currentBitIndex = clz(edges) + 1) {
	
		// Get edge's index
		const uint edgeIndex = indices + (static_cast<char>(sizeof(ulong) * BITS_IN_A_BYTE) - currentBitIndex - previousBitIndex);
		
		// Get edge's node
		const uint node = sipHash24(sipHashKeys, (static_cast<ulong>(edgeIndex) * 2) | nodesInSecondPartition);
		
		// Get edge's node's bucket index
		const uint bucketIndex = node >> NUMBER_OF_LEAST_SIGNIFICANT_BITS_IGNORED_DURING_BUCKET_SORTING;
		
		// Get bucket's next edge index
		const uint nextEdgeIndex = min(atomic_fetch_add_explicit(&numberOfEdgesPerBucket[bucketIndex], 1, memory_order_relaxed), static_cast<uint>(AFTER_TRIMMING_ROUND_MAX_NUMBER_OF_EDGES_PER_BUCKET / 2 - 1));
		
		// Get bucket's next indices
		device uint2 *bucketNextIndices = &buckets[AFTER_TRIMMING_ROUND_MAX_NUMBER_OF_EDGES_PER_BUCKET / 2 * bucketIndex + nextEdgeIndex];
		
		// Set bucket's next edge to the edge and its node
		*bucketNextIndices = uint2(edgeIndex, node & BITMAP_MASK);
	}
}

// Trim edges step thirty-three
[[kernel]] void trimEdgesStepThirtyThree(device const uint2 *sourceBuckets [[buffer(0)]], device const uint *numberOfEdgesPerSourceBucket [[buffer(1)]], device uint *destinationBuckets [[buffer(6)]], device atomic_uint *numberOfEdgesPerDestinationBucket [[buffer(7)]], const ushort localId [[thread_position_in_threadgroup]], const ushort localSize [[threads_per_threadgroup]], const uint groupId [[threadgroup_position_in_grid]]) {

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
		setBitInBitmap(bitmap, indices[i].y);
	}
	
	// Synchronize work group
	threadgroup_barrier(mem_flags::mem_threadgroup);
	
	// Go through all edges in this work group's bucket as a work group
	for(uint i = localId; i < numberOfEdges; i += localSize) {
	
		// Get edge's index and node
		const uint2 edgeIndexAndNode = indices[i];
		
		// Check if edge's node doesn't have a pair in the bitmap
		if(!isBitSetInBitmap(bitmap, edgeIndexAndNode.y ^ 1)) {
		
			// Get edge's index's bucket index
			const uint bucketIndex = edgeIndexAndNode.x >> (NUMBER_OF_LEAST_SIGNIFICANT_BITS_IGNORED_DURING_REMAINING_EDGES_BUCKET_SORTING - 4);
			
			// Get bucket's next edge index
			const uint nextEdgeIndex = min(atomic_fetch_add_explicit(&numberOfEdgesPerDestinationBucket[bucketIndex], 1, memory_order_relaxed), static_cast<uint>(AFTER_TRIMMING_ROUND_MAX_NUMBER_OF_EDGES_PER_REMAINING_EDGES_BUCKET / 16 - 1));
			
			// Get destination bucket's next indices
			device uint *bucketNextIndices = &destinationBuckets[AFTER_TRIMMING_ROUND_MAX_NUMBER_OF_EDGES_PER_REMAINING_EDGES_BUCKET / 16 * bucketIndex + nextEdgeIndex];
			
			// Set destination bucket's next edge to the edge
			*bucketNextIndices = edgeIndexAndNode.x;
		}
	}
}

// Trim edges step thirty-four
[[kernel]] void trimEdgesStepThirtyFour(device const uint *buckets [[buffer(6)]], device const uint *numberOfEdgesPerBucket [[buffer(7)]], device uint *edgesBitmap [[buffer(8)]], const ushort localId [[thread_position_in_threadgroup]], const ushort localSize [[threads_per_threadgroup]], const uint groupId [[threadgroup_position_in_grid]]) {

	// Declare bitmap
	threadgroup atomic_uint bitmap[static_cast<short>(NUMBER_OF_REMAINING_EDGES_BITMAP_BYTES / sizeof(uint))];
		
	// Get work group's edges bitmap's index
	device uint *edgesBitmapIndex = &edgesBitmap[groupId * static_cast<short>(NUMBER_OF_REMAINING_EDGES_BITMAP_BYTES / sizeof(uint))];
	
	// Go through all groups of bytes in the bitmap as a work group
	for(short i = localId; i < static_cast<short>(NUMBER_OF_REMAINING_EDGES_BITMAP_BYTES / sizeof(uint)); i += localSize) {
	
		// Set group of bytes in the bitmap to the edges bitmap's
		atomic_store_explicit(&bitmap[i], edgesBitmapIndex[i], memory_order_relaxed);
	}
	
	// Get number of edges in this work group's bucket
	const uint numberOfEdges = min(numberOfEdgesPerBucket[groupId], static_cast<uint>(AFTER_TRIMMING_ROUND_MAX_NUMBER_OF_EDGES_PER_REMAINING_EDGES_BUCKET / 16));
	
	// Get work group's bucket's indices
	device const uint *indices = &buckets[AFTER_TRIMMING_ROUND_MAX_NUMBER_OF_EDGES_PER_REMAINING_EDGES_BUCKET / 16 * groupId];
	
	// Synchronize work group
	threadgroup_barrier(mem_flags::mem_threadgroup);
	
	// Go through all edges in this work group's bucket as a work group
	for(uint i = localId; i < numberOfEdges; i += localSize) {
	
		// Disable edge's index in the bitmap
		clearBitInBitmap(bitmap, indices[i] & REMAINING_EDGES_BITMAP_MASK);
	}
	
	// Synchronize work group
	threadgroup_barrier(mem_flags::mem_threadgroup);
	
	// Go through all groups of bytes in the bitmap as a work group
	for(short i = localId; i < static_cast<short>(NUMBER_OF_REMAINING_EDGES_BITMAP_BYTES / sizeof(uint)); i += localSize) {
	
		// Set group of bytes in edges bitmap to the bitmap's
		edgesBitmapIndex[i] = atomic_load_explicit(&bitmap[i], memory_order_relaxed);
	}
}

// Check if number of buckets is one
#if NUMBER_OF_BUCKETS == 1

	// Clear number of edges per source bucket
	[[kernel]] void clearNumberOfEdgesPerSourceBucket(device uint *numberOfEdgesPerSourceBucket [[buffer(1)]], const uint globalId [[thread_position_in_grid]]) {
	
// Otherwise
#else

	// Clear number of edges per source bucket
	[[kernel]] void clearNumberOfEdgesPerSourceBucket(device ulong *numberOfEdgesPerSourceBucket [[buffer(1)]], const uint globalId [[thread_position_in_grid]]) {
#endif

	// Set this work item's value in number of edges per source bucket to zero
	numberOfEdgesPerSourceBucket[globalId] = 0;
}

// Check if number of remaining edges buckets is one
#if NUMBER_OF_REMAINING_EDGES_BUCKETS == 1

	// Clear number of edges per destination bucket one
	[[kernel]] void clearNumberOfEdgesPerDestinationBucketOne(device uint *numberOfEdgesPerDestinationBucket [[buffer(7)]], const uint globalId [[thread_position_in_grid]]) {

// Otherwise
#else

	// Clear number of edges per destination bucket one
	[[kernel]] void clearNumberOfEdgesPerDestinationBucketOne(device ulong *numberOfEdgesPerDestinationBucket [[buffer(7)]], const uint globalId [[thread_position_in_grid]]) {
#endif

	// Set this work item's value in number of edges per destination bucket to zero
	numberOfEdgesPerDestinationBucket[globalId] = 0;
}

// Clear number of edges per destination bucket two
[[kernel]] void clearNumberOfEdgesPerDestinationBucketTwo(device ulong *numberOfEdgesPerDestinationBucket [[buffer(7)]], const uint globalId [[thread_position_in_grid]]) {

	// Set this work item's value in number of edges per destination bucket to zero
	numberOfEdgesPerDestinationBucket[globalId] = 0;
}

// Clear number of edges per destination bucket four
[[kernel]] void clearNumberOfEdgesPerDestinationBucketFour(device ulong *numberOfEdgesPerDestinationBucket [[buffer(7)]], const uint globalId [[thread_position_in_grid]]) {

	// Set this work item's value in number of edges per destination bucket to zero
	numberOfEdgesPerDestinationBucket[globalId] = 0;
}

// Clear number of edges per destination bucket eight
[[kernel]] void clearNumberOfEdgesPerDestinationBucketEight(device ulong *numberOfEdgesPerDestinationBucket [[buffer(7)]], const uint globalId [[thread_position_in_grid]]) {

	// Set this work item's value in number of edges per destination bucket to zero
	numberOfEdgesPerDestinationBucket[globalId] = 0;
}

// Clear number of edges per destination bucket sixteen
[[kernel]] void clearNumberOfEdgesPerDestinationBucketSixteen(device ulong *numberOfEdgesPerDestinationBucket [[buffer(7)]], const uint globalId [[thread_position_in_grid]]) {

	// Set this work item's value in number of edges per destination bucket to zero
	numberOfEdgesPerDestinationBucket[globalId] = 0;
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

// Clear bit in bitmap
void clearBitInBitmap(threadgroup atomic_uint *bitmap, const uint index) {

	// Clear bit in bitmap
	atomic_fetch_xor_explicit(&bitmap[index / static_cast<char>(sizeof(uint) * BITS_IN_A_BYTE)], 1 << (index % static_cast<char>(sizeof(uint) * BITS_IN_A_BYTE)), memory_order_relaxed);
}

// Is bit set in bitmap
bool isBitSetInBitmap(threadgroup const atomic_uint *bitmap, const uint index) {

	// Return if bit is set in bitmap
	return atomic_load_explicit(&bitmap[index / static_cast<char>(sizeof(uint) * BITS_IN_A_BYTE)], memory_order_relaxed) & (1 << (index % static_cast<char>(sizeof(uint) * BITS_IN_A_BYTE)));
}

// Is bit set in bitmap
bool isBitSetInBitmap(threadgroup const uint *bitmap, const uint index) {

	// Return if bit is set in bitmap
	return bitmap[index / static_cast<char>(sizeof(uint) * BITS_IN_A_BYTE)] & (1 << (index % static_cast<char>(sizeof(uint) * BITS_IN_A_BYTE)));
}


)")

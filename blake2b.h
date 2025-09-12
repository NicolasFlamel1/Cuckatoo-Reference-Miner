// Header guard
#ifndef BLAKE2B_H
#define BLAKE2B_H


// Header files
using namespace std;


// Constants

// BLAKE2b buffer size
#define BLAKE2B_BUFFER_SIZE 128

// BLAKE2B number of rounds
#define BLAKE2B_NUMBER_OF_ROUNDS 12

// BLAKE2b components per vector
#define BLAKE2B_COMPONENTS_PER_VECTOR 4

// BLAKE2b initial working state
static const uint64_t BLAKE2B_INITIAL_WORKING_STATE[] = {7640891576939301160, 13503953896175478587ULL, 4354685564936845355, 11912009170470909681ULL, 5840696475078001361, 11170449401992604703ULL, 2270897969802886507, 6620516959819538809, 7640891576956012808, 13503953896175478587ULL, 4354685564936845355, 11912009170470909681ULL, 5840696475078001233, 11170449401992604703ULL, 2270897969802886507, 6620516959819538809};

// BLAKE2b initial state first half
static const uint64_t __attribute__((vector_size(sizeof(uint64_t) * BLAKE2B_COMPONENTS_PER_VECTOR))) BLAKE2B_INITIAL_STATE_FIRST_HALF = {BLAKE2B_INITIAL_WORKING_STATE[0], BLAKE2B_INITIAL_WORKING_STATE[1], BLAKE2B_INITIAL_WORKING_STATE[2], BLAKE2B_INITIAL_WORKING_STATE[3]};

// BLAKE2b initial state second half
static const uint64_t __attribute__((vector_size(sizeof(uint64_t) * BLAKE2B_COMPONENTS_PER_VECTOR))) BLAKE2B_INITIAL_STATE_SECOND_HALF = {BLAKE2B_INITIAL_WORKING_STATE[4], BLAKE2B_INITIAL_WORKING_STATE[5], BLAKE2B_INITIAL_WORKING_STATE[6], BLAKE2B_INITIAL_WORKING_STATE[7]};

// BLAKE2b SIGMA
static const uint8_t BLAKE2B_SIGMA[][16] = {
	{0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15},
	{14, 10, 4, 8, 9, 15, 13, 6, 1, 12, 0, 2, 11, 7, 5, 3},
	{11, 8, 12, 0, 5, 2, 15, 13, 10, 14, 3, 6, 7, 1, 9, 4},
	{7, 9, 3, 1, 13, 12, 11, 14, 2, 6, 5, 10, 4, 0, 15, 8},
	{9, 0, 5, 7, 2, 4, 10, 15, 14, 1, 11, 12, 6, 8, 3, 13},
	{2, 12, 6, 10, 0, 11, 8, 3, 4, 13, 7, 5, 15, 14, 1, 9},
	{12, 5, 1, 15, 14, 13, 4, 10, 0, 7, 6, 3, 9, 2, 8, 11},
	{13, 11, 7, 14, 12, 1, 3, 9, 5, 0, 15, 4, 8, 6, 2, 10},
	{6, 15, 14, 9, 11, 3, 0, 8, 12, 2, 13, 7, 1, 4, 10, 5},
	{10, 2, 8, 4, 7, 6, 1, 5, 15, 11, 9, 14, 3, 12, 13, 0},
	{0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15},
	{14, 10, 4, 8, 9, 15, 13, 6, 1, 12, 0, 2, 11, 7, 5, 3}
};


// Function prototypes

// BLAKE2b
static inline void blake2b(uint64_t __attribute__((vector_size(sizeof(uint64_t) * BLAKE2B_COMPONENTS_PER_VECTOR))) &result, const uint8_t header[HEADER_SIZE], uint64_t nonce) noexcept;

// BLAKE2b step
static inline void blake2bStep(uint64_t __attribute__((vector_size(sizeof(uint64_t) * BLAKE2B_COMPONENTS_PER_VECTOR))) &__restrict__ a, uint64_t __attribute__((vector_size(sizeof(uint64_t) * BLAKE2B_COMPONENTS_PER_VECTOR))) &__restrict__ b, uint64_t __attribute__((vector_size(sizeof(uint64_t) * BLAKE2B_COMPONENTS_PER_VECTOR))) &__restrict__ c, uint64_t __attribute__((vector_size(sizeof(uint64_t) * BLAKE2B_COMPONENTS_PER_VECTOR))) &__restrict__ d, const uint64_t __attribute__((vector_size(sizeof(uint64_t) * BLAKE2B_COMPONENTS_PER_VECTOR))) &__restrict__ x, const uint64_t __attribute__((vector_size(sizeof(uint64_t) * BLAKE2B_COMPONENTS_PER_VECTOR))) &__restrict__ y) noexcept;


// Supporting function implementation

// BLAKE2b
void blake2b(uint64_t __attribute__((vector_size(sizeof(uint64_t) * BLAKE2B_COMPONENTS_PER_VECTOR))) &result, const uint8_t header[HEADER_SIZE], uint64_t nonce) noexcept {

	// Throw error if header size is invalid
	static_assert(HEADER_SIZE + sizeof(nonce) > BLAKE2B_BUFFER_SIZE && HEADER_SIZE + sizeof(nonce) <= BLAKE2B_BUFFER_SIZE * 3, "Header's size is invalid");
	
	// Set buffer to beginning of header
	uint64_t buffer[BLAKE2B_BUFFER_SIZE / sizeof(uint64_t)] = {};
	memcpy(buffer, header, sizeof(buffer));
	
	// Go through all rounds
	uint64_t __attribute__((vector_size(sizeof(uint64_t) * BLAKE2B_COMPONENTS_PER_VECTOR))) a = BLAKE2B_INITIAL_STATE_FIRST_HALF;
	uint64_t __attribute__((vector_size(sizeof(uint64_t) * BLAKE2B_COMPONENTS_PER_VECTOR))) b = BLAKE2B_INITIAL_STATE_SECOND_HALF;
	uint64_t __attribute__((vector_size(sizeof(uint64_t) * BLAKE2B_COMPONENTS_PER_VECTOR))) c = {BLAKE2B_INITIAL_WORKING_STATE[8], BLAKE2B_INITIAL_WORKING_STATE[9], BLAKE2B_INITIAL_WORKING_STATE[10], BLAKE2B_INITIAL_WORKING_STATE[11]};
	uint64_t __attribute__((vector_size(sizeof(uint64_t) * BLAKE2B_COMPONENTS_PER_VECTOR))) d = {BLAKE2B_INITIAL_WORKING_STATE[12], BLAKE2B_INITIAL_WORKING_STATE[13], BLAKE2B_INITIAL_WORKING_STATE[14], BLAKE2B_INITIAL_WORKING_STATE[15]};
	
	for(uint_fast8_t i = 0; i < BLAKE2B_NUMBER_OF_ROUNDS; ++i) {
	
		// Set x and y for column step
		uint64_t __attribute__((vector_size(sizeof(uint64_t) * BLAKE2B_COMPONENTS_PER_VECTOR))) x = {buffer[BLAKE2B_SIGMA[i][0]], buffer[BLAKE2B_SIGMA[i][2]], buffer[BLAKE2B_SIGMA[i][4]], buffer[BLAKE2B_SIGMA[i][6]]};
		uint64_t __attribute__((vector_size(sizeof(uint64_t) * BLAKE2B_COMPONENTS_PER_VECTOR))) y = {buffer[BLAKE2B_SIGMA[i][1]], buffer[BLAKE2B_SIGMA[i][3]], buffer[BLAKE2B_SIGMA[i][5]], buffer[BLAKE2B_SIGMA[i][7]]};
		
		// Perform column step
		blake2bStep(a, b, c, d, x, y);
		
		// Update b, c, and d for diagonal step
		b = __builtin_shufflevector(b, b, 1, 2, 3, 0);
		c = __builtin_shufflevector(c, c, 2, 3, 0, 1);
		d = __builtin_shufflevector(d, d, 3, 0, 1, 2);
		
		// Set x, and y for diagonal step
		x = (const uint64_t __attribute__((vector_size(sizeof(uint64_t) * BLAKE2B_COMPONENTS_PER_VECTOR)))){buffer[BLAKE2B_SIGMA[i][8]], buffer[BLAKE2B_SIGMA[i][10]], buffer[BLAKE2B_SIGMA[i][12]], buffer[BLAKE2B_SIGMA[i][14]]};
		y = (const uint64_t __attribute__((vector_size(sizeof(uint64_t) * BLAKE2B_COMPONENTS_PER_VECTOR)))){buffer[BLAKE2B_SIGMA[i][9]], buffer[BLAKE2B_SIGMA[i][11]], buffer[BLAKE2B_SIGMA[i][13]], buffer[BLAKE2B_SIGMA[i][15]]};
		
		// Perform diagonal step
		blake2bStep(a, b, c, d, x, y);
		
		// Update b, c, and d for column step
		b = __builtin_shufflevector(b, b, 3, 0, 1, 2);
		c = __builtin_shufflevector(c, c, 2, 3, 0, 1);
		d = __builtin_shufflevector(d, d, 1, 2, 3, 0);
	}
	
	// Check if using Windows
	#ifdef _WIN32
	
		// Make nonce big endian
		nonce = _byteswap_uint64(nonce);
		
	// Otherwise check if using an Apple device
	#elif defined __APPLE__
	
		// Make nonce big endian
		nonce = OSSwapHostToBigInt64(nonce);
		
	// Otherwise
	#else
	
		// Make nonce big endian
		nonce = htobe64(nonce);
	#endif
	
	// Check if stratum server uses one mining algorithm
	#if STRATUM_SERVER_NUMBER_OF_MINING_ALGORITHMS == 1
	
		// Set buffer to end of header
		memcpy(buffer, &header[sizeof(buffer)], HEADER_SIZE - sizeof(buffer));
		
		// Append nonce in big endian to buffer
		memcpy(&reinterpret_cast<uint8_t *>(buffer)[HEADER_SIZE - sizeof(buffer)], &nonce, sizeof(nonce));
		
		// Pad buffer with zeros
		memset(&reinterpret_cast<uint8_t *>(buffer)[HEADER_SIZE - sizeof(buffer) + sizeof(nonce)], 0, sizeof(buffer) - (HEADER_SIZE - sizeof(buffer) + sizeof(nonce)));
		
		// Prepare values for next rounds
		a ^= BLAKE2B_INITIAL_STATE_FIRST_HALF ^ c;
		b ^= BLAKE2B_INITIAL_STATE_SECOND_HALF ^ d;
		const uint64_t __attribute__((vector_size(sizeof(uint64_t) * BLAKE2B_COMPONENTS_PER_VECTOR))) initialStateCurrentPartFirstHalf = a;
		
	// Otherwise
	#else
	
		// Set buffer to next part of header
		memcpy(buffer, &header[sizeof(buffer)], sizeof(buffer));
		
		// Go through all rounds
		a ^= BLAKE2B_INITIAL_STATE_FIRST_HALF ^ c;
		b ^= BLAKE2B_INITIAL_STATE_SECOND_HALF ^ d;
		c = (const uint64_t __attribute__((vector_size(sizeof(uint64_t) * BLAKE2B_COMPONENTS_PER_VECTOR)))){BLAKE2B_INITIAL_WORKING_STATE[8], BLAKE2B_INITIAL_WORKING_STATE[9], BLAKE2B_INITIAL_WORKING_STATE[10], BLAKE2B_INITIAL_WORKING_STATE[11]};
		d = (const uint64_t __attribute__((vector_size(sizeof(uint64_t) * BLAKE2B_COMPONENTS_PER_VECTOR)))){BLAKE2B_INITIAL_WORKING_STATE[12] ^ (sizeof(buffer) ^ (sizeof(buffer) * 2)), BLAKE2B_INITIAL_WORKING_STATE[13], BLAKE2B_INITIAL_WORKING_STATE[14], BLAKE2B_INITIAL_WORKING_STATE[15]};
		uint64_t __attribute__((vector_size(sizeof(uint64_t) * BLAKE2B_COMPONENTS_PER_VECTOR))) initialStateCurrentPartFirstHalf = a;
		const uint64_t __attribute__((vector_size(sizeof(uint64_t) * BLAKE2B_COMPONENTS_PER_VECTOR))) initialStateCurrentPartSecondHalf = b;
		
		for(uint_fast8_t i = 0; i < BLAKE2B_NUMBER_OF_ROUNDS; ++i) {
		
			// Set x and y for column step
			uint64_t __attribute__((vector_size(sizeof(uint64_t) * BLAKE2B_COMPONENTS_PER_VECTOR))) x = {buffer[BLAKE2B_SIGMA[i][0]], buffer[BLAKE2B_SIGMA[i][2]], buffer[BLAKE2B_SIGMA[i][4]], buffer[BLAKE2B_SIGMA[i][6]]};
			uint64_t __attribute__((vector_size(sizeof(uint64_t) * BLAKE2B_COMPONENTS_PER_VECTOR))) y = {buffer[BLAKE2B_SIGMA[i][1]], buffer[BLAKE2B_SIGMA[i][3]], buffer[BLAKE2B_SIGMA[i][5]], buffer[BLAKE2B_SIGMA[i][7]]};
			
			// Perform column step
			blake2bStep(a, b, c, d, x, y);
			
			// Update b, c, and d for diagonal step
			b = __builtin_shufflevector(b, b, 1, 2, 3, 0);
			c = __builtin_shufflevector(c, c, 2, 3, 0, 1);
			d = __builtin_shufflevector(d, d, 3, 0, 1, 2);
			
			// Set x, and y for diagonal step
			x = (const uint64_t __attribute__((vector_size(sizeof(uint64_t) * BLAKE2B_COMPONENTS_PER_VECTOR)))){buffer[BLAKE2B_SIGMA[i][8]], buffer[BLAKE2B_SIGMA[i][10]], buffer[BLAKE2B_SIGMA[i][12]], buffer[BLAKE2B_SIGMA[i][14]]};
			y = (const uint64_t __attribute__((vector_size(sizeof(uint64_t) * BLAKE2B_COMPONENTS_PER_VECTOR)))){buffer[BLAKE2B_SIGMA[i][9]], buffer[BLAKE2B_SIGMA[i][11]], buffer[BLAKE2B_SIGMA[i][13]], buffer[BLAKE2B_SIGMA[i][15]]};
			
			// Perform diagonal step
			blake2bStep(a, b, c, d, x, y);
			
			// Update b, c, and d for column step
			b = __builtin_shufflevector(b, b, 3, 0, 1, 2);
			c = __builtin_shufflevector(c, c, 2, 3, 0, 1);
			d = __builtin_shufflevector(d, d, 1, 2, 3, 0);
		}
		
		// Set buffer to end of header
		memcpy(buffer, &header[sizeof(buffer) * 2], HEADER_SIZE - sizeof(buffer) * 2);
		
		// Append nonce in big endian to buffer
		memcpy(&reinterpret_cast<uint8_t *>(buffer)[HEADER_SIZE - sizeof(buffer) * 2], &nonce, sizeof(nonce));
		
		// Pad buffer with zeros
		memset(&reinterpret_cast<uint8_t *>(buffer)[HEADER_SIZE - sizeof(buffer) * 2 + sizeof(nonce)], 0, sizeof(buffer) - (HEADER_SIZE - sizeof(buffer) * 2 + sizeof(nonce)));
		
		// Prepare values for next rounds
		a ^= initialStateCurrentPartFirstHalf ^ c;
		b ^= initialStateCurrentPartSecondHalf ^ d;
		initialStateCurrentPartFirstHalf = a;
	#endif
	
	// Go through all rounds
	c = (const uint64_t __attribute__((vector_size(sizeof(uint64_t) * BLAKE2B_COMPONENTS_PER_VECTOR)))){BLAKE2B_INITIAL_WORKING_STATE[8], BLAKE2B_INITIAL_WORKING_STATE[9], BLAKE2B_INITIAL_WORKING_STATE[10], BLAKE2B_INITIAL_WORKING_STATE[11]};
	d = (const uint64_t __attribute__((vector_size(sizeof(uint64_t) * BLAKE2B_COMPONENTS_PER_VECTOR)))){BLAKE2B_INITIAL_WORKING_STATE[12] ^ (sizeof(buffer) ^ (HEADER_SIZE + sizeof(nonce))), BLAKE2B_INITIAL_WORKING_STATE[13], BLAKE2B_INITIAL_WORKING_STATE[14] ^ UINT64_MAX, BLAKE2B_INITIAL_WORKING_STATE[15]};
		
	for(uint_fast8_t i = 0; i < BLAKE2B_NUMBER_OF_ROUNDS; ++i) {
	
		// Set x and y for column step
		uint64_t __attribute__((vector_size(sizeof(uint64_t) * BLAKE2B_COMPONENTS_PER_VECTOR))) x = {buffer[BLAKE2B_SIGMA[i][0]], buffer[BLAKE2B_SIGMA[i][2]], buffer[BLAKE2B_SIGMA[i][4]], buffer[BLAKE2B_SIGMA[i][6]]};
		uint64_t __attribute__((vector_size(sizeof(uint64_t) * BLAKE2B_COMPONENTS_PER_VECTOR))) y = {buffer[BLAKE2B_SIGMA[i][1]], buffer[BLAKE2B_SIGMA[i][3]], buffer[BLAKE2B_SIGMA[i][5]], buffer[BLAKE2B_SIGMA[i][7]]};
		
		// Perform column step
		blake2bStep(a, b, c, d, x, y);
		
		// Update b, c, and d for diagonal step
		b = __builtin_shufflevector(b, b, 1, 2, 3, 0);
		c = __builtin_shufflevector(c, c, 2, 3, 0, 1);
		d = __builtin_shufflevector(d, d, 3, 0, 1, 2);
		
		// Set x, and y for diagonal step
		x = (const uint64_t __attribute__((vector_size(sizeof(uint64_t) * BLAKE2B_COMPONENTS_PER_VECTOR)))){buffer[BLAKE2B_SIGMA[i][8]], buffer[BLAKE2B_SIGMA[i][10]], buffer[BLAKE2B_SIGMA[i][12]], buffer[BLAKE2B_SIGMA[i][14]]};
		y = (const uint64_t __attribute__((vector_size(sizeof(uint64_t) * BLAKE2B_COMPONENTS_PER_VECTOR)))){buffer[BLAKE2B_SIGMA[i][9]], buffer[BLAKE2B_SIGMA[i][11]], buffer[BLAKE2B_SIGMA[i][13]], buffer[BLAKE2B_SIGMA[i][15]]};
		
		// Perform diagonal step
		blake2bStep(a, b, c, d, x, y);
		
		// Update b, c, and d for column step
		b = __builtin_shufflevector(b, b, 3, 0, 1, 2);
		c = __builtin_shufflevector(c, c, 2, 3, 0, 1);
		d = __builtin_shufflevector(d, d, 1, 2, 3, 0);
	}
	
	// Get result from working state
	result = initialStateCurrentPartFirstHalf ^ a ^ c;
}

// BLAKE2b step
void blake2bStep(uint64_t __attribute__((vector_size(sizeof(uint64_t) * BLAKE2B_COMPONENTS_PER_VECTOR))) &__restrict__ a, uint64_t __attribute__((vector_size(sizeof(uint64_t) * BLAKE2B_COMPONENTS_PER_VECTOR))) &__restrict__ b, uint64_t __attribute__((vector_size(sizeof(uint64_t) * BLAKE2B_COMPONENTS_PER_VECTOR))) &__restrict__ c, uint64_t __attribute__((vector_size(sizeof(uint64_t) * BLAKE2B_COMPONENTS_PER_VECTOR))) &__restrict__ d, const uint64_t __attribute__((vector_size(sizeof(uint64_t) * BLAKE2B_COMPONENTS_PER_VECTOR))) &__restrict__ x, const uint64_t __attribute__((vector_size(sizeof(uint64_t) * BLAKE2B_COMPONENTS_PER_VECTOR))) &__restrict__ y) noexcept {

	// Perform step on values
	a += b + x;
	d = ((d ^ a) >> 32) | ((d ^ a) << (64 - 32));
	c += d;
	b = ((b ^ c) >> 24) | ((b ^ c) << (64 - 24));
	a += b + y;
	d = ((d ^ a) >> 16) | ((d ^ a) << (64 - 16));
	c += d;
	b = ((b ^ c) >> 63) | ((b ^ c) << (64 - 63));
}


#endif

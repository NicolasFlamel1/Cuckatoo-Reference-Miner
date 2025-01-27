// Header guard
#ifndef SIPHASH_H
#define SIPHASH_H


// Header files
using namespace std;


// Constants

// SipHash keys size
#define SIPHASH_KEYS_SIZE 4

// SipRound rotation
#define SIP_ROUND_ROTATION 21


// Function prototypes

// SipHash-2-4
template<const unsigned int numberOfStates> static inline void sipHash24(uint64_t __attribute__((vector_size(sizeof(uint64_t) * numberOfStates))) *uvs, const uint64_t __attribute__((vector_size(sizeof(uint64_t) * SIPHASH_KEYS_SIZE))) &keys, const uint64_t __attribute__((vector_size(sizeof(uint64_t) * numberOfStates))) *nonces) noexcept;

// SipRound
template<const unsigned int numberOfStates> static inline void sipRound(uint64_t __attribute__((vector_size(sizeof(uint64_t) * numberOfStates))) states[SIPHASH_KEYS_SIZE]) noexcept;


// Supporting function implementation

// SipHash-2-4
template<const unsigned int numberOfStates> void sipHash24(uint64_t __attribute__((vector_size(sizeof(uint64_t) * numberOfStates))) *uvs, const uint64_t __attribute__((vector_size(sizeof(uint64_t) * SIPHASH_KEYS_SIZE))) &keys, const uint64_t __attribute__((vector_size(sizeof(uint64_t) * numberOfStates))) *nonces) noexcept {

	// Initialize states
	uint64_t __attribute__((vector_size(sizeof(uint64_t) * numberOfStates))) states[SIPHASH_KEYS_SIZE] = {};
	
	// Set states to keys
	for(uint_fast8_t i = 0; i < SIPHASH_KEYS_SIZE; ++i) {
		states[i] += keys[i];
	}
	
	// Perform hash on states
	states[3] ^= *nonces;
	sipRound<numberOfStates>(states);
	sipRound<numberOfStates>(states);
	states[0] ^= *nonces;
	states[2] ^= 255;
	sipRound<numberOfStates>(states);
	sipRound<numberOfStates>(states);
	sipRound<numberOfStates>(states);
	sipRound<numberOfStates>(states);
	
	// Get UVs from states
	*uvs = (states[0] ^ states[1] ^ states[2] ^ states[3]) & EDGE_MASK;
}

// SipRound
template<const unsigned int numberOfStates> void sipRound(uint64_t __attribute__((vector_size(sizeof(uint64_t) * numberOfStates))) states[SIPHASH_KEYS_SIZE]) noexcept {

	// Perform SipRound on states
	states[0] += states[1];
	states[2] += states[3];
	states[1] = (states[1] << 13) | (states[1] >> (64 - 13));
	states[3] = (states[3] << 16) | (states[3] >> (64 - 16));
	states[1] ^= states[0];
	states[3] ^= states[2];
	states[0] = (states[0] << 32) | (states[0] >> (64 - 32));
	states[2] += states[1];
	states[0] += states[3];
	states[1] = (states[1] << 17) | (states[1] >> (64 - 17));
	states[3] = (states[3] << SIP_ROUND_ROTATION) | (states[3] >> (64 - SIP_ROUND_ROTATION));
	states[1] ^= states[2];
	states[3] ^= states[0];
	states[2] = (states[2] << 32) | (states[2] >> (64 - 32));
}


#endif

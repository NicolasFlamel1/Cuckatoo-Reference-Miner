// Header guard
#ifndef BITMAP_H
#define BITMAP_H


// Header files
using namespace std;


// Constants

// Bitmap unit width
#define BITMAP_UNIT_WIDTH (sizeof(uint64_t) * BITS_IN_A_BYTE)


// Classes

// Bitmap class
template<const uint64_t size> class Bitmap final {

	// Public
	public:
	
		// Constructor
		inline explicit Bitmap() noexcept;
		
		// Get buffer
		inline uint64_t *getBuffer() const noexcept;
		
		// Set bit
		inline void setBit(const uint64_t index) noexcept;
		
		// Clear bit
		inline void clearBit(const uint64_t index) noexcept;
		
		// Is bit set
		inline bool isBitSet(const uint64_t index) const noexcept;
		
		// Set all bits
		inline void setAllBits() noexcept;
		
		// Clear all bits
		inline void clearAllBits() noexcept;
	
	// Private
	private:
	
		// Buffer
		unique_ptr<uint64_t[]> buffer;
};


// Supporting function implementation

// Constructor
template<const uint64_t size> Bitmap<size>::Bitmap() noexcept :

	// Create buffer
	buffer(make_unique<uint64_t[]>(size / BITMAP_UNIT_WIDTH))
{

	// Throw error is size is invalid
	static_assert(size && size % BITMAP_UNIT_WIDTH == 0, "Bitmap's size is invalid");
	
	// Clear buffer
	fill(buffer.get(), buffer.get() + size / BITMAP_UNIT_WIDTH, 0);
}

// Get buffer
template<const uint64_t size> uint64_t *Bitmap<size>::getBuffer() const noexcept {

	// Return buffer
	return buffer.get();
}

// Set bit
template<const uint64_t size> void Bitmap<size>::setBit(const uint64_t index) noexcept {

	// Set bit in buffer
	buffer[index / BITMAP_UNIT_WIDTH] |= static_cast<uint64_t>(1) << (index % BITMAP_UNIT_WIDTH);
}

// Clear bit
template<const uint64_t size> void Bitmap<size>::clearBit(const uint64_t index) noexcept {

	// Clear bit in buffer
	buffer[index / BITMAP_UNIT_WIDTH] &= ~(static_cast<uint64_t>(1) << (index % BITMAP_UNIT_WIDTH));
}

// Is bit set
template<const uint64_t size> bool Bitmap<size>::isBitSet(const uint64_t index) const noexcept {

	// Return if bit is set in buffer
	return buffer[index / BITMAP_UNIT_WIDTH] & (static_cast<uint64_t>(1) << (index % BITMAP_UNIT_WIDTH));
}

// Set all bits
template<const uint64_t size> void Bitmap<size>::setAllBits() noexcept {

	// Set all bits in buffer
	fill(buffer.get(), buffer.get() + size / BITMAP_UNIT_WIDTH, UINT64_MAX);
}

// Clear all bits
template<const uint64_t size> void Bitmap<size>::clearAllBits() noexcept {

	// Clear all bits in buffer
	fill(buffer.get(), buffer.get() + size / BITMAP_UNIT_WIDTH, 0);
}


#endif

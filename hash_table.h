// Header guard
#ifndef HASH_TABLE_H
#define HASH_TABLE_H


// Header files
using namespace std;


// Classes

// Hash table class
template<typename ValueType, const uint32_t size> class HashTable final {

	// Public
	public:
	
		// Constructor
		inline explicit HashTable() noexcept;
		
		// Bool operator
		inline explicit operator bool() const noexcept;
		
		// Set unique
		inline void setUnique(const uint32_t key, const ValueType *value) noexcept;
		
		// Set unique and get index
		inline uint32_t setUniqueAndGetIndex(const uint32_t key, const ValueType *value) noexcept;
		
		// Replace
		inline const ValueType *replace(const uint32_t key, const ValueType *value) noexcept;
		
		// Remove most recent set unique
		inline void removeMostRecentSetUique(const uint32_t index) noexcept;
		
		// Clear
		inline void clear() noexcept;
		
		// Contains
		inline bool contains(const uint32_t key) const noexcept;
		
		// Get
		inline const ValueType *get(const uint32_t key) const noexcept;
		
		// Get values
		inline void getValues(ValueType *values) const noexcept;
	
	// Private
	private:
	
		// Hash table entry structure
		struct HashTableEntry {
		
			// Key
			uint32_t key;
			
			// Value
			const ValueType *value;
		};
		
		// Entries
		unique_ptr<HashTableEntry[]> entries;
};


// Supporting function implementation

// Constructor
template<typename ValueType, const uint32_t size> HashTable<ValueType, size>::HashTable() noexcept :

	// Create entries
	entries(new(nothrow) HashTableEntry[bitCeilingConstantExpression(size + 1)])
{

	// Throw error if size is invalid
	static_assert(size <= bitCeilingConstantExpression(UINT32_MAX >> 1) - 1, "Hash table's size is invalid");
	
	// Check if creating entries was successful
	if(entries) {
	
		// Clear entries
		memset(entries.get(), 0, sizeof(HashTableEntry) * bitCeilingConstantExpression(size + 1));
	}
}

// Bool operator
template<typename ValueType, const uint32_t size> HashTable<ValueType, size>::operator bool() const noexcept {

	// Return if creating entries was successful
	return entries.get();
}

// Set unique
template<typename ValueType, const uint32_t size> void HashTable<ValueType, size>::setUnique(const uint32_t key, const ValueType *value) noexcept {

	// Get key's index
	uint32_t index = key % bitCeilingConstantExpression(size + 1);
	
	// Loop while entry at index exists
	while(entries[index].value) {
	
		// Check if index isn't for the last entry
		if(index != bitCeilingConstantExpression(size + 1) - 1) {
		
			// Increment index
			++index;
		}
		
		// Otherwise
		else {
		
			// Set index to the first entry
			index = 0;
		}
	}
	
	// Set entry at index to the value
	entries[index] = {key, value};
}

// Set unique and get index
template<typename ValueType, const uint32_t size> uint32_t HashTable<ValueType, size>::setUniqueAndGetIndex(const uint32_t key, const ValueType *value) noexcept {

	// Get key's index
	uint32_t index = key % bitCeilingConstantExpression(size + 1);
	
	// Loop while entry at index exists
	while(entries[index].value) {
	
		// Check if index isn't for the last entry
		if(index != bitCeilingConstantExpression(size + 1) - 1) {
		
			// Increment index
			++index;
		}
		
		// Otherwise
		else {
		
			// Set index to the first entry
			index = 0;
		}
	}
	
	// Set entry at index to the value
	entries[index] = {key, value};
	
	// Return index
	return index;
}

// Replace
template<typename ValueType, const uint32_t size> const ValueType *HashTable<ValueType, size>::replace(const uint32_t key, const ValueType *value) noexcept {

	// Get key's index
	uint32_t index = key % bitCeilingConstantExpression(size + 1);
	
	// Loop while entry at index exists
	while(entries[index].value) {
	
		// Check if entry has the same key
		if(entries[index].key == key) {
		
			// Get current value
			const ValueType *currentValue = entries[index].value;
			
			// Set entry at index to the value
			entries[index].value = value;
			
			// Return current value
			return currentValue;
		}
		
		// Check if index isn't for the last entry
		if(index != bitCeilingConstantExpression(size + 1) - 1) {
		
			// Increment index
			++index;
		}
		
		// Otherwise
		else {
		
			// Set index to the first entry
			index = 0;
		}
	}
	
	// Set entry at index to the value
	entries[index] = {key, value};
	
	// Return nothing
	return nullptr;
}

// Remove most recent set unique
template<typename ValueType, const uint32_t size> void HashTable<ValueType, size>::removeMostRecentSetUique(const uint32_t index) noexcept {

	// Clear entry at index
	entries[index].value = nullptr;
}

// Clear
template<typename ValueType, const uint32_t size> void HashTable<ValueType, size>::clear() noexcept {

	// Clear entries
	memset(entries.get(), 0, sizeof(HashTableEntry) * bitCeilingConstantExpression(size + 1));
}

// Contains
template<typename ValueType, const uint32_t size> bool HashTable<ValueType, size>::contains(const uint32_t key) const noexcept {

	// Go through all existing entries starting at the key's index
	for(uint32_t index = key % bitCeilingConstantExpression(size + 1); entries[index].value;) {
	
		// Check if entry has the same key
		if(entries[index].key == key) {
		
			// Return true
			return true;
		}
		
		// Check if index isn't for the last entry
		if(index != bitCeilingConstantExpression(size + 1) - 1) {
		
			// Increment index
			++index;
		}
		
		// Otherwise
		else {
		
			// Set index to the first entry
			index = 0;
		}
	}
	
	// Return false
	return false;
}

// Get
template<typename ValueType, const uint32_t size> const ValueType *HashTable<ValueType, size>::get(const uint32_t key) const noexcept {

	// Go through all existing entries starting at the key's index
	for(uint32_t index = key % bitCeilingConstantExpression(size + 1); entries[index].value;) {
	
		// Check if entry has the same key
		if(entries[index].key == key) {
		
			// Return entry's value
			return entries[index].value;
		}
		
		// Check if index isn't for the last entry
		if(index != bitCeilingConstantExpression(size + 1) - 1) {
		
			// Increment index
			++index;
		}
		
		// Otherwise
		else {
		
			// Set index to the first entry
			index = 0;
		}
	}
	
	// Return nothing
	return nullptr;
}

// Get values
template<typename ValueType, const uint32_t size> void HashTable<ValueType, size>::getValues(ValueType *values) const noexcept {

	// Go through all entries
	for(uint32_t index = 0, valuesIndex = 0; index < bitCeilingConstantExpression(size + 1); ++index) {
	
		// Check if entry exists
		if(entries[index].value) {
		
			// Set value in values
			values[valuesIndex++] = *entries[index].value;
		}
	}
}


#endif

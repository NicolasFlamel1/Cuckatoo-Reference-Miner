// Header guard
#ifndef MAIN_H
#define MAIN_H


// Header files
#include <functional>
#include <streambuf>

// Check if using Android
#ifdef __ANDROID__

	// Header files
	#include <jni.h>
#endif

using namespace std;


// Classes

// Redirect cout class
class RedirectCout final : public streambuf {

	// Public
	public:
	
		// Constructor
		explicit RedirectCout(const function<void(const char)> &callback) noexcept;
		
		// Destructor
		~RedirectCout() noexcept;
		
	// Private
	private:
	
		// Overflow
		traits_type::int_type overflow(const traits_type::int_type character) noexcept override final;
		
		// Callback
		const function<void(const char)> &callback;
		
		// Buffer
		streambuf *buffer;
};


// Function prototypes

// Start miner
bool startMiner(const int argc, char *argv[]) noexcept;

// Stop miner
void stopMiner() noexcept;
	
// Check if using Android
#ifdef __ANDROID__

	// Start miner
	extern "C" JNIEXPORT jboolean JNICALL Java_com_Cuckatoo_1Reference_1Miner_MainActivity_startMiner(JNIEnv *environment, const jobject object, const jint argc, const jobjectArray argv) noexcept;
	
	// Stop miner
	extern "C" JNIEXPORT void JNICALL Java_com_Cuckatoo_1Reference_1Miner_MainActivity_stopMiner(const JNIEnv *environment, const jobject object) noexcept;
#endif


#endif

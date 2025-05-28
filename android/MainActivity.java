// Package
package com.Cuckatoo_Reference_Miner;


// Imports
import android.app.Activity;
import android.os.Bundle;


// Classes

// Main activity class
public final class MainActivity extends Activity {

	// Static
	static {
	
		// Load library
		System.loadLibrary("Cuckatoo_Reference_Miner");
	}
	
	// Start miner
	private native boolean startMiner(int argc, String[] argv);
	
	// Stop miner
	private native void stopMiner();
	
	// On create
	@Override protected void onCreate(Bundle savedInstanceState) {
	
		// Call parent function
		super.onCreate(savedInstanceState);
		
		// Set arguments
		String[] argv = {this.getClass().getCanonicalName()};
		
		// Start miner with arguments
		startMiner(argv.length, argv);
	}
}

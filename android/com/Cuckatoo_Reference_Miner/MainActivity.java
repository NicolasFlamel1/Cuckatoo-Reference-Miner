// Package
package com.Cuckatoo_Reference_Miner;


// Imports
import android.app.Activity;
import android.content.Context;
import android.graphics.Color;
import android.graphics.Insets;
import android.graphics.Rect;
import android.graphics.Typeface;
import android.Manifest;
import android.os.Build;
import android.os.Bundle;
import android.text.InputType;
import android.text.method.ScrollingMovementMethod;
import android.text.PrecomputedText;
import android.view.Gravity;
import android.view.inputmethod.EditorInfo;
import android.view.inputmethod.InputMethodManager;
import android.view.KeyEvent;
import android.view.MotionEvent;
import android.view.View;
import android.view.ViewGroup.MarginLayoutParams;
import android.view.WindowInsets;
import android.widget.ArrayAdapter;
import android.widget.Button;
import android.widget.EditText;
import android.widget.LinearLayout;
import android.widget.Spinner;
import android.widget.TextView;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.nio.charset.StandardCharsets;
import java.util.ArrayList;
import org.json.JSONObject;


// Classes

// Main activity class
public final class MainActivity extends Activity {

	// Set library loaded to true
	static private boolean libraryLoaded = true;
	
	// Static
	static {
	
		// Try
		try {
		
			// Load library
			System.loadLibrary("Cuckatoo_Reference_Miner");
		}
		
		// Catch errors
		catch(final Throwable error) {
		
			// Set library loaded to false
			libraryLoaded = false;
		}
	}
	
	// Prepare miner
	native private void prepareMiner();
	
	// Start miner
	native private boolean startMiner(final String[] argv);
	
	// Stop miner
	native private void stopMiner();
	
	// On create
	@Override protected final void onCreate(final Bundle savedInstanceState) {
	
		// Call parent function
		super.onCreate(savedInstanceState);
		
		// Create layout
		final LinearLayout layout = new LinearLayout(this);
		
		// Configure layout
		setContentView(layout);
		layout.setOrientation(LinearLayout.VERTICAL);
		
		// Check if API level is at least Vanilla Ice Cream
		if(Build.VERSION.SDK_INT >= Build.VERSION_CODES.VANILLA_ICE_CREAM) {
		
			// Layout on apply window insets
			layout.setOnApplyWindowInsetsListener((final View view, final WindowInsets insets) -> {
			
				// Check if getting system bar insets was successful
				final Insets systemBarInsets = insets.getInsets(WindowInsets.Type.systemBars());
				if(systemBarInsets != null) {
				
					// Check if getting view's layout parameters was successful
					final MarginLayoutParams layoutParameters = (MarginLayoutParams)view.getLayoutParams();
					if(layoutParameters != null) {
					
						// Set layout parameter's to use system bar insets as margins
						layoutParameters.setMargins(systemBarInsets.left, systemBarInsets.top, systemBarInsets.right, systemBarInsets.bottom);
						
						// Set view's layout parameters
						view.setLayoutParams(layoutParameters);
					}
				}
				
				// Return consumed
				return WindowInsets.CONSUMED;
			});
		}
		
		// Check if loading library failed
		if(!libraryLoaded) {
		
			// Create message
			final TextView message = new TextView(this);
			
			// Configure message
			layout.addView(message, new LinearLayout.LayoutParams(LinearLayout.LayoutParams.MATCH_PARENT, LinearLayout.LayoutParams.MATCH_PARENT, 1));
			message.setText("Device isn't compatible");
			message.setGravity(Gravity.CENTER);
			
			// Return
			return;
		}
		
		// Create stratum server input
		stratumServerInput = new EditText(this);
		
		// Configure stratum server input
		layout.addView(stratumServerInput);
		stratumServerInput.setHint("Stratum Server");
		stratumServerInput.setSingleLine();
		stratumServerInput.setImeOptions(EditorInfo.IME_ACTION_NEXT);
		stratumServerInput.setInputType(InputType.TYPE_CLASS_TEXT | InputType.TYPE_TEXT_VARIATION_URI);
		
		// Create username input
		usernameInput = new EditText(this);
		
		// Configure username input
		layout.addView(usernameInput);
		usernameInput.setHint("Username");
		usernameInput.setSingleLine();
		usernameInput.setImeOptions(EditorInfo.IME_ACTION_NEXT);
		
		// Create password input
		passwordInput = new EditText(this);
		
		// Configure password input
		layout.addView(passwordInput);
		passwordInput.setHint("Password");
		passwordInput.setSingleLine();
		passwordInput.setImeOptions(EditorInfo.IME_ACTION_DONE);
		passwordInput.setInputType(InputType.TYPE_CLASS_TEXT | InputType.TYPE_TEXT_VARIATION_PASSWORD);
		passwordInput.setOnEditorActionListener((final TextView view, final int actionId, final KeyEvent event) -> {
		
			// Check if done was pressed
			if(actionId == EditorInfo.IME_ACTION_DONE) {
			
				// Clear focus
				passwordInput.clearFocus();
			}
			
			// Return false
			return false;
		});
		
		// Create trimming type selection
		trimmingTypeSelection = new Spinner(this, Spinner.MODE_DIALOG);
		
		// Configure trimming type selection
		layout.addView(trimmingTypeSelection);
		final String[] trimmingTypes = {"Mean Trimming", "Slean Then Mean Trimming", "Slean Trimming", "Lean Trimming"};
		final ArrayAdapter<String> trimmingTypesAdapter = new ArrayAdapter<String>(this, android.R.layout.simple_spinner_item, trimmingTypes);
		trimmingTypesAdapter.setDropDownViewResource(android.R.layout.select_dialog_singlechoice);
		trimmingTypeSelection.setAdapter(trimmingTypesAdapter);
		trimmingTypeSelection.setPrompt("Trimming Type");
		
		// Create button
		final Button button = new Button(this);
		
		// Configure button
		layout.addView(button);
		button.setText("Start Miner");
		button.setAllCaps(false);
		button.setOnClickListener((final View view) -> {
		
			// Disable button
			button.setEnabled(false);
			
			// Check if button is to start miner
			if(button.getText().equals("Start Miner")) {
			
				// Disable inputs
				stratumServerInput.setEnabled(false);
				usernameInput.setEnabled(false);
				passwordInput.setEnabled(false);
				trimmingTypeSelection.setEnabled(false);
				
				// Create arguments
				final ArrayList<String> arguments = new ArrayList<String>();
				arguments.add(getClass().getCanonicalName());
				
				// Check if stratum server exists
				if(!stratumServerInput.getText().toString().isEmpty()) {
				
					// Append stratum server arguments to list
					arguments.add("--stratum_server_address");
					arguments.add(stratumServerInput.getText().toString());
				}
				
				// Check if username exists
				if(!usernameInput.getText().toString().isEmpty()) {
				
					// Append username arguments to list
					arguments.add("--stratum_server_username");
					arguments.add(usernameInput.getText().toString());
				}
				
				// Check if password exists
				if(!passwordInput.getText().toString().isEmpty()) {
				
					// Append password arguments to list
					arguments.add("--stratum_server_password");
					arguments.add(passwordInput.getText().toString());
				}
				
				// Check selected trimming type
				switch(trimmingTypeSelection.getSelectedItem().toString()) {
				
					// Mean trimming
					case "Mean Trimming":
					
						// Append mean trimming argument to list
						arguments.add("--mean_trimming");
						
						// Break
						break;
						
					// Slean then mean trimming
					case "Slean Then Mean Trimming":
					
						// Append slean then mean trimming argument to list
						arguments.add("--slean_then_mean_trimming");
						
						// Break
						break;
						
					// Slean trimming
					case "Slean Trimming":
					
						// Append slean trimming argument to list
						arguments.add("--slean_trimming");
						
						// Break
						break;
						
					// Lean trimming
					case "Lean Trimming":
					
						// Append lean trimming argument to list
						arguments.add("--lean_trimming");
						
						// Break
						break;
				}
				
				// Get arguments as an array
				final String[] argv = new String[arguments.size()];
				arguments.toArray(argv);
				
				// Prepare miner
				prepareMiner();
				
				// Set button's text to stop miner
				button.setText("Stop Miner");
				
				// Enable button
				button.setEnabled(true);
				
				// Create thread
				new Thread(() -> {
				
					// Start miner with arguments
					startMiner(argv);
					
					// Run on the UI thread
					runOnUiThread(() -> {
					
						// Set button's text to start miner
						button.setText("Start Miner");
						
						// Enable button
						button.setEnabled(true);
						
						// Enable inputs
						stratumServerInput.setEnabled(true);
						usernameInput.setEnabled(true);
						passwordInput.setEnabled(true);
						trimmingTypeSelection.setEnabled(true);
					});
					
				}).start();
			}
			
			// Otherwise
			else {
			
				// Stop miner
				stopMiner();
			}
		});
		
		// Create text view
		textView = new TextView(this);
		
		// Configure text view
		layout.addView(textView, new LinearLayout.LayoutParams(LinearLayout.LayoutParams.MATCH_PARENT, LinearLayout.LayoutParams.MATCH_PARENT, 1));
		textView.setBackgroundColor(Color.BLACK);
		textView.setTextColor(Color.WHITE);
		textView.setTypeface(Typeface.MONOSPACE);
		textView.setMovementMethod(new ScrollingMovementMethod());
		textView.setGravity(Gravity.BOTTOM);
		
		// Get text view's parameters
		textViewParameters = textView.getTextMetricsParams();
		
		// Set text view's number of lines to zero
		textViewNumberOfLines = 0;
		
		// Set text view's text to nothing
		textViewText = "";
		
		// Try
		try {
		
			// Open settings file
			final File file = new File(getFilesDir(), SETTINGS_FILE_NAME);
			final FileInputStream settingsFile = new FileInputStream(file);
			
			// Try
			final byte[] data = new byte[(int)file.length()];
			try {
			
				// Read data from settings file
				settingsFile.read(data);
			}
			
			// Finally
			finally {
			
				// Close settings file
				settingsFile.close();
			}
			
			// Get settings from data
			final JSONObject settings = new JSONObject(new String(data, StandardCharsets.UTF_8));
			
			// Restore stratum server if setting exists
			if(settings.has("Stratum Server")) {
				stratumServerInput.setText(settings.getString("Stratum Server"));
			}
			
			// Restore username if setting exists
			if(settings.has("Username")) {
				usernameInput.setText(settings.getString("Username"));
			}
			
			// Restore password if setting exists
			if(settings.has("Password")) {
				passwordInput.setText(settings.getString("Password"));
			}
			
			// Restore trimming type if setting exists
			if(settings.has("Trimming Type")) {
				trimmingTypeSelection.setSelection(trimmingTypesAdapter.getPosition(settings.getString("Trimming Type")));
			}
		}
		
		// Catch errors
		catch(final Throwable error) {
		
		}
		
		// Request internet permission
		requestPermissions(new String[]{Manifest.permission.INTERNET}, REQUEST_INTERNET_PERMISSION);
	}
	
	// On request permissions result
	@Override public final void onRequestPermissionsResult(final int requestCode, final String[] permissions, final int[] grantResults) {
	
		// Check request code
		switch(requestCode) {
		
			// Request internet permission
			case REQUEST_INTERNET_PERMISSION:
			
				// break
				break;
				
			// Default
			default:
			
				// Call parent function
				super.onRequestPermissionsResult(requestCode, permissions, grantResults);
				
				// Break
				break;
		}
	}
	
	// On pause
	@Override protected final void onPause() {
	
		// call parent function
		super.onPause();
		
		// Check if loading library was successful
		if(libraryLoaded) {
		
			// Try
			try {
			
				// Create settings
				final JSONObject settings = new JSONObject();
				
				// Add stratum server to settings
				settings.put("Stratum Server", stratumServerInput.getText().toString());
				
				// Add username to settings
				settings.put("Username", usernameInput.getText().toString());
				
				// Add password to settings
				settings.put("Password", passwordInput.getText().toString());
				
				// Add trimming type to settings
				settings.put("Trimming Type", trimmingTypeSelection.getSelectedItem().toString());
				
				// Create settings file
				final FileOutputStream settingsFile = openFileOutput(SETTINGS_FILE_NAME, Context.MODE_PRIVATE);
				
				// Try
				try {
				
					// Write settings to settings file
					settingsFile.write(settings.toString().getBytes(StandardCharsets.UTF_8));
				}
				
				// Finally
				finally {
				
					// Close settings file
					settingsFile.close();
				}
			}
			
			// Catch errors
			catch(final Throwable error) {
			
			}
		}
	}
	
	// Dispatch touch event
	@Override public final boolean dispatchTouchEvent(final MotionEvent event) {
	
		// Check if start of touch
		if(event != null && event.getAction() == MotionEvent.ACTION_DOWN) {
		
			// Check if an input has focus
			final View view = getCurrentFocus();
			if(view != null && view instanceof EditText) {
			
				// Check if not touching the input with focus
				final Rect rectangle = new Rect();
				view.getGlobalVisibleRect(rectangle);
				if(!rectangle.contains((int)event.getRawX(), (int)event.getRawY())) {
				
					// Clear focus
					view.clearFocus();
					
					// Hide keyboard
					((InputMethodManager)getSystemService(Context.INPUT_METHOD_SERVICE)).hideSoftInputFromWindow(view.getWindowToken(), 0);
				}
			}
		}
		
		// Return calling parent function
		return super.dispatchTouchEvent(event);
	}
	
	// Append to text view
	private void appendToTextView(final String text) {
	
		// Check if text exists
		if(text != null) {
		
			// Append text to the text view's text
			textViewText += text;
			
			// Go through all lines in the text
			for(int i = text.indexOf('\n'); i != -1; i = text.indexOf('\n', i + 1)) {
			
				// Increment text view's number of lines
				++textViewNumberOfLines;
			}
			
			// Loop while text view's number of lines is greater than the max number of lines
			int lineIndex = 0;
			while(textViewNumberOfLines > MAX_NUMBER_OF_TEXT_VIEW_LINES) {
			
				// Get next line's index
				lineIndex = textViewText.indexOf('\n', lineIndex) + 1;
				
				// Decrement text view's number of lines
				--textViewNumberOfLines;
			}
			
			// Remove text view's starting lines
			textViewText = textViewText.substring(lineIndex);
			
			// Precompute text view's text
			final PrecomputedText precomputedText = PrecomputedText.create(textViewText, textViewParameters);
			
			// Run on the UI thread
			runOnUiThread(() -> {
			
				// Set text view's text to the precomputed text
				textView.setText(precomputedText);
			});
		}
	}
	
	// Settings file name
	static private final String SETTINGS_FILE_NAME = "settings";
	
	// Max number of text view lines
	static private final int MAX_NUMBER_OF_TEXT_VIEW_LINES = 200;
	
	// Request internet permission
	static private final int REQUEST_INTERNET_PERMISSION = 1;
	
	// Stratum server input
	private EditText stratumServerInput;
	
	// Username input
	private EditText usernameInput;
	
	// Password input
	private EditText passwordInput;
	
	// Trimming type selection
	private Spinner trimmingTypeSelection;
	
	// Text view
	private TextView textView;
	
	// Text view parameters
	private PrecomputedText.Params textViewParameters;
	
	// Text view number of lines
	private int textViewNumberOfLines;
	
	// Text view text
	private String textViewText;
}

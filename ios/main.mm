// Header files
#include <UIKit/UIKit.h>
#include "../main.h"


// Classes

// Picker view interface
@interface PickerView : UIPickerView

@end

// Picker view implementation
@implementation PickerView

	// Hit test
	-(UIView *) hitTest:(const CGPoint) point withEvent:(UIEvent *) event {
	
		// Go through all windows
		for(const UIWindow *window in [UIApplication sharedApplication].windows) {
		
			// Check if window is key window
			if(window.isKeyWindow) {
			
				// End editing for window
				[window.rootViewController.view endEditing:YES];
				
				// Break
				break;
			}
		}
		
		// Return calling parent function
		return [super hitTest:point withEvent:event];
	}
@end

// View controller interface
@interface ViewController : UIViewController <UITextFieldDelegate, UIPickerViewDelegate, UIPickerViewDataSource>

@end

// View controller implementation
@implementation ViewController

	// Settings key
	static NSString *const SETTINGS_KEY = @TO_STRING(NAME) @"_settings";
	
	// Max number of text view lines
	static const NSUInteger MAX_NUMBER_OF_TEXT_VIEW_LINES = 200;
	
	// Stratum server input
	UITextField *stratumServerInput;
	
	// Username input
	UITextField *usernameInput;
	
	// Password input
	UITextField *passwordInput;
	
	// Trimming type selection
	PickerView *trimmingTypeSelection;
	
	// Button
	UIButton *button;
	
	// Text view
	UITextView *textView;
	
	// Text view number of lines
	NSUInteger textViewNumberOfLines;
	
	// Text view text
	const NSMutableString *textViewText;
	
	// View did load
	-(void) viewDidLoad {
	
		// Call parent function
		[super viewDidLoad];
		
		// Get settings
		const NSArray *settings = [[NSUserDefaults standardUserDefaults] objectForKey:SETTINGS_KEY];
		
		// Configure view
		self.view.backgroundColor = UIColor.whiteColor;
		
		// Check if creating stratum server input failed
		stratumServerInput = [[UITextField alloc] init];
		if(!stratumServerInput) {
		
			// Exit failure
			exit(EXIT_FAILURE);
		}
		
		// Configure stratum server input
		[self.view addSubview:stratumServerInput];
		stratumServerInput.placeholder = @"Stratum Server";
		stratumServerInput.returnKeyType = UIReturnKeyNext;
		stratumServerInput.keyboardType = UIKeyboardTypeURL;
		stratumServerInput.autocorrectionType = UITextAutocorrectionTypeNo;
		stratumServerInput.autocapitalizationType = UITextAutocapitalizationTypeNone;
		stratumServerInput.borderStyle = UITextBorderStyleRoundedRect;
		stratumServerInput.delegate = self;
		[stratumServerInput addTarget:self action:@selector(saveSettings) forControlEvents:UIControlEventEditingDidEnd];
		stratumServerInput.translatesAutoresizingMaskIntoConstraints = NO;
		[stratumServerInput.leadingAnchor constraintEqualToAnchor:self.view.safeAreaLayoutGuide.leadingAnchor].active = YES;
		[stratumServerInput.trailingAnchor constraintEqualToAnchor:self.view.safeAreaLayoutGuide.trailingAnchor].active = YES;
		[stratumServerInput.topAnchor constraintEqualToAnchor:self.view.safeAreaLayoutGuide.topAnchor].active = YES;
		const CGFloat inputHeight = 50;
		[stratumServerInput.heightAnchor constraintEqualToConstant:inputHeight].active = YES;
		stratumServerInput.text = settings ? [settings objectAtIndex:0] : @"";
		
		// Check if creating username input failed
		usernameInput = [[UITextField alloc] init];
		if(!usernameInput) {
		
			// Exit failure
			exit(EXIT_FAILURE);
		}
		
		// Configure username input
		[self.view addSubview:usernameInput];
		usernameInput.placeholder = @"Username";
		usernameInput.returnKeyType = UIReturnKeyNext;
		usernameInput.autocorrectionType = UITextAutocorrectionTypeNo;
		usernameInput.autocapitalizationType = UITextAutocapitalizationTypeNone;
		usernameInput.borderStyle = UITextBorderStyleRoundedRect;
		usernameInput.delegate = self;
		[usernameInput addTarget:self action:@selector(saveSettings) forControlEvents:UIControlEventEditingDidEnd];
		usernameInput.translatesAutoresizingMaskIntoConstraints = NO;
		[usernameInput.leadingAnchor constraintEqualToAnchor:self.view.safeAreaLayoutGuide.leadingAnchor].active = YES;
		[usernameInput.trailingAnchor constraintEqualToAnchor:self.view.safeAreaLayoutGuide.trailingAnchor].active = YES;
		[usernameInput.topAnchor constraintEqualToAnchor:stratumServerInput.bottomAnchor].active = YES;
		[usernameInput.heightAnchor constraintEqualToConstant:inputHeight].active = YES;
		usernameInput.text = settings ? [settings objectAtIndex:1] : @"";
		
		// Check if creating password input failed
		passwordInput = [[UITextField alloc] init];
		if(!passwordInput) {
		
			// Exit failure
			exit(EXIT_FAILURE);
		}
		
		// Configure password input
		[self.view addSubview:passwordInput];
		passwordInput.placeholder = @"Password";
		passwordInput.returnKeyType = UIReturnKeyDone;
		passwordInput.secureTextEntry = YES;
		passwordInput.autocorrectionType = UITextAutocorrectionTypeNo;
		passwordInput.autocapitalizationType = UITextAutocapitalizationTypeNone;
		passwordInput.borderStyle = UITextBorderStyleRoundedRect;
		passwordInput.delegate = self;
		[passwordInput addTarget:self action:@selector(saveSettings) forControlEvents:UIControlEventEditingDidEnd];
		passwordInput.translatesAutoresizingMaskIntoConstraints = NO;
		[passwordInput.leadingAnchor constraintEqualToAnchor:self.view.safeAreaLayoutGuide.leadingAnchor].active = YES;
		[passwordInput.trailingAnchor constraintEqualToAnchor:self.view.safeAreaLayoutGuide.trailingAnchor].active = YES;
		[passwordInput.topAnchor constraintEqualToAnchor:usernameInput.bottomAnchor].active = YES;
		[passwordInput.heightAnchor constraintEqualToConstant:inputHeight].active = YES;
		passwordInput.text = settings ? [settings objectAtIndex:2] : @"";
		
		// Check if creating trimming type selection failed
		trimmingTypeSelection = [[PickerView alloc] init];
		if(!trimmingTypeSelection) {
		
			// Exit failure
			exit(EXIT_FAILURE);
		}
		
		// Configure trimming type selection
		[self.view addSubview:trimmingTypeSelection];
		trimmingTypeSelection.delegate = self;
		trimmingTypeSelection.dataSource = self;
		trimmingTypeSelection.translatesAutoresizingMaskIntoConstraints = NO;
		[trimmingTypeSelection.leadingAnchor constraintEqualToAnchor:self.view.safeAreaLayoutGuide.leadingAnchor].active = YES;
		[trimmingTypeSelection.trailingAnchor constraintEqualToAnchor:self.view.safeAreaLayoutGuide.trailingAnchor].active = YES;
		[trimmingTypeSelection.topAnchor constraintEqualToAnchor:passwordInput.bottomAnchor].active = YES;
		const CGFloat selectionHeight = 75;
		[trimmingTypeSelection.heightAnchor constraintEqualToConstant:selectionHeight].active = YES;
		[trimmingTypeSelection selectRow:(settings ? [[settings objectAtIndex:3] intValue] : 0) inComponent:0 animated:NO];
		
		// Check if creating button failed
		button = [UIButton buttonWithType:UIButtonTypeSystem];
		if(!button) {
		
			// Exit failure
			exit(EXIT_FAILURE);
		}
		
		// Configure button
		[self.view addSubview:button];
		[button setTitle:@"Start Miner" forState:UIControlStateNormal];
		[button addTarget:self action:@selector(buttonClicked) forControlEvents:UIControlEventTouchUpInside];
		button.translatesAutoresizingMaskIntoConstraints = NO;
		[button.leadingAnchor constraintEqualToAnchor:self.view.safeAreaLayoutGuide.leadingAnchor].active = YES;
		[button.trailingAnchor constraintEqualToAnchor:self.view.safeAreaLayoutGuide.trailingAnchor].active = YES;
		[button.topAnchor constraintEqualToAnchor:trimmingTypeSelection.bottomAnchor].active = YES;
		const CGFloat buttonHeight = 50;
		[button.heightAnchor constraintEqualToConstant:buttonHeight].active = YES;
		
		// Check if creating text view failed
		textView = [[UITextView alloc] init];
		if(!textView) {
		
			// Exit failure
			exit(EXIT_FAILURE);
		}
		
		// Configure text view
		[self.view addSubview:textView];
		textView.backgroundColor = UIColor.blackColor;
		textView.textColor = UIColor.whiteColor;
		textView.font = [UIFont monospacedSystemFontOfSize:14 weight:UIFontWeightRegular];
		textView.editable = NO;
		textView.translatesAutoresizingMaskIntoConstraints = NO;
		[textView.leadingAnchor constraintEqualToAnchor:self.view.safeAreaLayoutGuide.leadingAnchor].active = YES;
		[textView.trailingAnchor constraintEqualToAnchor:self.view.safeAreaLayoutGuide.trailingAnchor].active = YES;
		[textView.topAnchor constraintEqualToAnchor:button.bottomAnchor].active = YES;
		[textView.bottomAnchor constraintEqualToAnchor:self.view.safeAreaLayoutGuide.bottomAnchor].active = YES;
		
		// Set text view's number of lines to zero
		textViewNumberOfLines = 0;
		
		// Check if creating text view's text failed
		textViewText = [[NSMutableString alloc] init];
		if(!textViewText) {
		
			// Exit failure
			exit(EXIT_FAILURE);
		}
		
		// Enable orientation changed events
		[[UIDevice currentDevice] beginGeneratingDeviceOrientationNotifications];
		[[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(orientationChanged) name:UIDeviceOrientationDidChangeNotification object:[UIDevice currentDevice]];
	}
	
	// Save settings
	-(void) saveSettings {
	
		// Check if creating settings was successful
		const NSArray *settings = [NSArray arrayWithObjects:stratumServerInput.text, usernameInput.text, passwordInput.text, [@([trimmingTypeSelection selectedRowInComponent:0]) stringValue], nil];
		if(settings) {
		
			// Save settings
			[[NSUserDefaults standardUserDefaults] setObject:settings forKey:SETTINGS_KEY];
			
			// Synchronize settings
			[[NSUserDefaults standardUserDefaults] synchronize];
		}
	}
	
	// Text field should return
	-(BOOL) textFieldShouldReturn:(const UITextField *) textField {
	
		// Check if text field is stratum server input
		if(textField == stratumServerInput) {
		
			// Focus on username input
			[usernameInput becomeFirstResponder];
		}
		
		// Otherwise check if text field is username input
		else if(textField == usernameInput) {
		
			// Focus on password input
			[passwordInput becomeFirstResponder];
		}
		
		// Otherwise check if text field is password input
		else if(textField == passwordInput) {
		
			// Clear focus
			[passwordInput resignFirstResponder];
		}
		
		// Return no
		return NO;
	}
	
	// Touches began
	-(void) touchesBegan:(const NSSet *) touches withEvent:(const UIEvent *) event {
	
		// End editing
		[self.view endEditing:YES];
	}
	
	// Number of components in picker view
	-(NSInteger) numberOfComponentsInPickerView:(const UIPickerView *) pickerView {
	
		// Return one
		return 1;
	}
	
	// Picker view number of rows in components
	-(NSInteger) pickerView:(const UIPickerView *) pickerView numberOfRowsInComponent:(const NSInteger) component {
	
		// Return four
		return 4;
	}
	
	// Picker view title for row
	-(NSString *) pickerView:(const UIPickerView *) pickerView titleForRow:(const NSInteger) row forComponent:(const NSInteger) component {
	
		// Check row
		switch(row) {
		
			// Zero
			case 0:
			
				// Return row's title
				return @"Mean Trimming";
				
			// One
			case 1:
			
				// Return row's title
				return @"Slean Then Mean Trimming";
				
			// Two
			case 2:
			
				// Return row's title
				return @"Slean Trimming";
				
			// Three or default
			case 3:
			default:
			
				// Return row's title
				return @"Lean Trimming";
		}
	}
	
	// Picker view did select row
	-(void) pickerView:(const UIPickerView *) pickerView didSelectRow:(const NSInteger) row inComponent:(const NSInteger) component {
	
		// Save settings
		[self saveSettings];
	}
	
	// Button clicked
	-(void) buttonClicked {
	
		// Disable button
		button.enabled = NO;
		
		// Check if button is to start miner
		if([button.currentTitle isEqualToString:@"Start Miner"]) {
		
			// Disable inputs
			stratumServerInput.enabled = NO;
			usernameInput.enabled = NO;
			passwordInput.enabled = NO;
			trimmingTypeSelection.userInteractionEnabled = NO;
			trimmingTypeSelection.alpha = 0.5;
			
			// Check if creating arguments failed
			const NSMutableArray *arguments = [[NSMutableArray alloc] init];
			if(!arguments) {
			
				// Exit failure
				exit(EXIT_FAILURE);
			}
			
			// Append program's name to list
			[arguments addObject:@TO_STRING(NAME)];
			
			// Check if stratum server exists
			if(stratumServerInput.text.length) {
			
				// Append stratum server arguments to list
				[arguments addObject:@"--stratum_server_address"];
				[arguments addObject:stratumServerInput.text];
			}
			
			// Check if username exists
			if(usernameInput.text.length) {
			
				// Append username arguments to list
				[arguments addObject:@"--stratum_server_username"];
				[arguments addObject:usernameInput.text];
			}
			
			// Check if password exists
			if(passwordInput.text.length) {
			
				// Append password arguments to list
				[arguments addObject:@"--stratum_server_password"];
				[arguments addObject:passwordInput.text];
			}
			
			// Check selected trimming type index
			switch([trimmingTypeSelection selectedRowInComponent:0]) {
			
				// Zero
				case 0:
				
					// Append mean trimming argument to list
					[arguments addObject:@"--mean_trimming"];
					
					// Break
					break;
					
				// One
				case 1:
				
					// Append slean then mean trimming argument to list
					[arguments addObject:@"--slean_then_mean_trimming"];
					
					// Break
					break;
					
				// Two
				case 2:
				
					// Append slean trimming argument to list
					[arguments addObject:@"--slean_trimming"];
					
					// Break
					break;
					
				// Three
				case 3:
				
					// Append lean trimming argument to list
					[arguments addObject:@"--lean_trimming"];
					
					// Break
					break;
			}
			
			// Prepare miner
			prepareMiner();
			
			// Set button's text to stop miner
			[button setTitle:@"Stop Miner" forState:UIControlStateNormal];
			
			// Enable button
			button.enabled = YES;
			
			// Start miner thread
			[self performSelectorInBackground:@selector(startMinerThread:) withObject:arguments];
		}
		
		// Otherwise
		else {
		
			// Stop miner
			stopMiner();
		}
	}
	
	// Start miner thread
	-(void) startMinerThread:(const NSMutableArray *) arguments {
	
		// Create autorelease pool
		@autoreleasepool {
		
			// Redirect cout
			const RedirectCout redirectCout([](const char character) {
			
				// Create autorelease pool
				@autoreleasepool {
				
					// Check if character isn't a tab
					if(character != '\t') {
					
						// Append character to text view's text
						[textViewText appendFormat:@"%c", character];
						
						// Check if character is a newline
						if(character == '\n') {
						
							// Check if at the max number of text view lines
							if(textViewNumberOfLines == MAX_NUMBER_OF_TEXT_VIEW_LINES) {
							
								// Remove text view's starting line
								[textViewText deleteCharactersInRange:NSMakeRange(0, [textViewText rangeOfString:@"\n"].location + 1)];
							}
							
							// Otherwise
							else {
							
								// Increment text view's number of lines
								++textViewNumberOfLines;
							}
							
							// Check if getting text view's resulting text failed
							NSString *resultingText = [textViewText copy];
							if(!resultingText) {
							
								// Exit failure
								exit(EXIT_FAILURE);
							}
							
							// Run on the UI thread
							dispatch_async(dispatch_get_main_queue(), ^{
							
								// Set text view's text to the resulting text
								textView.text = resultingText;
								
								// Scroll text view to bottom
								[textView setContentOffset:CGPointMake(0, textView.contentSize.height - textView.bounds.size.height) animated:NO];
							});
						}
					}
				}
			});
			
			// Go through all arguments
			char *argv[arguments.count];
			for(NSUInteger i = 0; i < arguments.count; ++i) {
			
				// Get argument as a C string
				argv[i] = const_cast<char *>(((const NSString *)[arguments objectAtIndex:i]).UTF8String);
			}
			
			// Start miner with arguments
			startMiner(arguments.count, argv);
			
			// Run on the UI thread
			dispatch_async(dispatch_get_main_queue(), ^{
			
				// Set button's text to start miner
				[button setTitle:@"Start Miner" forState:UIControlStateNormal];
				
				// Enable button
				button.enabled = YES;
				
				// Enable inputs
				stratumServerInput.enabled = YES;
				usernameInput.enabled = YES;
				passwordInput.enabled = YES;
				trimmingTypeSelection.userInteractionEnabled = YES;
				trimmingTypeSelection.alpha = 1;
			});
		}
	}
	
	// Orientation changed
	-(void) orientationChanged {
	
		// Scroll text view to bottom
		[self scrollTextViewToBottom];
		
		// Scroll text view to bottom again after the orientation is done changing
		[self performSelector:@selector(scrollTextViewToBottom) withObject:nil afterDelay:0];
	}
	
	// Scroll text view to bottom
	-(void) scrollTextViewToBottom {
	
		// Scroll text view to bottom
		[textView setContentOffset:CGPointMake(0, textView.contentSize.height - textView.bounds.size.height) animated:NO];
	}
@end

// Scene delegate interface
@interface SceneDelegate : UIResponder <UIWindowSceneDelegate>

@end

// Scene delegate implementation
@implementation SceneDelegate

	// Initialize window
	@synthesize window;
	
	// Will connect to session
	-(void) scene:(const UIScene *) scene willConnectToSession:(const UISceneSession *) session options:(const UISceneConnectionOptions *) connectionOptions {
	
		// Check if creating window failed
		window = [[UIWindow alloc] initWithWindowScene:(UIWindowScene *)scene];
		if(!window) {
		
			// Exit failure
			exit(EXIT_FAILURE);
		}
		
		// Check if creating window's root view controller failed
		window.rootViewController = [[ViewController alloc] init];
		if(!window.rootViewController) {
		
			// Exit failure
			exit(EXIT_FAILURE);
		}
		
		// Show window
		[window makeKeyAndVisible];
	}
@end

// Application delegate interface
@interface ApplicationDelegate : UIResponder <UIApplicationDelegate>

@end

// Application delegate implementation
@implementation ApplicationDelegate

	// Configuration for connecting scene session
	-(UISceneConfiguration *) application:(const UIApplication *) application configurationForConnectingSceneSession:(const UISceneSession *) connectingSceneSession options:(const UISceneConnectionOptions *) options {
	
		// Check if creating scene configuration failed
		UISceneConfiguration *sceneConfiguration = [[UISceneConfiguration alloc] initWithName:nil sessionRole:connectingSceneSession.role];
		if(!sceneConfiguration) {
		
			// Exit failure
			exit(EXIT_FAILURE);
		}
		
		// Set scene configuration's delegate class
		sceneConfiguration.delegateClass = SceneDelegate.class;
		
		// Return scene configuration
		return sceneConfiguration;
	}
@end


// Main function
int main(const int argc, char *argv[]) {

	// Create autorelease pool
	@autoreleasepool {
	
		// Return application main
		return UIApplicationMain(argc, argv, nil, NSStringFromClass(ApplicationDelegate.class));
	}
}

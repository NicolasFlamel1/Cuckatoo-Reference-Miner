// Header files
#include <UIKit/UIKit.h>
#include "../main.h"

using namespace std;


// View controller interface
@interface ViewController : UIViewController <UITextFieldDelegate>

@end

// View controller implementation
@implementation ViewController

	// text view
	UITextView *textView;
	
	// View did load
	-(void) viewDidLoad {
	
		// Call parent function
		[super viewDidLoad];
		
		// Configure view
		self.view.backgroundColor = UIColor.whiteColor;
		
		// Check if creating stratum server label failed
		UILabel *stratumServerLabel = [[UILabel alloc] init];
		if(!stratumServerLabel) {
		
			// Exit failure
			exit(EXIT_FAILURE);
		}
		
		// Configure stratum server label
		[self.view addSubview:stratumServerLabel];
		stratumServerLabel.text = @"Stratum server";
		stratumServerLabel.translatesAutoresizingMaskIntoConstraints = NO;
		[stratumServerLabel.leadingAnchor constraintEqualToAnchor:self.view.safeAreaLayoutGuide.leadingAnchor].active = YES;
		[stratumServerLabel.trailingAnchor constraintEqualToAnchor:self.view.safeAreaLayoutGuide.trailingAnchor].active = YES;
		[stratumServerLabel.topAnchor constraintEqualToAnchor:self.view.safeAreaLayoutGuide.topAnchor].active = YES;
		const CGFloat labelHeight = 50;
		[stratumServerLabel.heightAnchor constraintEqualToConstant:labelHeight].active = YES;
		
		// Check if creating stratum server text field failed
		UITextField *stratumServerTextField = [[UITextField alloc] init];
		if(!stratumServerTextField) {
		
			// Exit failure
			exit(EXIT_FAILURE);
		}
		
		// Configure stratum server text field
		[self.view addSubview:stratumServerTextField];
		stratumServerTextField.borderStyle = UITextBorderStyleRoundedRect;
		stratumServerTextField.keyboardType = UIKeyboardTypeDefault;
		stratumServerTextField.returnKeyType = UIReturnKeyDone;
		stratumServerTextField.clearButtonMode = UITextFieldViewModeWhileEditing;
		stratumServerTextField.autocorrectionType = UITextAutocorrectionTypeNo;
		stratumServerTextField.contentVerticalAlignment = UIControlContentVerticalAlignmentCenter;
		stratumServerTextField.delegate = self;
		stratumServerTextField.translatesAutoresizingMaskIntoConstraints = NO;
		[stratumServerTextField.leadingAnchor constraintEqualToAnchor:self.view.safeAreaLayoutGuide.leadingAnchor].active = YES;
		[stratumServerTextField.trailingAnchor constraintEqualToAnchor:self.view.safeAreaLayoutGuide.trailingAnchor].active = YES;
		[stratumServerTextField.topAnchor constraintEqualToAnchor:stratumServerLabel.bottomAnchor].active = YES;
		const CGFloat textFieldHeight = 50;
		[stratumServerTextField.heightAnchor constraintEqualToConstant:textFieldHeight].active = YES;
		
		// Check if creating button failed
		UIButton *button = [UIButton buttonWithType:UIButtonTypeSystem];
		if(!button) {
		
			// Exit failure
			exit(EXIT_FAILURE);
		}
		
		// Configure button
		[self.view addSubview:button];
		[button setTitle:@"Start Miner" forState:UIControlStateNormal];
		[button addTarget:self action:@selector(buttonClicked:) forControlEvents:UIControlEventTouchUpInside];
		button.translatesAutoresizingMaskIntoConstraints = NO;
		[button.leadingAnchor constraintEqualToAnchor:self.view.safeAreaLayoutGuide.leadingAnchor].active = YES;
		[button.trailingAnchor constraintEqualToAnchor:self.view.safeAreaLayoutGuide.trailingAnchor].active = YES;
		[button.topAnchor constraintEqualToAnchor:stratumServerTextField.bottomAnchor].active = YES;
		const CGFloat buttonHeight = 100;
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
	}
	
	// Button clicked
	-(void) buttonClicked:(UIButton *) button {
	
		// Disable button
		//button.enabled = NO;
		
		// Clear text view
		//textView.text = @"";
		
		// Start miner thread
		[self performSelectorInBackground:@selector(startMinerThread) withObject:nil];
	}
	
	// Start miner thread
	-(void) startMinerThread {
	
		// Redirect cout
		const RedirectCout redirectCout([](const char character) {
		
			// Run on the UI thread
			dispatch_async(dispatch_get_main_queue(), ^{
			
				// Append character to text view
				textView.text = [textView.text stringByAppendingFormat:@"%c" , character];
				
				// Scroll text view to bottom
				[textView scrollRangeToVisible:NSMakeRange(textView.text.length - 1, 1)];
			});
		});
		
		// Start miner
		startMiner(0, nullptr);
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
	-(void) scene:(UIScene *) scene willConnectToSession:(UISceneSession *) session options:(UISceneConnectionOptions *) connectionOptions {
	
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
	-(UISceneConfiguration *) application:(UIApplication *) application configurationForConnectingSceneSession:(UISceneSession *) connectingSceneSession options:(UISceneConnectionOptions *) options {
	
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
int main(int argc, char *argv[]) {

	// Create autorelease pool
	@autoreleasepool {
	
		// Return application main
		return UIApplicationMain(argc, argv, nil, NSStringFromClass(ApplicationDelegate.class));
	}
}

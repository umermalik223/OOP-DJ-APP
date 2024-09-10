#pragma once

// Ensures that the contents of the file are included only once during compilation. 
// This is a header guard used to prevent multiple inclusions of the same header file, 
// which can cause errors due to redefinition of classes, functions, and variables.
// The #pragma once directive is a modern and convenient way to achieve this, 
// avoiding potential issues that traditional include guards (using #ifndef, #define, and #endif) may have.

// Including the necessary JUCE framework headers and custom classes for audio and GUI components.
#include <JuceHeader.h>                // Main JUCE header that includes all the essential JUCE modules.
#include "DJAudioPlayer.h"             // Custom class for handling audio playback functionalities.
#include "WaveformDisplay.h"           // Custom class for displaying the waveform of the audio.
#include "ZoomedWaveform.h"            // Custom class for showing a zoomed-in view of the waveform.
#include "JogWheel.h"                  // Custom class for the jog wheel, typically used for scrubbing through audio.
#include "CustomLookAndFeel.h"         // Custom look and feel for the GUI components.
#include "Library.h"                   // Custom class for managing the audio library.

// DeckGUI is a class that represents the graphical user interface (GUI) for an audio deck component.
class DeckGUI : public juce::Component,
	public juce::Button::Listener,               // Inherits from Button::Listener to handle button click events.
	public juce::Slider::Listener,               // Inherits from Slider::Listener to handle slider value changes.
	public juce::FileDragAndDropTarget,          // Inherits from FileDragAndDropTarget to handle drag-and-drop events for files.
	public juce::Timer                          // Inherits from Timer to allow periodic updates through timer callbacks.
{
public:
	// Constructor: Initializes the DeckGUI with the required dependencies.
	// Takes a pointer to a DJAudioPlayer, references to AudioFormatManager and AudioThumbnailCache, 
	// a pointer to a ZoomedWaveform, a reference to the Library, and a Colour for theming.
	DeckGUI(DJAudioPlayer* player, juce::AudioFormatManager& formatManagerToUse,
		juce::AudioThumbnailCache& cacheToUse, ZoomedWaveform* _zoomedDisplay,
		Library& _library, juce::Colour _colour);

	// Destructor: Ensures that resources allocated during the lifetime of the DeckGUI are released properly.
	~DeckGUI() override;

private:
	// Paths to sample files for different drum sounds, stored as juce::String. 
	// These paths point to the location of the audio files on the user's system, 
	// which are used for loading drum sounds into the DeckGUI. 
	// The file paths are hardcoded and assume that the files exist in the specified directories. 
	// If the paths are incorrect or the files are missing, the program might fail to load the samples.
	juce::String snareSamplePath = "D:\\OOP\\CM2005 FINAL SUBMISSION OOP\\NewJUCEProject\\NewJUCEProject\\Assets\\effects\\drum.aif";
	juce::String kickSamplePath = "D:\\OOP\\CM2005 FINAL SUBMISSION OOP\\NewJUCEProject\\NewJUCEProject\\Assets\\effects\\kick.wav";
	juce::String clapSamplePath = "D:\\OOP\\CM2005 FINAL SUBMISSION OOP\\NewJUCEProject\\NewJUCEProject\\Assets\\effects\\clap.wav";
	juce::String hiHatSamplePath = "D:\\OOP\\CM2005 FINAL SUBMISSION OOP\\NewJUCEProject\\NewJUCEProject\\Assets\\effects\\hiHat.wav";


	// Four DrawableButton instances for loading and playing different audio samples: kick, snare, hi-hat, and clap.
	// Each button is labeled "Load" and uses the ImageFitted style to ensure that the images associated 
	// with the buttons are scaled to fit the button's dimensions. 
	// These buttons are likely tied to specific audio samples that users can trigger when interacting with the GUI.
	juce::DrawableButton kickButton{ "Load", juce::DrawableButton::ButtonStyle::ImageFitted };
	juce::DrawableButton snareButton{ "Load", juce::DrawableButton::ButtonStyle::ImageFitted };
	juce::DrawableButton hiHatButton{ "Load", juce::DrawableButton::ButtonStyle::ImageFitted };
	juce::DrawableButton clapButton{ "Load", juce::DrawableButton::ButtonStyle::ImageFitted };

	// The paint method is responsible for drawing the component's appearance using the juce::Graphics object.
	// This method is automatically called whenever the component needs to be redrawn. 
	// It is where you define how the DeckGUI component should visually represent itself, 
	// such as drawing shapes, text, images, or other visual elements.
	void paint(juce::Graphics&) override;

	// The resized method is called whenever the component is resized. 
	// It is used to adjust the layout and positioning of child components within the DeckGUI.
	// This method ensures that all elements of the GUI are properly scaled and positioned according to the new size.
	void resized() override;

	// Handles button click events. The buttonClicked method is automatically invoked whenever a button in the DeckGUI is clicked.
	// It determines which button was clicked and performs the corresponding action. 
	// This is a crucial method for responding to user interactions with the GUI, such as starting or stopping playback, 
	// loading samples, or adjusting settings based on button presses.
	void buttonClicked(juce::Button* button) override;

	// CustomLookAndFeel is a class that defines the visual style of the GUI components. 
	// The customLookAndFeel instance here is used to apply a specific look and feel to the buttons and possibly other components within the DeckGUI.
	// This allows for a consistent and unique visual theme that differentiates the DeckGUI from standard JUCE components.
	CustomLookAndFeel customLookAndFeel;

	// Unique pointers to Drawable objects, which are graphical representations of the various button images and states.
	// These pointers manage the lifecycle of the images used in the GUI, ensuring they are properly allocated and deallocated. 
	// The Drawables include images for the stop, play, clap, hi-hat, kick, and snare buttons, as well as hover states for play and load buttons.
	// Each Drawable is associated with a specific button or state, enhancing the interactivity and visual feedback of the GUI.
	std::unique_ptr<juce::Drawable> stopButtonImage;
	std::unique_ptr<juce::Drawable> stopButtonHoverImage;
	std::unique_ptr<juce::Drawable> playButtonImage;
	std::unique_ptr<juce::Drawable> clapButtonImage;
	std::unique_ptr<juce::Drawable> hiHatButtonImage;
	std::unique_ptr<juce::Drawable> kickButtonImage;
	std::unique_ptr<juce::Drawable> snareButtonImage;
	std::unique_ptr<juce::Drawable> playButtonHoverImage;
	std::unique_ptr<juce::Drawable> loadButtonImage;
	std::unique_ptr<juce::Drawable> loadButtonHoverImage;

	// The sliderValueChanged method is called whenever a slider in the DeckGUI is adjusted by the user.
	// This method responds to changes in slider values, which could control aspects like volume, speed, or effects.
	// Implementing this method is essential for making the sliders functional, 
	// allowing the DeckGUI to react dynamically to user input and adjust the audio playback or processing accordingly.
	void sliderValueChanged(juce::Slider* slider) override;
	void highlightSection(juce::Graphics& g, juce::Rectangle<int> section, juce::Colour highlightColour);
	void applyBlurEffect(juce::Graphics& g, juce::Component& component);
	void rotateElement(juce::Graphics& g, juce::Rectangle<float> area, float angleDegrees);
	void drawGradientBackground(juce::Graphics& g, juce::Rectangle<int> area);
	void createCustomBorder(juce::Graphics& g, juce::Component& component, int thickness);
	void animateComponentResize(juce::Component& component, juce::Rectangle<int> targetBounds, int durationMs);
	void drawShadowEffect(juce::Graphics& g, juce::Rectangle<int> area, juce::Colour shadowColour);


	// Determines if the component is interested in the files being dragged over it. 
	// The isInterestedInFileDrag method returns true if the DeckGUI can accept the dragged files, 
	// allowing for drag-and-drop functionality where users can add audio files directly to the deck.
	// This method enhances user experience by simplifying the process of loading tracks into the deck.
	bool isInterestedInFileDrag(const juce::StringArray& files) override;

	// Handles the event when files are dropped onto the component. 
	// The filesDropped method is invoked after a file drag is completed and the files are released within the component's bounds.
	// This method processes the dropped files, such as loading them into the audio player or updating the GUI, 
	// and provides a direct and intuitive way for users to add content to the DeckGUI.
	void filesDropped(const juce::StringArray& files, int x, int y) override;

	// The timerCallback method is part of the Timer class and is called at regular intervals defined by the startTimer method.
	// This method is used for periodic updates, such as refreshing the waveform display, updating playback positions, or handling animations.
	// Using a timer ensures that certain tasks are performed continuously and consistently, 
	// keeping the DeckGUI responsive and up-to-date with the current state of the audio playback or user interaction.
	void timerCallback() override;

	// Loads a track into the deck for playback. 
	// The loadDeck method is a custom function that takes a track object as a parameter and handles the process of loading it into the DeckGUI's audio player.
	// This function is crucial for initializing the playback of new audio content and ensuring the deck is ready for user interaction.
	void loadDeck(track track);

	// Pointers to the Library and DJAudioPlayer instances. 
	// The library pointer is used for managing the collection of audio tracks available to the DeckGUI, 
	// while the player pointer is used to control the playback of audio within the deck. 
	// These pointers allow the DeckGUI to interact with and control the audio playback system and the track library effectively.
	Library* library;
	DJAudioPlayer* player;

	// Buttons for playing and loading audio tracks. 
	// The playButton and loadButton are DrawableButton instances, each associated with a specific function in the DeckGUI. 
	// The playButton is used to start or pause playback, while the loadButton is used to load new audio tracks into the deck. 
	// These buttons are essential for user interaction, providing control over the playback process.
	juce::DrawableButton playButton{ "Play", juce::DrawableButton::ButtonStyle::ImageFitted };
	juce::DrawableButton loadButton{ "Load", juce::DrawableButton::ButtonStyle::ImageFitted };

	// The theme is a juce::Colour object that defines the overall color scheme of the DeckGUI. 
	// This color is used throughout the GUI to create a cohesive and visually appealing interface, 
	// aligning with the design aesthetics or branding of the application. 
	// The use of a single color theme ensures consistency and helps in creating a recognizable user experience.
	juce::Colour theme;

	// Labels and sliders for controlling various audio parameters like volume, speed, and filters.
	// These GUI components allow users to adjust the audio output to their preference, 
	// such as changing the playback speed, adjusting the volume, or applying filters to the audio signal. 
	// Each label corresponds to a specific slider, providing a clear and intuitive interface for manipulating the audio.
	juce::Label volLabel{ "VOLUME", "VOLUME" };
	juce::Slider volSlider{ juce::Slider::SliderStyle::LinearVertical, juce::Slider::TextEntryBoxPosition::NoTextBox };
	juce::Label speedLabel{ "SPEED                CUE POINTS ", "SPEED                CUE POINTS " };
	juce::Slider speedSlider{ juce::Slider::SliderStyle::LinearVertical, juce::Slider::TextEntryBoxPosition::NoTextBox };
	juce::Slider filter{ juce::Slider::SliderStyle::RotaryVerticalDrag, juce::Slider::TextEntryBoxPosition::NoTextBox };
	juce::Slider midBandFilter{ juce::Slider::SliderStyle::RotaryVerticalDrag, juce::Slider::TextEntryBoxPosition::NoTextBox };
	juce::Label mbLabel{ "MID", "MID" };
	juce::Slider highBandFilter{ juce::Slider::SliderStyle::RotaryVerticalDrag, juce::Slider::TextEntryBoxPosition::NoTextBox };
	juce::Label hbLabel{ "HIGH", "HIGH" };
	juce::Slider lowBandFilter{ juce::Slider::SliderStyle::RotaryVerticalDrag, juce::Slider::TextEntryBoxPosition::NoTextBox };
	juce::Label lbLabel{ "LOW", "LOW" };
	juce::Label filterLabel{ "FILTER", "FILTER" };

	// GUI components for waveform visualization and user interaction. 
	// WaveformDisplay, JogWheel, and ZoomedWaveform are custom components that provide visual feedback on the audio's waveform, 
	// allowing users to see and interact with the audio in a more detailed and intuitive way. 
	// These components are essential for DJing or any audio editing application, 
	// where precise control and visualization of the audio are critical.
	WaveformDisplay waveformDisplay;
	JogWheel jogWheel;
	ZoomedWaveform* zoomedDisplay;

	// A collection of waveform displays and other visual components.
	// The displays vector holds pointers to various waveform displays and the jog wheel, 
	// enabling easy access and management of these components. 
	// This vector is used to iterate through the visual components, ensuring they are properly updated and rendered within the DeckGUI.
	std::vector<WaveformDisplay*> displays{ &waveformDisplay, zoomedDisplay, &jogWheel };

	// A collection of buttons and their associated cue points. 
	// The cues vector stores pointers to TextButton instances, each representing a cue point in the audio. 
	// The cueTargets map links each button to a specific playback position (double) and gain value (float), 
	// allowing users to jump to and modify predefined points in the audio. 
	// This feature is particularly useful in DJing and music production, where precise control over cue points is essential.
	std::vector<juce::TextButton*> cues;
	std::map<juce::TextButton*, std::pair<double, float>> cueTargets;

	// Variables for keeping track of the playback position and state of the DeckGUI. 
	// prevPlayerPos stores the last known position of the audio playback, 
	// while canContinue, modeIsPlaying, draggedIndex, flash, counter, and volRMS 
	// are used to manage various states and conditions during playback, such as whether playback can continue, 
	// whether the deck is currently playing, or the RMS volume level. 
	// These variables are critical for maintaining the functionality and responsiveness of the DeckGUI.
	double prevPlayerPos;
	bool canContinue = true;
	bool modeIsPlaying = false;
	int draggedIndex;
	bool flash;
	int counter;
	float volRMS;

	// JUCE's built-in macro to prevent the copying and assigning of instances of this class.
	// The JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR macro defines private copy constructor and assignment operator 
	// to prevent accidental copying of the DeckGUI instance, which could lead to resource management issues and unintended behavior. 
	// It also includes a memory leak detector, which helps identify memory leaks during the development process, 
	// ensuring that all allocated resources are properly managed and deallocated.
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(DeckGUI);
};

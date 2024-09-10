#pragma once

#include <JuceHeader.h>
#include "DJAudioPlayer.h"
#include "DeckGUI.h"
#include "Library.h"
#include "CustomLookAndFeel.h"

// MainComponent is the central component of the application
// It manages audio playback, user interface, and interactions between different components
class MainComponent : public juce::AudioAppComponent, public juce::Slider::Listener, public juce::KeyListener
{
public:
    // Constructor initializes the main component and sets up the user interface
    MainComponent();

    // Destructor cleans up resources when the component is destroyed
    ~MainComponent() override;

    // Handles key press events
    bool keyPressed(const juce::KeyPress& key, Component* originatingComponent) override;

    // Prepares the audio playback with the expected number of samples per block and sample rate
    void prepareToPlay(int samplesPerBlockExpected, double sampleRate) override;

    // Fills the audio buffer with the next block of audio data
    void getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill) override;

    // Releases resources associated with audio playback
    void releaseResources() override;
    void complexFunction();

    // Paints the component's user interface
    void paint(juce::Graphics& g) override;

    // Resizes the component and arranges its child components
    void resized() override;

    // Responds to changes in the slider's value
    void sliderValueChanged(juce::Slider* slider) override;

private:
    // Custom look-and-feel settings for the user interface
    CustomLookAndFeel customLookAndFeel;

    // Library component to manage and display audio files and playlists
    Library library{ formatManager };

    // Caches for audio thumbnails
    juce::AudioThumbnailCache thumbCache{ 100 };

    // Two audio players for simultaneous playback
    DJAudioPlayer player1{ formatManager };
    DJAudioPlayer player2{ formatManager };

    // Mixer source to combine audio from multiple players
    juce::MixerAudioSource mixerSource;

    // Displays for zoomed waveforms of the audio tracks
    ZoomedWaveform zoomedDisplay1{ formatManager, thumbCache, juce::Colours::aqua };
    ZoomedWaveform zoomedDisplay2{ formatManager, thumbCache, juce::Colours::hotpink };

    // Audio format manager to handle different audio formats
    juce::AudioFormatManager formatManager;

    // DeckGUI components for controlling the audio players
    DeckGUI deckGUI1{ &player1, formatManager, thumbCache, &zoomedDisplay1, library, juce::Colours::aqua };
    DeckGUI deckGUI2{ &player2, formatManager, thumbCache, &zoomedDisplay2, library, juce::Colours::hotpink };

    // Crossfader slider to blend audio between the two players
    juce::Slider crossFader{ juce::Slider::SliderStyle::LinearHorizontal, juce::Slider::TextEntryBoxPosition::NoTextBox };

    // Prevent copying and leaking of the MainComponent class
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainComponent)
};

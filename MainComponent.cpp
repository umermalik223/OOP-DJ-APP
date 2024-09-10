#include "MainComponent.h"

// Constructor for MainComponent
MainComponent::MainComponent()
{
    // Set the initial size of the component
    setSize(800, 600);

    // Check if runtime permissions for recording audio are required and if they are granted
    if (juce::RuntimePermissions::isRequired(juce::RuntimePermissions::recordAudio)
        && !juce::RuntimePermissions::isGranted(juce::RuntimePermissions::recordAudio))
    {
        // Request runtime permissions if not already granted
        juce::RuntimePermissions::request(juce::RuntimePermissions::recordAudio,
            [&](bool granted) { setAudioChannels(granted ? 2 : 0, 2); });
    }
    else
    {
        // Set up audio channels if permissions are already granted
        setAudioChannels(2, 2);
    }

    // Add and make visible various components in the UI
    addAndMakeVisible(deckGUI1);
    addAndMakeVisible(deckGUI2);
    addAndMakeVisible(library);
    addAndMakeVisible(zoomedDisplay1);
    addAndMakeVisible(zoomedDisplay2);
    addAndMakeVisible(crossFader);

    // Configure the crossfader slider properties
    crossFader.setRange(-1, 1);  // Set the range of the slider (-1 to 1)
    crossFader.setValue(0);      // Initialize the slider value to 0
    crossFader.addListener(this);  // Add this component as a listener for slider changes

    // Register basic audio formats with the format manager
    formatManager.registerBasicFormats();

    // Set the background colour of the window
    getLookAndFeel().setColour(juce::ResizableWindow::backgroundColourId, juce::Colour::fromRGBA(25, 25, 25, 255));

    // Apply a custom look-and-feel to the crossfader and library components
    crossFader.setLookAndFeel(&customLookAndFeel);
    library.setLookAndFeel(&customLookAndFeel);

    // Add this component as a key listener to handle key events
    library.addKeyListener(this);
}

// Destructor for MainComponent
MainComponent::~MainComponent()
{
    // Shutdown the audio system and release any audio resources
    shutdownAudio();
}

// Prepare the audio playback system before starting playback
void MainComponent::prepareToPlay(int samplesPerBlockExpected, double sampleRate)
{
    // Add input sources to the mixer
    mixerSource.addInputSource(&player1, false);
    mixerSource.addInputSource(&player2, false);

    // Prepare individual players for playback
    player1.prepareToPlay(samplesPerBlockExpected, sampleRate);
    player2.prepareToPlay(samplesPerBlockExpected, sampleRate);
}

// Process audio data for playback
void MainComponent::getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill)
{
    // Pass the audio data through the mixer source
    mixerSource.getNextAudioBlock(bufferToFill);
}

// Release audio resources and clean up
void MainComponent::releaseResources()
{
    // Remove all input sources from the mixer
    mixerSource.removeAllInputs();
    // Release resources for the mixer and players
    mixerSource.releaseResources();
    player1.releaseResources();
    player2.releaseResources();
}

// Paint the component's background and UI elements
void MainComponent::paint(juce::Graphics& g)
{
    // Fill the background with the window's background colour
    g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));

    // Set font and colour for drawing text
    g.setFont(20.0f);
    g.setColour(juce::Colour::fromRGBA(25, 25, 25, 255));

    // Draw the crossfader's background
    g.fillRect(crossFader.getLocalBounds());
}

// Adjust component bounds when resized
void MainComponent::resized()
{
    DBG("MainComponent::resized");

    // Calculate row height for layout
    double rowH = getHeight() / 8;

    // Set bounds for each component in the layout
    zoomedDisplay1.setBounds(0, 0, getWidth(), 75 + getHeight() / 32);
    zoomedDisplay2.setBounds(0, 75 + getHeight() / 32, getWidth(), 75 + getHeight() / 32);
    deckGUI1.setBounds(0, 150 + getHeight() / 16, getWidth() / 2, 300);
    deckGUI2.setBounds(getWidth() / 2, 150 + getHeight() / 16, getWidth() / 2, 300);
    crossFader.setBounds(getWidth() / 2 - 80, 412.5 + getHeight() / 16, 160, 37.5);
    library.setBounds(0, 450 + getHeight() / 16, getWidth(), getHeight() - 450 - getHeight() / 16);
}

void MainComponent::sliderValueChanged(juce::Slider* slider) {
    // Check if the changed slider is the crossfader
    if (slider == &crossFader) {
        double val;  // Variable to store the calculated gain value

        // Determine the crossfader value and adjust gains accordingly
        if (slider->getValue() > 0) {
            // When the crossfader is positive, reduce the gain of player1 and keep player2 at full volume
            val = 1 - slider->getValue();
            player1.setGain(val, false);  // Set the gain for player1
            player2.setGain(1, false);    // Set player2 to full volume (1.0)
        }
        else if (slider->getValue() < 0) {
            // When the crossfader is negative, reduce the gain of player2 and keep player1 at full volume
            val = 1 + slider->getValue();
            player2.setGain(val, false);  // Set the gain for player2
            player1.setGain(1, false);    // Set player1 to full volume (1.0)
        }
    }
}


// Handle key press events
bool MainComponent::keyPressed(const juce::KeyPress& key, juce::Component* originatingComponent) {
    DBG(key.getKeyCode());
    if (key.getKeyCode() == 68) {  // Check if the 'D' key (key code 68) is pressed
        DBG("Delete Match");
        library.deleteItem();  // Call deleteItem on the library component
    }
    return true;  // Return true to indicate that the key event was handled
}
void complexFunction()
{
    // Declare and initialize local variables
    //juce::AudioPlayer audioPlayer;
    juce::TextButton playButton;
    juce::TextButton pauseButton;
    juce::Label statusLabel;
    juce::Slider volumeSlider;
    juce::ComboBox trackList;
    juce::Label trackTitleLabel;
   // juce::ProgressBar progressBar;
    juce::ToggleButton shuffleButton;

    // Setup the play button
    //playButton.setButtonText("Play");
    //playButton.onClick = [this, &audioPlayer]() {
    //    audioPlayer.play();
    //    statusLabel.setText("Playing", juce::dontSendNotification);
    //    playButton.setEnabled(false); // Disable play button when playing
    //    pauseButton.setEnabled(true); // Enable pause button
    //    juce::Logger::writeToLog("Play button clicked");
    //    };

    //// Setup the pause button
    //pauseButton.setButtonText("Pause");
    //pauseButton.onClick = [this, &audioPlayer]() {
    //    audioPlayer.pause();
    //    statusLabel.setText("Paused", juce::dontSendNotification);
    //    playButton.setEnabled(true); // Enable play button
    //    pauseButton.setEnabled(false); // Disable pause button
    //    juce::Logger::writeToLog("Pause button clicked");
    //    };

    // Setup volume slider
    //volumeSlider.setRange(0.0, 1.0); // Volume range from 0 to 1
    //volumeSlider.onValueChange = [this, &audioPlayer]() {
    //    audioPlayer.setVolume(volumeSlider.getValue());
    //    juce::Logger::writeToLog("Volume set to: " + juce::String(volumeSlider.getValue()));
    //    };

    //// Setup track list
    //trackList.addItem("Track 1", 1);
    //trackList.addItem("Track 2", 2);
    //trackList.addItem("Track 3", 3);
    //trackList.onChange = [this, &audioPlayer, &trackTitleLabel]() {
    //    int selectedTrackId = trackList.getSelectedId();
    //    juce::String selectedTrackTitle = trackList.getItemText(selectedTrackId);
    //    trackTitleLabel.setText("Now Playing: " + selectedTrackTitle, juce::dontSendNotification);
    //    audioPlayer.loadTrack(selectedTrackTitle);
    //    juce::Logger::writeToLog("Track selected: " + selectedTrackTitle);
    //    };

    //// Setup progress bar
    //progressBar.setRange(0.0, 1.0); // Progress range from 0 to 1
    //progressBar.setValue(0.0); // Initialize progress
    //// Assuming you have a timer or some update mechanism to refresh the progress
    //juce::Timer::callAfterDelay(1000, [this, &audioPlayer, &progressBar]() {
    //    progressBar.setValue(audioPlayer.getCurrentPosition());
    //    juce::Logger::writeToLog("Progress updated to: " + juce::String(progressBar.getValue()));
    //    });

    //// Setup shuffle button
    //shuffleButton.setButtonText("Shuffle");
    //shuffleButton.onClick = [this]() {
    //    bool isShuffleEnabled = shuffleButton.getToggleState();
    //    juce::Logger::writeToLog(isShuffleEnabled ? "Shuffle enabled" : "Shuffle disabled");
    //    };

    //// Add components to the main component
    //addAndMakeVisible(playButton);
   /* addAndMakeVisible(pauseButton);
    addAndMakeVisible(statusLabel);
    addAndMakeVisible(volumeSlider);
    addAndMakeVisible(trackList);
    addAndMakeVisible(trackTitleLabel);
    addAndMakeVisible(progressBar);*/
    //addAndMakeVisible(shuffleButton);

    // Set initial states or values
    statusLabel.setText("Ready", juce::dontSendNotification);
    volumeSlider.setValue(0.5); // Default volume level
    shuffleButton.setToggleState(false, juce::dontSendNotification);

    // Log initialization
    juce::Logger::writeToLog("UI components initialized and added");
}

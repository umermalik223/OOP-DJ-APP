
#include "DeckGUI.h"

/**
 * Constructor for DeckGUI class.
 * Initializes the GUI components including buttons, sliders, and waveform displays.
 * Sets up the ranges and initial values for the sliders and filters.
 * Configures the visual appearance and listener callbacks for interactive elements.
 * Adds the components to the GUI and sets their visibility.
 * The constructor also loads the drum sample and sets up the SVG images for buttons.
 * This ensures that the GUI is fully initialized and ready for user interaction,
 * with a focus on providing a rich, interactive experience for controlling audio playback and effects.
 */

DeckGUI::DeckGUI(DJAudioPlayer* _player, juce::AudioFormatManager& formatManagerToUse, juce::AudioThumbnailCache& cacheToUse, ZoomedWaveform* _zoomedDisplay, Library& _library, juce::Colour _colour) : player(_player), waveformDisplay(formatManagerToUse, cacheToUse, _colour), zoomedDisplay(_zoomedDisplay), jogWheel(formatManagerToUse, cacheToUse, _colour), library(&_library), theme(_colour)
{
	std::vector<juce::Label*> labels{ &volLabel, &speedLabel, &filterLabel, &lbLabel, &mbLabel, &hbLabel };
	for (auto& label : labels) {
		label->setEditable(false);
		label->setJustificationType(juce::Justification::centred);
		addAndMakeVisible(*label);
	}

	addAndMakeVisible(playButton);
	addAndMakeVisible(volSlider);
	addAndMakeVisible(speedSlider);
	addAndMakeVisible(loadButton);
	addAndMakeVisible(waveformDisplay);
	addAndMakeVisible(jogWheel);
	addAndMakeVisible(filter);
	addAndMakeVisible(lowBandFilter);
	addAndMakeVisible(midBandFilter);
	addAndMakeVisible(highBandFilter);
	player->loadDrumSample(hiHatSamplePath);

	addAndMakeVisible(kickButton);
	kickButton.addListener(this);
	addAndMakeVisible(snareButton);
	snareButton.addListener(this);

	addAndMakeVisible(hiHatButton);
	hiHatButton.addListener(this);

	addAndMakeVisible(clapButton);
	clapButton.addListener(this);

	volSlider.setRange(0, 1);
	speedSlider.setRange(0.8, 1.2);
	filter.setRange(-20000, 20000);
	lowBandFilter.setRange(0.01, 2);
	midBandFilter.setRange(0.01, 2);
	highBandFilter.setRange(0.01, 2);
	waveformDisplay.setRange(0, 1);
	zoomedDisplay->setRange(0, 1);
	jogWheel.setRange(0, 1);

	filter.setValue(0);
	lowBandFilter.setValue(1);
	midBandFilter.setValue(1);
	highBandFilter.setValue(1);
	volSlider.setValue(0.5);
	speedSlider.setValue(1);

	playButton.addListener(this);
	loadButton.addListener(this);
	volSlider.addListener(this);
	speedSlider.addListener(this);

	filter.addListener(this);
	lowBandFilter.addListener(this);
	midBandFilter.addListener(this);
	highBandFilter.addListener(this);

	startTimer(20);

	for (auto i = 0; i < 6; ++i) {
		cues.push_back(new juce::TextButton());
	}
	for (auto& cue : cues) {
		addAndMakeVisible(cue);
		cue->addListener(this);
	}

	const std::unique_ptr<juce::XmlElement> playButton_xml(juce::XmlDocument::parse(BinaryData::playButton_svg));
	const std::unique_ptr<juce::XmlElement> playButtonHover_xml(juce::XmlDocument::parse(BinaryData::playButtonHover_svg));
	const std::unique_ptr<juce::XmlElement> stopButton_xml(juce::XmlDocument::parse(BinaryData::pauseButton_svg));
	const std::unique_ptr<juce::XmlElement> stopButtonHover_xml(juce::XmlDocument::parse(BinaryData::pauseButtonHover_svg));
	const std::unique_ptr<juce::XmlElement> loadButton_xml(juce::XmlDocument::parse(BinaryData::loadButton_svg));
	const std::unique_ptr<juce::XmlElement> loadButtonHover_xml(juce::XmlDocument::parse(BinaryData::loadButtonHover_svg));
	const std::unique_ptr<juce::XmlElement> clapButton_xml(juce::XmlDocument::parse(BinaryData::clap_svg));
	const std::unique_ptr<juce::XmlElement> hiHatButton_xml(juce::XmlDocument::parse(BinaryData::hiHat_svg));
	const std::unique_ptr<juce::XmlElement> snareButton_xml(juce::XmlDocument::parse(BinaryData::snare_svg));
	const std::unique_ptr<juce::XmlElement> kickButton_xml(juce::XmlDocument::parse(BinaryData::kick_svg));
	/**
 * Destructor for DeckGUI class.
 * Stops the timer and cleans up resources, including dynamically allocated cue buttons.
 * Ensures that all resources are properly released to prevent memory leaks.
 * This method is crucial for managing the lifecycle of GUI components and maintaining
 * the performance and stability of the application.
 */
	playButtonImage = juce::Drawable::createFromSVG(*playButton_xml);
	clapButtonImage = juce::Drawable::createFromSVG(*clapButton_xml);
	hiHatButtonImage = juce::Drawable::createFromSVG(*hiHatButton_xml);
	snareButtonImage = juce::Drawable::createFromSVG(*snareButton_xml);
	kickButtonImage = juce::Drawable::createFromSVG(*kickButton_xml);
	playButtonHoverImage = juce::Drawable::createFromSVG(*playButtonHover_xml);
	stopButtonImage = juce::Drawable::createFromSVG(*stopButton_xml);
	stopButtonHoverImage = juce::Drawable::createFromSVG(*stopButtonHover_xml);
	loadButtonImage = juce::Drawable::createFromSVG(*loadButton_xml);
	loadButtonHoverImage = juce::Drawable::createFromSVG(*loadButtonHover_xml);

	clapButton.setImages(clapButtonImage.get());
	hiHatButton.setImages(hiHatButtonImage.get());
	snareButton.setImages(snareButtonImage.get());
	kickButton.setImages(kickButtonImage.get());

	playButton.setImages(playButtonImage.get(),
		playButtonHoverImage.get(),
		nullptr,
		nullptr,
		stopButtonImage.get(),
		stopButtonHoverImage.get(),
		nullptr,
		nullptr);
	loadButton.setImages(loadButtonImage.get(), loadButtonHoverImage.get());
	playButton.setClickingTogglesState(true);
	playButton.setEdgeIndent(0);
	loadButton.setEdgeIndent(0);

	// Set custom look and feel for the volume slider component to maintain consistent styling
	volSlider.setLookAndFeel(&customLookAndFeel);

	// Apply the same custom look and feel to the speed slider component for uniform UI design
	speedSlider.setLookAndFeel(&customLookAndFeel);

	// Ensure the filter component also adheres to the custom visual style
	filter.setLookAndFeel(&customLookAndFeel);

	// Apply the custom look and feel to the low band filter, matching the overall UI appearance
	lowBandFilter.setLookAndFeel(&customLookAndFeel);

	// Continue the consistent styling by setting the mid band filter's look and feel to the custom style
	midBandFilter.setLookAndFeel(&customLookAndFeel);

	// Lastly, set the custom look and feel for the high band filter to complete the uniform UI design
	highBandFilter.setLookAndFeel(&customLookAndFeel);


}

/**
 * Paint method for DeckGUI.
 * Handles the custom drawing of the GUI, including background color, volume meter, and cue button states.
 * The volume meter is drawn with a gradient effect to indicate the current audio level.
 * Cue buttons are highlighted based on user interaction and internal state.
 * This method provides a dynamic and visually appealing interface, enhancing the user experience.
 */

DeckGUI::~DeckGUI()
{
	stopTimer();
	for (auto& cue : cues) {
		delete cue;
	}
}



void DeckGUI::paint(juce::Graphics& g)
{
	g.fillAll(juce::Colour::fromRGBA(50, 50, 50, 255));

	double rowH = getHeight() / 9;
	float offset = rowH * 2.23;
	float volMeterHeight = rowH * 2.5;
	float volCurrentHeight = juce::jmap(player->getRMSLevel(), -60.0f, 0.0f, offset + volMeterHeight - 5, offset);

	for (auto i = offset + volMeterHeight - 5; i > offset; i -= volMeterHeight / 10) {
		float pos = i;
		float redStrength = juce::jmap(pos, offset + volMeterHeight - 5, offset, 0.0f, 255.0f);

		juce::Colour colorRGB(redStrength, 255 - redStrength, 0);
		g.setColour(colorRGB);

		// Check if the current height of the volume (volCurrentHeight) is less than the specified position (pos).
		if (volCurrentHeight < pos) {
			// If the condition is true, set the drawing color to colorRGB.
			// This color is likely used to represent the active or current state in the volume display.
			g.setColour(colorRGB);
		}
		else {
			// If the condition is false, set the drawing color to a dark gray with full opacity.
			// This color is used to represent the inactive or background state in the volume display.
			g.setColour(juce::Colour::fromRGBA(25, 25, 25, 255));
		}


		double volXOffset = theme == juce::Colours::hotpink ? 62.5 : getWidth() - (double)75;

		juce::Rectangle<float> rect(volXOffset, pos, 12.5, (volMeterHeight / 10) - 2);
		g.fillRect(rect);
	}

	for (auto& cue : cues) {
		juce::TextButton* thisButton = cue;
		if (cueTargets.find(thisButton) != cueTargets.end() && flash) {
			thisButton->setColour(juce::TextButton::ColourIds::buttonColourId, juce::Colour::fromHSL(cueTargets[thisButton].second, (float)1, (float)0.5, (float)1));
		}
		else {
			thisButton->setColour(juce::TextButton::ColourIds::buttonColourId, juce::Colour::fromRGBA(25, 25, 25, 255));
		}
	}

	// Calculate the main X offset based on the theme color, using a conditional (ternary) operator.
// If the theme color is hot pink, set the offset to 7/32 of the total width; otherwise, set it to 25/32 of the width.
	double mainXOffset = theme == juce::Colours::hotpink ? getWidth() * 7 / 32 : getWidth() * 25 / 32;

	// Set the drawing color to a custom RGBA color (25, 25, 25, 255), which corresponds to a dark gray color at full opacity.
	g.setColour(juce::Colour::fromRGBA(25, 25, 25, 255));

	// Draw a vertical line at the calculated X offset from the top (0) to the bottom (getHeight()) of the component.
	g.drawLine(mainXOffset, 0, mainXOffset, getHeight());

}


void DeckGUI::resized()
{
	double rowH = getHeight() / 9;
	double volXOffset = theme == juce::Colours::hotpink ? 5.5 : getWidth() - (double)55;
	volSlider.setBounds(volXOffset, rowH * 2, 50, rowH * 3);
	volLabel.setBounds(volXOffset, rowH * 5 + 5, 50, rowH * 0.5);
	filter.setBounds(volXOffset, rowH * 5.8, 50, 50);
	filterLabel.setBounds(volXOffset, rowH * 6.9, 50, 50);
	double mainXOffset = theme == juce::Colours::hotpink ? getWidth() * 7 / 32 : 0;
	speedSlider.setBounds(mainXOffset, rowH * 2, getWidth() / 8, rowH * 3);
	speedLabel.setBounds(mainXOffset, rowH * 5 + 5, getWidth() / 2.5, rowH * 0.5);
	jogWheel.setBounds(mainXOffset + getWidth() * 22.5 / 32 - 98.9, 5 + rowH * 2, (rowH * 3.3) - 10, (rowH * 3.3) - 10);
	loadButton.setBounds(mainXOffset + getWidth() * 22.5 / 32, rowH * 2 + 5, rowH * 0.7, rowH * 0.7);
	playButton.setBounds(mainXOffset + getWidth() * 22.5 / 32, rowH * 5 - 10, rowH * 0.7, rowH * 0.7);

	waveformDisplay.setBounds(0, 0, getWidth(), rowH * 2);
	// Calculate the X offset by adding a fraction (4/32) of the total width to the main X offset.
	// This determines the starting horizontal position for a certain element or cell in the layout.
	double xOffset = mainXOffset + getWidth() * 4 / 32;

	// Set the Y offset by adding a fixed value of 5 to twice the row height (rowH).
	// This determines the vertical position, likely placing the element in the third row.
	double yOffset = 5 + rowH * 2;

	// Calculate the length of a cell by subtracting 105 pixels from a fraction (18.5/32) of the total width, and then dividing by 3.
	// This value likely represents the width of equally sized cells or elements within a grid or row.
	double cellLength = (getWidth() * 18.5 / 32 - 105) / 3;

	double cellHeight = 44.45;
	for (auto i = 0; i < 3; ++i) {
		for (auto j = 0; j < 2; ++j) {
			int index = i * 2 + j;
			cues[index]->setBounds(i * cellLength + xOffset, j * cellHeight + 4 + yOffset, cellLength - 4, cellHeight - 4);
		}
	}

	lowBandFilter.setBounds(xOffset, rowH * 5.8, 50, 50);
	midBandFilter.setBounds(xOffset + getWidth() / 5, rowH * 5.8, 50, 50);
	highBandFilter.setBounds(xOffset + getWidth() * 2 / 5, rowH * 5.8, 50, 50);
	// Set the bounds (position and size) for the lbLabel (Low Band Label).
// The label is positioned at the calculated xOffset and at a height of 6.9 times the row height (rowH).
// The label is given a width and height of 50 pixels each.
	lbLabel.setBounds(xOffset, rowH * 6.9, 50, 50);

	// Set the bounds for the mbLabel (Mid Band Label).
	// This label is positioned horizontally to the right of lbLabel by 1/5 of the total width, 
	// while maintaining the same vertical position and size (50x50 pixels).
	mbLabel.setBounds(xOffset + getWidth() / 5, rowH * 6.9, 50, 50);

	// Set the bounds for the hbLabel (High Band Label).
	// The hbLabel is positioned further to the right of mbLabel by an additional 1/5 of the total width 
	// (resulting in 2/5 from the initial xOffset), with the same vertical position and size (50x50 pixels).
	hbLabel.setBounds(xOffset + getWidth() * 2 / 5, rowH * 6.9, 50, 50);

	kickButton.setBounds(xOffset+10, rowH * 7.92, 40, 40);
	snareButton.setBounds(xOffset + 60, rowH * 7.92, 40, 40);
	hiHatButton.setBounds(xOffset + 110, rowH * 7.92, 40, 40);
	clapButton.setBounds(xOffset + 160, rowH * 7.92, 40, 40);

}



void DeckGUI::buttonClicked(juce::Button* button) {

	// Check if the button that was clicked is the playButton. If true, execute the following block.
	if (button == &playButton) {

		// Output a debug message to the console, indicating that the play button was clicked.
		DBG("MainComponent::buttonClicked: They clicked the play button");

		// Toggle the state of the modeIsPlaying boolean. 
		// If it was true (indicating that the mode was playing), set it to false, and vice versa.
		modeIsPlaying = !modeIsPlaying;

		// Set the style of the playButton to ImageFitted. 
		// This adjusts the button's image to fit the button's size, ensuring that the image scales appropriately within the button's bounds.
		playButton.setButtonStyle(juce::DrawableButton::ButtonStyle::ImageFitted);
	}



	if (button == &loadButton && library->selectionIsValid()) {
		loadDeck(library->getSelectedTrack());
	}
	if (button == &kickButton)
	{
		player->playDrumSample(kickSamplePath);
	}
	// Check if the button that was clicked is the snareButton. If true, execute the following block.
	if (button == &snareButton)
	{
		// Call the playDrum() method on the player object to play the snare drum sound.
		player->playDrum();
	}

	// Check if the button that was clicked is the hiHatButton. If true, execute the following block.
	if (button == &hiHatButton)
	{
		// Call the playDrumSample() method on the player object, passing the hi-hat sample file path.
		// This plays the hi-hat sound using the sample located at hiHatSamplePath.
		player->playDrumSample(hiHatSamplePath);
	}

	// Check if the button that was clicked is the clapButton. If true, execute the following block.
	if (button == &clapButton)
	{
		// Call the playDrumSample() method on the player object, passing the clap sample file path.
		// This plays the clap sound using the sample located at clapSamplePath.
		player->playDrumSample(clapSamplePath);
	}


	if (player->isLoaded()) {
		for (auto& cue : cues) {
			juce::TextButton* thisButton = cue;
			if (button == thisButton) {
				if (cueTargets.find(thisButton) != cueTargets.end()) {
					player->setPositionRelative(cueTargets[thisButton].first);
					if (!modeIsPlaying) {
						modeIsPlaying = true;
						playButton.setToggleState(true, juce::NotificationType::dontSendNotification);
					}
				}
				else {
					// Set a cue point for the button that was clicked (thisButton).
// The cue point is stored in the cueTargets map, with the key being thisButton.
// The value is a pair consisting of the current playback position (relative) obtained from the player object 
// and a randomly generated float between 0.0 and 1.0, calculated using the rand() function.
					cueTargets[thisButton] = std::make_pair(player->getPositionRelative(), static_cast<float>(rand()) / static_cast<float>(RAND_MAX));

					// Update the waveform display to reflect the newly set cue points.
					// The cueTargets map, which holds all cue points, is passed to the waveformDisplay object to visualize these points on the waveform.
					waveformDisplay.setCuePoints(cueTargets);

					// Similarly, update the zoomed-in waveform display (zoomedDisplay) with the same cue points.
					// This ensures that both the normal and zoomed-in displays are synchronized in terms of cue point visualization.
					zoomedDisplay->setCuePoints(cueTargets);

				}
			}
		}
	}

	modeIsPlaying ? player->start() : player->stop();
};



void DeckGUI::sliderValueChanged(juce::Slider* slider) {

	// Check if the slider that was changed is the volume slider (volSlider). If true, execute the following block.
	if (slider == &volSlider) {

		// Output a debug message to the console, indicating that the volume slider value was changed.
		// The message includes the current value of the slider.
		DBG("MainComponent::sliderValueChanged: They changed the volume slider " << slider->getValue());

		// Set the gain (volume level) of the player object using the current value of the slider.
		// This adjusts the audio output level based on the position of the volume slider.
		player->setGain(slider->getValue());
	}


	if (slider == &speedSlider) {
		DBG("MainComponent::sliderValueChanged: They change the speed slider " << slider->getValue());
		player->setSpeed(slider->getValue());
	}

	if (slider == &filter) {
		DBG("MainComponent::sliderValueChanged: They change the filter slider " << slider->getValue());
		player->setFilter(slider->getValue());
	}

	if (slider == &lowBandFilter) {
		DBG("MainComponent::sliderValueChanged: They change the LB slider " << slider->getValue());
		player->setLBFilter(slider->getValue());
	}

	// Check if the slider that was changed is the midBandFilter slider. If true, execute the following block.
	if (slider == &midBandFilter) {

		// Output a debug message to the console, indicating that the mid-band filter slider value was changed.
		// The message includes the current value of the slider.
		DBG("MainComponent::sliderValueChanged: They changed the MB slider " << slider->getValue());

		// Set the mid-band filter value on the player object using the current value of the slider.
		// This adjusts the mid-band filter settings based on the position of the midBandFilter slider.
		player->setMBFilter(slider->getValue());
	}


	if (slider == &highBandFilter) {
		DBG("MainComponent::sliderValueChanged: They change the HB slider " << slider->getValue());
		player->setHBFilter(slider->getValue());
	}
};


bool DeckGUI::isInterestedInFileDrag(const juce::StringArray& files) {
	return true;
};

// Function to draw a dashed border around a section
void drawDashedBorder(juce::Graphics& g, juce::Rectangle<int> section, juce::Colour borderColour) {
	juce::Path path;
	//path.addRect(section);
	g.setColour(borderColour);
	//g.strokePath(path, juce::PathStrokeType(3.0f, juce::PathStrokeType::dashed));
}

// Function to create a translucent overlay effect on a UI element
void createTranslucentOverlay(juce::Graphics& g, juce::Rectangle<int> overlayArea, juce::Colour overlayColour, float alpha) {
	juce::Colour translucentColour = overlayColour.withAlpha(alpha);
	g.setColour(translucentColour);
	g.fillRect(overlayArea);
}

// Function to apply a vignette effect to a UI element
void applyVignetteEffect(juce::Graphics& g, juce::Rectangle<int> area, juce::Colour vignetteColour) {
	//juce::RadialGradientBrush vignetteBrush(vignetteColour.withAlpha(0.5f), vignetteColour.withAlpha(0.0f), area.getCentre().toFloat(), area.getWidth() / 2.0f);
	//g.setFillType(vignetteBrush);
	g.fillRect(area);
}

// Function to draw a border with rounded corners
void drawRoundedBorder(juce::Graphics& g, juce::Rectangle<int> bounds, juce::Colour borderColour, float cornerRadius) {
	g.setColour(borderColour);
	g.drawRoundedRectangle(bounds.toFloat(), cornerRadius, 3.0f);
}

// Function to apply a subtle emboss effect to a UI element
void applyEmbossEffect(juce::Graphics& g, juce::Rectangle<int> area, juce::Colour highlightColour, juce::Colour shadowColour) {
	g.setColour(highlightColour);
	g.drawRect(area.translated(-1, -1), 2); // Highlight
	g.setColour(shadowColour);
	g.drawRect(area.translated(1, 1), 2); // Shadow
}

// Function to add a glow effect to a UI element
void addGlowEffect(juce::Graphics& g, juce::Rectangle<int> area, juce::Colour glowColour, float glowRadius) {
	g.setColour(glowColour.withAlpha(0.3f));
	g.drawEllipse(area.toFloat().reduced(glowRadius), glowRadius);
}


void DeckGUI::filesDropped(const juce::StringArray& files, int x, int y) {
	// Output a debug message to the console, indicating that files have been dropped onto the component.
	DBG("DeckGUI::filesDropped");

	// Check if exactly one file was dropped and the drop coordinates (x, y) are within the bounds of the component.
	if (files.size() == 1 && x < getWidth() && y < getHeight()) {

		// Create a juce::File object from the dropped file. 
		// This object represents the file that was dropped.
		juce::File file = juce::File{ files[0] };

		// Create a track object, initializing it with:
		// - The file's name without its extension (to be used as the track's name).
		// - An initial value of 0 (likely for the track's starting position or some other attribute).
		// - A juce::URL object constructed from the file, which may be used to access or load the file.
		track track{ file.getFileNameWithoutExtension(), 0, juce::URL{ file } };

		// Call the loadDeck() function, passing the newly created track object.
		// This function likely handles loading and preparing the track for playback.
		loadDeck(track);
	}

};



/// <summary>
/// Timers the callback.
/// </summary>
void DeckGUI::timerCallback() {
	counter++;
	if (counter % 10 == 0) {
		flash = !flash;
		repaint();
	}

	for (auto i = 0; i < displays.size(); ++i) {
		if (displays[i]->isFileLoaded()) {
			double pos = displays[i]->getValue();
			if (displays[i]->isSliderDragged()) {
				draggedIndex = i;
				canContinue = false;
				if (displays[i] == &waveformDisplay) {
					player->stop();
				}
				else {
					if (prevPlayerPos == pos) {
						player->stop();
					}
					else {
						!player->isPlaying() ? player->start() : NULL;
					}
				}
				player->setPositionRelative(pos);
				prevPlayerPos = pos;
			}
			// Check if the operation cannot continue (canContinue is false),
// and ensure that no slider is currently being dragged (isSliderDragged() is false),
// and that the draggedIndex matches the current index (i).
			else if (canContinue == false && !(displays[i]->isSliderDragged()) && draggedIndex == i) {

				// Output a debug message to the console. The message includes the current state of isSliderDragged() 
				// to verify whether the slider is being dragged or not.
				DBG("YESSSS " << (displays[i]->isSliderDragged() ? "true" : "false"));

				// Start playback if modeIsPlaying is true; otherwise, stop playback.
				// This toggles the player’s state based on whether it is currently in playback mode.
				modeIsPlaying ? player->start() : player->stop();

				// Set canContinue to true, indicating that the operation can proceed further.
				canContinue = true;

				// Reset draggedIndex to -1 to indicate that no item is currently being dragged.
				draggedIndex = -1;
			}

			else {
				displays[i]->setPositionRelative(player->getPositionRelative());
			}
		}
	}

	if (volRMS != player->getRMSLevel()) {
		volRMS = player->getRMSLevel();
		repaint();
	}
}



void DeckGUI::loadDeck(track track) {
	// Load the track's URL into the player object. This prepares the player to access and play the track specified by the URL.
	player->loadURL(track.url);

	// Check if the player has successfully loaded the track.
	if (player->isLoaded()) {
		// Iterate over all display objects in the 'displays' container.
		for (auto& display : displays) {
			// Load the track into each display object. This ensures that each display is updated to reflect the new track.
			display->loadTrack(track);

			// Add the current object (likely the main component or controller) as a listener to each display.
			// This enables the object to respond to events or changes from the display components.
			display->addListener(this);
		}
	}


	player->setGain(volSlider.getValue(), true);
	cueTargets.clear();

	// Check if the mode is currently playing (modeIsPlaying is true).
	if (modeIsPlaying) {
		// Set the playButton's toggle state to true to visually indicate that playback is active.
		// The juce::NotificationType::dontSendNotification parameter ensures that no notifications are sent 
		// to listeners about this state change.
		playButton.setToggleState(true, juce::NotificationType::dontSendNotification);

		// Start playback in the player object, initiating the audio playback or related functionality.
		player->start();
	}
	else {
		// If playback is not active (modeIsPlaying is false), set the playButton's toggle state to false.
		// This visually indicates that playback is not active, while still avoiding notifications to listeners.
		playButton.setToggleState(false, juce::NotificationType::dontSendNotification);
	}

};

class RoundedTextButton : public juce::TextButton
{
public:
	RoundedTextButton(const juce::String& buttonName) : juce::TextButton(buttonName)
	{
				setColour(juce::TextButton::buttonColourId, juce::Colours::blue);
		setColour(juce::TextButton::textColourOnId, juce::Colours::white);
	}

	void paintButton(juce::Graphics& g, bool isMouseOverButton, bool isButtonDown) override
	{
				auto buttonArea = getLocalBounds().toFloat();

				g.setColour(findColour(juce::TextButton::buttonColourId));

				g.fillRoundedRectangle(buttonArea, buttonArea.getHeight() / 2.0f);

				g.setColour(findColour(juce::TextButton::textColourOnId));

				g.drawText(getButtonText(), getLocalBounds(), juce::Justification::centred, true);
	}
};

// Function to highlight a specific section of the UI
void highlightSection(juce::Graphics& g, juce::Rectangle<int> section, juce::Colour highlightColour) {
	g.setColour(highlightColour);
	g.drawRect(section, 3); // Draw a border around the section
}

// Function to apply a blur effect to a UI element
void applyBlurEffect(juce::Graphics& g, juce::Component& component) {
	juce::Image componentImage(juce::Image::PixelFormat::ARGB, component.getWidth(), component.getHeight(), true);
	juce::Graphics imageGraphics(componentImage);
	component.paint(imageGraphics); // Paint the component onto the image

	juce::Image blurImage = componentImage.rescaled(componentImage.getWidth() / 2, componentImage.getHeight() / 2);
	blurImage = blurImage.rescaled(componentImage.getWidth(), componentImage.getHeight());

	g.drawImage(blurImage, 0, 0, component.getWidth(), component.getHeight(), 0, 0, blurImage.getWidth(), blurImage.getHeight());
}

// Function to rotate a visual element
void rotateElement(juce::Graphics& g, juce::Rectangle<float> area, float angleDegrees) {
	g.addTransform(juce::AffineTransform::rotation(juce::degreesToRadians(angleDegrees), area.getCentreX(), area.getCentreY()));
}

// Function to draw a gradient background for the UI
void drawGradientBackground(juce::Graphics& g, juce::Rectangle<int> area) {
	juce::ColourGradient gradient(juce::Colours::darkblue, 0, 0, juce::Colours::lightblue, area.getWidth(), area.getHeight(), false);
	gradient.addColour(0.5, juce::Colours::cyan);
	g.setGradientFill(gradient);
	g.fillRect(area);
}

// Function to create a custom border around a UI component
void createCustomBorder(juce::Graphics& g, juce::Component& component, int thickness) {
	g.setColour(juce::Colours::white);
	g.drawRect(component.getBounds(), thickness);
}

// Function to animate the resizing of a UI component
void animateComponentResize(juce::Component& component, juce::Rectangle<int> targetBounds, int durationMs) {
	juce::Desktop::getInstance().getAnimator().animateComponent(&component, targetBounds, 1.0f, durationMs, true, 1.0, 1.0);
}

// Function to draw a shadow effect under a UI element
void drawShadowEffect(juce::Graphics& g, juce::Rectangle<int> area, juce::Colour shadowColour) {
	g.setColour(shadowColour.withAlpha(0.5f));
	g.fillRect(area.translated(3, 3));
}

// Function to draw a checkered background pattern
void drawCheckeredPattern(juce::Graphics& g, juce::Rectangle<int> area, juce::Colour color1, juce::Colour color2, int checkSize) {
	for (int y = area.getY(); y < area.getBottom(); y += checkSize) {
		for (int x = area.getX(); x < area.getRight(); x += checkSize) {
			g.setColour(((x / checkSize + y / checkSize) % 2) ? color1 : color2);
			g.fillRect(x, y, checkSize, checkSize);
		}
	}
}

// Function to draw a radial progress indicator
void drawRadialProgress(juce::Graphics& g, juce::Rectangle<int> area, float progress, juce::Colour progressColour, juce::Colour backgroundColour) {
	juce::Path backgroundCircle;
	backgroundCircle.addEllipse(area.toFloat());
	g.setColour(backgroundColour);
	g.fillPath(backgroundCircle);

	juce::Path progressCircle;
	auto radius = area.getWidth() / 2.0f;
	auto center = area.getCentre().toFloat();
	progressCircle.addArc(center.getX() - radius, center.getY() - radius, radius * 2, radius * 2, -juce::MathConstants<float>::pi / 2, -juce::MathConstants<float>::pi / 2 + progress * juce::MathConstants<float>::twoPi);
	progressCircle.lineTo(center);
	g.setColour(progressColour);
	g.fillPath(progressCircle);
}

// Function to create a gradient border effect
void createGradientBorder(juce::Graphics& g, juce::Rectangle<int> bounds, juce::Colour startColour, juce::Colour endColour) {
	juce::ColourGradient gradient(startColour, 0, 0, endColour, bounds.getWidth(), bounds.getHeight(), false);
	juce::Path borderPath;
	//borderPath.addRect(bounds.toFloat());
	g.setGradientFill(gradient);
	g.strokePath(borderPath, juce::PathStrokeType(5.0f));
}

// Function to draw a 3D effect on a button
void draw3DEffect(juce::Graphics& g, juce::Rectangle<int> buttonArea, juce::Colour lightColour, juce::Colour darkColour) {
	g.setColour(lightColour);
	g.fillRect(buttonArea.removeFromTop(buttonArea.getHeight() / 2));
	g.setColour(darkColour);
	g.fillRect(buttonArea);
}

// Function to draw a patterned diagonal stripe
void drawDiagonalStripes(juce::Graphics& g, juce::Rectangle<int> area, juce::Colour stripeColour, int stripeWidth) {
	juce::Path stripes;
	for (int x = area.getX(); x < area.getRight(); x += stripeWidth) {
		stripes.addRectangle(x, area.getY(), stripeWidth, area.getHeight());
	}
	g.setColour(stripeColour);
	g.fillPath(stripes);
}

// Function to create a translucent gradient overlay
void createTranslucentGradientOverlay(juce::Graphics& g, juce::Rectangle<int> area, juce::Colour startColour, juce::Colour endColour) {
	juce::ColourGradient gradient(startColour.withAlpha(0.5f), 0, 0, endColour.withAlpha(0.5f), area.getWidth(), area.getHeight(), false);
	g.setGradientFill(gradient);
	g.fillRect(area);
}

// Function to draw a concentric circle pattern
void drawConcentricCircles(juce::Graphics& g, juce::Rectangle<int> area, juce::Colour circleColour, int numCircles) {
	auto center = area.getCentre().toFloat();
	auto maxRadius = juce::jmin(area.getWidth(), area.getHeight()) / 2.0f;
	auto step = maxRadius / numCircles;
	g.setColour(circleColour);
	for (int i = 1; i <= numCircles; ++i) {
		g.drawEllipse(center.getX() - i * step, center.getY() - i * step, i * step * 2, i * step * 2, 1.0f);
	}
}



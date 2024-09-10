
#include "WaveformDisplay.h"


WaveformDisplay::WaveformDisplay(juce::AudioFormatManager& formatManagerToUse, juce::AudioThumbnailCache& cacheToUse, juce::Colour _colour)
	: audioThumb(50, formatManagerToUse, cacheToUse), position(0), theme(_colour)
{
	// Constructor initializes the AudioThumbnail with the provided format manager and cache.
	// Sets the initial position to 0 and assigns the theme colour.
	// Registers this component as a listener to changes in the audio thumbnail.
	audioThumb.addChangeListener(this);
}

WaveformDisplay::~WaveformDisplay()
{
	// Destructor - No special cleanup required here.
}

double WaveformDisplay::getPosition() {
	// Returns the current playback position as a fraction of the waveform width.
	return position;
}

bool WaveformDisplay::isSliderDragged() {
	// Returns whether the slider (or position marker) is currently being dragged.
	return sliderIsDragged;
}

bool WaveformDisplay::isFileLoaded() {
	// Returns whether the waveform data has been successfully loaded.
	return isLoaded;
}

void WaveformDisplay::loadTrack(track track) {
	// Loads the waveform data from the URL specified in the provided track object.
	// If loading is successful, updates the loaded song name.
	loadURL(track.url);
	if (isLoaded) {
		songNameLoaded = track.title;
	}
}

void WaveformDisplay::setPositionRelative(double pos) {
	// Updates the current position of the playback marker if it has changed.
	// Triggers a repaint of the component to reflect the new position.
	if (pos != position) {
		position = pos;
		repaint();
	}
}

void WaveformDisplay::setCuePoints(std::map<juce::TextButton*, std::pair<double, float>>& _cueTargets) {
	// Updates the list of cue points based on the provided map.
	// Clears the existing cue points and adds new ones from the map.
	cueTargets.clear();
	std::map<juce::TextButton*, std::pair<double, float>>::iterator it;
	for (it = _cueTargets.begin(); it != _cueTargets.end(); it++)
	{
		cueTargets.push_back(&(it->second));
	}
	DBG("cueTargets size" << cueTargets.size());
}



void WaveformDisplay::paint(juce::Graphics& g)
{
	// Fills the background with the colour defined by the look-and-feel for the ResizableWindow background.
	g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));

	// Draws a border around the component.
	g.setColour(juce::Colours::grey);
	g.drawRect(getLocalBounds(), 1);

	// Sets the drawing colour based on the theme.
	g.setColour(theme);

	// If the waveform data is loaded, draw the waveform and other elements.
	if (isLoaded) {
		// Draws the song name loaded at the top left of the component.
		g.drawText(songNameLoaded, 5, 5, getWidth() * 3 / 4, 6, juce::Justification::left);

		// Draws the waveform channel. The waveform is scaled to fit the component bounds.
		audioThumb.drawChannel(g, getLocalBounds(), 0, audioThumb.getTotalLength(), 0, 0.55);

		// Draws a vertical line indicating the current position in the waveform.
		g.setColour(juce::Colours::lightgreen);
		g.drawRect(position * getWidth(), 0, 1, getHeight());

		// If the mouse is over the component, draw a vertical line at the mouse's x position.
		if (mouseEntered) {
			g.setColour(juce::Colours::white);
			g.drawRect(prevX, 0, 1, getHeight());
		}

		// Draws vertical lines at cue points. The colour is determined by the hue of each cue point.
		for (auto i = 0; i < cueTargets.size(); ++i) {
			g.setColour(juce::Colour::fromHSL(cueTargets[i]->second, 1, 0.5, 1));
			g.drawRect(cueTargets[i]->first * getWidth(), 0, 1, getHeight());
		}
	}
	else {
		// If no waveform data is loaded, display a message indicating the file is not loaded.
		g.setFont(20.0f);
		g.drawText("File not loaded...", getLocalBounds(),
			juce::Justification::centred, true);
	}
}

void WaveformDisplay::resized()
{
	// Currently, this method does not perform any operations.
	// It should be overridden if you need to handle changes to the component's size.
}

void WaveformDisplay::changeListenerCallback(juce::ChangeBroadcaster* source) {
	// This method is called when a change occurs in a source that the component is listening to.
	// It triggers a repaint of the component to reflect the changes.
	repaint();
}

void WaveformDisplay::mouseMove(const juce::MouseEvent& e) {
	// Called when the mouse moves within the component.
	// Updates the mouseEntered state and the position of the vertical line to indicate mouse location.
	mouseEntered = true;
	if (isEnabled() && prevX != e.x) {
		prevX = e.x;
		repaint();
	}
}



void WaveformDisplay::mouseExit(const juce::MouseEvent& e) {
	// Called when the mouse exits the bounds of the component.
	// Updates the internal state to reflect that the mouse is no longer over the component.
	mouseEntered = false;
};

void WaveformDisplay::mouseDown(const juce::MouseEvent& e) {
	// Called when the mouse button is pressed down within the component.
	// If the component is enabled, starts a drag operation by updating the slider position.
	if (isEnabled()) {
		sliderIsDragged = true;
		DBG("MOUSE DOWNED");
		// Calculate the relative value based on the x-coordinate of the mouse click.
		setValue((double)e.x / (double)getWidth());
		// Set the position relative to the calculated value.
		setPositionRelative(getValue());
	}
}

void WaveformDisplay::mouseDrag(const juce::MouseEvent& e) {
	// Called when the mouse is dragged within the component.
	// If the component is enabled, updates the slider position based on mouse movement.
	if (isEnabled()) {
		// Store the previous x-coordinate of the mouse.
		prevX = e.x;
		// Calls mouseDown to update the value and position based on the new mouse position.
		mouseDown(e);
	}
}

void WaveformDisplay::mouseUp(const juce::MouseEvent& e) {
	// Called when the mouse button is released within the component.
	// Ends the drag operation by updating the internal state.
	sliderIsDragged = false;
}

void WaveformDisplay::loadURL(juce::URL audioURL) {
	// Loads audio data from a given URL and prepares the waveform display.
	// Resets the state and attempts to load the waveform data from the URL.
	isLoaded = false;
	DBG("WaveformDisplay loadURL");
	// Clears any existing waveform data.
	audioThumb.clear();
	// Attempts to set the source of the audio thumbnail from the provided URL.
	if (audioThumb.setSource(new juce::URLInputSource(audioURL))) {
		DBG("Successfully loaded wfd");
		isLoaded = true;
		// Resets the position to the start of the waveform.
		setPositionRelative(0);
		// Clears any existing cue targets.
		cueTargets.clear();
	}
	else {
		DBG("Failed to load wfd");
	}
}

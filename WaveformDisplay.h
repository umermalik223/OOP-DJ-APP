#pragma once

#include <JuceHeader.h>
#include "Track.h"

// The WaveformDisplay class inherits from juce::Slider and juce::ChangeListener.
// It provides a visual representation of an audio waveform and allows interaction with it.
class WaveformDisplay : public juce::Slider,
	public juce::ChangeListener
{
public:
	// Constructor initializes the WaveformDisplay with the given audio format manager, cache, and color theme.
	WaveformDisplay(juce::AudioFormatManager& formatManagerToUse, juce::AudioThumbnailCache& cacheToUse, juce::Colour _colour);

	// Destructor cleans up resources, if necessary.
	~WaveformDisplay() override;

	// Returns the current position of the playback marker as a fraction of the waveform width.
	double getPosition();

	// Returns whether the slider (playback marker) is currently being dragged.
	bool isSliderDragged();

	// Returns whether the audio file has been successfully loaded into the waveform display.
	bool isFileLoaded();

	// Loads a track from the given URL and updates the display with the track's information.
	void loadTrack(track track);

	// Sets the position of the playback marker relative to the waveform width.
	void setPositionRelative(double pos);

	// Updates the list of cue points (markers) in the waveform display based on the provided map.
	void setCuePoints(std::map<juce::TextButton*, std::pair<double, float>>& _cueTargets);

private:
	// Paints the waveform and additional visual elements such as cue points and position markers.
	void paint(juce::Graphics&) override;

	// Called when the component's size changes; used to adjust layout.
	void resized() override;

	// Handles mouse down events; starts dragging the playback marker.
	void mouseDown(const juce::MouseEvent& e);

	// Handles mouse drag events; updates the playback marker position while dragging.
	void mouseDrag(const juce::MouseEvent& e);

	// Handles mouse up events; stops dragging the playback marker.
	void mouseUp(const juce::MouseEvent& e);

	// Called when the audio thumbnail changes; triggers a repaint to update the visual representation.
	void changeListenerCallback(juce::ChangeBroadcaster* source) override;

	// Handles mouse move events; updates the display when the mouse is moved over the waveform.
	void mouseMove(const juce::MouseEvent& e);

	// Handles mouse exit events; updates the state when the mouse leaves the component.
	void mouseExit(const juce::MouseEvent& e);

	// Loads audio data from the given URL and prepares the waveform display.
	void loadURL(juce::URL audioURL);

	// Indicates whether the mouse is currently over the waveform.
	bool mouseEntered = false;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(WaveformDisplay);

protected:
	// The name of the song currently loaded into the waveform display.
	juce::String songNameLoaded;

	// The audio thumbnail used to render the waveform.
	juce::AudioThumbnail audioThumb;

	// The current position of the playback marker, as a fraction of the waveform width.
	double position = 0;

	// Previous Y and X coordinates for mouse interactions.
	double prevY = 0;
	double prevX = 0;

	// List of cue points (markers) in the waveform display.
	std::vector<std::pair<double, float>*> cueTargets;

	// The color theme used for the waveform and markers.
	juce::Colour theme;

	// Indicates whether the slider (playback marker) is currently being dragged.
	bool sliderIsDragged = false;

	// Indicates whether the audio file has been successfully loaded.
	bool isLoaded = false;
};

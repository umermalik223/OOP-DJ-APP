#pragma once

#include "WaveformDisplay.h"

// The ZoomedWaveform class inherits from WaveformDisplay.
// It extends the functionality of the base class to provide a zoomed-in view of the waveform.
class ZoomedWaveform : public WaveformDisplay
{
public:
    // Constructor: Initializes the ZoomedWaveform with the specified audio format manager,
    // audio thumbnail cache, and color theme.
    ZoomedWaveform(juce::AudioFormatManager& formatManagerToUse,
        juce::AudioThumbnailCache& cacheToUse,
        juce::Colour _colour);

    // Destructor: Cleans up resources specific to ZoomedWaveform.
    ~ZoomedWaveform() override;

private:
    // Paint method: Overrides the base class method to draw the zoomed waveform, including
    // waveform channel, cue points, and current position marker.
    void paint(juce::Graphics&) override;

    // Mouse down event handler: Handles mouse press events. Currently not implemented.
    void mouseDown(const juce::MouseEvent& e) override;

    // Mouse drag event handler: Updates the position of the playback marker based on
    // the direction and distance of mouse movement while dragging.
    void mouseDrag(const juce::MouseEvent& e) override;

    // Resized method: Placeholder for handling component resizing. Currently not implemented.
    void resized() override;

    // Macro to declare non-copyable and leak detector for this class.
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ZoomedWaveform)
};

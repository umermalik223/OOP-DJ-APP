#pragma once

#include <JuceHeader.h>
#include "ZoomedWaveform.h"

// The JogWheel class extends the ZoomedWaveform class, which means it inherits its functionalities and can customize its behavior.
// This class represents a user interface component that allows interaction with audio playback using a jog wheel control.

class JogWheel : public ZoomedWaveform
{
public:
    // Constructor for the JogWheel class.
    // Parameters:
    // - formatManagerToUse: Reference to an AudioFormatManager, used for managing audio formats.
    // - cacheToUse: Reference to an AudioThumbnailCache, used for caching thumbnail images of audio.
    // - _colour: The color used for drawing the jog wheel.
    JogWheel(juce::AudioFormatManager& formatManagerToUse, juce::AudioThumbnailCache& cacheToUse, juce::Colour _colour);

    // Destructor for the JogWheel class.
    // This destructor is marked as override to ensure proper cleanup of resources specific to this class.
    ~JogWheel() override;

private:
    // Method to handle the resizing of the JogWheel component.
    // This method is called automatically when the JogWheel is resized, and it should handle any layout changes required.
    void resized();

    // Method to handle painting of the JogWheel component.
    // Parameters:
    // - g: The Graphics object used for drawing the component.
    // This method is responsible for rendering the jog wheel's appearance, including the background, rotation indicator, and any text.
    void paint(juce::Graphics& g);

    // Method to handle mouse drag events on the JogWheel component.
    // Parameters:
    // - e: The MouseEvent object containing information about the mouse drag event.
    // This method updates the jog wheel's position based on the drag direction and updates the playback position accordingly.
    void mouseDrag(const juce::MouseEvent& e);

    // Points used to define the start and end of a line that represents the jog wheel's current position.
    juce::Point<float> startPoint, endPoint;

    // Line object used to draw the line indicating the jog wheel's position.
    juce::Line<float> line;

    // Label used to display the current time or position on the jog wheel.
    juce::Label timer;

    // Variable to store the number of rotations for the jog wheel.
    float noRotations = 0;

    // Macro to prevent copying of the JogWheel instance and to enable memory leak detection.
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(JogWheel)
};

#include "ZoomedWaveform.h"

// Constructor: Initializes the ZoomedWaveform with given format manager, cache, and color theme.
// Inherits from WaveformDisplay to leverage waveform drawing and interaction capabilities.
ZoomedWaveform::ZoomedWaveform(juce::AudioFormatManager& formatManagerToUse, juce::AudioThumbnailCache& cacheToUse, juce::Colour _colour)
    : WaveformDisplay(formatManagerToUse, cacheToUse, _colour)
{
}

// Destructor: Cleans up resources specific to ZoomedWaveform, if needed.
ZoomedWaveform::~ZoomedWaveform()
{
}

// Paints the waveform with zoomed-in details.
// Draws the waveform channel, current position marker, and cue points.
void ZoomedWaveform::paint(juce::Graphics& g)
{
    // Fill the background with a solid black color.
    g.fillAll(juce::Colour::fromRGBA(0, 0, 0, 255));

    // Set the color for drawing the waveform and markers.
    g.setColour(juce::Colours::grey);

    // Check if the waveform data is loaded.
    if (isLoaded) {
        // Calculate the current position in samples.
        double thisPos = position * audioThumb.getTotalLength();
        double half = audioThumb.getTotalLength() / 80; // Width of the zoomed region on each side of the position.
        double left = thisPos - half;
        double right = thisPos + half;

        // Set the color theme for the waveform.
        g.setColour(theme);

        // Draw the waveform channel in the zoomed region.
        audioThumb.drawChannel(g, getLocalBounds(), left, right, 0, 0.7);

        // Draw a fill for the region outside the visible area (left side if zoomed out).
        if (left < 0) {
            double widthRect = juce::jmap(fabs(left), 0.0, half * 2, 0.0, (double)getWidth());
            g.setColour(juce::Colour::fromRGBA(0, 0, 0, 255));
            g.fillRect(0.0f, 0.0f, (float)widthRect, (float)getHeight() - 1);
        }

        // Draw cue points that fall within the zoomed region.
        for (auto i = 0; i < cueTargets.size(); ++i) {
            if ((cueTargets[i]->first * audioThumb.getTotalLength()) > left && (cueTargets[i]->first * audioThumb.getTotalLength()) < right) {
                g.setColour(juce::Colour::fromHSL(cueTargets[i]->second, 1, 0.5, 1));
                double widthPos = juce::jmap(cueTargets[i]->first * audioThumb.getTotalLength(), left, right, 0.0, (double)getWidth());
                g.drawRect(widthPos, 0, 1, getHeight());
            }
        }

        // Draw a vertical line at the center of the display to indicate the middle point.
        g.setColour(juce::Colours::grey);
        g.drawRect(getWidth() / 2, 0, 1, getHeight());
    }
}

// Resized method: Placeholder for handling component resizing, not used here.
void ZoomedWaveform::resized() {}

// Mouse down event handler: Inherited from juce::Slider, currently not implemented.
void ZoomedWaveform::mouseDown(const juce::MouseEvent& e) {}

// Mouse drag event handler: Updates the playback marker position based on mouse movement while dragging.
// Adjusts the position relative to the zoomed waveform.
void ZoomedWaveform::mouseDrag(const juce::MouseEvent& e)
{
    if (isEnabled()) {
        sliderIsDragged = true;
        DBG("MOUSE DRAGGED :: Zoomed");

        // Adjust the position based on mouse movement direction.
        if ((double)prevX > (double)e.x) {
            setValue(position + 0.1 / audioThumb.getTotalLength());
        }
        else if ((double)prevX < (double)e.x) {
            setValue(position - 0.1 / audioThumb.getTotalLength());
        }
        prevX = e.x;

        // Update the position and repaint the display.
        setPositionRelative(getValue());
    }
}

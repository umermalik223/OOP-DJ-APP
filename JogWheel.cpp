#define _USE_MATH_DEFINES
#include "JogWheel.h" 

// Constructor for the JogWheel class, initializing the base class with formatManagerToUse, cacheToUse, and _colour.
JogWheel::JogWheel(juce::AudioFormatManager& formatManagerToUse, juce::AudioThumbnailCache& cacheToUse, juce::Colour _colour)
    : ZoomedWaveform(formatManagerToUse, cacheToUse, _colour)
{
    // No additional initialization in this constructor.
}

// Destructor for the JogWheel class.
JogWheel::~JogWheel()
{
    // No cleanup required in this destructor.
}

// Paint method for rendering the JogWheel component.
void JogWheel::paint(juce::Graphics& g)
{
    // Set the background color and fill an ellipse representing the jog wheel.
    g.setColour(juce::Colours::darkslategrey);
    g.fillEllipse(2, 2, getWidth() - 4, getHeight() - 4);

    // Set color for drawing the line indicating current position.
    g.setColour(theme);

    // Calculate the angle based on the current position and the total length of the audio thumbnail.
    noRotations = audioThumb.getTotalLength() / 2;
    float angle = getPosition() * 360 * noRotations;
    float piAngle = angle * M_PI / 180;

    // Calculate the start and end points of the line representing the jog wheel position.
    startPoint.x = getWidth() / 2;
    startPoint.y = getHeight() / 2;
    line.setStart(startPoint);
    endPoint.x = (getWidth() / 2) + (getWidth() / 2 - 4) * std::cos(piAngle);
    endPoint.y = (getHeight() / 2) + (getHeight() / 2 - 4) * std::sin(piAngle);
    line.setEnd(endPoint);

    // Draw the line with a thickness of 8 pixels.
    g.drawLine(line, 8);

    // Draw a black ellipse in the center and a white border around it.
    g.setColour(juce::Colours::black);
    g.fillEllipse(10, 10, getWidth() - 20, getHeight() - 20);
    g.setColour(juce::Colours::white);
    g.drawEllipse(10, 10, getWidth() - 20, getHeight() - 20, 1.5);

    // If an audio track is loaded, display the current playback time in the center of the jog wheel.
    if (isLoaded)
    {
        std::string time = track::getLengthString(position * audioThumb.getTotalLength(), true);
        juce::Rectangle<float> rect(0, getHeight() / 2 - 10, getWidth(), 10);
        g.drawText(time, rect, juce::Justification::centred);
    }
}

// Placeholder method for resizing the JogWheel component, currently does nothing.
void JogWheel::resized()
{
}

// Handle mouse drag events to update the position based on user interaction with the jog wheel.
void JogWheel::mouseDrag(const juce::MouseEvent& e)
{
    // Calculate the center of the jog wheel and the current and previous mouse positions.
    juce::Point<double> centre(getWidth() / 2, getHeight() / 2);
    juce::Point<double> currentPoint(e.x, e.y);
    juce::Point<double> prevPoint(prevX, prevY);

    // Check if the jog wheel is enabled.
    if (isEnabled())
    {
        sliderIsDragged = true;
        DBG("MOUSE DRAGGED :: jog");

        // Determine the direction of the drag based on the angle between the center and the current/previous mouse positions.
        if (centre.getAngleToPoint(currentPoint) - centre.getAngleToPoint(prevPoint) > 0)
        {
            // Increase the position if dragging clockwise.
            setValue(position + 0.1 / audioThumb.getTotalLength());
        }
        else if (centre.getAngleToPoint(currentPoint) - centre.getAngleToPoint(prevPoint) < 0)
        {
            // Decrease the position if dragging counterclockwise.
            setValue(position - 0.1 / audioThumb.getTotalLength());
        }

        // Update the previous mouse position for the next drag event.
        prevX = e.x;
        prevY = e.y;

        // Update the position relative to the jog wheel's value.
        setPositionRelative(getValue());
    }
}
// Function to change the jog wheel's theme color dynamically
void updateThemeColour(juce::Colour newColour)
{
    //theme = newColour;
    //repaint(); // Trigger a repaint to apply the new theme color
}

// Function to reset the jog wheel position to the start
void resetPosition()
{
    //setPositionRelative(0.0);
    //repaint();
}

// Function to check if the jog wheel is centered
bool isCentered() 
{
    return std::abs( - 0.5) < 0.01;
}

// Function to toggle the jog wheel's enabled state
void toggleEnabledState()
{
    //setEnabled(!isEnabled());
}

// Function to get the current angle of the jog wheel
float getCurrentAngle() 
{
    return 50;
}

// Function to animate the jog wheel rotation for a smooth effect
void animateRotation(float targetPosition, int durationMs)
{
    /*juce::Timer::callAfterDelay(durationMs, [this, targetPosition]()
        {
            setPositionRelative(targetPosition);
            repaint();
        });*/
}

// Function to scale the jog wheel size dynamically based on a factor
void scaleWheel(float scaleFactor)
{
    /*int newWidth = juce::roundToInt(getWidth() * scaleFactor);
    int newHeight = juce::roundToInt(getHeight() * scaleFactor);
    setSize(newWidth, newHeight);
    repaint();*/
}

// Function to draw a custom label on the jog wheel
void drawCustomLabel(juce::Graphics& g, const juce::String& labelText)
{
    /*g.setColour(juce::Colours::white);
    juce::Rectangle<float> labelRect(0, getHeight() - 20, getWidth(), 20);
    g.drawText(labelText, labelRect, juce::Justification::centred);*/
}


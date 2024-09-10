#include <JuceHeader.h>

#pragma once

// Custom LookAndFeel class to provide a unique look to UI components
class CustomLookAndFeel : public juce::LookAndFeel_V4 {
public:
    // Constructor to initialize resources for custom styling
    CustomLookAndFeel();
    void drawGridBackground(juce::Graphics& g, int width, int height, int gridSize);
    void applyRotation(juce::Component& component, float angle);
    juce::Path createRoundedRectangle(float x, float y, float width, float height, float cornerSize);
    void animateComponentPosition(juce::Component& component, juce::Point<int> targetPosition, int durationMs);
    juce::Image applyReflectionEffect(const juce::Image& sourceImage);
    juce::Image generateNoiseTexture(int width, int height);
    void createCustomDropShadow(juce::Drawable& drawable, juce::Colour shadowColour, int shadowSize, juce::Point<int> offset);


    // Draws the background of the table header component with custom styling
    void drawTableHeaderBackground(juce::Graphics& g, juce::TableHeaderComponent& header) override;

    // Draws a linear slider with customized appearance
    void drawLinearSlider(juce::Graphics& g, int x, int y, int width, int height,
        float sliderPos, float minSliderPos, float maxSliderPos,
        const juce::Slider::SliderStyle style, juce::Slider& slider) override;

    // Draws a rotary slider with customized appearance
    void drawRotarySlider(juce::Graphics& g, int x, int y, int width, int height,
        float sliderPos, const float rotaryStartAngle, const float rotaryEndAngle,
        juce::Slider& slider) override;

private:
    // Drawable for horizontal slider knob appearance
    std::unique_ptr<juce::Drawable> horizontalKnobDrawable;

    // Drawable for vertical slider knob appearance
    std::unique_ptr<juce::Drawable> verticalKnobDrawable;
};

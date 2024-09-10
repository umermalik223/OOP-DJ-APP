#include "CustomLookAndFeel.h"

// Constructor to load SVG images and initialize knob graphics
CustomLookAndFeel::CustomLookAndFeel() {
    // Parse SVG data for vertical slider knob and create drawable
    const std::unique_ptr<juce::XmlElement> verticalKnobXml(juce::XmlDocument::parse(BinaryData::verticalKnob_svg));
    verticalKnobDrawable = juce::Drawable::createFromSVG(*verticalKnobXml);

    // Parse SVG data for horizontal slider knob and create drawable
    const std::unique_ptr<juce::XmlElement> horizontalKnobXml(juce::XmlDocument::parse(BinaryData::horizontalKnob_svg));
    horizontalKnobDrawable = juce::Drawable::createFromSVG(*horizontalKnobXml);
}

// Custom implementation to draw a linear slider
void CustomLookAndFeel::drawLinearSlider(juce::Graphics& g, int xPos, int yPos, int width, int height,
    float sliderPosition, float minPosition, float maxPosition,
    const juce::Slider::SliderStyle sliderStyle, juce::Slider& slider) {

    if (slider.isBar()) {
        // Draw filled rectangle for bar slider
        g.setColour(slider.findColour(juce::Slider::trackColourId));
        if (slider.isHorizontal()) {
            g.fillRect(juce::Rectangle<float>(static_cast<float>(xPos), static_cast<float>(yPos) + 0.5f, sliderPosition - static_cast<float>(xPos), static_cast<float>(height) - 1.0f));
        }
        else {
            g.fillRect(juce::Rectangle<float>(static_cast<float>(xPos) + 0.5f, sliderPosition, static_cast<float>(width) - 1.0f, static_cast<float>(yPos) + (static_cast<float>(height) - sliderPosition)));
        }
    }
    else {
        // Determine slider type
        bool isTwoValue = (sliderStyle == juce::Slider::SliderStyle::TwoValueVertical || sliderStyle == juce::Slider::SliderStyle::TwoValueHorizontal);
        bool isThreeValue = (sliderStyle == juce::Slider::SliderStyle::ThreeValueVertical || sliderStyle == juce::Slider::SliderStyle::ThreeValueHorizontal);

        // Define track width based on slider orientation
        auto trackThickness = juce::jmin(1.5f, slider.isHorizontal() ? static_cast<float>(height) * 0.25f : static_cast<float>(width) * 0.05f);

        // Calculate start and end points of the track
        juce::Point<float> startPoint(slider.isHorizontal() ? static_cast<float>(xPos) : static_cast<float>(xPos) + static_cast<float>(width) * 0.5f,
            slider.isHorizontal() ? static_cast<float>(yPos) + static_cast<float>(height) * 0.5f : static_cast<float>(height + yPos));
        juce::Point<float> endPoint(slider.isHorizontal() ? static_cast<float>(width + xPos) : startPoint.x,
            slider.isHorizontal() ? startPoint.y : static_cast<float>(yPos));

        // Draw background track
        juce::Path trackPath;
        trackPath.startNewSubPath(startPoint);
        trackPath.lineTo(endPoint);
        g.setColour(juce::Colours::white);
        g.strokePath(trackPath, { trackThickness, juce::PathStrokeType::curved, juce::PathStrokeType::rounded });

        // Prepare to draw value track
        juce::Path valuePath;
        juce::Point<float> minPoint, maxPoint, thumbPoint;

        if (isTwoValue || isThreeValue) {
            minPoint = { slider.isHorizontal() ? minPosition : static_cast<float>(width) * 0.5f,
                         slider.isHorizontal() ? static_cast<float>(height) * 0.5f : minPosition };

            if (isThreeValue) {
                thumbPoint = { slider.isHorizontal() ? sliderPosition : static_cast<float>(width) * 0.5f,
                               slider.isHorizontal() ? static_cast<float>(height) * 0.5f : sliderPosition };
            }

            maxPoint = { slider.isHorizontal() ? maxPosition : static_cast<float>(width) * 0.5f,
                         slider.isHorizontal() ? static_cast<float>(height) * 0.5f : maxPosition };
        }
        else {
            auto posX = slider.isHorizontal() ? sliderPosition : static_cast<float>(xPos) + static_cast<float>(width) * 0.5f;
            auto posY = slider.isHorizontal() ? static_cast<float>(yPos) + static_cast<float>(height) * 0.5f : sliderPosition;

            minPoint = startPoint;
            maxPoint = { posX, posY };
        }

        // Draw value track
        valuePath.startNewSubPath(minPoint);
        valuePath.lineTo(isThreeValue ? thumbPoint : maxPoint);
        g.setColour(juce::Colours::white);
        g.strokePath(valuePath, { trackThickness, juce::PathStrokeType::curved, juce::PathStrokeType::rounded });

        if (!isTwoValue) {
            // Draw tick marks on the slider
            if (slider.isHorizontal()) {
                for (float tickX = xPos; tickX < xPos + width + 3; tickX += (width + 3) / 5) {
                    g.drawLine(tickX, startPoint.getY() - height / 3, tickX, endPoint.getY() + height / 3);
                }
                horizontalKnobDrawable->drawWithin(g, juce::Rectangle<float>(static_cast<float>(getSliderThumbRadius(slider) * 2.5), static_cast<float>(getSliderThumbRadius(slider) * 2.5)).withCentre(isThreeValue ? thumbPoint : maxPoint), juce::RectanglePlacement::centred, 1.0);
            }
            else {
                for (float tickY = yPos; tickY < yPos + height + 4; tickY += (height + 4) / 5) {
                    g.drawLine(startPoint.getX() - trackThickness * 6, tickY, endPoint.getX() + trackThickness * 6, tickY);
                }
                g.setColour(juce::Colours::white);
                g.fillRect(juce::Rectangle<float>(static_cast<float>(getSliderThumbRadius(slider) * 1.2), static_cast<float>(getSliderThumbRadius(slider) * 2)).withCentre(isThreeValue ? thumbPoint : maxPoint));
                verticalKnobDrawable->drawWithin(g, juce::Rectangle<float>(static_cast<float>(getSliderThumbRadius(slider) * 2.5), static_cast<float>(getSliderThumbRadius(slider) * 2.5)).withCentre(isThreeValue ? thumbPoint : maxPoint), juce::RectanglePlacement::centred, 1.0);
            }
        }

        // Draw pointers for two-value or three-value sliders
        if (isTwoValue || isThreeValue) {
            auto pointerSize = juce::jmin(trackThickness, (slider.isHorizontal() ? static_cast<float>(height) : static_cast<float>(width)) * 0.4f);
            auto pointerColour = slider.findColour(juce::Slider::thumbColourId);

            if (slider.isHorizontal()) {
                drawPointer(g, minPosition - pointerSize, juce::jmax(0.0f, static_cast<float>(yPos) + static_cast<float>(height) * 0.5f - trackThickness * 2.0f), trackThickness * 2.0f, pointerColour, 2);
                drawPointer(g, maxPosition - trackThickness, juce::jmin(static_cast<float>(yPos + height) - trackThickness * 2.0f, static_cast<float>(yPos) + static_cast<float>(height) * 0.5f), trackThickness * 2.0f, pointerColour, 4);
            }
            else {
                drawPointer(g, juce::jmax(0.0f, static_cast<float>(xPos) + static_cast<float>(width) * 0.5f - trackThickness * 2.0f), minPosition - trackThickness, trackThickness * 2.0f, pointerColour, 1);
                drawPointer(g, juce::jmin(static_cast<float>(xPos + width) - trackThickness * 2.0f, static_cast<float>(xPos) + static_cast<float>(width) * 0.5f), maxPosition - pointerSize, trackThickness * 2.0f, pointerColour, 3);
            }
        }
    }
}


// Custom implementation to draw a rotary slider
void CustomLookAndFeel::drawRotarySlider(juce::Graphics& g, int xPos, int yPos, int width, int height,
    float sliderPosition, const float startAngle, const float endAngle,
    juce::Slider& slider) {

    // Get colors for the rotary slider
    auto outlineColor = slider.findColour(juce::Slider::rotarySliderOutlineColourId);
    auto fillColor = slider.findColour(juce::Slider::rotarySliderFillColourId);

    // Define bounds and radius
    auto sliderBounds = juce::Rectangle<int>(xPos, yPos, width, height).toFloat().reduced(10);
    auto sliderRadius = juce::jmin(sliderBounds.getWidth(), sliderBounds.getHeight()) / 2.0f;

    // Calculate the angle for the slider position
    auto targetAngle = startAngle + sliderPosition * (endAngle - startAngle);
    auto strokeWidth = juce::jmin(8.0f, sliderRadius * 0.5f);
    auto arcRadius = sliderRadius - strokeWidth * 0.5f;
    auto thumbDiameter = sliderRadius * 2.0f;

    // Calculate the end point of the rotary slider's pointer
    juce::Point<float> pointerEndPoint(
        sliderBounds.getCentreX() + arcRadius * std::cos(targetAngle - juce::MathConstants<float>::halfPi),
        sliderBounds.getCentreY() + arcRadius * std::sin(targetAngle - juce::MathConstants<float>::halfPi)
    );

    // Center point for the thumb
    juce::Point<float> thumbCenter(xPos + width / 2, yPos + height / 2);

    // Draw markers around the slider
    int markerCount = 0;
    for (auto angle = startAngle; angle < endAngle; angle += (endAngle - startAngle) / 10) {
        juce::Point<float> markerStart(
            sliderBounds.getCentreX() + arcRadius * 1.5f * std::cos(angle - juce::MathConstants<float>::halfPi),
            sliderBounds.getCentreY() + arcRadius * 1.7f * std::sin(angle - juce::MathConstants<float>::halfPi)
        );
        float markerScale = 2.0f;
        juce::Point<float> markerEnd(
            sliderBounds.getCentreX() + arcRadius * markerScale * std::cos(angle - juce::MathConstants<float>::halfPi),
            sliderBounds.getCentreY() + arcRadius * markerScale * std::sin(angle - juce::MathConstants<float>::halfPi)
        );

        juce::Path markerPath;
        markerPath.startNewSubPath(markerStart);
        markerPath.lineTo(markerEnd);
        g.setColour(juce::Colour::fromRGBA(255, 255, 255, 255));

        // Draw markers with varying stroke width
        if (markerCount % 5 == 0) {
            g.strokePath(markerPath, juce::PathStrokeType(2));
        }
        else {
            g.strokePath(markerPath, juce::PathStrokeType(1));
        }

        markerCount++;
    }

    // Draw the rotary slider thumb and pointer
    juce::Path pointerPath;
    pointerPath.startNewSubPath(thumbCenter);
    pointerPath.lineTo(pointerEndPoint);
    g.setColour(juce::Colour::fromRGBA(25, 25, 25, 255));
    g.fillEllipse(juce::Rectangle<float>(thumbDiameter, thumbDiameter).withCentre(thumbCenter));
    g.setColour(juce::Colour::fromRGBA(255, 255, 255, 255));
    g.strokePath(pointerPath, juce::PathStrokeType(3));
}


// Custom implementation to draw the background of a table header
void CustomLookAndFeel::drawTableHeaderBackground(juce::Graphics& g, juce::TableHeaderComponent& header) {
    auto bounds = header.getLocalBounds();
    auto outlineColour = header.findColour(juce::TableHeaderComponent::outlineColourId);

    // Draw bottom border
    g.setColour(outlineColour);
    g.fillRect(bounds.removeFromBottom(1));

    // Fill background color
    g.setColour(juce::Colour::fromRGBA(200, 200, 200, 255));
    g.fillRect(bounds);

    // Draw vertical dividers between columns
    g.setColour(outlineColour);
    for (int i = header.getNumColumns(true); --i >= 0;) {
        g.fillRect(header.getColumnPosition(i).removeFromRight(1));
    }
}
// Function to apply a custom shadow effect to components
void applyShadowEffect(juce::Component& component) {
    juce::DropShadow shadow(juce::Colours::black.withAlpha(0.5f), 5, juce::Point<int>(2, 2));
    //juce::DropShadower shadower(&shadow);
    //shadower.setOwner(&component);
}

// Function to calculate the gradient color based on the component size
juce::ColourGradient calculateGradientColor(int width, int height) {
    juce::ColourGradient gradient(juce::Colours::white, 0, 0, juce::Colours::black, width, height, false);
    gradient.addColour(0.5, juce::Colours::grey);
    return gradient;
}

// Function to create a custom font for text rendering
juce::Font createCustomFont(float fontSize) {
    juce::Font customFont("Arial", fontSize, juce::Font::bold);
    return customFont;
}

// Function to set a custom border around a component
void setCustomBorder(juce::Component& component) {
    //component.setBorderSize(juce::BorderSize<int>(4));
}

// Function to generate a random color
juce::Colour generateRandomColour() {
    return juce::Colour::fromRGB(juce::Random::getSystemRandom().nextInt(256),
        juce::Random::getSystemRandom().nextInt(256),
        juce::Random::getSystemRandom().nextInt(256));
}

// Function to add a custom button listener
void addCustomButtonListener(juce::Button& button, juce::Button::Listener* listener) {
    button.addListener(listener);
}

// Function to reset the graphics context
void resetGraphicsContext(juce::Graphics& g) {
    g.resetToDefaultState();
}





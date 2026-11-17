#include "PluginEditor.h"

#include "BinaryData.h"

//==============================================================================
ViolentLookAndFeel::ViolentLookAndFeel()
{
    setColour(juce::Slider::textBoxTextColourId, juce::Colours::white);
}

void ViolentLookAndFeel::drawPill(juce::Graphics& g, juce::Rectangle<float> area,
                                  const juce::String& text)
{
    g.setColour(juce::Colour(pillColour));
    g.fillRoundedRectangle(area, area.getHeight() * 0.5f);
    g.setColour(juce::Colour(pillTextColour));
    g.setFont(juce::FontOptions(area.getHeight() * 0.62f, juce::Font::bold));
    g.drawText(text, area, juce::Justification::centred);
}

void ViolentLookAndFeel::drawRotarySlider(juce::Graphics& g, int x, int y, int width, int height,
                                          float sliderPos, float rotaryStartAngle,
                                          float rotaryEndAngle, juce::Slider& slider)
{
    const auto bounds = juce::Rectangle<float>((float) x, (float) y, (float) width, (float) height);
    const auto centre = bounds.getCentre();
    const float radius = juce::jmin(bounds.getWidth(), bounds.getHeight()) * 0.33f;
    const auto knobBounds = juce::Rectangle<float>(radius * 2.0f, radius * 2.0f).withCentre(centre);

    // Knob body
    g.setColour(juce::Colour(knobEdgeColour));
    g.fillEllipse(knobBounds.expanded(2.0f));
    g.setColour(juce::Colour(knobColour));
    g.fillEllipse(knobBounds);

    // Parameter name inside the knob
    g.setColour(juce::Colours::white);
    g.setFont(juce::FontOptions(radius * 0.30f, juce::Font::bold));
    g.drawFittedText(slider.getName().toUpperCase(),
                     knobBounds.reduced(radius * 0.22f).toNearestInt(),
                     juce::Justification::centred, 2);

    // Position dot
    const float angle = rotaryStartAngle + sliderPos * (rotaryEndAngle - rotaryStartAngle);
    const auto dotCentre = centre.getPointOnCircumference(radius * 0.80f, angle);
    g.setColour(juce::Colours::white);
    g.fillEllipse(juce::Rectangle<float>(radius * 0.18f, radius * 0.18f).withCentre(dotCentre));

    // Min/max arrows just outside the knob
    g.setColour(juce::Colours::grey);
    for (const float a : { rotaryStartAngle, rotaryEndAngle })
    {
        const auto tip  = centre.getPointOnCircumference(radius * 0.92f, a);
        const auto base = centre.getPointOnCircumference(radius * 1.12f, a);
        juce::Path arrow;
        const auto perp = juce::Point<float>(-(tip.y - base.y), tip.x - base.x) * 0.35f;
        arrow.addTriangle(tip, base + perp, base - perp);
        g.fillPath(arrow);
    }

    // Value pill above, range pills at the lower corners
    const float pillH = juce::jmax(16.0f, radius * 0.30f);

    drawPill(g, juce::Rectangle<float>(radius * 1.1f, pillH)
                    .withCentre({ centre.x, knobBounds.getY() - pillH * 0.75f }),
             slider.getTextFromValue(slider.getValue()));

    const auto minPos = centre.getPointOnCircumference(radius * 1.45f, rotaryStartAngle);
    const auto maxPos = centre.getPointOnCircumference(radius * 1.45f, rotaryEndAngle);

    drawPill(g, juce::Rectangle<float>(radius * 0.85f, pillH * 0.85f).withCentre(minPos),
             juce::String(slider.getMinimum(), slider.getMinimum() < 1.0 ? 1 : 0));
    drawPill(g, juce::Rectangle<float>(radius * 0.85f, pillH * 0.85f).withCentre(maxPos),
             juce::String(slider.getMaximum(), 0));
}

//==============================================================================
VstChorusEditor::VstChorusEditor(VstChorusProcessor& p)
    : AudioProcessorEditor(&p), processorRef(p)
{
    logo = juce::ImageCache::getFromMemory(BinaryData::logo_png, BinaryData::logo_pngSize);

    setupKnob(rate, "rate", "Rate");
    setupKnob(mix, "mix", "Mix");
    setupKnob(depth, "depth", "Depth");
    setupKnob(centreDelay, "centreDelay", "Center Delay");
    setupKnob(feedback, "feedback", "Feedback");

    setLookAndFeel(&lookAndFeel);
    setSize(680, 470);
}

VstChorusEditor::~VstChorusEditor()
{
    setLookAndFeel(nullptr);
}

void VstChorusEditor::setupKnob(Knob& knob, const juce::String& paramID,
                                const juce::String& displayName)
{
    knob.slider.setSliderStyle(juce::Slider::RotaryVerticalDrag);
    knob.slider.setTextBoxStyle(juce::Slider::NoTextBox, true, 0, 0);
    knob.slider.setName(displayName);
    knob.attachment = std::make_unique<SliderAttachment>(
        processorRef.getAPVTS(), paramID, knob.slider);
    addAndMakeVisible(knob.slider);
}

void VstChorusEditor::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colour(ViolentLookAndFeel::backgroundColour));

    if (logo.isValid())
        g.drawImage(logo, logoArea.toFloat(), juce::RectanglePlacement::centred);
}

void VstChorusEditor::resized()
{
    auto area = getLocalBounds().reduced(20);

    auto topRow = area.removeFromTop(area.getHeight() / 2);
    const int topCol = topRow.getWidth() / 3;

    rate.slider.setBounds(topRow.removeFromLeft(topCol));
    logoArea = topRow.removeFromLeft(topCol).reduced(8, 24);
    mix.slider.setBounds(topRow);

    const int bottomCol = area.getWidth() / 3;
    depth.slider.setBounds(area.removeFromLeft(bottomCol));
    centreDelay.slider.setBounds(area.removeFromLeft(bottomCol));
    feedback.slider.setBounds(area);
}

#pragma once

#include "PluginProcessor.h"

// Draws the maroon knobs from the Violent Inc. design: label inside the knob,
// white position dot, min/max arrows, and value/range pills around it.
class ViolentLookAndFeel : public juce::LookAndFeel_V4
{
public:
    ViolentLookAndFeel();

    void drawRotarySlider(juce::Graphics&, int x, int y, int width, int height,
                          float sliderPosProportional, float rotaryStartAngle,
                          float rotaryEndAngle, juce::Slider&) override;

    static constexpr juce::uint32 backgroundColour = 0xff0b0b0b;
    static constexpr juce::uint32 knobColour       = 0xff7e2220;
    static constexpr juce::uint32 knobEdgeColour   = 0xff581715;
    static constexpr juce::uint32 pillColour       = 0xfff2f2f2;
    static constexpr juce::uint32 pillTextColour   = 0xff7e2220;

private:
    void drawPill(juce::Graphics&, juce::Rectangle<float> area, const juce::String& text);
};

class VstChorusEditor : public juce::AudioProcessorEditor
{
public:
    explicit VstChorusEditor(VstChorusProcessor&);
    ~VstChorusEditor() override;

    void paint(juce::Graphics&) override;
    void resized() override;

private:
    using SliderAttachment = juce::AudioProcessorValueTreeState::SliderAttachment;

    struct Knob
    {
        juce::Slider slider;
        std::unique_ptr<SliderAttachment> attachment;
    };

    void setupKnob(Knob&, const juce::String& paramID, const juce::String& displayName);

    VstChorusProcessor& processorRef;
    ViolentLookAndFeel lookAndFeel;
    juce::Image logo;

    Knob rate, depth, centreDelay, feedback, mix;
    juce::Rectangle<int> logoArea;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(VstChorusEditor)
};

#include "PluginProcessor.h"
#include "PluginEditor.h"

VstChorusProcessor::VstChorusProcessor()
    : AudioProcessor(BusesProperties()
                         .withInput("Input", juce::AudioChannelSet::stereo(), true)
                         .withOutput("Output", juce::AudioChannelSet::stereo(), true)),
      apvts(*this, nullptr, "Parameters", createParameterLayout())
{
    rateParam        = apvts.getRawParameterValue("rate");
    depthParam       = apvts.getRawParameterValue("depth");
    centreDelayParam = apvts.getRawParameterValue("centreDelay");
    feedbackParam    = apvts.getRawParameterValue("feedback");
    mixParam         = apvts.getRawParameterValue("mix");
}

juce::AudioProcessorValueTreeState::ParameterLayout VstChorusProcessor::createParameterLayout()
{
    using namespace juce;

    AudioProcessorValueTreeState::ParameterLayout layout;

    layout.add(std::make_unique<AudioParameterFloat>(
        ParameterID { "rate", 1 }, "Rate",
        NormalisableRange<float>(0.1f, 10.0f, 0.01f, 0.4f), 1.0f,
        AudioParameterFloatAttributes().withLabel("Hz")));

    layout.add(std::make_unique<AudioParameterFloat>(
        ParameterID { "depth", 1 }, "Depth",
        NormalisableRange<float>(0.0f, 1.0f, 0.01f), 0.25f));

    layout.add(std::make_unique<AudioParameterFloat>(
        ParameterID { "centreDelay", 1 }, "Centre Delay",
        NormalisableRange<float>(1.0f, 60.0f, 0.1f), 7.0f,
        AudioParameterFloatAttributes().withLabel("ms")));

    layout.add(std::make_unique<AudioParameterFloat>(
        ParameterID { "feedback", 1 }, "Feedback",
        NormalisableRange<float>(-0.95f, 0.95f, 0.01f), 0.0f));

    layout.add(std::make_unique<AudioParameterFloat>(
        ParameterID { "mix", 1 }, "Mix",
        NormalisableRange<float>(0.0f, 1.0f, 0.01f), 0.5f));

    return layout;
}

void VstChorusProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    juce::dsp::ProcessSpec spec;
    spec.sampleRate = sampleRate;
    spec.maximumBlockSize = static_cast<juce::uint32>(samplesPerBlock);
    spec.numChannels = static_cast<juce::uint32>(getTotalNumOutputChannels());

    chorus.prepare(spec);
    chorus.reset();
}

void VstChorusProcessor::releaseResources()
{
    chorus.reset();
}

bool VstChorusProcessor::isBusesLayoutSupported(const BusesLayout& layouts) const
{
    const auto& mainOut = layouts.getMainOutputChannelSet();

    if (mainOut != juce::AudioChannelSet::mono() && mainOut != juce::AudioChannelSet::stereo())
        return false;

    return mainOut == layouts.getMainInputChannelSet();
}

void VstChorusProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer&)
{
    juce::ScopedNoDenormals noDenormals;

    for (auto i = getTotalNumInputChannels(); i < getTotalNumOutputChannels(); ++i)
        buffer.clear(i, 0, buffer.getNumSamples());

    chorus.setRate(rateParam->load());
    chorus.setDepth(depthParam->load());
    chorus.setCentreDelay(centreDelayParam->load());
    chorus.setFeedback(feedbackParam->load());
    chorus.setMix(mixParam->load());

    juce::dsp::AudioBlock<float> block(buffer);
    chorus.process(juce::dsp::ProcessContextReplacing<float>(block));
}

juce::AudioProcessorEditor* VstChorusProcessor::createEditor()
{
    return new VstChorusEditor(*this);
}

void VstChorusProcessor::getStateInformation(juce::MemoryBlock& destData)
{
    if (auto xml = apvts.copyState().createXml())
        copyXmlToBinary(*xml, destData);
}

void VstChorusProcessor::setStateInformation(const void* data, int sizeInBytes)
{
    if (auto xml = getXmlFromBinary(data, sizeInBytes))
        apvts.replaceState(juce::ValueTree::fromXml(*xml));
}

// This creates the plugin instance for the host
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new VstChorusProcessor();
}

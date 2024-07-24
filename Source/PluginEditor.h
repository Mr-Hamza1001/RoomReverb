/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "RoomRender.h"

//==============================================================================
/**
*/
class RoomReverbPluginAudioProcessorEditor  : public juce::AudioProcessorEditor
{
public:
    RoomReverbPluginAudioProcessorEditor (RoomReverbPluginAudioProcessor&);
    ~RoomReverbPluginAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    RoomReverbPluginAudioProcessor& audioProcessor;

    RoomRender roomRender;
    //juce::TextButton button1{ "Button 1" };
    //juce::TextButton button2{ "Button 2" };
    juce::Slider slider1{ juce::Slider::LinearHorizontal, juce::Slider::TextBoxBelow };
    juce::Slider slider2{ juce::Slider::LinearHorizontal, juce::Slider::TextBoxBelow };
    juce::Slider slider3{ juce::Slider::LinearHorizontal, juce::Slider::TextBoxBelow };

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (RoomReverbPluginAudioProcessorEditor)
};

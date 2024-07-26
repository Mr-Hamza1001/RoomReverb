/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "RoomRender.h"
#include "ProcessReflections.h"

//==============================================================================
/**
*/
class RoomReverbPluginAudioProcessorEditor  : public juce::AudioProcessorEditor, public juce::Button::Listener
{
public:
    RoomReverbPluginAudioProcessorEditor (RoomReverbPluginAudioProcessor&);
    ~RoomReverbPluginAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;
    void buttonClicked(juce::Button* button) override;

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    RoomReverbPluginAudioProcessor& audioProcessor;

    RoomRender roomRender;
    ProcessReflections processReflections;

    juce::TextButton buttonProcess{ "Process.." };
    juce::TextButton button2{ "Button 2" };
    juce::Slider slider1{ juce::Slider::Rotary, juce::Slider::TextBoxBelow };
    juce::Slider slider2{ juce::Slider::Rotary, juce::Slider::TextBoxBelow };
    juce::Slider slider3{ juce::Slider::Rotary, juce::Slider::TextBoxBelow };

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (RoomReverbPluginAudioProcessorEditor)
};

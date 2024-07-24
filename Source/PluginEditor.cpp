/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
RoomReverbPluginAudioProcessorEditor::RoomReverbPluginAudioProcessorEditor (RoomReverbPluginAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    //Make room window visible
    addAndMakeVisible(roomRender);
    //addAndMakeVisible(button1);
    //addAndMakeVisible(button2);
    addAndMakeVisible(slider1);
    addAndMakeVisible(slider2);
    addAndMakeVisible(slider3);


    //Set size of main window
    setSize(1000, 600);

    juce::Grid grid;
    using Track = juce::Grid::TrackInfo;

    grid.templateRows = { Track(1_fr), Track(1_fr), Track(1_fr) };
    grid.templateColumns = { Track(1_fr), Track(1_fr), Track(1_fr), Track(1_fr) };

    grid.items = { juce::GridItem(slider1).withArea(juce::GridItem::Span(1), juce::GridItem::Span(1)),
                   juce::GridItem(roomRender).withArea(juce::GridItem::Span(2), juce::GridItem::Span(2)),
                   juce::GridItem(slider2).withArea(juce::GridItem::Span(1), juce::GridItem::Span(1)),
                   juce::GridItem(slider3).withArea(juce::GridItem::Span(1), juce::GridItem::Span(1)) };

    grid.performLayout(getLocalBounds());
}

RoomReverbPluginAudioProcessorEditor::~RoomReverbPluginAudioProcessorEditor()
{
}

//==============================================================================
void RoomReverbPluginAudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));

    g.setColour (juce::Colours::white);
    g.setFont (juce::FontOptions (15.0f));
    g.drawFittedText ("Hello World!", getLocalBounds(), juce::Justification::centred, 1);
}

void RoomReverbPluginAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
}

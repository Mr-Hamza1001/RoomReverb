/*
 * Copyright (c) 2025 James G. Stanier
 *
 * This file is part of RoomReverbPlugin.
 *
 * This software is dual-licensed under:
 *   1. The GNU General Public License v3.0 (GPLv3)
 *   2. A commercial license (contact j.stanier766(at)gmail.com for details)
 *
 * You may use this file under the terms of the GPLv3 as published by
 * the Free Software Foundation. For proprietary/commercial use,
 * please see the LICENSE-COMMERCIAL file or contact the copyright holder.
 */
 
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
    buttonProcess.addListener(this);
    addAndMakeVisible(roomRender);
    addAndMakeVisible(buttonProcess);
    addAndMakeVisible(button2);
    addAndMakeVisible(slider1);
    addAndMakeVisible(slider2);
    addAndMakeVisible(slider3);


    //Set size of main window
    setSize(1000, 600);

    juce::Grid grid;
    using Track = juce::Grid::TrackInfo;

    grid.templateRows = { Track(1_fr), Track(1_fr), Track(1_fr), Track(1_fr), Track(1_fr), Track(1_fr) };
    grid.templateColumns = { Track(1_fr), Track(1_fr), Track(1_fr), Track(1_fr), Track(1_fr), Track(1_fr), Track(1_fr), Track(1_fr), Track(1_fr), Track(1_fr) };

    grid.items = { juce::GridItem(slider1).withArea(juce::GridItem::Span(1), juce::GridItem::Span(1)),
                   juce::GridItem(slider2).withArea(juce::GridItem::Span(1), juce::GridItem::Span(1)),
                   juce::GridItem(roomRender).withArea(juce::GridItem::Span(5), juce::GridItem::Span(5)),
                   juce::GridItem(slider3).withArea(juce::GridItem::Span(1), juce::GridItem::Span(1)),
                   juce::GridItem(buttonProcess).withArea(juce::GridItem::Span(1), juce::GridItem::Span(1)),
                   juce::GridItem(button2).withArea(juce::GridItem::Span(1), juce::GridItem::Span(1)) };

    grid.performLayout(getLocalBounds());
}

RoomReverbPluginAudioProcessorEditor::~RoomReverbPluginAudioProcessorEditor()
{
    buttonProcess.removeListener(this);

    processReflections.stopThread(1000);
}

//==============================================================================
void RoomReverbPluginAudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));

    g.setColour (juce::Colours::white);
    g.setFont (juce::FontOptions (15.0f));
    g.drawFittedText ("Please populate me!", getLocalBounds(), juce::Justification::centredBottom, 1);
}

void RoomReverbPluginAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
    
}

void RoomReverbPluginAudioProcessorEditor::buttonClicked(juce::Button* button)
{
    if (button == &buttonProcess)
    {
        DBG("Process button pressed!");

        processReflections.startThread();

    }
}

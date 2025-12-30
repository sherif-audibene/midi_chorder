#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
AudioPluginAudioProcessorEditor::AudioPluginAudioProcessorEditor (AudioPluginAudioProcessor& p)
    : AudioProcessorEditor (&p), 
      processorRef (p),
      midiKeyboard (p.keyboardState, juce::MidiKeyboardComponent::horizontalKeyboard)
{
    // Configure the keyboard appearance
    midiKeyboard.setKeyWidth (40.0f);
    midiKeyboard.setAvailableRange (36, 96); // C2 to C7
    addAndMakeVisible (midiKeyboard);

    setSize (800, 200);
    
    // Start timer to refresh keyboard display
    startTimerHz (30);
}

AudioPluginAudioProcessorEditor::~AudioPluginAudioProcessorEditor()
{
    stopTimer();
}

//==============================================================================
void AudioPluginAudioProcessorEditor::paint (juce::Graphics& g)
{
    g.fillAll (juce::Colours::darkgrey);

    g.setColour (juce::Colours::white);
    g.setFont (18.0f);
    g.drawText ("MIDI Chord Generator - Output Notes", 
                getLocalBounds().removeFromTop (30), 
                juce::Justification::centred, true);
}

void AudioPluginAudioProcessorEditor::resized()
{
    auto bounds = getLocalBounds();
    bounds.removeFromTop (35); // Space for title
    midiKeyboard.setBounds (bounds);
}

void AudioPluginAudioProcessorEditor::timerCallback()
{
    // Repaint keyboard to show current notes
    midiKeyboard.repaint();
}

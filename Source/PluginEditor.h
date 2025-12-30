#pragma once

#include "PluginProcessor.h"
#include <juce_audio_utils/juce_audio_utils.h>

//==============================================================================
class AudioPluginAudioProcessorEditor final : public juce::AudioProcessorEditor,
                                               private juce::Timer
{
public:
    explicit AudioPluginAudioProcessorEditor (AudioPluginAudioProcessor&);
    ~AudioPluginAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;
    void timerCallback() override;

private:
    AudioPluginAudioProcessor& processorRef;
    
    // Pattern selector
    juce::ComboBox patternSelector;
    juce::Label patternLabel { {}, "Rhythm:" };
    
    // Chord type selector
    juce::ComboBox chordTypeSelector;
    juce::Label chordTypeLabel { {}, "Chord:" };
    
    // Tempo control
    juce::Slider tempoSlider;
    juce::Label tempoLabel { {}, "Tempo:" };
    
    // Enable/Disable button
    juce::TextButton enableButton { "ON" };
    
    // MIDI keyboard to visualize output notes
    juce::MidiKeyboardComponent midiKeyboard;
    
    // Style helpers
    void setupComboBox (juce::ComboBox& box);
    void setupLabel (juce::Label& label);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AudioPluginAudioProcessorEditor)
};

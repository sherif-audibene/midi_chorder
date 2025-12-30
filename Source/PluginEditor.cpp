#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
AudioPluginAudioProcessorEditor::AudioPluginAudioProcessorEditor (AudioPluginAudioProcessor& p)
    : AudioProcessorEditor (&p), 
      processorRef (p),
      midiKeyboard (p.keyboardState, juce::MidiKeyboardComponent::horizontalKeyboard)
{
    // Pattern selector setup
    setupLabel (patternLabel);
    addAndMakeVisible (patternLabel);
    
    patternSelector.addItemList (processorRef.getPatternNames(), 1);
    patternSelector.setSelectedId (processorRef.currentPatternIndex.load() + 1, juce::dontSendNotification);
    patternSelector.onChange = [this] {
        processorRef.currentPatternIndex.store (patternSelector.getSelectedId() - 1);
    };
    setupComboBox (patternSelector);
    addAndMakeVisible (patternSelector);
    
    // Chord type selector setup
    setupLabel (chordTypeLabel);
    addAndMakeVisible (chordTypeLabel);
    
    chordTypeSelector.addItemList (AudioPluginAudioProcessor::chordTypeNames, 1);
    chordTypeSelector.setSelectedId (processorRef.chordTypeIndex.load() + 1, juce::dontSendNotification);
    chordTypeSelector.onChange = [this] {
        processorRef.chordTypeIndex.store (chordTypeSelector.getSelectedId() - 1);
    };
    setupComboBox (chordTypeSelector);
    addAndMakeVisible (chordTypeSelector);
    
    // Tempo slider setup
    setupLabel (tempoLabel);
    addAndMakeVisible (tempoLabel);
    
    tempoSlider.setRange (40.0, 240.0, 1.0);
    tempoSlider.setValue (processorRef.internalTempo.load(), juce::dontSendNotification);
    tempoSlider.setTextBoxStyle (juce::Slider::TextBoxRight, false, 50, 25);
    tempoSlider.setColour (juce::Slider::thumbColourId, juce::Colour (0xffff6b6b));
    tempoSlider.setColour (juce::Slider::trackColourId, juce::Colour (0xff4a4a6a));
    tempoSlider.setColour (juce::Slider::backgroundColourId, juce::Colour (0xff2a2a4a));
    tempoSlider.setColour (juce::Slider::textBoxTextColourId, juce::Colours::white);
    tempoSlider.setColour (juce::Slider::textBoxOutlineColourId, juce::Colours::transparentBlack);
    tempoSlider.onValueChange = [this] {
        processorRef.internalTempo.store (static_cast<float> (tempoSlider.getValue()));
    };
    addAndMakeVisible (tempoSlider);
    
    // Enable button setup
    enableButton.setClickingTogglesState (true);
    enableButton.setToggleState (processorRef.patternEnabled.load(), juce::dontSendNotification);
    enableButton.setColour (juce::TextButton::buttonColourId, juce::Colour (0xff3a3a5a));
    enableButton.setColour (juce::TextButton::buttonOnColourId, juce::Colour (0xff4ecdc4));
    enableButton.setColour (juce::TextButton::textColourOnId, juce::Colour (0xff1a1a2e));
    enableButton.setColour (juce::TextButton::textColourOffId, juce::Colours::white);
    enableButton.onClick = [this] {
        bool isOn = enableButton.getToggleState();
        processorRef.patternEnabled.store (isOn);
        enableButton.setButtonText (isOn ? "ON" : "OFF");
    };
    addAndMakeVisible (enableButton);
    
    // MIDI keyboard setup
    midiKeyboard.setKeyWidth (35.0f);
    midiKeyboard.setAvailableRange (36, 96);
    midiKeyboard.setColour (juce::MidiKeyboardComponent::whiteNoteColourId, juce::Colour (0xfff0f0f0));
    midiKeyboard.setColour (juce::MidiKeyboardComponent::blackNoteColourId, juce::Colour (0xff2a2a4a));
    midiKeyboard.setColour (juce::MidiKeyboardComponent::keySeparatorLineColourId, juce::Colour (0xff3a3a5a));
    midiKeyboard.setColour (juce::MidiKeyboardComponent::mouseOverKeyOverlayColourId, juce::Colour (0x40ff6b6b));
    midiKeyboard.setColour (juce::MidiKeyboardComponent::keyDownOverlayColourId, juce::Colour (0xccff6b6b));
    addAndMakeVisible (midiKeyboard);

    setSize (850, 280);
    startTimerHz (30);
}

AudioPluginAudioProcessorEditor::~AudioPluginAudioProcessorEditor()
{
    stopTimer();
}

void AudioPluginAudioProcessorEditor::setupComboBox (juce::ComboBox& box)
{
    box.setColour (juce::ComboBox::backgroundColourId, juce::Colour (0xff2a2a4a));
    box.setColour (juce::ComboBox::textColourId, juce::Colours::white);
    box.setColour (juce::ComboBox::outlineColourId, juce::Colour (0xff4a4a6a));
    box.setColour (juce::ComboBox::arrowColourId, juce::Colour (0xff4ecdc4));
}

void AudioPluginAudioProcessorEditor::setupLabel (juce::Label& label)
{
    label.setFont (juce::FontOptions (14.0f).withStyle ("Bold"));
    label.setColour (juce::Label::textColourId, juce::Colour (0xffaaaacc));
    label.setJustificationType (juce::Justification::centredRight);
}

//==============================================================================
void AudioPluginAudioProcessorEditor::paint (juce::Graphics& g)
{
    // Dark gradient background
    juce::ColourGradient gradient (juce::Colour (0xff1a1a2e), 0.0f, 0.0f,
                                    juce::Colour (0xff16213e), 0.0f, static_cast<float> (getHeight()), false);
    g.setGradientFill (gradient);
    g.fillAll();
    
    // Title with accent color
    g.setColour (juce::Colour (0xff4ecdc4));
    g.setFont (juce::FontOptions (24.0f).withStyle ("Bold"));
    g.drawText ("CHORD PATTERN PLAYER", getLocalBounds().removeFromTop (45), 
                juce::Justification::centred, true);
    
    // Subtle separator line
    g.setColour (juce::Colour (0xff3a3a5a));
    g.drawLine (20.0f, 50.0f, static_cast<float> (getWidth() - 20), 50.0f, 1.0f);
    
    // Control panel background
    auto controlBounds = getLocalBounds().reduced (15).removeFromTop (100);
    controlBounds.removeFromTop (40);
    g.setColour (juce::Colour (0x20ffffff));
    g.fillRoundedRectangle (controlBounds.toFloat(), 8.0f);
}

void AudioPluginAudioProcessorEditor::resized()
{
    auto bounds = getLocalBounds().reduced (20);
    bounds.removeFromTop (55); // Space for title + separator
    
    // Control row
    auto controlRow = bounds.removeFromTop (45);
    controlRow.reduce (10, 8);
    
    // Pattern selector
    patternLabel.setBounds (controlRow.removeFromLeft (60));
    controlRow.removeFromLeft (5);
    patternSelector.setBounds (controlRow.removeFromLeft (130));
    
    controlRow.removeFromLeft (20);
    
    // Chord type selector  
    chordTypeLabel.setBounds (controlRow.removeFromLeft (55));
    controlRow.removeFromLeft (5);
    chordTypeSelector.setBounds (controlRow.removeFromLeft (100));
    
    controlRow.removeFromLeft (20);
    
    // Tempo slider
    tempoLabel.setBounds (controlRow.removeFromLeft (55));
    controlRow.removeFromLeft (5);
    tempoSlider.setBounds (controlRow.removeFromLeft (180));
    
    controlRow.removeFromLeft (20);
    
    // Enable button
    enableButton.setBounds (controlRow.removeFromLeft (60));
    
    bounds.removeFromTop (15);
    
    // MIDI keyboard
    midiKeyboard.setBounds (bounds);
}

void AudioPluginAudioProcessorEditor::timerCallback()
{
    midiKeyboard.repaint();
    
    // Sync UI with processor state
    if (patternSelector.getSelectedId() - 1 != processorRef.currentPatternIndex.load())
        patternSelector.setSelectedId (processorRef.currentPatternIndex.load() + 1, juce::dontSendNotification);
        
    if (chordTypeSelector.getSelectedId() - 1 != processorRef.chordTypeIndex.load())
        chordTypeSelector.setSelectedId (processorRef.chordTypeIndex.load() + 1, juce::dontSendNotification);
}

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
AudioPluginAudioProcessor::AudioPluginAudioProcessor()
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       )
{
    // Initialize all rhythm patterns
    patterns = RhythmPatternFactory::createAllPatterns();
}

AudioPluginAudioProcessor::~AudioPluginAudioProcessor()
{
}

//==============================================================================
const juce::String AudioPluginAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool AudioPluginAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool AudioPluginAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool AudioPluginAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double AudioPluginAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int AudioPluginAudioProcessor::getNumPrograms()
{
    return 1;
}

int AudioPluginAudioProcessor::getCurrentProgram()
{
    return 0;
}

void AudioPluginAudioProcessor::setCurrentProgram (int index)
{
    juce::ignoreUnused (index);
}

const juce::String AudioPluginAudioProcessor::getProgramName (int index)
{
    juce::ignoreUnused (index);
    return {};
}

void AudioPluginAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
    juce::ignoreUnused (index, newName);
}

//==============================================================================
void AudioPluginAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    currentSampleRate = sampleRate;
    accumulatedBeats = 0.0;
    lastPatternBeat = 0.0;
    pendingNoteOffs.clear();
    activeOutputNotes.clear();
    heldNotes.clear();
    currentChord = DetectedChord();
    setDetectedChordName ("---");
    juce::ignoreUnused (samplesPerBlock);
}

void AudioPluginAudioProcessor::releaseResources()
{
    pendingNoteOffs.clear();
    activeOutputNotes.clear();
    heldNotes.clear();
    currentChord = DetectedChord();
    setDetectedChordName ("---");
}

void AudioPluginAudioProcessor::updateDetectedChord()
{
    currentChord = ChordDetector::detect (heldNotes);
    setDetectedChordName (currentChord.chordName);
}

bool AudioPluginAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}

void AudioPluginAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer,
                                              juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

    const int numSamples = buffer.getNumSamples();
    
    // Get tempo and transport info from host
    double bpm = internalTempo.load();
    bool useHostTiming = false;
    double ppqPosition = 0.0;
    
    if (auto* playHead = getPlayHead())
    {
        if (auto posInfo = playHead->getPosition())
        {
            if (auto bpmOpt = posInfo->getBpm())
            {
                bpm = *bpmOpt;
            }
            
            // Only use host PPQ if transport is playing
            if (posInfo->getIsPlaying())
            {
                if (auto ppqOpt = posInfo->getPpqPosition())
                {
                    ppqPosition = *ppqOpt;
                    useHostTiming = true;
                }
            }
        }
    }
    
    // Process input MIDI - track held notes for chord detection
    juce::MidiBuffer inputMidi;
    inputMidi.swapWith (midiMessages);
    
    bool chordChanged = false;
    
    for (const auto metadata : inputMidi)
    {
        auto message = metadata.getMessage();
        
        if (message.isNoteOn())
        {
            heldNotes.insert (message.getNoteNumber());
            chordChanged = true;
        }
        else if (message.isNoteOff())
        {
            heldNotes.erase (message.getNoteNumber());
            
            if (heldNotes.empty())
            {
                // All notes released - stop pattern and turn off active notes
                currentChord = DetectedChord();
                setDetectedChordName ("---");
                stopAllActiveNotes (midiMessages, metadata.samplePosition);
            }
            
            chordChanged = true;
        }
    }
    
    // Update chord detection when notes change
    if (chordChanged && !heldNotes.empty())
    {
        updateDetectedChord();
    }
    
    // Process rhythm pattern if enabled and we have a valid chord
    if (patternEnabled.load() && currentChord.isValid)
    {
        processRhythmPattern (midiMessages, numSamples, bpm, useHostTiming, ppqPosition);
    }
    
    // Process pending note-offs
    for (auto it = pendingNoteOffs.begin(); it != pendingNoteOffs.end(); )
    {
        if (it->samplePosition < numSamples)
        {
            auto noteOff = juce::MidiMessage::noteOff (it->channel, it->noteNumber);
            midiMessages.addEvent (noteOff, it->samplePosition);
            activeOutputNotes.erase (it->noteNumber);
            it = pendingNoteOffs.erase (it);
        }
        else
        {
            it->samplePosition -= numSamples;
            ++it;
        }
    }
    
    // Update keyboard state for UI visualization
    keyboardState.processNextMidiBuffer (midiMessages, 0, numSamples, false);
}

void AudioPluginAudioProcessor::processRhythmPattern (juce::MidiBuffer& midiMessages, 
                                                       int numSamples,
                                                       double bpm, 
                                                       bool useHostTiming,
                                                       double ppqPosition)
{
    const int patternIdx = juce::jlimit (0, (int) patterns.size() - 1, currentPatternIndex.load());
    const auto& pattern = patterns[patternIdx];
    const double patternLength = pattern.lengthInBeats;
    
    // Calculate beats per sample
    const double beatsPerSecond = bpm / 60.0;
    const double beatsPerSample = beatsPerSecond / currentSampleRate;
    const double beatsInBlock = beatsPerSample * numSamples;
    
    // Determine start beat position
    double startBeat;
    if (useHostTiming)
    {
        // Use host PPQ position when transport is playing
        startBeat = std::fmod (ppqPosition, patternLength);
    }
    else
    {
        // Use internal accumulator for standalone/stopped transport
        startBeat = std::fmod (accumulatedBeats, patternLength);
    }
    
    double endBeat = startBeat + beatsInBlock;
    
    // Add notes for this block
    addPatternNotes (midiMessages, startBeat, endBeat, 0, numSamples, bpm);
    
    // Always update accumulated beats (used for standalone/internal timing)
    accumulatedBeats += beatsInBlock;
    // Keep it from growing too large
    if (accumulatedBeats > patternLength * 1000.0)
        accumulatedBeats = std::fmod (accumulatedBeats, patternLength);
}

void AudioPluginAudioProcessor::addPatternNotes (juce::MidiBuffer& midiMessages,
                                                  double startBeat,
                                                  double endBeat,
                                                  int blockStartSample,
                                                  int numSamples,
                                                  double bpm)
{
    const int patternIdx = juce::jlimit (0, (int) patterns.size() - 1, currentPatternIndex.load());
    const auto& pattern = patterns[patternIdx];
    const double patternLength = pattern.lengthInBeats;
    
    const double beatsPerSecond = bpm / 60.0;
    const double samplesPerBeat = currentSampleRate / beatsPerSecond;
    
    for (const auto& note : pattern.notes)
    {
        double noteBeat = note.beatPosition;
        
        // Check if note falls within this block (handle pattern wrap)
        bool shouldTrigger = false;
        double relativeBeat = 0.0;
        
        if (endBeat > patternLength)
        {
            // Pattern wraps in this block
            if (noteBeat >= startBeat || noteBeat < (endBeat - patternLength))
            {
                shouldTrigger = true;
                if (noteBeat >= startBeat)
                    relativeBeat = noteBeat - startBeat;
                else
                    relativeBeat = (patternLength - startBeat) + noteBeat;
            }
        }
        else
        {
            // Normal case - no wrap
            if (noteBeat >= startBeat && noteBeat < endBeat)
            {
                shouldTrigger = true;
                relativeBeat = noteBeat - startBeat;
            }
        }
        
        if (shouldTrigger && currentChord.isValid)
        {
            int midiNote = getChordNote (note.chordIndex);
            
            if (midiNote >= 0 && midiNote <= 127)
            {
                int samplePos = blockStartSample + static_cast<int> (relativeBeat * samplesPerBeat);
                samplePos = juce::jlimit (0, numSamples - 1, samplePos);
                
                // Note on
                auto noteOn = juce::MidiMessage::noteOn (1, midiNote, note.velocity);
                midiMessages.addEvent (noteOn, samplePos);
                activeOutputNotes.insert (midiNote);
                
                // Schedule note off
                int noteOffSample = samplePos + static_cast<int> (note.duration * samplesPerBeat);
                pendingNoteOffs.push_back ({ midiNote, 1, noteOffSample });
            }
        }
    }
}

int AudioPluginAudioProcessor::getChordNote (int chordIndex) const
{
    if (!currentChord.isValid)
        return -1;
        
    int rootNote = currentChord.rootNote;
    const auto& intervals = currentChord.intervals;
    
    if (chordIndex == -1)
    {
        // Bass note - one octave down from root
        return rootNote - 12;
    }
    
    if (chordIndex >= 0 && chordIndex < (int) intervals.size())
    {
        return rootNote + intervals[chordIndex];
    }
    
    // Default to root if index out of range
    return rootNote;
}

void AudioPluginAudioProcessor::stopAllActiveNotes (juce::MidiBuffer& midiMessages, int samplePosition)
{
    for (int note : activeOutputNotes)
    {
        auto noteOff = juce::MidiMessage::noteOff (1, note);
        midiMessages.addEvent (noteOff, samplePosition);
    }
    activeOutputNotes.clear();
    pendingNoteOffs.clear();
}

//==============================================================================
bool AudioPluginAudioProcessor::hasEditor() const
{
    return true;
}

juce::AudioProcessorEditor* AudioPluginAudioProcessor::createEditor()
{
    return new AudioPluginAudioProcessorEditor (*this);
}

//==============================================================================
void AudioPluginAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    juce::ValueTree state ("ChordPlayerState");
    state.setProperty ("patternIndex", currentPatternIndex.load(), nullptr);
    state.setProperty ("tempo", internalTempo.load(), nullptr);
    state.setProperty ("enabled", patternEnabled.load(), nullptr);
    
    std::unique_ptr<juce::XmlElement> xml (state.createXml());
    copyXmlToBinary (*xml, destData);
}

void AudioPluginAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    std::unique_ptr<juce::XmlElement> xml (getXmlFromBinary (data, sizeInBytes));
    
    if (xml != nullptr)
    {
        juce::ValueTree state = juce::ValueTree::fromXml (*xml);
        
        if (state.isValid())
        {
            currentPatternIndex.store (state.getProperty ("patternIndex", 0));
            internalTempo.store (state.getProperty ("tempo", 120.0f));
            patternEnabled.store (state.getProperty ("enabled", true));
        }
    }
}

//==============================================================================
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new AudioPluginAudioProcessor();
}

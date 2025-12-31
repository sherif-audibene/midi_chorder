#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_audio_utils/juce_audio_utils.h>
#include "RhythmPattern.h"
#include "ChordDetector.h"
#include <set>

//==============================================================================
class AudioPluginAudioProcessor final : public juce::AudioProcessor
{
public:
    //==============================================================================
    AudioPluginAudioProcessor();
    ~AudioPluginAudioProcessor() override;
    
    // Keyboard state to visualize output notes in the UI
    juce::MidiKeyboardState keyboardState;

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;
    using AudioProcessor::processBlock;

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

    //==========================================================================
    // Rhythm pattern control
    juce::StringArray getPatternNames() const { return RhythmPatternFactory::getPatternNames(); }
    
    // Current selected pattern index (thread-safe)
    std::atomic<int> currentPatternIndex { 0 };
    
    // Tempo control (BPM) - used when host tempo not available
    std::atomic<float> internalTempo { 120.0f };
    
    // Enable/disable pattern playback
    std::atomic<bool> patternEnabled { true };
    
    // Detected chord info (for UI display)
    juce::String getDetectedChordName() const 
    { 
        juce::SpinLock::ScopedLockType lock (chordNameLock);
        return detectedChordName; 
    }
    
private:
    //==============================================================================
    // Thread-safe chord name for UI display
    mutable juce::SpinLock chordNameLock;
    juce::String detectedChordName { "---" };
    
    void setDetectedChordName (const juce::String& name)
    {
        juce::SpinLock::ScopedLockType lock (chordNameLock);
        detectedChordName = name;
    }
    
    // Rhythm patterns
    std::vector<RhythmPattern> patterns;
    
    // Currently detected chord (used for playback)
    DetectedChord currentChord;
    
    // Currently held input notes (for chord detection)
    std::set<int> heldNotes;
    
    // Timing state
    double currentSampleRate { 44100.0 };
    double lastPatternBeat { 0.0 };
    double accumulatedBeats { 0.0 };
    
    // Scheduled note-offs (sample position -> notes to turn off)
    struct ScheduledNoteOff
    {
        int noteNumber;
        int channel;
        int samplePosition;
    };
    std::vector<ScheduledNoteOff> pendingNoteOffs;
    
    // Currently playing notes (to handle note-offs properly)
    std::set<int> activeOutputNotes;
    
    // Helper methods
    void processRhythmPattern (juce::MidiBuffer& midiMessages, int numSamples, 
                               double bpm, bool useHostTiming, double ppqPosition);
    void addPatternNotes (juce::MidiBuffer& midiMessages, double startBeat, 
                          double endBeat, int blockStartSample, int numSamples, double bpm);
    int getChordNote (int chordIndex) const;
    void stopAllActiveNotes (juce::MidiBuffer& midiMessages, int samplePosition);
    void updateDetectedChord();
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AudioPluginAudioProcessor)
};

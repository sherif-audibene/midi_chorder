#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_audio_utils/juce_audio_utils.h>
#include "RhythmPattern.h"
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
    
    // Chord type selection (for chord building from single notes)
    enum class ChordType { Major, Minor, Dominant7, Minor7, Major7, Sus4, Sus2 };
    std::atomic<int> chordTypeIndex { 0 }; // Default to Major
    
    static inline const juce::StringArray chordTypeNames {
        "Major", "Minor", "Dom7", "Min7", "Maj7", "Sus4", "Sus2"
    };

private:
    //==============================================================================
    // Rhythm patterns
    std::vector<RhythmPattern> patterns;
    
    // Chord intervals for different chord types
    const std::vector<std::vector<int>> chordIntervals {
        { 0, 4, 7 },           // Major
        { 0, 3, 7 },           // Minor
        { 0, 4, 7, 10 },       // Dominant 7th
        { 0, 3, 7, 10 },       // Minor 7th
        { 0, 4, 7, 11 },       // Major 7th
        { 0, 5, 7 },           // Sus4
        { 0, 2, 7 },           // Sus2
    };
    
    // Currently held input notes (for chord detection)
    std::set<int> heldNotes;
    int currentRootNote { -1 };
    
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
    int getChordNote (int rootNote, int chordIndex) const;
    void stopAllActiveNotes (juce::MidiBuffer& midiMessages, int samplePosition);
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AudioPluginAudioProcessor)
};

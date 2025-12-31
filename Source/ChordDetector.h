#pragma once

#include <juce_core/juce_core.h>
#include <set>
#include <vector>
#include <algorithm>

//==============================================================================
// Result of chord detection
struct DetectedChord
{
    int rootNote { -1 };                    // MIDI note number of root (-1 if no chord)
    juce::String chordName { "---" };       // Display name (e.g., "C Major", "Am7")
    std::vector<int> intervals;             // Intervals from root (always includes 0 for root)
    bool isValid { false };                 // True if a valid chord was detected
};

//==============================================================================
// Chord detection from MIDI input notes
class ChordDetector
{
public:
    //==========================================================================
    // Analyze held notes and detect chord
    static DetectedChord detect (const std::set<int>& heldNotes)
    {
        DetectedChord result;
        
        if (heldNotes.size() < 2)
        {
            // Need at least 2 notes for a chord
            if (heldNotes.size() == 1)
            {
                result.rootNote = *heldNotes.begin();
                result.chordName = getNoteNameWithOctave (result.rootNote);
                result.intervals = { 0 };
                result.isValid = true;
            }
            return result;
        }
        
        // Get root (lowest note) and calculate intervals
        int root = *heldNotes.begin();
        std::vector<int> rawIntervals;
        
        for (int note : heldNotes)
        {
            int interval = (note - root) % 12;  // Normalize to single octave
            if (std::find (rawIntervals.begin(), rawIntervals.end(), interval) == rawIntervals.end())
            {
                rawIntervals.push_back (interval);
            }
        }
        
        std::sort (rawIntervals.begin(), rawIntervals.end());
        
        // Match against known chord patterns
        juce::String chordType = identifyChordType (rawIntervals);
        
        result.rootNote = root;
        result.chordName = getNoteName (root) + " " + chordType;
        result.intervals = rawIntervals;
        result.isValid = true;
        
        return result;
    }
    
    //==========================================================================
    // Get the full intervals for playback (including octave variations)
    static std::vector<int> getPlaybackIntervals (const DetectedChord& chord)
    {
        if (!chord.isValid)
            return { 0, 4, 7 };  // Default to major if invalid
            
        return chord.intervals;
    }

private:
    //==========================================================================
    static juce::String identifyChordType (const std::vector<int>& intervals)
    {
        // Check for various chord types based on interval patterns
        // intervals are normalized to 0-11 range
        
        bool has2  = hasInterval (intervals, 2);
        bool has3m = hasInterval (intervals, 3);   // Minor 3rd
        bool has3M = hasInterval (intervals, 4);   // Major 3rd
        bool has4  = hasInterval (intervals, 5);   // Perfect 4th
        bool has5  = hasInterval (intervals, 7);   // Perfect 5th
        bool has6  = hasInterval (intervals, 9);   // Major 6th
        bool has7m = hasInterval (intervals, 10);  // Minor 7th
        bool has7M = hasInterval (intervals, 11);  // Major 7th
        bool hasb5 = hasInterval (intervals, 6);   // Diminished 5th
        
        // Determine chord type based on intervals
        
        // Seventh chords
        if (has3M && has5 && has7M)
            return "Maj7";
        if (has3M && has5 && has7m)
            return "7";  // Dominant 7th
        if (has3m && has5 && has7m)
            return "m7";
        if (has3m && hasb5 && has7m)
            return "m7b5";
        if (has3m && has5 && has7M)
            return "mMaj7";
            
        // Sixth chords
        if (has3M && has5 && has6)
            return "6";
        if (has3m && has5 && has6)
            return "m6";
        
        // Suspended chords
        if (has4 && has5 && !has3M && !has3m)
            return "sus4";
        if (has2 && has5 && !has3M && !has3m)
            return "sus2";
        
        // Triads
        if (has3m && hasb5)
            return "dim";
        if (has3M && hasInterval (intervals, 8))  // Augmented 5th
            return "aug";
        if (has3m && has5)
            return "m";
        if (has3M && has5)
            return "Maj";
            
        // Power chord
        if (has5 && !has3M && !has3m)
            return "5";
            
        // Default - just show as chord
        return "chord";
    }
    
    //==========================================================================
    static bool hasInterval (const std::vector<int>& intervals, int target)
    {
        return std::find (intervals.begin(), intervals.end(), target) != intervals.end();
    }
    
    //==========================================================================
    static juce::String getNoteName (int midiNote)
    {
        static const char* noteNames[] = { "C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B" };
        return noteNames[midiNote % 12];
    }
    
    //==========================================================================
    static juce::String getNoteNameWithOctave (int midiNote)
    {
        return getNoteName (midiNote) + juce::String (midiNote / 12 - 1);
    }
};


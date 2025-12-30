#pragma once

#include <juce_core/juce_core.h>
#include <vector>

//==============================================================================
// Represents a single note event within a rhythm pattern
struct PatternNote
{
    double beatPosition;    // Position in beats (0.0 to patternLength)
    int chordIndex;         // Which chord note to play: 0=root, 1=3rd, 2=5th, 3=7th, -1=bass (octave down)
    float velocity;         // Note velocity (0.0 to 1.0)
    double duration;        // Duration in beats
};

//==============================================================================
// A complete rhythm pattern definition
struct RhythmPattern
{
    juce::String name;
    double lengthInBeats;           // Pattern length (typically 4 or 8 beats)
    std::vector<PatternNote> notes; // All notes in the pattern
};

//==============================================================================
// Factory class for creating rhythm patterns
class RhythmPatternFactory
{
public:
    static std::vector<RhythmPattern> createAllPatterns()
    {
        return {
            createSamba(),
            createBossaNova(),
            createRumba(),
            createChacha(),
            createReggae(),
            createWaltz(),
            createMarch(),
            createBallad(),
            createDisco(),
            createRock()
        };
    }

    static juce::StringArray getPatternNames()
    {
        return {
            "Samba",
            "Bossa Nova", 
            "Rumba",
            "Cha-Cha",
            "Reggae",
            "Waltz",
            "March",
            "Ballad",
            "Disco",
            "Rock"
        };
    }

private:
    //==========================================================================
    // SAMBA - Syncopated Brazilian rhythm, 4/4 time
    static RhythmPattern createSamba()
    {
        RhythmPattern pattern;
        pattern.name = "Samba";
        pattern.lengthInBeats = 4.0;
        pattern.notes = {
            // Bass hits
            { 0.0,   -1, 0.9f, 0.25 },
            { 1.5,   -1, 0.7f, 0.25 },
            { 3.0,   -1, 0.8f, 0.25 },
            // Chord stabs (syncopated)
            { 0.5,    0, 0.7f, 0.2 },
            { 0.5,    1, 0.7f, 0.2 },
            { 0.5,    2, 0.7f, 0.2 },
            { 1.0,    0, 0.6f, 0.2 },
            { 1.0,    1, 0.6f, 0.2 },
            { 1.0,    2, 0.6f, 0.2 },
            { 2.5,    0, 0.7f, 0.2 },
            { 2.5,    1, 0.7f, 0.2 },
            { 2.5,    2, 0.7f, 0.2 },
            { 3.5,    0, 0.6f, 0.2 },
            { 3.5,    1, 0.6f, 0.2 },
            { 3.5,    2, 0.6f, 0.2 },
        };
        return pattern;
    }

    //==========================================================================
    // BOSSA NOVA - Smooth Brazilian jazz rhythm
    static RhythmPattern createBossaNova()
    {
        RhythmPattern pattern;
        pattern.name = "Bossa Nova";
        pattern.lengthInBeats = 4.0;
        pattern.notes = {
            // Classic bossa bass pattern
            { 0.0,   -1, 0.8f, 0.4 },
            { 2.0,   -1, 0.7f, 0.4 },
            // Gentle chord comping
            { 0.0,    0, 0.5f, 0.3 },
            { 0.0,    1, 0.5f, 0.3 },
            { 0.0,    2, 0.5f, 0.3 },
            { 1.5,    0, 0.4f, 0.2 },
            { 1.5,    1, 0.4f, 0.2 },
            { 1.5,    2, 0.4f, 0.2 },
            { 3.0,    0, 0.5f, 0.3 },
            { 3.0,    1, 0.5f, 0.3 },
            { 3.0,    2, 0.5f, 0.3 },
        };
        return pattern;
    }

    //==========================================================================
    // RUMBA - Cuban rhythm with clave feel
    static RhythmPattern createRumba()
    {
        RhythmPattern pattern;
        pattern.name = "Rumba";
        pattern.lengthInBeats = 4.0;
        pattern.notes = {
            // Strong bass pattern
            { 0.0,   -1, 0.9f, 0.3 },
            { 2.5,   -1, 0.7f, 0.3 },
            // Clave-inspired chord pattern
            { 0.0,    0, 0.7f, 0.2 },
            { 0.0,    1, 0.7f, 0.2 },
            { 0.0,    2, 0.7f, 0.2 },
            { 1.0,    0, 0.5f, 0.2 },
            { 1.0,    1, 0.5f, 0.2 },
            { 2.0,    0, 0.6f, 0.2 },
            { 2.0,    1, 0.6f, 0.2 },
            { 2.0,    2, 0.6f, 0.2 },
            { 3.0,    0, 0.7f, 0.2 },
            { 3.0,    1, 0.7f, 0.2 },
            { 3.5,    2, 0.5f, 0.2 },
        };
        return pattern;
    }

    //==========================================================================
    // CHA-CHA - Latin dance rhythm
    static RhythmPattern createChacha()
    {
        RhythmPattern pattern;
        pattern.name = "Cha-Cha";
        pattern.lengthInBeats = 4.0;
        pattern.notes = {
            // Bass on 1 and 3
            { 0.0,   -1, 0.9f, 0.25 },
            { 2.0,   -1, 0.8f, 0.25 },
            // Cha-cha-cha hits on 4-and-1
            { 0.0,    0, 0.7f, 0.2 },
            { 0.0,    1, 0.7f, 0.2 },
            { 0.0,    2, 0.7f, 0.2 },
            { 2.0,    0, 0.6f, 0.2 },
            { 2.0,    1, 0.6f, 0.2 },
            { 2.0,    2, 0.6f, 0.2 },
            // The "cha-cha-cha" syncopation
            { 3.0,    0, 0.7f, 0.15 },
            { 3.0,    1, 0.7f, 0.15 },
            { 3.5,    0, 0.6f, 0.15 },
            { 3.5,    1, 0.6f, 0.15 },
            { 3.75,   0, 0.5f, 0.15 },
            { 3.75,   1, 0.5f, 0.15 },
        };
        return pattern;
    }

    //==========================================================================
    // REGGAE - Offbeat emphasis
    static RhythmPattern createReggae()
    {
        RhythmPattern pattern;
        pattern.name = "Reggae";
        pattern.lengthInBeats = 4.0;
        pattern.notes = {
            // One drop bass
            { 2.0,   -1, 0.9f, 0.4 },
            // Offbeat skank chords
            { 0.5,    0, 0.6f, 0.2 },
            { 0.5,    1, 0.6f, 0.2 },
            { 0.5,    2, 0.6f, 0.2 },
            { 1.5,    0, 0.6f, 0.2 },
            { 1.5,    1, 0.6f, 0.2 },
            { 1.5,    2, 0.6f, 0.2 },
            { 2.5,    0, 0.6f, 0.2 },
            { 2.5,    1, 0.6f, 0.2 },
            { 2.5,    2, 0.6f, 0.2 },
            { 3.5,    0, 0.6f, 0.2 },
            { 3.5,    1, 0.6f, 0.2 },
            { 3.5,    2, 0.6f, 0.2 },
        };
        return pattern;
    }

    //==========================================================================
    // WALTZ - 3/4 time signature
    static RhythmPattern createWaltz()
    {
        RhythmPattern pattern;
        pattern.name = "Waltz";
        pattern.lengthInBeats = 3.0;
        pattern.notes = {
            // Strong bass on 1
            { 0.0,   -1, 0.9f, 0.5 },
            // Chord on 2 and 3
            { 1.0,    0, 0.5f, 0.3 },
            { 1.0,    1, 0.5f, 0.3 },
            { 1.0,    2, 0.5f, 0.3 },
            { 2.0,    0, 0.5f, 0.3 },
            { 2.0,    1, 0.5f, 0.3 },
            { 2.0,    2, 0.5f, 0.3 },
        };
        return pattern;
    }

    //==========================================================================
    // MARCH - Strong 4/4 military style
    static RhythmPattern createMarch()
    {
        RhythmPattern pattern;
        pattern.name = "March";
        pattern.lengthInBeats = 4.0;
        pattern.notes = {
            // Strong bass on 1 and 3
            { 0.0,   -1, 1.0f, 0.3 },
            { 2.0,   -1, 0.8f, 0.3 },
            // Full chords on every beat
            { 0.0,    0, 0.8f, 0.25 },
            { 0.0,    1, 0.8f, 0.25 },
            { 0.0,    2, 0.8f, 0.25 },
            { 1.0,    0, 0.6f, 0.25 },
            { 1.0,    1, 0.6f, 0.25 },
            { 1.0,    2, 0.6f, 0.25 },
            { 2.0,    0, 0.7f, 0.25 },
            { 2.0,    1, 0.7f, 0.25 },
            { 2.0,    2, 0.7f, 0.25 },
            { 3.0,    0, 0.6f, 0.25 },
            { 3.0,    1, 0.6f, 0.25 },
            { 3.0,    2, 0.6f, 0.25 },
        };
        return pattern;
    }

    //==========================================================================
    // BALLAD - Slow, sustained chords
    static RhythmPattern createBallad()
    {
        RhythmPattern pattern;
        pattern.name = "Ballad";
        pattern.lengthInBeats = 4.0;
        pattern.notes = {
            // Gentle bass
            { 0.0,   -1, 0.6f, 0.8 },
            { 2.0,   -1, 0.5f, 0.8 },
            // Sustained chord
            { 0.0,    0, 0.5f, 1.5 },
            { 0.0,    1, 0.5f, 1.5 },
            { 0.0,    2, 0.5f, 1.5 },
            { 2.0,    0, 0.4f, 1.5 },
            { 2.0,    1, 0.4f, 1.5 },
            { 2.0,    2, 0.4f, 1.5 },
        };
        return pattern;
    }

    //==========================================================================
    // DISCO - Four-on-the-floor with offbeat chords
    static RhythmPattern createDisco()
    {
        RhythmPattern pattern;
        pattern.name = "Disco";
        pattern.lengthInBeats = 4.0;
        pattern.notes = {
            // Four-on-the-floor bass
            { 0.0,   -1, 0.9f, 0.2 },
            { 1.0,   -1, 0.9f, 0.2 },
            { 2.0,   -1, 0.9f, 0.2 },
            { 3.0,   -1, 0.9f, 0.2 },
            // Offbeat chord stabs
            { 0.5,    0, 0.7f, 0.2 },
            { 0.5,    1, 0.7f, 0.2 },
            { 0.5,    2, 0.7f, 0.2 },
            { 1.5,    0, 0.7f, 0.2 },
            { 1.5,    1, 0.7f, 0.2 },
            { 1.5,    2, 0.7f, 0.2 },
            { 2.5,    0, 0.7f, 0.2 },
            { 2.5,    1, 0.7f, 0.2 },
            { 2.5,    2, 0.7f, 0.2 },
            { 3.5,    0, 0.7f, 0.2 },
            { 3.5,    1, 0.7f, 0.2 },
            { 3.5,    2, 0.7f, 0.2 },
        };
        return pattern;
    }

    //==========================================================================
    // ROCK - Driving eighth note rhythm
    static RhythmPattern createRock()
    {
        RhythmPattern pattern;
        pattern.name = "Rock";
        pattern.lengthInBeats = 4.0;
        pattern.notes = {
            // Driving bass
            { 0.0,   -1, 0.9f, 0.25 },
            { 2.0,   -1, 0.9f, 0.25 },
            // Power chord hits
            { 0.0,    0, 0.8f, 0.4 },
            { 0.0,    2, 0.8f, 0.4 },
            { 1.0,    0, 0.6f, 0.2 },
            { 1.0,    2, 0.6f, 0.2 },
            { 2.0,    0, 0.8f, 0.4 },
            { 2.0,    2, 0.8f, 0.4 },
            { 3.0,    0, 0.6f, 0.2 },
            { 3.0,    2, 0.6f, 0.2 },
            { 3.5,    0, 0.7f, 0.2 },
            { 3.5,    2, 0.7f, 0.2 },
        };
        return pattern;
    }
};


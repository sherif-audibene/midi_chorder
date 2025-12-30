//
// Created by Sherif Hamad on 23.12.25.
//

#include "square.h"

void Square::paint (juce::Graphics& g){
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));
    g.drawRect(100,100,100,100);
    g.setColour(juce::Colours::aqua);
    g.setFont(16.f);
    g.drawText("Change the text",getBounds(),juce::Justification::centred);
}

void  Square::resized(){

}
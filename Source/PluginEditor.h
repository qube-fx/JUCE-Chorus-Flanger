/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

//==============================================================================
/**
*/
class QubeChorusflangerAudioProcessorEditor  : public AudioProcessorEditor
{
public:
    QubeChorusflangerAudioProcessorEditor (QubeChorusflangerAudioProcessor&);
    ~QubeChorusflangerAudioProcessorEditor();

    //==============================================================================
    void paint (Graphics&) override;
    void resized() override;

private:
    
    Slider mDrywetSlider;
    Slider mDepthSlider;
    Slider mRateSlider;
    Slider mPhaseOffsetSlider;
    Slider mFeedbackSlider;
    
    ComboBox mType;
    
    Label dryWetLabel;
    Label depthLabel;
    Label rateLabel;
    Label phaseOffsetLabel;
    Label feedbackLabel;


    QubeChorusflangerAudioProcessor& processor;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (QubeChorusflangerAudioProcessorEditor)
};

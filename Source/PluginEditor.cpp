/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
QubeChorusflangerAudioProcessorEditor::QubeChorusflangerAudioProcessorEditor (QubeChorusflangerAudioProcessor& p)
    : AudioProcessorEditor (&p), processor (p)
{

     setSize (600, 120);
     
     auto& params = processor.getParameters();
     
     //  0 - DRY / WET
     AudioParameterFloat* dryWetParameter = (AudioParameterFloat*)params.getUnchecked(0);
     // Slider set up
     mDrywetSlider.setBounds(0, 20, 100, 100);
     mDrywetSlider.setSliderStyle(Slider::SliderStyle::RotaryVerticalDrag);
     mDrywetSlider.setTextBoxStyle(Slider::TextEntryBoxPosition::NoTextBox, true, 0, 0);
     mDrywetSlider.setRange(dryWetParameter->range.start, dryWetParameter->range.end);
     mDrywetSlider.setValue(*dryWetParameter);
     addAndMakeVisible(mDrywetSlider);
     // Label
     addAndMakeVisible (dryWetLabel);
     dryWetLabel.setText ("Dry/Wet", dontSendNotification);
     dryWetLabel.attachToComponent (&mDrywetSlider, false);
     // Update the parameter value
     mDrywetSlider.onValueChange = [this, dryWetParameter] { *dryWetParameter = mDrywetSlider.getValue();};
     mDrywetSlider.onDragStart = [dryWetParameter] { dryWetParameter->beginChangeGesture(); };
     mDrywetSlider.onDragEnd = [dryWetParameter] { dryWetParameter->endChangeGesture(); };
     
     
     // 1 - DEPTH
     AudioParameterFloat* depthParameter = (AudioParameterFloat*)params.getUnchecked(1);
     
     mDepthSlider.setBounds(100, 20, 100, 100);
     mDepthSlider.setSliderStyle(Slider::SliderStyle::RotaryVerticalDrag);
     mDepthSlider.setTextBoxStyle(Slider::TextEntryBoxPosition::NoTextBox, true, 0, 0);
     mDepthSlider.setRange(depthParameter->range.start, depthParameter->range.end);
     mDepthSlider.setValue(*depthParameter);
     addAndMakeVisible(mDepthSlider);
     
     addAndMakeVisible (depthLabel);
     depthLabel.setText ("Depth", dontSendNotification);
     depthLabel.attachToComponent (&mDepthSlider, false);
     
     mDepthSlider.onValueChange = [this, depthParameter] { *depthParameter = mDepthSlider.getValue();};
     mDepthSlider.onDragStart = [depthParameter] { depthParameter->beginChangeGesture(); };
     mDepthSlider.onDragEnd = [depthParameter] { depthParameter->endChangeGesture(); };
     
     
     // 2 - RATE
     AudioParameterFloat* rateParameter = (AudioParameterFloat*)params.getUnchecked(2);
        
     mRateSlider.setBounds(200, 20, 100, 100);
     mRateSlider.setSliderStyle(Slider::SliderStyle::RotaryVerticalDrag);
     mRateSlider.setTextBoxStyle(Slider::TextEntryBoxPosition::NoTextBox, true, 0, 0);
     mRateSlider.setRange(rateParameter->range.start, rateParameter->range.end);
     mRateSlider.setValue(*rateParameter);
     addAndMakeVisible(mRateSlider);
        
     addAndMakeVisible (rateLabel);
     rateLabel.setText ("Rate", dontSendNotification);
     rateLabel.attachToComponent (&mRateSlider, false);
        
     mRateSlider.onValueChange = [this, rateParameter] { *rateParameter = mRateSlider.getValue();};
     mRateSlider.onDragStart = [rateParameter] { rateParameter->beginChangeGesture(); };
     mRateSlider.onDragEnd = [rateParameter] { rateParameter->endChangeGesture(); };
     
    // 3 - PHASE OFFSET
    AudioParameterFloat* phaseOffsetParameter = (AudioParameterFloat*)params.getUnchecked(3);
       
    mPhaseOffsetSlider.setBounds(300, 20, 100, 100);
    mPhaseOffsetSlider.setSliderStyle(Slider::SliderStyle::RotaryVerticalDrag);
    mPhaseOffsetSlider.setTextBoxStyle(Slider::TextEntryBoxPosition::NoTextBox, true, 0, 0);
    mPhaseOffsetSlider.setRange(phaseOffsetParameter->range.start, phaseOffsetParameter->range.end);
    mPhaseOffsetSlider.setValue(*phaseOffsetParameter);
    addAndMakeVisible(mPhaseOffsetSlider);
       
    addAndMakeVisible (phaseOffsetLabel);
    phaseOffsetLabel.setText ("Phase offset", dontSendNotification);
    phaseOffsetLabel.attachToComponent (&mPhaseOffsetSlider, false);
       
    mPhaseOffsetSlider.onValueChange = [this, phaseOffsetParameter] { *phaseOffsetParameter = mPhaseOffsetSlider.getValue();};
    mPhaseOffsetSlider.onDragStart = [phaseOffsetParameter] { phaseOffsetParameter->beginChangeGesture(); };
    mPhaseOffsetSlider.onDragEnd = [phaseOffsetParameter] { phaseOffsetParameter->endChangeGesture(); };
     
     // 4 - FEEDBACK
     AudioParameterFloat* feedbackParameter = (AudioParameterFloat*)params.getUnchecked(4);
     
     mFeedbackSlider.setBounds(400, 20, 100, 100);
     mFeedbackSlider.setSliderStyle(Slider::SliderStyle::RotaryVerticalDrag);
     mFeedbackSlider.setTextBoxStyle(Slider::TextEntryBoxPosition::NoTextBox, true, 0, 0);
     mFeedbackSlider.setRange(feedbackParameter->range.start, feedbackParameter->range.end);
     mFeedbackSlider.setValue(*feedbackParameter);
     addAndMakeVisible(mFeedbackSlider);
     
     addAndMakeVisible (feedbackLabel);
     feedbackLabel.setText ("Feedback", dontSendNotification);
     feedbackLabel.attachToComponent (&mFeedbackSlider, false);
     
     mFeedbackSlider.onValueChange = [this, feedbackParameter] { *feedbackParameter = mFeedbackSlider.getValue(); };
     mFeedbackSlider.onDragStart = [feedbackParameter] { feedbackParameter->beginChangeGesture(); };
     mFeedbackSlider.onDragEnd = [feedbackParameter] { feedbackParameter->endChangeGesture(); };

     // 5 - TYPE
     AudioParameterInt* typeParameter = (AudioParameterInt*)params.getUnchecked(5);

     mType.setBounds(500, 20, 100, 30);
     mType.addItem("Chorus", 1);
     mType.addItem("Flanger", 2);
     addAndMakeVisible(mType);
     
     mType.onChange = [this, typeParameter] {
         typeParameter->beginChangeGesture();
         *typeParameter = mType.getSelectedItemIndex();
         typeParameter->endChangeGesture();
     };

     mType.setSelectedItemIndex(*typeParameter);
}

QubeChorusflangerAudioProcessorEditor::~QubeChorusflangerAudioProcessorEditor()
{
}

//==============================================================================
void QubeChorusflangerAudioProcessorEditor::paint (Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (ResizableWindow::backgroundColourId));

}

void QubeChorusflangerAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
}

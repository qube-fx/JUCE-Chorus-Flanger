/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
QubeChorusflangerAudioProcessor::QubeChorusflangerAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", AudioChannelSet::stereo(), true)
                     #endif
                       )
#endif
{
    // set up parameters and ranges
    addParameter(mDryWetParameter = new AudioParameterFloat("drywet",
                                                            "Dry Wet",
                                                            0.0,
                                                            1.0,
                                                            0.3));
    
    addParameter(mDepthParameter = new AudioParameterFloat("depth",
                                                           "Depth",
                                                           0.0,
                                                           1.0,
                                                           0.3));
    
    addParameter(mRateParameter = new AudioParameterFloat("rate",
                                                          "Rate",
                                                          0.1f,
                                                          20.0f,
                                                          5.0f));
    
    addParameter(mPhaseOffsetParameter = new AudioParameterFloat("phaseoffset",
                                                                 "Phase Offset",
                                                                 0.0f,
                                                                 1.0f,
                                                                 0.0f));
    
    addParameter(mFeedbackParameter = new AudioParameterFloat("feedback",
                                                              "Feedback",
                                                              0,
                                                              0.98,
                                                              0.2));
    
    addParameter(mTypeParameter = new AudioParameterInt("type",
                                                        "Type",
                                                        0,
                                                        1,
                                                        0));
    
    // initialise variables
    mCircularBufferLeft = nullptr;
    mCircularBufferRight = nullptr;
    mCircularBufferWriteHead = 0;
    mCircularBufferLength = 0;
    mFeedbackLeft = 0;
    mFeedbackRight = 0;
    mLFOPhase = 0;
}

QubeChorusflangerAudioProcessor::~QubeChorusflangerAudioProcessor()
{
    if (mCircularBufferLeft != nullptr) {
        delete [] mCircularBufferLeft;
        mCircularBufferLeft = nullptr;
    }
    
    if (mCircularBufferRight != nullptr) {
        delete [] mCircularBufferRight;
        mCircularBufferRight = nullptr;
    }
}

//==============================================================================
const String QubeChorusflangerAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool QubeChorusflangerAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool QubeChorusflangerAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool QubeChorusflangerAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double QubeChorusflangerAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int QubeChorusflangerAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int QubeChorusflangerAudioProcessor::getCurrentProgram()
{
    return 0;
}

void QubeChorusflangerAudioProcessor::setCurrentProgram (int index)
{
}

const String QubeChorusflangerAudioProcessor::getProgramName (int index)
{
    return {};
}

void QubeChorusflangerAudioProcessor::changeProgramName (int index, const String& newName)
{
}

//==============================================================================
void QubeChorusflangerAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
     // initialise the LFO
     mLFOPhase = 0;
     // set the buffer length
     mCircularBufferLength = sampleRate * MAX_DELAY_TIME;
     
    // Buffer set up - left channel
    // if the buffer is not a nullptr, set it to be
     if (mCircularBufferLeft != nullptr ) {
       delete [] mCircularBufferLeft;
       mCircularBufferLeft = nullptr;
    }
    // if the buffer is a nullptr, create a new float for it
    if (mCircularBufferLeft == nullptr ) {
        mCircularBufferLeft = new float[(int)mCircularBufferLength];
    }
     // fill the buffer with zeros
     zeromem(mCircularBufferLeft, mCircularBufferLength * sizeof(float));
     
    // Buffer set up - right channel
    if (mCircularBufferRight != nullptr ) {
        delete [] mCircularBufferRight;
        mCircularBufferRight = nullptr;
     }
      
     if (mCircularBufferRight == nullptr ) {
         mCircularBufferRight = new float[(int)mCircularBufferLength];
     }
     zeromem(mCircularBufferRight, mCircularBufferLength * sizeof(float));
     
     // initialise the buffer write head to 0
     mCircularBufferWriteHead = 0;
}

void QubeChorusflangerAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool QubeChorusflangerAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    if (layouts.getMainOutputChannelSet() != AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

float QubeChorusflangerAudioProcessor::linInterp(float sample_x, float sample_x1, float inPhase)
{
    // calculate the interpolated sample at position 'inPhase' between two samples 'x' and 'x1'
    return (1 - inPhase) * sample_x + inPhase * sample_x1;
}

void QubeChorusflangerAudioProcessor::processBlock (AudioBuffer<float>& buffer, MidiBuffer& midiMessages)
{
    ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

    float* leftChannel = buffer.getWritePointer(0);
    float* rightChannel = buffer.getWritePointer(1);
    
    // iterate over the buffer
    for (int j = 0; j < buffer.getNumSamples(); j++)
    {
        // set up a sinusoidal LFO for the left channel
        float lfoOutLeft = sin(2*M_PI * mLFOPhase);
        // set the LFO frequency according to the Rate parameter
        mLFOPhase += *mRateParameter / getSampleRate();
        // in case it's out of range
        if (mLFOPhase > 1) {
            mLFOPhase -= 1;
        }
        // set up the LFO phase for right channel according to the phase offset parameter
        float lfoPhaseRight = mLFOPhase + *mPhaseOffsetParameter;
        
        if (lfoPhaseRight > 1) {
            lfoPhaseRight -= 1;
        }
        // set up a sinusoidal LFO for the right channel
        float lfoOutRight =sin(2*M_PI * lfoPhaseRight);
        // apply the Depth parameter to the LFO waveform
        lfoOutLeft *= *mDepthParameter;
        lfoOutRight *= *mDepthParameter;
        // declare the mapped LFos for both channels
        float lfoOutMappedLeft = 0;
        float lfoOutMappedRight = 0;
        
        // LFO mapping
        // CHORUS
        if (*mTypeParameter == 0) {
            // map LFO to oscillate between 5ms and 30ms
            lfoOutMappedLeft = jmap(lfoOutLeft, -1.f, 1.f, 0.005f, 0.03f);
            lfoOutMappedRight = jmap(lfoOutRight, -1.f, 1.f, 0.005f, 0.03f);
            
        // FLANGER
        } else {
            // map LFO to oscillate between 1ms and 5ms
            lfoOutMappedLeft = jmap(lfoOutLeft, -1.f, 1.f, 0.001f, 0.005f);
            lfoOutMappedRight = jmap(lfoOutRight, -1.f, 1.f, 0.001f, 0.005f);
        }
        
        // calculate the delay time in samples accoring to the LFO
        float delayTimeSamplesLeft = getSampleRate() * lfoOutMappedLeft;
        float delayTimeSamplesRight = getSampleRate() * lfoOutMappedRight;
        
        // populate the circular buffer with the current write sample plus feedback
        mCircularBufferLeft[mCircularBufferWriteHead] = leftChannel[j] + mFeedbackLeft;
        mCircularBufferRight[mCircularBufferWriteHead] = rightChannel[j] + mFeedbackRight;
        
        // set up the delay read head for the left channel
        float delayReadHeadLeft = mCircularBufferWriteHead - delayTimeSamplesLeft;
        // in case it's not within the buffer range
        if (delayReadHeadLeft < 0) {
            delayReadHeadLeft += mCircularBufferLength;
        }
        // the same for right channel
        float delayReadHeadRight = mCircularBufferWriteHead - delayTimeSamplesRight;
        if (delayReadHeadRight < 0) {
            delayReadHeadRight += mCircularBufferLength;
        }
        // set up variables for linear interpolation
        int readHeadLeft_x = (int)delayReadHeadLeft;
        int readHeadLeft_x1 = readHeadLeft_x + 1;
        if (readHeadLeft_x1 >= mCircularBufferLength) {
            readHeadLeft_x1 -= mCircularBufferLength;
        }
        float readHeadFloatLeft = delayReadHeadLeft - readHeadLeft_x;
        // perform linear interpolation for delay sample on left channel
        float delay_sample_left = linInterp(mCircularBufferLeft[readHeadLeft_x], mCircularBufferLeft[readHeadLeft_x1], readHeadFloatLeft);
        
        // same for right channel
        int readHeadRight_x = (int)delayReadHeadRight;
        int readHeadRight_x1 = readHeadRight_x + 1;
        if (readHeadRight_x1 >= mCircularBufferLength) {
            readHeadRight_x1 -= mCircularBufferLength;
        }
        float readHeadFloatRight = delayReadHeadRight - readHeadRight_x;
        float delay_sample_right = linInterp(mCircularBufferRight[readHeadRight_x], mCircularBufferRight[readHeadRight_x1], readHeadFloatRight);
        
        // calculate the feedback according to the Feedback parameter
        mFeedbackLeft = delay_sample_left * *mFeedbackParameter;
        mFeedbackRight = delay_sample_right * *mFeedbackParameter;
        
        // sum the dry and delayed signalsaccording to the DryWet parameter
        buffer.setSample(0, j, buffer.getSample(0,j) * (1 - *mDryWetParameter) + delay_sample_left * *mDryWetParameter);
        buffer.setSample(1, j, buffer.getSample(1,j) * (1 - *mDryWetParameter) + delay_sample_right * *mDryWetParameter);
        
        // increment the write head circular buffer iterator
        mCircularBufferWriteHead++;
        // in case it's out of range
        if (mCircularBufferWriteHead >= mCircularBufferLength) {
            mCircularBufferWriteHead = 0;
            
            
        }
    }

}

//==============================================================================
bool QubeChorusflangerAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

AudioProcessorEditor* QubeChorusflangerAudioProcessor::createEditor()
{
    return new QubeChorusflangerAudioProcessorEditor (*this);
}

//==============================================================================
void QubeChorusflangerAudioProcessor::getStateInformation (MemoryBlock& destData)
{
    std::unique_ptr<XmlElement> xml(new XmlElement("ChorusFlanger"));
    
    xml->setAttribute("DryWet", *mDryWetParameter);
    xml->setAttribute("Depth", *mDepthParameter);
    xml->setAttribute("Rate", *mRateParameter);
    xml->setAttribute("PhaseOffset", *mPhaseOffsetParameter);
    xml->setAttribute("Feedback", *mFeedbackParameter);
    xml->setAttribute("Type", *mTypeParameter);
    
    copyXmlToBinary(*xml, destData);
}

void QubeChorusflangerAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    std::unique_ptr<XmlElement> xml(getXmlFromBinary(data, sizeInBytes));
    
    if (xml.get() != nullptr && xml->hasTagName("ChorusFlanger"))
    {
        *mDryWetParameter = xml->getDoubleAttribute("DryWet");
        *mDepthParameter = xml->getDoubleAttribute("Depth");
        *mRateParameter = xml->getDoubleAttribute("Rate");
        *mPhaseOffsetParameter = xml->getDoubleAttribute("PhaseOffset");
        *mFeedbackParameter = xml->getDoubleAttribute("Feedback");
        *mTypeParameter = xml->getIntAttribute("Type");

    }
}

//==============================================================================
// This creates new instances of the plugin..
AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new QubeChorusflangerAudioProcessor();
}

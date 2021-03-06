/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
ClarityPluginAudioProcessor::ClarityPluginAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       )
#endif
{
    //Setiing the max min and default of the gain
    addParameter(mGainParameter = new juce::AudioParameterFloat("gainID",
                                                                "Gain",
                                                                0.0f,
                                                                2.0f,
                                                                1.0));
    // Smoothing the gain, getting rid of pops
    
    mGainSmoothed = mGainParameter -> get();
}

ClarityPluginAudioProcessor::~ClarityPluginAudioProcessor()
{
}

//==============================================================================
const juce::String ClarityPluginAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool ClarityPluginAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool ClarityPluginAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool ClarityPluginAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double ClarityPluginAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int ClarityPluginAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int ClarityPluginAudioProcessor::getCurrentProgram()
{
    return 0;
}

void ClarityPluginAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String ClarityPluginAudioProcessor::getProgramName (int index)
{
    return {};
}

void ClarityPluginAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void ClarityPluginAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
}

void ClarityPluginAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool ClarityPluginAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
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

void ClarityPluginAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    // In case we have more outputs than inputs, this code clears any output
    // channels that didn't contain input data, (because these aren't
    // guaranteed to be empty - they may contain garbage).
    // This is here to avoid people getting screaming feedback
    // when they first compile a plugin, but obviously you don't need to keep
    // this code if your algorithm always overwrites all the output channels.
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

    // This is the place where you'd normally do the guts of your plugin's
    // audio processing...
    // Make sure to reset the state if your inner loop is processing
    // the samples and the outer loop is handling the channels.
    // Alternatively, you can process the samples with the channels
    // interleaved by keeping the same state.
    
    
    // I got rid of this outer loop, because I had to grab left and right channels at the same time (Liam)
    
    //for (int channel = 0; channel < totalNumInputChannels; ++channel)
    //{
    
    
    //left channel
    auto* channelLeft = buffer.getWritePointer (0);
    
    //right channel
    auto* channelRight = buffer.getWritePointer (1);

        // ..do something to the data...
    for(int sample = 0; sample < buffer.getNumSamples(); sample++)
    {
        // Smoothing (formula: x = x-z * (x-y) / x=smoothed value, y= target value, z= speed.
        
        mGainSmoothed = mGainSmoothed - 0.001 * (mGainSmoothed - mGainParameter -> get() );
        
        channelLeft[sample] *= mGainSmoothed;
        channelRight[sample] *= mGainSmoothed;
        
        DBG(*mGainParameter);
    }
    
    
    //}   dont touch! outer loop closing bracket (Liam)
}

//==============================================================================
bool ClarityPluginAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
    
    //return false; //Changed it to false in order to disable the editor (Liam)
}

juce::AudioProcessorEditor* ClarityPluginAudioProcessor::createEditor()
{
    return new ClarityPluginAudioProcessorEditor (*this);
    
    //return nullptr; //Changed it to false in order to disable the editor,
    //comment above line to disable the editor (Liam) 
}

//==============================================================================
void ClarityPluginAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void ClarityPluginAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new ClarityPluginAudioProcessor();
}

#pragma once
#include <JuceHeader.h>
#include "Globals/Parameters.h"

class ViatorvoicelevelerAudioProcessor  : public juce::AudioProcessor
, public juce::AudioProcessorValueTreeState::Listener
{
public:
    //==============================================================================
    ViatorvoicelevelerAudioProcessor();
    ~ViatorvoicelevelerAudioProcessor() override;

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

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
    
    // params
    ViatorParameters::Params _parameterMap;
    juce::AudioProcessorValueTreeState _treeState;
    juce::ValueTree variableTree
    { "Variables", {},
        {
        { "Group", {{ "name", "Vars" }},
            {
                { "Parameter", {{ "id", "width" }, { "value", 0.0 }}},
                { "Parameter", {{ "id", "height" }, { "value", 0.0 }}},
                { "Parameter", {{ "id", "presetFolder" }, { "value", "" }}},
                { "Parameter", {{ "id", "presetMenu" }, { "value", 1 }}},
                { "Parameter", {{ "id", "tooltipState" }, { "value", 1 }}},
                { "Parameter", {{ "id", "colorMenu" }, { "value", 1 }}}
            }
        }
        }
    };
    
    float _width = 0.0f;
    float _height = 0.0f;
    
    void calculateRMS(juce::AudioBuffer<float>& buffer);
    float getCurrentRMS();

private:
    juce::dsp::ProcessSpec _spec;
    
    juce::dsp::Gain<float> _gainModule;
    juce::dsp::Gain<float> _volumeModule;
    juce::dsp::Limiter<float> _limiterModule;
    juce::dsp::Compressor<float> _compressorModule;
    
private:
    juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();
    void parameterChanged (const juce::String& parameterID, float newValue) override;
    using Parameter = juce::AudioProcessorValueTreeState::Parameter;
    static juce::String valueToTextFunction(float x) { return juce::String(static_cast<int>(x)); }
    static float textToValueFunction(const juce::String& str) { return str.getFloatValue(); }
    
    void updateParameters();
    
private:
    juce::SmoothedValue<double> levelGain = -60.0;
    float peakDB = -60.0;
    float rmsDB = -60.0;
    int count = 0;
    int secondElapsed = 0;
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ViatorvoicelevelerAudioProcessor)
};

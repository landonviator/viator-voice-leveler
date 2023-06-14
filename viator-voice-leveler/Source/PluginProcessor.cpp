#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
ViatorvoicelevelerAudioProcessor::ViatorvoicelevelerAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       )
, _treeState(*this, nullptr, "PARAMETERS", createParameterLayout())
#endif
{
    // sliders
    for (int i = 0; i < _parameterMap.getSliderParams().size(); i++)
    {
        _treeState.addParameterListener(_parameterMap.getSliderParams()[i].paramID, this);
    }
    
    // buttons
    for (int i = 0; i < _parameterMap.getButtonParams().size(); i++)
    {
        _treeState.addParameterListener(_parameterMap.getButtonParams()[i]._id, this);
    }
    
    // init var states
    variableTree.setProperty("width", 0, nullptr);
    variableTree.setProperty("heigt", 0, nullptr);
    variableTree.setProperty("colorMenu", 1, nullptr);
    variableTree.setProperty("tooltipState", 1, nullptr);
}

ViatorvoicelevelerAudioProcessor::~ViatorvoicelevelerAudioProcessor()
{
    // sliders
    for (int i = 0; i < _parameterMap.getSliderParams().size(); i++)
    {
        _treeState.removeParameterListener(_parameterMap.getSliderParams()[i].paramID, this);
    }
    
    // buttons
    for (int i = 0; i < _parameterMap.getButtonParams().size(); i++)
    {
        _treeState.removeParameterListener(_parameterMap.getButtonParams()[i]._id, this);
    }
}

//==============================================================================
const juce::String ViatorvoicelevelerAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool ViatorvoicelevelerAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool ViatorvoicelevelerAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool ViatorvoicelevelerAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double ViatorvoicelevelerAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int ViatorvoicelevelerAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int ViatorvoicelevelerAudioProcessor::getCurrentProgram()
{
    return 0;
}

void ViatorvoicelevelerAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String ViatorvoicelevelerAudioProcessor::getProgramName (int index)
{
    return {};
}

void ViatorvoicelevelerAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

juce::AudioProcessorValueTreeState::ParameterLayout ViatorvoicelevelerAudioProcessor::createParameterLayout()
{
    std::vector <std::unique_ptr<juce::RangedAudioParameter>> params;
    
    // sliders
    for (int i = 0; i < _parameterMap.getSliderParams().size(); i++)
    {
        auto param = _parameterMap.getSliderParams()[i];
        
        if (param.isInt == ViatorParameters::SliderParameterData::NumericType::kInt || param.isSkew == ViatorParameters::SliderParameterData::SkewType::kSkew)
        {
            auto range = juce::NormalisableRange<float>(param.min, param.max);
            
            if (param.isSkew == ViatorParameters::SliderParameterData::SkewType::kSkew)
            {
                range.setSkewForCentre(param.center);
            }
            
            params.push_back (std::make_unique<juce::AudioProcessorValueTreeState::Parameter>(juce::ParameterID { param.paramID, 1 }, param.name, param.name, range, param.initial, valueToTextFunction, textToValueFunction));
        }
        
        else
        {
            params.push_back (std::make_unique<juce::AudioParameterFloat>(juce::ParameterID { param.paramID, 1 }, param.name, param.min, param.max, param.initial));
        }
    }
    
    // buttons
    for (int i = 0; i < _parameterMap.getButtonParams().size(); i++)
    {
        auto param = _parameterMap.getButtonParams()[i];
        params.push_back (std::make_unique<juce::AudioParameterBool>(juce::ParameterID { param._id, 1 }, param._name, false));
    }
        
    return { params.begin(), params.end() };
}

void ViatorvoicelevelerAudioProcessor::parameterChanged(const juce::String &parameterID, float newValue)

{
    updateParameters();
}

void ViatorvoicelevelerAudioProcessor::updateParameters()
{
    // params
    auto gain = _treeState.getRawParameterValue(ViatorParameters::inputID)->load();
    auto volume = _treeState.getRawParameterValue(ViatorParameters::outputID)->load();
    auto leveler = _treeState.getRawParameterValue(ViatorParameters::levelID)->load();
    auto levelerScaled = juce::jmap(leveler, 0.0f, 100.0f, 0.0f, -30.0f);
    
    // update
    _gainModule.setGainDecibels(gain);
    
//    _compressorModule.setThreshold(thresh);
//    _compressorModule.setRatio(ratio);
//    _compressorModule.setAttack(attack);
//    _compressorModule.setRelease(release);
    
    _limiterModule.setThreshold(levelerScaled);
    _limiterCompensationModule.setGainDecibels(levelerScaled);
    
    _volumeModule.setGainDecibels(volume);
}

//==============================================================================
void ViatorvoicelevelerAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    _spec.sampleRate = sampleRate;
    _spec.maximumBlockSize = samplesPerBlock;
    _spec.numChannels = getTotalNumInputChannels();
    
    _gainModule.prepare(_spec);
    _gainModule.setRampDurationSeconds(0.02);
    
    _limiterModule.prepare(_spec);
    _limiterModule.setRelease(50.0);
    _limiterCompensationModule.prepare(_spec);
    _limiterCompensationModule.setRampDurationSeconds(0.02);
    
    _volumeModule.prepare(_spec);
    _volumeModule.setRampDurationSeconds(0.02);
    
    levelGain.reset(sampleRate, 0.1);
    
    _compressorModule.prepare(_spec);
    
    updateParameters();
    
    //secondElapsed = static_cast<int>(getSampleRate() / getBlockSize());
    
}

void ViatorvoicelevelerAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool ViatorvoicelevelerAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
    return layouts.getMainOutputChannelSet() == juce::AudioChannelSet::mono()
        || layouts.getMainOutputChannelSet() == juce::AudioChannelSet::stereo();
}
#endif

void ViatorvoicelevelerAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::dsp::AudioBlock<float> block {buffer};

    // gain
    _gainModule.process(juce::dsp::ProcessContextReplacing<float>(block));
    
//    if (count > secondElapsed)
//    {
//        calculateRMS(buffer);
//
//        if (rmsDB - peakDB < -10.0)
//        {
//            _compressorModule.setThreshold(levelGain.getNextValue());
//        }
//
//        count = 0;
//    }
//
//    _compressorModule.process(juce::dsp::ProcessContextReplacing<float>(block));
    
    // limiter
    _limiterModule.process(juce::dsp::ProcessContextReplacing<float>(block));
    _limiterCompensationModule.process(juce::dsp::ProcessContextReplacing<float>(block));
    
    // volume
    _volumeModule.process(juce::dsp::ProcessContextReplacing<float>(block));
    
    //count++;
}

void ViatorvoicelevelerAudioProcessor::calculateRMS(juce::AudioBuffer<float> &buffer)
{
    levelGain.skip(buffer.getNumSamples());
    peakDB = juce::Decibels::gainToDecibels(buffer.getMagnitude(0, 0, buffer.getNumSamples()));
    rmsDB = juce::Decibels::gainToDecibels(buffer.getRMSLevel(0, 0, buffer.getNumSamples()));
    
    if (rmsDB < levelGain.getCurrentValue())
    {
        levelGain.setTargetValue(rmsDB);
    }

    else
    {
        levelGain.setCurrentAndTargetValue(rmsDB);
    }
}

float ViatorvoicelevelerAudioProcessor::getCurrentRMS()
{
    return levelGain.getNextValue();
}

//==============================================================================
bool ViatorvoicelevelerAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* ViatorvoicelevelerAudioProcessor::createEditor()
{
    return new ViatorvoicelevelerAudioProcessorEditor (*this);
    //return new juce::GenericAudioProcessorEditor (*this);
}

//==============================================================================
void ViatorvoicelevelerAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    _treeState.state.appendChild(variableTree, nullptr);
    juce::MemoryOutputStream stream(destData, false);
    _treeState.state.writeToStream (stream);
}

void ViatorvoicelevelerAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    auto tree = juce::ValueTree::readFromData (data, size_t(sizeInBytes));
    variableTree = tree.getChildWithName("Variables");
    
    if (tree.isValid())
    {
        _treeState.state = tree;
        _width = variableTree.getProperty("width");
        _height = variableTree.getProperty("height");
    }
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new ViatorvoicelevelerAudioProcessor();
}

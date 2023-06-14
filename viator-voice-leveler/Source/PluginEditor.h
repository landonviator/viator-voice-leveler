#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "includes.h"
#include "LAF/Colors.h"

class ViatorvoicelevelerAudioProcessorEditor  : public juce::AudioProcessorEditor
, public juce::ChangeListener
, private juce::Timer
{
public:
    ViatorvoicelevelerAudioProcessorEditor (ViatorvoicelevelerAudioProcessor&);
    ~ViatorvoicelevelerAudioProcessorEditor() override;

    void paint (juce::Graphics&) override;
    void resized() override;
    
    ViatorThemes::ViatorThemeData& getThemeData()
    {
            return _theme;
    }
    
    void setTooltipText(const juce::String& tooltip);
    
    void timerCallback() override
    {
        _levelMeter.setValue(audioProcessor.getCurrentRMS());
    }

private:
    ViatorvoicelevelerAudioProcessor& audioProcessor;
    
    // theme
    ViatorThemes::ViatorThemeData _theme;
    
    // comps
    Header _headerComp;
    SettingsPage _settingsComp;
    
    // dials
    juce::OwnedArray<viator_gui::Dial> _pluginDials;
    juce::OwnedArray<juce::AudioProcessorValueTreeState::SliderAttachment> _pluginSliderAttachments;
    juce::OwnedArray<viator_gui::Dial> _ioDials;
    juce::OwnedArray<juce::AudioProcessorValueTreeState::SliderAttachment> _ioSliderAttachments;
    
    // tooltip
    viator_gui::Label _tooltipLabel;
    juce::StringArray _pluginDialTooltips =
    {
      "Increases the consistency of your voical signal. Very high dial settings will cause gnarly distortion!"
    };
    
    juce::StringArray _ioDialTooltips =
    {
      "Sets the volume of the signal coming into the plugin.", "Sets the volume of the signal coming out of the plugin."
    };
    
    juce::Slider _levelMeter;
    
private:
    // window
    void setWindowSizeLogic();
    
    // background
    void setBackgroundStyle(juce::Graphics& g);
    
    // settings
    void setSettingsState(bool isActive);
    void initOverlayProps();
    
    // change listener
    void changeListenerCallback(juce::ChangeBroadcaster *source) override;
    
    // dials
    void updateSliderColors();
    void initDialProps();
    
    // tooltip
    void initTooltipLabel();
    void mouseEnter(const juce::MouseEvent &event) override;
    void mouseExit(const juce::MouseEvent &event) override;
    
private:
    const int _numPluginDials = 1;
    const int _numIODials = 2;
    juce::Rectangle<int> _headerArea;
    juce::Rectangle<int> _ioArea;
    juce::Rectangle<int> _pluginArea;
    
    std::vector<juce::String> _pluginDialNames =
    {
      "Level-fy"
    };
    
    std::vector<juce::String> _ioDialNames =
    {
        "Input", "Master Volume"
    };

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ViatorvoicelevelerAudioProcessorEditor)
};

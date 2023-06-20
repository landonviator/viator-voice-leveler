#include "Parameters.h"


ViatorParameters::Params::Params()
{
    initSliderParams();
    initButtonParams();
}

std::vector<ViatorParameters::SliderParameterData>& ViatorParameters::Params::getSliderParams()
{
    return _sliderParams;
}

std::vector<ViatorParameters::SliderParameterData>& ViatorParameters::Params::getPluginSliderParams()
{
    return _pluginSliderParams;
}

std::vector<ViatorParameters::SliderParameterData>& ViatorParameters::Params::getIOSliderParams()
{
    return _ioSliderParams;
}

void ViatorParameters::Params::initSliderParams()
{
    using skew = SliderParameterData::SkewType;
    using type = SliderParameterData::NumericType;
    _sliderParams.push_back({ViatorParameters::levelID, ViatorParameters::levelName, 0.0, 100.0, 0.0, skew::kNoSkew, 50.0, type::kInt});
    _sliderParams.push_back({ViatorParameters::inputID, ViatorParameters::inputName, -20.0f, 20.0f, 0.0f, skew::kNoSkew, 0.0, type::kFloat});
    _sliderParams.push_back({ViatorParameters::outputID, ViatorParameters::outputName, -20.0f, 20.0f, 0.0f, skew::kNoSkew, 0.0, type::kFloat});
    _sliderParams.push_back({ViatorParameters::offsetID, ViatorParameters::offsetName, 0.0f, 60.0f, 0.0f, skew::kNoSkew, 0.0, type::kInt});
    _sliderParams.push_back({ViatorParameters::threshID, ViatorParameters::threshName, -60.0f, 0.0f, 0.0f, skew::kNoSkew, 0.0, type::kFloat});
    _sliderParams.push_back({ViatorParameters::ratioID, ViatorParameters::ratioName, 1.0f, 50.0f, 1.0f, skew::kNoSkew, 0.0, type::kInt});
    _sliderParams.push_back({ViatorParameters::attackID, ViatorParameters::attackName, 1.0f, 100.0f, 5.0f, skew::kNoSkew, 0.0, type::kInt});
    _sliderParams.push_back({ViatorParameters::releaseID, ViatorParameters::releaseName, 1.0f, 100.0f, 50.0f, skew::kNoSkew, 0.0, type::kInt});
    _sliderParams.push_back({ViatorParameters::gainCompID, ViatorParameters::gainCompName, 0.0f, 100.0f, 15.0f, skew::kNoSkew, 0.0, type::kFloat});
    _sliderParams.push_back({ViatorParameters::limiterCompID, ViatorParameters::limiterCompName, 0.0f, 100.0f, 50.0f, skew::kNoSkew, 0.0, type::kFloat});
    
    // for attachments
    _pluginSliderParams.push_back({ViatorParameters::levelID, ViatorParameters::levelName, 0.0, 100.0, 0.0, skew::kSkew, 75.0, type::kInt});
    _ioSliderParams.push_back({ViatorParameters::inputID, ViatorParameters::inputName, -20.0f, 20.0f, 0.0f, skew::kNoSkew, 0.0, type::kFloat});
    _ioSliderParams.push_back({ViatorParameters::outputID, ViatorParameters::outputName, -20.0f, 20.0f, 0.0f, skew::kNoSkew, 0.0, type::kFloat});
}

std::vector<ViatorParameters::ButtonParameterData>& ViatorParameters::Params::getButtonParams()
{
    return _buttonParams;
}

void ViatorParameters::Params::initButtonParams()
{
}

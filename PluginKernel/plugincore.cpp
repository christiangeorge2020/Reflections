// -----------------------------------------------------------------------------
//    ASPiK Plugin Kernel File:  plugincore.cpp
//
/**
    \file   plugincore.cpp
    \author Will Pirkle
    \date   17-September-2018
    \brief  Implementation file for PluginCore object
    		- http://www.aspikplugins.com
    		- http://www.willpirkle.com
*/
// -----------------------------------------------------------------------------
#include "plugincore.h"
#include "plugindescription.h"

/**
\brief PluginCore constructor is launching pad for object initialization

Operations:
- initialize the plugin description (strings, codes, numbers, see initPluginDescriptors())
- setup the plugin's audio I/O channel support
- create the PluginParameter objects that represent the plugin parameters (see FX book if needed)
- create the presets
*/
PluginCore::PluginCore()
{
    // --- describe the plugin; call the helper to init the static parts you setup in plugindescription.h
    initPluginDescriptors();

    // --- default I/O combinations
	// --- for FX plugins
	if (getPluginType() == kFXPlugin)
	{
		addSupportedIOCombination({ kCFMono, kCFMono });
		addSupportedIOCombination({ kCFMono, kCFStereo });
		addSupportedIOCombination({ kCFStereo, kCFStereo });
	}
	else // --- synth plugins have no input, only output
	{
		addSupportedIOCombination({ kCFNone, kCFMono });
		addSupportedIOCombination({ kCFNone, kCFStereo });
	}

	// --- for sidechaining, we support mono and stereo inputs; auxOutputs reserved for future use
	addSupportedAuxIOCombination({ kCFMono, kCFNone });
	addSupportedAuxIOCombination({ kCFStereo, kCFNone });

	// --- create the parameters
    initPluginParameters();

    // --- create the presets
    initPluginPresets();
}

/**
\brief create all of your plugin parameters here

\return true if parameters were created, false if they already existed
*/
bool PluginCore::initPluginParameters()
{
	if (pluginParameterMap.size() > 0)
		return false;

    // --- Add your plugin parameter instantiation code bewtween these hex codes
	// **--0xDEA7--**


	// --- Declaration of Plugin Parameter Objects 
	PluginParameter* piParam = nullptr;

	// --- continuous control: Rate
	piParam = new PluginParameter(controlID::delay_ms, "Rate", "ms", controlVariableType::kDouble, 0.000000, 2000.000000, 400.000000, taper::kAntiLogTaper);
	piParam->setParameterSmoothing(false);
	piParam->setSmoothingTimeMsec(100.00);
	piParam->setBoundVariable(&delay_ms, boundVariableType::kDouble);
	addPluginParameter(piParam);

	// --- continuous control: Intensity
	piParam = new PluginParameter(controlID::feedback1_pct, "Intensity", "%", controlVariableType::kDouble, 0.000000, 100.000000, 0.000000, taper::kLinearTaper);
	piParam->setParameterSmoothing(false);
	piParam->setSmoothingTimeMsec(100.00);
	piParam->setBoundVariable(&feedback1_pct, boundVariableType::kDouble);
	addPluginParameter(piParam);

	// --- continuous control: Echo Volume
	piParam = new PluginParameter(controlID::wetVolume_db, "Echo Volume", "dB", controlVariableType::kDouble, -60.000000, 10.000000, -3.000000, taper::kLinearTaper);
	piParam->setParameterSmoothing(false);
	piParam->setSmoothingTimeMsec(100.00);
	piParam->setBoundVariable(&wetVolume_db, boundVariableType::kDouble);
	addPluginParameter(piParam);

	// --- continuous control: Attack
	piParam = new PluginParameter(controlID::attack_ms, "Attack", "ms", controlVariableType::kDouble, 0.000000, 50.000000, 10.000000, taper::kLinearTaper);
	piParam->setParameterSmoothing(false);
	piParam->setSmoothingTimeMsec(100.00);
	piParam->setBoundVariable(&attack_ms, boundVariableType::kDouble);
	addPluginParameter(piParam);

	// --- continuous control: Release
	piParam = new PluginParameter(controlID::release_ms, "Release", "ms", controlVariableType::kDouble, 10.000000, 500.000000, 50.000000, taper::kLinearTaper);
	piParam->setParameterSmoothing(false);
	piParam->setSmoothingTimeMsec(100.00);
	piParam->setBoundVariable(&release_ms, boundVariableType::kDouble);
	addPluginParameter(piParam);

	// --- continuous control: Threshold
	piParam = new PluginParameter(controlID::threshold_db, "Threshold", "dB", controlVariableType::kDouble, -40.000000, 0.000000, -15.000000, taper::kLinearTaper);
	piParam->setParameterSmoothing(false);
	piParam->setSmoothingTimeMsec(100.00);
	piParam->setBoundVariable(&threshold_db, boundVariableType::kDouble);
	addPluginParameter(piParam);

	// --- discrete control: Effect
	piParam = new PluginParameter(controlID::delayFX, "Effect", "Flanger,Chorus,Vibrato", "Flanger");
	piParam->setBoundVariable(&delayFX, boundVariableType::kInt);
	piParam->setIsDiscreteSwitch(true);
	addPluginParameter(piParam);

	// --- discrete control: Enable FX
	piParam = new PluginParameter(controlID::enableDelayFX, "Enable FX", "SWITCH OFF,SWITCH ON", "SWITCH OFF");
	piParam->setBoundVariable(&enableDelayFX, boundVariableType::kInt);
	piParam->setIsDiscreteSwitch(true);
	addPluginParameter(piParam);

	// --- continuous control: FX Rate
	piParam = new PluginParameter(controlID::fxRate, "FX Rate", "Hz", controlVariableType::kDouble, 0.010000, 20.000000, 1.000000, taper::kLinearTaper);
	piParam->setParameterSmoothing(false);
	piParam->setSmoothingTimeMsec(100.00);
	piParam->setBoundVariable(&fxRate, boundVariableType::kDouble);
	addPluginParameter(piParam);

	// --- continuous control: FX Depth
	piParam = new PluginParameter(controlID::fxDepth, "FX Depth", "%", controlVariableType::kDouble, 0.000000, 100.000000, 50.000000, taper::kLinearTaper);
	piParam->setParameterSmoothing(false);
	piParam->setSmoothingTimeMsec(100.00);
	piParam->setBoundVariable(&fxDepth, boundVariableType::kDouble);
	addPluginParameter(piParam);

	// --- continuous control: FX Feedback
	piParam = new PluginParameter(controlID::fxFeedback, "FX Feedback", "%", controlVariableType::kDouble, 0.000000, 100.000000, 0.000000, taper::kLinearTaper);
	piParam->setParameterSmoothing(false);
	piParam->setSmoothingTimeMsec(100.00);
	piParam->setBoundVariable(&fxFeedback, boundVariableType::kDouble);
	addPluginParameter(piParam);

	// --- continuous control: Env Amount
	piParam = new PluginParameter(controlID::envAmount, "Env Amount", "", controlVariableType::kDouble, 1.000000, 10.000000, 1.000000, taper::kLinearTaper);
	piParam->setParameterSmoothing(false);
	piParam->setSmoothingTimeMsec(100.00);
	piParam->setBoundVariable(&envAmount, boundVariableType::kDouble);
	addPluginParameter(piParam);

	// --- discrete control: Envelope
	piParam = new PluginParameter(controlID::enableEnvelope, "Envelope", "SWITCH OFF,SWITCH ON", "SWITCH OFF");
	piParam->setBoundVariable(&enableEnvelope, boundVariableType::kInt);
	piParam->setIsDiscreteSwitch(true);
	addPluginParameter(piParam);

	// --- discrete control: Sidechain
	piParam = new PluginParameter(controlID::enableSidechain, "Sidechain", "SWITCH OFF,SWITCH ON", "SWITCH OFF");
	piParam->setBoundVariable(&enableSidechain, boundVariableType::kInt);
	piParam->setIsDiscreteSwitch(true);
	addPluginParameter(piParam);

	// --- discrete control: LFO
	piParam = new PluginParameter(controlID::enableLFO, "LFO", "SWITCH OFF,SWITCH ON", "SWITCH OFF");
	piParam->setBoundVariable(&enableLFO, boundVariableType::kInt);
	piParam->setIsDiscreteSwitch(true);
	addPluginParameter(piParam);

	// --- discrete control: Delay 1 BPM
	piParam = new PluginParameter(controlID::delay1_bpm, "Delay 1 BPM", "Half,Quarter,Quarter Triplet,Eighth,Eighth Triplet,Sixteenth", "Half");
	piParam->setBoundVariable(&delay1_bpm, boundVariableType::kInt);
	piParam->setIsDiscreteSwitch(true);
	addPluginParameter(piParam);

	// --- discrete control: Env Target
	piParam = new PluginParameter(controlID::envTarget, "Env Target", "Delay,FX", "Delay");
	piParam->setBoundVariable(&envTarget, boundVariableType::kInt);
	piParam->setIsDiscreteSwitch(true);
	addPluginParameter(piParam);

	// --- discrete control: Sidechain Target
	piParam = new PluginParameter(controlID::sidechainTarget, "Sidechain Target", "Delay,FX", "Delay");
	piParam->setBoundVariable(&sidechainTarget, boundVariableType::kInt);
	piParam->setIsDiscreteSwitch(true);
	addPluginParameter(piParam);

	// --- discrete control: LFO Target
	piParam = new PluginParameter(controlID::lfoTarget, "LFO Target", "Delay,FX", "Delay");
	piParam->setBoundVariable(&lfoTarget, boundVariableType::kInt);
	piParam->setIsDiscreteSwitch(true);
	addPluginParameter(piParam);

	// --- continuous control: LFO Rate
	piParam = new PluginParameter(controlID::lfoRate_hz, "LFO Rate", "Hz", controlVariableType::kDouble, 0.100000, 10.000000, 1.000000, taper::kLinearTaper);
	piParam->setParameterSmoothing(false);
	piParam->setSmoothingTimeMsec(100.00);
	piParam->setBoundVariable(&lfoRate_hz, boundVariableType::kDouble);
	addPluginParameter(piParam);

	// --- continuous control: LFO Amount
	piParam = new PluginParameter(controlID::lfoAmount, "LFO Amount", "", controlVariableType::kDouble, 1.000000, 10.000000, 1.000000, taper::kLinearTaper);
	piParam->setParameterSmoothing(false);
	piParam->setSmoothingTimeMsec(100.00);
	piParam->setBoundVariable(&lfoAmount, boundVariableType::kDouble);
	addPluginParameter(piParam);

	// --- continuous control: Mode
	piParam = new PluginParameter(controlID::mode, "Mode", "", controlVariableType::kInt, 1.000000, 6.000000, 1.000000, taper::kLinearTaper);
	piParam->setParameterSmoothing(false);
	piParam->setSmoothingTimeMsec(100.00);
	piParam->setBoundVariable(&mode, boundVariableType::kInt);
	addPluginParameter(piParam);

	// --- continuous control: Dry Volume
	piParam = new PluginParameter(controlID::dryVolume_db, "Dry Volume", "dB", controlVariableType::kDouble, -60.000000, 10.000000, -3.000000, taper::kLinearTaper);
	piParam->setParameterSmoothing(false);
	piParam->setSmoothingTimeMsec(100.00);
	piParam->setBoundVariable(&dryVolume_db, boundVariableType::kDouble);
	addPluginParameter(piParam);

	// --- discrete control: BPM Sync
	piParam = new PluginParameter(controlID::enableBPM, "BPM Sync", "SWITCH OFF,SWITCH ON", "SWITCH OFF");
	piParam->setBoundVariable(&enableBPM, boundVariableType::kInt);
	piParam->setIsDiscreteSwitch(true);
	addPluginParameter(piParam);

	// --- continuous control: Sidechain Amount
	piParam = new PluginParameter(controlID::sidechainAmt, "Sidechain Amount", "Units", controlVariableType::kDouble, 1.000000, 10.000000, 1.000000, taper::kLinearTaper);
	piParam->setParameterSmoothing(false);
	piParam->setSmoothingTimeMsec(100.00);
	piParam->setBoundVariable(&sidechainAmt, boundVariableType::kDouble);
	addPluginParameter(piParam);

	// --- continuous control: Sidechain Threshold
	piParam = new PluginParameter(controlID::sidechainThresh_db, "Sidechain Threshold", "dB", controlVariableType::kDouble, -40.000000, 0.000000, -20.000000, taper::kLinearTaper);
	piParam->setParameterSmoothing(false);
	piParam->setSmoothingTimeMsec(100.00);
	piParam->setBoundVariable(&sidechainThresh_db, boundVariableType::kDouble);
	addPluginParameter(piParam);

	// --- Aux Attributes
	AuxParameterAttribute auxAttribute;

	// --- RAFX GUI attributes
	// --- controlID::delay_ms
	auxAttribute.reset(auxGUIIdentifier::guiControlData);
	auxAttribute.setUintAttribute(2147483648);
	setParamAuxAttribute(controlID::delay_ms, auxAttribute);

	// --- controlID::feedback1_pct
	auxAttribute.reset(auxGUIIdentifier::guiControlData);
	auxAttribute.setUintAttribute(2147483648);
	setParamAuxAttribute(controlID::feedback1_pct, auxAttribute);

	// --- controlID::wetVolume_db
	auxAttribute.reset(auxGUIIdentifier::guiControlData);
	auxAttribute.setUintAttribute(2147483648);
	setParamAuxAttribute(controlID::wetVolume_db, auxAttribute);

	// --- controlID::attack_ms
	auxAttribute.reset(auxGUIIdentifier::guiControlData);
	auxAttribute.setUintAttribute(2147483648);
	setParamAuxAttribute(controlID::attack_ms, auxAttribute);

	// --- controlID::release_ms
	auxAttribute.reset(auxGUIIdentifier::guiControlData);
	auxAttribute.setUintAttribute(2147483648);
	setParamAuxAttribute(controlID::release_ms, auxAttribute);

	// --- controlID::threshold_db
	auxAttribute.reset(auxGUIIdentifier::guiControlData);
	auxAttribute.setUintAttribute(2147483648);
	setParamAuxAttribute(controlID::threshold_db, auxAttribute);

	// --- controlID::delayFX
	auxAttribute.reset(auxGUIIdentifier::guiControlData);
	auxAttribute.setUintAttribute(805306368);
	setParamAuxAttribute(controlID::delayFX, auxAttribute);

	// --- controlID::enableDelayFX
	auxAttribute.reset(auxGUIIdentifier::guiControlData);
	auxAttribute.setUintAttribute(1073741824);
	setParamAuxAttribute(controlID::enableDelayFX, auxAttribute);

	// --- controlID::fxRate
	auxAttribute.reset(auxGUIIdentifier::guiControlData);
	auxAttribute.setUintAttribute(2147483648);
	setParamAuxAttribute(controlID::fxRate, auxAttribute);

	// --- controlID::fxDepth
	auxAttribute.reset(auxGUIIdentifier::guiControlData);
	auxAttribute.setUintAttribute(2147483648);
	setParamAuxAttribute(controlID::fxDepth, auxAttribute);

	// --- controlID::fxFeedback
	auxAttribute.reset(auxGUIIdentifier::guiControlData);
	auxAttribute.setUintAttribute(2147483648);
	setParamAuxAttribute(controlID::fxFeedback, auxAttribute);

	// --- controlID::envAmount
	auxAttribute.reset(auxGUIIdentifier::guiControlData);
	auxAttribute.setUintAttribute(2147483648);
	setParamAuxAttribute(controlID::envAmount, auxAttribute);

	// --- controlID::enableEnvelope
	auxAttribute.reset(auxGUIIdentifier::guiControlData);
	auxAttribute.setUintAttribute(1073741824);
	setParamAuxAttribute(controlID::enableEnvelope, auxAttribute);

	// --- controlID::enableSidechain
	auxAttribute.reset(auxGUIIdentifier::guiControlData);
	auxAttribute.setUintAttribute(1073741824);
	setParamAuxAttribute(controlID::enableSidechain, auxAttribute);

	// --- controlID::enableLFO
	auxAttribute.reset(auxGUIIdentifier::guiControlData);
	auxAttribute.setUintAttribute(1073741824);
	setParamAuxAttribute(controlID::enableLFO, auxAttribute);

	// --- controlID::delay1_bpm
	auxAttribute.reset(auxGUIIdentifier::guiControlData);
	auxAttribute.setUintAttribute(805306368);
	setParamAuxAttribute(controlID::delay1_bpm, auxAttribute);

	// --- controlID::envTarget
	auxAttribute.reset(auxGUIIdentifier::guiControlData);
	auxAttribute.setUintAttribute(805306368);
	setParamAuxAttribute(controlID::envTarget, auxAttribute);

	// --- controlID::sidechainTarget
	auxAttribute.reset(auxGUIIdentifier::guiControlData);
	auxAttribute.setUintAttribute(805306368);
	setParamAuxAttribute(controlID::sidechainTarget, auxAttribute);

	// --- controlID::lfoTarget
	auxAttribute.reset(auxGUIIdentifier::guiControlData);
	auxAttribute.setUintAttribute(805306368);
	setParamAuxAttribute(controlID::lfoTarget, auxAttribute);

	// --- controlID::lfoRate_hz
	auxAttribute.reset(auxGUIIdentifier::guiControlData);
	auxAttribute.setUintAttribute(2147483648);
	setParamAuxAttribute(controlID::lfoRate_hz, auxAttribute);

	// --- controlID::lfoAmount
	auxAttribute.reset(auxGUIIdentifier::guiControlData);
	auxAttribute.setUintAttribute(2147483648);
	setParamAuxAttribute(controlID::lfoAmount, auxAttribute);

	// --- controlID::mode
	auxAttribute.reset(auxGUIIdentifier::guiControlData);
	auxAttribute.setUintAttribute(2147483648);
	setParamAuxAttribute(controlID::mode, auxAttribute);

	// --- controlID::dryVolume_db
	auxAttribute.reset(auxGUIIdentifier::guiControlData);
	auxAttribute.setUintAttribute(2147483648);
	setParamAuxAttribute(controlID::dryVolume_db, auxAttribute);

	// --- controlID::enableBPM
	auxAttribute.reset(auxGUIIdentifier::guiControlData);
	auxAttribute.setUintAttribute(1073741824);
	setParamAuxAttribute(controlID::enableBPM, auxAttribute);

	// --- controlID::sidechainAmt
	auxAttribute.reset(auxGUIIdentifier::guiControlData);
	auxAttribute.setUintAttribute(2147483648);
	setParamAuxAttribute(controlID::sidechainAmt, auxAttribute);

	// --- controlID::sidechainThresh_db
	auxAttribute.reset(auxGUIIdentifier::guiControlData);
	auxAttribute.setUintAttribute(2147483648);
	setParamAuxAttribute(controlID::sidechainThresh_db, auxAttribute);


	// **--0xEDA5--**
   
    // --- BONUS Parameter
    // --- SCALE_GUI_SIZE
    PluginParameter* piParamBonus = new PluginParameter(SCALE_GUI_SIZE, "Scale GUI", "tiny,small,medium,normal,large,giant", "normal");
    addPluginParameter(piParamBonus);

	// --- create the super fast access array
	initPluginParameterArray();

    return true;
}

/**
\brief initialize object for a new run of audio; called just before audio streams

Operation:
- store sample rate and bit depth on audioProcDescriptor - this information is globally available to all core functions
- reset your member objects here

\param resetInfo structure of information about current audio format

\return true if operation succeeds, false otherwise
*/
bool PluginCore::reset(ResetInfo& resetInfo)
{
    // --- save for audio processing
    audioProcDescriptor.sampleRate = resetInfo.sampleRate;
    audioProcDescriptor.bitDepth = resetInfo.bitDepth;

	// --- RESET is in 2 parts: first the normal call
	monoDelay.reset(resetInfo.sampleRate);

	// --- THEN create 2 second delay buffers
	monoDelay.createDelayBuffers(resetInfo.sampleRate, 2000.0);

	// --- modulated delay FX
	modDelay.reset(resetInfo.sampleRate);

	
	
    // --- other reset inits
    return PluginBase::reset(resetInfo);
}

/**
\brief one-time initialize function called after object creation and before the first reset( ) call

Operation:
- saves structure for the plugin to use; you can also load WAV files or state information here
*/
bool PluginCore::initialize(PluginInfo& pluginInfo)
{
	// --- add one-time init stuff here

	return true;
}

/**
\brief do anything needed prior to arrival of audio buffers

Operation:
- syncInBoundVariables when preProcessAudioBuffers is called, it is *guaranteed* that all GUI control change information
  has been applied to plugin parameters; this binds parameter changes to your underlying variables
- NOTE: postUpdatePluginParameter( ) will be called for all bound variables that are acutally updated; if you need to process
  them individually, do so in that function
- use this function to bulk-transfer the bound variable data into your plugin's member object variables

\param processInfo structure of information about *buffer* processing

\return true if operation succeeds, false otherwise
*/
bool PluginCore::preProcessAudioBuffers(ProcessBufferInfo& processInfo)
{
    // --- sync internal variables to GUI parameters; you can also do this manually if you don't
    //     want to use the auto-variable-binding
    syncInBoundVariables();

    return true;
}

// --- updater
void PluginCore::updateParameters() {

	// ** MULTI TAP DELAY **
	// --- get
	RE201Parameters params = monoDelay.getParameters();

	// --- dry level
	params.dryVolume = dryVolume_db;
	params.wetVolume = wetVolume_db;

	params.delay_ms = delay_ms;
	params.feedback_pct = feedback1_pct;

	// --- bpm
	params.enableBPM = enableBPM;
	
	params.delay_bpm = convertIntToEnum(delay1_bpm, rateBPM);
	
	// --- detector
	params.enableEnv = enableEnvelope;
	if (!compareIntToEnum(envTarget, envTargetEnum::Delay))
		params.enableEnv = false;
		
	params.envAmount = envAmount;
	params.threshold_db = threshold_db;

	// --- sidechain
	params.sidechainThresh_db = sidechainThresh_db;
	params.enableSidechain = enableSidechain;
	if (!compareIntToEnum(sidechainTarget, sidechainTargetEnum::Delay))
		params.enableSidechain = false;

	// --- lfo
	params.enableLFO = enableLFO;
	if (!compareIntToEnum(lfoTarget, lfoTargetEnum::Delay))
		params.enableLFO = false;
		
	params.lfoAmount = lfoAmount;
	params.lfoRate_hz = lfoRate_hz;

	// --- mode
	params.mode = mode;

	// --- set
	monoDelay.setParameters(params);


	// ** MODULATED DELAY **
	ModulatedDelayParameters modParams = modDelay.getParameters();

	modParams.algorithm = convertIntToEnum(delayFX, modDelaylgorithm);
	modParams.lfoRate_Hz = fxRate;
	modParams.lfoDepth_Pct = fxDepth;
	modParams.feedback_Pct = fxFeedback;

	modParams.enableEnv = enableEnvelope;
	if (!compareIntToEnum(envTarget, envTargetEnum::FX))
		modParams.enableEnv = false;
	modParams.envAmount = envAmount;
	modParams.threshold_db = threshold_db;
	
	modParams.enableModLFO = enableLFO;
	if (!compareIntToEnum(lfoTarget, lfoTargetEnum::FX))
		modParams.enableModLFO = false;
		

	modParams.modLFO_depthpct = lfoAmount;
	modParams.modLFO_rateHz = lfoRate_hz;

	modParams.sidechainThresh_db = sidechainThresh_db;
	modParams.enableSidechain = enableSidechain;
	if (!compareIntToEnum(sidechainTarget, sidechainTargetEnum::FX))
		modParams.enableSidechain = false;


	modDelay.setParameters(modParams);

}

/**
\brief frame-processing method

Operation:
- decode the plugin type - for synth plugins, fill in the rendering code; for FX plugins, delete the if(synth) portion and add your processing code
- note that MIDI events are fired for each sample interval so that MIDI is tightly sunk with audio
- doSampleAccurateParameterUpdates will perform per-sample interval smoothing

\param processFrameInfo structure of information about *frame* processing

\return true if operation succeeds, false otherwise
*/
bool PluginCore::processAudioFrame(ProcessFrameInfo& processFrameInfo)
{
    // --- fire any MIDI events for this sample interval
    processFrameInfo.midiEventQueue->fireMidiEvents(processFrameInfo.currentFrame);

	// --- do per-frame updates; VST automation and parameter smoothing
	doSampleAccurateParameterUpdates();

	updateParameters();

	// --- create input variables
	double xnL = processFrameInfo.audioInputFrame[0];
	double xnR = processFrameInfo.audioInputFrame[1];

	float bufferFrame[2];

	// --- bpm
	if (enableBPM) {
		monoDelay.getHostBPM(processFrameInfo.hostInfo);
	}
		

	// ** SIDECHAIN **
	double auxFrame[2];
	if (processFrameInfo.numAuxAudioInChannels > 0)
	{
		auxFrame[0] = processFrameInfo.auxAudioInputFrame[0];
		auxFrame[1] = processFrameInfo.auxAudioInputFrame[1];
		monoDelay.enableAuxInput(true); 
		monoDelay.processAuxInputAudioSample(auxFrame[0], auxFrame[1]);
		
		modDelay.enableAuxInput(true);
		modDelay.processAuxInputAudioSample(auxFrame[0], auxFrame[1]);
	}

	// ** DELAY **
	

	bool processed = monoDelay.processAudioFrame(processFrameInfo.audioInputFrame,
		processFrameInfo.audioOutputFrame,
		processFrameInfo.numAudioInChannels,
		processFrameInfo.numAudioOutChannels
	);

	bufferFrame[0] = processFrameInfo.audioOutputFrame[0];
	bufferFrame[1] = processFrameInfo.audioOutputFrame[1];

  // ** MODULATED DELAY **
	if (!enableDelayFX)
		return processed;

	processed = modDelay.processAudioFrame(bufferFrame,
		processFrameInfo.audioOutputFrame,
		processFrameInfo.numAudioInChannels,
		processFrameInfo.numAudioOutChannels);
	return processed;

	
  //  if(processFrameInfo.channelIOConfig.inputChannelFormat == kCFMono &&
  //     processFrameInfo.channelIOConfig.outputChannelFormat == kCFMono)
  //  {
		//if (!enableDelayFX)
		//	return processed;
		//
		//processFrameInfo.audioOutputFrame[0] = modDelay.processAudioSample(bufferFrame[0]);

  //      return true; /// processed
  //  }

  //  // --- Mono-In/Stereo-Out
  //  else if(processFrameInfo.channelIOConfig.inputChannelFormat == kCFMono &&
  //     processFrameInfo.channelIOConfig.outputChannelFormat == kCFStereo)
  //  {
		//if (!enableDelayFX)
		//	return processed;

		//processed = modDelay.processAudioFrame(bufferFrame,
		//	processFrameInfo.audioOutputFrame,
		//	processFrameInfo.numAudioInChannels,
		//	processFrameInfo.numAudioOutChannels);

  //      return processed; /// processed
  //  }

  //  // --- Stereo-In/Stereo-Out
  //  else if(processFrameInfo.channelIOConfig.inputChannelFormat == kCFStereo &&
  //     processFrameInfo.channelIOConfig.outputChannelFormat == kCFStereo)
  //  {
		//if (!enableDelayFX)
		//	return processed;

		//processed = modDelay.processAudioFrame(bufferFrame,
		//								processFrameInfo.audioOutputFrame,
		//								processFrameInfo.numAudioInChannels,
		//								processFrameInfo.numAudioOutChannels);
		//
  //      return processed; /// processed
  //  }
}



/**
\brief do anything needed prior to arrival of audio buffers

Operation:
- updateOutBoundVariables sends metering data to the GUI meters

\param processInfo structure of information about *buffer* processing

\return true if operation succeeds, false otherwise
*/
bool PluginCore::postProcessAudioBuffers(ProcessBufferInfo& processInfo)
{
	// --- update outbound variables; currently this is meter data only, but could be extended
	//     in the future
	updateOutBoundVariables();

    return true;
}

/**
\brief update the PluginParameter's value based on GUI control, preset, or data smoothing (thread-safe)

Operation:
- update the parameter's value (with smoothing this initiates another smoothing process)
- call postUpdatePluginParameter to do any further processing

\param controlID the control ID value of the parameter being updated
\param controlValue the new control value
\param paramInfo structure of information about why this value is being udpated (e.g as a result of a preset being loaded vs. the top of a buffer process cycle)

\return true if operation succeeds, false otherwise
*/
bool PluginCore::updatePluginParameter(int32_t controlID, double controlValue, ParameterUpdateInfo& paramInfo)
{
    // --- use base class helper
    setPIParamValue(controlID, controlValue);

    // --- do any post-processing
    postUpdatePluginParameter(controlID, controlValue, paramInfo);

    return true; /// handled
}

/**
\brief update the PluginParameter's value based on *normlaized* GUI control, preset, or data smoothing (thread-safe)

Operation:
- update the parameter's value (with smoothing this initiates another smoothing process)
- call postUpdatePluginParameter to do any further processing

\param controlID the control ID value of the parameter being updated
\param normalizedValue the new control value in normalized form
\param paramInfo structure of information about why this value is being udpated (e.g as a result of a preset being loaded vs. the top of a buffer process cycle)

\return true if operation succeeds, false otherwise
*/
bool PluginCore::updatePluginParameterNormalized(int32_t controlID, double normalizedValue, ParameterUpdateInfo& paramInfo)
{
	// --- use base class helper, returns actual value
	double controlValue = setPIParamValueNormalized(controlID, normalizedValue, paramInfo.applyTaper);

	// --- do any post-processing
	postUpdatePluginParameter(controlID, controlValue, paramInfo);

	return true; /// handled
}

/**
\brief perform any operations after the plugin parameter has been updated; this is one paradigm for
	   transferring control information into vital plugin variables or member objects. If you use this
	   method you can decode the control ID and then do any cooking that is needed. NOTE: do not
	   overwrite bound variables here - this is ONLY for any extra cooking that is required to convert
	   the GUI data to meaninful coefficients or other specific modifiers.

\param controlID the control ID value of the parameter being updated
\param controlValue the new control value
\param paramInfo structure of information about why this value is being udpated (e.g as a result of a preset being loaded vs. the top of a buffer process cycle)

\return true if operation succeeds, false otherwise
*/
bool PluginCore::postUpdatePluginParameter(int32_t controlID, double controlValue, ParameterUpdateInfo& paramInfo)
{
    // --- now do any post update cooking; be careful with VST Sample Accurate automation
    //     If enabled, then make sure the cooking functions are short and efficient otherwise disable it
    //     for the Parameter involved
    /*switch(controlID)
    {
        case 0:
        {
            return true;    /// handled
        }

        default:
            return false;   /// not handled
    }*/

    return false;
}

/**
\brief has nothing to do with actual variable or updated variable (binding)

CAUTION:
- DO NOT update underlying variables here - this is only for sending GUI updates or letting you
  know that a parameter was changed; it should not change the state of your plugin.

WARNING:
- THIS IS NOT THE PREFERRED WAY TO LINK OR COMBINE CONTROLS TOGETHER. THE PROPER METHOD IS
  TO USE A CUSTOM SUB-CONTROLLER THAT IS PART OF THE GUI OBJECT AND CODE.
  SEE http://www.willpirkle.com for more information

\param controlID the control ID value of the parameter being updated
\param actualValue the new control value

\return true if operation succeeds, false otherwise
*/
bool PluginCore::guiParameterChanged(int32_t controlID, double actualValue)
{
	/*
	switch (controlID)
	{
		case controlID::<your control here>
		{

			return true; // handled
		}

		default:
			break;
	}*/

	return false; /// not handled
}

/**
\brief For Custom View and Custom Sub-Controller Operations

NOTES:
- this is for advanced users only to implement custom view and custom sub-controllers
- see the SDK for examples of use

\param messageInfo a structure containing information about the incoming message

\return true if operation succeeds, false otherwise
*/
bool PluginCore::processMessage(MessageInfo& messageInfo)
{
	// --- decode message
	switch (messageInfo.message)
	{
		// --- add customization appearance here
	case PLUGINGUI_DIDOPEN:
	{
		return false;
	}

	// --- NULL pointers so that we don't accidentally use them
	case PLUGINGUI_WILLCLOSE:
	{
		return false;
	}

	// --- update view; this will only be called if the GUI is actually open
	case PLUGINGUI_TIMERPING:
	{
		return false;
	}

	// --- register the custom view, grab the ICustomView interface
	case PLUGINGUI_REGISTER_CUSTOMVIEW:
	{

		return false;
	}

	case PLUGINGUI_REGISTER_SUBCONTROLLER:
	case PLUGINGUI_QUERY_HASUSERCUSTOM:
	case PLUGINGUI_USER_CUSTOMOPEN:
	case PLUGINGUI_USER_CUSTOMCLOSE:
	case PLUGINGUI_EXTERNAL_SET_NORMVALUE:
	case PLUGINGUI_EXTERNAL_SET_ACTUALVALUE:
	{

		return false;
	}

	default:
		break;
	}

	return false; /// not handled
}


/**
\brief process a MIDI event

NOTES:
- MIDI events are 100% sample accurate; this function will be called repeatedly for every MIDI message
- see the SDK for examples of use

\param event a structure containing the MIDI event data

\return true if operation succeeds, false otherwise
*/
bool PluginCore::processMIDIEvent(midiEvent& event)
{
	return true;
}

/**
\brief (for future use)

NOTES:
- MIDI events are 100% sample accurate; this function will be called repeatedly for every MIDI message
- see the SDK for examples of use

\param vectorJoysickData a structure containing joystick data

\return true if operation succeeds, false otherwise
*/
bool PluginCore::setVectorJoystickParameters(const VectorJoystickData& vectorJoysickData)
{
	return true;
}

/**
\brief use this method to add new presets to the list

NOTES:
- see the SDK for examples of use
- for non RackAFX users that have large paramter counts, there is a secret GUI control you
  can enable to write C++ code into text files, one per preset. See the SDK or http://www.willpirkle.com for details

\return true if operation succeeds, false otherwise
*/
bool PluginCore::initPluginPresets()
{
	// **--0xFF7A--**

	// --- Plugin Presets 
	int index = 0;
	PresetInfo* preset = nullptr;

	// --- Preset: Factory Preset
	preset = new PresetInfo(index++, "Factory Preset");
	initPresetParameters(preset->presetParameters);
	setPresetParameter(preset->presetParameters, controlID::delay_ms, 400.000000);
	setPresetParameter(preset->presetParameters, controlID::feedback1_pct, 0.000000);
	setPresetParameter(preset->presetParameters, controlID::wetVolume_db, -3.000000);
	setPresetParameter(preset->presetParameters, controlID::attack_ms, 10.000000);
	setPresetParameter(preset->presetParameters, controlID::release_ms, 50.000000);
	setPresetParameter(preset->presetParameters, controlID::threshold_db, -15.000000);
	setPresetParameter(preset->presetParameters, controlID::delayFX, -0.000000);
	setPresetParameter(preset->presetParameters, controlID::enableDelayFX, -0.000000);
	setPresetParameter(preset->presetParameters, controlID::fxRate, 1.000000);
	setPresetParameter(preset->presetParameters, controlID::fxDepth, 50.000000);
	setPresetParameter(preset->presetParameters, controlID::fxFeedback, 0.000000);
	setPresetParameter(preset->presetParameters, controlID::envAmount, 1.000000);
	setPresetParameter(preset->presetParameters, controlID::enableEnvelope, -0.000000);
	setPresetParameter(preset->presetParameters, controlID::enableSidechain, -0.000000);
	setPresetParameter(preset->presetParameters, controlID::enableLFO, -0.000000);
	setPresetParameter(preset->presetParameters, controlID::delay1_bpm, -0.000000);
	setPresetParameter(preset->presetParameters, controlID::envTarget, -0.000000);
	setPresetParameter(preset->presetParameters, controlID::sidechainTarget, -0.000000);
	setPresetParameter(preset->presetParameters, controlID::lfoTarget, -0.000000);
	setPresetParameter(preset->presetParameters, controlID::lfoRate_hz, 1.000000);
	setPresetParameter(preset->presetParameters, controlID::lfoAmount, 1.000000);
	setPresetParameter(preset->presetParameters, controlID::mode, 1.000000);
	setPresetParameter(preset->presetParameters, controlID::dryVolume_db, -3.000000);
	setPresetParameter(preset->presetParameters, controlID::enableBPM, -0.000000);
	setPresetParameter(preset->presetParameters, controlID::sidechainAmt, 1.000000);
	setPresetParameter(preset->presetParameters, controlID::sidechainThresh_db, -20.000000);
	addPreset(preset);


	// **--0xA7FF--**

    return true;
}

/**
\brief setup the plugin description strings, flags and codes; this is ordinarily done through the ASPiKreator or CMake

\return true if operation succeeds, false otherwise
*/
bool PluginCore::initPluginDescriptors()
{
    pluginDescriptor.pluginName = PluginCore::getPluginName();
    pluginDescriptor.shortPluginName = PluginCore::getShortPluginName();
    pluginDescriptor.vendorName = PluginCore::getVendorName();
    pluginDescriptor.pluginTypeCode = PluginCore::getPluginType();

	// --- describe the plugin attributes; set according to your needs
	pluginDescriptor.hasSidechain = kWantSidechain;
	pluginDescriptor.latencyInSamples = kLatencyInSamples;
	pluginDescriptor.tailTimeInMSec = kTailTimeMsec;
	pluginDescriptor.infiniteTailVST3 = kVSTInfiniteTail;

    // --- AAX
    apiSpecificInfo.aaxManufacturerID = kManufacturerID;
    apiSpecificInfo.aaxProductID = kAAXProductID;
    apiSpecificInfo.aaxBundleID = kAAXBundleID;  /* MacOS only: this MUST match the bundle identifier in your info.plist file */
    apiSpecificInfo.aaxEffectID = "aaxDeveloper.";
    apiSpecificInfo.aaxEffectID.append(PluginCore::getPluginName());
    apiSpecificInfo.aaxPluginCategoryCode = kAAXCategory;

    // --- AU
    apiSpecificInfo.auBundleID = kAUBundleID;   /* MacOS only: this MUST match the bundle identifier in your info.plist file */

    // --- VST3
    apiSpecificInfo.vst3FUID = PluginCore::getVSTFUID(); // OLE string format
    apiSpecificInfo.vst3BundleID = kVST3BundleID;/* MacOS only: this MUST match the bundle identifier in your info.plist file */
	apiSpecificInfo.enableVST3SampleAccurateAutomation = kVSTSAA;
	apiSpecificInfo.vst3SampleAccurateGranularity = kVST3SAAGranularity;

    // --- AU and AAX
    apiSpecificInfo.fourCharCode = PluginCore::getFourCharCode();

    return true;
}

// --- static functions required for VST3/AU only --------------------------------------------- //
const char* PluginCore::getPluginBundleName() { return kAUBundleName; }
const char* PluginCore::getPluginName(){ return kPluginName; }
const char* PluginCore::getShortPluginName(){ return kShortPluginName; }
const char* PluginCore::getVendorName(){ return kVendorName; }
const char* PluginCore::getVendorURL(){ return kVendorURL; }
const char* PluginCore::getVendorEmail(){ return kVendorEmail; }
const char* PluginCore::getAUCocoaViewFactoryName(){ return AU_COCOA_VIEWFACTORY_STRING; }
pluginType PluginCore::getPluginType(){ return kPluginType; }
const char* PluginCore::getVSTFUID(){ return kVSTFUID; }
int32_t PluginCore::getFourCharCode(){ return kFourCharCode; }

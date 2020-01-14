// -----------------------------------------------------------------------------
//    ASPiK Plugin Kernel File:  plugincore.h
//
/**
    \file   plugincore.h
    \author Will Pirkle
    \date   17-September-2018
    \brief  base class interface file for ASPiK plugincore object
    		- http://www.aspikplugins.com
    		- http://www.willpirkle.com
*/
// -----------------------------------------------------------------------------
#ifndef __pluginCore_h__
#define __pluginCore_h__

#include "pluginbase.h"
#include "RE201.h"

// **--0x7F1F--**

// --- Plugin Variables controlID Enumeration 

enum controlID {
	delay_ms = 1,
	feedback1_pct = 2,
	wetVolume_db = 3,
	attack_ms = 51,
	release_ms = 52,
	threshold_db = 53,
	delayFX = 5,
	enableDelayFX = 15,
	fxRate = 6,
	fxDepth = 16,
	fxFeedback = 26,
	envAmount = 55,
	enableEnvelope = 54,
	enableSidechain = 64,
	enableLFO = 74,
	delay1_bpm = 0,
	envTarget = 56,
	sidechainTarget = 66,
	lfoTarget = 76,
	lfoRate_hz = 73,
	lfoAmount = 75,
	mode = 12,
	dryVolume_db = 4,
	enableBPM = 10,
	sidechainAmt = 65,
	sidechainThresh_db = 63
};

	// **--0x0F1F--**

/**
\class PluginCore
\ingroup ASPiK-Core
\brief
The PluginCore object is the default PluginBase derived object for ASPiK projects.
Note that you are fre to change the name of this object (as long as you change it in the compiler settings, etc...)


PluginCore Operations:
- overrides the main processing functions from the base class
- performs reset operation on sub-modules
- processes audio
- processes messages for custom views
- performs pre and post processing functions on parameters and audio (if needed)

\author Will Pirkle http://www.willpirkle.com
\remark This object is included in Designing Audio Effects Plugins in C++ 2nd Ed. by Will Pirkle
\version Revision : 1.0
\date Date : 2018 / 09 / 7
*/
class PluginCore : public PluginBase
{
public:
    PluginCore();

	/** Destructor: empty in default version */
    virtual ~PluginCore(){}

	// --- PluginBase Overrides ---
	//
	/** this is the creation function for all plugin parameters */
	bool initPluginParameters();

	/** called when plugin is loaded, a new audio file is playing or sample rate changes */
	virtual bool reset(ResetInfo& resetInfo);

	/** one-time post creation init function; pluginInfo contains path to this plugin */
	virtual bool initialize(PluginInfo& _pluginInfo);

	// --- preProcess: sync GUI parameters here; override if you don't want to use automatic variable-binding
	virtual bool preProcessAudioBuffers(ProcessBufferInfo& processInfo);

	/** process frames of data */
	virtual bool processAudioFrame(ProcessFrameInfo& processFrameInfo);

	// --- uncomment and override this for buffer processing; see base class implementation for
	//     help on breaking up buffers and getting info from processBufferInfo
	//virtual bool processAudioBuffers(ProcessBufferInfo& processBufferInfo);

	/** preProcess: do any post-buffer processing required; default operation is to send metering data to GUI  */
	virtual bool postProcessAudioBuffers(ProcessBufferInfo& processInfo);

	/** called by host plugin at top of buffer proccess; this alters parameters prior to variable binding operation  */
	virtual bool updatePluginParameter(int32_t controlID, double controlValue, ParameterUpdateInfo& paramInfo);

	/** called by host plugin at top of buffer proccess; this alters parameters prior to variable binding operation  */
	virtual bool updatePluginParameterNormalized(int32_t controlID, double normalizedValue, ParameterUpdateInfo& paramInfo);

	/** this can be called: 1) after bound variable has been updated or 2) after smoothing occurs  */
	virtual bool postUpdatePluginParameter(int32_t controlID, double controlValue, ParameterUpdateInfo& paramInfo);

	/** this is ony called when the user makes a GUI control change */
	virtual bool guiParameterChanged(int32_t controlID, double actualValue);

	/** processMessage: messaging system; currently used for custom/special GUI operations */
	virtual bool processMessage(MessageInfo& messageInfo);

	/** processMIDIEvent: MIDI event processing */
	virtual bool processMIDIEvent(midiEvent& event);

	/** specialized joystick servicing (currently not used) */
	virtual bool setVectorJoystickParameters(const VectorJoystickData& vectorJoysickData);

	/** create the presets */
	bool initPluginPresets();

	// --- BEGIN USER VARIABLES AND FUNCTIONS -------------------------------------- //
	//	   Add your variables and methods here
	RE201 monoDelay;
	ModulatedDelay modDelay;
	void updateParameters();


	// --- END USER VARIABLES AND FUNCTIONS -------------------------------------- //

private:
	//  **--0x07FD--**

	// --- Continuous Plugin Variables 
	double delay_ms = 0.0;
	double feedback1_pct = 0.0;
	double wetVolume_db = 0.0;
	double attack_ms = 0.0;
	double release_ms = 0.0;
	double threshold_db = 0.0;
	double fxRate = 0.0;
	double fxDepth = 0.0;
	double fxFeedback = 0.0;
	double envAmount = 0.0;
	double lfoRate_hz = 0.0;
	double lfoAmount = 0.0;
	int mode = 0;
	double dryVolume_db = 0.0;
	double sidechainAmt = 0.0;
	double sidechainThresh_db = 0.0;

	// --- Discrete Plugin Variables 
	int delayFX = 0;
	enum class delayFXEnum { Flanger,Chorus,Vibrato };	// to compare: if(compareEnumToInt(delayFXEnum::Flanger, delayFX)) etc... 

	int enableDelayFX = 0;
	enum class enableDelayFXEnum { SWITCH_OFF,SWITCH_ON };	// to compare: if(compareEnumToInt(enableDelayFXEnum::SWITCH_OFF, enableDelayFX)) etc... 

	int enableEnvelope = 0;
	enum class enableEnvelopeEnum { SWITCH_OFF,SWITCH_ON };	// to compare: if(compareEnumToInt(enableEnvelopeEnum::SWITCH_OFF, enableEnvelope)) etc... 

	int enableSidechain = 0;
	enum class enableSidechainEnum { SWITCH_OFF,SWITCH_ON };	// to compare: if(compareEnumToInt(enableSidechainEnum::SWITCH_OFF, enableSidechain)) etc... 

	int enableLFO = 0;
	enum class enableLFOEnum { SWITCH_OFF,SWITCH_ON };	// to compare: if(compareEnumToInt(enableLFOEnum::SWITCH_OFF, enableLFO)) etc... 

	int delay1_bpm = 0;
	enum class delay1_bpmEnum { Half,Quarter,Quarter_Triplet,Eighth,Eighth_Triplet,Sixteenth };	// to compare: if(compareEnumToInt(delay1_bpmEnum::Half, delay1_bpm)) etc... 

	int envTarget = 0;
	enum class envTargetEnum { Delay,FX };	// to compare: if(compareEnumToInt(envTargetEnum::Delay, envTarget)) etc... 

	int sidechainTarget = 0;
	enum class sidechainTargetEnum { Delay,FX };	// to compare: if(compareEnumToInt(sidechainTargetEnum::Delay, sidechainTarget)) etc... 

	int lfoTarget = 0;
	enum class lfoTargetEnum { Delay,FX };	// to compare: if(compareEnumToInt(lfoTargetEnum::Delay, lfoTarget)) etc... 

	int enableBPM = 0;
	enum class enableBPMEnum { SWITCH_OFF,SWITCH_ON };	// to compare: if(compareEnumToInt(enableBPMEnum::SWITCH_OFF, enableBPM)) etc... 

	// **--0x1A7F--**
    // --- end member variables

public:
    /** static description: bundle folder name

	\return bundle folder name as a const char*
	*/
    static const char* getPluginBundleName();

    /** static description: name

	\return name as a const char*
	*/
    static const char* getPluginName();

	/** static description: short name

	\return short name as a const char*
	*/
	static const char* getShortPluginName();

	/** static description: vendor name

	\return vendor name as a const char*
	*/
	static const char* getVendorName();

	/** static description: URL

	\return URL as a const char*
	*/
	static const char* getVendorURL();

	/** static description: email

	\return email address as a const char*
	*/
	static const char* getVendorEmail();

	/** static description: Cocoa View Factory Name

	\return Cocoa View Factory Name as a const char*
	*/
	static const char* getAUCocoaViewFactoryName();

	/** static description: plugin type

	\return type (FX or Synth)
	*/
	static pluginType getPluginType();

	/** static description: VST3 GUID

	\return VST3 GUID as a const char*
	*/
	static const char* getVSTFUID();

	/** static description: 4-char code

	\return 4-char code as int
	*/
	static int32_t getFourCharCode();

	/** initalizer */
	bool initPluginDescriptors();

};




#endif /* defined(__pluginCore_h__) */

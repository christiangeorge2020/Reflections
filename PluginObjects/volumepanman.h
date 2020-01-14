#pragma once

#ifndef __VolumePanMan__
#define __VolumePanMan__

#include "fxobjects.h"
#include "pluginstructures.h"
#include "superlfo.h"

/**
\struct VolumePanManParameters
\ingroup FX-Objects
\brief
Custom parameter structure for the VolumePanMan object.

\author <Your Name> <http://www.yourwebsite.com>
\remark <Put any remarks or notes here>
\version Revision : 1.0
\date Date : 2019 / 01 / 31
*/

enum class channelSelection { kStereo, kLeft, kRight };
enum class panRateBPM {kQuarter, kEighth, kSixteenth, kQuarter_Triplet, kEighth_Triplet};
enum class waveformSelection {kTriangle, kSin, kSaw, kSquare};

struct VolumePanManParameters
{
	VolumePanManParameters() {}

	/** all FXObjects parameter objects require overloaded= operator so remember to add new entries if you add new variables. */
	VolumePanManParameters& operator=(const VolumePanManParameters& params)	// need this override for collections to work
	{
		// --- it is possible to try to make the object equal to itself
		//     e.g. thisObject = thisObject; so this code catches that
		//     trivial case and just returns this object
		if (this == &params)
			return *this;

		// --- copy from params (argument) INTO our variables
		volume_dB = params.volume_dB;
		panValue = params.panValue;
		enableMute = params.enableMute;
		inputMeter = params.inputMeter;
		outputMeter = params.outputMeter;
		channelSwitch = params.channelSwitch;
		enableMSDecoder = params.enableMSDecoder;
		
		// autopan variables
		enableAutoPan = params.enableAutoPan;
		panRate_Hz = params.panRate_Hz;
		panDepth_Pct = params.panDepth_Pct;
		panRateSelect = params.panRateSelect;
		enableRate_BPM = params.enableRate_BPM;
		waveSelect = params.waveSelect;
		invertWaveform = params.invertWaveform;
		autopanMix = params.autopanMix;
		panDouble = params.panDouble;

		// --- MUST be last
		return *this;
	}

	// --- individual parameters
	double volume_dB = 0.0; ///< init 0dB = unity gain
	double panValue = 0.0; ///< 0 = center, -1 = left, +1 = right
	bool enableMute = false; ///< default is OFF so audio flows
	float outputMeter = 0.f; ///< example of output variable
	float inputMeter = 0.f;
	bool enableMSDecoder = false;

	// autopan parameters
	bool enableAutoPan = false;
	double panRate_Hz = 10.0;
	double panDepth_Pct = 50.0;
	bool enableRate_BPM = false;
	bool invertWaveform = false;
	double autopanMix = 100.0;
	bool panDouble = false;
							 
	channelSelection channelSwitch = channelSelection::kStereo;///< init
	panRateBPM panRateSelect = panRateBPM::kQuarter;
	waveformSelection waveSelect = waveformSelection::kSin;
};

/**
\class VolumePanMan
\ingroup FX-Objects
\brief
The VolumePanMan object implements ....

Audio I/O:
- Processes mono input to mono output.
- *** Optionally, process frame *** Modify this according to your object functionality

Control I/F:
- Use VolumePanManParameters structure to get/set object params.

\author <Your Name> <http://www.yourwebsite.com>
\remark <Put any remarks or notes here>
\version Revision : 1.0
\date Date : 2019 / 01 / 31
*/
class VolumePanMan : public IAudioSignalProcessor
{
public:
	VolumePanMan(void) {}	/* C-TOR */
	~VolumePanMan(void) {}	/* D-TOR */

public:
	/** reset members to initialized state */
	virtual bool reset(double _sampleRate)
	{
		// --- store the sample rate
		sampleRate = _sampleRate;

		// --- do any other per-audio-run inits here

		// initialize autopan
		panLFO.reset(sampleRate);

		//
		SuperLFOParameters params = panLFO.getParameters();
		params.waveform = LFOWaveform::kSin;
		panLFO.setParameters(params);

		return true;
	}

	/** process MONO input */
	/**
	\param xn input
	\return the processed sample
	*/
	virtual double processAudioSample(double xn)
	{
		// --- the output variable
		double yn = 0.0;

		

		// --- do your DSP magic here to create yn
		double gain = volumeCooked;

		// --- mute branch
		if (parameters.enableMute)
			gain = 0.0;

		// --- the DSP
		yn = gain * xn;

		// --- set meter
		parameters.inputMeter = xn;
		parameters.outputMeter = yn;

		// --- done
		return yn;
	}

	/** query to see if this object can process frames */
	virtual bool canProcessAudioFrame() { return true; } // <-- change this!

	/** process audio frame: implement this function if you answer "true" to above query */
	virtual bool processAudioFrame(const float* inputFrame,	/* ptr to one frame of data: pInputFrame[0] = left, pInputFrame[1] = right, etc...*/
		float* outputFrame,
		uint32_t inputChannels,
		uint32_t outputChannels,
		HostInfo* hostInfo)
	{

		// --- mono/mono; use existing function and done
		if (inputChannels == 1 && outputChannels == 1)
		{
			outputFrame[0] = processAudioSample(inputFrame[0]);
			return true; // --- processed!
		}

		// --- one of a zillion ways to do this...
		double gain_L = volumeCooked;
		double gain_R = volumeCooked;

		// --- branching
		if (parameters.channelSwitch == channelSelection::kLeft) {
			gain_R = 0.0;
		}
		else if (parameters.channelSwitch == channelSelection::kRight) {
			gain_L = 0.0;
		}
		// else if we get here we know the user has a stereo selection;
		// --- now check mute as an override:
		if (parameters.enableMute)
		{
			gain_L = 0.0;
			gain_R = 0.0;
		}

		// --- take bpm from hostInfo pointer
		double bPM = hostInfo->dBPM;

		// --- generate pan modifier variable
		double panCenter = parameters.panValue;
		

		// --- check enable autopan
		if (parameters.enableAutoPan)
		{
			// --- determine pan rate type, bpm vs hz
			if (parameters.enableRate_BPM)
			{
				double bps = (hostInfo->dBPM) / 60;

				// generate new lfo param to update panLFO with bpm information
				SuperLFOParameters setRateBPM = panLFO.getParameters();

				// --- quarter note selection
				if (parameters.panRateSelect == panRateBPM::kQuarter)
					setRateBPM.frequency_Hz = bps / 4;

				// --- eighth note selection
				else if (parameters.panRateSelect == panRateBPM::kEighth)
					setRateBPM.frequency_Hz = bps / 2;

				// --- sixteenth note selection
				else if (parameters.panRateSelect == panRateBPM::kSixteenth)
					setRateBPM.frequency_Hz = bps;

				// --- quarter triplets note selection
				else if (parameters.panRateSelect == panRateBPM::kQuarter_Triplet)
					setRateBPM.frequency_Hz = bps * 3 / 2;

				// --- eighth triplets note selection
				else if (parameters.panRateSelect == panRateBPM::kEighth_Triplet)
					setRateBPM.frequency_Hz = bps * 3;

				panLFO.setParameters(setRateBPM);
			}

			// generate panmod lfo if enabled
			SignalModulatorOutput panLFOOutput = panLFO.renderModulatorOutput();
			double panModifier = 0;

			// check waveform inversion
			if (parameters.invertWaveform) 
			{
				panModifier = doBipolarModulation(panLFOOutput.invertedOutput, -1.5, 1.5);
			}
			else
			{
				panModifier = doBipolarModulation(panLFOOutput.normalOutput, -1.5, 1.5);
			}

			panCenter = panCenter + panModifier;

			// --- keep modulated panning in bounds of -1.5 to 1.5
			if (panCenter > 1.5)
				panCenter = 1.5;
			else if (panCenter < -1.5)
				panCenter = -1.5;
		}
		

		double panCenterDry = parameters.panValue;
		double panMixScaled = (parameters.autopanMix - 50) / 50;

		// --- generate constant power panning
		double wetPanValue_L = cos((panCenter + 1.0) * (kPi / 4.0));
		double wetPanValue_R = sin((panCenter + 1.0) * (kPi / 4.0));
		double dryPanValue_L = cos((panCenterDry + 1.0) * (kPi / 4.0));
		double dryPanValue_R = sin((panCenterDry + 1.0) * (kPi / 4.0));

		double input_L = inputFrame[0];
		double input_R = inputFrame[1];
	
		// --- mono/stereo: pan left (mono) input channel to
		// left + right outputs
		if (inputChannels == 1 && outputChannels == 2)
		{
			// --- generate dry wet mixes
			double wetSignal_L = gain_L * wetPanValue_L * input_L;
			double wetSignal_R = gain_R * wetPanValue_R * input_L;

			double drySignal_L = gain_L * dryPanValue_L * input_L;
			double drySignal_R = gain_R * dryPanValue_R * input_L;

			double panMixDry = cos((panMixScaled + 1.0) * (kPi / 4.0));
			double panMixWet = sin((panMixScaled + 1.0) * (kPi / 4.0));

			// --- scale dry wet mixes by autopan mix variable
			double signal_L = panMixDry * drySignal_L + panMixWet * wetSignal_L;
			double signal_R = panMixDry * drySignal_R + panMixWet * wetSignal_R;

			outputFrame[0] = signal_L;
			outputFrame[1] = signal_R;

			if (parameters.panDouble)
			{
				outputFrame[0] = -outputFrame[1];
				parameters.outputMeter = outputFrame[1];
			} 
			else
			{
				parameters.outputMeter = 0.5 * outputFrame[0] +
					0.5 * outputFrame[1];
			}
			// --- outbound variables
			parameters.inputMeter = inputFrame[0];
			
			return true; // --- processed!
		}

		// --- stereo/stereo: pan is actually "balance" control
		else if (inputChannels == 2 && outputChannels == 2)
		{
			// --- check for ms decoder
			if (parameters.enableMSDecoder)
			{
				input_L = inputFrame[0] + inputFrame[1];
				input_R = inputFrame[0] - inputFrame[1];
			}

			// --- generate dry wet mixes
			double wetSignal_L = gain_L * wetPanValue_L * input_L;
			double wetSignal_R = gain_R * wetPanValue_R * input_R;


			double drySignal_L = gain_L * dryPanValue_L * input_L;
			double drySignal_R = gain_R * dryPanValue_R * input_R;

			double panMixDry = cos((panMixScaled + 1.0) * (kPi / 4.0));
			double panMixWet = sin((panMixScaled + 1.0) * (kPi / 4.0));

			// --- scale dry wet mixes by autopan mix variable
			double signal_L = panMixDry * drySignal_L + panMixWet * wetSignal_L;
			double signal_R = panMixDry * drySignal_R + panMixWet * wetSignal_R;

			outputFrame[0] = signal_L;
			outputFrame[1] = signal_R;

			if (parameters.panDouble)
			{
				outputFrame[0] = -outputFrame[1];
				parameters.outputMeter = outputFrame[1];

			}
			else
			{
				parameters.outputMeter = 0.5 * outputFrame[0] +
					0.5 * outputFrame[1];
			}

			parameters.inputMeter = 0.5 * inputFrame[0] +
				0.5 * inputFrame[1];

			return true; // --- processed!
		}

		return false; // NOT handled
	}


	/** get parameters: note use of custom structure for passing param data */
	/**
	\return VolumePanManParameters custom data structure
	*/
	VolumePanManParameters getParameters()
	{
		return parameters;
	}

	/** set parameters: note use of custom structure for passing param data */
	/**
	\param VolumePanManParameters custom data structure
	*/
	void setParameters(const VolumePanManParameters& params)
	{
		// --- copy them; note you may choose to ignore certain items
		//     and copy the variables one at a time, or you may test
		//     to see if cook-able variables have changed; if not, then
		//     do not re-cook them as it just wastes CPU

		// --- cook parameters here

		// Update volume_dB; check first due to cooking
		if (parameters.volume_dB != params.volume_dB) 
		{
			parameters.volume_dB = params.volume_dB;

			// --- if dB = -60, then we shut off completely
			if (parameters.volume_dB == -60.0) 
			{
				volumeCooked = 0.0;
			}
			else 
			{
				volumeCooked = pow(10.0, parameters.volume_dB / 20.0);
			}
		}

		// --- update panValue
		parameters.panValue = params.panValue;

		// --- update enableMute
		parameters.enableMute = params.enableMute;

		// --- update channelSwitch
		parameters.channelSwitch = params.channelSwitch;

		// --- update msdecoder
		parameters.enableMSDecoder = params.enableMSDecoder;

		// --- update autopan
		parameters.enableAutoPan = params.enableAutoPan;

		// --- pan double
		parameters.panDouble = params.panDouble;

		// --- generate panLFO parameters
		SuperLFOParameters panLFOParams = panLFO.getParameters();

		// --- switch between hertz and bpm
		parameters.enableRate_BPM = params.enableRate_BPM;

		// --- bpm division
		parameters.panRateSelect = params.panRateSelect;

		//  --- update hertz rate
		parameters.panRate_Hz = params.panRate_Hz;

		// --- update lfo depth
		parameters.panDepth_Pct = params.panDepth_Pct;

		panLFOParams.outputAmplitude = parameters.panDepth_Pct / 100;

		parameters.autopanMix = params.autopanMix;
		
		// --- invert lfo waveform
		parameters.invertWaveform = params.invertWaveform;

		// --- set lfo params frequency
		panLFOParams.frequency_Hz = parameters.panRate_Hz;

		// --- set lfo waveform
		parameters.waveSelect = params.waveSelect;
		panLFOParams.waveform = convertIntToEnum(parameters.waveSelect, LFOWaveform);

		// --- set lfo params
		panLFO.setParameters(panLFOParams);


	}

private:
	VolumePanManParameters parameters; ///< object parameters

	// --- the modulator
	SuperLFO panLFO;

	// --- local variables used by this object
	double sampleRate = 0.0;	///< sample rate
	double volumeCooked = 1.0; ///< unity gain
	
};

#endif
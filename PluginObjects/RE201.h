#pragma once

#ifndef __RE201__
#define __RE201__

#include "fxobjects.h"
#include "superlfo.h"

/**
\struct RE201Parameters
\ingroup FX-Objects
\brief
Custom parameter structure for the RE201 object.

\author <Your Name> <http://www.yourwebsite.com>
\remark <Put any remarks or notes here>
\version Revision : 1.0
\date Date : 2019 / 01 / 31
*/
enum class rateBPM{ kHalf, kQuarter, kQuarter_Triplet, kEighth, kEighth_Triplet, kSixteenth };

struct RE201Parameters
{
	RE201Parameters() {}

	/** all FXObjects parameter objects require overloaded= operator so remember to add new entries if you add new variables. */
	RE201Parameters& operator=(const RE201Parameters& params)	// need this override for collections to work
	{
		// --- it is possible to try to make the object equal to itself
		//     e.g. thisObject = thisObject; so this code catches that
		//     trivial case and just returns this object
		if (this == &params)
			return *this;

		dryVolume = params.dryVolume;
		wetVolume = params.wetVolume;

		// --- delay
		delay_ms = params.delay_ms;
		feedback_pct = params.feedback_pct;

		// --- bpm
		delay_bpm = params.delay_bpm;
		enableBPM = params.enableBPM;


		// ** MODULATION **

		// --- detector params
		threshold_db = params.threshold_db;
		enableEnv = params.enableEnv;
		envAmount = params.envAmount;

		// --- sidechain
		enableSidechain = params.enableSidechain;
		sidechainAmount = params.sidechainAmount;
		sidechainThresh_db = params.sidechainThresh_db;

		// --- lfo
		enableLFO = params.enableLFO;
		lfoAmount = params.lfoAmount;
		lfoRate_hz = params.lfoRate_hz;

		// --- mode
		mode = params.mode;

		// --- MUST be last
		return *this;
	}

	double dryVolume = 0.0;
	double wetVolume = 0.0;

	// --- delay
	double delay_ms = 0.0;
	double feedback_pct = 0.0;

	// --- bpm
	rateBPM delay_bpm = { rateBPM::kEighth };
	bool enableBPM = false;


	// ** MODULATION **

	// --- detector parameters
	double threshold_db = 0.0;
	bool enableEnv = false;
	double envAmount = 1;

	// --- sidechain parameters
	bool enableSidechain = false;
	double sidechainAmount = 0.0;
	double sidechainThresh_db = 0.0;

	// --- lfo parameters
	bool enableLFO = false;
	double lfoAmount = 0.0;
	double lfoRate_hz = 0.0;

	// --- mode
	int mode = 9;

};


/**
\class RE201
\ingroup FX-Objects
\brief
The RE201 object implements ....

Audio I/O:
- Processes mono input to mono output.
- *** Optionally, process frame *** Modify this according to your object functionality

Control I/F:
- Use RE201Parameters structure to get/set object params.

\author <Your Name> <http://www.yourwebsite.com>
\remark <Put any remarks or notes here>
\version Revision : 1.0
\date Date : 2019 / 01 / 31
*/
class RE201 : public IAudioSignalProcessor
{
public:
	RE201(void) {}	/* C-TOR */
	~RE201(void) {}	/* D-TOR */

public:
	/** reset members to initialized state */
	virtual bool reset(double _sampleRate)
	{
		// --- if sample rate did not change
		if (sampleRate == _sampleRate)
		{
			delayBuffer[0].flushBuffer();
			delayBuffer[1].flushBuffer();
		}
		else
			createDelayBuffers(_sampleRate, bufferLength_mSec);

		// ** MODULATION **
		// --- detector
		detector.reset(_sampleRate);
		AudioDetectorParameters detectorParams = detector.getParameters();
		detectorParams.clampToUnityMax = false;
		detectorParams.detect_dB = true;
		detectorParams.attackTime_mSec = 10.0;
		detectorParams.releaseTime_mSec = 50.0;
		detector.setParameters(detectorParams);

		// --- lfo
		modLFO.reset(_sampleRate);
		
		return true;
	}

	/** process the sidechain by saving the value for the upcoming processAudioSample() call */
	virtual double processAuxInputAudioSample(double xnL, double xnR)
	{
		sidechainInputSample[0] = xnL;
		sidechainInputSample[1] = xnR;
		return 0.0;
	}

	//** get bpm from the hostinfo pointer
	virtual void getHostBPM(HostInfo* hostInfo)
	{
		bpm = hostInfo->dBPM;
	}

	/** process MONO input */
	/**
	\param xn input
	\return the processed sample
	*/
	virtual double processAudioSample(double xn)
	{
		return xn;
	}

	/** query to see if this object can process frames */
	virtual bool canProcessAudioFrame() { return true; } // <-- change this!

	/** process audio frame: implement this function if you answer "true" to above query */
	virtual bool processAudioFrame(const float* inputFrame,	/* ptr to one frame of data: pInputFrame[0] = left, pInputFrame[1] = right, etc...*/
					     float* outputFrame,
					     uint32_t inputChannels,
					     uint32_t outputChannels)
	{
		// --- read delay
		double ynL[4];
		double ynR[4];

		double detectorReduction[2] = { 1.0, 1.0 };
		

		// ** DETECTOR **
		if (parameters.enableEnv)
		{
			double detect_db[2];
			double output_db[2];
			double gainReduction[2];

			for (int i = 0; i < 2; i++)
			{
				detect_db[i] = detector.processAudioSample(inputFrame[i]);

				if (detect_db[i] > parameters.threshold_db)
				{
					output_db[i] = parameters.threshold_db + 
									(detect_db[i] - parameters.threshold_db) / parameters.envAmount;

					gainReduction[i] = output_db[i] - detect_db[i];
					detectorReduction[i] = pow(10, (gainReduction[i]) / 20);
				}
			}
				
		}

		// ** LFO **
		double lfoModifier = 1.0;

		if (parameters.enableLFO)
		{
			SignalModulatorOutput lfoOutput = modLFO.renderModulatorOutput();

			double uniModulator = bipolarToUnipolar(lfoOutput.normalOutput);
			lfoModifier = doUnipolarModulationFromMax(uniModulator, 0.0, 1.0);
		}


		// ** SIDECHAIN **
		double sidechainModulator[2] = { 1.0, 1.0 };

		if (parameters.enableSidechain)
		{
			double detect_db[2];
			double output_db[2];
			double gainReduction[2];

			for (int i = 0; i < 2; i++)
			{
				detect_db[i] = detector.processAudioSample(inputFrame[i]);

				if (detect_db[i] > parameters.sidechainThresh_db)
				{
					output_db[i] = parameters.sidechainThresh_db +
						(detect_db[i] - parameters.sidechainThresh_db) / parameters.envAmount;

					gainReduction[i] = output_db[i] - detect_db[i];
					detectorReduction[i] = pow(10, (gainReduction[i]) / 20);
				}
			}
		}

		// ** OUTPUT **
		for (int i = 0; i < 4; i++)
		{
			ynL[i] = delayBuffer[0].readBuffer(delayInSamples[i]);
			ynR[i] = delayBuffer[1].readBuffer(delayInSamples[i]);
		}

		// --- create input for delay buffer
		double dn[2];

		for (int i = 0; i < 2; i++)
			dn[i] = inputFrame[i];


		// --- mode
		dn[0] += feedback_cooked * ynL[0];
		dn[1] += feedback_cooked * ynR[0];

		// --- write to delay buffer
		for (int i = 0; i < 2; i++)
			delayBuffer[i].writeBuffer(dn[i]);



		// ** LEFT **
		// --- pan modifier
		double panModifier[4];

		for (int i = 0; i < 4; i++)
			panModifier[i] = cos((panCenter[i] + 1.0) * (kPi / 4.0));

		// --- incorporate panning
		for (int i = 0; i < 4; i++)
			ynL[i] *= panModifier[i];


		// ** RIGHT **
		// --- pan modifier
		for (int i = 0; i < 4; i++)
			panModifier[i] = sin((panCenter[i] + 1.0) * (kPi / 4.0));

		// --- incorporate panning
		for (int i = 0; i < 4; i++)
		{
			ynR[i] *= panModifier[i];
		}
			



		// --- form mixture out = dry*xn + wet*yn
		for (int i = 0; i < 4; i++)
		{
			ynL[i] *= lfoModifier * detectorReduction[0] * wetMix[i] * sidechainModulator[0];
			ynR[i] *= lfoModifier * detectorReduction[1] * wetMix[i] * sidechainModulator[1];
		}

		double outputL = inputFrame[0] * dryMix;
		double outputR = inputFrame[1] * dryMix;
		
		for (int i = 0; i < 4; i++)
		{
			outputL += ynL[i];
			outputR += ynR[i];
		}
			
		
		if (inputChannels == 1 && outputChannels == 1)
		{
			outputFrame[0] = outputL;
			return true;
		}
		else if (inputChannels == 1 && outputChannels == 2)
		{

			outputFrame[0] = outputL;
			outputFrame[1] = outputL;
			return true;
		}
		else if (inputChannels == 2 && outputChannels == 2)
		{
			outputFrame[0] = outputL;
			outputFrame[1] = outputR;
			return true;
		}
	}

	/** creation function */
	void createDelayBuffers(double _sampleRate, double _bufferLength_mSec)
	{
		// --- store for math
		bufferLength_mSec = _bufferLength_mSec;
		sampleRate = _sampleRate;
		samplesPerMSec = sampleRate / 1000.0;

		// --- total buffer length including fractional part
		bufferLength = (unsigned int)(bufferLength_mSec*(samplesPerMSec)) + 1; // +1 for fractional part

																			   // --- create new buffer
		delayBuffer[0].createCircularBuffer(bufferLength);
		delayBuffer[1].createCircularBuffer(bufferLength);
	}


	/** get parameters: note use of custom structure for passing param data */
	/**
	\return RE201Parameters custom data structure
	*/
	RE201Parameters getParameters()
	{
		return parameters;
	}

	/** set parameters: note use of custom structure for passing param data */
	/**
	\param RE201Parameters custom data structure
	*/
	void setParameters(const RE201Parameters& params)
	{
		// --- copy them; note you may choose to ignore certain items
		//     and copy the variables one at a time, or you may test
		//     to see if cook-able variables have changed; if not, then
		//     do not re-cook them as it just wastes CPU

		// --- delay parameters

		if (true || parameters.delay_ms != params.delay_ms
			|| parameters.enableBPM != params.enableBPM
			|| parameters.delay_bpm != params.delay_bpm)
		{
			parameters.delay_ms = params.delay_ms;
			parameters.enableBPM = params.enableBPM;
			parameters.delay_bpm = params.delay_bpm;

			if (!parameters.enableBPM)
				delayInSamples[0] = parameters.delay_ms * samplesPerMSec;
			else
			{
				double sPB = 60 / (bpm);
				double bpmDelay = sPB * sampleRate;

				if (parameters.delay_bpm == rateBPM::kHalf)
					delayInSamples[0] = 2 * bpmDelay;
				else if (parameters.delay_bpm == rateBPM::kQuarter)
					delayInSamples[0] = bpmDelay;
				else if (parameters.delay_bpm == rateBPM::kQuarter_Triplet)
					delayInSamples[0] = 3 / 2 * bpmDelay;
				else if (parameters.delay_bpm == rateBPM::kEighth)
					delayInSamples[0] = bpmDelay / 2;
				else if (parameters.delay_bpm == rateBPM::kEighth_Triplet)
					delayInSamples[0] = bpmDelay * 3 / 4;
				else if (parameters.delay_bpm == rateBPM::kSixteenth)
					delayInSamples[0] = bpmDelay / 4;
			}

			delayInSamples[1] = delayInSamples[0] / 2;
			delayInSamples[2] = delayInSamples[0] * 3;
			delayInSamples[3] = delayInSamples[0] * 2;
			
		}

		// --- dry signal
		if (parameters.dryVolume != params.dryVolume) {
			parameters.dryVolume = params.dryVolume;

			dryMix = pow(10, parameters.dryVolume / 20);
		}

		/*if (parameters.wetVolume != params.wetVolume ||
			parameters.mode != params.mode)
		{
			parameters.wetVolume = params.wetVolume;
			for (int i = 0; i < 4; i++)
				wetMix[i] = pow(10, parameters.wetVolume / 20);
		}*/

		// ** MODE **
		if (parameters.mode != params.mode ||
			parameters.wetVolume != params.wetVolume)
		{
			parameters.wetVolume = params.wetVolume;
			for (int i = 0; i < 4; i++)
				wetMix[i] = pow(10, parameters.wetVolume / 20);

			parameters.mode = params.mode;

			if (parameters.mode == 1)
			{
				for (int i = 1; i < 4; i++)
					wetMix[i] = 0.0;
			}
			else if (parameters.mode == 2)
			{
				wetMix[0] *= 0.5;
				wetMix[1] *= 0.5;
				wetMix[2] = 0.0;
				wetMix[3] = 0.0;
				panCenter[1] = 1.0;
			}
			else if (parameters.mode == 3)
			{ 
				wetMix[0] *= 0.5;
				wetMix[1] = 0.0;
				wetMix[2] *= 0.5;
				wetMix[3] = 0.0;
				panCenter[2] = -1.0;
			}
			else if (parameters.mode == 4)
			{
				wetMix[0] *= 0.5;
				wetMix[1] = 0.0;
				wetMix[2] = 0.0;
				wetMix[3] *= 0.5;
				panCenter[3] = 0.7;
			}
			else if (parameters.mode == 5)
			{
				wetMix[0] *= 0.33;
				wetMix[1] = 0.0;
				wetMix[2] *= 0.33;
				wetMix[3] *= 0.33;
				panCenter[2] = 1.0;
				panCenter[3] = -1.0;
			}
			else if (parameters.mode == 6)
			{
				wetMix[0] *= 0.25;
				wetMix[1] *= 0.25;
				wetMix[2] *= 0.25;
				wetMix[3] *= 0.25;
				panCenter[1] = 1;
				panCenter[2] = 0.5;
				panCenter[3] = -1.0;
			}
		}

		if (parameters.sidechainThresh_db != params.sidechainThresh_db)
		{
			parameters.sidechainThresh_db = params.sidechainThresh_db;
			sidechainThresh_cooked = pow(10, parameters.sidechainThresh_db / 20);
		}
		
		// ** DETECTOR **
		if (parameters.threshold_db != params.threshold_db)
		{
			parameters.threshold_db = params.threshold_db;
			threshold_cooked = pow(10, parameters.threshold_db / 20);
		}

		if (parameters.feedback_pct != params.feedback_pct) 
		{
				parameters.feedback_pct = params.feedback_pct;
				feedback_cooked = parameters.feedback_pct / 100;
		}

		// ** LFO **
		SuperLFOParameters lfoParams = modLFO.getParameters();
		lfoParams.frequency_Hz = params.lfoRate_hz;
		lfoParams.outputAmplitude = (params.lfoAmount / 10);
		modLFO.setParameters(lfoParams);

		parameters = params;
	}

private:
	RE201Parameters parameters; ///< object parameters

	// --- local variables used by this object
	double sampleRate = 0.0;	///< sample rate

	
	double samplesPerMSec = 0.0;	///< samples per millisecond, for easy access calculation
	double bufferLength_mSec = 0.0;	///< buffer length in mSec
	unsigned int bufferLength = 0;	///< buffer length in samples

	// --- wet dry mix
	double dryMix = 0.707;
	double wetMix[4] = { 0.707 };

	// --- tap
	double delayInSamples[4];;	///< double includes fractional part
	double feedback_cooked = 0.0;

	// --- delay buffer of doubles
	CircularBuffer<double> delayBuffer[2];	///< delay buffer of doubles

	// --- bpm
	double bpm = 0.0;

	// ** MODULATION **
	// --- detector
	AudioDetector detector;
	double threshold_cooked = 0.0;

	// --- sidechain
	double sidechainInputSample[2];
	double sidechainThresh_cooked;
	
	// --- lfo
	SuperLFO modLFO;

	// --- pan
	double panCenter[4] = { 0.0 };


};

#endif
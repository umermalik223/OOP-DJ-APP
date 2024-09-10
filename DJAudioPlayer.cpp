
#include "DJAudioPlayer.h"



DJAudioPlayer::DJAudioPlayer(juce::AudioFormatManager& _formatManager)
	: formatManager(_formatManager)
{
	
};


DJAudioPlayer::~DJAudioPlayer() {};



// Define the prepareToPlay() method for the DJAudioPlayer class, which prepares the audio components for playback.
void DJAudioPlayer::prepareToPlay(int samplesPerBlockExpected, double sampleRate) {
	// Prepare the transport source for playback, specifying the number of samples per block and the sample rate.
	transportSource.prepareToPlay(samplesPerBlockExpected, sampleRate);

	// Prepare the resample source for playback with the same parameters.
	resampleSource.prepareToPlay(samplesPerBlockExpected, sampleRate);

	// Prepare the low-pass filter for playback with the given parameters.
	audioLPFilter.prepareToPlay(samplesPerBlockExpected, sampleRate);

	// Prepare the high-pass filter for playback with the given parameters.
	audioHPFilter.prepareToPlay(samplesPerBlockExpected, sampleRate);

	// Prepare the low-band filter for playback with the given parameters.
	audioLBFilter.prepareToPlay(samplesPerBlockExpected, sampleRate);

	// Prepare the mid-band filter for playback with the given parameters.
	audioMBFilter.prepareToPlay(samplesPerBlockExpected, sampleRate);

	// Prepare the high-band filter for playback with the given parameters.
	audioHBFilter.prepareToPlay(samplesPerBlockExpected, sampleRate);

	// Prepare the drum transport source for playback with the given parameters.
	//drumTransportSource.prepareToPlay(samplesPerBlockExpected, sampleRate);

	// Store the sample rate for use in other methods or calculations.
	thisSampleRate = sampleRate;
}



void DJAudioPlayer::getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill) {
	audioLPFilter.getNextAudioBlock(bufferToFill);
	float rmsLevelLeft = juce::Decibels::gainToDecibels(bufferToFill.buffer->getRMSLevel(0, 0, bufferToFill.buffer->getNumSamples()));
	float rmsLevelRight = juce::Decibels::gainToDecibels(bufferToFill.buffer->getRMSLevel(1, 0, bufferToFill.buffer->getNumSamples()));
	level = (rmsLevelLeft + rmsLevelRight) / 2;
	};

//void DJAudioPlayer::getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill) {
//	// Get the main track audio
//	transportSource.getNextAudioBlock(bufferToFill);
//
//	// Prepare a temporary buffer for the drum sound
//	juce::AudioBuffer<float> drumBuffer(bufferToFill.buffer->getNumChannels(), bufferToFill.buffer->getNumSamples());
//	juce::AudioSourceChannelInfo drumBufferInfo(&drumBuffer, 0, bufferToFill.numSamples);
//
//	// Get the drum sound audio
//	drumTransportSource.getNextAudioBlock(drumBufferInfo);
//
//	// Mix drum sound with the main track
//	for (int channel = 0; channel < bufferToFill.buffer->getNumChannels(); ++channel) {
//		bufferToFill.buffer->addFrom(channel, 0, drumBuffer, channel, 0, drumBuffer.getNumSamples());
//	}
//
//	// Calculate RMS levels (optional)
//	float rmsLevelLeft = juce::Decibels::gainToDecibels(bufferToFill.buffer->getRMSLevel(0, 0, bufferToFill.buffer->getNumSamples()));
//	float rmsLevelRight = juce::Decibels::gainToDecibels(bufferToFill.buffer->getRMSLevel(1, 0, bufferToFill.buffer->getNumSamples()));
//	level = (rmsLevelLeft + rmsLevelRight) / 2;
//}


void DJAudioPlayer::releaseResources() {
	audioLPFilter.releaseResources();
};


void DJAudioPlayer::start() {
	transportSource.start();
};


// Define the stop() method for the DJAudioPlayer class, which stops playback of the audio.
void DJAudioPlayer::stop() {
	// Stop the transport source, which halts the playback of the audio.
	transportSource.stop();
}

// Define the isPlaying() method for the DJAudioPlayer class, which checks if the audio is currently playing.
bool DJAudioPlayer::isPlaying() {
	// Return whether the transport source is currently playing.
	// This indicates the playback status of the audio.
	return transportSource.isPlaying();
}

// Define the isLoaded() method for the DJAudioPlayer class, which checks if an audio file is loaded.
bool DJAudioPlayer::isLoaded() {
	// Return whether the audio file has been successfully loaded.
	// This indicates if the player is ready to play audio.
	return loaded;
}

// Define the returnURL() method for the DJAudioPlayer class, which returns the URL of the currently loaded audio.
juce::URL DJAudioPlayer::returnURL() {
	// Return the URL of the currently loaded audio file.
	return currentAudioURL;
}



// Define the loadURL() method for the DJAudioPlayer class, which loads an audio file from a given URL.
void DJAudioPlayer::loadURL(juce::URL audioURL) {
	// Create an AudioFormatReader for the audio URL by opening an input stream to the file.
	auto* reader = formatManager.createReaderFor(audioURL.createInputStream(false));

	// Check if the reader was successfully created.
	if (reader != nullptr) {
		// Create a new AudioFormatReaderSource using the reader and enable playback.
		std::unique_ptr<juce::AudioFormatReaderSource> newSource(new juce::AudioFormatReaderSource(reader, true));

		// Set the new source to the transport source, specifying the sample rate from the reader.
		transportSource.setSource(newSource.get(), 0, nullptr, reader->sampleRate);

		// Release ownership of the newSource to readerSource, managing its lifetime.
		readerSource.reset(newSource.release());

		// Log the size of the metadata associated with the loaded file.
		DBG("real metadata size: " << reader->metadataValues.size());

		// Update the loaded file name and URL.
		loadedFileName = audioURL.getFileName();
		loaded = true;
		currentAudioURL = audioURL;
	}
	else {
		// Log a debug message if the file could not be loaded.
		DBG("Something went wrong loading the file ");
		loaded = false;
	}
}




// Define the getRMSLevel() method for the DJAudioPlayer class, which returns the current RMS level.
float DJAudioPlayer::getRMSLevel() {
	// Return the current RMS (Root Mean Square) level, which represents the average power of the audio signal.
	return level;
}

// Define the getPositionRelative() method for the DJAudioPlayer class, which returns the current playback position as a fraction of the total length.
double DJAudioPlayer::getPositionRelative() {
	// Calculate and return the relative position of the playback.
	// If the length of the transport source is zero (which could indicate no audio is loaded), return 0.
	// Otherwise, return the current position divided by the total length, giving a value between 0 and 1.
	return (transportSource.getLengthInSeconds() == 0 ? 0 : transportSource.getCurrentPosition() / transportSource.getLengthInSeconds());
}




// Define the setGain() method for the DJAudioPlayer class, which sets the gain for either volume or crossfade.
void DJAudioPlayer::setGain(double gain, bool isVol) {
	// Check if the gain should be applied to the volume (isVol is true) or crossfade (isVol is false).
	if (isVol) {
		// Update the playerVol variable with the new gain value.
		playerVol = gain;
	}
	else {
		// Update the crossFadeVol variable with the new gain value.
		crossFadeVol = gain;
	}

	// Validate that the gain value is within the range of 0 to 1.
	if (gain < 0 || gain > 1.0) {
		// Log a debug message if the gain value is out of the valid range.
		DBG("DJAudioPlayer::setGain Gain should be between 0 and 1");
	}
	else {
		// Set the gain of the transport source based on the product of playerVol and crossFadeVol.
		// This allows for combined volume and crossfade adjustments.
		transportSource.setGain(playerVol * crossFadeVol);
	}
}

// Define the setSpeed() method for the DJAudioPlayer class, which sets the speed of the resample source.
void DJAudioPlayer::setSpeed(double ratio) {
	// Validate that the speed ratio is within the range of 0 to 100.
	if (ratio < 0 || ratio > 100.0) {
		// Log a debug message if the ratio value is out of the valid range.
		DBG("DJAudioPlayer::setSpeed Ratio should be between 0 and 100");
	}
	else {
		// Set the resampling ratio of the resample source to the specified value.
		// This adjusts the playback speed or pitch of the audio.
		resampleSource.setResamplingRatio(ratio);
	}
}



// Define the setPosition() method for the DJAudioPlayer class, which sets the position of the transport source.
void DJAudioPlayer::setPosition(double posInSecs) {
	// Set the position of the transport source to the specified value in seconds.
	transportSource.setPosition(posInSecs);
}

// Define the setPositionRelative() method for the DJAudioPlayer class, which sets the position as a fraction of the total length.
void DJAudioPlayer::setPositionRelative(double pos) {
	// Check if the relative position is outside the valid range (0 to 1).
	if (pos < 0 || pos > 1) {
		// Log a debug message indicating that the position should be within the range of 0 to 1.
		DBG("DJAudioPlayer::setPositionRelative pos should be between 0 and 1");
	}
	else {
		// Calculate the position in seconds based on the total length of the transport source and the relative position.
		double posInSecs = transportSource.getLengthInSeconds() * pos;

		// Call the setPosition() method with the calculated position in seconds to update the transport source.
		setPosition(posInSecs);
	}
}



// Define the setFilter() method for the DJAudioPlayer class, which sets the filter based on the given frequency.
void DJAudioPlayer::setFilter(double freq) {
	// Check if the frequency is within the valid range for a high-pass filter (greater than 0 and less than 20000 Hz).
	if (freq > 0 && freq < 20000) {
		// Make the high-pass filter inactive, as we are setting up a low-pass filter.
		audioHPFilter.makeInactive();

		// Configure the low-pass filter with the specified cutoff frequency.
		// Parameters:
		// - thisSampleRate: The sample rate of the audio.
		// - freq: The cutoff frequency in Hz for the low-pass filter.
		audioLPFilter.setCoefficients(juce::IIRCoefficients::makeLowPass(thisSampleRate, freq));
	}
	// Check if the frequency is within the valid range for a low-pass filter (less than 0 and greater than -20000 Hz).
	else if (freq < 0 && freq > -20000) {
		// Make the low-pass filter inactive, as we are setting up a high-pass filter.
		audioLPFilter.makeInactive();

		// Configure the high-pass filter with the absolute value of the frequency.
		// Parameters:
		// - thisSampleRate: The sample rate of the audio.
		// - freq: The cutoff frequency in Hz for the high-pass filter (adjusted to be positive).
		audioHPFilter.setCoefficients(juce::IIRCoefficients::makeHighPass(thisSampleRate, -freq));
	}
	// Check if the frequency is zero.
	else if (freq == 0) {
		// Make both filters inactive, effectively removing any filtering from the signal.
		audioHPFilter.makeInactive();
		audioLPFilter.makeInactive();
	}
}



// Define the setLBFilter() method for the DJAudioPlayer class, which sets the coefficients for the low-band filter.
void DJAudioPlayer::setLBFilter(double gain) {
	// Configure the low-band filter with a low-shelf filter type.
	// Parameters:
	// - thisSampleRate: The sample rate of the audio.
	// - 500: The cutoff frequency in Hz.
	// - 1.0 / juce::MathConstants<double>::sqrt2: The filter's quality factor (Q), which determines the filter's sharpness.
	// - gain: The gain value to be applied to the filter.
	audioLBFilter.setCoefficients(juce::IIRCoefficients::makeLowShelf(thisSampleRate, 500, 1.0 / juce::MathConstants<double>::sqrt2, gain));
}
void applyFadeIn(juce::AudioBuffer<float>& buffer, int fadeInDuration) {
	int numSamples = buffer.getNumSamples();
	for (int channel = 0; channel < buffer.getNumChannels(); ++channel) {
		float* channelData = buffer.getWritePointer(channel);
		for (int sample = 0; sample < fadeInDuration && sample < numSamples; ++sample) {
			float fadeFactor = static_cast<float>(sample) / static_cast<float>(fadeInDuration);
			channelData[sample] *= fadeFactor;
		}
	}
}

void applyFadeOut(juce::AudioBuffer<float>& buffer, int fadeOutDuration) {
	int numSamples = buffer.getNumSamples();
	for (int channel = 0; channel < buffer.getNumChannels(); ++channel) {
		float* channelData = buffer.getWritePointer(channel);
		for (int sample = 0; sample < fadeOutDuration && sample < numSamples; ++sample) {
			float fadeFactor = 1.0f - (static_cast<float>(sample) / static_cast<float>(fadeOutDuration));
			channelData[numSamples - sample - 1] *= fadeFactor;
		}
	}
}

void reverseAudio(juce::AudioBuffer<float>& buffer) {
	int numSamples = buffer.getNumSamples();
	for (int channel = 0; channel < buffer.getNumChannels(); ++channel) {
		float* channelData = buffer.getWritePointer(channel);
		std::reverse(channelData, channelData + numSamples);
	}
}

void applyLowPassFilter(juce::AudioBuffer<float>& buffer, float cutoffFrequency, double sampleRate) {
	juce::IIRFilter lowPassFilter;
	lowPassFilter.setCoefficients(juce::IIRCoefficients::makeLowPass(sampleRate, cutoffFrequency));
	for (int channel = 0; channel < buffer.getNumChannels(); ++channel) {
		lowPassFilter.processSamples(buffer.getWritePointer(channel), buffer.getNumSamples());
	}
}

void applyHighPassFilter(juce::AudioBuffer<float>& buffer, float cutoffFrequency, double sampleRate) {
	juce::IIRFilter highPassFilter;
	highPassFilter.setCoefficients(juce::IIRCoefficients::makeHighPass(sampleRate, cutoffFrequency));
	for (int channel = 0; channel < buffer.getNumChannels(); ++channel) {
		highPassFilter.processSamples(buffer.getWritePointer(channel), buffer.getNumSamples());
	}
}

void applyDelayEffect(juce::AudioBuffer<float>& buffer, int delaySamples, float feedback) {
	juce::AudioBuffer<float> delayBuffer(buffer.getNumChannels(), buffer.getNumSamples() + delaySamples);
	for (int channel = 0; channel < buffer.getNumChannels(); ++channel) {
		float* channelData = buffer.getWritePointer(channel);
		float* delayData = delayBuffer.getWritePointer(channel);

		for (int sample = 0; sample < buffer.getNumSamples(); ++sample) {
			int delayIndex = sample + delaySamples;
			delayData[delayIndex] = channelData[sample] + delayData[delayIndex] * feedback;
			channelData[sample] = delayData[delayIndex];
		}
	}
}

void normalizeAudio(juce::AudioBuffer<float>& buffer) {
	for (int channel = 0; channel < buffer.getNumChannels(); ++channel) {
		float* channelData = buffer.getWritePointer(channel);
		float maxSample = *std::max_element(channelData, channelData + buffer.getNumSamples());
		if (maxSample > 0.0f) {
			float gainFactor = 1.0f / maxSample;
			for (int sample = 0; sample < buffer.getNumSamples(); ++sample) {
				channelData[sample] *= gainFactor;
			}
		}
	}
}


// Define the setMBFilter() method for the DJAudioPlayer class, which sets the coefficients for the mid-band filter.
void DJAudioPlayer::setMBFilter(double gain) {
	// Configure the mid-band filter with a peak filter type.
	// Parameters:
	// - thisSampleRate: The sample rate of the audio.
	// - 3250: The center frequency of the peak filter in Hz.
	// - 1.0 / juce::MathConstants<double>::sqrt2: The filter's quality factor (Q), which determines the filter's bandwidth.
	// - gain: The gain value to be applied to the filter.
	audioMBFilter.setCoefficients(juce::IIRCoefficients::makePeakFilter(thisSampleRate, 3250, 1.0 / juce::MathConstants<double>::sqrt2, gain));
}

// Define the setHBFilter() method for the DJAudioPlayer class, which sets the coefficients for the high-band filter.
void DJAudioPlayer::setHBFilter(double gain) {
	// Configure the high-band filter with a high-shelf filter type.
	// Parameters:
	// - thisSampleRate: The sample rate of the audio.
	// - 5000: The cutoff frequency in Hz.
	// - 1.0 / juce::MathConstants<double>::sqrt2: The filter's quality factor (Q), which determines the filter's sharpness.
	// - gain: The gain value to be applied to the filter.
	audioHBFilter.setCoefficients(juce::IIRCoefficients::makeHighShelf(thisSampleRate, 5000, 1.0 / juce::MathConstants<double>::sqrt2, gain));
}


void DJAudioPlayer::playDrumSample(const juce::String& drumSamplePath)
{
	auto* reader = formatManager.createReaderFor(juce::File(drumSamplePath));
	if (reader == nullptr)
	{
		juce::AlertWindow::showMessageBoxAsync(juce::AlertWindow::WarningIcon,
			"Error",
			"Drum sample file not found or could not be loaded.",
			"OK");
		return;
	}

	try
	{
				if (drumReaderSource != nullptr)
		{
			mixerSource.removeInputSource(drumReaderSource.get());
			drumReaderSource.reset();
		}

				drumReaderSource.reset(new juce::AudioFormatReaderSource(reader, true));
		mixerSource.addInputSource(drumReaderSource.get(), false);

				drumReaderSource->setNextReadPosition(0);
	}
	catch (const std::exception& e)
	{
		juce::AlertWindow::showMessageBoxAsync(juce::AlertWindow::WarningIcon,
			"Error",
			"An error occurred while playing the drum sample: " + juce::String(e.what()),
			"OK");
	}
}

// Define the loadDrumSample() method for the DJAudioPlayer class, which loads a drum sample from a given path.
void DJAudioPlayer::loadDrumSample(const juce::String& drumSamplePath) {
	// Create a juce::File object from the provided drumSamplePath.
	juce::File drumFile(drumSamplePath);

	// Check if the file exists.
	if (drumFile.existsAsFile()) {
		// Create an AudioFormatReader for the drum file using the format manager.
		auto* reader = formatManager.createReaderFor(drumFile);

		// Check if the reader was successfully created.
		if (reader != nullptr) {
			// If there is a previously loaded drum sample source, clean it up.
			if (drumReaderSource != nullptr) {
				// Remove the previous source from the drumTransportSource and reset the drumReaderSource.
				drumTransportSource.setSource(nullptr);
				drumReaderSource.reset();
			}

			// Create a new AudioFormatReaderSource with the reader, and enable playback.
			drumReaderSource.reset(new juce::AudioFormatReaderSource(reader, true));

			// Set the new source to the drumTransportSource, specifying the sample rate of the reader.
			drumTransportSource.setSource(drumReaderSource.get(), 0, nullptr, reader->sampleRate);

			// Log a debug message indicating successful loading of the drum sample.
			DBG("Drum sample loaded successfully.");

			// Show a message box to the user indicating success.
			juce::AlertWindow::showMessageBoxAsync(juce::AlertWindow::InfoIcon,
				"Success",
				"Drum Sample Loaded successfully.",
				"OK");
		}
		else {
			// Log a debug message if the reader could not be created.
			DBG("Failed to create reader for drum sample.");

			// Show a message box indicating an error with file existence.
			/*juce::AlertWindow::showMessageBoxAsync(juce::AlertWindow::WarningIcon,
				"Error",
				"Drum Sound File Not Found.",
				"OK");*/
		}
	}
	else {
		// Log a debug message if the drum file does not exist.
		DBG("Drum file does not exist.");

		// Show a message box indicating an error with file existence.
		/*juce::AlertWindow::showMessageBoxAsync(juce::AlertWindow::WarningIcon,
			"Error",
			"Drum Sound File Not Found.",
			"OK");*/
	}
}




// Define the playDrum() method for the DJAudioPlayer class.
void DJAudioPlayer::playDrum() {
	// Check if the drumTransportSource is currently playing.
	if (drumTransportSource.isPlaying()) {
		// If the drumTransportSource is playing, reset its position to the start (0.0).
		// This effectively restarts the drum sample from the beginning.
		drumTransportSource.setPosition(0.0);
	}
	else {
		// If the drumTransportSource is not playing, start playback of the drum sample.
		// This begins playing the drum sample from its current position.
		drumTransportSource.start();
	}
}




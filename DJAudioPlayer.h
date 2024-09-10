
#pragma once
#include <JuceHeader.h>


class DJAudioPlayer : public juce::AudioSource {
public:


	// Declaration of the DJAudioPlayer class methods and constructor.

// Method to load a drum sample from a given file path.
	void loadDrumSample(const juce::String& drumSamplePath);

	// Method to start playback of the drum sample.
	void playDrum();

	// Constructor for the DJAudioPlayer class, which initializes with a reference to an AudioFormatManager.
	DJAudioPlayer(juce::AudioFormatManager& formatManager);

	// Destructor for the DJAudioPlayer class.
	~DJAudioPlayer();

	// Method to prepare the audio player for playback.
	// Parameters:
	// - samplesPerBlockExpected: The number of audio samples expected per block.
	// - sampleRate: The sample rate of the audio.
	void prepareToPlay(int samplesPerBlockExpected, double sampleRate) override;

	// Method to fill the audio buffer with the next block of audio data.
	// Parameters:
	// - bufferToFill: Contains the buffer information to be filled with audio data.
	void getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill) override;

	// Method to release any resources held by the audio player.
	void releaseResources() override;

	// Method to start playback of the audio.
	void start();

	// Method to stop playback of the audio.
	void stop();

	// Method to check if the audio is currently playing.
	// Returns:
	// - true if the audio is playing, false otherwise.
	bool isPlaying();

	// Method to check if an audio file is currently loaded.
	// Returns:
	// - true if an audio file is loaded, false otherwise.
	bool isLoaded();

	// Method to return the URL of the currently loaded audio file.
	// Returns:
	// - The URL of the currently loaded audio file.
	juce::URL returnURL();

	// Method to load an audio file from a given URL.
	// Parameters:
	// - audioURL: The URL of the audio file to be loaded.
	void loadURL(juce::URL audioURL);

	// Method to get the RMS (Root Mean Square) level of the audio signal.
	// Returns:
	// - The current RMS level of the audio signal.
	float getRMSLevel();

	// Method to get the current playback position as a fraction of the total length.
	// Returns:
	// - The current position relative to the total length of the audio, ranging from 0 to 1.
	double getPositionRelative();

	// Method to set a filter based on the given frequency.
	// Parameters:
	// - freq: The cutoff frequency for the filter in Hz.
	void setFilter(double freq);

	// Method to set the gain for the low-band filter.
	// Parameters:
	// - gain: The gain value for the low-band filter.
	void setLBFilter(double gain);

	// Method to set the gain for the mid-band filter.
	// Parameters:
	// - gain: The gain value for the mid-band filter.
	void setMBFilter(double gain);

	// Method to set the gain for either volume or crossfade.
	// Parameters:
	// - gain: The gain value to be set.
	// - isVol: Flag indicating whether the gain applies to volume (true) or crossfade (false). Defaults to true.
	void setGain(double gain, bool isVol = true);

	// Method to set the speed of the audio playback.
	// Parameters:
	// - ratio: The resampling ratio for speed adjustment.
	void setSpeed(double ratio);

	// Method to set the playback position in seconds.
	// Parameters:
	// - posInSecs: The position in seconds to set.
	void setPosition(double posInSecs);

	// Method to set the playback position as a fraction of the total length.
	// Parameters:
	// - pos: The relative position, ranging from 0 to 1.
	void setPositionRelative(double pos);

	// Method to set the gain for the high-band filter.
	// Parameters:
	// - gain: The gain value for the high-band filter.
	void setHBFilter(double gain);

	// Method to play a drum sample from a given file path.
	// Parameters:
	// 
	void applyFadeIn(juce::AudioBuffer<float>& buffer, int fadeInDuration);
	void applyFadeOut(juce::AudioBuffer<float>& buffer, int fadeOutDuration);
	void reverseAudio(juce::AudioBuffer<float>& buffer);
	void applyLowPassFilter(juce::AudioBuffer<float>& buffer, float cutoffFrequency, double sampleRate);
	void applyHighPassFilter(juce::AudioBuffer<float>& buffer, float cutoffFrequency, double sampleRate);
	void applyDelayEffect(juce::AudioBuffer<float>& buffer, int delaySamples, float feedback);
	void normalizeAudio(juce::AudioBuffer<float>& buffer);

	// - drumSamplePath: The file path of the drum sample to be played.
	void playDrumSample(const juce::String& drumSamplePath);


	
private:


	// Define member variables for the DJAudioPlayer class that are used for audio processing and playback.

	// Reference to the AudioFormatManager used for creating audio format readers.
	juce::AudioFormatManager& formatManager;

	// Unique pointer to an AudioFormatReaderSource, which is used to read audio data from a file.
	std::unique_ptr<juce::AudioFormatReaderSource> readerSource;

	// Transport source used for controlling playback of the audio.
	juce::AudioTransportSource transportSource;

	// Resampling audio source, which resamples audio from the transport source.
	// Parameters:
	// - &transportSource: The source from which audio is resampled.
	// - false: Whether the source should be owned by this object.
	// - 2: Number of channels.
	juce::ResamplingAudioSource resampleSource{ &transportSource, false, 2 };

	// Low-band IIR filter audio source for filtering audio with a low-band filter.
	// Parameters:
	// - &resampleSource: The source to which the filter is applied.
	// - false: Whether the source should be owned by this object.
	juce::IIRFilterAudioSource audioLBFilter{ &resampleSource , false };

	// The name of the currently loaded audio file.
	juce::String loadedFileName;

	// Sample rate of the audio, used for various audio processing tasks.
	double thisSampleRate;

	// Flag indicating whether an audio file has been successfully loaded.
	bool loaded = false;

	// Volume of the player, used to control the overall volume of the audio.
	double playerVol = 1;

	// Volume of the crossfade, used to control the crossfade volume.
	double crossFadeVol = 1;

	// URL of the currently loaded audio file.
	juce::URL currentAudioURL;

	// Mid-band IIR filter audio source for filtering audio with a mid-band filter.
	// Parameters:
	// - &audioLBFilter: The source to which the filter is applied.
	// - false: Whether the source should be owned by this object.
	juce::IIRFilterAudioSource audioMBFilter{ &audioLBFilter , false };

	// High-band IIR filter audio source for filtering audio with a high-band filter.
	// Parameters:
	// - &audioMBFilter: The source to which the filter is applied.
	// - false: Whether the source should be owned by this object.
	juce::IIRFilterAudioSource audioHBFilter{ &audioMBFilter , false };

	// High-pass IIR filter audio source for filtering audio with a high-pass filter.
	// Parameters:
	// - &audioHBFilter: The source to which the filter is applied.
	// - false: Whether the source should be owned by this object.
	juce::IIRFilterAudioSource audioHPFilter{ &audioHBFilter , false };

	// Low-pass IIR filter audio source for filtering audio with a low-pass filter.
	// Parameters:
	// - &audioHPFilter: The source to which the filter is applied.
	// - false: Whether the source should be owned by this object.
	juce::IIRFilterAudioSource audioLPFilter{ &audioHPFilter , false };

	// The RMS level of the audio signal, representing its average power.
	float level;

	// Mixer audio source used for mixing multiple audio sources together.
	juce::MixerAudioSource mixerSource;

	// Unique pointer to an AudioFormatReaderSource for reading drum sample data.
	std::unique_ptr<juce::AudioFormatReaderSource> drumReaderSource;

	// Transport source for controlling playback of the drum audio.
	juce::AudioTransportSource drumTransportSource;

};

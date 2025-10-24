#pragma once 
#include <JuceHeader.h>
#include <vector>

class PlayerAudio : public juce::ChangeListener
{
public:
	PlayerAudio();
	~PlayerAudio();

	void prepareToPlay(int samplesPerBlockExpected, double sampleRate) ;
	void getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill) ;
	void releaseResources() ;

	bool loadFile(const juce::File& file);
	void play();
	void stop();
    void pause();

	void setGain(float gain);
	void setPosition(double pos);

	double getPosition() const;
	double getLength() const;

	void setVolume(float newVolume);

	void changeListenerCallback(juce::ChangeBroadcaster* source) override;
	void getRepeatValue(bool value);
	bool repeatValue;
	void skipforward(double seconds);
	void skipbackward(double seconds);



private:
	juce::AudioFormatManager formatManager;
	std::unique_ptr<juce::AudioFormatReaderSource> readerSource;
	juce::AudioTransportSource transportSource;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PlayerAudio)
};


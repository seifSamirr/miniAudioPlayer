#pragma once // PlayerGUI.h
#include "PlayerAudio.h"
#include <JuceHeader.h>

class PlayerGUI : public juce::Component,
	public juce::Button::Listener,
	public juce::Slider::Listener,
	public juce::Timer
{
public:
	PlayerGUI();
	~PlayerGUI() override;

	void paint(juce::Graphics& g) override;
	void resized() override;

	void prepareToPlay(int samplesPerBlockExpected, double sampleRate);
	void getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill);
	void releaseResources();
	void timerCallback() override;


private:
	PlayerAudio playerAudio;
	// GUI elements
	//lol
	juce::AudioThumbnailCache thumbnailCache{ 5 };
	juce::AudioThumbnail thumbnail{ 512, playerAudio.getFormatManager(), thumbnailCache};
	juce::Rectangle<int> waveformBounds;

	
	juce::TextButton loadButton{ "Load Files" };
	juce::TextButton restartButton{ "Restart" };
	juce::TextButton stopButton{ "Stop" };
	juce::TextButton repeatButton{ "Repeat" };
	juce::TextButton muteButton{ "Mute" };
    juce::TextButton endButton{ "End" };
    juce::ToggleButton loopButton{ "Loop" };
    juce::TextButton pauseButton{ "Pause" };
    juce::TextButton playButton{ "Play" };
	juce::Slider volumeSlider;
	juce::Slider speedSlider;
	juce::TextButton forwardButton{"10s Forward"};
	juce::TextButton backwardButton{"10s Backward"};

	std::unique_ptr<juce::FileChooser> fileChooser;


	// Event handlers
	void buttonClicked(juce::Button* button) override;
	void sliderValueChanged(juce::Slider* slider) override;
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PlayerGUI)
};

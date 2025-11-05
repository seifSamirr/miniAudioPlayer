#pragma once // PlayerGUI.h
#include "PlayerAudio.h"
#include <JuceHeader.h>
#include "Playlist.h"
#include "PlayerGUI.h"
#include "../../../project111/Source/Playlist.h"


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
	 juce::Label metadataLabel;
     void updateMetadataDisplay();
	juce::TextButton loadPlaylistButton{ "Load Playlist" };
	juce::ListBox playlistBox;
	juce::TextButton prevTrackButton{ "Previous" };
	juce::TextButton nextTrackButton{ "Next" };
	juce::Label currentTrackLabel;							   

	std::unique_ptr<juce::FileChooser> fileChooser;

								   
	    Playlist playlist;  
class PlaylistModel : public juce::ListBoxModel
{
public:
    PlaylistModel(PlayerGUI& owner) : owner(owner) {}

    int getNumRows() override
    {
        return owner.playlist.getNumFiles();
    }

    void paintListBoxItem(int rowNumber, juce::Graphics& g,
        int width, int height, bool rowIsSelected) override
    {
        if (rowIsSelected)
            g.fillAll(juce::Colours::lightblue);

        g.setColour(juce::Colours::white);
        g.setFont(14.0f);

        juce::String text = owner.playlist.getFileName(rowNumber);
        if (rowNumber == owner.playlist.getCurrentIndex())
            text = "> " + text;

        g.drawText(text, 5, 0, width - 5, height, juce::Justification::centredLeft);
    }

    void listBoxItemClicked(int row, const juce::MouseEvent& event) override
    {
        owner.playlist.setCurrentIndex(row);
        owner.playSelectedTrack();
        owner.updatePlaylistDisplay();
    }

private:
    PlayerGUI& owner;
};

std::unique_ptr<PlaylistModel> playlistModel;  // ADD THIS LINE

void loadPlaylist();
void updatePlaylistDisplay();
void playSelectedTrack();
							   
	// Event handlers
	void buttonClicked(juce::Button* button) override;
	void sliderValueChanged(juce::Slider* slider) override;
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PlayerGUI)
};

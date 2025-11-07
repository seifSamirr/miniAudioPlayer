#pragma once 
#include "PlayerAudio.h"
#include <JuceHeader.h>
#include "Playlist.h"
#include "PlayerGUI.h"
#include <vector>


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
	juce::String getLastFilePath() const { return lastSessionFile.getFullPathName(); }

	PlayerAudio& getAudio();
	bool loadAudio(const juce::File& file);
	void setBackgroundGradient(juce::Colour start, juce::Colour end);
	juce::Colour backgroundColour = juce::Colours::black;

private:
	PlayerAudio playerAudio;
	// GUI elements
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
    juce::TextButton playButton{ "Play" };
	juce::Slider volumeSlider;
	juce::Slider speedSlider;
	juce::TextButton forwardButton{"10s >>"};
	juce::TextButton backwardButton{"<< 10s"};	   
	juce::Label metadataLabel;
	juce::TextButton loadPlaylistButton{ "Load Playlist" };
	juce::ListBox playlistBox;
	juce::TextButton prevTrackButton{ "Previous" };
	juce::TextButton nextTrackButton{ "Next" };
	juce::Label currentTrackLabel;
	juce::TextButton addMarkerButton{ "Add Marker" };
    juce::TextButton deleteMarkerButton{ "Delete Marker" };
    juce::ComboBox markersList;

	juce::Slider timeSlider;
	juce::TextButton abLoopButton{ "A-B Loop" };
	juce::TextButton Mixer{ "Mix" };

	// theam
	juce::TextButton backgroundToggleButton{ "Change Background" };
	juce::Image backgroundImage;
	bool useBackgroundImage = false;
	std::unique_ptr<juce::FileChooser> bgFileChooser;

    std::vector<std::pair<juce::String, double>> markers;
	juce::File lastSessionFile;							   

	std::unique_ptr<juce::FileChooser> fileChooser;

	juce::Colour borderColour = juce::Colours::white;
	juce::Colour gradientStart = juce::Colours::black;
	juce::Colour gradientEnd = juce::Colours::black;
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
					g.fillAll(juce::Colours::aqua);

				g.setColour(juce::Colours::white);
				g.setFont(18.0f);

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

	std::unique_ptr<PlaylistModel> playlistModel;  

	void loadPlaylist();
	void updatePlaylistDisplay();
	void playSelectedTrack();
    void updateMetadataDisplay();
	

	float valueToSliderX(double value);
	double sliderXToValue(float x);

	double startPoint = 0.0;
	double endPoint = 0.0;
	bool draggingStart = false;
	bool draggingEnd = false;
	bool showMarkers = false;

	// Event handlers
	void buttonClicked(juce::Button* button) override;
	void sliderValueChanged(juce::Slider* slider) override;
	void timerCallback();

	void mouseDown(const juce::MouseEvent& e);
	void mouseDrag(const juce::MouseEvent& e);
	void mouseUp(const juce::MouseEvent&);


	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PlayerGUI)
};

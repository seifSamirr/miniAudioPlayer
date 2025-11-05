#include "PlayerGui.h"
#include "PlayerAudio.h"

PlayerGUI::PlayerGUI() {
    for (auto* btn : { &loadButton, &restartButton, &stopButton, &repeatButton, &muteButton, &playButton, &pauseButton, &endButton, &forwardButton, &backwardButton, &loadPlaylistButton })
    {
        btn->addListener(this);
        addAndMakeVisible(btn);
    }
    // repeat button
    repeatButton.setClickingTogglesState(true);
    repeatButton.setToggleState(false, juce::dontSendNotification);
    repeatButton.addListener(this);
    addAndMakeVisible(repeatButton);

    // Volume slider
    volumeSlider.setRange(0.0, 1.0, 0.01);
    volumeSlider.setValue(0.5);
    volumeSlider.addListener(this);
    speedSlider.setRange(0.5,2.0, 0.01);
    speedSlider.setValue(1.0);
    speedSlider.addListener(this);
     playButton.addListener(this);
     restartButton.addListener(this);
     loadButton.addListener(this);
     stopButton.addListener(this);
     loopButton.addListener(this);
     pauseButton.addListener(this);
     endButton.addListener(this);

	loadPlaylistButton.addListener(this);
    prevTrackButton.addListener(this);
    nextTrackButton.addListener(this);
   
    addAndMakeVisible(volumeSlider);
    addAndMakeVisible(speedSlider);
    addAndMakeVisible(playButton);
    addAndMakeVisible(restartButton);
    addAndMakeVisible(loadButton);
    addAndMakeVisible(stopButton);
    addAndMakeVisible(loopButton);
    addAndMakeVisible(pauseButton);
    addAndMakeVisible(endButton);


	 addAndMakeVisible(loadPlaylistButton);
 addAndMakeVisible(prevTrackButton);
 addAndMakeVisible(nextTrackButton);
 addAndMakeVisible(currentTrackLabel);
 currentTrackLabel.setText("No track loded ", juce::dontSendNotification);
 playlistModel = std::make_unique<PlaylistModel>(*this);
 playlistBox.setModel(playlistModel.get());
 addAndMakeVisible(playlistBox);
 loadPlaylistButton.setColour(juce::TextButton::buttonColourId, juce::Colours::grey);

 addAndMakeVisible(metadataLabel);
 metadataLabel.setColour(juce::Label::textColourId, juce::Colours::white);
 metadataLabel.setFont(juce::Font(14.0f));
 metadataLabel.setText("No file loaded", juce::dontSendNotification);

 
    startTimerHz(30);
   
}
PlayerGUI::~PlayerGUI() {}

void PlayerGUI::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colours::darkgrey);

    auto bounds = waveformBounds;
    g.setColour(juce::Colours::black);
    g.fillRect(bounds);
    if (thumbnail.getTotalLength() > 0.0)
    {
        
        g.setColour(juce::Colours::aqua);
        thumbnail.drawChannel(g, bounds, 0.0, thumbnail.getTotalLength(), 0, 1.0f);


        
        double progress = playerAudio.getPosition() / playerAudio.getLength();
        int x = (int)(progress * bounds.getWidth()) + bounds.getX();

        g.setColour(juce::Colours::hotpink);
        g.drawLine((float)x, (float)bounds.getY(), (float)x, (float)bounds.getBottom(), 2.0f);
    }
    else
    {
        g.setColour(juce::Colours::white);
        g.drawFittedText("No Audio Loaded", bounds, juce::Justification::centred, 1);
    }
}


void PlayerGUI::prepareToPlay(int samplesPerBlockExpected, double sampleRate)
{
    playerAudio.prepareToPlay(samplesPerBlockExpected, sampleRate);
}

void PlayerGUI::getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill)
{
    playerAudio.getNextAudioBlock(bufferToFill);
}
void PlayerGUI::releaseResources()
{
    playerAudio.releaseResources();
}
void PlayerGUI::resized()
{
    int y = 20;
    loadButton.setBounds(10, y, 80, 30);
    restartButton.setBounds(110, y, 80, 30);
    stopButton.setBounds(210, y, 80, 30);
    repeatButton.setBounds(310, y, 80, 30);
    muteButton.setBounds(410, y, 80, 30);
    playButton.setBounds(510, y, 80, 30);
    pauseButton.setBounds(610, y, 80, 30);
    endButton.setBounds(710, y, 80, 30);
    forwardButton.setBounds(930, y, 100, 30);
    backwardButton.setBounds(810, y, 100, 30);
    
    volumeSlider.setBounds(10, 60, getWidth() - 20, 30);
    speedSlider.setBounds(10, 100, getWidth() - 20, 30);

    waveformBounds.setBounds(50, 160, getWidth() - 100, 80);


	auto area = getLocalBounds();
int playlistStartY = y + 200;
int playlistHeight = 400;   
loadPlaylistButton.setBounds(10, playlistStartY, 100, 25);
prevTrackButton.setBounds(120, playlistStartY, 70, 25);
nextTrackButton.setBounds(200, playlistStartY, 70, 25);
currentTrackLabel.setBounds(280, playlistStartY, 300, 25);
playlistBox.setBounds(10, playlistStartY + 40, getWidth() - 20, playlistHeight - 40);

int metadataY = y + 40;
auto metadataArea = getLocalBounds().withTop(metadataY).withHeight(180).reduced(20);
metadataLabel.setBounds(metadataArea);
int playlistY = metadataY + 70;
    

}
void PlayerGUI::buttonClicked(juce::Button* button)
{

     std::cout << "Button clicked: " << button->getButtonText().toRawUTF8() << std::endl;
	
    if (button == &loadButton)
    {
        juce::FileChooser chooser("Select audio files...",
            juce::File{},
            "*.wav;*.mp3");

        fileChooser = std::make_unique<juce::FileChooser>(
            "Select an audio file...",
            juce::File{},
            "*.wav;*.mp3");

        fileChooser->launchAsync(
            juce::FileBrowserComponent::openMode | juce::FileBrowserComponent::canSelectFiles,
            [this](const juce::FileChooser& fc)
            {
                auto file = fc.getResult();
                if (file.existsAsFile())
                {
                    playerAudio.loadFile(file);

                      updateMetadataDisplay();
					
                    thumbnail.setSource(new juce::FileInputSource(file));
                }
            }

            );
    }

    else if (button == &restartButton)
    {
		 playerAudio.setPosition(0.0);
         playerAudio.stop();
         playerAudio.play();
    }

    else if (button == &stopButton)
    {
        playerAudio.stop();
        playerAudio.setPosition(0.0);
    }
    else if (button == &repeatButton)
    {
        bool isRepeatOn = repeatButton.getToggleState();
        playerAudio.getRepeatValue(isRepeatOn);

        if (isRepeatOn)
        {
            repeatButton.setButtonText("Repeat: ON");
        }
        else
        {
            repeatButton.setButtonText("Repeat: OFF");
        }
    }
    else if (button == &muteButton) {
        static bool muted = false;
        static float prevvolume = 0.5f;
        if (!muted) {
            prevvolume = (float)volumeSlider.getValue();
            playerAudio.setVolume(0.0f);
            volumeSlider.setValue(0.0f);
            muteButton.setButtonText("Unmute");
            muted = true;
        }
        else {
            playerAudio.setVolume(prevvolume);
            volumeSlider.setValue(prevvolume);
            muteButton.setButtonText("Mute");
            muted = false;


        }
    }

    
    else if (button == &playButton)
    {
        playerAudio.play();
    }
    else if (button == &pauseButton)
    {
        playerAudio.pause();
    }
    else if (button == &endButton)
    {
        playerAudio.setPosition(playerAudio.getLength());
        playerAudio.stop();
    }
    else if (button == &forwardButton) {
		playerAudio.skipforward(10.0);
 }
    else if (button == &backwardButton) {
		playerAudio.skipbackward(10.0);
 }


 else if (button == &loadPlaylistButton)
 {
     loadPlaylist();

     updateMetadataDisplay();
 }
  
 else if (button == &prevTrackButton)
 {
     if (playlist.hasPrevious())
     {
         playlist.previous();
         playSelectedTrack();
         updatePlaylistDisplay();
     }
 }
 else if (button == &nextTrackButton)
 {
     if (playlist.hasNext())
     {
         playlist.next();
         playSelectedTrack();
         updatePlaylistDisplay();
     }
 }

	
}

void PlayerGUI::sliderValueChanged(juce::Slider* slider)
{
    if (slider == &volumeSlider)
        playerAudio.setGain((float)slider->getValue());
    if(slider == &speedSlider)
        playerAudio.setSpeed((float)slider->getValue());
}
void PlayerGUI::timerCallback()
{
    repaint();
}


void PlayerGUI::loadPlaylist()
{
    fileChooser = std::make_unique<juce::FileChooser>(
        "Select audio files for playlist",
        juce::File::getSpecialLocation(juce::File::userDesktopDirectory),
        "*.wav;*.mp3"
    );

    auto chooserFlags = juce::FileBrowserComponent::canSelectMultipleItems |
        juce::FileBrowserComponent::openMode;

    fileChooser->launchAsync(chooserFlags, [this](const juce::FileChooser& chooser)
        {
            auto results = chooser.getResults();

            if (results.size() > 0)
            {
                playlist.clear();

                
                for (auto& file : results)
                {
                    if (file.existsAsFile())
                    {
                        playlist.addFile(file);
                    }
                }

                updatePlaylistDisplay();

            
                if (playlist.getNumFiles() > 0)
                {
                    playlist.setCurrentIndex(0);
                    playSelectedTrack();
                }
            }
        });
    
    
}

void PlayerGUI::playSelectedTrack()
{
    auto file = playlist.getCurrentFile();

    if (file.existsAsFile())
    {
        playerAudio.loadFile(file);
        playerAudio.play();
        currentTrackLabel.setText("Now Playing: " + file.getFileName(), juce::dontSendNotification);

        updateMetadataDisplay();
    }
}

void PlayerGUI::updatePlaylistDisplay()
{
    
    playlistBox.updateContent();
    playlistBox.repaint();

    prevTrackButton.setEnabled(playlist.hasPrevious());
    nextTrackButton.setEnabled(playlist.hasNext());

    
    if (playlist.getNumFiles() > 0)
    {
        currentTrackLabel.setText("Tracks: " + juce::String(playlist.getNumFiles()), juce::dontSendNotification);
    }
}

void PlayerGUI::updateMetadataDisplay()
{
    auto metadata = playerAudio.getCurrentMetadata();

    juce::String metadataText;
    metadataText += "Title: " + metadata.title + "\n";
    metadataText += "Artist: " + metadata.artist + "\n";
    metadataText += "Duration: " + playerAudio.getFormattedDuration();

    metadataLabel.setText(metadataText, juce::dontSendNotification);

}


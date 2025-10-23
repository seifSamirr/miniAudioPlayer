#include "PlayerGui.h"

PlayerGUI::PlayerGUI() {
    for (auto* btn : { &loadButton, &restartButton, &stopButton, &repeatButton, &muteButton, &playButton, &pauseButton, &endButton })
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
    volumeSlider.setRange(0.0, 100.0, 1.0);
    volumeSlider.setValue(50.0);
    volumeSlider.addListener(this);
     playButton.addListener(this);
     restartButton.addListener(this);
     loadButton.addListener(this);
     stopButton.addListener(this);
     loopButton.addListener(this);
     pauseButton.addListener(this);
     endButton.addListener(this);
   
    
    addAndMakeVisible(volumeSlider);

    addAndMakeVisible(playButton);
    addAndMakeVisible(restartButton);
    addAndMakeVisible(loadButton);
    addAndMakeVisible(stopButton);
    addAndMakeVisible(loopButton);
    addAndMakeVisible(pauseButton);
    addAndMakeVisible(endButton);
}
PlayerGUI::~PlayerGUI() {}

void PlayerGUI::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colours::darkgrey);
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
    
    volumeSlider.setBounds(10, 60, getWidth() - 20, 30);
}
void PlayerGUI::buttonClicked(juce::Button* button)
{
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
                playerAudio.loadFile(file);
            });
    }

    else if (button == &restartButton)
    {
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
}

void PlayerGUI::sliderValueChanged(juce::Slider* slider)
{
    if (slider == &volumeSlider)
        playerAudio.setGain((float)slider->getValue());
}


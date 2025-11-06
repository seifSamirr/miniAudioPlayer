#include "PlayerGui.h"

PlayerGUI::PlayerGUI() {
    for (auto* btn : { &loadButton, &restartButton, &stopButton, &repeatButton, &muteButton, &playButton, &pauseButton, &endButton, &forwardButton, &backwardButton })
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
   
    addAndMakeVisible(volumeSlider);
    addAndMakeVisible(speedSlider);
    addAndMakeVisible(playButton);
    addAndMakeVisible(restartButton);
    addAndMakeVisible(loadButton);
    addAndMakeVisible(stopButton);
    addAndMakeVisible(loopButton);
    addAndMakeVisible(pauseButton);
    addAndMakeVisible(endButton);

   
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
                if (file.existsAsFile())
                {
                    playerAudio.loadFile(file);
                    thumbnail.setSource(new juce::FileInputSource(file));
                }
            }

            );
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
    else if (button == &forwardButton) {
		playerAudio.skipforward(10.0);
 }
    else if (button == &backwardButton) {
		playerAudio.skipbackward(10.0);
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


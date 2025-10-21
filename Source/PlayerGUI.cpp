#include "PlayerGUI.h"

PlayerGUI::PlayerGUI(PlayerAudio& playerAudioRef)
    : player(playerAudioRef)
{
    // Buttons
    for (auto* btn : { &loadButton, &playButton, &stopButton, &muteButton })
    {
        addAndMakeVisible(btn);
        btn->addListener(this);
    }

    // Volume slider
    volumeSlider.setRange(0.0, 1.0, 0.01);
    volumeSlider.setValue(0.5);
    volumeSlider.addListener(this);
    addAndMakeVisible(volumeSlider);
   
}

PlayerGUI::~PlayerGUI() {}

void PlayerGUI::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colours::hotpink);
}

void PlayerGUI::resized()
{
    int y = 20;
    loadButton.setBounds(20, y, 100, 40);
    playButton.setBounds(140, y, 80, 40);
    stopButton.setBounds(240, y, 80, 40);
    muteButton.setBounds(340, y, 80, 40);
    volumeSlider.setBounds(20, 100, getWidth() - 40, 30);

}
void PlayerGUI::buttonClicked(juce::Button* button)
{
    if (button == &loadButton)
    {
        auto chooser = std::make_unique<juce::FileChooser>(
            "Select an audio file...",
            juce::File{},
            "*.wav;*.mp3");

        auto chooserFlags = juce::FileBrowserComponent::openMode
            | juce::FileBrowserComponent::canSelectFiles;

        chooser->launchAsync(chooserFlags, [this](const juce::FileChooser& fc)
            {
                auto file = fc.getResult();
                if (file.existsAsFile())
                    player.loadFile(file);
            });

        fileChooser = std::move(chooser);
    
    }
    else if (button == &playButton)
    {
        player.play();
    }
    else if (button == &stopButton)
    {
        player.stop();
    }
    else if (button == &muteButton) {
        static bool muted = false;
        static float prevvolume = 0.5f;
        if (!muted) {
            prevvolume = (float)volumeSlider.getValue();
            player.setVolume(0.0f);
            volumeSlider.setValue(0.0f);
            muteButton.setButtonText("Unmute");
            muted = true;
        }
        else {
            player.setVolume(prevvolume);
            volumeSlider.setValue(prevvolume);
            muteButton.setButtonText("Mute");
            muted = false;


        }
    }                 
    
}

void PlayerGUI::sliderValueChanged(juce::Slider* slider)
{
    if (slider == &volumeSlider)
        player.setVolume((float)slider->getValue());
   
}
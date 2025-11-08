#include "PlayerGui.h"
#include "PlayerAudio.h"

PlayerGUI::PlayerGUI() {
    updateComponentStyles();
    for (auto* btn : { &loadButton, &restartButton, &stopButton, &repeatButton, &muteButton, &playButton, &endButton, &forwardButton, &backwardButton, &loadPlaylistButton, &abLoopButton, &Mixer, &repeatButton, &backgroundToggleButton })
    {
        btn->addListener(this);
        addAndMakeVisible(btn);
        btn->setOpaque(false);
    }
    //for sec theam 
    volumeSlider.setOpaque(false);
    speedSlider.setOpaque(false);
    timeSlider.setOpaque(false);
    markersList.setOpaque(false);
    playlistBox.setOpaque(false);
    currentTrackLabel.setOpaque(false);
    metadataLabel.setOpaque(false);

    // repeat button
    repeatButton.setClickingTogglesState(true);
    repeatButton.setToggleState(false, juce::dontSendNotification);

    playButton.setClickingTogglesState(true);
    playButton.setToggleState(false, juce::dontSendNotification);

    

    loopButton.addListener(this);

    loadPlaylistButton.addListener(this);
    prevTrackButton.addListener(this);
    nextTrackButton.addListener(this);

    deleteMarkerButton.addListener(this);
    addMarkerButton.addListener(this);

    addAndMakeVisible(volumeSlider);
    addAndMakeVisible(loopButton);

    addAndMakeVisible(addMarkerButton);
    addAndMakeVisible(deleteMarkerButton);
    addAndMakeVisible(markersList);

    markersList.onChange = [this]()
        {
            int index = markersList.getSelectedId() - 1;
            if (index >= 0 && index < static_cast<int>(markers.size()))
            {
                double pos = markers[index].second;
                playerAudio.setPosition(pos);
            }
        };

    addAndMakeVisible(loadPlaylistButton);
    addAndMakeVisible(prevTrackButton);
    addAndMakeVisible(nextTrackButton);
    addAndMakeVisible(currentTrackLabel);
    currentTrackLabel.setText("No track loded ", juce::dontSendNotification);
    playlistModel = std::make_unique<PlaylistModel>(*this);
    playlistBox.setModel(playlistModel.get());
    addAndMakeVisible(playlistBox);
    playlistBox.setColour(juce::ListBox::textColourId, juce::Colours::black);
    playlistBox.setColour(juce::ListBox::outlineColourId, juce::Colours::white);

    addAndMakeVisible(metadataLabel);
    metadataLabel.setColour(juce::Label::textColourId, juce::Colours::white);
    metadataLabel.setFont(juce::Font(14.0f));
    metadataLabel.setText("No file loaded", juce::dontSendNotification);
    metadataLabel.setJustificationType(juce::Justification::centredLeft);

    backgroundColour = juce::Colours::darkgrey;
    gradientStart = juce::Colours::darkgrey;
    gradientEnd = juce::Colours::black;

    startTimerHz(10);

    // Volume slider
    volumeSlider.setRange(0.0, 1.0, 0.01);
    volumeSlider.setValue(0.5);
    volumeSlider.addListener(this);
    addAndMakeVisible(volumeSlider);
    volumeSlider.setColour(juce::Slider::thumbColourId, juce::Colours::aqua);        
    volumeSlider.setColour(juce::Slider::trackColourId, juce::Colours::aqua);


    // speed slider (noura)
    addAndMakeVisible(speedSlider);
    speedSlider.setRange(0.5, 2.0, 0.01);
    speedSlider.setValue(1.0);
    speedSlider.addListener(this);
    speedSlider.setColour(juce::Slider::thumbColourId, juce::Colours::aqua);
    speedSlider.setColour(juce::Slider::trackColourId, juce::Colours::aqua);


    // Time Slider
    addAndMakeVisible(timeSlider);
    timeSlider.addListener(this);
    timeSlider.addMouseListener(this, false);

    timeSlider.setColour(juce::Slider::thumbColourId, juce::Colours::aqua);         
    timeSlider.setColour(juce::Slider::trackColourId, juce::Colours::aqua);     
    timeSlider.setColour(juce::Slider::textBoxTextColourId, juce::Colours::white); 

   
    // (a - b) Button
    abLoopButton.setClickingTogglesState(true);
    abLoopButton.setToggleState(false, juce::dontSendNotification);

}
PlayerGUI::~PlayerGUI() {}

void PlayerGUI::paint(juce::Graphics& g)
{
    if (useBackgroundImage && backgroundImage.isValid())
    {
        g.drawImage(backgroundImage, getLocalBounds().toFloat(),
            juce::RectanglePlacement::fillDestination);
    }
    else
    {
        juce::ColourGradient backgroundGradient(
            gradientStart, 0, 0,
            gradientEnd, 0, (float)getHeight(),
            false
        );
        g.setGradientFill(backgroundGradient);
        g.fillAll();
    }

    auto bounds = waveformBounds;
    g.setColour(juce::Colours::black);
    g.fillRect(bounds);

    if (thumbnail.getTotalLength() > 0.0)
    {

        g.setColour(juce::Colours::aqua);
        thumbnail.drawChannel(g, bounds, 0.0, thumbnail.getTotalLength(), 0,1.0f);



        double progress = playerAudio.getPosition() / playerAudio.getLength();
        int x = (int)(progress * bounds.getWidth()) + bounds.getX();

        g.setColour(juce::Colours::hotpink);
        g.drawLine((float)x, (float)bounds.getY(), (float)x, (float)bounds.getBottom(), 2.0f);
    }
    else
    {
        g.setColour(juce::Colours::black);
        g.drawFittedText("No Audio Loaded", bounds, juce::Justification::centred, 1);
    }

    // for a-b loop (seif)
    if (showMarkers)
    {
        float startX = valueToSliderX(startPoint);
        float endX = valueToSliderX(endPoint);

        auto sliderBounds = timeSlider.getBounds().toFloat();
        float y = sliderBounds.getCentreY();

        g.setColour(juce::Colours::black);
        g.drawLine(startX, sliderBounds.getY(), startX, sliderBounds.getBottom(), 2.0f);

        g.setColour(juce::Colours::black);
        g.drawLine(endX, sliderBounds.getY(), endX, sliderBounds.getBottom(), 2.0f);
    }
    g.setColour(borderColour);
    g.drawRoundedRectangle(getLocalBounds().toFloat().reduced(4), 0.0f, 3.0f);
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
    auto area = getLocalBounds().reduced(10);
    int spacing = 5;

    auto topRow = area.removeFromTop(35);
    int buttonWidth = juce::jmax(60, (topRow.getWidth() - 9 * spacing) / 10);

    loadButton.setBounds(topRow.removeFromLeft(buttonWidth).reduced(1));
    topRow.removeFromLeft(spacing);
    restartButton.setBounds(topRow.removeFromLeft(buttonWidth).reduced(1));
    topRow.removeFromLeft(spacing);
    stopButton.setBounds(topRow.removeFromLeft(buttonWidth).reduced(1));
    topRow.removeFromLeft(spacing);
    repeatButton.setBounds(topRow.removeFromLeft(buttonWidth).reduced(1));
    topRow.removeFromLeft(spacing);
    muteButton.setBounds(topRow.removeFromLeft(buttonWidth).reduced(1));
    topRow.removeFromLeft(spacing);
    playButton.setBounds(topRow.removeFromLeft(buttonWidth).reduced(1));
    topRow.removeFromLeft(spacing);
    endButton.setBounds(topRow.removeFromLeft(buttonWidth).reduced(1));
    topRow.removeFromLeft(spacing);
    abLoopButton.setBounds(topRow.removeFromLeft(buttonWidth).reduced(1));
    topRow.removeFromLeft(spacing);
    backgroundToggleButton.setBounds(topRow.reduced(1));

    area.removeFromTop(spacing);
    auto slidersRow = area.removeFromTop(40);
    int sliderWidth = (slidersRow.getWidth() - 10) / 2;
    volumeSlider.setBounds(slidersRow.removeFromLeft(sliderWidth));
    slidersRow.removeFromLeft(10);
    speedSlider.setBounds(slidersRow);

    area.removeFromTop(spacing);
    timeSlider.setBounds(area.removeFromTop(35));

    area.removeFromTop(spacing);
    waveformBounds = area.removeFromTop(80).reduced(3);

    area.removeFromTop(spacing);

    auto transportRow = area.removeFromTop(70);
    int navButtonWidth = juce::jmax(65, (transportRow.getWidth() - spacing * 2) / 8);

    backwardButton.setBounds(transportRow.removeFromLeft(navButtonWidth).reduced(1));
    transportRow.removeFromLeft(spacing);
    forwardButton.setBounds(transportRow.removeFromRight(navButtonWidth).reduced(1));
    transportRow.removeFromRight(spacing);

    metadataLabel.setBounds(transportRow.reduced(5));

    area.removeFromTop(spacing);

    auto markerRow = area.removeFromTop(35);
    int markerButtonWidth = juce::jmax(90, (markerRow.getWidth() - 2 * spacing) / 4);

    addMarkerButton.setBounds(markerRow.removeFromLeft(markerButtonWidth).reduced(1));
    markerRow.removeFromLeft(spacing);
    deleteMarkerButton.setBounds(markerRow.removeFromRight(markerButtonWidth).reduced(1));
    markerRow.removeFromRight(spacing);
    markersList.setBounds(markerRow.reduced(1));

    area.removeFromTop(spacing);

    auto playlistHeader = area.removeFromTop(30);
    int plButtonWidth = juce::jmax(80, (playlistHeader.getWidth() - 3 * spacing) / 5);

    loadPlaylistButton.setBounds(playlistHeader.removeFromLeft(plButtonWidth).reduced(1));
    playlistHeader.removeFromLeft(spacing);
    prevTrackButton.setBounds(playlistHeader.removeFromLeft(plButtonWidth).reduced(1));
    playlistHeader.removeFromLeft(spacing);
    nextTrackButton.setBounds(playlistHeader.removeFromLeft(plButtonWidth).reduced(1));
    playlistHeader.removeFromLeft(spacing);
    currentTrackLabel.setBounds(playlistHeader.reduced(1));

    area.removeFromTop(spacing);

    playlistBox.setBounds(area);
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

                    timeSlider.setRange(0.0, playerAudio.getLength(), 0.01);
                    timeSlider.setValue(0.0);

                    startPoint = 0.0;
                    endPoint = playerAudio.getLength();
                    repaint();
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
            repeatButton.setButtonText("Repeat:ON");
        }
        else
        {
            repeatButton.setButtonText("Repeat:OFF");
        }
    }
    else if (button == &muteButton) {
        static bool muted = false;
        static float prevvolume = 0.5f;
        if (!muted) {
            prevvolume = (float)volumeSlider.getValue();
            playerAudio.setVolume(0.0f);
            volumeSlider.setValue(0.0f);
            muteButton.setButtonText("UnMute");
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
        if (playButton.getToggleState())
        {
            playerAudio.play();
            playButton.setButtonText("Pause");
        }
        else
        {
            playerAudio.pause();
            playButton.setButtonText("Play");
        }
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


    else if (button == &addMarkerButton)
    {
        double pos = playerAudio.getPosition();
        int totalSec = static_cast<int>(pos);
        int minutes = totalSec / 60;
        int seconds = totalSec % 60;
        juce::String timeMarker = juce::String(minutes) + ":" + juce::String(seconds);


        juce::String label = "Marker " + juce::String((int)markers.size() + 1) + " (" + timeMarker + ")";
        markers.push_back({ label, pos });
        markersList.addItem(label, (int)markers.size());
    }
    else if (button == &deleteMarkerButton)
    {
        int index = markersList.getSelectedId() - 1;
        if (index >= 0 && index < static_cast<int>(markers.size()))
        {
            markers.erase(markers.begin() + index);
            markersList.clear();
            for (int i = 0; i < static_cast<int>(markers.size()); ++i)
                markersList.addItem(markers[i].first, i + 1);
        }
    }
    else if (button == &abLoopButton)
    {
        showMarkers = abLoopButton.getToggleState();

        if (showMarkers)
        {
            abLoopButton.setButtonText("A-B: ON");

            if (startPoint == 0.0 && endPoint == timeSlider.getMaximum())
            {
                double length = playerAudio.getLength();
                startPoint = 0.0;
                endPoint = length;
            }
        }
        else
        {
            abLoopButton.setButtonText("A-B Loop");
        }

        repaint();
    }
    else if (button == &backgroundToggleButton)
    {
        if (useBackgroundImage)
        {
            useBackgroundImage = false;
            backgroundImage = juce::Image();
            backgroundToggleButton.setButtonText("Change Background");

            updateComponentStyles();
            repaint();
        }
        else
        {
            bgFileChooser = std::make_unique<juce::FileChooser>(
                "Select background image...",
                juce::File{},
                "*.jpg;*.jpeg;*.png;*.bmp");

            bgFileChooser->launchAsync(
                juce::FileBrowserComponent::openMode | juce::FileBrowserComponent::canSelectFiles,
                [this](const juce::FileChooser& fc)
                {
                    auto file = fc.getResult();
                    if (file.existsAsFile())
                    {
                        backgroundImage = juce::ImageFileFormat::loadFrom(file);
                        if (backgroundImage.isValid())
                        {
                            useBackgroundImage = true;
                            backgroundToggleButton.setButtonText("Remove BG");
                            updateComponentStyles();
                            repaint();
                        }
                    }
                }
            );
        }
    }
    else if (button == &nextTrackButton)
    {
        if (playlist.hasNext())
        {
            int currentIndex = playlist.getCurrentIndex();
            playlist.setCurrentIndex(currentIndex + 1);

            playSelectedTrack();     
            updatePlaylistDisplay();  
        }
        }
    else if (button == &prevTrackButton)
    {
        if (playlist.hasPrevious())
        {
            int currentIndex = playlist.getCurrentIndex();
            playlist.setCurrentIndex(currentIndex - 1);

            playSelectedTrack();    
            updatePlaylistDisplay();     
        }
        }
}

void PlayerGUI::sliderValueChanged(juce::Slider* slider)
{
    if (slider == &volumeSlider)
        playerAudio.setGain((float)slider->getValue());
    else if (slider == &speedSlider) {
        playerAudio.setSpeed((float)slider->getValue());
    }
    else if (slider == &timeSlider) {
        playerAudio.setPosition((double)slider->getValue());
    }
}
void PlayerGUI::timerCallback()
{
    repaint();
    if (playerAudio.getLength() <= 0)
        return;

    double currentPos = playerAudio.getPosition();

    if (!timeSlider.isMouseButtonDown() && !draggingStart && !draggingEnd) {
        timeSlider.setValue(currentPos, juce::dontSendNotification);
    };

    if (showMarkers)
    {
        if (currentPos < startPoint)
        {
            playerAudio.setPosition(startPoint);
            timeSlider.setValue(startPoint, juce::dontSendNotification);
            return;
        }
        if (currentPos >= endPoint)
        {
            playerAudio.setPosition(startPoint);
            timeSlider.setValue(startPoint, juce::dontSendNotification);
            return;
        }
    }
}

PlayerAudio& PlayerGUI::getAudio() {
    return playerAudio;
}

bool PlayerGUI::loadAudio(const juce::File& file)
{
    bool ok = playerAudio.loadFile(file);
    if (ok)
        thumbnail.setSource(new juce::FileInputSource(file));
    lastSessionFile = file;
    return ok;
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
    metadataText << "Title: " << metadata.title << "  |  ";
    metadataText << "Artist: " << metadata.artist << "  |  ";
    metadataText << "Duration: " << playerAudio.getFormattedDuration();

    metadataLabel.setText(metadataText, juce::dontSendNotification);
}

float PlayerGUI::valueToSliderX(double value)
{
    auto sliderBounds = timeSlider.getBounds().toFloat();

    double range = timeSlider.getMaximum() - timeSlider.getMinimum();
    if (range <= 0)
        return sliderBounds.getX();

    double proportion = (value - timeSlider.getMinimum()) / range;

    
    auto textBoxWidth = 0; 

    
    if (timeSlider.getTextBoxPosition() != juce::Slider::NoTextBox)
    {
        textBoxWidth = timeSlider.getTextBoxWidth();
    }

    float trackX = sliderBounds.getX() + textBoxWidth + 8; 
    float trackWidth = sliderBounds.getWidth() - textBoxWidth - 16; 

    return trackX + (float)(proportion * trackWidth);
}

double PlayerGUI::sliderXToValue(float x)
{
    auto sliderBounds = timeSlider.getBounds();

    
    int padding = 8;

    float trackX = sliderBounds.getX() + padding;
    float trackWidth = sliderBounds.getWidth() - (2 * padding);

    float localX = x - trackX;
    double proportion = localX / trackWidth;

    proportion = juce::jlimit(0.0, 1.0, proportion);

    double range = timeSlider.getMaximum() - timeSlider.getMinimum();
    return timeSlider.getMinimum() + (proportion * range);
}

void PlayerGUI::mouseDown(const juce::MouseEvent& e)
{
    
    if (e.eventComponent != &timeSlider)
        return;

    if (!showMarkers)
        return;

    
    auto mousePos = e.getEventRelativeTo(&timeSlider).position;

    float startX = valueToSliderX(startPoint) - timeSlider.getX();
    float endX = valueToSliderX(endPoint) - timeSlider.getX();
    float y = timeSlider.getHeight() / 2.0f;

    
    if (std::abs(mousePos.x - startX) < 15 && std::abs(mousePos.y - y) < 15)
    {
        draggingStart = true;
        draggingEnd = false;
        timeSlider.setInterceptsMouseClicks(false, false);
        e.eventComponent->mouseDown(e); 
        return;
    }

    if (std::abs(mousePos.x - endX) < 15 && std::abs(mousePos.y - y) < 15)
    {
        draggingEnd = true;
        draggingStart = false;
        timeSlider.setInterceptsMouseClicks(false, false);
        return;
    }
}


void PlayerGUI::mouseDrag(const juce::MouseEvent& e)
{
 
    if (!draggingStart && !draggingEnd)
        return;

    
    float mouseX = e.position.x;
    double newValue = sliderXToValue(mouseX);

    if (draggingStart)
    {
        startPoint = juce::jlimit(timeSlider.getMinimum(), endPoint - 0.1, newValue);
    }
    else if (draggingEnd)
    {

        endPoint = juce::jlimit(startPoint + 0.1, timeSlider.getMaximum(), newValue);
    }

    repaint();
}

void PlayerGUI::mouseUp(const juce::MouseEvent&)
{
    
    timeSlider.setInterceptsMouseClicks(true, false);

    draggingStart = false;
    draggingEnd = false;
}
void PlayerGUI::setBackgroundGradient(juce::Colour start, juce::Colour end)
{
    gradientStart = start;
    gradientEnd = end;
    repaint(); 
}

void PlayerGUI::updateComponentStyles()
{
    juce::Colour buttonBG;
    juce::Colour sliderBG;
    juce::Colour listBG;

    if (useBackgroundImage)
    {
        buttonBG = juce::Colours::transparentBlack;
        sliderBG = juce::Colours::transparentBlack;
        listBG = juce::Colours::black.withAlpha(0.7f); 
    }
    else
    {
        buttonBG = juce::Colours::black;
        sliderBG = juce::Colours::black;
        listBG = juce::Colours::black;
    }

    auto setButtonStyle = [buttonBG](juce::TextButton& btn)
        {
            btn.setColour(juce::TextButton::buttonColourId, buttonBG);
            btn.setColour(juce::TextButton::textColourOffId, juce::Colours::whitesmoke);
            btn.setColour(juce::TextButton::buttonOnColourId, juce::Colours::aqua);
        };

    for (auto* btn : { &loadButton, &restartButton, &stopButton, &repeatButton, &muteButton,
                        &playButton, &endButton, &forwardButton, &backwardButton,
                        &loadPlaylistButton, &addMarkerButton, &deleteMarkerButton, &abLoopButton,
                        &prevTrackButton, &nextTrackButton, &backgroundToggleButton })
    {
        setButtonStyle(*btn);
    }
    loadPlaylistButton.setColour(juce::TextButton::buttonColourId, buttonBG);

    volumeSlider.setColour(juce::Slider::backgroundColourId, sliderBG);
    speedSlider.setColour(juce::Slider::backgroundColourId, sliderBG);
    timeSlider.setColour(juce::Slider::backgroundColourId, sliderBG);

    playlistBox.setColour(juce::ListBox::backgroundColourId, listBG);
}
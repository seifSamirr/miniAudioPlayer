#include "PlayerGui.h"
#include "PlayerAudio.h"

PlayerGUI::PlayerGUI() {
    for (auto* btn : { &loadButton, &restartButton, &stopButton, &repeatButton, &muteButton, &playButton, &pauseButton, &endButton, &forwardButton, &backwardButton, &loadPlaylistButton, &abLoopButton })
    {
        btn->addListener(this);
        addAndMakeVisible(btn);
    }
    // repeat button
    repeatButton.setClickingTogglesState(true);
    repeatButton.setToggleState(false, juce::dontSendNotification);
    repeatButton.addListener(this);
    addAndMakeVisible(repeatButton);

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

	deleteMarkerButton.addListener(this);
    addMarkerButton.addListener(this);
   
    addAndMakeVisible(volumeSlider);
    addAndMakeVisible(speedSlider);
    addAndMakeVisible(playButton);
    addAndMakeVisible(restartButton);
    addAndMakeVisible(loadButton);
    addAndMakeVisible(stopButton);
    addAndMakeVisible(loopButton);
    addAndMakeVisible(pauseButton);
    addAndMakeVisible(endButton);

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
 loadPlaylistButton.setColour(juce::TextButton::buttonColourId, juce::Colours::grey);

 addAndMakeVisible(metadataLabel);
 metadataLabel.setColour(juce::Label::textColourId, juce::Colours::white);
 metadataLabel.setFont(juce::Font(14.0f));
 metadataLabel.setText("No file loaded", juce::dontSendNotification);

 
    startTimerHz(10);

    // Volume slider
    volumeSlider.setRange(0.0, 1.0, 0.01);
    volumeSlider.setValue(0.5);
    volumeSlider.addListener(this);
    addAndMakeVisible(volumeSlider);
    volumeSlider.setColour(juce::Slider::thumbColourId, juce::Colours::whitesmoke);         // لون المقبض
    volumeSlider.setColour(juce::Slider::backgroundColourId, juce::Colours::black);
    volumeSlider.setColour(juce::Slider::trackColourId, juce::Colours::white);


    // Time Slider
    addAndMakeVisible(timeSlider);
    timeSlider.addListener(this);
    timeSlider.addMouseListener(this, false);

    timeSlider.setColour(juce::Slider::thumbColourId, juce::Colours::whitesmoke);         // لون المقبض
    timeSlider.setColour(juce::Slider::trackColourId, juce::Colours::white);     // لون المسار
    timeSlider.setColour(juce::Slider::backgroundColourId, juce::Colours::black);  // خلفية السلايدر
    timeSlider.setColour(juce::Slider::textBoxTextColourId, juce::Colours::white); // لون النص

    //timeSlider.setTextBoxStyle(
    //    juce::Slider::TextBoxAbove,  // مكان المربع (None, Left, Right, Above, Below)
    //    false,                       // هل المربع editable؟
    //    50, 20                       // الحجم (width, height)
    //);

    // (a - b) Button
    abLoopButton.setClickingTogglesState(true);
    abLoopButton.setToggleState(false, juce::dontSendNotification);
   
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

    // ====== الصف العلوي للأزرار ======
    auto topRow = area.removeFromTop(40);

    juce::FlexBox topFlex;
    topFlex.flexDirection = juce::FlexBox::Direction::row;
    topFlex.justifyContent = juce::FlexBox::JustifyContent::spaceBetween;
    topFlex.alignItems = juce::FlexBox::AlignItems::center;

    topFlex.items = {
        juce::FlexItem(loadButton).withMinWidth(70).withHeight(30),
        juce::FlexItem(restartButton).withMinWidth(70).withHeight(30),
        juce::FlexItem(stopButton).withMinWidth(70).withHeight(30),
        juce::FlexItem(repeatButton).withMinWidth(70).withHeight(30),
        juce::FlexItem(muteButton).withMinWidth(70).withHeight(30),
        juce::FlexItem(playButton).withMinWidth(70).withHeight(30),
        juce::FlexItem(pauseButton).withMinWidth(70).withHeight(30),
        juce::FlexItem(endButton).withMinWidth(70).withHeight(30),
        juce::FlexItem(backwardButton).withMinWidth(90).withHeight(30),
        juce::FlexItem(forwardButton).withMinWidth(90).withHeight(30),
        juce::FlexItem(abLoopButton).withMinWidth(80).withHeight(30)
    };

    topFlex.performLayout(topRow);

    // ====== السلايدرز ======
    volumeSlider.setBounds(area.removeFromTop(40));
    speedSlider.setBounds(area.removeFromTop(40));
    timeSlider.setBounds(area.removeFromTop(40));

    // ====== موجة الصوت ======
    waveformBounds = area.removeFromTop(120).reduced(10);

    // ====== بيانات المقطع ======
    metadataLabel.setBounds(area.removeFromTop(60).reduced(5));

    // ====== ماركرز ======
    auto markerRow = area.removeFromTop(40);

    juce::FlexBox markerFlex;
    markerFlex.flexDirection = juce::FlexBox::Direction::row;
    markerFlex.justifyContent = juce::FlexBox::JustifyContent::flexStart;
    markerFlex.alignItems = juce::FlexBox::AlignItems::center;

    markerFlex.items = {
        juce::FlexItem(addMarkerButton).withMinWidth(120).withHeight(30),
        juce::FlexItem(markersList).withMinWidth(200).withHeight(30),
        juce::FlexItem(deleteMarkerButton).withMinWidth(120).withHeight(30)
    };

    markerFlex.performLayout(markerRow);

    // ====== Playlist ======
    auto playlistHeader = area.removeFromTop(30);

    juce::FlexBox playlistHeaderFlex;
    playlistHeaderFlex.flexDirection = juce::FlexBox::Direction::row;
    playlistHeaderFlex.justifyContent = juce::FlexBox::JustifyContent::flexStart;
    playlistHeaderFlex.alignItems = juce::FlexBox::AlignItems::center;

    playlistHeaderFlex.items = {
        juce::FlexItem(loadPlaylistButton).withMinWidth(120).withHeight(25),
        juce::FlexItem(prevTrackButton).withMinWidth(70).withHeight(25),
        juce::FlexItem(nextTrackButton).withMinWidth(70).withHeight(25),
        juce::FlexItem(currentTrackLabel).withFlex(1.0f).withHeight(25)
    };

    playlistHeaderFlex.performLayout(playlistHeader);

    playlistBox.setBounds(area.reduced(5)); // يتكبر مع الويندو
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

PlayerAudio& PlayerGUI::getAudio(){ 
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
    metadataText += "Title: " + metadata.title + "\n";
    metadataText += "Artist: " + metadata.artist + "\n";
    metadataText += "Duration: " + playerAudio.getFormattedDuration();

    metadataLabel.setText(metadataText, juce::dontSendNotification);

}

float PlayerGUI::valueToSliderX(double value)
{
    auto sliderBounds = timeSlider.getBounds().toFloat();

    double range = timeSlider.getMaximum() - timeSlider.getMinimum();
    if (range <= 0)
        return sliderBounds.getX();

    double proportion = (value - timeSlider.getMinimum()) / range;

    // ✅ Linear Slider style عنده textbox على الشمال
    // جرب تحسب العرض الفعلي:
    auto textBoxWidth = 0; // لو الـ TextBox مخفي

    // لو عندك TextBox:
    if (timeSlider.getTextBoxPosition() != juce::Slider::NoTextBox)
    {
        textBoxWidth = timeSlider.getTextBoxWidth();
    }

    float trackX = sliderBounds.getX() + textBoxWidth + 8; // 8 هو padding
    float trackWidth = sliderBounds.getWidth() - textBoxWidth - 16; // 16 = padding من الجهتين

    return trackX + (float)(proportion * trackWidth);
}

double PlayerGUI::sliderXToValue(float x)
{
    auto sliderBounds = timeSlider.getBounds();

    // ✅ نفس الـ padding
    int padding = 8;

    float trackX = sliderBounds.getX() + padding;
    float trackWidth = sliderBounds.getWidth() - (2 * padding);

    // احسب النسبة
    float localX = x - trackX;
    double proportion = localX / trackWidth;

    // حدد النطاق
    proportion = juce::jlimit(0.0, 1.0, proportion);

    // حول للقيمة
    double range = timeSlider.getMaximum() - timeSlider.getMinimum();
    return timeSlider.getMinimum() + (proportion * range);
}

void PlayerGUI::mouseDown(const juce::MouseEvent& e)
{
    // ✅ لو الماوس مش على الـ Slider، متعملش حاجة
    if (e.eventComponent != &timeSlider)
        return;

    if (!showMarkers)
        return;

    // ✅ احصل على الموضع relative للـ Slider نفسه
    auto mousePos = e.getEventRelativeTo(&timeSlider).position;

    float startX = valueToSliderX(startPoint) - timeSlider.getX();
    float endX = valueToSliderX(endPoint) - timeSlider.getX();
    float y = timeSlider.getHeight() / 2.0f;

    // تحقق من النقط
    if (std::abs(mousePos.x - startX) < 15 && std::abs(mousePos.y - y) < 15)
    {
        draggingStart = true;
        draggingEnd = false;
        timeSlider.setInterceptsMouseClicks(false, false);
        e.eventComponent->mouseDown(e); // أوقف التمرير للـ Slider
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
    // ✅ لو مش بنسحب حاجة، سيب الـ Slider يشتغل
    if (!draggingStart && !draggingEnd)
        return;

    // ✅ احصل على موضع الماوس
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
    // ✅ رجع الـ Slider للوضع الطبيعي
    timeSlider.setInterceptsMouseClicks(true, false);

    draggingStart = false;
    draggingEnd = false;
}




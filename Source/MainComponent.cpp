#include "MainComponent.h"
#include "PlayerAudio.h"
#include "PlayerGUI.h"

MainComponent::MainComponent()
{
    juce::Image logoImage = juce::ImageFileFormat::loadFrom(
        BinaryData::SMA3NY_ico,       
        BinaryData::SMA3NY_icoSize   
    );

    SMA3NYlogo.setImage(logoImage);
    addAndMakeVisible(SMA3NYlogo);

    addAndMakeVisible(player1);
    addAndMakeVisible(player2);
    setSize(1000, 1000);

    mixer.addInputSource(&player1.getAudio().getTransportSource(), false);
    mixer.addInputSource(&player2.getAudio().getTransportSource(), false);
    setAudioChannels(0, 2);
    loadSession();

    player1.setBackgroundGradient(juce::Colours::darkblue, juce::Colours::purple);
    player2.setBackgroundGradient(juce::Colours::mediumvioletred, juce::Colours::yellow);
    resized();
        
}

MainComponent::~MainComponent()
{
    saveSession();
    shutdownAudio();
}

void MainComponent::prepareToPlay(int samplesPerBlockExpected, double sampleRate)
{
    mixer.prepareToPlay(samplesPerBlockExpected, sampleRate);
    player1.prepareToPlay(samplesPerBlockExpected, sampleRate);
    player2.prepareToPlay(samplesPerBlockExpected, sampleRate);
}

void MainComponent::getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill)
{
    bufferToFill.clearActiveBufferRegion();
    mixer.getNextAudioBlock(bufferToFill);

}

void MainComponent::releaseResources()
{
    mixer.releaseResources();
    player1.releaseResources();
    player2.releaseResources();
}

void MainComponent::resized()
{
    int padding = 20;
    int playerWidth = (getWidth() / 2) - (padding * 1.5);
    int playerHeight = getHeight() - 60;

    player1.setBounds(padding, padding, playerWidth, playerHeight);
    player2.setBounds(playerWidth + (padding * 2), padding, playerWidth, playerHeight);

    SMA3NYlogo.setBounds(10, 10, 100, 100);
}

void MainComponent::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colours::black.brighter(0.1f));

}

void MainComponent::saveSession()
{
    juce::XmlElement lastSession("LASTSESSION");

    lastSession.setAttribute("audioFile", player1.getLastFilePath());
    lastSession.setAttribute("position", player1.getAudio().getPosition());

    auto sessionFile = juce::File::getSpecialLocation(juce::File::userApplicationDataDirectory)
        .getChildFile("session.xml");

    lastSession.writeToFile(sessionFile, {});
}



void MainComponent::loadSession()
{
    auto sessionFile = juce::File::getSpecialLocation(juce::File::userApplicationDataDirectory)
        .getChildFile("session.xml");

    if (!sessionFile.existsAsFile()) return;

    std::unique_ptr<juce::XmlElement> session = juce::XmlDocument::parse(sessionFile);
    if (!session) return;

    juce::String filePath = session->getStringAttribute("audioFile");
    double lastPosition = session->getDoubleAttribute("position", 0.0);

    juce::File file(filePath);
    if (file.existsAsFile())
    {
        player1.loadAudio(file);
        player1.getAudio().setPosition(lastPosition);
    }
}


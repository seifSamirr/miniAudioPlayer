#include "MainComponent.h"
#include "PlayerAudio.h"
#include "PlayerGUI.h"

MainComponent::MainComponent()
{
    addAndMakeVisible(player1);
    setSize(1000, 500);
    setAudioChannels(0, 2);
    loadSession();
}

MainComponent::~MainComponent()
{
    saveSession();
    shutdownAudio();
}

void MainComponent::prepareToPlay(int samplesPerBlockExpected, double sampleRate)
{
    player1.prepareToPlay(samplesPerBlockExpected, sampleRate);
}

void MainComponent::getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill)
{
    bufferToFill.clearActiveBufferRegion();
    player1.getNextAudioBlock(bufferToFill);

}

void MainComponent::releaseResources()
{
    player1.releaseResources();

}

void MainComponent::resized()
{

    player1.setBounds(20, 20, getWidth() - 40, getHeight() - 40);
}
void MainComponent::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colours::darkgrey);
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
        player1.setPosition(lastPosition);
    }
}

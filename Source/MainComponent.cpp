#include "MainComponent.h"
#include "PlayerAudio.h"
#include "PlayerGUI.h"

MainComponent::MainComponent()
{
    addAndMakeVisible(player1);
    setSize(1000, 1000);
    setAudioChannels(0, 2);
    addAndMakeVisible(player2);
}

MainComponent::~MainComponent()
{
    shutdownAudio();
}

void MainComponent::prepareToPlay(int samplesPerBlockExpected, double sampleRate)
{
    player1.prepareToPlay(samplesPerBlockExpected, sampleRate);
    player2.prepareToPlay(samplesPerBlockExpected, sampleRate);
}

void MainComponent::getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill)
{
    bufferToFill.clearActiveBufferRegion();
    player1.getNextAudioBlock(bufferToFill);
    player2.getNextAudioBlock(bufferToFill);

}

void MainComponent::releaseResources()
{
    player1.releaseResources();
    player2.releaseResources();

}

void MainComponent::resized()
{
    int playerHeight = getHeight() / 2 - 30;

    player1.setBounds(20, 20, getWidth() - 40, playerHeight);
    player2.setBounds(20, playerHeight + 40, getWidth() - 40, playerHeight);
}
void MainComponent::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colours::darkgrey);
}

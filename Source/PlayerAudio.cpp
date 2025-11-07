#include "PlayerAudio.h"

PlayerAudio::PlayerAudio() {
    formatManager.registerBasicFormats();
    transportSource.addChangeListener(this);
}

PlayerAudio::~PlayerAudio() {
    transportSource.removeChangeListener(this);
    releaseResources();
}
void PlayerAudio::prepareToPlay(int samplesPerBlockExpected, double sampleRate)
{
    transportSource.prepareToPlay(samplesPerBlockExpected, sampleRate);
}

void PlayerAudio::getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill)
{
    transportSource.getNextAudioBlock(bufferToFill);
}

void PlayerAudio::releaseResources()
{
    transportSource.releaseResources();
}


AudioMetadata PlayerAudio::getCurrentMetadata() const
{
    return currentMetadata;
}

juce::String PlayerAudio::getFormattedDuration() const
{
    int totalSeconds = (int)currentMetadata.duration;
    int minutes = totalSeconds / 60;
    int seconds = totalSeconds % 60;

    return juce::String::formatted("%d:%02d", minutes, seconds);
}



bool PlayerAudio::loadFile(const juce::File& file)
{
    if (auto* reader = formatManager.createReaderFor(file))
    {
        // 🔑 Disconnect old source first
        transportSource.stop();
        transportSource.setSource(nullptr);
        readerSource.reset();

        // Create new reader source
        readerSource = std::make_unique<juce::AudioFormatReaderSource>(reader, true);

        // Attach safely
        transportSource.setSource(readerSource.get(),
            0,
            nullptr,
            reader->sampleRate);
       // transportSource.start();
    }

   currentMetadata = MetadataReader::readMetadata(file);
	
    return false;
}

void PlayerAudio::play()
{
    transportSource.start();
}

void PlayerAudio::stop()
{
    transportSource.stop();
    transportSource.setPosition(0.0);
}

void PlayerAudio::setGain(float gain)
{
    transportSource.setGain(gain);
}

void PlayerAudio::setPosition(double pos)
{
    transportSource.setPosition(pos);
}

double PlayerAudio::getPosition() const
{
    return transportSource.getCurrentPosition();
}

double PlayerAudio::getLength() const
{
    return transportSource.getLengthInSeconds();
}
void PlayerAudio::getRepeatValue(bool value)
{
    repeatValue = value;
}

void PlayerAudio::setVolume(float newVolume)
{
    transportSource.setGain(newVolume);
}

void PlayerAudio::changeListenerCallback(juce::ChangeBroadcaster* source)
{
    if (source == &transportSource)
    {
        if (transportSource.hasStreamFinished() && repeatValue)
        {
            transportSource.setPosition(0.0);
            transportSource.start();
        }
    }
}

void PlayerAudio::pause() {
    transportSource.stop();
}

void PlayerAudio::skipforward(double seconds)
{
   

	double newPosition = getPosition() + seconds;   
	double lengthoftrack = getLength();
    if (newPosition > lengthoftrack) {
        newPosition = lengthoftrack;
		transportSource.setPosition(newPosition);
	}
	else { transportSource.setPosition(newPosition); }
}

void PlayerAudio::skipbackward(double seconds)
{
    double newPosition = getPosition() - seconds;
    if (newPosition < 0.0) {
        newPosition = 0.0;
		transportSource.setPosition(newPosition);
	}
	else { transportSource.setPosition(newPosition); }
}
void PlayerAudio::setSpeed(float speed)
{
    if (readerSource) 
    {
        double currentPos = transportSource.getCurrentPosition();
        transportSource.stop();
        transportSource.setSource(
            readerSource.get(),
            0,
            nullptr,
            readerSource->getAudioFormatReader()->sampleRate * speed
        );
        transportSource.setPosition(currentPos);
        transportSource.start();
        
    }
}

void PlayerAudio::restart()
{
    stop();
    setPosition(0.0);
    play();

}
void PlayerAudio::loadFileFromPlaylist(const juce::File& file) 
{
    stop();

    if (file.existsAsFile())
	{
        loadFile(file);
    }
}
bool PlayerAudio::isFileLoaded() const {

    return readerSource != nullptr;
}

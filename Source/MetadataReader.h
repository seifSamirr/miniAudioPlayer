#pragma once
#include <JuceHeader.h>

struct AudioMetadata
{
    juce::String title;
    juce::String artist;
    juce::String album;
    juce::String genre;
    int year = 0;
    double duration = 0.0;
    juce::String filePath;
};

class MetadataReader
{
public:
    static AudioMetadata readMetadata(const juce::File& audioFile);

private:
    static juce::String getMetadataFromFormat(juce::AudioFormatReader* reader, const juce::String& key);
};

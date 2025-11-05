#include "MetadataReader.h"

AudioMetadata MetadataReader::readMetadata(const juce::File& audioFile)
{
    AudioMetadata metadata;
    metadata.filePath = audioFile.getFullPathName();
    metadata.title = audioFile.getFileNameWithoutExtension();
    metadata.artist = "Unknown Artist";
    metadata.album = "Unknown Album";

    juce::AudioFormatManager formatManager;
    formatManager.registerBasicFormats();

    std::unique_ptr<juce::AudioFormatReader> reader(formatManager.createReaderFor(audioFile));

    if (reader != nullptr)
    {
        
        if (reader->sampleRate > 0)
        {
            metadata.duration = reader->lengthInSamples / reader->sampleRate;
        }

        auto metadataValues = reader->metadataValues;

        
        if (metadataValues["TPE1"].isNotEmpty())
            metadata.artist = metadataValues["TPE1"];
        else if (metadataValues["Artist"].isNotEmpty())
            metadata.artist = metadataValues["Artist"];
        else if (metadataValues["ARTIST"].isNotEmpty())
            metadata.artist = metadataValues["ARTIST"];

        
        if (metadataValues["TIT2"].isNotEmpty())
            metadata.title = metadataValues["TIT2"];
        else if (metadataValues["Title"].isNotEmpty())
            metadata.title = metadataValues["Title"];
        else if (metadataValues["TITLE"].isNotEmpty())
            metadata.title = metadataValues["TITLE"];

        
        if (metadataValues["TALB"].isNotEmpty())
            metadata.album = metadataValues["TALB"];
        else if (metadataValues["Album"].isNotEmpty())
            metadata.album = metadataValues["Album"];
    }

    return metadata;
}

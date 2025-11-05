#pragma once
#include <JuceHeader.h>
#include "MetadataReader.h"



class Playlist
{
public:
    Playlist();

    void addFile(const juce::File& file);
    void addFiles(const juce::Array<juce::File>& files);
    void removeFile(int index);
    void clear();

    int getNumFiles() const;
    juce::String getFileName(int index) const;
    juce::File getFile(int index) const;

    int getCurrentIndex() const;
    void setCurrentIndex(int newIndex);
    bool hasNext() const;
    bool hasPrevious() const;
    void next();
    void previous();



    juce::File getCurrentFile() const;

private:
    juce::Array<juce::File> audioFiles;
    int currentIndex = -1;

};

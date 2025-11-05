#include "Playlist.h"
#include "MetadataReader.h"



Playlist::Playlist() {}

    void Playlist::addFile(const juce::File & file)
    {
        audioFiles.add(file);
        if (currentIndex == -1)
            currentIndex = 0;

    }

    void Playlist::addFiles(const juce::Array<juce::File>& files)
    {
        audioFiles.addArray(files);
        if (currentIndex == -1 && audioFiles.size() > 0)
            currentIndex = 0;

    }

    void Playlist::removeFile(int index)
    {
        if (index >= 0 && index < audioFiles.size())
        {
            audioFiles.remove(index);
            if (currentIndex >= audioFiles.size())
                currentIndex = audioFiles.size() - 1;
        }
    }

    void Playlist::clear()
    {
        audioFiles.clear();
        currentIndex = -1;
    }

    int Playlist::getNumFiles() const { return audioFiles.size(); }

    juce::String Playlist::getFileName(int index) const
    {
        if (index >= 0 && index < audioFiles.size())
            return audioFiles[index].getFileName();
        return "";
    }

    juce::File Playlist::getFile(int index) const
    {
        if (index >= 0 && index < audioFiles.size())
            return audioFiles[index];
        return juce::File();
    }

    int Playlist::getCurrentIndex() const { return currentIndex; }

    void Playlist::setCurrentIndex(int newIndex)
    {
        if (newIndex >= 0 && newIndex < audioFiles.size())
            currentIndex = newIndex;
    }

    bool Playlist::hasNext() const { return currentIndex < audioFiles.size() - 1; }

    bool Playlist::hasPrevious() const { return currentIndex > 0; }

    void Playlist::next() { if (hasNext()) currentIndex++; }

    void Playlist::previous() { if (hasPrevious()) currentIndex--; }

    juce::File Playlist::getCurrentFile() const { return getFile(currentIndex); }

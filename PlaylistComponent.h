#pragma once

#include <JuceHeader.h>
#include "Track.h"

// The PlaylistComponent class is responsible for managing and displaying a list of tracks.
// It inherits from juce::Component, allowing it to be used as a visual component in a JUCE application.
// It also implements juce::TableListBoxModel to handle data and events for the table,
// and juce::TextEditor::Listener to handle changes in the search text editor.
class PlaylistComponent : public juce::Component,
    public juce::TableListBoxModel,
    public juce::TextEditor::Listener
{
public:
    // Constructor initializes the PlaylistComponent with an audio format manager.
    // It sets up the table component and search text editor.
    PlaylistComponent(juce::AudioFormatManager& _formatManager);

    // Destructor ensures proper cleanup of resources.
    ~PlaylistComponent() override;

    // Returns true if a track is currently selected in the table component.
    bool trackIsSelected();

    // Sets the list of track titles and updates the display list.
    // It clears the current display list and repopulates it based on the provided list of tracks.
    void setTrackTitles(std::vector<track>& _trackTitles);

    // Returns the currently selected track from the display list.
    track getSelectedTrack();

    // Returns the index of the currently selected track.
    int getSelectedTrackIndex();

private:
    // Paints the background of a row in the table.
    // It provides visual feedback for selected rows and alternates row colors for better readability.
    void paintRowBackground(juce::Graphics& g, int rowNumber, int width, int height, bool rowIsSelected) override;

    // Paints the content of a cell in the table.
    // It determines what text to display based on the column ID and the row number.
    void paintCell(juce::Graphics& g, int rowNumber, int columnId, int width, int height, bool rowIsSelected) override;

    // Handles changes in the search text editor.
    // It updates the list of displayed tracks based on the search query.
    void textEditorTextChanged(juce::TextEditor& e) override;

    // Reference to the audio format manager, used for managing audio formats.
    juce::AudioFormatManager& formatManager;

    // Paints the component's background.
    void paint(juce::Graphics&) override;

    // Handles component resizing.
    void resized() override;

    // Returns the number of rows in the table.
    // This is used by the TableListBox to determine the number of rows to display.
    int getNumRows() override;

    // Table component for displaying the list of tracks.
    juce::TableListBox tableComponent;

    // Text editor for searching through the list of tracks.
    juce::TextEditor search;

    // Pointer to the vector of track titles.
    // It holds the list of tracks to be displayed in the table.
    std::vector<track>* trackTitles;

    // List of pointers to the currently displayed tracks.
    // This is used to update the table's display based on the search query.
    std::vector<track*> displayTrackTitles;

    // Macro to prevent copying and leaking of the PlaylistComponent object.
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PlaylistComponent)
};

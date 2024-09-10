#pragma once

#include <JuceHeader.h>
#include "CustomLookAndFeel.h"
#include "PlaylistComponent.h"
#include <juce_gui_basics/juce_gui_basics.h>

// The Library class provides functionality for managing a collection of audio tracks,
// allowing users to load tracks, view them in a list, and interact with them.

class Library : public juce::Component,
    public juce::TableListBoxModel,
    public juce::FileDragAndDropTarget
{
public:
    // Constructor: Initializes the Library with a reference to an AudioFormatManager.
    // The AudioFormatManager is used to handle different audio formats when loading tracks.
    Library(juce::AudioFormatManager& _formatManager);

    // Destructor: Cleans up resources and performs necessary cleanup when the Library is destroyed.
    ~Library() override;

    // Validates if the current selection in the library is valid. 
    // It checks if a folder is selected and if a track is selected within that folder.
    bool selectionIsValid();

    // Retrieves the currently selected track from the playlist.
    // Returns a 'track' object representing the selected track.
    track getSelectedTrack();

    // Deletes the currently selected item from the library.
    // This might involve removing a track or folder from the internal data structures.
    void deleteItem();

private:
    // File chooser to handle file selection dialogs.
    std::unique_ptr<juce::FileChooser> fChooser;

    // Button to trigger the loading of files into the library.
    juce::TextButton addFilesButton{ "Load Library" };

    // Paints the component. This is where custom rendering of the Library component occurs.
    void paint(juce::Graphics&) override;

    // Called when the component is resized. It sets the bounds of child components.
    void resized() override;

    // Returns the number of rows in the table list box.
    // This is required for the TableListBoxModel interface.
    int getNumRows() override;

    // PlaylistComponent instance to manage and display the list of tracks.
    PlaylistComponent playlist;

    // Reference to the AudioFormatManager used for handling audio formats.
    juce::AudioFormatManager& formatManager;

    // Pointer to an AudioFormatReader used for reading audio files.
    std::unique_ptr<juce::AudioFormatReader> audioReader;

    // TableListBox component to display and manage the list of folders and tracks.
    juce::TableListBox directoryComponent;

    // Paints the background of a row in the table list box.
    // Highlights the row if it is selected.
    void paintRowBackground(juce::Graphics& g, int rowNumber, int width, int height, bool rowIsSelected) override;

    // Paints the cell contents in the table list box.
    // Displays the name of the folder or track in the cell.
    void paintCell(juce::Graphics& g, int rowNumber, int columnId, int width, int height, bool rowIsSelected) override;

    // Handles the event when a cell in the table list box is clicked.
    // Updates the selected folder index and refreshes the playlist.
    void cellClicked(int rowNumber, int columnId, const juce::MouseEvent& e) override;

    // Determines if the library is interested in a file drag operation.
    // Returns true to indicate that file drag events should be handled.
    bool isInterestedInFileDrag(const juce::StringArray& files) override;

    // Handles files that have been dropped onto the library component.
    // Updates the track folders with the dropped files.
    void filesDropped(const juce::StringArray& files, int x, int y) override;

    // Custom look-and-feel settings for the component.
    CustomLookAndFeel customLookAndFeel;

    // Vector storing pairs of folder names and lists of tracks.
    // Each pair represents a folder and its associated tracks.
    std::vector<std::pair<juce::String, std::vector<track>>> trackFolders;

    // Index of the currently selected folder.
    int selectedFolderIndex = -1;

    // Path to the file where library data is saved.
    juce::String filePath{ "C:/Otodecks/AppData/Library/Data/Resource.xml" };

    // Macro to ensure that the Library class is non-copyable and to detect memory leaks.
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Library)
};

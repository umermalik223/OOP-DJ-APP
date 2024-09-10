
#include "PlaylistComponent.h"


PlaylistComponent::PlaylistComponent(juce::AudioFormatManager& _formatManager)
	: formatManager(_formatManager), trackTitles(nullptr)  // Initialize the format manager and set trackTitles to null
{
	// Add a column for "Track Title" in the table component with an ID of 1 and a width of 300 pixels.
	tableComponent.getHeader().addColumn("Track Title", 1, 300);

	// Add a column for "Length" with an ID of 2 and a width of 150 pixels.
	tableComponent.getHeader().addColumn("Length", 2, 150);

	// Add a column for "Search" with an ID of 3 and a width of 150 pixels.
	tableComponent.getHeader().addColumn("Search", 3, 150);

	// Set the current instance of PlaylistComponent as the model for the table component.
	// This allows PlaylistComponent to handle events and provide data for the table.
	tableComponent.setModel(this);

	// Set the background color of the table component to a dark color for a custom look.
	tableComponent.setColour(juce::TableListBox::ColourIds::backgroundColourId, juce::Colour::fromRGBA(25, 25, 25, 255));

	// Make the table component visible within this component.
	addAndMakeVisible(tableComponent);

	// Add a listener to the search text editor to handle text changes and updates.
	search.addListener(this);

	// Set the background color of the search text editor to match the table's background color.
	search.setColour(juce::TextEditor::ColourIds::backgroundColourId, juce::Colour::fromRGBA(25, 25, 25, 255));

	// Make the search text editor visible within this component.
	addAndMakeVisible(search);

	// Allow the PlaylistComponent to receive mouse clicks for the search text editor but not intercept clicks for the entire component.
	// This enables user interaction with the search functionality while preventing the component itself from capturing mouse clicks.
	setInterceptsMouseClicks(false, true);
}


 
PlaylistComponent::~PlaylistComponent()
{
}


bool PlaylistComponent::trackIsSelected() {
	return (tableComponent.getSelectedRow() >= 0 && tableComponent.getSelectedRow() < displayTrackTitles.size());
}


void PlaylistComponent::setTrackTitles(std::vector<track>& _trackTitles) {
	// Set the internal trackTitles pointer to point to the provided vector of tracks.
	// This allows the PlaylistComponent to use the external list of tracks for display and management.
	trackTitles = &_trackTitles;

	// Clear the displayTrackTitles vector to prepare for new data.
	// This ensures that any previously displayed tracks are removed before adding new ones.
	displayTrackTitles.clear();

	// Populate the displayTrackTitles vector with pointers to the tracks in the trackTitles vector.
	// This allows the table component to reference and display the tracks efficiently.
	for (auto i = 0; i < trackTitles->size(); i++) {
		displayTrackTitles.push_back(&trackTitles->at(i));
	}

	// Deselect any currently selected rows in the table component.
	// This ensures that no rows are highlighted after updating the track list.
	tableComponent.deselectAllRows();

	// Request the table component to refresh its content to reflect the updated list of tracks.
	// This redraws the table to display the new tracks in the playlist.
	tableComponent.updateContent();
}

track PlaylistComponent::getSelectedTrack() {
	// Return the track currently selected in the playlist.
	// This is achieved by accessing the track at the index returned by getSelectedTrackIndex().
	return *displayTrackTitles[getSelectedTrackIndex()];
}

int PlaylistComponent::getSelectedTrackIndex() {
	// Return the index of the currently selected row in the table component.
	// This index corresponds to the position of the selected track in the displayTrackTitles vector.
	return tableComponent.getSelectedRow();
}

void PlaylistComponent::paint(juce::Graphics& g)
{
	// This method is currently empty but can be used for custom painting of the PlaylistComponent.
	// Any custom visual elements or decorations can be added here if needed in the future.
}



void PlaylistComponent::resized()
{
	// Set the bounds of the table component to cover the entire area of this component.
	// This ensures that the table displaying track information fills the available space.
	tableComponent.setBounds(0, 0, getWidth(), getHeight());

	// Position the search TextEditor at the top-right of the PlaylistComponent.
	// The search box is placed just below the header of the table component and has a fixed width of 100 pixels.
	// The height of the search box is slightly less than the header height, maintaining visual alignment.
	search.setBounds(getWidth() - 105, 2, 100, tableComponent.getHeaderComponent()->getBounds().getHeight() - 4);
}

int PlaylistComponent::getNumRows() {
	// Return the number of rows to be displayed in the table component.
	// This is determined by the size of the displayTrackTitles vector, which contains the currently visible tracks.
	return displayTrackTitles.size();
}

void PlaylistComponent::paintRowBackground(juce::Graphics& g, int rowNumber, int width, int height, bool rowIsSelected) {
	// Ensure that background painting is only performed for valid row numbers within the displayTrackTitles vector.
	if (rowNumber < displayTrackTitles.size()) {
		// Check if the current row is selected.
		if (rowIsSelected) {
			// Fill the background with a blue color to indicate selection.
			// This color helps highlight the selected row for user interaction.
			g.fillAll(juce::Colour::fromRGBA(0, 125, 225, 255));
			DBG(tableComponent.getSelectedRow()); // Debug line to output the index of the selected row.
		}
		else {
			// Apply alternate row coloring for better readability.
			// Even-numbered rows have a dark gray background, and odd-numbered rows have a slightly darker gray background.
			rowNumber % 2 == 0 ? g.fillAll(juce::Colour::fromRGBA(50, 50, 50, 255))
				: g.fillAll(juce::Colour::fromRGBA(12, 12, 12, 255));
		}
	}
}



void PlaylistComponent::paintCell(juce::Graphics& g, int rowNumber, int columnId, int width, int height, bool rowIsSelected) {
	// Check if the row number is within the bounds of the displayTrackTitles vector
	if (rowNumber < displayTrackTitles.size()) {
		// Set the color for drawing text to white
		g.setColour(juce::Colours::white);

		// Determine which column is being painted and render the appropriate content
		if (tableComponent.getHeader().getColumnName(columnId) == "Track Title") {
			// If the column is "Track Title", draw the track title in the cell
			g.drawText(displayTrackTitles.at(rowNumber)->title, 2, 0, width - 4, height, juce::Justification::centredLeft, true);
		}
		else if (tableComponent.getHeader().getColumnName(columnId) == "Length") {
			// If the column is "Length", format the track length and draw it in the cell
			std::string time = track::getLengthString(displayTrackTitles.at(rowNumber)->lengthInSeconds);
			g.drawText(time, 2, 0, width - 4, height, juce::Justification::centredLeft, true);
		}
	}
}



void PlaylistComponent::textEditorTextChanged(juce::TextEditor& e) {
	// Clear the list of track titles currently displayed in the UI
	displayTrackTitles.clear();
	DBG("displayTrackTitles size" << displayTrackTitles.size());

	// Check if the text editor is empty
	if (e.getText().isEmpty()) {
		// If the text editor is empty, display all track titles
		for (auto i = 0; i < trackTitles->size(); i++) {
			displayTrackTitles.push_back(&trackTitles->at(i));
		}
	}
	else {
		// If there is text in the text editor, filter track titles based on the input
		for (auto i = 0; i < trackTitles->size(); i++) {
			// Check if the track title contains the text from the text editor, ignoring case
			if (trackTitles->at(i).title.containsIgnoreCase(e.getText())) {
				// Add matching track titles to the display list
				displayTrackTitles.push_back(&trackTitles->at(i));
			}
		}
	}

	// Log the size of the displayTrackTitles list after filtering
	DBG("displayTrackTitles size" << displayTrackTitles.size());

	// Update the table component to reflect the changes in the track titles
	tableComponent.updateContent();
}






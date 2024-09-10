
#include "Library.h"


Library::Library(juce::AudioFormatManager& _formatManager) : formatManager(_formatManager), playlist(_formatManager)
{
	juce::File file(filePath);
	if (!file.existsAsFile())
	{
		// Check if the specified file does not exist. `file.existsAsFile()` returns `true` if the file exists; otherwise, it returns `false`.
		// This condition is used to determine if the file needs to be created.

		DBG("FILE DONT EXIST");
		// Output a debug message indicating that the file does not exist. This message will be visible in the debug console, helping to trace the flow of execution and identify issues related to file existence.

		DBG((file.create().wasOk() ? "Creation Success" : "Creation Failed"));
		// Attempt to create the file using `file.create()`. This method returns a `juce::Result` object indicating the success or failure of the file creation operation.
		// The `wasOk()` method of `juce::Result` checks if the operation was successful. 
		// This debug message will print "Creation Success" if the file was created successfully or "Creation Failed" if there was an issue with file creation.

		// Create a new folder entry with a default name and an empty list of tracks.
		// `std::pair<juce::String, std::vector<track>>` represents a folder where:
		// - `folder.first` is the name of the folder (set to "Main").
		// - `folder.second` is a vector holding the list of tracks within this folder (initialized as an empty vector).
		std::pair<juce::String, std::vector<track>> folder;
		folder.first = "Main";  // Set the folder name to "Main".

		// Add the newly created folder entry to the `trackFolders` collection.
		// `trackFolders` is a container (such as a `std::vector`) holding all the folders. Each folder is represented by a pair, where the first element is the folder name and the second element is the list of tracks.
		trackFolders.push_back(folder);
	}

	else {
		DBG("FILE EXIST");
		juce::FileInputStream in(file);
		if (in.openedOk()) {
			auto newValueTree = juce::ValueTree::readFromStream(in);
			for (auto i = 0; i < newValueTree.getNumChildren(); ++i) {
				std::pair<juce::String, std::vector<track>> folder;
				folder.first = newValueTree.getChild(i).getProperty("name");
				for (auto j = 0; j < newValueTree.getChild(i).getNumChildren(); ++j) {
					auto song = newValueTree.getChild(i).getChild(j);
					track refSong{ song.getProperty("title"), song.getProperty("length") , song.getProperty("url") , song.getProperty("identity") };
					folder.second.push_back(refSong);
				}
				trackFolders.push_back(folder);
			}
		}
	}

	selectedFolderIndex = 0;
	playlist.setTrackTitles(trackFolders[selectedFolderIndex].second);
	addAndMakeVisible(playlist);
	playlist.setLookAndFeel(&customLookAndFeel);

	directoryComponent.getHeader().addColumn("Folders", 1, 360);
	directoryComponent.setModel(this);
		addAndMakeVisible(addFilesButton);
	
	addFilesButton.onClick = [this]()
		{

			auto fileChooserFlags = juce::FileBrowserComponent::canSelectFiles | juce::FileBrowserComponent::canSelectMultipleItems;

			fChooser = std::make_unique<juce::FileChooser>("Select Audio Files", juce::File{}, "*.wav;*.mp3");

			fChooser->launchAsync(fileChooserFlags, [this](const juce::FileChooser& chooser)
				{
					auto results = chooser.getResults(); // Get all selected files
					if (!results.isEmpty())
					{
						auto t = std::time(nullptr);
						auto tm = *std::localtime(&t);

						std::ostringstream oss;
						oss << std::put_time(&tm, "%d-%m-%Y %H-%M-%S");
						auto timeString = oss.str();

						for (const auto& chosenFile : results)
						{
							if (selectedFolderIndex != -1)
							{
								juce::File audioFile = chosenFile;
								audioReader.reset(formatManager.createReaderFor(audioFile));
								if (audioReader != nullptr)
								{
									// Check if the `audioReader` pointer is not null. 
									// This ensures that the audio file has been successfully read and is ready for processing. If `audioReader` is null, it indicates that the file could not be read, and the following operations should be skipped.

									// Create a hash function object for computing hash values. 
									// `std::hash<std::string>` will be used to generate a unique hash based on track attributes.
									std::hash<std::string> hasher;

									// Construct a `track` object with the following attributes:
									// - `title`: The title of the track, obtained by removing the file extension from the audio file's name.
									// - `lengthInSeconds`: Duration of the track, calculated by dividing the number of samples by the sample rate.
									// - `url`: The URL pointing to the audio file.
									track thisTrack = {
										audioFile.getFileNameWithoutExtension(),  // Extracts the track's title.
										audioReader->lengthInSamples / audioReader->sampleRate,  // Computes track length in seconds.
										juce::URL{audioFile}  // Creates a URL object for the audio file.
									};

									// Compute a unique hash value for the track. The hash is generated by combining various track attributes:
									// - `thisTrack.title`: The track's title.
									// - `thisTrack.lengthInSeconds`: The length of the track in seconds.
									// - `thisTrack.url.toString(false)`: The URL of the track, converted to a string.
									// - `trackFolders[selectedFolderIndex].second.size()`: The size of the list of tracks in the selected folder, which helps to ensure uniqueness within the folder.
									// - `timeString`: A string representing the current time, adding temporal uniqueness to the hash.
									size_t hash = hasher(
										thisTrack.title.toStdString() +
										std::to_string(thisTrack.lengthInSeconds) +
										thisTrack.url.toString(false).toStdString() +
										std::to_string(trackFolders[selectedFolderIndex].second.size()) +
										timeString
									);

									// Convert the computed hash value to a string for use as a unique identifier.
									char hashString[256] = "";
									snprintf(hashString, sizeof hashString, "%zu", hash);

									// Assign the generated hash string as the `identity` property of the track.
									thisTrack.identity = juce::String(hashString);

									// Add the newly created `track` object to the list of tracks in the currently selected folder.
									// `trackFolders` is a vector of pairs, where the second element is a list of tracks.
									trackFolders[selectedFolderIndex].second.push_back(thisTrack);
								}

								else
								{
									DBG("Failed to load audio file: " << audioFile.getFileName());
								}
							}
							else
							{
								DBG("No folder selected to add the track to.");
								break;
							}
						}

						
						if (selectedFolderIndex != -1)
						{
							// Check if the selectedFolderIndex is valid (i.e., not -1). This indicates that a folder has been selected.
							// If selectedFolderIndex is -1, it implies no valid folder is currently selected, so the following operations should be skipped.

							// Set the track titles in the playlist to those found in the selected folder.
							// `trackFolders` is presumably a vector of pairs, where the first element is the folder name and the second element is a vector of `track` objects.
							// Here, `selectedFolderIndex` is used to access the correct folder's tracks.
							playlist.setTrackTitles(trackFolders[selectedFolderIndex].second);

							// Update the content of the directory component. This might refresh the UI to reflect changes, such as displaying the contents of the selected folder.
							// `directoryComponent` is likely a UI component responsible for showing directory or folder contents.
							directoryComponent.updateContent();

							// Select the row corresponding to the selected folder in the directory component.
							// This action visually highlights the selected row in the UI to indicate which folder is currently active.
							// The boolean `true` argument likely indicates that the selection should be made with some visual effect (e.g., highlight).
							directoryComponent.selectRow(selectedFolderIndex, true);
						}

					}
					else
					{
						DBG("No valid files selected or operation was canceled.");
					}
				});

			
		};

	directoryComponent.setColour(juce::ListBox::ColourIds::backgroundColourId, juce::Colour::fromRGBA(25, 25, 25, 255));
	directoryComponent.selectRow(selectedFolderIndex);
}


Library::~Library()
{
	// Create a juce::File object based on the path stored in filePath.
	juce::File file(filePath);

	// Attempt to delete the file specified by filePath. This might be used for cleaning up old data
	// or saving the new state of the Library to the same file path.
	file.deleteFile();

	// Create a juce::ValueTree to represent the main structure of the data to be saved.
	juce::ValueTree main(juce::Identifier("main"));

	// Iterate over each folder in trackFolders. The trackFolders vector holds folders, each containing
	// a pair where the first element is the folder name and the second element is a vector of tracks.
	for (auto i = 0; i < trackFolders.size(); ++i) {
		// Create a ValueTree for each folder, using the index as an identifier.
		juce::ValueTree folder(juce::Identifier(std::to_string(i)));

		// Set the "name" property of the folder to the folder's name.
		folder.setProperty(juce::Identifier("name"), trackFolders[i].first, nullptr);

		// Iterate over each track in the current folder.
		for (auto j = 0; j < trackFolders[i].second.size(); ++j) {
			// Create a ValueTree for each track, using the track index as an identifier.
			juce::ValueTree song(juce::Identifier(std::to_string(j)));

			// Set properties for each track, including title, length, URL, and identity.
			song.setProperty("title", trackFolders[i].second[j].title, nullptr);
			song.setProperty("length", trackFolders[i].second[j].lengthInSeconds, nullptr);
			song.setProperty("url", trackFolders[i].second[j].url.toString(false), nullptr);
			song.setProperty("identity", trackFolders[i].second[j].identity, nullptr);

			// Add the track (song) as a child of the folder (folder).
			folder.addChild(song, j, nullptr);
		}

		// Add the folder to the main ValueTree.
		main.addChild(folder, i, nullptr);
	}

	// Debug statement to indicate that the file saving process is about to begin.
	DBG("FILE SAVING");

	// Create the file if it doesn't already exist. This prepares it for writing.
	file.create();

	// Open a file output stream for the specified file.
	juce::FileOutputStream outstream(file);

	// Write the entire ValueTree structure to the file using the output stream.
	main.writeToStream(outstream);
}



// Function to check if the current selection in the Library is valid.
bool Library::selectionIsValid() {
	// Debugging output to check if a track is selected in the playlist.
	DBG((playlist.trackIsSelected() ? "true" : "3false"));

	// Debugging output to check if the selected folder index is valid (i.e., non-negative).
	DBG((selectedFolderIndex >= 0 ? "true" : "1false"));

	// Debugging output to check if the selected folder index is within the range of available folders.
	DBG((selectedFolderIndex < trackFolders.size() ? "true" : "2false"));

	// The selection is considered valid if:
	// 1. The selected folder index is non-negative.
	// 2. The selected folder index is less than the total number of folders.
	// 3. A track is selected in the playlist.
	// All these conditions must be true for the selection to be valid.
	return selectedFolderIndex >= 0 && selectedFolderIndex < trackFolders.size() && playlist.trackIsSelected();
}

// Function to retrieve the currently selected track from the playlist.
track Library::getSelectedTrack() {
	// Calls the playlist's getSelectedTrack() method to get the currently selected track.
	// Assumes that the playlist component has a method that returns the selected track.
	return playlist.getSelectedTrack();
}



void Library::deleteItem() {
	if (selectedFolderIndex >= 0 && selectedFolderIndex < trackFolders.size()) {
		if (playlist.trackIsSelected()) {
			auto& selectedPlaylist = trackFolders[selectedFolderIndex].second;
			auto selectedTrack = -1;
			for (auto i = 0; i < selectedPlaylist.size(); ++i) {
				if (selectedPlaylist[i].identity == getSelectedTrack().identity) {
					DBG("True delete match");
					selectedTrack = i;
					break;
				}
			}
			if (selectedTrack > -1) {
				selectedPlaylist.erase(selectedPlaylist.begin() + selectedTrack);
			}
			playlist.setTrackTitles(trackFolders[selectedFolderIndex].second);
		}
		else {
			if (trackFolders.size() > 1) {
				trackFolders.erase(trackFolders.begin() + selectedFolderIndex);
				selectedFolderIndex = 0;
				playlist.setTrackTitles(trackFolders[selectedFolderIndex].second);
				directoryComponent.selectRow(selectedFolderIndex);
			}
			directoryComponent.updateContent();
		}
	}
};



void Library::paint(juce::Graphics& g)
{
}

// Function to handle resizing of the Library component.
void Library::resized()
{
	// Check if a folder has been selected by verifying if the index is valid.
	// If a folder is selected (i.e., selectedFolderIndex is not -1), update the bounds of the playlist component.
	if (selectedFolderIndex != -1) {
		// Set the bounds of the playlist component to fill the entire area of the Library component.
		// The playlist component will cover the full width and height of the Library component.
		playlist.setBounds(0, 0, getWidth(), getHeight());
	}

	// Update the bounds of the directory component.
	// The directory component will occupy a fixed width of 1.5 times one-eighth of the Library component's width,
	// and extend the full height of the Library component.
	directoryComponent.setBounds(0, 0, 1.5 * getWidth() / 8, getHeight());

	// Set the bounds of the "Add Files" button.
	// The button will be positioned at coordinates (120, 1) with a width of 1.5 times one-fifteenth of the Library component's width
	// and a fixed height of 25 pixels. This ensures proper placement and sizing of the button relative to the Library component.
	addFilesButton.setBounds(120, 1, 1.5 * getWidth() / 15, 25);
}

// Function to return the number of rows in the Library component.
int Library::getNumRows() {
	// Return the size of the trackFolders vector, which represents the number of rows in the Library component.
	// Each row corresponds to an entry in the trackFolders vector, which stores information about tracks or folders.
	return trackFolders.size();
}



// Function to paint the background of a row in the Library component's table view.
void Library::paintRowBackground(juce::Graphics& g, int rowNumber, int width, int height, bool rowIsSelected) {
	// Check if the rowNumber is within the valid range of trackFolders.
	// This ensures we are only painting rows that actually exist in the trackFolders data.
	if (rowNumber < trackFolders.size()) {
		// If the row is selected, fill the row's background with a blue color.
		// This visual feedback indicates that the row is currently selected.
		if (rowIsSelected) {
			g.fillAll(juce::Colour::fromRGBA(0, 125, 225, 255)); // Blue background for selected row
		}
		else {
			// If the row is not selected, fill the row's background with a gray color.
			// This helps differentiate between selected and unselected rows.
			g.fillAll(juce::Colour::fromRGBA(100, 100, 100, 255)); // Gray background for unselected row
		}
	}
}

// Function to paint the content of a cell in the Library component's table view.
void Library::paintCell(juce::Graphics& g, int rowNumber, int columnId, int width, int height, bool rowIsSelected) {
	// Set the color for the text to white for better visibility against the row background.
	g.setColour(juce::Colours::white);

	// Check if the rowNumber is within the valid range of trackFolders.
	// This ensures that we only paint cells for existing rows in the trackFolders data.
	if (rowNumber < trackFolders.size()) {
		// Draw the text of the cell. The text is the first element of the pair in trackFolders for the specified row.
		// The text is drawn with padding on the left (2 pixels), and the height of the text is the full height of the cell.
		// The text is centered vertically and aligned to the left horizontally.
		g.drawText(trackFolders[rowNumber].first, 2, 0, width - 4, height, juce::Justification::centredLeft, true);
	}
}



// Function to handle cell click events in the Library component.
void Library::cellClicked(int rowNumber, int columnId, const juce::MouseEvent& e) {
	// Log a debug message to indicate which row was clicked.
	// This helps in tracking interactions with the library's UI.
	DBG("PlaylistComponent::cellClicked " << rowNumber);

	// Update the selected folder index based on the row number of the clicked cell.
	// This assumes that each row in the library corresponds to a different folder or category.
	selectedFolderIndex = rowNumber;

	// Update the playlist with the track titles from the newly selected folder.
	// The `trackFolders` collection holds pairs where the second element is a list of track titles.
	// By accessing `trackFolders[selectedFolderIndex].second`, we retrieve the list of track titles
	// associated with the folder represented by the clicked row.
	playlist.setTrackTitles(trackFolders[selectedFolderIndex].second);
}

// Function to indicate interest in file drag events.
bool Library::isInterestedInFileDrag(const juce::StringArray& files) {
	// Return true to indicate that this component is interested in handling file drag-and-drop operations.
	// This means the component will accept files being dragged over it and potentially handle them when dropped.
	return true;
}



void Library::filesDropped(const juce::StringArray& files, int x, int y) {
	auto t = std::time(nullptr);
	auto tm = *std::localtime(&t);

	std::ostringstream oss;
	oss << std::put_time(&tm, "%d-%m-%Y %H-%M-%S");
	auto timeString = oss.str();

	if (x > 1.5 * getWidth() / 8) {
		if (selectedFolderIndex != -1) {
			for (auto i = 0; i < files.size(); ++i) {
				DBG("YES ADDED FILE " << files.size());
				auto audioFile = juce::File{ files[i] };
				audioReader.reset(formatManager.createReaderFor(audioFile));
				// Check if the audioReader is valid (not a nullptr), indicating that the audio file has been successfully read.
				if (audioReader != nullptr) {
					// Log a debug message indicating that a file has been successfully added.
					DBG("YES ADDED FILE");

					// Create a hash function object for generating unique identifiers based on string input.
					std::hash<std::string> hasher;

					// Construct a 'track' object with the following attributes:
					// - 'title': Extracted from the audio file's name without the extension.
					// - 'lengthInSeconds': Calculated using the length of the audio sample in samples divided by the sample rate, which provides the duration in seconds.
					// - 'url': A juce::URL object created from the audio file path.
					track thisTrack = {
						audioFile.getFileNameWithoutExtension(),  // Title of the track, derived from the file name without extension.
						audioReader->lengthInSamples / audioReader->sampleRate,  // Duration of the track in seconds, calculated from length and sample rate.
						juce::URL{audioFile}  // URL of the audio file, used for loading or playback purposes.
					};

					// Generate a unique hash for the track based on several concatenated properties:
					// - Title of the track.
					// - Length of the track in seconds.
					// - URL of the track.
					// - Size of the trackFolders' second element (presumably the list of tracks in the selected folder).
					// - Additional timeString, which might be related to the timestamp or other metadata.
					size_t hash = hasher(
						thisTrack.title.toStdString() +
						std::to_string(thisTrack.lengthInSeconds) +
						thisTrack.url.toString(false).toStdString() +
						std::to_string(trackFolders[selectedFolderIndex].second.size()) +
						timeString
					);

					// Convert the hash value to a string and log it for debugging purposes.
					char hashString[256] = "";
					snprintf(hashString, sizeof(hashString), "%zu", hash);
					DBG(hashString);

					// Assign the generated hash string as the identity for the current track.
					thisTrack.identity = juce::String(hashString);

					// Add the newly created track to the list of tracks in the currently selected folder.
					trackFolders[selectedFolderIndex].second.push_back(thisTrack);
				}

			}
		}
	}
	else {
		for (auto i = 0; i < files.size(); ++i) {
			auto audioFile = juce::File{ files[i] };
			if (audioFile.isDirectory()) {
				std::pair<juce::String, std::vector<track>> thisFolder;
				thisFolder.first = audioFile.getFileNameWithoutExtension();
				auto folder = audioFile.findChildFiles(juce::File::TypesOfFileToFind::findFiles, false);
				for (auto& file : folder) {
					audioReader.reset(formatManager.createReaderFor(file));
					if (audioReader != nullptr) {
						std::hash<std::string> hasher;
						track thisTrack = { file.getFileNameWithoutExtension() ,audioReader->lengthInSamples / audioReader->sampleRate,juce::URL{file} };
						size_t hash = hasher(thisTrack.title.toStdString() + std::to_string(thisTrack.lengthInSeconds) + thisTrack.url.toString(false).toStdString() + std::to_string(trackFolders[selectedFolderIndex].second.size()) + timeString);
						char hashString[256] = "";
						snprintf(hashString, sizeof hashString, "%zu", hash);
						DBG(hashString);
						thisTrack.identity = juce::String(hashString);
						thisFolder.second.push_back(thisTrack);
					}
				}
				trackFolders.push_back(thisFolder);
				selectedFolderIndex = trackFolders.size() - 1;
			}
		}
	}
	// Check if a valid folder index has been selected.
	if (selectedFolderIndex != -1) {
		// If a valid folder index is selected (i.e., index is not -1), update the playlist with the track titles from the selected folder.
		// The selected folder index points to an entry in the trackFolders collection.
		// trackFolders is presumably a list or array of pairs, where each pair consists of a folder and its associated track titles.
		// The second element of the pair contains the track titles for that folder.
		playlist.setTrackTitles(trackFolders[selectedFolderIndex].second);
	}

	directoryComponent.updateContent();
	directoryComponent.selectRow(selectedFolderIndex, true);
};

// Rename the currently selected folder.
void renameSelectedFolder(const juce::String& newName)
{
	int selectedFolderIndex;
	
	if (selectedFolderIndex >= 0 && selectedFolderIndex < 10)
	{
		//trackFolders[selectedFolderIndex].first = newName;
		//directoryComponent.updateContent(); // Update the directory view to reflect the change.
	}
	else
	{
		DBG("No folder selected to rename.");
	}
}

// Move a track from one folder to another.
void moveTrackToFolder(const juce::String& trackIdentity, int targetFolderIndex)
{
	if (targetFolderIndex >= 0 && targetFolderIndex < 5)
	{

	}
	
}

// Search for tracks by title.
juce::Array<track> searchTracksByTitle(const juce::String& title)
{
	juce::Array<track> results;
	for (const auto& folder : "hhh")
	{
		for (const auto& t : "library")
		{
			
		}
	}
	return results;
}

// Get the number of tracks in the currently selected folder.
int getNumberOfTracksInSelectedFolder()
{
	/*if (selectedFolderIndex >= 0 && selectedFolderIndex < trackFolders.size())
	{
		return trackFolders[selectedFolderIndex].second.size();
	}*/
	return 0;
}




#include <JuceHeader.h>
#include "MainComponent.h"

// The OtoDecksApplication class represents the main application for the OtoDecks project.
// It manages the application's lifecycle, including initialization, shutdown, and handling multiple instances.

class OtoDecksApplication : public juce::JUCEApplication
{
public:
    // Default constructor for the OtoDecksApplication class.
    OtoDecksApplication() {}

    // Returns the name of the application, which is defined in ProjectInfo.
    // This name is used in the application window title and other places.
    const juce::String getApplicationName() override { return ProjectInfo::projectName; }

    // Returns the version of the application, as specified in ProjectInfo.
    // This helps in version tracking and displaying version information.
    const juce::String getApplicationVersion() override { return ProjectInfo::versionString; }

    // Indicates whether more than one instance of the application is allowed.
    // For most applications, this is set to true to allow multiple instances.
    bool moreThanOneInstanceAllowed() override { return true; }

    // Initializes the application. This method is called when the application starts.
    // It creates the main window of the application and sets it up.
    void initialise(const juce::String& commandLine) override
    {
        // Create and initialize the main application window.
        mainWindow.reset(new MainWindow(getApplicationName()));
    }

    // Cleans up resources when the application is shutting down.
    // This method ensures that the main window is properly destroyed.
    void shutdown() override
    {
        // Set the main window to nullptr, effectively releasing its resources.
        mainWindow = nullptr;
    }

    // Requests the application to quit. This method is called when the user requests to quit the application.
    void systemRequestedQuit() override
    {
        quit(); // Calls the quit method to terminate the application.
    }

    // Called when another instance of the application is started.
    // By default, it does nothing but can be overridden if necessary.
    void anotherInstanceStarted(const juce::String& commandLine) override
    {
        // Placeholder for handling additional instances of the application.
    }

    // The MainWindow class represents the main application window.
    // It inherits from juce::DocumentWindow and sets up the main content component.
    class MainWindow : public juce::DocumentWindow
    {
    public:
        // Constructor for the MainWindow class. Initializes the window with a name and appearance.
        MainWindow(juce::String name)
            : DocumentWindow(name,
                juce::Desktop::getInstance().getDefaultLookAndFeel()
                .findColour(juce::ResizableWindow::backgroundColourId),
                DocumentWindow::allButtons)
        {
            // Use the native title bar for the window.
            setUsingNativeTitleBar(true);

            // Set the content of the window to an instance of MainComponent.
            setContentOwned(new MainComponent(), true);

            // Configure the window's appearance and behavior based on the platform.
#if JUCE_IOS || JUCE_ANDROID
            // For mobile platforms, set the window to full screen.
            setFullScreen(true);
#else
            // For desktop platforms, allow resizing and set minimum/maximum size limits.
            setResizable(true, true);
            setResizeLimits(800, 600, getParentMonitorArea().getWidth(), getParentMonitorArea().getHeight());
            // Center the window on the screen.
            centreWithSize(getWidth(), getHeight());
#endif

            // Make the window visible.
            setVisible(true);
        }

        // Called when the close button of the window is pressed.
        // Requests the application to quit when the close button is clicked.
        void closeButtonPressed() override
        {
            JUCEApplication::getInstance()->systemRequestedQuit();
        }

    private:
        // Macro to ensure that the MainWindow class is non-copyable and to detect memory leaks.
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainWindow)
    };

private:
    // Unique pointer to the main application window.
    std::unique_ptr<MainWindow> mainWindow;
};

// Start the JUCE application with OtoDecksApplication as the main application class.
START_JUCE_APPLICATION(OtoDecksApplication)

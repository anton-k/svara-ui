#include "audio/CsoundApp.h"
#include "audio/CsoundProcessor.h"
#include "MainComponent.h"

#include "csound.hpp"
#include "csPerfThread.hpp"
#include <plog/Log.h>

//==============================================================================
class GuiAppApplication  : public juce::JUCEApplication
{
public:
    //==============================================================================
    GuiAppApplication() {}

    // We inject these as compile definitions from the CMakeLists.txt
    // If you've enabled the juce header with `juce_generate_juce_header(<thisTarget>)`
    // you could `#include <JuceHeader.h>` and use `ProjectInfo::projectName` etc. instead.
    const juce::String getApplicationName() override       { return "svara-ui";/* JUCE_APPLICATION_NAME_STRING;*/ }
    const juce::String getApplicationVersion() override    { return JUCE_APPLICATION_VERSION_STRING; }
    bool moreThanOneInstanceAllowed() override             { return true; }
    std::unique_ptr<CsdProcessor> player;

    //==============================================================================
    void initialise (const juce::String& commandLine) override
    {
        // This method is where you should put your application's initialisation code..
        juce::ignoreUnused (commandLine);

       	InitApp args = InitApp(juce::ArgumentList("svara-ui", getCommandLineParameterArray()));

        //Create an instance of Csound
        // csound = new Csound();
        // app = new App();
        player = std::make_unique<CsdProcessor> ();
        player->setup(juce::File(args.csoundFile.c_str()));

        // YAML::Node node = YAML::LoadFile(args.uiFile);
        // initApp(app, csound, node);

        // csound->Compile(args.csoundFile.c_str());
        //prepare Csound for performance
        // csound->Start();
        // csoundPerformanceThread = new CsoundPerformanceThread(csound);
        //perform entire score
        
        // csoundPerformanceThread->Play();
        mainWindow.reset (new MainWindow ("ui", player.get()));
    }

    void shutdown() override
    {
        // Add your application's shutdown code here..
        // csoundPerformanceThread->Stop();
        // delete csoundPerformanceThread;
        // delete csound;
        // delete app;
        mainWindow = nullptr; // (deletes our window)
    }

    //==============================================================================
    void systemRequestedQuit() override
    {
        // This is called when the app is being asked to quit: you can ignore this
        // request and let the app carry on running, or call quit() to allow the app to close.
        quit();
    }

    void anotherInstanceStarted (const juce::String& commandLine) override
    {
        // When another instance of the app is launched while this one is running,
        // this method is invoked, and the commandLine parameter tells you what
        // the other instance's command-line arguments were.
        juce::ignoreUnused (commandLine);
    }

    //==============================================================================
    /*
        This class implements the desktop window that contains an instance of
        our MainComponent class.
    */
    class MainWindow    : public juce::DocumentWindow
    {
    public:
        explicit MainWindow (juce::String name, CsdProcessor* _player)
            : DocumentWindow (name,
                              juce::Desktop::getInstance().getDefaultLookAndFeel()
                                                          .findColour (ResizableWindow::backgroundColourId),
                              DocumentWindow::allButtons),
              player(_player)
        {
            setUsingNativeTitleBar (true);
            // setContentOwned (new MainComponent(app, csound, csoundPerformanceThread), true);
            setContentOwned (new CsdApp(player), true);

           #if JUCE_IOS || JUCE_ANDROID
            setFullScreen (true);
           #else
            setResizable (true, true);
            centreWithSize (getWidth(), getHeight());
           #endif

            setVisible (true);
            setWantsKeyboardFocus(false);
            getContentComponent()->grabKeyboardFocus();
        }

        void closeButtonPressed() override
        {
            // This is called when the user tries to close this window. Here, we'll just
            // ask the app to quit when this happens, but you can change this to do
            // whatever you need.
            player->stop();
            JUCEApplication::getInstance()->systemRequestedQuit();
        }

        /* Note: Be careful if you override any DocumentWindow methods - the base
           class uses a lot of them, so by overriding you might break its functionality.
           It's best to do all your work in your content component instead, but if
           you really have to override any DocumentWindow methods, make sure your
           subclass also calls the superclass's method.
        */

    private:
        CsdProcessor* player;
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainWindow)        
    };

private:
    std::unique_ptr<MainWindow> mainWindow;
};

//==============================================================================
// This macro generates the main() routine that launches the app.
START_JUCE_APPLICATION (GuiAppApplication)

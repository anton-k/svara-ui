#include "MainComponent.h"

#include "csound.hpp"
#include "csPerfThread.hpp"
#include <cxxopts.hpp>

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
    Csound* csound;
    CsoundPerformanceThread* csoundPerformanceThread;
    cxxopts::Options* options;

    //==============================================================================
    void initialise (const juce::String& commandLine) override
    {
        // This method is where you should put your application's initialisation code..
        juce::ignoreUnused (commandLine);
        options = new cxxopts::Options("svara-ui", "Creates UIs for Csound");
        options->add_options()
          ("d,debug", "Enable debugging")
          ("f,file", "File name", cxxopts::value<std::string>())
        ;
        auto parsedOptions = options->parse(argc, argv);
      
        //Create an instance of Csound
        csound = new Csound();
        csound->SetOption("-odac");


        //compile instance of csound.
        csound->Compile("test1.csd");
        //prepare Csound for performance
        csound->Start();
        csoundPerformanceThread = new CsoundPerformanceThread(csound);
        //perform entire score
        csoundPerformanceThread->Play();

        mainWindow.reset (new MainWindow (getApplicationName(), csound, csoundPerformanceThread));
    }

    void shutdown() override
    {
        // Add your application's shutdown code here..
        csoundPerformanceThread->Stop();
        delete csoundPerformanceThread;
        delete csound;

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
        explicit MainWindow (juce::String name, Csound* _csound, CsoundPerformanceThread* _csoundPerformanceThread)
            : DocumentWindow (name,
                              juce::Desktop::getInstance().getDefaultLookAndFeel()
                                                          .findColour (ResizableWindow::backgroundColourId),
                              DocumentWindow::allButtons),
              csound(_csound),
              csoundPerformanceThread(_csoundPerformanceThread)
        {
            setUsingNativeTitleBar (true);
            setContentOwned (new MainComponent(csound, csoundPerformanceThread), true);

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
            JUCEApplication::getInstance()->systemRequestedQuit();
        }

        /* Note: Be careful if you override any DocumentWindow methods - the base
           class uses a lot of them, so by overriding you might break its functionality.
           It's best to do all your work in your content component instead, but if
           you really have to override any DocumentWindow methods, make sure your
           subclass also calls the superclass's method.
        */

    private:
        Csound* csound;
        CsoundPerformanceThread* csoundPerformanceThread;
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainWindow)        
    };

private:
    std::unique_ptr<MainWindow> mainWindow;
};

//==============================================================================
// This macro generates the main() routine that launches the app.
START_JUCE_APPLICATION (GuiAppApplication)

# todo

## v.0

End goal: working and stable application that
interprets UI inside csd file. And can work in several modes:

* UI and csd
* UI only for debug
* csd only for csound by-pass

steps to complete:

* add atomic examples for all UIs and see that they work
   and updated properly:
     * by user
     * by outside actions (change of the state)

* create several classes for CsdProcessor:
    * CsdSynth - for Midi to Audio
    * CsdEffect - for Audio to Audio
    * CsdMidi - for Midi to Midi
    * CsdParse - for Audio to Midi

  derive them from CsdProcessor. make CsdProcessor to semi interface 
  with processBuffer as virtual function

  define which type of Csd to use by Csound command line arguments:
    presence of dac or adc or M flags

* Color modifiers:
   * color name
   * [modifier]-name, where modifier = [dark, darker, darkest, light, lighter, lightest]
   * interpolation:  "color-1/color-2/percentage" - mix between colors

* Custom palette configurable from inits

* memory to persist some state:

  ```yaml
  save:
    file: ...
    state: [var1, var2, var3]
    save-settings: bool
  ```

  should persist the vars between calls

* presets:
    
   ```yaml
   presets: [directory]
   ```
   
   presets are loaded from directories

* UI to load presets (some menu)

* UI for default settings
   set it in config wheather to show it and in which style (menu, separate tab)

* Structure of the inits:
    * window size
    * title
    * memory
    * presets
    * ui-refresh-rate-hz
    * audio:
        * input
        * output
        * midi input
        * midi output

* New widgets:
    * menu
    * modal menu
    * histogram / graph for array input (useful to show spectrum or audio)
    * histogra / graph for array output (useful to write spectrum or audio)
    * file input:
        * input files or tables:
        * as directory
        * as drag and drop field
    * file output to to show files as graphs
    * modal window to show alerts
    * centered knob with [-1, 1] range. Useful for pans (maybe also slider)


* `dot` (button, toggle, group) - define with Drawable button (maybe use icons of circle: full and hollow)

* background image, add it to board. any panel should be able to 
    print background image

* optionals for style properties with proper inheritance rules:
   * colors inherit from previous levels
   * secondary color if missing equals to primary color
   * properties of panels/groups:
       * background
       * background-image
       * border
  * pads should have modifier of weather inherit or not
     maybe special name

* actions on widgets to modify appearance from Csound:
   * widget can have ref - unique name
   * widget listens for state change on the special channels:
      ref-name.property
   * possible properties:
       * active/disabled: bool
       * show/hide: bool
       * increment: int/float
       * toggle
       * click - on change
   * those properties become auxilliary UI-state.
      just use the same Model.State structure for updates
      
* actions without value:
    useful for clicks and toggles

* section in the update state:

  ```yaml
  update:
   set: -- set value
   click: - trigger click on toggle or button
   increment: to increment on value (set also the wrap: [min, max] values)
  ```
* shortcut for keyboard+down action, if modifier missing use always down

* Docs on the application with description of
   * How it works
   * how to set different sections: ui, config, csound
   * how widgets work
   * widgets by type
 
* all channels wildcard: [*]:
   * use all channels for csound if wild card is present
   * or save persisten all channels if wildcard is present
   
## v.1

Goal of the release: make v.0 work as VST plugin

* create build target as VST with possible config

* VST config should support two modes:
   * VST-container for different types of CSD files 
      (like Kontakt it can load synths of different types)
   * VST with fixed Csd file and only it's own set of presets
   * configs of UI-version

* React properly on HOST audio settings and update settings in accord
   but preserve the type of the Csd (synth, effect, midi, parser)

* Special UI settings for VST:
   * handle main CSD-change
   * preset change

## v.2

Goal: gather feedback, make it stable

## v.3

Goal: build for mobiles 

* make build of the app for android

## v.4

Goal: build for mobiles (iOS)

*  make build of the app for iOS

## v.5 

Goal: VSTs for mobiles

* make build of VSTs for mobile platforms

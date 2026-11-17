# VST Violent Chorus

A chorus plugin by Violent Studios, built with [JUCE](https://juce.com) and CMake.
Builds **VST3**, **AU** (macOS), and a **Standalone** app from one codebase.

## Parameters

| Parameter    | Range            | Default | Description                          |
|--------------|------------------|---------|--------------------------------------|
| Rate         | 0.1 – 10 Hz      | 1 Hz    | LFO speed of the chorus modulation   |
| Depth        | 0 – 1            | 0.25    | Modulation depth                     |
| Centre Delay | 1 – 60 ms        | 7 ms    | Base delay time of the chorus line   |
| Feedback     | -0.95 – 0.95     | 0       | Feedback into the delay line         |
| Mix          | 0 – 1            | 0.5     | Dry/wet balance                      |

## Requirements

- CMake ≥ 3.22
- A C++17 compiler (Xcode command line tools on macOS)
- JUCE is downloaded automatically by CMake on first configure

## Build

```sh
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release
```

The built plugin is copied to your user plugin folder automatically
(`~/Library/Audio/Plug-Ins/VST3` on macOS). The standalone app lives in
`build/VstChorus_artefacts/Release/Standalone/`.

### Plugin missing from `~/Library/Audio/Plug-Ins/VST3`?

The auto-copy is a **post-build step**: it only runs when the plugin target
actually re-links. If the build is already up to date (CMake reports nothing
to do), the copy is skipped — even if the installed bundle was deleted.

To re-trigger it, force the VST3 target to relink:

```sh
touch src/PluginEditor.cpp
cmake --build build --config Release --target VstChorus_VST3
```

Or copy the built artefact manually — it's the same bundle:

```sh
cp -R "build/VstChorus_artefacts/Release/VST3/VST Violent Chorus.vst3" ~/Library/Audio/Plug-Ins/VST3/
```

Finder tips: `~/Library` is hidden by default — use **Go → Go to Folder**
(⇧⌘G) and enter `~/Library/Audio/Plug-Ins/VST3`. The plugin shows up as a
single bundle icon, not a regular file; `.vst3` is a directory bundle.

## Project layout

```
CMakeLists.txt            # CMake project; fetches JUCE and defines the plugin target
assets/logo.png           # Logo, embedded into the binary via juce_add_binary_data
src/PluginProcessor.h     # AudioProcessor declaration + parameter pointers
src/PluginProcessor.cpp   # Parameters, chorus DSP (juce::dsp::Chorus), state save/load
src/PluginEditor.h        # Custom editor + ViolentLookAndFeel declaration
src/PluginEditor.cpp      # Knob layout, logo, custom rotary knob drawing
```

The UI is a custom `juce::AudioProcessorEditor` with rotary knobs drawn by
`ViolentLookAndFeel`: a black panel, the Violent Inc. logo, and maroon knobs
with the parameter name inside, a value pill above, and range pills below.

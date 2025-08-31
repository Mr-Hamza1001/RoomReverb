# 3D Room Reverb Plugin

A JUCE-based experimental audio plugin (VST3, etc.) that models the acoustics of a simple rectangular room using ray-tracing techniques.  
The plugin generates an impulse response (IR) of the simulated room which can be loaded into a convolution reverb application, allowing users to hear the reverberation of the modelled space.

---

## ✨ Features

- **Audio plugin format:** VST3 (and other JUCE-supported formats).
- **Room acoustic modelling:** Simulates a basic rectangular or square room using ray-tracing and reflection techniques.
- **Path tracing:** Randomly distributed ray paths.
- **Two-pass simulation:**
  - Pass 1: Identify successful ray paths.
  - Pass 2: Hone in on these paths to generate a more accurate sound field representation.
- **Room scaling:** User can modify the size of the room (currently only in code).
- **First-person display:** Graphical first-person view of the simulated room to visualise the model in real-time.
- **Impulse response generation:** Export IRs of the current room model with one click.
- **Convolution reverb compatibility:** Exported IR files can be loaded into a convolution reverb to hear the simulated room’s reverberation.

---

## 🚧 Roadmap / Future Features

Planned improvements and enhancements include:

- Integration of the **3D sound engine** from the [`ThreeDSoundEngine`](https://github.com/jamesstanier/ThreeDSoundEngine) project for 3D spatialisation of reverberation (pending refinement and testing of the engine).
- Improved **first-person display**:
  - Better lighting and texturing.
  - More intuitive navigation controls.
- Enhanced **GUI design**.
- Integration with **JUCE’s Convolution Reverb** module for in-plugin playback.
- Improved **memory handling** in reflection processing.
- Attempting **real-time processing**, enabling users to hear room changes interactively:
  - GPU acceleration where appropriate.
- Improved **error catching and reporting**.
- Improved IR generation.
- Potential integration with the **Spatial Audio Framework (SAF)**.

---

## 🛠️ Building

This project uses [JUCE](https://juce.com/).  
To build:

1. Clone this repository and initialise submodules if required.
2. Open the project in **Projucer** (or build directly via Makefiles if using the Linux exporter).
3. Select your desired plugin format (e.g., VST3) in Projucer.
4. Save and build.

Dependencies (Linux example):
```bash
sudo apt install build-essential cmake pkg-config libfreetype6-dev libx11-dev libxcomposite-dev libxinerama-dev libxcursor-dev libxext-dev libwebkit2gtk-4.0-dev libgtk-3-dev libasound2-dev libjack-jackd2-dev libsndfile1-dev libcurl4-openssl-dev
```

---

## 🎧 Usage

- Load the plugin into your DAW or run as a standalone application.
- View the simulation in the graphical first-person display.
- Generate an impulse response (IR) of the current room (using the 'Process' button).
- Import the IR ('output.wav') into any convolution reverb plugin to hear the reverberation.

---

## 🤝 Contributing

Contributions are welcome!  
Areas where help is especially valuable:
- Optimising the ray-tracing / reflection engine.
- Improving the IR to prevent distortion.
- Improving the GUI and 3D graphics rendering.
- Integrating external spatial audio engines (e.g. ThreeDSoundEngine, SAF).
- Extending platform support and build configurations.

Please fork the repository and submit a pull request.

---

## 📜 License

This project is licensed under the [GPL v3](https://www.gnu.org/licenses/gpl-3.0.html).  
For commercial licensing options, please contact the project maintainer.

---

## 🙌 Acknowledgements

Built with [JUCE](https://juce.com/), an open-source cross-platform C++ framework for audio and GUI applications.

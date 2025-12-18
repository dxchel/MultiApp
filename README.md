# 🌌 MultiApp

Small GUI MultiApp project for showcasing and learning C++ made on Linux (Not sure if WebKit is usable in Windows anymore).

This App will evolve with time, at the moment the plan is to have a general layout and testing for different GUI apps living inside the same Window for learning how to use Gtkmm4 and Webkit.

In the future it should have apps that work using SQL, SDL and even web requests.

- Current Apps:
  - Internet browser: Simple app for browsing the internet, can detect URLs even without 'https://' or 'www.', any other entry will be treated as search.
  - Fractal creator: Simple app for creating fractal images. (WIP)

## Table of Contents

- [MultiApp](#-multiapp)
  - [Project Structure](#-project-structure)
  - [Roadmap](#️-roadmap)
  - [Requirements](#️-requirements)
  - [Build and Run](#-build-and-run)
  - [Screenshots](#-screenshots)

## 🧬 Project Structure

``` C++
MultiApp/            // Core application files
├── src/             // C++ Source Files
│   └── include/     // C++ Header Files
│
├── res/             // Misc Resources
│   └── gtk/         // Gtk UI Resources
│
└── tests/           // Test Source Files
    └── include/     // Tests Header Files
```

## 🗺️ Roadmap

- 🚧 Simple internet browser
- ❌ Fractal creator
- 🔬 Other goodies in the future

## 🏗️ Requirements

In Arch based OS:

- gtkmm-4.0
- webkitgtk-6.0

In Ubuntu/Debian based OS:

- libgtkmm-4.0-dev
- libwebkitgtk-6.0-dev

## 🔨 Build and run

For building, run:
``` make ```
It creates a multiapp.exe executable.

For building tests, run:
``` make tests ```
It creates a multiapp_tests.exe executable.

For cleaning building objects and executables, run:
``` make clean ```
It removes the build/, build_tests/, multiapp.exe and multiapp_tests.exe folders and files.

## 📷 Screenshots

Here are some screenshots of the funtioning app on Linux.

Fresh window when opening app:
![Fresh window](https://github.com/dxchel/MultiApp/blob/main/res/fresh_window.png)

Window after entering google.com, notice back button enabled:
![Entered google.com](https://github.com/dxchel/MultiApp/blob/main/res/google.png)

Window after adding directions and returning to repo in URL history, notice both back and forward buttons enabled:
![Added some directions and returned to project Repo](https://github.com/dxchel/MultiApp/blob/main/res/multiapp_repo.png)

Fractal window placeholder:
![Fractal placeholder](https://github.com/dxchel/MultiApp/blob/main/res/fractal.png)

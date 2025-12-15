# 🌌 MultiApp

Small GUI MultiApp project for showcasing and learning C++.

This App will evolve with time, at the moment the plan is to have a general layout and testing for different GUI apps living inside the same Window for learning how to use Gtkmm4 and Webkit.

In the future it should have apps that work using SQL, SDL and even web requests.

## Table of Contents

- [MultiApp](#-multiapp)
  - [Project Structure](#-project-structure)
  - [Roadmap](#️-roadmap)
  - [Requirements](#️-requirements)

## 🧬 Project Structure

``` C++
MultiApp/            // Makefile/README
│
├── src/             // C++ Source Files
├── include/         // C++ Header Files
├── res/             // Misc Resources
│   │
│   └── gtk/         // Gtk UI Resources
└── tests/           // Test Source Files
    │
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

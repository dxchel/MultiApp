# 🌌 MultiApp

Small GUI MultiApp project for showcasing and learning C++ made on Linux (Not sure if WebKit is usable in Windows anymore).

This App will evolve with time, at the moment the plan is to have a general layout and testing for different GUI apps living inside the same Window for learning how to use Gtkmm4 and Webkit. it is not meant to have a goal, it's used to learn and find new ways of coding.

In the future it should have apps that work using SQL, SDL and even web requests.

- Current Apps:
  - Internet browser: Simple app for browsing the internet, can detect URLs even without 'https://' or 'www.', any other entry will be treated as search.
  - Fractal creator: Simple app for creating fractal images.
  - Local LAN chat: Simple LAN chatt app for chatting within LAN, can startup as client or host.

## Table of Contents

- [MultiApp](#-multiapp)
  - [Project Structure](#-project-structure)
  - [Roadmap](#️-roadmap)
  - [Requirements](#️-requirements)
  - [Build and Run](#-build-and-run)
    - [Docker](#-docker)
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

- 🎉 Simple internet browser
- 🎉 Fractal creator
- 🎉 Local chat app
- ❌ Game of life
- ❌ Maze generator/player/solver
- ❌ Music player
- ❌ Local AI chat with ollama
- 🔬 Other goodies in the future

## 🏗️ Requirements

In Arch based OS:

- gtkmm-4.0
- webkitgtk-6.0
- asio
- gtest

In Ubuntu/Debian based OS:

- libgtkmm-4.0-dev
- libwebkitgtk-6.0-dev
- libasio-dev
- libgtest-dev

## 🔨 Build and run

For building, run:
``` make ```
It creates a multiapp.exe executable.

For building tests, run:
``` make tests ```
It creates a multiapp_tests.exe executable that uses the [GTEST](https://github.com/google/googletest) framework.

For cleaning building objects and executables, run:
``` make clean ```
It removes the build/, build_tests/, multiapp.exe and multiapp_tests.exe folders and files.

### 🐋 Docker

You can also run with docker!
Just run ```sudo docker build -t multiapp .``` to create a local image.
Note: It can't currently show the web browser or Gtk4 icons.

And then run the container using:

- Linux: Run ``` xhost +local:docker ```
then run with ```sudo docker run -it --rm -e DISPLAY=$DISPLAY --network host -v /tmp/.X11-unix:/tmp/.X11-unix multiapp```

- Windows: Run with
``` docker run -it -e DISPLAY=$DISPLAY --network host -v /mnt/host/wslg/.X11-unix:/tmp/.X11-unix -v /mnt/host/wslg:/mnt/host/wslg multiapp ```

- Mac: ``` Use xquartz to allow the connection ```,
then run ``` xhost +local:docker ```
and run with ```sudo docker run -it --rm -e DISPLAY=$DISPLAY --network host -v /tmp/.X11-unix:/tmp/.X11-unix multiapp```

## 📷 Screenshots

Here are some screenshots of the funtioning app on Linux.

Fresh window when opening browser app:
![Browser App](https://github.com/dxchel/MultiApp/blob/main/res/screenshots/fresh_window.png)

Window after adding directions and returning to project repo in URL history, notice both back and forward buttons enabled:
![Added some directions and returned to project Repo](https://github.com/dxchel/MultiApp/blob/main/res/screenshots/multiapp_repo.png)

Fractal app window:
![Fractal App](https://github.com/dxchel/MultiApp/blob/main/res/screenshots/fractal.png)

Chat app window:
![Chat App](https://github.com/dxchel/MultiApp/blob/main/res/screenshots/chat.png)

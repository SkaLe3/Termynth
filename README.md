# Termynth 
[![License](https://img.shields.io/github/license/SkaLe3/Termynth)](https://github.com/SkaLe3/Termynth/blob/main/LICENSE)
[![Platform](https://img.shields.io/badge/Platform-Windows%20|%20Linux-lightgrey)](https://github.com/SkaLe3/Termynth)
[![C++](https://img.shields.io/badge/Language-C++20-blue)](https://isocpp.org)


**Termynth** is a cross-platform game engine that features console-based rendering and built-in networking for multiplayer games.
It is designed to be lightweight, flexible, and developer-friendly, supporting multiple ways of integration depending on your workflow. (Add: provides framework, but it is not strict, gives freedom)

***
## Table of Contents
- [Features](#features)
- [Requirements](#requirements)
- [Getting Started](#getting-started)
    - [Option 1: Install the Engine](#option-1-install-the-engine)
    - [Option 2: Use as a Submodule](#option-2-use-as-a-submodule)
    - [Option 3: Manual / Custom Setup](#option-3-manual--custom-setup)
- [Linux Input Setup](#linux-input-setup)
- [Project Structure](#project-structure)
- [Quick Start](#quick-start)
- [Building and Running](#building-and-running)
- [Contributing](#contributing)
- [License](#license)

***

## Features
- **✅ Cross-platform** (Windows, Linux others via GCC/MSVC toolchains)
- **✅ Console renderer** for lightweight output
- **✅ Networking support** for multiplayer games
- **✅ Custom project creation utility** (`CreateProject.exe`)
- ✅ Flexible integration: install engine once, embed as submodule, or manage manually
- ✅ Built with **CMake**, supporting standard build pipelines

***

## Requirements
Before using Termynth, ensure you have the following installed:
- CMake 3.22 or higher
- Windows or Linux operating system
- Git (optional, required for submodule usage)

***


## Getting Started

Termynth offers three ways to set up and use the engine depending on your workflow:

- **Install globally:** Recommended for reusing the engine across multiple projects.
- **Submodule:** Include the engine directly in your project repository.
- **Manual / Custom Setup:** Fully custom project structure and build pipeline using CMake.

**Common Notes for All Options**:
- Place headers in `Source/Public` and source files in `Source/Private`
- Build using either the provided scripts in `Scripts/` or CMake directly
- You can modify the engine source in any setup; simply rebuild as needed


### Option 1: Install the Engine

#### 1. Clone the repository
```bash
git clone https://github.com/SkaLe3/Termynth
```

Or download the ZIP archive directly from GitHub.

#### 2. Install the engine
```bash
Install.bat [Compiler] [Configuration]
```
- Compiler: `GCC` or `MSVC` (default: `GCC`)
- Configuration: `Debug`, `Development`, or `Shipping` (default: `Shipping`)

Example: 
```bash
Install.bat MSVC Development
```

The engine will be built and installed to: `Build/[Compiler]/Install`

#### 3. Create a new project
Run `CreateProject.exe` to generate the initial project structure and files, specify the project name, destination directory, and compiler. You can also install the engine at this step if needed.


### Option 2: Use as a submodule

#### 1. Add Termynth as a submodule
Inside your project root:
```bash
git submodule add https://github.com/SkaLe3/Termynth Engine/Termynth
git submodule update --init --recursive
git add .gitmodules Engine/Termynth
git commit -m "Add Termynth submodule"
```
(The path `Engine/Termynth` is an example; any subfolder works.)

#### 2. Create a new project using `CreateProject.exe`

- Set **Destination directory** -> your project root
- Select **Engine Source** option instead of Installed Engine
- Choose compiler

By default, this will build the Shipping configuration for the client target

### Option 3: Manual / Custom Setup

- Skip the provided automation tools entirely
- Use other compilers/toolchains beyond GCC/MSVC
- Fully customize project structure and build with CMake

## Linux Input Setup
For continuous input (without the ~500 ms key repeat delay), Linux requires extra configuration.
The Linux kernel restricts direct access to raw input devices (like the keyboard file) for security. These steps configure the system's udev device manager to permanently allow users in the standard input group to read the keyboard state, bypassing the default security block.

### Steps

1. **Grant User Access**

Make sure your user is in the `input` group:

```bash
sudo usermod -aG input $USER
```

2. **Configure Device Permisions**

Ensure input files are always owned by the input group.

- Create a udev rule file:
```bash
sudo nano /etc/udev/rules.d/99-input-access.rules
```
- Add the following content:
```bash
# Grant read/write access to all input event devices for the 'input' group
KERNEL=="event*", SUBSYSTEM=="input", MODE="0660", GROUP="input"
```
- Apply the new rules:
```bash
sudo udevadm control --reload-rules
sudo udevadm trigger
```


## Project Structure

A typical Termynth project created with `CreateProject.exe` looks like this:
```text
MyProject/
│
├── Engine/             # (optional) engine submodule
├── Source/
│   ├── Public/         # public headers
│   └── Private/        # source files
├── Scripts/            # build scripts (optional)
└── CMakeLists.txt      # cmake build configuration
```
## Quick Start
Here’s a minimal example to get started with Termynth:

```cpp
"placeholder"
```
- This initializes the engine and renders a simple message to the console.
- Save it as main.cpp in your project, then build using your preferred method (CMake or provided scripts).


## Building and Running
- Use the batch scripts from `Scripts/` for convenience.
- Or run CMake manually to configure and build your projects.


## Contributing
Contributions are welcome! Please fork the repository and open a pull request with your changes.

## License
This project is licensed under the terms of the MIT License. See the [LICENSE](https://github.com/SkaLe3/Termynth/blob/main/LICENSE) file for more details.

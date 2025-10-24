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
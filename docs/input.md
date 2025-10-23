# Input

## Overview

The Termynth input API is exposed through the `InputManager` class.

The `InputManager` provides a Unity-like input system. It handles keyboard input with support for key states, axis-based movement, and event callbacks.
Input polling is handled automatically by Termynth.

## Table of Contents

- [Core Concepts](#core-concepts)
- [API Reference](#api-reference)
- [Usage Examples](#usage-examples)
- [Supported Keys](#supported-keys)
---

## Core Concepts

### Key States

Each key can be in one of four states:

| State | Description | When to Use |
|-------|-------------|-------------|
| **None** | Key is not pressed | Default state |
| **Down** | Key was just pressed this frame | One-time actions (jump, shoot, open menu) |
| **Held** | Key is being held down | Continuous actions (move, aim) |
| **Up** | Key was just released this frame | Release actions (stop charging, close menu) |

- **Down** and **Up** states last for exactly one frame
- **Held** state persists across multiple frames while key is pressed
- States automatically transition: `Down → Held → Up → None`

### Input Axes

Axes provide normalized directional input (-1.0 to 1.0):

- **Smoothed Axes** (`GetAxis`): Values gradually interpolate, creating smooth movement
- **Raw Axes** (`GetAxisRaw`): Values instantly snap to -1, 0, or 1, perfect for grid-based movement

---

## API Reference

### Key Detection Methods

#### `bool GetKey(Key key) const`

Returns `true` while the specified key is held down (Down or Held state).

**Use for:** Continuous actions like movement, holding to charge

```cpp
if (input.GetKey(Key::W))
{
    player.MoveForward();  // Called every frame while W is held
}
```

---

#### `bool GetKeyDown(Key key) const`

Returns `true` only on the frame the key was pressed (Down state).

**Use for:** One-time actions like jumping, shooting, toggling menus

```cpp
if (input.GetKeyDown(Key::Space))
{
    player.Jump();  // Called once per press
}
```

---

#### `bool GetKeyUp(Key key) const`

Returns `true` only on the frame the key was released (Up state).

**Use for:** Release actions like stopping a charge, closing menus

```cpp
if (input.GetKeyUp(Key::Escape))
{
    TogglePauseMenu();  // Called once when released
}
```

---

#### `KeyState GetKeyState(Key key) const`

Returns the detailed state of a key (None, Down, Held, or Up).

**Use for:** Advanced input handling with state machines

```cpp
KeyState state = input.GetKeyState(Key::Shift);
switch (state)
{
    case KeyState::Down:  StartSprinting(); break;
    case KeyState::Held:  ContinueSprinting(); break;
    case KeyState::Up:    StopSprinting(); break;
}
```

---

### Axis Methods

#### `float GetAxis(const char* axisName) const`

Returns smoothed axis value between -1.0 and 1.0.

**Use for:** Smooth character movement, camera control

```cpp
float horizontal = input.GetAxis("Horizontal");  // Arrow keys
float vertical = input.GetAxis("Vertical");

player.Move(horizontal * speed, vertical * speed);
```

**Default Axes:**
- `"Horizontal"` - Left/Right arrow keys
- `"Vertical"` - Up/Down arrow keys
- `"HorizontalWASD"` - A/D keys
- `"VerticalWASD"` - W/S keys

---

#### `float GetAxisRaw(const char* axisName) const`

Returns instant axis value (-1.0, 0.0, or 1.0) with no smoothing.

**Use for:** Grid-based movement, UI navigation, precise control

```cpp
float horizontal = input.GetAxisRaw("Horizontal");
float vertical = input.GetAxisRaw("Vertical");

if (horizontal != 0 || vertical != 0)
{
    MoveOnGrid((int)horizontal, (int)vertical);
}
```

---

#### `void ConfigureAxis(const char* axisName, Key positive, Key negative)`

Creates or modifies a custom axis.

**Parameters:**
- `axisName` - Name of the axis
- `positive` - Key that produces +1.0
- `negative` - Key that produces -1.0

```cpp
// Create custom axis for strafing
input.ConfigureAxis("Strafe", Key::E, Key::Q);

// Use it
float strafe = input.GetAxis("Strafe");
```

---

### Event Callbacks

#### `void RegisterKeyDownCallback(Key key, KeyCallback callback)`

Registers a callback function to be called when a key is pressed.

```cpp
input.RegisterKeyDownCallback(Key::F, [](Key key) {
    InteractWithObject();
    std::cout << "Interacting...\n";
});
```

---

#### `void RegisterKeyUpCallback(Key key, KeyCallback callback)`

Registers a callback function to be called when a key is released.

```cpp
input.RegisterKeyUpCallback(Key::R, [](Key key) {
    FinishReload();
});
```

---

#### `void UnregisterKeyDownCallback(Key key)`

Removes all KeyDown callbacks for the specified key.

---

#### `void UnregisterKeyUpCallback(Key key)`

Removes all KeyUp callbacks for the specified key.

---

### Utility Methods

#### `bool IsAnyKeyPressed() const`

Returns `true` if any key is currently pressed (Down or Held state).

---

#### `void GetPressedKeys(std::vector<Key>& outKeys) const`

Fills the provided vector with all currently pressed keys.

```cpp
std::vector<Key> pressedKeys;
input.GetPressedKeys(pressedKeys);
```

---

#### `void ClearAllInput()`

Resets all key states and axis values to default.

---

## Usage Examples

### Example 1: Basic Character Movement

```cpp
void UpdatePlayer()
{
    auto& input = InputManager::Get();
    
    // Continuous movement while keys are held
    if (input.GetKey(Key::A)) player.MoveLeft();
    if (input.GetKey(Key::D)) player.MoveRight();
    
    // One-time actions
    if (input.GetKeyDown(Key::Space)) player.Jump();
    if (input.GetKeyDown(Key::E)) player.Shoot();
}
```

### Example 2: Smooth Movement with Axes

```cpp
void UpdatePlayerSmooth(float deltaTime)
{
    auto& input = InputManager::Get();
    
    float horizontal = input.GetAxis("HorizontalWASD");
    float vertical = input.GetAxis("VerticalWASD");
    
    Vector2 velocity(horizontal, vertical);
    velocity.Normalize();
    
    player.Move(velocity * speed * deltaTime);
}
```

### Example 3: Menu Navigation

```cpp
void UpdateMenu()
{
    auto& input = InputManager::Get();
    
    // Use raw axis for instant menu navigation
    float vertical = input.GetAxisRaw("Vertical");
    
    if (vertical > 0) menu.MoveUp();
    if (vertical < 0) menu.MoveDown();
    
    if (input.GetKeyDown(Key::Enter)) menu.Select();
    if (input.GetKeyDown(Key::Escape)) menu.Back();
}
```

### Example 4: Charge Attack

```cpp
void HandleChargeAttack()
{
    auto& input = InputManager::Get();
    static float chargeTime = 0.0f;
    
    if (input.GetKeyDown(Key::Space))
    {
        chargeTime = 0.0f;
        ShowChargingEffect();
    }
    
    if (input.GetKey(Key::Space))
    {
        chargeTime += deltaTime;
        UpdateChargingEffect(chargeTime);
    }
    
    if (input.GetKeyUp(Key::Space))
    {
        player.ReleaseChargedAttack(chargeTime);
        HideChargingEffect();
    }
}
```

### Example 5: Input Events

```cpp
void SetupUICallbacks()
{
    auto& input = InputManager::Get();
    
    // Register callbacks for UI actions
    input.RegisterKeyDownCallback(Key::P, [](Key key) {
        TogglePause();
    });
    
    input.RegisterKeyDownCallback(Key::M, [](Key key) {
        ToggleShootMode();
    });
    
    input.RegisterKeyDownCallback(Key::Tab, [](Key key) {
        CycleTargets();
    });
}
```

### Example 6: Custom Control Scheme

```cpp
void SetupCustomControls()
{
    auto& input = InputManager::Get();
    
    // Configure axes for custom controls
    input.ConfigureAxis("Strafe", Key::E, Key::Q);
    
    // Use them
    float strafe = input.GetAxis("Strafe"); 
    player.Strafe(strafe);
}
```

### Example 7: "Press Any Key" Screen

```cpp
void UpdateTitleScreen()
{
    auto& input = InputManager::Get();
    
    if (input.IsAnyKeyPressed())
    {
        FadeToMainMenu();
    }
}
```

### Example 8: Combo Detection

```cpp
void DetectCombo()
{
    auto& input = InputManager::Get();
    static std::vector<Key> comboSequence = {Key::W, Key::W, Key::S, Key::A};
    static std::vector<Key> inputSequence;
    
    std::vector<Key> pressedKeys;
    input.GetPressedKeys(pressedKeys);
    
    if (!pressedKeys.empty())
    {
        inputSequence.push_back(pressedKeys[0]);
        
        if (inputSequence.size() > comboSequence.size())
        {
            inputSequence.erase(inputSequence.begin());
        }
        
        if (inputSequence == comboSequence)
        {
            ActivateSpecialMove();
            inputSequence.clear();
        }
    }
}
```

---

## Supported Keys

The following keys are supported (defined in `Key.h` enum):

### Alphanumeric
- `Key::A` through `Key::Z`
- `Key::Num0` through `Key::Num9`

### Special Keys
- `Key::Space`, `Key::Enter`, `Key::Escape`
- `Key::Backspace`, `Key::Tab`

### Arrow Keys
- `Key::Up`, `Key::Down`, `Key::Left`, `Key::Right`

### Function Keys
- `Key::F1` through `Key::F12`

### Navigation Keys
- `Key::Insert`, `Key::DeleteKey`
- `Key::Home`, `Key::End`
- `Key::PageUp`, `Key::PageDown`

---

## Thread Safety

**The InputManager is not thread-safe.** All input operations should be called from the main game thread only.

---

#pragma once
#include "Core/Core.h"
#include "Input/Key.h"
#include <unordered_map>
#include <vector>
#include <functional>
#include <string>

using KeyCallback = std::function<void(Key)>;

class InputManager
{
public:
    static InputManager& Get();

    void PollInput();

    // State queries
    bool GetKey(Key key) const;
    bool GetKeyPressed(Key key) const;
    bool GetKeyReleased(Key key) const;

    // Additional utility functions
    bool IsAnyKeyPressed() const;
    void GetPressedKeys(std::vector<Key>& outKeys) const;
    
    // Event system for key presses
    void RegisterKeyPressedCalllback(Key key, KeyCallback callback);
    void RegisterKeyReleasedCallback(Key key, KeyCallback callback);
    void UnregisterKeyPressedCallback(Key key);
    void UnregisterKeyReleasedCallback(Key key);

    // Input axis simulation
    float GetAxis(const char* axisName) const;
    float GetAxisRaw(const char* axisName) const;

    // Custom axis configuration
    void ConfigureAxis(const char* axisName, Key positive, Key negative);

    // Utility
    void ClearAllInput();
    KeyState GetKeyState(Key key) const;

private:
    Key TranslateCharToKey(char c, int extended = 0);
    int32 TranslateKeyToVirtualKey(Key key);
    void TriggerKeyPressedCallbacks(Key key);
    void TriggerKeyReleasedCallbacks(Key key);

private:
    std::unordered_map<Key, KeyState> m_KeyStates;

    // Event callbacks
    std::unordered_map<Key, std::vector<KeyCallback>> m_KeyPressedCallbacks;
    std::unordered_map<Key, std::vector<KeyCallback>> m_KeyReleasedCallbacks;

    struct AxisConfig
    {
        Key Positive;
        Key Negative;
        float Smoothing = 0.1f;
        float CurrentValue = 0.0f;
    };
    std::unordered_map<std::string, AxisConfig> m_Axes;

private:
    InputManager();
    ~InputManager() = default;
    InputManager(const InputManager&) = delete;
    InputManager(InputManager&&) = delete;
    InputManager& operator=(const InputManager&) = delete;
    InputManager& operator=(InputManager&&) = delete;
};
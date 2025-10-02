#pragma once
#include "Input/Key.h"
#include <unordered_map>

class InputManager
{
public:
    static InputManager& Get();

    void PollInput();

    bool IsKeyPressed(Key key) const;

private:
    Key TranslateCharToKey(char c, int extended = 0);

private:
    std::unordered_map<Key, bool> m_CurrentState;
    std::unordered_map<Key, bool> m_PreviousState;

private:
    InputManager();
    ~InputManager() = default;
    InputManager(const InputManager&) = delete;
    InputManager(InputManager&&) = delete;
};
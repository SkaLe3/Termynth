#pragma once

class App
{
public:
    inline static double GetFixedDeltaTime()
    {
        return s_FixedDeltaTime;
    }

    static void SetFixedDeltaTime(double seconds)
    {
        s_FixedDeltaTime = seconds;
    }

    static bool UseFixedTimeStep()
    {
        return s_bUseFixedDeltaTime;
    }

    static void SetUseFixedTimeStep(bool val)
    {
        s_bUseFixedDeltaTime = val;
    }

    inline static double GetCurrentTime()
    {
        return s_CurrentTime;
    }

    static void SetCurrentTime(double seconds)
    {
        s_CurrentTime = seconds;
    }

    inline static double GetLastTime()
    {
        return s_LastTime;
    }

    static void UpdateLastTime()
    {
        s_LastTime = s_CurrentTime;
    }

    inline static double GetDeltaTime()
    {
        return s_DeltaTime;
    }

    static void SetDeltaTime(double seconds)
    {
        s_DeltaTime = seconds;
    }

    inline static double GetDeltaTimeAccumulator()
    {
        return s_DeltaTimeAccumulator;
    }

    inline static bool FixedTimeStepReached()
    {
        return s_DeltaTimeAccumulator >= s_FixedDeltaTime;
    }

    static void UpdateDeltaTimeAccumulator()
    {
        s_DeltaTimeAccumulator += s_DeltaTime;
    }

    static void SubtructDeltaTimeAccumulator()
    {
        s_DeltaTimeAccumulator -= s_FixedDeltaTime;
    }

private:
    static bool s_bUseFixedDeltaTime;
    static double s_FixedDeltaTime;
    static double s_CurrentTime;
    static double s_LastTime;
    static double s_DeltaTime;

    static double s_DeltaTimeAccumulator;
};
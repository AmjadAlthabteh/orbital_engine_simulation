#pragma once
#include <chrono>
#include <string>
#include <vector>
#include <unordered_map>

// OPTIMIZATION: Performance profiling system to track frame time breakdown
// Usage:
//   PerformanceProfiler profiler;
//   profiler.startFrame();
//   profiler.beginSection("Physics");
//   // ... physics code ...
//   profiler.endSection("Physics");
//   profiler.endFrame();
//   float physicsMs = profiler.getSectionTime("Physics");

struct ProfileSection
{
    std::string name;
    float timeMs;           // Average time in milliseconds
    float minMs;            // Minimum time
    float maxMs;            // Maximum time
    int callCount;          // Number of times called
};

class PerformanceProfiler
{
private:
    using Clock = std::chrono::high_resolution_clock;
    using TimePoint = std::chrono::time_point<Clock>;

    TimePoint frameStart;
    TimePoint sectionStart;
    std::unordered_map<std::string, ProfileSection> sections;
    std::unordered_map<std::string, float> currentSectionTimes;

    float totalFrameTime;
    int frameCount;
    bool inFrame;
    bool inSection;

public:
    PerformanceProfiler()
        : totalFrameTime(0.0f)
        , frameCount(0)
        , inFrame(false)
        , inSection(false)
    {
    }

    // Start tracking a new frame
    void startFrame()
    {
        frameStart = Clock::now();
        inFrame = true;
        currentSectionTimes.clear();
    }

    // Begin tracking a named section
    void beginSection(const std::string& name)
    {
        sectionStart = Clock::now();
        inSection = true;
    }

    // End tracking the current section
    void endSection(const std::string& name)
    {
        if (!inSection)
            return;

        auto sectionEnd = Clock::now();
        float duration = std::chrono::duration<float, std::milli>(sectionEnd - sectionStart).count();

        // Store current section time
        currentSectionTimes[name] = duration;

        // Update statistics
        if (sections.find(name) == sections.end())
        {
            sections[name] = ProfileSection{ name, duration, duration, duration, 1 };
        }
        else
        {
            ProfileSection& section = sections[name];
            section.timeMs = (section.timeMs * section.callCount + duration) / (section.callCount + 1);
            section.minMs = std::min(section.minMs, duration);
            section.maxMs = std::max(section.maxMs, duration);
            section.callCount++;
        }

        inSection = false;
    }

    // End frame tracking
    void endFrame()
    {
        if (!inFrame)
            return;

        auto frameEnd = Clock::now();
        totalFrameTime = std::chrono::duration<float, std::milli>(frameEnd - frameStart).count();
        frameCount++;
        inFrame = false;
    }

    // Get average time for a section in milliseconds
    float getSectionTime(const std::string& name) const
    {
        auto it = sections.find(name);
        if (it != sections.end())
            return it->second.timeMs;
        return 0.0f;
    }

    // Get current frame's section time
    float getCurrentSectionTime(const std::string& name) const
    {
        auto it = currentSectionTimes.find(name);
        if (it != currentSectionTimes.end())
            return it->second;
        return 0.0f;
    }

    // Get total frame time in milliseconds
    float getFrameTime() const
    {
        return totalFrameTime;
    }

    // Get frames per second
    float getFPS() const
    {
        if (totalFrameTime <= 0.0f)
            return 0.0f;
        return 1000.0f / totalFrameTime;
    }

    // Get all section statistics
    std::vector<ProfileSection> getAllSections() const
    {
        std::vector<ProfileSection> result;
        result.reserve(sections.size());
        for (const auto& pair : sections)
        {
            result.push_back(pair.second);
        }
        return result;
    }

    // Reset all statistics
    void reset()
    {
        sections.clear();
        currentSectionTimes.clear();
        totalFrameTime = 0.0f;
        frameCount = 0;
        inFrame = false;
        inSection = false;
    }

    // Get total number of frames profiled
    int getFrameCount() const
    {
        return frameCount;
    }

    // Get percentage of frame time for a section
    float getSectionPercentage(const std::string& name) const
    {
        if (totalFrameTime <= 0.0f)
            return 0.0f;
        float sectionTime = getSectionTime(name);
        return (sectionTime / totalFrameTime) * 100.0f;
    }

    // Print profiling report to console
    void printReport() const
    {
        printf("\n=== Performance Profile ===\n");
        printf("Total Frame Time: %.2f ms (%.1f FPS)\n", totalFrameTime, getFPS());
        printf("Frames Profiled: %d\n\n", frameCount);

        auto allSections = getAllSections();
        if (!allSections.empty())
        {
            printf("%-20s %8s %8s %8s %10s %8s\n",
                   "Section", "Avg (ms)", "Min (ms)", "Max (ms)", "Calls", "% Frame");
            printf("------------------------------------------------------------------------\n");

            for (const auto& section : allSections)
            {
                float percentage = totalFrameTime > 0.0f
                    ? (section.timeMs / totalFrameTime) * 100.0f
                    : 0.0f;

                printf("%-20s %8.2f %8.2f %8.2f %10d %7.1f%%\n",
                       section.name.c_str(),
                       section.timeMs,
                       section.minMs,
                       section.maxMs,
                       section.callCount,
                       percentage);
            }
        }
        printf("========================================================================\n\n");
    }
};

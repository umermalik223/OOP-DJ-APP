#include <JuceHeader.h>
#pragma once

// The `track` struct represents an audio track with various attributes.
// It includes the track's title, length, URL, and a unique identifier.
// It also provides a static method for formatting the track's length into a string.
struct track {

    // The title of the track.
    juce::String title;

    // The length of the track in seconds.
    double lengthInSeconds;

    // The URL to the track's file.
    juce::URL url;

    // A unique identifier for the track, useful for distinguishing tracks.
    juce::String identity;

    // Static method that formats the length of a track into a human-readable string.
    // The method converts the track length from seconds into hours, minutes, and seconds.
    // Optionally, it can include milliseconds for regular updates (e.g., during playback).
    static std::string getLengthString(double songLength, bool regularUpdate = false) {
        std::string time = "";

        // Calculate hours, minutes, and seconds from the total song length.
        int hour = std::floor(songLength / 3600);
        int min = std::floor((songLength - (static_cast<double>(hour) * 3600)) / 60);
        double sec = std::floor((songLength - (static_cast<double>(min) * 60)) * 100) / 100;
        int miniSec = std::floor((sec - std::floor(sec)) * 100);

        // Format hours, minutes, and seconds into a string.
        if (!regularUpdate) {
            // If regularUpdate is false, include hours in the format.
            if (hour < 10) {
                time += "0" + std::to_string(hour) + ":";
            }
            else {
                time += std::to_string(hour) + ":";
            }
        }

        // Format minutes with leading zero if necessary.
        if (min < 10) {
            time += "0" + std::to_string(min) + ":";
        }
        else {
            time += std::to_string(min) + ":";
        }

        // Format seconds with leading zero if necessary.
        if (sec < 10) {
            time += "0" + std::to_string(static_cast<int>(sec));
        }
        else {
            time += std::to_string(static_cast<int>(sec));
        }

        // If regularUpdate is true, include milliseconds in the format.
        if (regularUpdate) {
            if (miniSec < 10) {
                time += ":0" + std::to_string(miniSec);
            }
            else {
                time += ":" + std::to_string(miniSec);
            }
        }

        return time;
    };
};

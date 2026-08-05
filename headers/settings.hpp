#ifndef CALAMIT_SETTINGS_HPP
#define CALAMIT_SETTINGS_HPP

#include <string>

#include <raylib.h>

namespace calamit {

class Settings {
public:
    auto getTargetFps() const { return target_fps; }
    auto getProjectName() const { return project_name; }
    auto getWindowSize() const { return window_size; }
    auto getResizable() const { return resizable; }

    void setTargetFps(int fps) { target_fps = fps; }
    void setProjectName(const std::string& name) { project_name = name; }
    void setWindowSize(const Vector2& size) { window_size = size; }
    void setResizable(bool r) { resizable = r; }

private:
    int target_fps = 60;
    std::string project_name = "CalamitProject";
    Vector2 window_size = {800.0f, 600.0f};
    bool resizable = false;
};

}

#endif

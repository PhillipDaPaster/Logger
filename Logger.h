#include <filesystem>
#include <iostream>
#include <iomanip>
#include <windows.h>
#include <dwmapi.h>
#include "ImGui/imgui.h"

class Log {
public:
    Log() {}

    void clear() { entries.clear(); }

    void add_log(const char* pre_text, const char* fmt, double clear_interval = 3.0, bool b_runonce = true, ...) IM_FMTARGS(5) {
        if (b_runonce) {
            for (const auto& entry : entries) {
                if (entry.pre_text == pre_text && entry.text == fmt) {
                    return;
                }
            }
        }

        va_list args;
        va_start(args, b_runonce);
        char buf[256];
        vsnprintf(buf, 256, fmt, args);
        va_end(args);

        log_entry entry{ std::string(pre_text), std::string(buf), std::chrono::steady_clock::now(), clear_interval, b_runonce };
        entries.push_back(entry);
    }

    void draw(ImVec2 start_pos, bool background, ImColor pre_text_color = ImColor(255, 0, 0, 255)) {
        auto now = std::chrono::steady_clock::now();
        ImVec4* color = ImGui::GetStyle().Colors;

        entries.erase(std::remove_if(entries.begin(), entries.end(), [now, this](const log_entry& entry) {
            return std::chrono::duration<double>(now - entry.timestamp).count() > entry.clear_interval + fade_out_duration;
            }), entries.end());

        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 1));
        for (const auto& entry : entries) {
            double elapsed = std::chrono::duration<double>(now - entry.timestamp).count();

            ImVec2 pos;
            double lerp_duration = 0.4;
            if (elapsed < lerp_duration) {
                float t = elapsed / lerp_duration;
                pos.x = offscreen_start_pos.x + (start_pos.x - offscreen_start_pos.x) * t;
            }
            else {
                pos.x = start_pos.x;
            }
            pos.y = start_pos.y;

            float alpha = 1.0f;
            if (elapsed > entry.clear_interval) {
                double fade_elapsed = elapsed - entry.clear_interval;
                alpha = 1.0f - static_cast<float>(fade_elapsed / fade_out_duration);
                if (alpha < 0.0f) {
                    alpha = 0.0f;
                }
            }

            ImVec2 rect_size = ImVec2(ImGui::CalcTextSize(entry.pre_text.c_str()).x + ImGui::CalcTextSize(entry.text.c_str()).x + 2, ImGui::CalcTextSize(entry.pre_text.c_str()).y + 2);

            if (background) {
                ImColor color = ImGui::GetStyleColorVec4(ImGuiCol_Header);
                ImColor border = ImGui::GetStyleColorVec4(ImGuiCol_Border);
                color.Value.w = alpha;
                border.Value.w = alpha;

                ImGui::GetForegroundDrawList()->AddRectFilled(pos, ImVec2(pos.x + rect_size.x, pos.y + rect_size.y), color);
                ImGui::GetForegroundDrawList()->AddRect(pos, ImVec2(pos.x + rect_size.x, pos.y + rect_size.y), border, 0.f, 0, 0.5f);
            }

            ImColor text_color = ImGui::GetStyleColorVec4(ImGuiCol_Text);
            text_color.Value.w = alpha;

            ImGui::GetForegroundDrawList()->AddText(ImVec2(pos.x + 1, pos.y + 1), IM_COL32(0, 0, 0, static_cast<int>(255 * alpha)), entry.pre_text.c_str());
            ImGui::GetForegroundDrawList()->AddText(pos, IM_COL32(pre_text_color & 0xFF, (pre_text_color >> 8) & 0xFF, (pre_text_color >> 16) & 0xFF, static_cast<int>(255 * alpha)), entry.pre_text.c_str());

            ImGui::GetForegroundDrawList()->AddText(ImVec2(pos.x + ImGui::CalcTextSize(entry.pre_text.c_str()).x + 1, pos.y + 1), IM_COL32(0, 0, 0, static_cast<int>(255 * alpha)), entry.text.c_str());
            ImGui::GetForegroundDrawList()->AddText(ImVec2(pos.x + ImGui::CalcTextSize(entry.pre_text.c_str()).x, pos.y), text_color, entry.text.c_str());

            start_pos.y += ImGui::GetIO().Fonts->Fonts[0]->FontSize + 1;
        }
        ImGui::PopStyleVar();
    }

    void setautoscroll(bool auto_scroll) { AutoScroll = auto_scroll; }

private:
    struct log_entry {
        std::string pre_text;
        std::string text;
        std::chrono::time_point<std::chrono::steady_clock> timestamp;
        double clear_interval;
        bool b_runonce;
    };

    std::vector<log_entry> entries;
    bool AutoScroll = true;
    ImVec2 offscreen_start_pos = ImVec2(-300, 0);  // Off-screen start position to the left
    double fade_out_duration = 1.0;            
};

namespace D { inline Log log; }

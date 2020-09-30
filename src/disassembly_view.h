#ifndef DISASSEMBLY_VIEW_H
#define DISASSEMBLY_VIEW_H

#include <cstdio>
#include <cstdint>
#include <cmath> //trunc

struct disassembly_view
{
    // Settings
    bool            Open = true;                                // set to false when DrawWindow() was closed. ignore if not using DrawWindow().
    bool            ReadOnly = false;                           // disable any editing.
    unsigned int    MaxCols = 3;                                // Max number of columns per instruction
    unsigned int    MaxDisasmChars = 16;                         // Max number of columns per instruction
    bool            OptShowOptions = true;                      // display options button/context menu. when disabled, options will be locked unless you provide your own UI for them.
    bool            OptGreyOutZeroes = true;                    // display null/zero bytes using the TextDisabled color.
    bool            OptUpperCaseHex = true;                     // display hexadecimal values as "FF" instead of "ff".
    unsigned int    OptAddrDigitsCount = 0;                     // number of addr digits to display (default calculated based on maximum displayed addr).
    ImU32           HighlightColor = IM_COL32(255, 255, 255, 50); // background color of highlighted bytes. NOLINT(hicpp-signed-bitwise)

    // [Internal State]
    bool            ContentsWidthChanged = false;
    size_t          DataEditingAddr = false;
    bool            DataEditingTakeFocus = false;
    char            DataInputBuf[32] = {0};
    char            AddrInputBuf[32] = {0};
    size_t          GotoAddr = std::numeric_limits<std::size_t>::max();
    size_t          HighlightMin = std::numeric_limits<std::size_t>::max();
    size_t          HighlightMax = std::numeric_limits<std::size_t>::max();

    void GotoAddrAndHighlight(size_t addr_min, size_t addr_max)
    {
        GotoAddr = addr_min;
        HighlightMin = addr_min;
        HighlightMax = addr_max;
    }

    struct Sizes
    {
        unsigned int     AddrDigitsCount = 0;
        float   LineHeight = 0;
        float   GlyphWidth = 0.0;
        float   HexCellWidth = 0.0;
        float   SpacingBetweenMidCols= 0.0;
        float   PosHexStart= 0.0;
        float   PosHexEnd= 0.0;
        float   PosDisasmStart = 0.0;
        float   PosDisasmEnd = 0.0;
        float   WindowWidth= 0.0;
    };

    void CalcSizes(Sizes& s, size_t mem_size, size_t base_display_addr) const
    {
        ImGuiStyle& style = ImGui::GetStyle();
        s.AddrDigitsCount = OptAddrDigitsCount;
        if (s.AddrDigitsCount == 0)
            for (size_t n = base_display_addr + mem_size - 1; n > 0u; n >>= 4u)
                s.AddrDigitsCount++;
        s.LineHeight = ImGui::GetTextLineHeight();
        s.GlyphWidth = ImGui::CalcTextSize("F").x + 1;             // We assume the font is mono-space
        s.HexCellWidth = truncf(s.GlyphWidth * 2.5f);             // "FF " we include trailing space in the width to easily catch clicks everywhere

        s.PosHexStart = static_cast<float>(s.AddrDigitsCount + 2) * s.GlyphWidth;
        s.PosHexEnd = s.PosHexStart + (s.HexCellWidth * static_cast<float>(MaxCols));

        s.PosDisasmStart = s.PosHexEnd  + s.GlyphWidth; // Give 1 char of space betweeen hex and disasm
        s.PosDisasmEnd = s.PosDisasmStart + static_cast<float>(MaxDisasmChars) * s.GlyphWidth;

        s.WindowWidth = s.PosDisasmEnd + style.ScrollbarSize + style.WindowPadding.x * 2 + s.GlyphWidth;
    }

    // Standalone Memory Editor window
    void DrawWindow(const char* title, uint8_t* mem_data, size_t mem_size, size_t base_display_addr = 0x0000)
    {
        Sizes s;
        CalcSizes(s, mem_size, base_display_addr);
        ImGui::SetNextWindowSizeConstraints(ImVec2(0.0f, 0.0f), ImVec2(s.WindowWidth, FLT_MAX));

        Open = true;
        if (ImGui::Begin(title, &Open, ImGuiWindowFlags_NoScrollbar))
        {
            if (ImGui::IsWindowHovered(ImGuiHoveredFlags_RootAndChildWindows) && ImGui::IsMouseClicked(1))
                ImGui::OpenPopup("context");
            DrawContents(mem_data, mem_size, base_display_addr);
            if (ContentsWidthChanged)
            {
                CalcSizes(s, mem_size, base_display_addr);
                ImGui::SetWindowSize(ImVec2(s.WindowWidth, ImGui::GetWindowSize().y));
            }
        }
        ImGui::End();
    }

    // Memory Editor contents only
    void DrawContents(uint8_t* mem_data_void, size_t mem_size, size_t base_display_addr = 0x0000)
    {
        if (MaxCols < 1)
            MaxCols = 1;

        ImU8* mem_data = mem_data_void;
        Sizes s;
        CalcSizes(s, mem_size, base_display_addr);
        ImGuiStyle& style = ImGui::GetStyle();

        // We begin into our scrolling region with the 'ImGuiWindowFlags_NoMove' in order to prevent click from moving the window.
        // This is used as a facility since our main click detection code doesn't assign an ActiveId so the click would normally be caught as a window-move.
        const float height_separator = style.ItemSpacing.y;
        float footer_height = 0;
        if (OptShowOptions)
            footer_height += height_separator + ImGui::GetFrameHeightWithSpacing() * 1;
        ImGui::BeginChild("##scrolling", ImVec2(0, -footer_height), false, ImGuiWindowFlags_NoMove);
        ImDrawList* draw_list = ImGui::GetWindowDrawList();

        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 0));
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));

        const size_t line_total_count = (mem_size + MaxCols - 1) / MaxCols;
        ImGuiListClipper clipper(static_cast<int>(line_total_count), s.LineHeight);
        const size_t visible_start_addr = static_cast<size_t>(clipper.DisplayStart) * MaxCols;
        const size_t visible_end_addr = static_cast<size_t>( clipper.DisplayEnd) * MaxCols;

        bool data_next = false;

        if (ReadOnly || DataEditingAddr >= mem_size)
            DataEditingAddr = std::numeric_limits<std::size_t>::max();

        size_t data_editing_addr_backup = DataEditingAddr;
        size_t data_editing_addr_next = std::numeric_limits<std::size_t>::max();
        if (DataEditingAddr != std::numeric_limits<std::size_t>::max())
        {
            // Move cursor but only apply on next frame so scrolling with be synchronized (because currently we can't change the scrolling while the window is being rendered)
            if (ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_UpArrow)) && DataEditingAddr >= static_cast<size_t>(MaxCols))         { data_editing_addr_next = DataEditingAddr - MaxCols; DataEditingTakeFocus = true; }
            else if (ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_DownArrow)) && DataEditingAddr < mem_size - MaxCols) { data_editing_addr_next = DataEditingAddr + MaxCols; DataEditingTakeFocus = true; }
            else if (ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_LeftArrow)) && DataEditingAddr > 0)               { data_editing_addr_next = DataEditingAddr - 1; DataEditingTakeFocus = true; }
            else if (ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_RightArrow)) && DataEditingAddr < mem_size - 1)   { data_editing_addr_next = DataEditingAddr + 1; DataEditingTakeFocus = true; }
        }
        if (data_editing_addr_next != std::numeric_limits<std::size_t>::max() && (data_editing_addr_next / MaxCols) != (data_editing_addr_backup / MaxCols))
        {
            // Track cursor movements
            const int scroll_offset = (static_cast<int>(data_editing_addr_next / MaxCols) - static_cast<int>(data_editing_addr_backup / MaxCols));
            const bool scroll_desired = (scroll_offset < 0 && data_editing_addr_next < visible_start_addr + MaxCols * 2) || (scroll_offset > 0 && data_editing_addr_next > visible_end_addr - MaxCols * 2);
            if (scroll_desired)
                ImGui::SetScrollY(ImGui::GetScrollY() + static_cast<float>(scroll_offset) * s.LineHeight);
        }

        for (int line_i = clipper.DisplayStart; line_i < clipper.DisplayEnd; line_i++) // display only visible lines
        {
            size_t addr = static_cast<size_t>(line_i) * MaxCols;
            ImGui::Text((OptUpperCaseHex ? "%0*zX: " : "%0*zx: "), s.AddrDigitsCount, base_display_addr + addr);

            // Draw Hexadecimal
            for (size_t n = 0u; n < static_cast<size_t>(MaxCols) && addr < mem_size; n++, addr++)
            {
                float byte_pos_x = s.PosHexStart + s.HexCellWidth * static_cast<float>(n);
                ImGui::SameLine(byte_pos_x);

                // Draw highlight
                bool is_highlight_from_user_range = (addr >= HighlightMin && addr < HighlightMax);
                if (is_highlight_from_user_range)
                {
                    ImVec2 pos = ImGui::GetCursorScreenPos();
                    float highlight_width = s.GlyphWidth * 2;
                    bool is_next_byte_highlighted =  (addr + 1 < mem_size) && ((HighlightMax != std::numeric_limits<std::size_t>::max() && addr + 1 < HighlightMax));
                    if (is_next_byte_highlighted || (n + 1 == MaxCols))
                    {
                        highlight_width = s.HexCellWidth;
                    }
                    draw_list->AddRectFilled(pos, ImVec2(pos.x + highlight_width, pos.y + s.LineHeight), HighlightColor);
                }

                if (DataEditingAddr == addr)
                {
                    // Display text input on current byte
                    bool data_write = false;
                    ImGui::PushID(reinterpret_cast<void *>(addr));
                    if (DataEditingTakeFocus)
                    {
                        ImGui::SetKeyboardFocusHere();
                        ImGui::CaptureKeyboardFromApp(true);
                        sprintf(AddrInputBuf, (OptUpperCaseHex ? "%0*zX" : "%0*zx"), s.AddrDigitsCount, base_display_addr + addr);
                        sprintf(DataInputBuf, (OptUpperCaseHex ? "%02X" : "%02x"), mem_data[addr]);
                    }
                    ImGui::PushItemWidth(s.GlyphWidth * 2);
                    struct UserData
                    {
                        // FIXME: We should have a way to retrieve the text edit cursor position more easily in the API, this is rather tedious. This is such a ugly mess we may be better off not using InputText() at all here.
                        static int Callback(ImGuiInputTextCallbackData* data)
                        {
                            auto* user_data = static_cast<UserData*>(data->UserData);
                            if (!data->HasSelection())
                                user_data->CursorPos = data->CursorPos;
                            if (data->SelectionStart == 0 && data->SelectionEnd == data->BufTextLen)
                            {
                                // When not editing a byte, always rewrite its content (this is a bit tricky, since InputText technically "owns" the master copy of the buffer we edit it in there)
                                data->DeleteChars(0, data->BufTextLen);
                                data->InsertChars(0, user_data->CurrentBufOverwrite);
                                data->SelectionStart = 0;
                                data->SelectionEnd = data->CursorPos = 2;
                            }
                            return 0;
                        }
                        char   CurrentBufOverwrite[3] = {0};  // Input
                        int    CursorPos = -1;               // Output
                    };
                    UserData user_data;
                    sprintf(user_data.CurrentBufOverwrite, (OptUpperCaseHex ? "%02X" : "%02x"), mem_data[addr]);
                    ImGuiInputTextFlags flags = ImGuiInputTextFlags_CharsHexadecimal | ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_AutoSelectAll | ImGuiInputTextFlags_NoHorizontalScroll | ImGuiInputTextFlags_AlwaysInsertMode | ImGuiInputTextFlags_CallbackAlways; // NOLINT(hicpp-signed-bitwise)
                    if (ImGui::InputText("##data", DataInputBuf, 32, flags, UserData::Callback, &user_data))
                        data_write = data_next = true;
                    else if (!DataEditingTakeFocus && !ImGui::IsItemActive())
                        DataEditingAddr = data_editing_addr_next = std::numeric_limits<std::size_t>::max();
                    DataEditingTakeFocus = false;
                    ImGui::PopItemWidth();
                    if (user_data.CursorPos >= 2)
                        data_write = data_next = true;
                    if (data_editing_addr_next != std::numeric_limits<std::size_t>::max())
                        data_write = data_next = false;
                    unsigned int data_input_value = 0;
                    if (data_write && sscanf(DataInputBuf, "%X", &data_input_value) == 1)
                    {
                            mem_data[addr] = static_cast<ImU8>(data_input_value);
                    }
                    ImGui::PopID();
                }
                else
                {
                    // NB: The trailing space is not visible but ensure there's no gap that the mouse cannot click on.
                    ImU8 b = mem_data[addr];

                    if (b == 0 && OptGreyOutZeroes)
                        ImGui::TextDisabled("00 ");
                    else
                        ImGui::Text((OptUpperCaseHex ? "%02X " : "%02x "), b);
                    if (!ReadOnly && ImGui::IsItemHovered() && ImGui::IsMouseClicked(0))
                    {
                        DataEditingTakeFocus = true;
                        data_editing_addr_next = addr;
                    }
                }
            }

            // Draw disassembly view
            ImVec2 window_pos = ImGui::GetWindowPos();
            draw_list->AddLine(ImVec2(window_pos.x + s.PosDisasmStart - s.GlyphWidth, window_pos.y), ImVec2(window_pos.x + s.PosDisasmStart - s.GlyphWidth, window_pos.y + 9999), ImGui::GetColorU32(ImGuiCol_Border));

            ImGui::SameLine(s.PosDisasmStart);
            auto pos = ImGui::GetCursorScreenPos();
            ImGui::PushID(line_i);
            ImGui::InvisibleButton("disasm", ImVec2(s.PosDisasmEnd - s.PosDisasmStart, s.LineHeight));
            ImGui::PopID();
            const char teststr [] = "Hello";
            ImGui::GetWindowDrawList()->AddText(pos, ImGui::GetColorU32(ImGuiCol_Text), teststr, teststr+sizeof(teststr)-1);
        }
        clipper.End();
        ImGui::PopStyleVar(2);
        ImGui::EndChild();

        if (data_next && DataEditingAddr < mem_size)
        {
            DataEditingAddr = DataEditingAddr + 1;
            DataEditingTakeFocus = true;
        }
        else if (data_editing_addr_next != std::numeric_limits<std::size_t>::max())
        {
            DataEditingAddr = data_editing_addr_next;
        }

        if (OptShowOptions)
        {
            ImGui::Separator();
            DrawOptionsLine(s, mem_data, mem_size, base_display_addr);
        }


        // Notify the main window of our ideal child content size (FIXME: we are missing an API to get the contents size from the child)
        ImGui::SetCursorPosX(s.WindowWidth);
    }

    void DrawOptionsLine(const Sizes& s, void* mem_data, size_t mem_size, size_t base_display_addr)
    {
        IM_UNUSED(mem_data);
        ImGuiStyle& style = ImGui::GetStyle();

        // Options menu
        if (ImGui::Button("Options"))
            ImGui::OpenPopup("context");
        if (ImGui::BeginPopup("context"))
        {
            ImGui::PushItemWidth(56);
            ImGui::PopItemWidth();
            ImGui::Checkbox("Grey out zeroes", &OptGreyOutZeroes);
            ImGui::Checkbox("Uppercase Hex", &OptUpperCaseHex);

            ImGui::EndPopup();
        }

        ImGui::SameLine();
        ImGui::Text((OptUpperCaseHex ? "Range %0*zX..%0*zX" : "Range %0*zx..%0*zx"), s.AddrDigitsCount, base_display_addr, s.AddrDigitsCount, base_display_addr + mem_size - 1);
        ImGui::SameLine();
        ImGui::PushItemWidth(static_cast<float>(s.AddrDigitsCount + 1) * s.GlyphWidth + style.FramePadding.x * 2.0f);
        if (ImGui::InputText("##addr", AddrInputBuf, 32, ImGuiInputTextFlags_CharsHexadecimal | ImGuiInputTextFlags_EnterReturnsTrue))
        {
            size_t goto_addr;
            if (sscanf(AddrInputBuf, "%zX", &goto_addr) == 1)
            {
                GotoAddr = goto_addr - base_display_addr;
                HighlightMin = HighlightMax = std::numeric_limits<std::size_t>::max();
            }
        }
        ImGui::PopItemWidth();

        if (GotoAddr != std::numeric_limits<std::size_t>::max())
        {
            if (GotoAddr < mem_size)
            {
                ImGui::BeginChild("##scrolling");
                ImGui::SetScrollFromPosY(ImGui::GetCursorStartPos().y + static_cast<float>(GotoAddr / MaxCols) * ImGui::GetTextLineHeight()); // NOLINT(bugprone-integer-division)
                ImGui::EndChild();
                DataEditingAddr = GotoAddr;
                DataEditingTakeFocus = true;
            }
            GotoAddr = std::numeric_limits<std::size_t>::max();
        }
    }
};

#endif

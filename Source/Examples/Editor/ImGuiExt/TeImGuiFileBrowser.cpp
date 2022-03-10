#include "../ImGuiExt/TeImGuiFileBrowser.h"

#ifndef IMGUI_DEFINE_MATH_OPERATORS
#   define IMGUI_DEFINE_MATH_OPERATORS
#endif
#include "ImGui/imgui_internal.h"
#include "../ImGuiExt/TeIconsFontAwesome5.h"
#include "../ImGuiExt/TeImGuiExt.h"
#include "Image/TeTexture.h"
#include "String/TeUnicode.h"

#include <iostream>
#include <functional>
#include <climits>
#include <string.h>
#include <sstream>
#include <cwchar>
#include <cctype>
#include <algorithm>
#include <cmath>
#if TE_PLATFORM == TE_PLATFORM_WIN32
#   ifndef NOMINMAX
        #define NOMINMAX
#   endif

#   pragma warning(push, 0)
#   include "../ImGuiExt/dirent.h"
#   pragma warning(pop)
#   include <windows.h>
#else
#   include <dirent.h>
#endif

namespace te
{
    const Vector<String> ImGuiFileBrowser::_texturesExtensions = { ".png", ".jpeg", ".jpg", ".dds", ".tiff", ".tga" };
    const Vector<String> ImGuiFileBrowser::_meshesExtensions = { ".obj", ".dae", ".fbx", ".stl", ".gltf" };
    const Vector<String> ImGuiFileBrowser::_soundsExtensions = { ".ogg", ".wav", ".flac" };

    ImGuiFileBrowser::ImGuiFileBrowser()
    {
        filter_mode = FilterMode_Files | FilterMode_Dirs;

        show_inputbar_combobox = false;
        validate_file = false;
        show_hidden = false;
        is_dir = false;
        filter_dirty = true;
        is_appearing = true;
        Data.IsCancelled = false;
        parameters_step_done = false;

        col_items_limit = 12;
        selected_idx = -1;
        selected_ext_idx = 0;
        ext_box_width = -1.0f;
        col_width = 280.0f;
        min_size = ImVec2(500,300);

        invfile_modal_id = "Invalid File!";
        repfile_modal_id = "Replace File?";
        parameters_file_modal_id = "Resource Parameters";
        Data.SelectedFileName = "";
        Data.SelectedFileExt = "";
        Data.SelectedPath = "";
        input_fn[0] = '\0';
        dialog_mode = DialogMode::OPEN;

#if TE_PLATFORM == TE_PLATFORM_WIN32
        current_path = "./";
#else
        InitCurrentPath();
#endif
    }

    ImGuiFileBrowser::~ImGuiFileBrowser()
    { }

    void ImGuiFileBrowser::ClearFileList()
    {
        //Clear pointer references to subdirs and subfiles
        filtered_dirs.clear();
        filtered_files.clear();
        inputcb_filter_files.clear();

        //Now clear subdirs and subfiles
        subdirs.clear();
        subfiles.clear();
        filter_dirty = true;
        selected_idx = -1;
    }

    void ImGuiFileBrowser::CloseDialog()
    {
        valid_exts.clear();
        selected_ext_idx = 0;
        selected_idx = -1;

        input_fn[0] = '\0';  //Hide any text in Input bar for the next time save dialog is opened.
        filter.Clear();     //Clear Filter for the next time open dialog is called.

        show_inputbar_combobox = false;
        validate_file = false;
        show_hidden = false;
        is_dir = false;
        filter_dirty = true;
        is_appearing = true;
        parameters_step_done = false;

        //Clear pointer references to subdirs and subfiles
        filtered_dirs.clear();
        filtered_files.clear();
        inputcb_filter_files.clear();

        //Now clear subdirs and subfiles
        subdirs.clear();
        subfiles.clear();

        ImGui::CloseCurrentPopup();
    }

    bool ImGuiFileBrowser::ShowFileDialog(const String& label, const DialogMode mode, const ImVec2& sz_xy, bool parameters_step, const String& valid_types)
    {
        dialog_mode = mode;
        ImGuiIO& io = ImGui::GetIO();
        max_size.x = io.DisplaySize.x;
        max_size.y = io.DisplaySize.y;
        ImGui::SetNextWindowSizeConstraints(min_size, max_size);
        ImGui::SetNextWindowPos(ImGui::GetMainViewport()->GetCenter(), ImGuiCond_Appearing, ImVec2(0.5f,0.5f));
        ImGui::SetNextWindowSize(ImVec2(std::max(sz_xy.x, min_size.x), std::max(sz_xy.y, min_size.y)), ImGuiCond_Appearing);

        //Set Proper Filter Mode.
        if(mode == DialogMode::SELECT)
            filter_mode = FilterMode_Dirs;
        else
            filter_mode = FilterMode_Files | FilterMode_Dirs;

        if (ImGui::BeginPopupModal(label.c_str(), nullptr, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse))
        {
            bool show_error = false;
            Data.IsCancelled = false;

            // If this is the initial run, read current directory and load data once.
            if(is_appearing)
            {
                Data.SelectedFileName.clear();
                Data.SelectedFileExt.clear();
                Data.SelectedPath.clear();
                if(mode != DialogMode::SELECT)
                {
                    SetValidExtTypes(valid_types);
                }

                /* If current path is empty (can happen on Windows if user closes dialog while inside MyComputer.
                 * Since this is a virtual folder, path would be empty) load the drives on Windows else initialize the current path on Unix.
                 */
                if(current_path.empty())
                {
#if TE_PLATFORM == TE_PLATFORM_WIN32
                    show_error |= !(LoadWindowsDrives());
#else
                    InitCurrentPath();
                    show_error |= !(ReadDIR(current_path));
#endif
                }
                else
                    show_error |= !(ReadDIR(current_path));
                is_appearing = false;
            }

            show_error |= RenderNavAndSearchBarRegion();
            show_error |= RenderFileListRegion();
            show_error |= RenderInputTextAndExtRegion();
            show_error |= RenderButtonsAndCheckboxRegion();

            if(validate_file)
            {
                validate_file = false;
                bool check = ValidateFile();

                if(!check && dialog_mode == DialogMode::OPEN)
                {
                    ImGui::OpenPopup(invfile_modal_id.c_str());
                    Data.SelectedFileName.clear();
                    Data.SelectedFileExt.clear();
                    Data.SelectedPath.clear();
                }
                else if(!check && dialog_mode == DialogMode::SAVE)
                {
                    ImGui::OpenPopup(repfile_modal_id.c_str());
                }
                else if(!check && dialog_mode == DialogMode::SELECT)
                {
                    Data.SelectedFileName.clear();
                    Data.SelectedFileExt.clear();
                    Data.SelectedPath.clear();
                    show_error = true;
                    error_title = "Invalid Directory!";
                    error_msg = "Invalid Directory Selected. Please make sure the directory exists.";
                }

                //If selected file passes through validation check, set path to the file and close file dialog
                if(check)
                {
                    if(dialog_mode != DialogMode::SELECT && parameters_step && !parameters_step_done)
                    {
                        ImGui::OpenPopup(parameters_file_modal_id.c_str());
                    }
                    else
                    {
                        Data.SelectedPath = current_path + Data.SelectedFileName;

                        //Add a trailing "/" to emphasize its a directory not a file. If you want just the dir name it's accessible through "SelectedFileName"
                        if(dialog_mode == DialogMode::SELECT)
                            Data.SelectedPath += "/";

                        CloseDialog();
                    }
                }
            }

            // We don't need to check as the modals will only be shown if OpenPopup is called
            ShowInvalidFileModal();

            if(ShowReplaceFileModal())
                CloseDialog();

            if (ShowParametersFileModal())
                CloseDialog();

            //Show Error Modal if there was an error opening any directory
            if(show_error)
                ImGui::OpenPopup(error_title.c_str());
            ShowErrorModal();

            ImGui::EndPopup();
            return (!Data.SelectedFileName.empty() && !Data.SelectedFileExt.empty() &&  !Data.SelectedPath.empty());
        }
        else
            return false;
    }

    bool ImGuiFileBrowser::RenderNavAndSearchBarRegion()
    {
        ImGuiStyle& style = ImGui::GetStyle();
        bool show_error = false;
        float frame_height = ImGui::GetFrameHeight();
        float list_item_height = GImGui->FontSize + style.ItemSpacing.y;

        ImVec2 pw_content_size = ImGui::GetWindowSize() - style.WindowPadding * 2.0;
        ImVec2 sw_size = ImVec2(ImGui::CalcTextSize("Random").x + 140, style.WindowPadding.y * 2.0f + frame_height);
        ImVec2 sw_content_size = sw_size - style.WindowPadding * 2.0;
        ImVec2 nw_size = ImVec2(pw_content_size.x - style.ItemSpacing.x - sw_size.x, sw_size.y);


        ImGui::BeginChild("##NavigationWindow", nw_size, true, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoScrollbar);

        for(int i = 0; i < (int)current_dirlist.size(); i++)
        {
            if( ImGui::Button(current_dirlist[i].c_str()) )
            {
                //If last button clicked, nothing happens
                if(i != (int)current_dirlist.size() - 1)
                    show_error |= !(OnNavigationButtonClick(i));
            }

            //Draw Arrow Buttons
            if(i != (int)current_dirlist.size() - 1)
            {
                ImGui::SameLine(0,0);
                float next_label_width = ImGui::CalcTextSize(current_dirlist[i+1].c_str()).x;

                if(i+1 < (int)current_dirlist.size() - 1)
                    next_label_width += frame_height + ImGui::CalcTextSize(ICON_FA_CHEVRON_CIRCLE_RIGHT).x;

                if(ImGui::GetCursorPosX() + next_label_width >= (nw_size.x - style.WindowPadding.x * 3.0))
                {
                    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(1.0f, 1.0f, 1.0f, 0.01f));
                    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 1.0f,1.0f));

                    //Render a drop down of navigation items on button press
                    if(ImGui::Button(ICON_FA_CHEVRON_CIRCLE_RIGHT))
                        ImGui::OpenPopup("##NavBarDropboxPopup");
                    if(ImGui::BeginPopup("##NavBarDropboxPopup"))
                    {
                        ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0.125f, 0.125f, 0.125f, 1.0f));
                        if(ImGui::ListBoxHeader("##NavBarDropBox", ImVec2(0, list_item_height* 5)))
                        {
                            for(int j = i+1; j < (int)current_dirlist.size(); j++)
                            {
                                if(ImGui::Selectable(current_dirlist[j].c_str(), false) && j != (int)current_dirlist.size() - 1)
                                {
                                    show_error |= !(OnNavigationButtonClick(j));
                                    ImGui::CloseCurrentPopup();
                                }
                            }
                            ImGui::ListBoxFooter();
                        }
                        ImGui::PopStyleColor();
                        ImGui::EndPopup();
                    }
                    ImGui::PopStyleColor(2);
                    break;
                }
                else
                {
                    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(1.0f, 1.0f, 1.0f, 0.01f));
                    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 1.0f,1.0f));
                    ImGui::ArrowButtonEx("##Right", ImGuiDir_Right, ImVec2(frame_height, frame_height), ImGuiButtonFlags_Disabled);
                    ImGui::SameLine(0,0);
                    ImGui::PopStyleColor(2);
                }
            }
        }
        ImGui::EndChild();

        ImGui::SameLine();
        ImGui::BeginChild("##SearchWindow", sw_size, true, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoScrollbar);

        //Render Search/Filter bar
        float marker_width = ImGui::CalcTextSize("(?)").x + style.ItemSpacing.x;
        if(filter.Draw("##SearchBar", sw_content_size.x - marker_width) || filter_dirty )
            FilterFiles(filter_mode);

        //If filter bar was focused clear selection
        if(ImGui::GetFocusID() == ImGui::GetID("##SearchBar"))
            selected_idx = -1;

        ImGui::SameLine();
        ShowHelpMarker("Filter (inc, -exc)");

        ImGui::EndChild();
        return show_error;
    }

    bool ImGuiFileBrowser::RenderFileListRegion()
    {
        ImGuiStyle& style = ImGui::GetStyle();
        ImVec2 pw_size = ImGui::GetWindowSize();
        bool show_error = false;
        float list_item_height = ImGui::CalcTextSize("").y + style.ItemSpacing.y;
        float input_bar_ypos = pw_size.y - ImGui::GetFrameHeightWithSpacing() * 2.5f - style.WindowPadding.y;
        float window_height = input_bar_ypos - ImGui::GetCursorPosY() - style.ItemSpacing.y;
        float window_content_height = window_height - style.WindowPadding.y * 2.0f;
        float min_content_size = pw_size.x - style.WindowPadding.x * 4.0f;

        if(window_content_height <= 0.0f)
            return show_error;

        //Reinitialize the limit on number of selectables in one column based on height
        col_items_limit = static_cast<int>(std::max(1.0f, window_content_height/list_item_height));
        int num_cols = static_cast<int>(std::max(1.0f, std::ceil(static_cast<float>(filtered_dirs.size() + filtered_files.size()) / col_items_limit)));

        //Limitation by ImGUI in 1.75. If columns are greater than 64 readjust the limit on items per column and recalculate number of columns
        if(num_cols > 64)
        {
            int exceed_items_amount = (num_cols - 64) * col_items_limit;
            col_items_limit += static_cast<int>(std::ceil(exceed_items_amount/64.0));
            num_cols = static_cast<int>(std::max(1.0f, std::ceil(static_cast<float>(filtered_dirs.size() + filtered_files.size()) / col_items_limit)));
        }

        float content_width = num_cols * col_width;
        if(content_width < min_content_size)
            content_width = 0;

        ImGui::SetNextWindowContentSize(ImVec2(content_width, 0));
        ImGui::BeginChild("##ScrollingRegion", ImVec2(0, window_height), true, ImGuiWindowFlags_HorizontalScrollbar);
        ImGui::Columns(num_cols);

        int items = 0;
        for (int i = 0; i < (int)filtered_dirs.size(); i++)
        {
            if(!filtered_dirs[i]->is_hidden || show_hidden)
            {
                String label = filtered_dirs[i]->name;

                if (label.size() > 2 || (label != ".." && label != "."))
                    label = ICON_FA_FOLDER + String(" ") + label;

                items++;
                if(ImGui::Selectable(label.c_str(), selected_idx == i && is_dir, ImGuiSelectableFlags_AllowDoubleClick))
                {
                    selected_idx = i;
                    is_dir = true;

                    // If dialog mode is SELECT then copy the selected dir name to the input text bar
                    if(dialog_mode == DialogMode::SELECT)
                        strcpy(input_fn, filtered_dirs[i]->name.c_str());

                    if(ImGui::IsMouseDoubleClicked(0))
                    {
                        show_error |= !(OnDirClick(i));
                        break;
                    }
                }
                if( (items) % col_items_limit == 0)
                    ImGui::NextColumn();
            }
        }

        //Output files
        for (int i = 0; i < (int)filtered_files.size(); i++)
        {
            if(!filtered_files[i]->is_hidden || show_hidden)
            {
                String label;

                if (std::find(_texturesExtensions.begin(), _texturesExtensions.end(), filtered_files[i]->extension) != _texturesExtensions.end())
                    label = ICON_FA_FILE_IMAGE + String(" ") + (filtered_files[i]->name);
                else if (std::find(_meshesExtensions.begin(), _meshesExtensions.end(), filtered_files[i]->extension) != _meshesExtensions.end())
                    label = ICON_FA_FILE_ARCHIVE + String(" ") + (filtered_files[i]->name);
                else if (std::find(_soundsExtensions.begin(), _soundsExtensions.end(), filtered_files[i]->extension) != _soundsExtensions.end())
                    label = ICON_FA_FILE_AUDIO + String(" ") + (filtered_files[i]->name);
                else if (filtered_files[i]->extension == ".cpp")
                    label = ICON_FA_FILE_CODE + String(" ") + (filtered_files[i]->name);
                else if (filtered_files[i]->extension == ".scene")
                    label = ICON_FA_FILE_SIGNATURE + String(" ") + (filtered_files[i]->name);
                else
                    label = ICON_FA_FILE + String(" ") + (filtered_files[i]->name);

                items++;
                if(ImGui::Selectable(label.c_str(), selected_idx == i && !is_dir, ImGuiSelectableFlags_AllowDoubleClick))
                {
                    //int len = filtered_files[i]->name.length();
                    selected_idx = i;
                    is_dir = false;

                    // If dialog mode is OPEN/SAVE then copy the selected file name to the input text bar
                    strcpy(input_fn, filtered_files[i]->name.c_str());

                    if(ImGui::IsMouseDoubleClicked(0))
                    {
                        Data.SelectedFileName = filtered_files[i]->name;
                        Data.SelectedFileExt  = filtered_files[i]->extension;
                        validate_file = true;
                    }
                }
                if( (items) % col_items_limit == 0)
                    ImGui::NextColumn();
            }
        }
        ImGui::Columns(1);
        ImGui::EndChild();

        return show_error;
    }

    bool ImGuiFileBrowser::RenderInputTextAndExtRegion()
    {
        ImGuiStyle& style = ImGui::GetStyle();
        ImVec2 pw_pos = ImGui::GetWindowPos();
        ImVec2 pw_content_sz = ImGui::GetWindowSize() - style.WindowPadding * 2.0;
        ImVec2 cursor_pos = ImGui::GetCursorPos();

        if(ext_box_width < 0.0)
            ext_box_width = ImGui::CalcTextSize(".abc").x + 100;
        float frame_height_spacing = ImGui::GetFrameHeightWithSpacing();
        float input_bar_width = pw_content_sz.x;
        if(dialog_mode != DialogMode::SELECT)
            input_bar_width -= (ext_box_width + style.ItemSpacing.x);

        bool show_error = false;
        ImGui::SetCursorPosY(pw_content_sz.y - frame_height_spacing * 2.0f);

        //Render Input Text Bar
        input_combobox_pos = ImVec2(pw_pos + ImGui::GetCursorPos());
        input_combobox_sz = ImVec2(input_bar_width, 0);
        ImGui::PushItemWidth(input_bar_width);
        if(ImGui::InputTextWithHint("##FileNameInput", "Type a name...", &input_fn[0], 256, ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_AutoSelectAll))
        {
            if(strlen(input_fn) > 0)
            {
                Data.SelectedFileName = String(input_fn);
                Data.SelectedFileExt = "." + Data.SelectedFileName.substr(Data.SelectedFileName.find_last_of(".") + 1);
                validate_file = true;
            }
        }
        ImGui::PopItemWidth();

        //If input bar was focused clear selection
        if(ImGui::IsItemEdited())
            selected_idx = -1;

        // If Input Bar is edited show a list of files or dirs matching the input text.
        if(ImGui::IsItemEdited() || ImGui::IsItemActivated())
        {
            //If dialog_mode is OPEN/SAVE then filter from list of files..
            if(dialog_mode == DialogMode::OPEN || dialog_mode == DialogMode::SAVE)
            {
                inputcb_filter_files.clear();
                for(int i = 0; i < (int)subfiles.size(); i++)
                {
                    if(ImStristr(subfiles[i].name.c_str(), nullptr, input_fn, nullptr) != nullptr)
                        inputcb_filter_files.push_back(std::ref(subfiles[i].name));
                }
            }

            //If dialog_mode == SELECT then filter from list of directories
            else if(dialog_mode == DialogMode::SELECT)
            {
                inputcb_filter_files.clear();
                for(int i = 0; i < (int)subdirs.size(); i++)
                {
                    if(ImStristr(subdirs[i].name.c_str(), nullptr, input_fn, nullptr) != nullptr)
                        inputcb_filter_files.push_back(std::ref(subdirs[i].name));
                }
            }

            //If filtered list has any items show dropdown
            if(inputcb_filter_files.size() > 0)
                show_inputbar_combobox = true;
            else
                show_inputbar_combobox = false;
        }

        //Render Extensions and File Types DropDown
        if(dialog_mode != DialogMode::SELECT)
        {
            ImGui::SameLine();
            RenderExtBox();
        }

        //Render a Drop Down of files/dirs (depending on mode) that have matching characters as the input text only.
        show_error |= RenderInputComboBox();

        ImGui::SetCursorPos(cursor_pos);
        return show_error;
    }

    bool ImGuiFileBrowser::RenderButtonsAndCheckboxRegion()
    {
        ImVec2 pw_size = ImGui::GetWindowSize();
        ImGuiStyle& style = ImGui::GetStyle();
        bool show_error = false;
        float frame_height = ImGui::GetFrameHeight();
        float frame_height_spacing = ImGui::GetFrameHeightWithSpacing();
        float opensave_btn_width = GetButtonSize("Open").x;     // Since both Open/Save are 4 characters long, width gonna be same.
        float selcan_btn_width = GetButtonSize("Cancel").x;     // Since both Cacnel/Select have same number of characters, so same width.
        float buttons_xpos;

        if (dialog_mode == DialogMode::SELECT)
            buttons_xpos = pw_size.x - opensave_btn_width - (2.0f * selcan_btn_width) - ( 2.0f * style.ItemSpacing.x) - style.WindowPadding.x;
        else
            buttons_xpos = pw_size.x - opensave_btn_width - selcan_btn_width - style.ItemSpacing.x - style.WindowPadding.x;

        ImGui::SetCursorPosY(pw_size.y - frame_height_spacing - style.WindowPadding.y);

        //Render Checkbox
        float label_width = ImGui::CalcTextSize("Show Hidden Files and Folders").x + ImGui::GetCursorPosX() + frame_height;
        bool show_marker = (label_width >= buttons_xpos);
        ImGui::Checkbox( (show_marker) ? "##showHiddenFiles" : "Show Hidden Files and Folders", &show_hidden);
        if(show_marker)
        {
            ImGui::SameLine();
            ShowHelpMarker("Show Hidden Files and Folders");
        }

        //Render an Open Button (in OPEN/SELECT dialog_mode) or Open/Save depending on what's selected in SAVE dialog_mode
        ImGui::SameLine();
        ImGui::SetCursorPosX(buttons_xpos);
        if(dialog_mode == DialogMode::SAVE)
        {
            // If directory selected and Input Text Bar doesn't have focus, render Open Button
            if(selected_idx != -1 && is_dir && ImGui::GetFocusID() != ImGui::GetID("##FileNameInput"))
            {
                if (ImGui::Button("Open"))
                    show_error |= !(OnDirClick(selected_idx));
            }
            else if (ImGui::Button("Save") && strlen(input_fn) > 0)
            {
                Data.SelectedFileName = String(input_fn);
                Data.SelectedFileExt = "." + Data.SelectedFileName.substr(Data.SelectedFileName.find_last_of(".") + 1);
                validate_file = true;
            }
        }
        else
        {
            if (ImGui::Button("Open"))
            {
                //It's possible for both to be true at once (user selected directory but input bar has some text. In this case we chose to open the directory instead of opening the file.
                //Also note that we don't need to access the selected file through "selected_idx" since the if a file is selected, input bar will get populated with that name.
                if(selected_idx >= 0 && is_dir)
                    show_error |= !(OnDirClick(selected_idx));
                else if(strlen(input_fn) > 0)
                {
                    Data.SelectedFileName = String(input_fn);
                    Data.SelectedFileExt = "." + Data.SelectedFileName.substr(Data.SelectedFileName.find_last_of(".") + 1);
                    validate_file = true;
                }
            }

            //Render Select Button if in SELECT Mode
            if(dialog_mode == DialogMode::SELECT)
            {
                //Render Select Button
                ImGui::SameLine();
                if (ImGui::Button("Select"))
                {
                    if(strlen(input_fn) > 0)
                    {
                        Data.SelectedFileName = String(input_fn);
                        Data.SelectedFileExt = "." + Data.SelectedFileName.substr(Data.SelectedFileName.find_last_of(".") + 1);
                        validate_file = true;
                    }
                }
            }
        }

        //Render Cancel Button
        ImGui::SameLine();
        if (ImGui::Button("Cancel"))
        {
            Data.IsCancelled = true;
            CloseDialog();
        }

        return show_error;
    }

    bool ImGuiFileBrowser::RenderInputComboBox()
    {
        bool show_error = false;
        ImGuiStyle& style = ImGui::GetStyle();
        ImGuiID input_id =  ImGui::GetID("##FileNameInput");
        ImGuiID focus_scope_id = ImGui::GetID("##InputBarComboBoxListScope");
        float frame_height = ImGui::GetFrameHeight();

        input_combobox_sz.y = std::min((inputcb_filter_files.size() + 1) * frame_height + style.WindowPadding.y *  2.0f,
                                        8 * ImGui::GetFrameHeight() + style.WindowPadding.y *  2.0f);

        if(show_inputbar_combobox && ( ImGui::GetFocusScopeID() == focus_scope_id || ImGui::GetCurrentContext()->ActiveIdIsAlive == input_id  ))
        {
            ImGuiWindowFlags popupFlags = ImGuiWindowFlags_NoTitleBar           |
                                          ImGuiWindowFlags_NoResize             |
                                          ImGuiWindowFlags_NoMove               |
                                          ImGuiWindowFlags_NoFocusOnAppearing   |
                                          ImGuiWindowFlags_NoScrollbar          |
                                          ImGuiWindowFlags_NoSavedSettings;


            ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0.1f, 0.1f, 0.1f, 1.0f));
            ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0.125f, 0.125f, 0.125f, 1.0f));
            ImGui::SetNextWindowBgAlpha(1.0);
            ImGui::SetNextWindowPos(input_combobox_pos + ImVec2(0, ImGui::GetFrameHeightWithSpacing()));
            ImGui::PushClipRect(ImVec2(0,0), ImGui::GetIO().DisplaySize, false);

            ImGui::BeginChild("##InputBarComboBox", input_combobox_sz, true, popupFlags);

            ImVec2 listbox_size = input_combobox_sz - ImGui::GetStyle().WindowPadding * 2.0f;
            if(ImGui::ListBoxHeader("##InputBarComboBoxList", listbox_size))
            {
                ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 1.0f,1.0f));
                ImGui::PushFocusScope(focus_scope_id);
                for(auto& element : inputcb_filter_files)
                {
                    if(ImGui::Selectable(element.get().c_str(), false, ImGuiSelectableFlags_NoHoldingActiveID | ImGuiSelectableFlags_SelectOnClick))
                    {
                        if(element.get().size() > 256)
                        {
                            error_title = "Error!";
                            error_msg = "Selected File Name is longer than 256 characters.";
                            show_error = true;
                        }
                        else
                        {
                            strcpy(input_fn, element.get().c_str());
                            show_inputbar_combobox = false;
                        }
                    }
                }
                ImGui::PopFocusScope();
                ImGui::PopStyleColor(1);
                ImGui::ListBoxFooter();
            }
            ImGui::EndChild();
            ImGui::PopStyleColor(2);
            ImGui::PopClipRect();
        }
        return show_error;
    }

    bool ImGuiFileBrowser::ShowParametersFileModal()
    {
        ImVec2 window_size(400, 0);
        bool ret_val = false;

        ImGui::SetNextWindowSize(window_size);
        ImGui::SetNextWindowPos(ImGui::GetMainViewport()->GetCenter(), ImGuiCond_Appearing, ImVec2(0.5f,0.5f));

        if (ImGui::BeginPopupModal(parameters_file_modal_id.c_str(), nullptr, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoResize))
        {
            String ext = (Data.SelectedFileExt);

            if (dialog_mode == DialogMode::OPEN)
            {
                if (ext == ".obj" || ext == ".dae" || ext == ".fbx" || ext == ".stl" || ext == ".gltf")
                {
                    ImGuiExt::RenderOptionBool(Data.MeshParam.ImportNormals, "##file_dialog_parameters_mesh_normals", "Import normals");
                    ImGuiExt::RenderOptionBool(Data.MeshParam.ImportTangents, "##file_dialog_parameters_mesh_tangents", "Import tangents");
                    ImGuiExt::RenderOptionBool(Data.MeshParam.ImportUVCoords, "##file_dialog_parameters_mesh_uvs", "Import UVs");
                    ImGuiExt::RenderOptionBool(Data.MeshParam.ImportMaterials, "##file_dialog_parameters_mesh_material", "Import materials");
                    ImGuiExt::RenderOptionBool(Data.MeshParam.ImportVertexColors, "##file_dialog_parameters_mesh_vertex_colors", "Import vertex colors");
                    ImGui::Separator();

                    if (Data.MeshParam.ImportNormals)
                    {
                        ImGuiExt::RenderOptionBool(Data.MeshParam.ForceGenNormals, "##file_dialog_parameters_mesh_force_gen_normals", "Force normals generation");
                        ImGuiExt::RenderOptionBool(Data.MeshParam.GenSmoothNormals, "##file_dialog_parameters_mesh_force_gen_smooth_normal", "Generate smooth normals");
                        ImGui::Separator();
                    }

                    ImGuiExt::RenderOptionBool(Data.MeshParam.ImportSkin, "##file_dialog_parameters_mesh_skin", "Import skin");
                    ImGuiExt::RenderOptionBool(Data.MeshParam.ImportBlendShapes, "##file_dialog_parameters_mesh_blend_shapes", "Import blend shapes");
                    ImGuiExt::RenderOptionBool(Data.MeshParam.ImportAnimations, "##file_dialog_parameters_mesh_animation", "Import animations");
                    if (Data.MeshParam.ImportAnimations)
                    {
                        ImGui::Indent(30.0f);
                        ImGuiExt::RenderOptionBool(Data.MeshParam.ReduceKeyFrames, "##file_dialog_parameters_mesh_animation_reduce", "Reduce Key Frames");
                        ImGui::Unindent(30.0f);
                    }
                    ImGui::Separator();

                    ImGuiExt::RenderOptionBool(Data.MeshParam.ScaleSystemUnit, "##file_dialog_parameters_mesh_scale_system_unit", "Scale system unit");
                    if (Data.MeshParam.ScaleSystemUnit)
                    {
                        ImGui::Indent(30.0f);
                        ImGuiExt::RenderOptionFloat(Data.MeshParam.ScaleFactor, "##file_dialog_parameters_mesh_scaleFactor", "Scale Factor", 0.001f, 1000.0f, 100.0f);
                        ImGui::Unindent(30.0f);
                    }
                    ImGui::Separator();

                    ImGuiExt::RenderOptionBool(Data.MeshParam.ImportCollisionShape, "##file_dialog_parameters_mesh_import_collision_shape", "Import Collision Shape");
                }
                else if (ext == ".jpg" || ext == ".jpeg" || ext == ".png" || ext == ".dds" || ext == ".tiff" ||ext == ".tga")
                {
                    static ImGuiExt::ComboOptions<TextureType> textureTypeOptions;
                    if (textureTypeOptions.Options.size() == 0)
                    {
                        textureTypeOptions.AddOption(TextureType::TEX_TYPE_2D, "Texture 2D");
                        textureTypeOptions.AddOption(TextureType::TEX_TYPE_CUBE_MAP, "Cubemap");
                    }

                    ImGuiExt::RenderOptionCombo<TextureType>(&Data.TexParam.TexType, "##file_dialog_parameters_texture_type", "Texture type", textureTypeOptions, 300);

                    if(Data.TexParam.TexType == TextureType::TEX_TYPE_2D)
                    {
                        ImGuiExt::RenderOptionBool(Data.TexParam.GenerateMips, "##file_dialog_parameters_texture_generate_mips", "Generate MipMaps");

                        if(Data.TexParam.GenerateMips)
                        {
                            static ImGuiExt::ComboOptions<UINT32> maxMipsOptions;
                            if (maxMipsOptions.Options.size() == 0)
                            {
                                maxMipsOptions.AddOption(0, "Maximum");
                                maxMipsOptions.AddOption(1, "1");
                                maxMipsOptions.AddOption(2, "2");
                                maxMipsOptions.AddOption(3, "3");
                                maxMipsOptions.AddOption(4, "4");
                                maxMipsOptions.AddOption(5, "5");
                                maxMipsOptions.AddOption(6, "6");
                                maxMipsOptions.AddOption(7, "7");
                                maxMipsOptions.AddOption(8, "8");
                                maxMipsOptions.AddOption(9, "9");
                                maxMipsOptions.AddOption(10, "10");
                            }

                            ImGuiExt::RenderOptionCombo<UINT32>(&Data.TexParam.MaxMips, "##file_dialog_parameters_texture_max_mips", "Max mip level", maxMipsOptions, 300);
                        }
                    }

                    ImGuiExt::RenderOptionBool(Data.TexParam.SRGB, "##file_dialog_texture_srgb", "sRGB");

                    ImGuiExt::RenderOptionBool(Data.TexParam.CpuCached, "##file_dialog_texture_cpu_cached", "CPU cached");
                }
                else if (ext == ".ogg" || ext == ".wav" || ext == ".flac")
                {
                    ImGuiExt::RenderOptionBool(Data.AudioParam.Is3D, "##file_dialog_parameters_audio_3d", "Is 3D Sound");
                }
                else if (ext == ".scene")
                {
                    // TODO .scene file handling
                }
            }
            else if (dialog_mode == DialogMode::SAVE && ext == ".scene")
            { /** TODO */ }

            ImGui::Separator();

            if (ImGui::Button("Next", GetButtonSize("Next")))
            {
                Data.SelectedPath = current_path + Data.SelectedFileName;
                ImGui::CloseCurrentPopup();
                parameters_step_done = true;
                ret_val = true;
            }

            ImGui::SameLine();
            if (ImGui::Button("Cancel", GetButtonSize("Cancel")))
            {
                Data.SelectedFileName.clear();
                Data.SelectedPath.clear();
                ImGui::CloseCurrentPopup();
                ret_val = false;
            }

            ImGui::EndPopup();
        }
        
        return ret_val;
    }

    void ImGuiFileBrowser::RenderExtBox()
    {
        ImGui::PushItemWidth(ext_box_width);
        if(ImGui::BeginCombo("##FileTypes", valid_exts[selected_ext_idx].c_str()))
        {
            for(int i = 0; i < (int)valid_exts.size(); i++)
            {
                if(ImGui::Selectable(valid_exts[i].c_str(), selected_ext_idx == i))
                {
                    selected_ext_idx = i;
                    if(dialog_mode == DialogMode::SAVE)
                    {
                        String name(input_fn);
                        size_t idx = name.find_last_of(".");
                        if(idx == String::npos)
                            idx = strlen(input_fn);
                        for(int j = 0; j < (int)valid_exts[selected_ext_idx].size(); j++)
                            input_fn[idx++] = valid_exts[selected_ext_idx][j];
                        input_fn[idx++] = '\0';
                    }
                    FilterFiles(FilterMode_Files);
                }
            }
            ImGui::EndCombo();
        }
        Data.Ext = valid_exts[selected_ext_idx];
        ImGui::PopItemWidth();
    }

    bool ImGuiFileBrowser::OnNavigationButtonClick(int idx)
    {
        String new_path = "";

        //First Button corresponds to virtual folder Computer which lists all logical drives (hard disks and removables) and "/" on Unix
        if(idx == 0)
        {
#if TE_PLATFORM == TE_PLATFORM_WIN32
            if(!LoadWindowsDrives())
                return false;
            current_path.clear();
            current_dirlist.clear();
            current_dirlist.push_back("Computer");
            return true;
            #else
            new_path = "/";
#endif
        }
        else
        {
#if TE_PLATFORM == TE_PLATFORM_WIN32
            //Clicked on a drive letter?
            if(idx == 1)
                new_path = current_path.substr(0, 3);
            else
            {
                //Start from i=1 since at 0 lies "MyComputer" which is only virtual and shouldn't be read by readDIR
                for (int i = 1; i <= idx; i++)
                    new_path += current_dirlist[i] + "/";
            }
#else
            //Since UNIX absolute paths start at "/", we handle this separately to avoid adding a double slash at the beginning
            new_path += current_dirlist[0];
            for (int i = 1; i <= idx; i++)
                new_path += current_dirlist[i] + "/";
#endif
        }

        if(ReadDIR(new_path))
        {
            current_dirlist.erase(current_dirlist.begin()+idx+1, current_dirlist.end());
            current_path = new_path;
            return true;
        }
        else
            return false;
    }

    bool ImGuiFileBrowser::OnDirClick(int idx)
    {
        String name;
        String new_path(current_path);
        bool drives_shown = false;

#if TE_PLATFORM == TE_PLATFORM_WIN32
        drives_shown = (current_dirlist.size() == 1 && current_dirlist.back() == "Computer");
#endif

        name = filtered_dirs[idx]->name;

        if(name == "..")
        {
            new_path.pop_back(); // Remove trailing '/'
            new_path = new_path.substr(0, new_path.find_last_of('/') + 1); // Also include a trailing '/'
        }
        else
        {
            //Remember we displayed drives on Windows as *Local/Removable Disk: X* hence we need last char only
            if(drives_shown)
                name = String(1, name.back()) + ":";
            new_path += name + "/";
        }

        if(ReadDIR(new_path))
        {
            if(name == "..")
                current_dirlist.pop_back();
            else
                current_dirlist.push_back(name);

             current_path = new_path;
             return true;
        }
        else
           return false;
    }

    bool ImGuiFileBrowser::ReadDIR(String pathdir)
    {
        DIR* dir;
        struct dirent *ent;

        /* If the current directory doesn't exist, and we are opening the dialog for the first time, reset to defaults to avoid looping of showing error modal.
         * An example case is when user closes the dialog in a folder. Then deletes the folder outside. On reopening the dialog the current path (previous) would be invalid.
         */
        dir = opendir(pathdir.c_str());
        if(dir == nullptr && is_appearing)
        {
            current_dirlist.clear();
#if TE_PLATFORM == TE_PLATFORM_WIN32
            current_path = pathdir = "./";
#else
            InitCurrentPath();
            pathdir = current_path;
#endif

            dir = opendir(pathdir.c_str());
        }

        if (dir != nullptr)
        {
#if TE_PLATFORM == TE_PLATFORM_WIN32
            // If we are on Windows and current path is relative then get absolute path from dirent structure
            if(current_dirlist.empty() && pathdir == "./")
            {
                const wchar_t* absolute_path = dir->wdirp->patt;
                String current_directory = ToString(absolute_path);
                std::replace(current_directory.begin(), current_directory.end(), '\\', '/');

                //Remove trailing "*" returned by ** dir->wdirp->patt **
                current_directory.pop_back();
                current_path = current_directory;

                //Create a vector of each directory in the file path for the filepath bar. Not Necessary for linux as starting directory is "/"
                ParsePathTabs(current_path);
            }
#endif

            // store all the files and directories within directory and clear previous entries
            ClearFileList();
            while ((ent = readdir (dir)) != nullptr)
            {
                bool is_hidden = false;
                String name(ent->d_name);

                //Ignore current directory
                if(name == ".")
                    continue;

                //Somehow there is a '..' present in root directory in linux.
#if TE_PLATFORM == TE_PLATFORM_WIN32
                if(name == ".." && pathdir == "/")
                    continue;
#endif

                if(name != "..")
                {
#if TE_PLATFORM == TE_PLATFORM_WIN32
                    String fullDir = pathdir + String(ent->d_name);
                    // IF system file skip it...
                    if (FILE_ATTRIBUTE_SYSTEM & GetFileAttributesA(fullDir.c_str()))
                        continue;
                    if (FILE_ATTRIBUTE_HIDDEN & GetFileAttributesA(fullDir.c_str()))
                        is_hidden = true;
#else
                    if(name[0] == '.')
                        is_hidden = true;
#endif
                }
                //Store directories and files in separate vectors
                if(ent->d_type == DT_DIR)
                    subdirs.push_back(Info(name, "", is_hidden));
                else if(ent->d_type == DT_REG && dialog_mode != DialogMode::SELECT)
                    subfiles.push_back(Info(name, "." + name.substr(name.find_last_of(".") + 1),  is_hidden));
            }
            closedir (dir);
            std::sort(subdirs.begin(), subdirs.end(), AlphaSortComparator);
            std::sort(subfiles.begin(), subfiles.end(), AlphaSortComparator);

            //Initialize Filtered dirs and files
            FilterFiles(filter_mode);
        }
        else
        {
            error_title = "Error!";
            error_msg = "Error opening directory! Make sure the directory exists and you have the proper rights to access the directory.";
            return false;
        }
        return true;
    }

    void ImGuiFileBrowser::FilterFiles(int i_filter_mode)
    {
        filter_dirty = false;
        if(i_filter_mode & FilterMode_Dirs)
        {
            filtered_dirs.clear();
            for (size_t i = 0; i < subdirs.size(); ++i)
            {
                subdirs[i].name = UTF8::FromANSI(subdirs[i].name);

                if(filter.PassFilter(subdirs[i].name.c_str()))
                    filtered_dirs.push_back(&subdirs[i]);
            }
        }
        if(i_filter_mode & FilterMode_Files)
        {
            filtered_files.clear();
            for (size_t i = 0; i < subfiles.size(); ++i)
            {
                subfiles[i].name = UTF8::FromANSI(subfiles[i].name);

                if(valid_exts[selected_ext_idx] == "*.*")
                {
                    if(filter.PassFilter(subfiles[i].name.c_str()))
                        filtered_files.push_back(&subfiles[i]);
                }
                else
                {
                    for (int j = 0; j < valid_exts.size(); j++)
                    {
                        if (filter.PassFilter(subfiles[i].name.c_str()) && (ImStristr(subfiles[i].name.c_str(), nullptr, valid_exts[j].c_str(), nullptr)) != nullptr)
                            filtered_files.push_back(&subfiles[i]);
                    }
                }
            }
        }
    }

    void ImGuiFileBrowser::ShowHelpMarker(String desc)
    {
        ImGui::TextDisabled("(?)");
        if (ImGui::IsItemHovered())
        {
            ImGui::BeginTooltip();
            ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
            ImGui::TextUnformatted(desc.c_str());
            ImGui::PopTextWrapPos();
            ImGui::EndTooltip();
        }
    }

    void ImGuiFileBrowser::ShowErrorModal()
    {
        ImVec2 window_size(260, 0);
        ImGui::SetNextWindowSize(window_size);

        if (ImGui::BeginPopupModal(error_title.c_str(), nullptr, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoResize))
        {
            ImGui::TextWrapped("%s", error_msg.c_str());

            ImGui::Separator();
            ImGui::SetCursorPosX(window_size.x/2.0f - GetButtonSize("OK").x/2.0f);
            if (ImGui::Button("OK", GetButtonSize("OK")))
                ImGui::CloseCurrentPopup();
            ImGui::EndPopup();
        }
    }

    bool ImGuiFileBrowser::ShowReplaceFileModal()
    {
        ImVec2 window_size(250, 0);
        ImGui::SetNextWindowSize(window_size);
        bool ret_val = false;
        if (ImGui::BeginPopupModal(repfile_modal_id.c_str(), nullptr, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoResize))
        {
            String text = "A file with the following filename already exists. Are you sure you want to replace the existing file?";
            ImGui::TextWrapped("%s", text.c_str());

            ImGui::Separator();

            float buttons_width = GetButtonSize("Yes").x + GetButtonSize("No").x + ImGui::GetStyle().ItemSpacing.x;
            ImGui::SetCursorPosX(ImGui::GetCursorPosX() + ImGui::GetWindowWidth()/2.0f - buttons_width/2.0f - ImGui::GetStyle().WindowPadding.x);

            if (ImGui::Button("Yes", GetButtonSize("Yes")))
            {
                Data.SelectedPath = current_path + Data.SelectedFileName;
                ImGui::CloseCurrentPopup();
                ret_val = true;
            }

            ImGui::SameLine();
            if (ImGui::Button("No", GetButtonSize("No")))
            {
                Data.SelectedFileName.clear();
                Data.SelectedPath.clear();
                ImGui::CloseCurrentPopup();
                ret_val = false;
            }
            ImGui::EndPopup();
        }
        return ret_val;
    }

    void ImGuiFileBrowser::ShowInvalidFileModal()
    {
        String text = "Selected file either doesn't exist or is not supported. Please select a file with the following extensions...";
        ImVec2 button_size = GetButtonSize("OK");

        float frame_height = ImGui::GetFrameHeightWithSpacing();
        float cw_content_height = valid_exts.size() * frame_height;
        float cw_height = std::min(4.0f * frame_height, cw_content_height);
        ImVec2 window_size(350, 0);
        ImGui::SetNextWindowSize(window_size);

        if (ImGui::BeginPopupModal(invfile_modal_id.c_str(), nullptr, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoResize))
        {

            ImGui::TextWrapped("%s", text.c_str());
            ImGui::BeginChild("##SupportedExts", ImVec2(0, cw_height), true);
            for(int i = 0; i < (int)valid_exts.size(); i++)
                ImGui::BulletText("%s", valid_exts[i].c_str());
            ImGui::EndChild();

            ImGui::SetCursorPosX(window_size.x/2.0f - button_size.x/2.0f);
            if (ImGui::Button("OK", button_size))
                ImGui::CloseCurrentPopup();
            ImGui::EndPopup();
        }
    }

    void ImGuiFileBrowser::SetValidExtTypes(const String& valid_types_string)
    {
        /* Initialize a list of files extensions that are valid.
         * If the user chooses a file that doesn't match the extensions in the
         * list, we will show an error modal...
         */
        String max_str = "";
        valid_exts.clear();
        String extension = "";
        std::istringstream iss(valid_types_string);
        while(std::getline(iss, extension, ','))
        {
            if(!extension.empty())
            {
                if(max_str.size() < extension.size())
                    max_str = extension;
                valid_exts.push_back(extension);
            }
        }
        float min_width = ImGui::CalcTextSize(".abc").x + 100;
        ext_box_width = std::max(min_width, ImGui::CalcTextSize(max_str.c_str()).x);
    }

    bool ImGuiFileBrowser::ValidateFile()
    {
        bool match = false;

        //If there is an item selected, check if the selected file name (the input filename, in other words) matches the selection.
        if(selected_idx >= 0)
        {
            if(dialog_mode == DialogMode::SELECT)
                match = (filtered_dirs[selected_idx]->name == Data.SelectedFileName);
            else
                match = (filtered_files[selected_idx]->name == Data.SelectedFileName);
        }

        //If the input filename doesn't match we need to explicitly find the input filename..
        if(!match)
        {
            if(dialog_mode == DialogMode::SELECT)
            {
                for(int i = 0; i < (int)subdirs.size(); i++)
                {
                    if(subdirs[i].name == Data.SelectedFileName)
                    {
                        match = true;
                        break;
                    }
                }

            }
            else
            {
                for(int i = 0; i < (int)subfiles.size(); i++)
                {
                    if(subfiles[i].name == Data.SelectedFileName)
                    {
                        match = true;
                        break;
                    }
                }
            }
        }

        // If file doesn't match, return true on SAVE mode (since file doesn't exist, hence can be saved directly) and return false on other modes (since file doesn't exist so cant open/select)
        if(!match)
            return (dialog_mode == DialogMode::SAVE);

        // If file matches, return false on SAVE, we need to show a replace file modal
        if(dialog_mode == DialogMode::SAVE)
            return false;
        // Return true on SELECT, no need to validate extensions
        else if(dialog_mode == DialogMode::SELECT)
            return true;
        else
        {
            // If list of extensions has all types, no need to validate.
            for(auto ext : valid_exts)
            {
                if(ext == "*.*")
                    return true;
            }
            size_t idx = Data.SelectedFileName.find_last_of('.');
            String file_ext = idx == String::npos ? "" : Data.SelectedFileName.substr(idx, Data.SelectedFileName.length() - idx);
            return (std::find(valid_exts.begin(), valid_exts.end(), file_ext) != valid_exts.end());
        }
    }

    ImVec2 ImGuiFileBrowser::GetButtonSize(String button_text)
    {
        return (ImGui::CalcTextSize(button_text.c_str()) + ImGui::GetStyle().FramePadding * 2.0);
    }

    void ImGuiFileBrowser::ParsePathTabs(String path)
    {
        String path_element = "";

#if TE_PLATFORM == TE_PLATFORM_WIN32
        current_dirlist.push_back("Computer");
#else
        if(path[0] == '/')
            current_dirlist.push_back("/");
#endif

        std::istringstream iss(path);
        while(std::getline(iss, path_element, '/'))
        {
            if(!path_element.empty())
                current_dirlist.push_back(path_element);
        }
    }

    bool ImGuiFileBrowser::AlphaSortComparator(const Info& a, const Info& b)
    {
        const char* str1 = a.name.c_str();
        const char* str2 = b.name.c_str();
        int ca, cb;
        do
        {
            ca = (unsigned char) *str1++;
            cb = (unsigned char) *str2++;
            ca = std::tolower(std::toupper(ca));
            cb = std::tolower(std::toupper(cb));
        }
        while (ca == cb && ca != '\0');
        if(ca  < cb)
            return true;
        else
            return false;
    }

    //Windows Exclusive function
#if TE_PLATFORM == TE_PLATFORM_WIN32
    bool ImGuiFileBrowser::LoadWindowsDrives()
    {
        DWORD len = GetLogicalDriveStringsA(0,nullptr);
        char* drives = te_allocate<char>(sizeof(char) * len);
        if(!GetLogicalDriveStringsA(len,drives))
        {
            te_deallocate(drives);
            return false;
        }

        ClearFileList();
        char* temp = drives;
        for(char *drv = nullptr; *temp != '\0'; temp++)
        {
            drv = temp;
            if(DRIVE_REMOVABLE == GetDriveTypeA(drv))
                subdirs.push_back({"Removable Disk: " + String(1,drv[0]), "", false});
            else if(DRIVE_FIXED == GetDriveTypeA(drv))
                subdirs.push_back({"Local Disk: " + String(1,drv[0]), "", false});
            //Go to nullptr character
            while(*(++temp));
        }
        te_deallocate(drives);
        return true;
    }
#endif

    //Unix only
#if TE_PLATFORM == TE_PLATFORM_LINUX
    void ImGuiFileBrowser::InitCurrentPath()
    {
        bool path_max_def = false;

        #ifdef PATH_MAX
        path_max_def = true;
        #endif // PATH_MAX

        char* buffer = nullptr;

        //If PATH_MAX is defined deal with memory using new/delete. Else fallback to malloc'ed memory from `realpath()`
        if(path_max_def)
        {
            buffer = te_allocate<char>(sizeof(char) * PATH_MAX);
        }

        char* real_path = realpath("./", buffer);
        if (real_path == nullptr)
        {
            current_path = "/";
            current_dirlist.push_back("/");
        }
        else
        {
            current_path = String(real_path);
            current_path += "/";
            ParsePathTabs(current_path);
        }

        if(path_max_def)
            te_deallocate((void*)buffer);
        else
            free(real_path);
    }
#endif
}

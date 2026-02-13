#include "LibraryWidget.h"
#include "../services/ThemeController.h"

LibraryWidget::LibraryWidget(Hosting& hosting)
    : _hosting(hosting)
{
}

bool LibraryWidget::render(bool& showWindow) {

    startWidget("Library", showWindow, 782, 10, 400, 800, 350, 400);
    startBody(true);

    if (!showEditForm) {
        renderGameList();
    }
    else {
        renderForm(popupEditIndex);
    }

    endBody();

    startFooter();

    if (!showEditForm) {
        if (elBtn("Add Game")) {
            popupEditIndex = -1;
            memset(_editName, 0, 256);
            memset(_editPath, 0, 256);
            memset(_editParam, 0, 256);
            showEditForm = true;
        }
	} else {
        if (elBtnSecondary("Cancel")) {
            popupEditIndex = -1;
            showEditForm = false;
        }
		ImGui::SameLine();
		ImGui::Indent(70);
        if (elBtn("Save")) {
            GameData game = GameData();
            game.itemID = Cache::cache.gameList.getGames().size() + 1;
            game.name = _editName;
            game.path = _editPath;
            game.parameters = _editParam;

            // Add to game list
            Cache::cache.gameList.getGames().push_back(game);
            Cache::cache.gameList.SaveToFile();

			showEditForm = false;
        }
	}

    endFooter();

    endWidget();
    return true;

}

bool LibraryWidget::renderGameList() {
    
    static bool showRemovePopup = false;
    static bool showEditPopup = false;
    static string popupTitle = "";
    static string name;
    static string path;
    static string parameters;
    static string thumbnailPath;
    static string gameID;
    static vector<GameData>& _games = Cache::cache.gameList.getGames();
    static string filterTextStr;
    static bool filterSuccess = false;
	string defaultID = "1wdoHfhhZH5lPuZCwGBete0HIAj";

	ImGui::BeginGroup();

        ImGui::SetCursorPos(ImVec2(0, 0));
        ImGui::Unindent(20);
        ImGui::BeginChild("filterbar", ImVec2(size.x, 81));
        ImGui::SetCursorPos(ImVec2(0, 0));
        ImDrawList* drawList = ImGui::GetWindowDrawList();
        pos = ImGui::GetCursorScreenPos();
        ImGui::Dummy(ImVec2(0, 10));
        drawList->AddRectFilled(pos, ImVec2(pos.x + size.x, pos.y + 84), ImGui::ColorConvertFloat4ToU32(ImVec4(0, 0, 0, 0.1)), 8, ImDrawFlags_RoundCornersBottom);
        ImGui::Indent(20);
        elText("", _filterText, "Filter by game name");
        ImGui::Unindent(20);
        ImGui::EndChild();
        ImGui::SetCursorPos(ImVec2(0, 84));
        ImGui::Separator();
        ImGui::Indent(10);

		ImGui::Dummy(ImVec2(0, 10));
        ImGui::BeginGroup();

            for (size_t i = 0; i < _games.size(); ++i){

                name = _games[i].name;
                path = _games[i].path;
                parameters = _games[i].parameters;
        
                // Remove game
                IconButton::render(AppIcons::trash, AppColors::primary, ImVec2(20, 20));
                if (ImGui::IsItemActive()) {
                    popupTitle = string("Remove Game##Popup");
                    showRemovePopup = true;
                    popupRemoveIndex = i;
                    ImGui::OpenPopup(popupTitle.c_str());
                }

			    // Edit game
			    ImGui::SameLine();
			    IconButton::render(AppIcons::edit, AppColors::primary, ImVec2(20, 20));
			    if (ImGui::IsItemActive()) {
                    strncpy_s(_editName, _games[i].name.c_str(), 256);
                    strncpy_s(_editPath, _games[i].path.c_str(), 256);
                    strncpy_s(_editParam, _games[i].parameters.c_str(), 256);

				    popupEditIndex = i;
                    showEditForm = true;
			    }

                // Remove game popup
                if (i == popupRemoveIndex) {

                    if (ConfirmPopupWidget::render(
                        popupTitle.c_str(),
                        showRemovePopup,
                        ("Remove\n" + name).c_str()
                    )) {

                        // Remove from game list
                        Cache::cache.gameList.remove(name, [&](GameData& guest) {
		
	                    });
                        popupRemoveIndex = -1;
                        showRemovePopup = false;

                    }
                
                }

                // Game title and path
                ImGui::SameLine();
                ImGui::BeginGroup();
			        ImGui::Indent(10);
                    ImGui::PushStyleColor(ImGuiCol_Text, AppColors::panelText);
                    ImGui::Text("%s", name.c_str());
                    ImGui::PopStyleColor();
                    ImGui::PushStyleColor(ImGuiCol_Text, AppColors::formHelpText);


                    std::string p;
					if (path.length() > 32) {
						p = path.substr(0, 32) + "...";
					}
                    else {
                        p = path;
                    }
                    ImGui::PushFont(AppFonts::input);
				    ImGui::Text("%s", p.c_str());
					ImGui::PopFont();
                    ImGui::PopStyleColor();
				    ImGui::Unindent(10);
                ImGui::EndGroup();

                ImGui::Dummy(ImVec2(0, 5));
            
            }

        ImGui::EndGroup();
    ImGui::EndGroup();

    return true;

}

bool LibraryWidget::renderForm(int index) {

	elText("Game Name", _editName, "Name of the game you want to add.");
    elText("Game Path", _editPath, "Path to the game executable.");

    if (elBtn("Browse...")) {
        std::string filePath = openFileDialog();
        if (!filePath.empty()) {

            // Copy to _editPath
            strncpy_s(_editPath, filePath.c_str(), 256);

        }
    }

    ImGui::Dummy(ImVec2(0, 10));

	elText("Launch Parameters", _editParam, "Parameters to pass to the game executable when launching in kiosk mode.");

    return true;
    
}

std::string LibraryWidget::openFileDialog() {
    OPENFILENAME ofn;
    TCHAR szFile[MAX_PATH] = { 0 };

    ZeroMemory(&ofn, sizeof(ofn));
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = NULL;
    ofn.lpstrFilter = TEXT("Executable Files\0*.exe\0All Files\0*.*\0");
    ofn.lpstrFile = szFile;
    ofn.lpstrFile[0] = '\0';
    ofn.nMaxFile = sizeof(szFile);
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

    if (GetOpenFileName(&ofn) == TRUE) {
        // Convert TCHAR* to std::string
        int size_needed = WideCharToMultiByte(CP_UTF8, 0, ofn.lpstrFile, -1, NULL, 0, NULL, NULL);
        std::string filePath(size_needed, 0);
        WideCharToMultiByte(CP_UTF8, 0, ofn.lpstrFile, -1, &filePath[0], size_needed, NULL, NULL);
        return filePath;
    }
    else {
        // User canceled the dialog or an error occurred
        return std::string();
    }
}



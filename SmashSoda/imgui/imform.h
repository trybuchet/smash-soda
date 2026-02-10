#pragma once

#include "../globals/AppStyle.h"
#include "imgui.h"
#include "../Widgets/ToggleIconButtonWidget.h"
#include "../Widgets/IntRangeWidget.h"

/// <summary>
/// Helper class for making form elements.
/// </summary>
class ImForm {
public:

	/// <summary>
	/// Creates a text input field.
	/// </summary>
	/// <param name="label">Label</param>
	/// <param name="buffer">Buffer for value</param>
	/// <param name="description">A description for the field</param>
	/// <returns>bool</returns>
	static bool InputText(std::string label, char* buffer, std::string description = "") {

        bool response;
        string inputLabel = "##" + label;
        ImVec2 size = ImGui::GetContentRegionAvail();

        AppStyle::pushLabel();
        ImGui::Text(label.c_str());
        AppStyle::pop();
        AppStyle::pushInput();
        ImGui::SetNextItemWidth(size.x - 20);
        if (ImGui::InputText(inputLabel.c_str(), buffer, 256)) {
            response = true;
        }
        else {
            response = false;
        }
        AppStyle::pop();
        AppStyle::pushLabel();
        ImGui::SetNextItemWidth(size.x - 20);
        ImGui::TextWrapped(description.c_str());
        AppStyle::pop();

        ImGui::Dummy(ImVec2(0, 10.0f));

        return response;

	}

    /// <summary>
    /// Creates a text input field.
    /// </summary>
    /// <param name="label">Label</param>
    /// <param name="buffer">Buffer for value</param>
    /// <param name="description">A description for the field</param>
    /// <returns>bool</returns>
    static bool InputPassword(std::string label, char* buffer, std::string description = "") {

		bool response;
		string inputLabel = "##" + label;
		ImVec2 size = ImGui::GetContentRegionAvail();

		AppStyle::pushLabel();
		ImGui::Text(label.c_str());
		AppStyle::pop();
		AppStyle::pushInput();
		ImGui::SetNextItemWidth(size.x - 20);
		if (ImGui::InputText(inputLabel.c_str(), buffer, 256, ImGuiInputTextFlags_Password)) {
			response = true;
		}
		else {
			response = false;
		}
		AppStyle::pop();
		AppStyle::pushLabel();
		ImGui::SetNextItemWidth(size.x - 20);
		ImGui::TextWrapped(description.c_str());
		AppStyle::pop();

		ImGui::Dummy(ImVec2(0, 10.0f));

		return response;
        
    }

    /// <summary>
    /// Creates a multiline text input field.
    /// </summary>
    /// <param name="label">Label</param>
    /// <param name="buffer">Buffer for value</param>
    /// <param name="description">A description for the field</param>
    /// <returns>bool</returns>
    static bool InputTextArea(std::string label, char* buffer, std::string description = "") {

        bool response;
        string inputLabel = "##" + label;
        ImVec2 size = ImGui::GetContentRegionAvail();

        AppStyle::pushLabel();
        ImGui::Text(label.c_str());
        AppStyle::pop();
        AppStyle::pushInput();
        ImGui::SetNextItemWidth(size.x - 20);
        if (ImGui::InputTextMultiline(inputLabel.c_str(), buffer, 500)) {
            response = true;
        }
        else {
            response = false;
        }
        AppStyle::pop();
        AppStyle::pushLabel();
        ImGui::SetNextItemWidth(size.x - 20);
        ImGui::TextWrapped(description.c_str());
        AppStyle::pop();

        ImGui::Dummy(ImVec2(0, 10.0f));

        return response;

    }

    /// <summary>
    /// Creates a number input field.
    /// </summary>
    /// <param name="label">Label for the input.</param>
    /// <param name="value">The value of the field.</param>
    /// <param name="from">Minimum number allowed.</param>
    /// <param name="to">Maximum number allowed.</param>
    /// <param name="description">Description for the field.</param>
    /// <returns>boolean</returns>
    static bool InputNumber(std::string label, int &value, int from, int to, std::string description = "") {

        bool response;
        ImVec2 size = ImGui::GetContentRegionAvail();

        AppStyle::pushLabel();
        ImGui::Text(label.c_str());
        AppStyle::pop();
        AppStyle::pushInput();
        ImGui::SetNextItemWidth(size.x - 20);
        if (IntRangeWidget::render(label.c_str(), value, from, to, 0.025f)) {
            response = true;
        }
        else {
            response = false;
        }
        AppStyle::pop();
        AppStyle::pushLabel();
        ImGui::SetNextItemWidth(size.x - 20);
        ImGui::TextWrapped(description.c_str());
        AppStyle::pop();

        ImGui::Dummy(ImVec2(0, 10.0f));

        return response;

    }

    /// <summary>
    /// Creates a toggle input field.
    /// </summary>
    /// <param name="label">Label for the input.</param>
    /// <param name="value">The value of the field.</param>
    /// <param name="description">Description for the field.</param>
    /// <returns>boolean</returns>
    static bool InputToggle(std::string label, bool &isOn, std::string description = "") {

        bool response = false;
        ImVec2 size = ImGui::GetContentRegionAvail();

        AppStyle::pushLabel();
        ImGui::Text(label.c_str());
        AppStyle::pop();
        AppStyle::pushInput();
        ImGui::SetNextItemWidth(size.x - 20);
        if (ToggleIconButtonWidget::render(AppIcons::yes, AppIcons::no, isOn, AppColors::positive, AppColors::negative, ImVec2(22, 22))) {
            isOn = !isOn;
            response = true;
        }
        AppStyle::pop();
        AppStyle::pushLabel();
        ImGui::SetNextItemWidth(size.x - 20);
        ImGui::TextWrapped(description.c_str());
        AppStyle::pop();

        ImGui::Dummy(ImVec2(0, 10.0f));

        return response;

    }

    /// <summary>
    /// Creates a toggle checkbox field.
    /// </summary>
    /// <param name="label">Label for the input.</param>
    /// <param name="value">The value of the field.</param>
    /// <param name="description">Description for the field.</param>
    /// <returns>boolean</returns>
    static bool InputCheckbox(std::string label, bool& isOn, std::string description = "") {

        bool response = false;
        ImVec2 size = ImGui::GetContentRegionAvail();

        AppStyle::pushInput();
        ImGui::SetNextItemWidth(size.x - 20);
        if (ImGui::Checkbox(label.c_str(), &isOn)) {
            response = true;
        }
        AppStyle::pop();
        AppStyle::pushLabel();
        ImGui::SetNextItemWidth(size.x - 40);
        ImGui::TextWrapped(description.c_str());
        AppStyle::pop();

		if (description != "") {
			ImGui::Dummy(ImVec2(0, 10.0f));
		}

        return response;

    }

    /// <summary>
    /// Creates a button.
    /// </summary>
    /// <param name="label">Label for the button.</param>
    /// <returns>boolean</returns>
    static bool Button(std::string label) {

        ImGui::BeginGroup();
        ImGui::Indent(10);
        AppColors::pushButtonSolid();
        if (ImGui::Button(label.c_str())) {
            return true;
        }
        ImGui::PopStyleColor(4);
        ImGui::Unindent(10);
        ImGui::EndGroup();

        return false;

	}

};



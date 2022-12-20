#include "cdpch.hpp"
#include "EditorUI/DragData.hpp"
#include "Cardia/Core/Log.hpp"
#include <imgui_internal.h>


namespace Cardia::EditorUI
{
	constexpr float COLUMN_SIZE = 80.0f;

	bool EditorUI::DragInt(const char *label, int *data, float speed, int v_min, int v_max)
	{
		ImGui::PushID(label);
		ImGui::Columns(2);
		ImGui::SetColumnWidth(0, COLUMN_SIZE);

		ImGui::Text("%s", label);

		ImGui::NextColumn();

		ImGui::SetNextItemWidth(-0.01f);
		bool res = ImGui::DragInt("", data, speed, v_min, v_max);

		ImGui::Columns(1);

		ImGui::PopID();

		return res;
	}

	bool EditorUI::DragFloat(const char *label, float *data, float speed, float v_min, float v_max)
	{
		ImGui::PushID(label);
		ImGui::Columns(2);
		ImGui::SetColumnWidth(0, COLUMN_SIZE);

		ImGui::Text("%s", label);

		ImGui::NextColumn();

		ImGui::SetNextItemWidth(-0.01f);
		bool res = ImGui::DragFloat("", data, speed, v_min, v_max);

		ImGui::Columns(1);

		ImGui::PopID();

		return res;
	}

	bool InputText(const char *label, char *buffer, std::size_t size, ImGuiInputTextFlags flags)
	{
		ImGui::PushID(label);
		ImGui::Columns(2);
		ImGui::SetColumnWidth(0, COLUMN_SIZE);

		ImGui::Text("%s", label);

		ImGui::NextColumn();

		ImGui::SetNextItemWidth(-0.01f);
		bool res = ImGui::InputText("", buffer, size, flags);

		ImGui::Columns(1);

		ImGui::PopID();

		return res;
	}

	bool ColorEdit3(const char* label, float* col, ImGuiColorEditFlags flags)
	{
		ImGui::PushID(label);
		ImGui::Columns(2);
		ImGui::SetColumnWidth(0, COLUMN_SIZE);

		ImGui::Text("%s", label);

		ImGui::NextColumn();

		ImGui::SetNextItemWidth(-0.01f);
		bool res = ImGui::ColorEdit3("", col, flags);

		ImGui::Columns(1);

		ImGui::PopID();

		return res;
	}

	bool ColorEdit4(const char *label, float *col, ImGuiColorEditFlags flags)
	{
		ImGui::PushID(label);
		ImGui::Columns(2);
		ImGui::SetColumnWidth(0, COLUMN_SIZE);

		ImGui::Text("%s", label);

		ImGui::NextColumn();

		ImGui::SetNextItemWidth(-0.01f);
		bool res = ImGui::ColorEdit4("", col, flags);

		ImGui::Columns(1);

		ImGui::PopID();

		return res;
	}

	void DragFloat3(const std::string &label, glm::vec3 &vector, float reset)
	{
		ImGuiIO& io = ImGui::GetIO();
		auto boldFont = io.Fonts->Fonts[0];

		ImGui::PushID(label.c_str());

		ImGui::Columns(2);
		ImGui::SetColumnWidth(0, COLUMN_SIZE);
		ImGui::Text("%s", label.c_str());
		ImGui::NextColumn();

		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 0, 0 });

		float lineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
		ImVec2 buttonSize = { lineHeight + 3.0f, lineHeight };
		const auto item_width = (ImGui::GetContentRegionAvail().x / 3.0f) - buttonSize.x;

		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.8f, 0.1f, 0.15f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.9f, 0.2f, 0.2f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.8f, 0.1f, 0.15f, 1.0f });
		ImGui::PushFont(boldFont);
		ImGui::SetNextItemWidth(buttonSize.x);
		if (ImGui::Button("X", buttonSize))
		{
			vector.x = reset;
		}
		ImGui::PopFont();
		ImGui::PopStyleColor(3);

		ImGui::SameLine();
		ImGui::SetNextItemWidth(item_width);
		ImGui::DragFloat("##X", &vector.x, 0.1f, 0.0f, 0.0f, "%.2f");
		ImGui::SameLine();

		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.2f, 0.7f, 0.2f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.3f, 0.8f, 0.3f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.2f, 0.7f, 0.2f, 1.0f });
		ImGui::PushFont(boldFont);
		ImGui::SetNextItemWidth(buttonSize.x);
		if (ImGui::Button("Y", buttonSize))
			vector.y = reset;
		ImGui::PopFont();
		ImGui::PopStyleColor(3);

		ImGui::SameLine();
		ImGui::SetNextItemWidth(item_width);
		ImGui::DragFloat("##Y", &vector.y, 0.1f, 0.0f, 0.0f, "%.2f");
		ImGui::SameLine();

		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.1f, 0.25f, 0.8f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.2f, 0.35f, 0.9f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.1f, 0.25f, 0.8f, 1.0f });
		ImGui::PushFont(boldFont);
		ImGui::SetNextItemWidth(buttonSize.x);
		if (ImGui::Button("Z", buttonSize))
			vector.z = reset;
		ImGui::PopFont();
		ImGui::PopStyleColor(3);

		ImGui::SameLine();
		ImGui::SetNextItemWidth(item_width);
		ImGui::DragFloat("##Z", &vector.z, 0.1f, 0.0f, 0.0f, "%.2f");

		ImGui::PopStyleVar();

		ImGui::Columns(1);

		ImGui::PopID();
	}

	bool Combo(const char* label, int* current_item, const char* const items[], int items_count,
		   int popup_max_height_in_items)
	{
		ImGui::PushID(label);
		ImGui::Columns(2);
		ImGui::SetColumnWidth(0, COLUMN_SIZE);

		ImGui::Text("%s", label);

		ImGui::NextColumn();

		ImGui::SetNextItemWidth(-0.01f);
		bool res = ImGui::Combo("", current_item, items, items_count, popup_max_height_in_items);
		ImGui::Columns(1);

		ImGui::PopID();

		return res;
	}

	bool Checkbox(const char *label, bool *v)
	{
		ImGui::PushID(label);
		ImGui::Columns(2);
		ImGui::SetColumnWidth(0, COLUMN_SIZE);

		ImGui::Text("%s", label);

		ImGui::NextColumn();

		ImGui::SetNextItemWidth(-0.01f);
		bool res = ImGui::Checkbox(label, v);
		ImGui::Columns(1);

		ImGui::PopID();

		return res;
	}
}
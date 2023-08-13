#include "cdpch.hpp"
#include "EditorUI/DragData.hpp"
#include "Cardia/Core/Log.hpp"
#include <imgui_internal.h>

#include "Cardia/Math/Vector2.hpp"
#include "Cardia/Math/Vector3.hpp"
#include "Cardia/Math/Vector4.hpp"


namespace Cardia::EditorUI
{
	constexpr float COLUMN_SIZE = 80.0f;

	bool DragInt(const char *label, int *data, float speed, int v_min, int v_max)
	{
		ImGui::PushID(label);
		ImGui::Columns(2);
		ImGui::SetColumnWidth(0, COLUMN_SIZE);

		ImGui::Text("%s", label);

		ImGui::NextColumn();

		ImGui::SetNextItemWidth(-0.01f);
		bool res = ImGui::DragInt("##int", data, speed, v_min, v_max);

		ImGui::Columns(1);

		ImGui::PopID();

		return res;
	}

	bool DragFloat(const char *label, float *data, float speed, float v_min, float v_max)
	{
		ImGui::PushID(label);
		ImGui::Columns(2);
		ImGui::SetColumnWidth(0, COLUMN_SIZE);

		ImGui::Text("%s", label);

		ImGui::NextColumn();

		ImGui::SetNextItemWidth(-0.01f);
		bool res = ImGui::DragFloat("##flt", data, speed, v_min, v_max);

		ImGui::Columns(1);

		ImGui::PopID();

		return res;
	}

	struct InputTextCallback_UserData
	{
		std::string*            Str;
		ImGuiInputTextCallback  ChainCallback;
		void*                   ChainCallbackUserData;
	};

	static int InputTextCallback(ImGuiInputTextCallbackData* data)
	{
		const InputTextCallback_UserData* user_data = static_cast<InputTextCallback_UserData*>(data->UserData);
		if (data->EventFlag == ImGuiInputTextFlags_CallbackResize)
		{
			// Resize string callback
			// If for some reason we refuse the new length (BufTextLen) and/or capacity (BufSize) we need to set them back to what we want.
			std::string* str = user_data->Str;
			CdCoreAssert(data->Buf == str->c_str());
			str->resize(data->BufTextLen);
			data->Buf = const_cast<char*>(str->c_str());
		}
		else if (user_data->ChainCallback)
		{
			// Forward to user callback, if any
			data->UserData = user_data->ChainCallbackUserData;
			return user_data->ChainCallback(data);
		}
		return 0;
	}

	bool InputText(const char* label, std::string* str, const Vector4f& color, ImGuiInputTextFlags flags, ImGuiInputTextCallback callback, void* user_data)
	{
		ImGui::PushID(label);
		ImGui::Columns(2);
		ImGui::SetColumnWidth(0, COLUMN_SIZE);

		ImGui::Text("%s", label);

		ImGui::NextColumn();

		ImGui::SetNextItemWidth(-0.01f);
		ImGui::PushStyleColor(ImGuiCol_Text, {color.x, color.y, color.z, color.w});
		
		IM_ASSERT((flags & ImGuiInputTextFlags_CallbackResize) == 0);
		flags |= ImGuiInputTextFlags_CallbackResize;

		InputTextCallback_UserData cb_user_data;
		cb_user_data.Str = str;
		cb_user_data.ChainCallback = callback;
		cb_user_data.ChainCallbackUserData = user_data;
		bool res = ImGui::InputText("##txt", const_cast<char*>(str->c_str()), str->capacity() + 1, flags, InputTextCallback, &cb_user_data);
		ImGui::PopStyleColor();

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
		bool res = ImGui::ColorEdit3("##ce3", col, flags);

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
		bool res = ImGui::ColorEdit4("##ce4", col, flags);

		ImGui::Columns(1);

		ImGui::PopID();

		return res;
	}

	template <typename T>
	static bool InternalDragFloat4(const std::string &label, T& vector, float reset)
	{

		bool res = false;
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
		const auto item_width = (ImGui::GetContentRegionAvail().x / static_cast<float>(vector.Size())) - buttonSize.x;

		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.8f, 0.1f, 0.15f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.9f, 0.2f, 0.2f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.8f, 0.1f, 0.15f, 1.0f });
		ImGui::PushFont(boldFont);
		ImGui::SetNextItemWidth(buttonSize.x);
		if (ImGui::Button("X", buttonSize))
		{
			vector.x = reset;
			res = true;
		}
		ImGui::PopFont();
		ImGui::PopStyleColor(3);

		ImGui::SameLine();
		ImGui::SetNextItemWidth(item_width);
		if (ImGui::DragFloat("##X", &vector.x, 0.1f, 0.0f, 0.0f, "%.2f")) {
			res = true;
		}

		if constexpr (std::is_assignable_v<Vector2f, T>) {
			ImGui::SameLine();

			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.2f, 0.7f, 0.2f, 1.0f });
			ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.3f, 0.8f, 0.3f, 1.0f });
			ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.2f, 0.7f, 0.2f, 1.0f });
			ImGui::PushFont(boldFont);
			ImGui::SetNextItemWidth(buttonSize.x);
			if (ImGui::Button("Y", buttonSize))
			{
				vector.y = reset;
				res = true;
			}
			ImGui::PopFont();
			ImGui::PopStyleColor(3);

			ImGui::SameLine();
			ImGui::SetNextItemWidth(item_width);
			if (ImGui::DragFloat("##Y", &vector.y, 0.1f, 0.0f, 0.0f, "%.2f")) {
				res = true;
			}
		}

		if constexpr (std::is_assignable_v<Vector3f, T>) {
			ImGui::SameLine();

			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.1f, 0.25f, 0.8f, 1.0f });
			ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.2f, 0.35f, 0.9f, 1.0f });
			ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.1f, 0.25f, 0.8f, 1.0f });
			ImGui::PushFont(boldFont);
			ImGui::SetNextItemWidth(buttonSize.x);
			if (ImGui::Button("Z", buttonSize))
			{
				vector.z = reset;
				res = true;
			}
			ImGui::PopFont();
			ImGui::PopStyleColor(3);

			ImGui::SameLine();
			ImGui::SetNextItemWidth(item_width);
			if (ImGui::DragFloat("##Z", &vector.z, 0.1f, 0.0f, 0.0f, "%.2f")) {
				res = true;
			}
		}

		if constexpr (std::is_assignable_v<Vector4f, T>) {

			ImGui::SameLine();

			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.6f, 0.6f, 0.6f, 1.0f });
			ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.7f, 0.7f, 0.7f, 1.0f });
			ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.6f, 0.6f, 0.6f, 1.0f });
			ImGui::PushFont(boldFont);
			ImGui::SetNextItemWidth(buttonSize.x);
			if (ImGui::Button("W", buttonSize))
			{
				vector.w = reset;
				res = true;
			}
			ImGui::PopFont();
			ImGui::PopStyleColor(3);

			ImGui::SameLine();
			ImGui::SetNextItemWidth(item_width);
			if (ImGui::DragFloat("##W", &vector.w, 0.1f, 0.0f, 0.0f, "%.2f")) {
				res = true;
			}
		}

		ImGui::PopStyleVar();
		ImGui::Columns(1);
		ImGui::PopID();
		return res;
	}

	bool DragFloat2(const std::string &label, Vector2f &vector, float reset)
	{
		return InternalDragFloat4(label, vector, reset);
	}

	bool DragFloat3(const std::string &label, Vector3f &vector, float reset)
	{
		return InternalDragFloat4(label, vector, reset);
	}

	bool DragFloat4(const std::string &label, Vector4f &vector, float reset)
	{
		return InternalDragFloat4(label, vector, reset);
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
		bool res = ImGui::Combo("##cmb", current_item, items, items_count, popup_max_height_in_items);
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
		bool res = ImGui::Checkbox("##lbl", v);
		ImGui::Columns(1);

		ImGui::PopID();

		return res;
	}
}

#include "horizon.hpp"
#include "imgui_extension.hpp"

ImVec2 operator/(ImVec2 v, int i) { return ImVec2(v.x / i, v.y / i); }
ImVec2 operator+(ImVec2 v1, ImVec2 v2) { return ImVec2(v1.x + v2.x, v1.y + v2.y); }
ImVec2 operator-(ImVec2 v1, ImVec2 v2) { return ImVec2(v1.x - v2.x, v1.y - v2.y); }
bool operator==(ImVec2 v1, ImVec2 v2) { return v1.x == v2.x && v1.y == v2.y; }
bool operator!=(ImVec2 v1, ImVec2 v2) { return v1 != v2; }

namespace ImGui
{
	namespace Center
	{
		int start_idx = 0;
		ImDrawList* draw_list = nullptr;

		void Start(ImDrawList* draw_list)
		{
			if (!draw_list)
				draw_list = ImGui::GetWindowDrawList();

			ImGui::Center::start_idx = draw_list->VtxBuffer.Size;
			ImGui::Center::draw_list = draw_list;
		}

		void CenterX()
		{
			auto item_width = ImGui::CalcItemWidth();
			auto window_width = ImGui::GetWindowWidth();

			auto& buf = draw_list->VtxBuffer;
			for (int i = start_idx; i < buf.Size; i++)
				buf[i].pos.x += (window_width / 2.f) - (item_width / 2.f);

			start_idx = draw_list->VtxBuffer.Size;
		}

		void End()
		{
			start_idx = 0;
			draw_list = nullptr;
		}
	}

	namespace Rotation
	{
		int start_idx = 0;
		ImDrawList* draw_list = nullptr;

		void Start(ImDrawList* draw_list)
		{
			if (!draw_list)
				draw_list = ImGui::GetWindowDrawList();

			ImGui::Rotation::start_idx = draw_list->VtxBuffer.Size;
			ImGui::Rotation::draw_list = draw_list;
		}

		void Rotate(float rad, ImVec2 center)
		{
			auto s = sin(rad);
			auto c = cos(rad);
			center = ImRotate(center, c, s) - center;

			auto& buf = draw_list->VtxBuffer;
			for (int i = start_idx; i < buf.Size; i++)
				buf[i].pos = ImRotate(buf[i].pos, c, s) - center;
		}

		void End()
		{
			start_idx = 0;
			draw_list = nullptr;
		}
	}

	namespace Page
	{
		size_t count = 0;
		size_t page = 0;
		float width = 0;

		void Begin(size_t size)
		{
			page = 0;
			count = size;
			width = ImGui::GetWindowSize().x;
			width /= count;
			width -= (ImGui::GetStyle().FramePadding.x * (count - 1));
			ImGui::BeginChild("page_start", ImVec2(width, 0));
		}

		void Next()
		{
			//assert(count > 0);

			char buffer[128] = "";
			sprintf(buffer, "page_%d", page);
			page++;

			ImGui::EndChild();
			ImGui::SameLine();
			ImGui::BeginChild(buffer, ImVec2(width, 0));
		}

		void End()
		{
			ImGui::EndChild();
			count = 0;
			width = 0;
			page = 0;
		}
	}
}
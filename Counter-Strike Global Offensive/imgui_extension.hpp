#pragma once

extern ImVec2 operator / (ImVec2 v, int i);
extern ImVec2 operator + (ImVec2 v1, ImVec2 v2);
extern ImVec2 operator - (ImVec2 v1, ImVec2 v2);
extern bool operator == (ImVec2 v1, ImVec2 v2);
extern bool operator != (ImVec2 v1, ImVec2 v2);

namespace ImGui
{
	namespace Center
	{
		extern int start_idx;
		extern ImDrawList* draw_list;

		extern void Start(ImDrawList* draw_list = nullptr);

		extern void CenterX();

		extern void End();
	}

	namespace Rotation
	{
		extern int start_idx;
		extern ImDrawList* draw_list;

		extern void Start(ImDrawList* draw_list = nullptr);

		extern void Rotate(float rad, ImVec2 center);

		extern void End();
	}

	namespace Page
	{
		extern size_t count;
		extern size_t page;
		extern float width;

		extern void Begin(size_t size = 2);

		extern void Next();

		extern void End();
	}
}
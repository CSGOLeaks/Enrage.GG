#include "hooked.hpp"
#define STB_IMAGE_IMPLEMENTATION
#include "include/stb_image.h"


vgui::HFont F::Menu;
vgui::HFont F::MenuV2;
vgui::HFont F::MenuV2i;
vgui::HFont F::MenuV2n;
vgui::HFont F::ESPInfo;
vgui::HFont F::ESP;
vgui::HFont F::LBY;
vgui::HFont F::Events;
vgui::HFont F::Icons;
vgui::HFont F::MenuIcons;

const char* Drawing::LastFontName;

void Drawing::CreateFonts()
{
	VIRTUALIZER_FISH_LITE_START;
	VIRTUALIZER_STR_ENCRYPT_START;
	LastFontName = "";

	LastFontName = "Verdana"; //yes
	csgo.m_surface()->SetFontGlyphSet(F::MenuV2 = csgo.m_surface()->CreateFont_(), sxor("Verdana"), 12, 500, NULL, NULL, FONTFLAG_DROPSHADOW | FONTFLAG_ANTIALIAS); // supremacy
	csgo.m_surface()->SetFontGlyphSet(F::MenuV2i = csgo.m_surface()->CreateFont_(), sxor("Verdana"), 12, 700, NULL, NULL, FONTFLAG_DROPSHADOW | FONTFLAG_ANTIALIAS); // supremacy
	LastFontName = "Segoe UI"; //yes
	csgo.m_surface()->SetFontGlyphSet(F::Menu = csgo.m_surface()->CreateFont_(), sxor("Segoe UI"), 13, 400, NULL, NULL, FONTFLAG_DROPSHADOW | FONTFLAG_ANTIALIAS);//sxor("Visitor_Rus"), 12, 400, NULL, NULL, FONTFLAG_DROPSHADOW);
	LastFontName = "Tahoma"; //yes
	csgo.m_surface()->SetFontGlyphSet(F::ESP = csgo.m_surface()->CreateFont_(), sxor("Tahoma"), 12, 400, NULL, NULL, FONTFLAG_DROPSHADOW | FONTFLAG_ANTIALIAS); // supremacy esp
	csgo.m_surface()->SetFontGlyphSet(F::MenuV2n = csgo.m_surface()->CreateFont_(), sxor("Tahoma"), 12, 500, NULL, NULL, 0); // supremacy
	LastFontName = "Verdana"; //yes
	csgo.m_surface()->SetFontGlyphSet(F::LBY = csgo.m_surface()->CreateFont_(), sxor("Verdana"), 26, 700, NULL, NULL, 144);
	LastFontName = "Smallest Pixel-7"; //yes 
	csgo.m_surface()->SetFontGlyphSet(F::ESPInfo = csgo.m_surface()->CreateFont_(), sxor("Smallest Pixel-7"), 10, 100, NULL, NULL, FONTFLAG_OUTLINE);
	LastFontName = "Lucida Console"; //yes
	csgo.m_surface()->SetFontGlyphSet(F::Events = csgo.m_surface()->CreateFont_(), sxor("Lucida Console"), 10, NULL, NULL, NULL, FONTFLAG_DROPSHADOW);
	LastFontName = "undefeated"; //yes
	csgo.m_surface()->SetFontGlyphSet(F::Icons = csgo.m_surface()->CreateFont_(), sxor("undefeated"), 12, 500, NULL, NULL, FONTFLAG_ANTIALIAS | FONTFLAG_DROPSHADOW);
	LastFontName = "SkeetFont"; //yes
	csgo.m_surface()->SetFontGlyphSet(F::MenuIcons = csgo.m_surface()->CreateFont_(), sxor("SkeetFont"), 36, 500, NULL, NULL, FONTFLAG_ANTIALIAS | FONTFLAG_DROPSHADOW);
	LastFontName = "";

	VIRTUALIZER_STR_ENCRYPT_END;
	VIRTUALIZER_FISH_LITE_END;
}

void Drawing::Texture(const int32_t x, const int32_t y, const int32_t w, const int32_t h, const int texture, const Color tint)
{
	csgo.m_surface()->DrawSetColor(tint.r(), tint.g(), tint.b(), tint.a());
	
	csgo.m_surface()->DrawTexturedRect(x, y, x + w, y + h);
}

void Drawing::LimitDrawingArea(int x, int y, int w, int h) {
	typedef void(__thiscall * OriginalFn)(void*, int, int, int, int);
	Memory::VCall<OriginalFn>(csgo.m_surface(), 147)(csgo.m_surface(), x, y, x + w + 1, y + h + 1);
}

void Drawing::GetDrawingArea(int& x, int& y, int& w, int& h) {
	typedef void(__thiscall * OriginalFn)(void*, int&, int&, int&, int&);
	Memory::VCall<OriginalFn>(csgo.m_surface(), 146)(csgo.m_surface(), x, y, w, h);
}

void Drawing::ColorPicker(Vector2D pos, int width, int height, float alpha) {
	static auto gradient_texture = csgo.m_surface()->CreateNewTextureID(true);
	static std::unique_ptr<Color[]> gradient = nullptr;

	if (!gradient) {
		gradient = std::make_unique<Color[]>(width * height);

		// hue, saturation, lightness
		auto h = 0.f, l = 1.f;
		const auto s = 1;

		// loop through each pixel in the picker
		for (auto x = 0; x < width; x++) {
			l -= 1.f / float(width); // slowly change lightness from 1 - 0
			for (auto y = 0; y < height; y++) {
				h += 1.f / float(height); // slowly change hue to 0 - 1

				Color current = Color(0, 0, 0); // init
				*reinterpret_cast<Color*>(gradient.get() + x + y * width) = current.FromHSL(h, s, l);
			}

			h = 0.f; // reset hue after each loop on the "y" axis
		}

		csgo.m_surface()->DrawSetTextureRGBA(gradient_texture, reinterpret_cast<unsigned char*>(gradient.get()), width, height);
	}
	csgo.m_surface()->DrawSetColor(Color(255, 255, 255, alpha));
	csgo.m_surface()->DrawSetTexture(gradient_texture);
	csgo.m_surface()->DrawTexturedRect(pos.x, pos.y, pos.x + width, pos.y + height);
}

void Drawing::DrawString(vgui::HFont font, int x, int y, Color color, DWORD alignment, const char* msg, ...)
{
	va_list va_alist;
	char buf[1024];
	va_start(va_alist, msg);
	_vsnprintf_s(buf, sizeof(buf), msg, va_alist);
	va_end(va_alist);
	wchar_t wbuf[1024];
	MultiByteToWideChar(CP_UTF8, 0, buf, 256, wbuf, 256);

	int r = 255, g = 255, b = 255, a = 255;
	color.GetColor(r, g, b, a);

	int width, height;
	csgo.m_surface()->GetTextSize(font, wbuf, width, height);

	if (alignment & FONT_RIGHT)
		x -= width;
	if (alignment & FONT_CENTER)
		x -= width / 2;

	csgo.m_surface()->DrawSetTextFont(font);

	if (width > 65536)
		return;

	if (alignment & FONT_OUTLINE)
	{
		csgo.m_surface()->DrawSetTextColor(Color(0, 0, 0, color.a()));
		csgo.m_surface()->DrawSetTextPos(x + 1, y /*+ 1*/);
		csgo.m_surface()->DrawPrintText(wbuf, wcslen(wbuf));

		csgo.m_surface()->DrawSetTextColor(Color(0, 0, 0, color.a()));
		csgo.m_surface()->DrawSetTextPos(x - 1, y /*- 1*/);
		csgo.m_surface()->DrawPrintText(wbuf, wcslen(wbuf));
	}

	csgo.m_surface()->DrawSetTextColor(r, g, b, a);
	csgo.m_surface()->DrawSetTextPos(x, y /*- height / 2*/);
	csgo.m_surface()->DrawPrintText(wbuf, wcslen(wbuf));
}

void Drawing::DrawStringFont(vgui::HFont font, int x, int y, Color clrColor, bool bCenter, const char* szText, ...)
{
	if (!szText)
		return;

	va_list va_alist;
	char buf[1024];
	va_start(va_alist, szText);
	_vsnprintf_s(buf, sizeof(buf), szText, va_alist);
	va_end(va_alist);
	wchar_t wbuf[1024];
	MultiByteToWideChar(CP_UTF8, 0, buf, 256, wbuf, 256);

	if (bCenter)
	{
		int Wide = 0, Tall = 0;

		csgo.m_surface()->GetTextSize(font, wbuf, Wide, Tall);

		x -= Wide / 2;
	}

	csgo.m_surface()->DrawSetTextFont(font);
	csgo.m_surface()->DrawSetTextPos(x, y);
	csgo.m_surface()->DrawSetTextColor(clrColor);
	csgo.m_surface()->DrawPrintText(wbuf, wcslen(wbuf));
}

void Drawing::DrawStringUnicode(vgui::HFont font, int x, int y, Color color, bool bCenter, const wchar_t* msg, ...)
{
	int r = 255, g = 255, b = 255, a = 255;
	color.GetColor(r, g, b, a);

	int iWidth, iHeight;

	csgo.m_surface()->GetTextSize(font, msg, iWidth, iHeight);
	csgo.m_surface()->DrawSetTextFont(font);
	csgo.m_surface()->DrawSetTextColor(r, g, b, a);
	csgo.m_surface()->DrawSetTextPos(x, y);
	csgo.m_surface()->DrawPrintText(msg, wcslen(msg));
}

void Drawing::DrawRect(int x, int y, int w, int h, Color col)
{
	csgo.m_surface()->DrawSetColor(col);
	csgo.m_surface()->DrawFilledRect(x, y, x + w, y + h);
}

void Drawing::DrawRectA(int x, int y, int w, int h, Color col)
{
	csgo.m_surface()->DrawSetColor(col);
	csgo.m_surface()->DrawFilledRect(x, y, w, h);
}

void Drawing::Rectangle(float x, float y, float w, float h, float px, Color col)
{
	DrawRect(x, (y + h - px), w, px, col);
	DrawRect(x, y, px, h, col);
	DrawRect(x, y, w, px, col);
	DrawRect((x + w - px), y, px, h, col);
}

void Drawing::Border(int x, int y, int w, int h, int line, Color col)
{
	DrawRect(x, y, w, line, col);
	DrawRect(x, y, line, h, col);
	DrawRect((x + w), y, line, h, col);
	DrawRect(x, (y + h), (w + line), line, col);
}

void Drawing::DrawRectRainbow(int x, int y, int width, int height, float flSpeed, float& flRainbow)
{
	Color colColor(0, 0, 0);

	flRainbow += flSpeed;
	if (flRainbow > 1.f) flRainbow = 0.f;

	for (int i = 0; i < width; i++)
	{
		float hue = (1.f / (float)width) * i;
		hue -= flRainbow;
		if (hue < 0.f) hue += 1.f;

		Color colRainbow = colColor.FromHSB(hue, 1.f, 1.f);
		Drawing::DrawRect(x + i, y, 1, height, colRainbow);
	}
}

void Drawing::DrawRectGradientVertical(int x, int y, int width, int height, Color color1, Color color2)
{
	float flDifferenceR = (float)(color2.r() - color1.r()) / (float)height;
	float flDifferenceG = (float)(color2.g() - color1.g()) / (float)height;
	float flDifferenceB = (float)(color2.b() - color1.b()) / (float)height;
	float flDifferenceA = (float)(color2.a() - color1.a()) / (float)height;

	for (float i = 0.f; i < height; i++)
	{
		Color colGradient = Color(color1.r() + (flDifferenceR * i), color1.g() + (flDifferenceG * i), color1.b() + (flDifferenceB * i), color1.a() + (flDifferenceA * i));
		Drawing::DrawRect(x, y + i, width, 1, colGradient);
	}
}

void Drawing::DrawRectGradientVerticalA(int x, int y, int width, int height, Color color1, Color color2)
{
	auto draw_filled_rect = [&](bool reversed) {
		csgo.m_surface()->DrawFilledRectFade(x, y, width, height,
			reversed ? color1.a() : 0, reversed ? 0 : color2.a(), false);
	};

	csgo.m_surface()->DrawSetColor(color1);
	draw_filled_rect(true);

	csgo.m_surface()->DrawSetColor(color2);
	draw_filled_rect(false);
}

void Drawing::DrawRectGradientHorizontal(int x, int y, int width, int height, Color color1, Color color2)
{
	float flDifferenceR = (float)(color2.r() - color1.r()) / (float)width;
	float flDifferenceG = (float)(color2.g() - color1.g()) / (float)width;
	float flDifferenceB = (float)(color2.b() - color1.b()) / (float)width;
	float flDifferenceA = (float)(color2.a() - color1.a()) / (float)width;

	for (float i = 0.f; i < width; i++)
	{
		Color colGradient = Color(color1.r() + (flDifferenceR * i), color1.g() + (flDifferenceG * i), color1.b() + (flDifferenceB * i), color1.a() + (flDifferenceA * i));
		Drawing::DrawRect(x + i, y, 1, height, colGradient);
	}
}

void Drawing::DrawPixel(int x, int y, Color col)
{
	csgo.m_surface()->DrawSetColor(col);
	csgo.m_surface()->DrawFilledRect(x, y, x + 1, y + 1);
}

void Drawing::DrawOutlinedRect(int x, int y, int w, int h, Color col)
{
	csgo.m_surface()->DrawSetColor(col);
	csgo.m_surface()->DrawOutlinedRect(x, y, x + w, y + h);
}

void Drawing::DrawOutlinedRectA(int x, int y, int w, int h, Color col)
{
	csgo.m_surface()->DrawSetColor(col);
	csgo.m_surface()->DrawOutlinedRect(x, y, w, h);
}

void Drawing::DrawOutlinedCircle(int x, int y, int r, Color col)
{
	csgo.m_surface()->DrawSetColor(col);
	csgo.m_surface()->DrawOutlinedCircle(x, y, r, 1);
}

void Drawing::DrawLine(int x0, int y0, int x1, int y1, Color col)
{
	csgo.m_surface()->DrawSetColor(col);
	csgo.m_surface()->DrawLine(x0, y0, x1, y1);
}

void Drawing::DrawCircularProgressBar(Vector2D pos, float radius, int thickness, int num_segments, float progress, Color color) {
	Vector2D size{ radius * 2, radius * 2 };

	int start = static_cast<int>(abs(sin(static_cast<float>(csgo.m_globals()->realtime * 1.8f)) * (num_segments - 5)));
	const float a_min = M_PI * 2.0f * ((float)start) / (float)num_segments;
	const float a_max = M_PI * 2.0f * ((float)num_segments - 3) / (float)num_segments;
	const auto&& centre = pos + radius;
	for (auto i = 0; i < num_segments; i++) {
		const float a = a_min + ((float)i / (float)num_segments) * (a_max - a_min);
		auto time = static_cast<float>(csgo.m_globals()->realtime);
		DrawLine(centre.x + cos(a + time * 8) * (radius - thickness), centre.y + sin(a + time * 8) * (radius - thickness), centre.x + cos(a + time * 8) * radius,
									  centre.y + sin(a + time * 8) * radius, color);
	}

	//window->DrawList->PathStroke(GetColorU32(color), false, thickness);
}

void Drawing::DrawCorner(int iX, int iY, int iWidth, int iHeight, bool bRight, bool bDown, Color colDraw)
{
	int iRealX = bRight ? iX - iWidth : iX;
	int iRealY = bDown ? iY - iHeight : iY;

	if (bDown && bRight)
		iWidth = iWidth + 1;

	Drawing::DrawRect(iRealX, iY, iWidth, 1, colDraw);
	Drawing::DrawRect(iX, iRealY, 1, iHeight, colDraw);

	Drawing::DrawRect(iRealX, bDown ? iY + 1 : iY - 1, !bDown && bRight ? iWidth + 1 : iWidth, 1, Color(0, 0, 0, 255));
	Drawing::DrawRect(bRight ? iX + 1 : iX - 1, bDown ? iRealY : iRealY - 1, 1, bDown ? iHeight + 2 : iHeight + 1, Color(0, 0, 0, 255));
}
//
//#define COL2DWORD(x) (D3DCOLOR_ARGB(x.alpha, x.red, x.green, x.blue))
//void Drawing::Triangle(Vector2D pos1, Vector2D pos2, Vector2D pos3, Color color) const
//{
//	const auto dwColor = COL2DWORD(color);
//	Vertex vert[4] =
//	{
//		{ pos1.x, pos1.y, 0.0f, 1.0f, dwColor },
//		{ pos2.x, pos2.y, 0.0f, 1.0f, dwColor },
//		{ pos3.x, pos3.y, 0.0f, 1.0f, dwColor },
//		{ pos1.x, pos1.y, 0.0f, 1.0f, dwColor }
//	};
//
//	this->pDevice->SetTexture(0, nullptr);
//	this->pDevice->DrawPrimitiveUP(D3DPT_LINESTRIP, 3, &vert, sizeof(Vertex));
//}
//
//
//void Drawing::TriangleFilled(Vector2D pos1, Vector2D pos2, Vector2D pos3, Color color) const
//{
//	const auto dwColor = COL2DWORD(color);
//	Vertex vert[3] =
//	{
//		{ pos1.x, pos1.y, 0.0f, 1.0f, dwColor },
//		{ pos2.x, pos2.y, 0.0f, 1.0f, dwColor },
//		{ pos3.x, pos3.y, 0.0f, 1.0f, dwColor }
//	};
//
//	this->pDevice->SetTexture(0, nullptr);
//	this->pDevice->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 1, &vert, sizeof(Vertex));
//}

void Drawing::Triangle(Vector ldcorner, Vector rucorner, Color col)
{
	DrawLine(ldcorner.x, ldcorner.y, (rucorner.x / 2) - 1, rucorner.y, col); // left shit

	DrawLine(rucorner.x, ldcorner.y, (rucorner.x / 2) - 1, rucorner.y, col); // right shit

	DrawLine(ldcorner.x, ldcorner.y, rucorner.x, ldcorner.y/*(ldcorner.x - rucorner.x), (ldcorner.y - rucorner.y)*/, col); // down shit
}

void Drawing::DrawPolygon(int count, Vertex_t* Vertexs, Color color)
{
	static int Texture = csgo.m_surface()->CreateNewTextureID(true);
	unsigned char buffer[4] = { 255, 255, 255, 255 };

	csgo.m_surface()->DrawSetTextureRGBA(Texture, buffer, 1, 1);
	csgo.m_surface()->DrawSetColor(color);
	csgo.m_surface()->DrawSetTexture(Texture);

	csgo.m_surface()->DrawTexturedPolygon(count, Vertexs);
}

void Drawing::DrawBox(int x, int y, int w, int h, Color color)
{
	// top
	DrawRect(x, y, w, 1, color);
	// right
	DrawRect(x, y + 1, 1, h - 1, color);
	// left
	DrawRect(x + w - 1, y + 1, 1, h - 1, color);
	// bottom
	DrawRect(x, y + h - 1, w - 1, 1, color);
}

void Drawing::DrawRoundedBox(int x, int y, int w, int h, int r, int v, Color col)
{
	std::vector<Vertex_t> p;
	for (int _i = 0; _i < 3; _i++)
	{
		int _x = x + (_i < 2 && r || w - r);
		int _y = y + (_i % 3 > 0 && r || h - r);
		for (int i = 0; i < v; i++)
		{
			int a = RAD2DEG((i / v) * -90 - _i * 90);
			p.emplace_back(Vector2D(_x + sin(a) * r, _y + cos(a) * r));
		}
	}

	Drawing::DrawPolygon(4 * (v + 1), &p[0], col);
	/*
	function DrawRoundedBox(x, y, w, h, r, v, col)
	local p = {};
	for _i = 0, 3 do
	local _x = x + (_i < 2 && r || w - r)
	local _y = y + (_i%3 > 0 && r || h - r)
	for i = 0, v do
	local a = math.rad((i / v) * - 90 - _i * 90)
	table.insert(p, {x = _x + math.sin(a) * r, y = _y + math.cos(a) * r})
	end
	end

	surface.SetDrawColor(col.r, col.g, col.b, 255)
	draw.NoTexture()
	surface.DrawPoly(p)
	end
	*/

	// Notes: amount of vertexes is 4(v + 1) where v is the number of vertices on each corner bit.
	// I did it in lua cause I have no idea how the vertex_t struct works and i'm still aids at C++
}

bool Drawing::ScreenTransform(const Vector& point, Vector& screen) // tots not pasted
{
	float w;
	const VMatrix& worldToScreen = csgo.m_engine()->WorldToScreenMatrix();

	screen.x = worldToScreen[0][0] * point[0] + worldToScreen[0][1] * point[1] + worldToScreen[0][2] * point[2] + worldToScreen[0][3];
	screen.y = worldToScreen[1][0] * point[0] + worldToScreen[1][1] * point[1] + worldToScreen[1][2] * point[2] + worldToScreen[1][3];
	w = worldToScreen[3][0] * point[0] + worldToScreen[3][1] * point[1] + worldToScreen[3][2] * point[2] + worldToScreen[3][3];
	screen.z = 0.0f;

	bool behind = false;

	if (w < 0.001f)
	{
		behind = true;
		screen.x *= 100000;
		screen.y *= 100000;
	}
	else
	{
		behind = false;
		float invw = 1.0f / w;
		screen.x *= invw;
		screen.y *= invw;
	}

	return behind;
}

bool Drawing::WorldToScreen(const Vector& origin, Vector& screen)
{
	if (!ScreenTransform(origin, screen))
	{
		float x = ctx.screen_size.x / 2;
		float y = ctx.screen_size.y / 2;
		x += 0.5 * screen.x * ctx.screen_size.x + 0.5;
		y -= 0.5 * screen.y * ctx.screen_size.y + 0.5;
		screen.x = x;
		screen.y = y;
		return true;
	}

	return false;
}

RECT Drawing::GetViewport()
{
	RECT Viewport = { 0, 0, 0, 0 };
	Viewport.right = ctx.screen_size.x; Viewport.bottom = ctx.screen_size.y;
	return Viewport;
}

int Drawing::GetStringWidth(vgui::HFont font, const char* msg, ...)
{
	va_list va_alist;
	char buf[1024];
	va_start(va_alist, msg);
	_vsnprintf_s(buf, sizeof(buf), msg, va_alist);
	va_end(va_alist);
	wchar_t wbuf[1024];
	MultiByteToWideChar(CP_UTF8, 0, buf, 256, wbuf, 256);

	int iWidth, iHeight;

	csgo.m_surface()->GetTextSize(font, wbuf, iWidth, iHeight);

	return iWidth;
}

RECT Drawing::GetTextSize(vgui::HFont font, const char* text)
{
	/*size_t origsize = strlen(text) + 1;
	const size_t newsize = 500;
	size_t convertedChars = 0;
	wchar_t wcstring[newsize];
	mbstowcs_s(&convertedChars, wcstring, origsize, text, _TRUNCATE);*/

	RECT rect;

	rect.left = rect.right = rect.bottom = rect.top = 0;

	wchar_t wbuf[1024];
	if (MultiByteToWideChar(CP_UTF8, 0, text, -1, wbuf, 256) > 0 && wcslen(wbuf) > 0) {
		int x, y;
		csgo.m_surface()->GetTextSize(font, wbuf, x, y);
		rect.left = x; rect.bottom = y;
		rect.right = x; rect.top = y;
	}

	return rect;
}

RECT Drawing::GetTextSize(vgui::HFont font, const wchar_t* text)
{
	RECT rect;

	rect.left = rect.right = rect.bottom = rect.top = 0;

	int x, y;
	csgo.m_surface()->GetTextSize(font, text, x, y);
	rect.left = x; rect.bottom = y;
	rect.right = x; rect.top = y;

	return rect;
}

void Drawing::Draw3DBox(Vector* boxVectors, Color color)
{
	Vector boxVectors0, boxVectors1, boxVectors2, boxVectors3,
		boxVectors4, boxVectors5, boxVectors6, boxVectors7;


	if (Drawing::WorldToScreen(boxVectors[0], boxVectors0) &&
		Drawing::WorldToScreen(boxVectors[1], boxVectors1) &&
		Drawing::WorldToScreen(boxVectors[2], boxVectors2) &&
		Drawing::WorldToScreen(boxVectors[3], boxVectors3) &&
		Drawing::WorldToScreen(boxVectors[4], boxVectors4) &&
		Drawing::WorldToScreen(boxVectors[5], boxVectors5) &&
		Drawing::WorldToScreen(boxVectors[6], boxVectors6) &&
		Drawing::WorldToScreen(boxVectors[7], boxVectors7))
	{

		/*
		.+--5---+
		.8 4    6'|
		+--7---+'  11
		9   |  10  |
		|  ,+--|3--+
		|.0    | 2'
		+--1---+'
		*/

		Vector2D lines[12][2];
		//bottom of box
		lines[0][0] = { boxVectors0.x, boxVectors0.y };
		lines[0][1] = { boxVectors1.x, boxVectors1.y };
		lines[1][0] = { boxVectors1.x, boxVectors1.y };
		lines[1][1] = { boxVectors2.x, boxVectors2.y };
		lines[2][0] = { boxVectors2.x, boxVectors2.y };
		lines[2][1] = { boxVectors3.x, boxVectors3.y };
		lines[3][0] = { boxVectors3.x, boxVectors3.y };
		lines[3][1] = { boxVectors0.x, boxVectors0.y };

		lines[4][0] = { boxVectors0.x, boxVectors0.y };
		lines[4][1] = { boxVectors6.x, boxVectors6.y };

		// top of box
		lines[5][0] = { boxVectors6.x, boxVectors6.y };
		lines[5][1] = { boxVectors5.x, boxVectors5.y };
		lines[6][0] = { boxVectors5.x, boxVectors5.y };
		lines[6][1] = { boxVectors4.x, boxVectors4.y };
		lines[7][0] = { boxVectors4.x, boxVectors4.y };
		lines[7][1] = { boxVectors7.x, boxVectors7.y };
		lines[8][0] = { boxVectors7.x, boxVectors7.y };
		lines[8][1] = { boxVectors6.x, boxVectors6.y };


		lines[9][0] = { boxVectors5.x, boxVectors5.y };
		lines[9][1] = { boxVectors1.x, boxVectors1.y };

		lines[10][0] = { boxVectors4.x, boxVectors4.y };
		lines[10][1] = { boxVectors2.x, boxVectors2.y };

		lines[11][0] = { boxVectors7.x, boxVectors7.y };
		lines[11][1] = { boxVectors3.x, boxVectors3.y };

		for (int i = 0; i < 12; i++)
		{
			Drawing::DrawLine(lines[i][0].x, lines[i][0].y, lines[i][1].x, lines[i][1].y, color);
		}
	}
}

void Drawing::rotate_point(Vector2D& point, Vector2D origin, bool clockwise, float angle) {
	Vector2D delta = point - origin;
	Vector2D rotated;

	if (clockwise) {
		rotated = Vector2D(delta.x * cosf(angle) - delta.y * sinf(angle), delta.x * sinf(angle) + delta.y * cosf(angle));
	}
	else {
		rotated = Vector2D(delta.x * sinf(angle) - delta.y * cosf(angle), delta.x * cosf(angle) + delta.y * sinf(angle));
	}

	point = rotated + origin;
}

void Drawing::DrawFilledCircle(int x, int y, int radius, int segments, Color color) {
	std::vector< Vertex_t > vertices;

	float step = M_PI * 2.0f / segments;

	for (float a = 0; a < (M_PI * 2.0f); a += step)
		vertices.emplace_back(radius * cosf(a) + x, radius * sinf(a) + y);

	TexturedPolygon(vertices.size(), vertices, color);
}

void Drawing::TexturedPolygon(int n, std::vector< Vertex_t > vertice, Color color) {
	static int texture_id = csgo.m_surface()->CreateNewTextureID(true); // 
	static unsigned char buf[4] = { 255, 255, 255, 255 };
	csgo.m_surface()->DrawSetTextureRGBA(texture_id, buf, 1, 1); //
	csgo.m_surface()->DrawSetColor(color); //
	csgo.m_surface()->DrawSetTexture(texture_id); //
	csgo.m_surface()->DrawTexturedPolygon(n, vertice.data()); //
}

/*
std::vector< Vertex_t > vertices =
	{
		Vertex_t{ Vector2D(position.x - size.x, position.y + size.y), Vector2D() },
		Vertex_t{ Vector2D(position.x, position.y - size.y), Vector2D() },
		Vertex_t{ position + size, Vector2D() },
		Vertex_t{  Vector2D(position.x, position.y + size.y * 0.2f), Vector2D() }
	};
*/


void Drawing::filled_tilted_triangle(Vector2D position, Vector2D size, Vector2D origin, bool clockwise, float angle, Color color, bool rotate, Color col2) {
	std::vector< Vertex_t > vertices =
	{
		Vertex_t{ Vector2D(position.x - size.x, position.y + size.y), Vector2D() },
		Vertex_t{ Vector2D(position.x, position.y + size.y * 0.3f), Vector2D() },
		Vertex_t{ position + size, Vector2D() },
	};

	std::vector< Vertex_t > vertices2 =
	{
		Vertex_t{ Vector2D(position.x - size.x, position.y + size.y), Vector2D() },
		Vertex_t{ Vector2D(position.x, position.y - size.y), Vector2D() },
		Vertex_t{ Vector2D(position.x, position.y + size.y * 0.3f), Vector2D() },
	};

	if (rotate) {
		for (unsigned int p = 0; p < vertices.size(); p++) {
			rotate_point(vertices[p].m_Position, origin, clockwise, angle);
		}

		for (unsigned int p = 0; p < vertices2.size(); p++) {
			rotate_point(vertices2[p].m_Position, origin, clockwise, angle);
		}
	}
	
	//int x, y, w, h;
	///Drawing::GetDrawingArea(x, y, w, h);
	//Drawing::LimitDrawingArea(position.y + size.y * 0.2f)

	TexturedPolygon(vertices.size(), vertices, color.malpha(1.f));

	//TexturedPolygon(vertices.size(), vertices, color.malpha(0.3f));

	//DrawLine(vertices[0].m_Position.x, vertices[0].m_Position.y, vertices2[1].m_Position.x, vertices2[1].m_Position.y, col2);
	//DrawLine(vertices2[1].m_Position.x, vertices2[1].m_Position.y, vertices[2].m_Position.x, vertices[2].m_Position.y, col2);
	//DrawLine(vertices[2].m_Position.x, vertices[2].m_Position.y, vertices[1].m_Position.x, vertices[1].m_Position.y, col2);
	//DrawLine(vertices[1].m_Position.x, vertices[1].m_Position.y, vertices[0].m_Position.x, vertices[0].m_Position.y, col2);

	//DrawLine(vertices2[1].m_Position.x, vertices2[1].m_Position.y, vertices[1].m_Position.x, vertices[1].m_Position.y, col2);

	//DrawLine(vertices[2].m_Position.x, vertices[2].m_Position.y, vertices[0].m_Position.x, vertices[0].m_Position.y, col2);

	//DrawLine(vertices[0].m_Position.x, vertices[0].m_Position.y, vertices[2].m_Position.x, vertices[2].m_Position.y, col2);

	//vertices.clear();
}

void Drawing::DrawCircle(float x, float y, float r, float s, Color color)
{
	float Step = M_PI * 2.0f / s;
	for (float a = 0; a < (M_PI * 2.0f); a += Step)
	{
		float x1 = r * cosf(a) + x;
		float y1 = r * sinf(a) + y;
		float x2 = r * cosf(a + Step) + x;
		float y2 = r * sinf(a + Step) + y;

		DrawLine(x1, y1, x2, y2, color);
	}
}
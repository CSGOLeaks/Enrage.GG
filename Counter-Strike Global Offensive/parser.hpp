#pragma once
#include "vdf_parser.hpp"
#include "optional.hpp"

class Color;

class item_t
{
public:
	uint16_t id = 0;
	std::string item_name;
	std::wstring translated_name;
};

template <typename T = item_t>
class item_list_t
{
public:
	std::vector<T> list;

	virtual std::string to_string() = 0;
};

class rarity_t
{
public:
	int32_t id = 0;
	std::string name;
	std::string Color_name;
	Color Color;
};

class rarity_list_t : public item_list_t<rarity_t>
{
public:

	rarity_t get_by_id(int id)
	{
		for (auto& rarity : list)
		{
			if (rarity.id == id) {
				return rarity;
			}
		}

		return list[0];
	}

	std::string to_string() override { return ""; }
};

class skin_t : public item_t
{
public:

	int32_t rarity = 0;
};

class skin_list_t : public item_list_t<skin_t>
{
private:
	std::string string_result;

public:

	tl::optional<skin_t&> get_by_id(uint16_t id)
	{
		for (auto& skin : list) {
			if (skin.id == id) {
				return skin;
			}
		}

		return tl::nullopt;
	}

	tl::optional<skin_t&> get_by_item_name(std::string name)
	{
		for (auto& skin : list) {
			if (skin.item_name == name) {
				return skin;
			}
		}

		return tl::nullopt;
	}

	std::string to_string() override
	{
		if (string_result.empty())
		{
			for (auto& skin : list)
			{
				std::string str(skin.translated_name.begin(), skin.translated_name.end());

				string_result += str;
				string_result.append("\0");
			}
			string_result.append("\0");
		}

		return string_result;
	}
};

class weapon_t : public item_t
{
public:
	std::string prefab;
	skin_list_t skins;
};

class weapon_list_t : public item_list_t<weapon_t>
{
private:
	std::string string_result;

public:

	tl::optional<weapon_t&> get_by_id(uint16_t id)
	{
		for (auto& weapon : list) {
			if (weapon.id == id) {
				return weapon;
			}
		}

		return tl::nullopt;
	}

	weapon_t* idi_nahui(uint16_t id)
	{
		for (auto& weapon : list) {
			if (weapon.id == id) {
				return &weapon;
			}
		}

		return 0;
	}

	tl::optional<weapon_t&> get_by_item_name(std::string name)
	{
		for (auto& weapon : list) {
			if (weapon.item_name == name) {
				return weapon;
			}
		}

		return tl::nullopt;
	}

	std::string to_string() override
	{
		if (string_result.empty())
		{
			string_result = "";

			for (auto& weapon : list)
			{
				std::string str(weapon.translated_name.begin(), weapon.translated_name.end());

				string_result += str;
				string_result.append("\0");
			}
			string_result.append("\0");
		}

		return string_result;
	}
};

class knife_t : public item_t
{
public:
	int model_player;
	int model_world;
	std::string	model_player_path;
	std::string  model_world_path;
};

class knife_list_t : public item_list_t<knife_t>
{
private:
	std::string string_result;

public:

	tl::optional<knife_t&> get_by_id(uint16_t id)
	{
		for (auto& knife : list) {
			if (knife.id == id) {
				return knife;
			}
		}

		return tl::nullopt;
	}

	std::string to_string() override
	{
		if (string_result.empty())
		{
			for (auto& knife : list)
			{
				std::string str(knife.translated_name.begin(), knife.translated_name.end());

				string_result += str;
				string_result.append("\0");
			}
			string_result.append("\0");
		}

		return string_result;
	}
};

namespace parser
{
	extern bool starts_with(std::string str, std::string v);
	extern bool ends_with(std::string str, std::string v);

	extern std::string to_lower(std::string str);
	extern std::wstring to_wlower(std::wstring str);

	extern vdf::document document;
	extern weapon_list_t weapons;
	extern knife_list_t knifes;
	extern knife_list_t default_knifes;
	extern rarity_list_t rarities;

	extern std::vector<skin_t*> get_skins_by_name(std::string name);

	extern const wchar_t* translate(std::string token);

	extern bool parse_rarities();

	extern bool parse_weapons();

	extern bool parse_skins();

	extern bool parse_knifes();

	extern bool sort_weapons();

	extern bool parse();
}
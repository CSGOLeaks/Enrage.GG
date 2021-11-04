#pragma once

#include <cstdio>
#include <vector>
#include <memory>
#include <string>
#include <deque>

namespace vdf
{
	class document;
	class keyvalue;
	class object;

	enum ENCODING
	{
		UTF8,
		UTF16_LE,
		UTF16_BE,
		UTF32_LE,
		UTF32_BE,
		MAX
	};

	enum
	{
		STRING = '"',
		OBJECT_OPEN = '{',
		OBJECT_CLOSE = '}',
		COMMENT = '/',
	};


	class str
	{
	public:
		char32_t* _start = 0;
		char32_t* _end = 0;
	public:
		std::string to_string()
		{
			if (!_start || !_end)
				return "";

			//convert utf32 to utf16
			std::vector<char16_t> utf16;
			utf16.reserve(200);

			for (char32_t* text = _start; text != _end + 1; text++)
			{
				if ((*text >= 0 && *text <= 0xD7FF) ||
					(*text >= 0xE000 && *text <= 0xFFFF))
				{
					utf16.push_back(*text);
				}
				if (*text >= 0x10000 && *text <= 0x10FFFF)
				{
					char32_t offset = *text - 0x10000;
					char16_t hi = (offset & 0xFFC00) >> 10;
					char16_t lo = offset & 0x3FF;
					hi += 0xD800;
					lo += 0xDC00;
					utf16.push_back(hi);
					utf16.push_back(lo);
				}
			}
			return std::string(utf16.begin(), utf16.end());
		}

		int to_int()
		{
			try { return atoi(to_string().c_str()); }
			catch (...) { return 0; }
		}

		float to_float()
		{
			try { return atof(to_string().c_str()); }
			catch (...) { return 0; }
		}

		//checks whether the next read character equals expected_tag, if not 0 is returned
		static char32_t* parse_text_expected_tag(char32_t* p, char32_t expected_tag, bool IGNORE_SPACE_TAB_LF_CR)
		{
			for (; *p; p++)
			{
				//skip comment line
				if (*p == COMMENT && *(p + 1) && *(p + 1) == COMMENT)
				{
					for (; *p && *p != '\n'; p++);
				}

				//skip options line
				// [�xbox|�360]
				if (*p == '[')
				{
					for (; *p && *p != ']'; p++);
					p++;
				}

				if (IGNORE_SPACE_TAB_LF_CR)
				{
					if (*p == '\n' || *p == '\r' || *p == '\t' || *p == ' ')
						continue;
				}

				if (*p == expected_tag)
					return p;
				else
					return 0;
			}

			return 0;
		}

		static char32_t* end_reached(char32_t* p, char32_t expectedTag)
		{
			for (; *p; p++)
			{
				//skip comment line
				if (*p == COMMENT && *(p + 1) && *(p + 1) == COMMENT)
				{
					for (; *p && *p != '\n'; p++);
				}

				//skip options line
				// [�xbox|�360]
				if (*p == '[')
				{
					for (; *p && *p != ']'; p++);
					p++;
				}

				if (*p == '\n' || *p == '\r' || *p == '\t' || *p == ' ')
					continue;

				if (*p == expectedTag)
					return p;
				else
					return (char32_t*)-1;
			}

			return 0;
		}

		//reads string until it ecnounters endTag
		static char32_t* parse_text_end_tag(char32_t* p, char32_t endTag)
		{
			for (; *p; p++)
			{
				//skip escape sequence
				//there are also hex escapes
				//\xAA45, no need to handle them tho
				if (*p == '\\')
					p += 2;

				if (*p == endTag)
					return p;
			}

			return 0;
		}
	};

	class util
	{
	public:
		static bool str_equ(str str1, char* str2)
		{
			return str1.to_string() == std::string(str2);
		}

		static bool str_equ_i(str str1, char* str2)
		{
			std::string str = str1.to_string();
			return str.compare(str2) == std::string::npos;
		}
	};

	class node
	{
	protected:
		document* _doc;
	public:
		std::vector<std::shared_ptr<node>> children;
		virtual bool parse();

		node(document* doc)
		{
			_doc = doc;
		}

		virtual object* to_object()
		{
			return 0;
		}

		virtual keyvalue* to_keyvalue()
		{
			return 0;
		}
	};

	class keyvalue : public node
	{
	public:
		str key;
		str value;

		keyvalue(document* doc) : node(doc) {}

		keyvalue* to_keyvalue()
		{
			return this;
		}
	};

	class object : public node
	{
	public:
		str name;
		object(document* doc);
		object* to_object();
		bool parse();
		std::shared_ptr<node> get_object_by_name(char* name);
		std::shared_ptr<node> get_key_by_name(char* name);
	};

	class document
	{
	private:
		friend class object;
		friend class node;

		struct BOM
		{
			const char* str;
			size_t len;
		};
		BOM BOMS[ENCODING::MAX];

		char32_t* utf32text = 0;
		char32_t* p;
	public:
		std::shared_ptr<node> root;

		document()
		{
			BOMS[UTF8] = { "\xEF\xBB\xBF", 3 };
			BOMS[UTF16_LE] = { "\xFF\xFE", 2 };
			BOMS[UTF16_BE] = { "\xFE\xFF", 2 };
			BOMS[UTF32_LE] = { "\xFF\xFE\x00\x00", 4 };
			BOMS[UTF32_BE] = { "\x00\x00\xFE\xFF", 4 };
		}

		~document()
		{
			if (utf32text)
				delete[] utf32text;
		}

		std::shared_ptr<node> breadth_first_search(char* name, bool case_insensitive = false)
		{
			std::deque<std::shared_ptr<node>> q;
			q.push_back(root);
			while (!q.empty())
			{
				std::shared_ptr<node> f = q.front();
				q.pop_front();

				if (f->to_keyvalue())
				{
					if (!case_insensitive && util::str_equ(f->to_keyvalue()->key, name))
						return f;

					if (case_insensitive && util::str_equ_i(f->to_keyvalue()->key, name))
						return f;
				}
				if (f->to_object())
				{
					if (!case_insensitive && util::str_equ(f->to_object()->name, name))
						return f;

					if (case_insensitive && util::str_equ_i(f->to_object()->name, name))
						return f;
				}

				for (auto& child : f->children)
					q.push_back(child);
			}

			return 0;
		}

		std::vector<std::shared_ptr<node>> breadth_first_search_multiple(char* name, bool case_insensitive = false)
		{
			std::vector<std::shared_ptr<node>> vec;
			std::deque<std::shared_ptr<node>> q;
			q.push_back(root);
			while (!q.empty())
			{
				std::shared_ptr<node> f = q.front();
				q.pop_front();

				if (f->to_keyvalue())
				{
					if (!case_insensitive && util::str_equ(f->to_keyvalue()->key, name))
						vec.push_back(f);

					if (case_insensitive && util::str_equ_i(f->to_keyvalue()->key, name))
						vec.push_back(f);
				}
				if (f->to_object())
				{
					if (!case_insensitive && util::str_equ(f->to_object()->name, name))
						vec.push_back(f);

					if (case_insensitive && util::str_equ_i(f->to_object()->name, name))
						vec.push_back(f);
				}

				for (auto& child : f->children)
					q.push_back(child);
			}

			return vec;
		}

		//returns true when format is correct
		//parse from file
		bool load(char* path, ENCODING encoding)
		{
			auto f = fopen(path, "rb");
			if (!f)
				return false;

			fseek(f, 0, SEEK_END);
			auto size = ftell(f);
			if (!size)
				return false;

			size_t null_terminator_BYTEs = 0;
			if (encoding == ENCODING::UTF16_BE ||
				encoding == ENCODING::UTF16_LE)
				null_terminator_BYTEs = 2;

			if (encoding == ENCODING::UTF8)
				null_terminator_BYTEs = 1;

			if (encoding == ENCODING::UTF32_BE ||
				encoding == ENCODING::UTF32_LE)
				null_terminator_BYTEs = 4;

			char* _text = new char[size + null_terminator_BYTEs];
			fseek(f, 0, SEEK_SET);
			if (fread(_text, 1, size, f) != size)
			{
				delete[] _text;
				return false;
			}

			for (int i = 0; i < (int)null_terminator_BYTEs; i++)
				_text[size + i] = 0;

			fclose(f);

			//convert to utf32
			//which makes everything easy to handle
			//but increases buffer size
			cnv_to_utf32(_text, encoding);
			delete[] _text;

			p = trim_space(utf32text);
			if (!p)
				return false;

			//return true;
			return parse_deep();
		}
	private:
		char* skip_BOM(char* p, ENCODING encoding)
		{
			if (!memcmp(p, BOMS[encoding].str, BOMS[encoding].len))
				return p + BOMS[encoding].len;
			else
				return p;
		}

		void cnv_to_utf32(char* text, ENCODING encoding)
		{
			if (encoding == ENCODING::UTF16_LE)
			{
				char* bom = skip_BOM(text, ENCODING::UTF16_LE);
				cnv_utf16_le(bom);
			}

			if (encoding == ENCODING::UTF16_BE)
			{
				char* bom = skip_BOM(text, ENCODING::UTF16_BE);
				cnv_utf16_be(bom);
			}

			if (encoding == ENCODING::UTF8)
			{
				char* bom = skip_BOM(text, ENCODING::UTF8);
				cnv_utf8(bom);
			}

			if (encoding == ENCODING::UTF32_LE)
			{
				char* bom = skip_BOM(text, ENCODING::UTF32_LE);
				cnv_utf32_le(bom);
			}

			if (encoding == ENCODING::UTF32_BE)
			{
				char* bom = skip_BOM(text, ENCODING::UTF32_BE);
				cnv_utf32_be(bom);
			}
		}

		size_t cnv_utf16_be_len(char* text)
		{
			size_t len = 0;
			char16_t* u16text = (char16_t*)text;
			while (*u16text)
			{

				char16_t c = 0;
				char* t = (char*)u16text;
				c |= (char16_t) * (t + 1) << 8;
				c |= *t;

				//4 BYTEs
				if (c >= 0xD800 && c <= 0xDFFF)
					u16text++;

				len++;
				u16text++;
			}

			return len;
		}

		size_t cnv_utf16_le_len(char* text)
		{
			size_t len = 0;
			char16_t* u16text = (char16_t*)text;
			while (*u16text)
			{
				if (*u16text >= 0xD800 && *u16text <= 0xDFFF)
					u16text++;

				len++;
				u16text++;
			}

			return len;
		}

		void cnv_utf16_be(char* text)
		{
			auto s = cnv_utf16_be_len(text);
			utf32text = new char32_t[s + 1];
			utf32text[s] = 0;
			int i = 0;

			char16_t* u16text = (char16_t*)text;
			char32_t k;

			while (*u16text)
			{

				char16_t c = 0;
				char* t = (char*)u16text;
				c |= (char16_t) * (t + 1) << 8;
				c |= *t;

				//4 BYTEs
				if (c >= 0xD800 && c <= 0xDFFF)
				{
					char16_t hi = c - 0xD800;

					t = (char*)(u16text + 1);
					c |= (char16_t) * (t + 1) << 8;
					c |= *t;
					char16_t lo = c - 0xDC00;

					k = (char32_t)(hi & 0x3FF) << 10;
					k |= lo & 0x3FF;
					k += 0x10000;
					u16text++;
				}
				else //2 BYTEs
				{
					k = c;
				}
				utf32text[i] = k;
				i++;
				u16text++;
			}
		}

		void cnv_utf16_le(char* text)
		{
			auto s = cnv_utf16_le_len(text);
			utf32text = new char32_t[s + 1];
			utf32text[s] = 0;
			int i = 0;

			char16_t* u16text = (char16_t*)text;
			char32_t k;

			while (*u16text)
			{
				//4 BYTEs
				if (*u16text >= 0xD800 && *u16text <= 0xDFFF)
				{
					char16_t hi = *u16text - 0xD800;
					char16_t lo = *(u16text + 1) - 0xDC00;
					k = (char32_t)(hi & 0x3FF) << 10;
					k |= lo & 0x3FF;
					k += 0x10000;
					u16text++;
				}
				else //2 BYTEs
				{
					k = *u16text;
				}
				utf32text[i] = k;
				i++;
				u16text++;
			}
		}

		size_t cnv_utf8_len(char* text)
		{
			size_t len = 0;
			unsigned char c = (unsigned char)* text;

			while (c)
			{
				if (c >= 0xc0 && c <= 0xdf)
				{
					text++;
				}
				if (c >= 0xe0 && c <= 0xef)
				{
					text += 2;
				}
				if (c >= 0xf0 && c <= 0xf7)
				{
					text += 3;
				}

				len++;
				c = *++text;
			}

			return len;
		}

		void cnv_utf8(char* text)
		{
			auto s = cnv_utf8_len(text);
			utf32text = new char32_t[s + 1];
			utf32text[s] = 0;
			int i = 0;

			unsigned char c = (unsigned char)* text;

			while (c)
			{
				char32_t k = 0;
				if (c >= 0 && c <= 0x7f)
				{
					k = c;
				}
				if (c >= 0xc0 && c <= 0xdf)
				{
					k = (char32_t)(c ^ 0xc0) << 6;
					c = *++text;
					k |= c ^ 0x80;
				}
				if (c >= 0xe0 && c <= 0xef)
				{
					k = (char32_t)(c ^ 0xe0) << 12;
					c = *++text;
					k |= (char32_t)(c ^ 0x80) << 6;
					c = *++text;
					k |= c ^ 0x80;
				}
				if (c >= 0xf0 && c <= 0xf7)
				{
					k = (char32_t)(c ^ 0xf0) << 18;
					c = *++text;
					k |= (char32_t)(c ^ 0x80) << 12;
					c = *++text;
					k |= (char32_t)(c ^ 0x80) << 6;
					c = *++text;
					k |= c ^ 0x80;
				}
				utf32text[i] = k;
				i++;
				c = *++text;
			}
		}

		size_t cnv_utf32_len(char* text)
		{
			size_t len = 0;
			char32_t* p = (char32_t*)text;

			while (*p)
			{
				len++;
				p++;
			}

			return len;
		}

		void cnv_utf32_le(char* text)
		{
			auto s = cnv_utf32_len(text);
			utf32text = new char32_t[s + 1];
			utf32text[s] = 0;
			int i = 0;

			char32_t* p = (char32_t*)text;

			while (*p)
			{
				utf32text[i] = *p;
				i++;
				p++;
			}
		}

		void cnv_utf32_be(char* text)
		{
			auto s = cnv_utf32_len(text);
			utf32text = new char32_t[s + 1];
			utf32text[s] = 0;
			int i = 0;

			char32_t* p = (char32_t*)text;
			char32_t k;

			while (*p)
			{
				char* t = (char*)p;
				k = (char32_t) * (t + 3) << 24;
				k |= (char32_t) * (t + 2) << 16;
				k |= (char32_t) * (t + 1) << 8;
				k |= *t;

				utf32text[i] = k;
				i++;
				p++;
			}
		}

		char32_t* trim_space(char32_t* p)
		{
			while (*p)
			{
				if (*p == ' ')
					p++;
				else
					return p;
			}

			return 0;
		}

		bool identify(std::shared_ptr<node>& node)
		{
			auto string_begin = str::parse_text_expected_tag(p, STRING, true);
			if (!string_begin)
				return false;

			auto string_end = str::parse_text_end_tag(string_begin + 1, STRING);
			if (!string_end)
				return false;


			auto object_open = str::parse_text_expected_tag(string_end + 1, OBJECT_OPEN, true);
			auto val_start = str::parse_text_expected_tag(string_end + 1, STRING, true);

			if (!object_open && !val_start)
				return false;

			if (object_open)
			{
				std::shared_ptr<object> obj = std::make_shared<object>(this);
				obj->name._start = string_begin + 1;
				obj->name._end = string_end - 1;
				node = obj;
				p = object_open + 1;
				return true;
			}

			if (val_start)
			{
				auto val_end = str::parse_text_end_tag(val_start + 1, STRING);
				if (!val_end)
					return false;

				std::shared_ptr<keyvalue> keyVal = std::make_shared<keyvalue>(this);
				keyVal->key._start = string_begin + 1;
				keyVal->key._end = string_end - 1;
				keyVal->value._start = val_start + 1;
				keyVal->value._end = val_end - 1;
				node = keyVal;
				p = val_end + 1;
				return true;
			}

			return false;
		}

		bool parse_deep()
		{
			root = std::make_shared<node>(this);
			return root->parse();
		}
	};
}
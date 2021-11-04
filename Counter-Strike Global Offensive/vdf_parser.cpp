#include "vdf_parser.hpp"

namespace vdf
{
	object::object(document* doc) : node(doc) {}

	object* object::to_object()
	{
		return this;
	}

	std::shared_ptr<node> object::get_object_by_name(char* name)
	{
		for (auto& child : children)
		{
			if (child->to_object())
			{
				if (util::str_equ(child->to_object()->name, name))
					return child;
			}
		}

		return 0;
	}

	std::shared_ptr<node> object::get_key_by_name(char* name)
	{
		for (auto& child : children)
		{
			if (child->to_keyvalue())
			{
				if (util::str_equ(child->to_keyvalue()->key, name))
					return child;
			}
		}

		return 0;
	}

	bool object::parse()
	{
		std::shared_ptr<node> n;

		while (*_doc->p)
		{
			//check for object close
			auto string_begin = str::parse_text_expected_tag(_doc->p, STRING, true);
			if (!string_begin)
			{
				auto obj_close = str::parse_text_expected_tag(_doc->p, OBJECT_CLOSE, true);
				if (obj_close)
				{
					_doc->p = obj_close + 1;
					return true;
				}
				else
					return false;
			}

			if (!_doc->identify(n))
				return false;

			if (n->to_keyvalue())
			{
				this->children.push_back(n);
			}

			if (n->to_object())
			{
				this->children.push_back(n);
				object* obj = n->to_object();
				if (!obj->parse())
					return false;
			}
		}

		return false;
	}

	bool node::parse()
	{
		std::shared_ptr<node> n;

		while (*_doc->p)
		{
			if (!_doc->identify(n))
			{
				if (!str::end_reached(_doc->p, OBJECT_OPEN) &&
					!str::end_reached(_doc->p, OBJECT_CLOSE) &&
					!str::end_reached(_doc->p, STRING))
					return true;
				else
					return false;
			}

			if (n->to_keyvalue())
			{
				this->children.push_back(n);
			}

			if (n->to_object())
			{
				this->children.push_back(n);
				object* obj = n->to_object();
				if (!obj->parse())
					return false;
			}
		}

		//return false;
		return true;
	}
}
//#pragma once
//
//#include "sdk.hpp"
//#include <curl\curl.h>
//
//#define RandomInt(min, max) (rand() % (max - min + 1) + min)
//
//class PostRequest
//{
//public:
//	PostRequest(const std::string& script)
//		: m_script(script)
//	{
//
//	}
//
//	void Set(const std::string& name, const std::string& value)
//	{
//		if (m_data.empty())
//			m_data += name + "=" + value;
//		else
//			m_data += "&" + name + "=" + value;
//	}
//
//	const std::string& GetScript() const
//	{
//		return m_script;
//	}
//
//	const std::string& GetData() const
//	{
//		return m_data;
//	}
//
//private:
//	std::string m_script;
//	std::string m_data;
//};
//
//class Internet
//{
//public:
//	~Internet();
//
//	std::string GetIP(std::string hostname);
//
//	bool Connect(const std::string& url);
//	void Disconnect();
//
//	bool SendRequest(const PostRequest& request, std::string& response);
//
//	static Internet& Instance()
//	{
//		static Internet instance;
//		return instance;
//	}
//
//	std::string random_string(size_t length)
//	{
//		auto randchar = []() -> char
//		{
//			const char charset[] =
//				"0123456789"
//				"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
//				"abcdefghijklmnopqrstuvwxyz";
//			const size_t max_index = (sizeof(charset) - 1);
//			return charset[rand() % max_index];
//		};
//		std::string str(length, 0);
//		std::generate_n(str.begin(), length, randchar);
//		return str;
//	}
//
//	std::string base64_encode(const std::string& in)
//	{
//		std::string out;
//
//		int val = 0, valb = -6;
//		for (unsigned char c : in) {
//			val = (val << 8) + c;
//			valb += 8;
//			while (valb >= 0) {
//				out.push_back("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/"[(val >> valb) & 0x3F]);
//				valb -= 6;
//			}
//		}
//		if (valb > -6) out.push_back("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/"[((val << 8) >> (valb + 8)) & 0x3F]);
//		while (out.size() % 4) out.push_back('=');
//		return out;
//	}
//
//	std::string encryptDecrypt(std::string toEncrypt)
//	{
//		char key[] = { 'K','c','8','e','7','I','a','6','J','1','X','G','n','u','V','w','q','D','2','Z','V','h','t','5','k','b','o','U','6','F','S','5' }; //Any chars will work
//
//		std::string output = toEncrypt;
//
//		for (auto i = 0; i < (int)toEncrypt.size(); i++)
//			output[i] = toEncrypt[i] ^ key[i % (sizeof(key) / sizeof(char))];
//
//		return output;
//	}
//
//	std::string base64_decode(const std::string& in)
//	{
//		std::string out;
//
//		std::vector<int> T(256, -1);
//		for (int i = 0; i < 64; i++) T["ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/"[i]] = i;
//
//		int val = 0, valb = -8;
//		for (unsigned char c : in) {
//			if (T[c] == -1) break;
//			val = (val << 6) + T[c];
//			valb += 6;
//			if (valb >= 0) {
//				out.push_back(char((val >> valb) & 0xFF));
//				valb -= 8;
//			}
//		}
//		return out;
//	}
//
//private:
//	static std::size_t ResponseProxy(void* data, std::size_t count, std::size_t size, void* user_data);
//
//private:
//	bool m_connected = false;
//	std::string m_url;
//	CURL* m_curl = nullptr;
//};
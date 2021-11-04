//#include "internet.hpp"
//#include "source.hpp"
//#include <WinInet.h>
//#include <IPHlpApi.h>
//#include <WbemIdl.h>
//#include <iphlpapi.h>
//#include <Psapi.h>
//#include <Windows.h>
//#include <Winternl.h>
//#include <TlHelp32.h>
//#pragma comment(lib, "iphlpapi.lib")
//#pragma comment(lib,"wininet.lib")
//#pragma comment(lib, "urlmon.lib")
//#pragma comment(lib,"WS2_32.lib")
//
//Internet::~Internet()
//{
//	Disconnect();
//}
//
////std::string Internet::GetIP(std::string hostname)
////{
////	WSADATA wsaData;
////	IN_ADDR addr;
////	HOSTENT* list_ip;
////	WSAStartup(MAKEWORD(2, 0), &wsaData);
////	list_ip = gethostbyname(hostname.c_str());
////	if (list_ip) {
////		memcpy(&addr.S_un.S_addr, list_ip->h_addr, list_ip->h_length);
////		return inet_ntoa(addr);
////		WSACleanup();
////	}
////	else
////		return "";
////}
//
//bool Internet::Connect(const std::string& url)
//{
//	Disconnect();
//
//	/*static bool firsttimeno = (strcmp(GetIP(sxor("")).c_str(), sxor("31.31.196.209")) == 0);
//
//	if (!firsttimeno) return false;*/
//
//	m_url = url;
//
//	if (m_url.empty())
//		return false;
//
//	auto code = curl_global_init(CURL_GLOBAL_ALL);
//
//	if (code != CURLE_OK)
//		return false;
//
//	m_connected = true;
//
//	m_curl = curl_easy_init();
//
//	if (!m_curl)
//		return false;
//
//	return true;
//}
//
//void Internet::Disconnect()
//{
//	if (m_connected)
//	{
//		if (m_curl)
//			curl_easy_cleanup(m_curl);
//
//		curl_global_cleanup();
//	}
//
//	m_connected = false;
//	m_url.clear();
//	m_curl = nullptr;
//}
//
//struct PostResponse
//{
//	PostResponse(char* data, std::size_t size)
//		: m_data(data)
//		, m_size(size)
//	{}
//
//	char* m_data = nullptr;
//	std::size_t m_size = 0;
//};
//
//bool Internet::SendRequest(const PostRequest& request, std::string& response)
//{
//	response.clear();
//
//	auto post_script = m_url + request.GetScript() + "?" + request.GetData();
//
//	curl_easy_setopt(m_curl, CURLOPT_URL, post_script.c_str());
//
//#ifndef _DEBUG
//	curl_easy_setopt(m_curl, CURLOPT_USERAGENT, std::string(sxor("2K17_") + std::string(ctx.data->username)).c_str());
//#endif
//
//	PostResponse post_response{ (char*)malloc(1), 0 };
//	curl_easy_setopt(m_curl, CURLOPT_WRITEDATA, &post_response);
//	curl_easy_setopt(m_curl, CURLOPT_WRITEFUNCTION, ResponseProxy);
//
//	auto post_code = curl_easy_perform(m_curl);
//
//	response += post_response.m_data + static_cast<char>(0);
//
//	free(post_response.m_data);
//
//	return (post_code == CURLE_OK);
//}
//
//std::size_t Internet::ResponseProxy(void* data, std::size_t count, std::size_t size, void* user_data)
//{
//	auto response_size = count * size;
//	auto response_data = (PostResponse*)user_data;
//
//	auto previous_size = response_data->m_size;
//
//	response_data->m_data = (char*)realloc(response_data->m_data, previous_size + response_size + 1);
//
//	memcpy(&response_data->m_data[previous_size], data, response_size);
//	response_data->m_size += response_size;
//	response_data->m_data[response_data->m_size] = static_cast<char>(0);
//
//	return response_size;
//}
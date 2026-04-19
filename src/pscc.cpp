/*
	PanasonicSmartChinaControl
	Copyright (C) 2026  realjhen123

	This program is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/
#include <iostream>
#include <string>
#include <iomanip>
#include <json/json.h>
#include <curl/curl.h>
#include "openssl/sha.h"
#include "openssl/evp.h"
#include "openssl/err.h"
#include "openssl/ssl.h"
#include "openssl/md5.h"

#define SUCCEED 0
#ifdef _DEBUG 
bool debugmode = true;
#elif
bool debugmode = false;
#endif

class jsonfile {
public:
    static Json::Value readJsonFromString(const std::string& mystr)
    {
        Json::CharReaderBuilder ReaderBuilder;
        ReaderBuilder["emitUTF8"] = true;
        std::unique_ptr<Json::CharReader> charread(ReaderBuilder.newCharReader());
        Json::Value root;
        std::string strerr;
        bool isok = charread->parse(mystr.c_str(), mystr.c_str() + mystr.size(), &root, &strerr);
        return root;
    }
	static std::string jsontoString(const Json::Value& json_val, bool pretty = false) {
		Json::StreamWriterBuilder builder;
		builder["emitUTF8"] = true;
		if (pretty) {
			builder["indentation"] = "\t";
		}
		else {
			builder["indentation"] = "";
		}
		builder["commentStyle"] = "None";
		builder["allowComments"] = false;
		std::string json_str;
		try {
			json_str = Json::writeString(builder, json_val);
		}
		catch (const Json::Exception& e) {
			return "";
		}
		return json_str;
	}
};
class PanasonicSmartChinaControl {
private:
	CURL* curl_;
	struct curl_slist* headers = nullptr;
	std::string username;
	std::string password;
	std::string usrId;
	std::string ssId;
	int realFamilyId = 0;
	int familyId = 0;
	const char* PSC_URL_GET_TOKEN = "https://app.psmartcloud.com/App/UsrGetToken";
	const char* PSC_URL_LOGIN = "https://app.psmartcloud.com/App/UsrLogin";
	const char* PSC_URL_GET_DEV = "https://app.psmartcloud.com/App/UsrGetBindDevInfo";
	const char* PSC_URL_SET = "https://app.psmartcloud.com/App/ACDevSetStatusInfoAW";
	const char* PSC_URL_GET = "https://app.psmartcloud.com/App/ACDevGetStatusInfoAW";
	std::string calcpassword(std::string rawToken_) {
		return PanasonicSmartChinaControl::md5_forPSC(PanasonicSmartChinaControl::md5_forPSC(PanasonicSmartChinaControl::md5_forPSC(this->password) + this->username) + rawToken_);
	}
	std::string getToken(std::string username_) {
		Json::Value req;
		std::string res;
		req["id"] = 1;
		req["uiVersion"] = 4.0;
		req["params"]["usrId"] = username_;
		this->POST(this->PSC_URL_GET_TOKEN, jsonfile::jsontoString(req), res);
		Json::Value response = jsonfile::readJsonFromString(res);
		return response["results"]["token"].asString();
	}
	static std::string md5_forPSC(const std::string& data) {
		unsigned char digest[MD5_DIGEST_LENGTH];
		MD5(reinterpret_cast<const unsigned char*>(data.c_str()), data.size(), digest);
		char buf[MD5_DIGEST_LENGTH * 2 + 1];
		for (int i = 0; i < MD5_DIGEST_LENGTH; ++i) {
			sprintf(&buf[i * 2], "%02X", digest[i]);
		}
		return buf;
	}
	static std::string sha512_forPSC(const std::string& input) {
		unsigned char hash[SHA512_DIGEST_LENGTH];
		SHA512_CTX ctx;
		SHA512_Init(&ctx);
		SHA512_Update(&ctx, input.data(), input.size());
		SHA512_Final(hash, &ctx);
		std::ostringstream oss;
		oss << std::hex << std::setfill('0');
		for (int i = 0; i < SHA512_DIGEST_LENGTH; ++i) {
			oss << std::setw(2) << static_cast<unsigned int>(hash[i]);
		}
		return oss.str();
	}
	int POST(std::string url_, std::string requestbody_, std::string& response) {
		curl_easy_setopt(this->curl_, CURLOPT_URL, url_.c_str());
		curl_easy_setopt(this->curl_, CURLOPT_FOLLOWLOCATION, 1L);
		curl_easy_setopt(this->curl_, CURLOPT_WRITEFUNCTION, PanasonicSmartChinaControl::WriteCallback);
		curl_easy_setopt(this->curl_, CURLOPT_WRITEDATA, &response);
		curl_easy_setopt(this->curl_, CURLOPT_HTTPHEADER, this->headers);
		curl_easy_setopt(this->curl_, CURLOPT_POST, 1L);
		curl_easy_setopt(this->curl_, CURLOPT_POSTFIELDS, requestbody_.c_str());
		curl_easy_setopt(this->curl_, CURLOPT_POSTFIELDSIZE, requestbody_.size());
		curl_easy_setopt(this->curl_, CURLOPT_SSL_VERIFYPEER, 0L);
		curl_easy_setopt(this->curl_, CURLOPT_SSL_VERIFYHOST, 0L);
		CURLcode res = curl_easy_perform(this->curl_);
		return res;
	}
	int GET(std::string url_, std::string& response) {
		curl_easy_setopt(this->curl_, CURLOPT_URL, url_.c_str());
		curl_easy_setopt(this->curl_, CURLOPT_FOLLOWLOCATION, 1L);
		curl_easy_setopt(this->curl_, CURLOPT_WRITEFUNCTION, PanasonicSmartChinaControl::WriteCallback);
		curl_easy_setopt(this->curl_, CURLOPT_WRITEDATA, &response);
		curl_easy_setopt(this->curl_, CURLOPT_HTTPHEADER, this->headers);
		curl_easy_setopt(this->curl_, CURLOPT_SSL_VERIFYPEER, 0L);
		curl_easy_setopt(this->curl_, CURLOPT_SSL_VERIFYHOST, 0L);
		CURLcode res = curl_easy_perform(this->curl_);
		return res;
	}
	int addHeader(std::string header) {
		this->headers = curl_slist_append(this->headers, header.c_str());
		return SUCCEED;
	}
	static std::string GenerateToken(std::string deviceId) {
		std::string mac = deviceId.substr(0, 12);
		std::string category = deviceId.substr(13, 4);
		std::string suffix = deviceId.substr(18);
		std::string stoken = PanasonicSmartChinaControl::sha512_forPSC(mac.substr(6) + '_' + category + '_' + mac.substr(0, 6));
		std::string token = PanasonicSmartChinaControl::sha512_forPSC(stoken + '_' + suffix);
		return token;
	}
public:
	PanasonicSmartChinaControl() {
		curl_global_init(CURL_GLOBAL_ALL);
		curl_ = curl_easy_init();
	}
	~PanasonicSmartChinaControl() {
		curl_slist_free_all(this->headers);
		curl_easy_cleanup(this->curl_);
		curl_global_cleanup();
	}
	int Login(std::string username_, std::string password_) {
		this->username = username_;
		this->password = password_;
		std::string pwd = this->calcpassword(
			this->getToken(this->username));
		std::string r_res;
		{
			Json::Value req;
			req["id"] = 2;
			req["uiVersion"] = 4.0;
			req["params"]["telId"] = "00:00:00:00:00:00";
			req["params"]["checkFailCount"] = 0;
			req["params"]["usrId"] = this->username;
			req["params"]["pwd"] = pwd;
			this->POST(this->PSC_URL_LOGIN, jsonfile::jsontoString(req), r_res);
		}
		Json::Value res = jsonfile::readJsonFromString(r_res);
		this->usrId = res["results"]["usrId"].asString();
		this->ssId = res["results"]["ssId"].asString();
		this->realFamilyId = res["results"]["realFamilyId"].asInt();
		this->familyId = res["results"]["familyId"].asInt();
		return SUCCEED;
	}
	std::string GetDevice() {
		this->addHeader("Cookie: SSID=" + this->ssId);
		std::string r_res;
		{
			Json::Value req;
			req["id"] = 3;
			req["uiVersion"] = 4.0;
			req["params"]["realFamilyId"] = this->realFamilyId;
			req["params"]["familyId"] = this->familyId;
			req["params"]["usrId"] = this->usrId;
			this->POST(this->PSC_URL_GET_DEV, jsonfile::jsontoString(req), r_res);
		}
		if (debugmode)std::cout << jsonfile::readJsonFromString(r_res).toStyledString();
		return r_res;
	}

	int Set(std::string deviceId) {
		Json::Value req;
		req["id"] = 100;
		req["usrId"] = this->usrId;
		req["deviceId"] = deviceId;
		req["token"] = PanasonicSmartChinaControl::GenerateToken(deviceId);
		req["params"]["runStatus"] = 1;
		if (debugmode)std::cout << req.toStyledString();
		if (debugmode)std::cout << this->ssId;
		std::string r_res;
		this->POST(this->PSC_URL_SET, jsonfile::jsontoString(req), r_res);
		if (debugmode)std::cout << jsonfile::readJsonFromString(r_res).toStyledString();
		return SUCCEED;
	}
	int Init() {
		this->addHeader("User-Agent: Mozilla/5.0 (iPhone; CPU iPhone OS 18_5 like Mac OS X");
		this->addHeader("Content-Type: application/json");
		return SUCCEED;
	}
	static size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userp) {
		size_t realsize = size * nmemb;
		std::string* str = static_cast<std::string*>(userp);
		str->append(static_cast<char*>(contents), realsize);
		return realsize;
	}
};
int main() {
	std::string username, password;
	username = "";
	password = "";
	PanasonicSmartChinaControl pscc;
	pscc.Init();
	pscc.Login(username, password);
	pscc.GetDevice();
	std::string deviceId;
	deviceId = "";
	pscc.Set(deviceId);
}
#include "httpClient.h"
 
static size_t downloadCallback(void *buffer, size_t sz, size_t nmemb, void *writer)
{
	std::string* psResponse = (std::string*) writer;
	size_t size = sz * nmemb;
	psResponse->append((char*) buffer, size);
	return sz * nmemb;
}


CURLcode httpGet(std::string strUrl, std::string *response ) {
	CURL *curl = curl_easy_init();
    curl_easy_setopt(curl, CURLOPT_URL, strUrl.c_str());
	curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1L);
	curl_easy_setopt(curl, CURLOPT_TIMEOUT, 2); 
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, downloadCallback); 
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, response);
	CURLcode res = curl_easy_perform(curl);
	curl_easy_cleanup(curl);
	return res;
}

CURLcode httpPost(std::string strUrl, std::string params, std::string *response ) {
    CURL *curl = curl_easy_init();
    curl_easy_setopt(curl, CURLOPT_URL, strUrl.c_str());
    curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1L);
	curl_easy_setopt(curl, CURLOPT_TIMEOUT, 2);
    
    curl_slist *http_headers = NULL;
    http_headers = curl_slist_append(http_headers, "Content-Type: application/json");
    http_headers = curl_slist_append(http_headers, "charsets: utf-8");
    curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "POST");
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, http_headers);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, params.c_str());
    
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, downloadCallback); 
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, response);
	CURLcode res = curl_easy_perform(curl);
	curl_easy_cleanup(curl);
    return res;
}

int main()
{
    std::string strResponse;
	std::string strUrl = "http://localhost:8888/test/";
    std::string params = "{\"type\": \"cron\", \"detail\": { \"interval\": 10, \"command\": \"sh mo_load.sh\", \"name\": \"mo_load\"}}";
    //CURLcode res = httpGet(strUrl, &strResponse );
    CURLcode res = httpPost(strUrl, params, &strResponse );
    if (res != CURLE_OK)
	{
		strResponse = "error";
	}
	
	printf("strResponse is |%s|\n", strResponse.c_str());
	return 0;
}

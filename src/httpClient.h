#ifndef HTTPCLIENT_H
#define HTTPCLIENT_H

#include "curl.h"
#include <string>

CURLcode httpGet(std::string strUrl, std::string *response );
CURLcode httpPost(std::string strUrl, std::string params, std::string *response );

#endif
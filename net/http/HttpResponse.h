#ifndef NET_HTTP_HTTPRESPONSE_H
#define NET_HTTP_HTTPRESPONSE_H

#include <map>
#include <string>

#include "base/copyable.h"
#include "net/Buffer.h"
class HttpResponse: copyable {
public:
    enum HttpStatusCode {
        kUnknown,
        k200 = 200,
        k404 = 404,
        k500 = 500
    };
    HttpResponse()
        : statusCode_(kUnknown)
        , statusMessage_()
    {
    }


    void setHeader(std::string& field, std::string& value)
    {
        headers_[field] = value;
    }
    void setHeader(std::string&& field, std::string&& value)
    {
        setHeader(field, value);
    }
    void setBody(std::string& body)
    {
        body_ = body;
    }
    void setBody(std::string&& body)
    {
        setBody(body);
    }
    void setStatusMessage(std::string statusMessage)
    {
        statusMessage_ = statusMessage;
    }
    void setStatusCode(HttpStatusCode statusCode)
    {
        statusCode_ = statusCode;
    }
    void appendToBuffer(Buffer* output);

private:
    std::map<std::string, std::string> headers_;
    std::string statusMessage_;
    HttpStatusCode statusCode_;
    std::string body_;
};
#endif

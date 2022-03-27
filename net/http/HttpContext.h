#ifndef NET_HTTP_HTTPCONTEXT_H
#define NET_HTTP_HTTPCONTEXT_H

#include "base/copyable.h"
#include "net/http/HttpRequest.h"
#include "net/Buffer.h"

class HttpContext: copyable {
public:
    enum HttpRequestParseState {
        kExpectRequestLine,
        kExpectHeader,
        kExpectBody,
        kGotAll
    };
    HttpContext(): state_(kExpectRequestLine) {}
    bool parseHttpRequest(Buffer* buf);
    void reset()
    {
        state_ = kExpectRequestLine;
        HttpRequest dummy;
        request_.swap(dummy);
    }
    const HttpRequest& getRequest() const
    {
        return request_;
    }
    HttpRequest& getRequest()
    {
        return request_;
    }

    bool gotAll()
    {
        return state_ == kGotAll;
    }
private:
    bool parseRequestLine(const char* start, const char* end);
    HttpRequestParseState state_;
    HttpRequest request_;
};
#endif

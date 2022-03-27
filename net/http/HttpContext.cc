#include "net/http/HttpContext.h"

#include <algorithm>
#include <cctype>

using namespace std;

bool HttpContext::parseHttpRequest(Buffer *buf)
{
    bool hasMore = true;
    bool succeed = false;
    while (hasMore) {
        if (state_ == kExpectRequestLine) {
            const char* crlf = buf->findCRLF();
            if (crlf) {
                if (parseRequestLine(buf->peek(), crlf)) {
                    buf->retrieveUntil(crlf + 2);
                    state_ = kExpectHeader;
                }
                else {
                    hasMore = false;
                }
            }
            else {
                hasMore = false;
            }
        }
        else if(state_ == kExpectHeader) {
            const char* crlf = buf->findCRLF();
            if (crlf) {
                const char* colon = find(buf->peek(), crlf, ':');
                if (colon != crlf) {
                    request_.addHeader(buf->peek(), colon, crlf);
                    buf->retrieveUntil(crlf + 2);
                }
                else {
                    state_ = kExpectBody;
                }
            }
            else {
                succeed = true;
                hasMore = false;
            }

        }
        else if (state_ == kExpectBody) {
            hasMore = false;
            succeed = true;
        }
    }
    return succeed;
}

bool HttpContext::parseRequestLine(const char* start, const char* end)
{
    bool succeed = false;
    const char* space = find(start, end, ' ');
    request_.setMethod(start, space);
    while (space < end && isspace(*space)) {
        space++;
    }
    start = space;
    space = find(start, end, ' ');
    request_.setPath(start, space);
    while (space < end && isspace(*space)) {
        space++;
    }
    string version(space, end - 1);
    if (version == "HTTP/1.") {
        if (*(end - 1) == '1') {
            request_.setVersion(HttpRequest::kHttp11);
            succeed = true;
        }
        else if(*(end - 1) == '0') {
            request_.setVersion(HttpRequest::kHttp10);
            succeed = true;
        }
    }
    return succeed;
}

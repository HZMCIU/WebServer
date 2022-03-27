#include "net/http/HttpResponse.h"
#include <cstdio>

using namespace std;

void HttpResponse::appendToBuffer(Buffer *output)
{
    char buf[256];
    size_t n = snprintf(buf, sizeof(buf), "HTTP/1.1 %d %s\r\n", statusCode_, statusMessage_.c_str());
    output->append(buf, n);

    for(const pair<string, string>& h : headers_) {
        n = snprintf(buf, sizeof(buf), "%s: %s\r\n", h.first.c_str(), h.second.c_str());
        output->append(buf, n);
    }
    output->append("Connection: Keep-Alive\r\n");
    n = snprintf(buf, sizeof(buf), "Content-Length: %zd\r\n", body_.size());
    output->append(buf, n);
    output->append("\r\n");
    output->append(body_);
}

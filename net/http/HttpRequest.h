#ifndef NET_HTTP_HTTPREQUEST_H
#define NET_HTTP_HTTPREQUEST_H
#include "base/copyable.h"
#include <cctype>
#include <map>
#include <utility>

class HttpRequest: copyable {
public:
    enum Method {
        kInvalid, kGet, kPost
    };
    enum Version {
        kUnknown, kHttp10, kHttp11
    };

    HttpRequest()
        : method_(kInvalid)
        , version_(kUnknown)

    {
    }

    void setMethod(const char* start, const char* end)
    {
        std::string method(start, end);
        if (method == "GET") {
            method_ = kGet;
        }
        else if (method == "POST") {
            method_ = kPost;
        }
        else {
            method_ = kInvalid;
        }
    }
    Method getMethod()
    {
        return method_;
    }

    void setPath(const char* start, const char* end)
    {
        path_.assign(start, end);
    }
    const std::string& getPath() const
    {
        return path_;
    }

    void setVersion(Version version)
    {
        version_ = version;
    }
    Version getVersion()
    {
        return version_;
    }

    void addHeader(const char* start, const char* colon, const char* end)
    {
        std::string field(start, colon);
        ++colon;
        while (colon < end && isspace(*colon)) {
            colon++;
        }
        std::string value(colon, end);
        while (!value.empty() && isspace(value[value.size() - 1])) {
            value.resize(value.size() - 1);
        }
        headers_[field] = value;
    }
    std::string getHeader(const std::string& field)
    {
        std::map<std::string, std::string>::iterator it = headers_.find(field);
        if (it != headers_.end()) {
            return it->second;
        }
        return std::string();
    }
    void addQuery(const char* start, const char* end);
    std::string getQuery(std::string& field) 
    {
        return queries_[field];
    }
    const std::map<std::string, std::string>& getQueries() const
    {
        return queries_;
    }
    const std::map<std::string, std::string>& getHeaders() const
    {
        return headers_;
    }

    void swap(HttpRequest& that)
    {
        std::swap(method_, that.method_);
        std::swap(path_, that.path_);
        std::swap(version_, that.version_);
        std::swap(headers_, that.headers_);
    }
private:
    Method method_;
    std::string path_;
    Version version_;
    std::map<std::string, std::string> headers_;
    std::map<std::string, std::string> queries_;
};
#endif

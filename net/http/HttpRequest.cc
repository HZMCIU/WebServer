#include "net/http/HttpRequest.h"

#include <algorithm>

using namespace std;

void HttpRequest::addQuery(const char *start, const char *end)
{
    if (start != nullptr && end != nullptr && start != end) {
        const char* equal = find(start, end, '=');
        string field;
        string value;
        if (equal != end) {
            field.assign(start, equal);
            equal += 1;
            if (equal != end) {
                value.assign(equal, end);
            }
        }
        else {
            field.assign(start, end);
        }
        queries_[field] = value;
    }
}

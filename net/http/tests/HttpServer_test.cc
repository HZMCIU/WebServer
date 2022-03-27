#include "net/EventLoop.h"
#include "net/http/HttpRequest.h"
#include "net/http/HttpResponse.h"
#include "net/http/HttpServer.h"
#include <fstream>
#include <sstream>

using namespace std;

void onRequest(const HttpRequest& req, HttpResponse& resp)
{
    if (req.getPath() == "/") {
        resp.setHeader("Content-Type", "text/html");
        resp.setBody("<h1>Hello HZMCIU</h1>");
    }
    else if (req.getPath() == "/data") {
        const map<string, string>& queries = req.getQueries();

        if (queries.find("image") != queries.end()) {
            resp.setHeader("Content-Type", "image/jpeg");
            ifstream in(queries.at("image"));
            stringstream buffer;
            buffer << in.rdbuf();
            resp.setBody(buffer.str());
        }
        else if (queries.find("text") != queries.end()) {
            resp.setHeader("Content-Type", "text/plain;charset=utf-8");
            ifstream in(queries.at("text"));
            stringstream buffer;
            buffer << in.rdbuf();
            resp.setBody(buffer.str());
        }
    }
}
int main(int argc, char *argv[])
{
    EventLoop loop;
    HttpServer server(&loop, "hxy", 8080);
    server.setThreadNum(3);
    server.setHttpCallback(onRequest);
    server.start();
    loop.loop();
    return 0;
}

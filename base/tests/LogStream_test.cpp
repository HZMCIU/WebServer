#include "base/LogStream.h"
#include <memory>

using namespace std;

int main(int argc, char *argv[])
{
    {
        LogStream stream;
        stream << -1912;
        printf("%s\n", stream.buffer().data());
    }
    {
        LogStream stream;
        stream << -3.1415926f;
        printf("%s\n", stream.buffer().data());
    }
    {
        LogStream stream;
        stream << 'a';
        stream << '\n';
        printf("%s\n", stream.buffer().data());
    }
    {
        LogStream stream;
        stream << "abcdefghi";
        printf("%s\n", stream.buffer().data());
    }
    {
        LogStream stream;
        stream << string("abcdefghi");
        printf("%s\n", stream.buffer().data());
    }

    using Buffer = FixedBuffer<kLargeBuffer>;
    using BufferPtr = shared_ptr<Buffer>;

    BufferPtr newBuffer1(new Buffer);
    printf("%d\n", newBuffer1->avail());
    newBuffer1->append("hello", 11);
    printf("%s\n", newBuffer1->data());
    return 0;
}

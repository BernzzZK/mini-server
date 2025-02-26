#include <time.h>
#include "Timestamp.h"

Timestamp::Timestamp()
: _microSecondSinceEpoch(0)
{
}

Timestamp::Timestamp(int64_t microSecond)
: _microSecondSinceEpoch(microSecond)
{
}

Timestamp Timestamp::now() {
    return Timestamp(time(NULL));
}

string Timestamp::to_string() {
    char buff[128] = {0};
    tm *tm_time = localtime(&_microSecondSinceEpoch);
    snprintf(buff, 128, "%4d/%02d/%02d %02d:%02d:%02d",
             tm_time->tm_year + 1900, // start with 1900
             tm_time->tm_mon + 1,     // start with 0
             tm_time->tm_mday,
             tm_time->tm_hour,
             tm_time->tm_min,
             tm_time->tm_sec
             );
    return buff;
}

// int main() {
//     cout << "current time: " << Timestamp::now().to_string() << endl;
//     return 0;
// }
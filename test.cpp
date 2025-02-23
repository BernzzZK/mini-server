#include "redis.h"

int main(){
    // redisContext *connect = redisConnect("127.0.0.1", 6379);
    // if (connect->err != 0)
    //     return -1;

    // void *result = redisCommand(connect, "set k v");
    //  redisReply *ply = (redisReply *)result;
    //  if(ply->type == 5)
    //      cout << "状态：" << ply->type << endl;
    //  freeReplyObject(ply);

    // result = redisCommand(connect, "get k");
    // ply = (redisReply *)result;
    // if(ply->type == 1) {
    //     cout << "字符串长度：" << ply->len << endl;
    //     cout << "字符串内容：" << ply->str << endl;
    // }
    // freeReplyObject(ply);

    // redisFree(connect);
    redisConnection rc;
    rc.setCommand(1, 1);
    rc.setCommand(2, "v2");
    rc.setCommand("k3", "v3");
    rc.setCommand("k4", 4);
    cout << rc.getIntValue(1) << endl;
    cout << rc.getIntValue("k4") << endl;
    cout << rc.getStringValue(2) << endl;
    cout << rc.getStringValue("k3") << endl;
    vector<char*> r = rc.keys();
    for(auto s : r)
        cout << s << endl;
    return 0;
}
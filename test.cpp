// #include "redis.h"

// int main(){
//     // redisContext *connect = redisConnect("127.0.0.1", 6379);
//     // if (connect->err != 0)
//     //     return -1;

//     // void *result = redisCommand(connect, "set k v");
//     //  redisReply *ply = (redisReply *)result;
//     //  if(ply->type == 5)
//     //      cout << "状态：" << ply->type << endl;
//     //  freeReplyObject(ply);

//     // result = redisCommand(connect, "get k");
//     // ply = (redisReply *)result;
//     // if(ply->type == 1) {
//     //     cout << "字符串长度：" << ply->len << endl;
//     //     cout << "字符串内容：" << ply->str << endl;
//     // }
//     // freeReplyObject(ply);

//     // redisFree(connect);
//     redisConnection rc;
//     rc.setCommand(1, 1);
//     rc.setCommand(2, "v2");
//     rc.setCommand("k3", "v3");
//     rc.setCommand("k4", 4);
//     cout << rc.getIntValue(1) << endl;
//     cout << rc.getIntValue("k4") << endl;
//     cout << rc.getStringValue(2) << endl;
//     cout << rc.getStringValue("k3") << endl;
//     vector<char*> r = rc.keys();
//     for(auto s : r)
//         cout << s << endl;
//     return 0;
// }

// #include "json.h"

// int main() {
//     jsoncpp jc;
    
//     // jc.jsonAppend("k1", 1);
//     // jc.jsonAppend("k2", "v2");
//     // Value v;
//     // v["v.k1"] = "v.v1";
//     // v["v.k2"] = 2;
//     // jc.jsonAppend(v);
//     // jc.saveJson("config.json", 0);

//     jc.readJson("config.json");
//     Value v = jc.getRoot();
//     Value::Members keys = v.getMemberNames();
//     for (int i = 0; i < keys.size(); ++i)
//         cout << keys.at(i) << ": " << v[keys[i]] << ", ";
//     cout << endl;
//     return 0;
// }

#include "ThreadPool.h"
#include <iostream>

// void calc(int x, int y) {
//     int z = x + y;
//     cout << "z: " << z << endl;
//     this_thread::sleep_for(chrono::seconds(2));
// }

int calc1(int x, int y) {
    int z = x + y;
    this_thread::sleep_for(chrono::seconds(2));
    return z;
}

int main() {
    ThreadPool pool;
    vector<future<int>> res;
    for (int i = 0; i < 10; ++i) {
        //auto func = bind(calc, i, i * 2);
        res.push_back(pool.addTask(calc1, i, i * 2));
    }
    for(auto &i:res)
        cout << "result: " << i.get() << endl;
    return 0;
}
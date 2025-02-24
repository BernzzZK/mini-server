#include "json.h"

void jsoncpp::jsonAppend(Value& value) {
    for (const auto &key : value.getMemberNames()) {
        _root[key] = value[key];
    }
}

void jsoncpp::saveJson(string filename, int type) {
    _ofs.open(filename);
    if(_ofs) {
        string json;
        if (type == config) {
            json = _root.toStyledString();

        } else if (type == normal) {
            FastWriter w;
            json = w.write(_root);
        }           
        _ofs << json;
        _ofs.close();
    }
}

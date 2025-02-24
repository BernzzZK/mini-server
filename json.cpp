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
    } else {
        assert(false);
    }
}

void jsoncpp::readJson(string filename) {
    _ifs.open(filename);
    if(_ifs) {
        if(!_root.empty())
            _root.clear();
        _read.parse(_ifs, _root);
        if(_root.empty())
            assert(false);
    }
}

Value jsoncpp::getRoot() {
    return _root;
}

// jsoncpp 1.9.7은 C++14로 컴파일되어 string_view 심볼이 없음.
// 헤더는 C++17+에서 string_view 오버로드만 선언하고 const char* 오버로드를 숨김.
// 라이브러리에 있는 const char* 심볼을 dlsym으로 직접 호출하여 무한 재귀를 피함.
#include <json/json.h>
#include <dlfcn.h>
#include <stdexcept>

namespace Json {

namespace {

using BracketFn      = Value& (*)(Value*, const char*);
using BracketConstFn = const Value& (*)(const Value*, const char*);
using IsMemberFn     = bool (*)(const Value*, const char*);
using GetFn          = Value (*)(const Value*, const char*, const Value*);

// _ZN4Json5ValueixEPKc  = Json::Value::operator[](const char*)
// _ZNK4Json5ValueixEPKc = Json::Value::operator[](const char*) const
// _ZNK4Json5Value8isMemberEPKc = Json::Value::isMember(const char*) const
// _ZNK4Json5Value3getEPKcRKS0_  = Json::Value::get(const char*, const Value&) const

BracketFn      g_bracket      = nullptr;
BracketConstFn g_bracketConst = nullptr;
IsMemberFn     g_isMember     = nullptr;
GetFn          g_get          = nullptr;

struct Init {
    Init() {
        g_bracket      = reinterpret_cast<BracketFn>(dlsym(RTLD_DEFAULT,      "_ZN4Json5ValueixEPKc"));
        g_bracketConst = reinterpret_cast<BracketConstFn>(dlsym(RTLD_DEFAULT, "_ZNK4Json5ValueixEPKc"));
        g_isMember     = reinterpret_cast<IsMemberFn>(dlsym(RTLD_DEFAULT,     "_ZNK4Json5Value8isMemberEPKc"));
        g_get          = reinterpret_cast<GetFn>(dlsym(RTLD_DEFAULT,          "_ZNK4Json5Value3getEPKcRKS0_"));
    }
} g_init;

} // anonymous namespace

Value& Value::operator[](std::string_view key) {
    return g_bracket(this, std::string(key).c_str());
}

const Value& Value::operator[](std::string_view key) const {
    return g_bracketConst(this, std::string(key).c_str());
}

bool Value::isMember(std::string_view key) const {
    return g_isMember(this, std::string(key).c_str());
}

Value Value::get(std::string_view key, const Value& defaultValue) const {
    return g_get(this, std::string(key).c_str(), &defaultValue);
}

} // namespace Json

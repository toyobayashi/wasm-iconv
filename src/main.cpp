#include <cerrno>
#include <cstdio>
#include <cstring>
#include <vector>
#include <string>
#include <iostream>
#include "iconv.h"

#include <emscripten/bind.h>

namespace wasmiconv {

static int on_iconvlist(unsigned int namescount, const char * const * names, void* data) {
  emscripten::val* callback = static_cast<emscripten::val*>(data);
  std::vector<std::string> js_names(names, names + namescount);

  emscripten::val result = callback->operator()(namescount, emscripten::val::array(js_names));
  if (result.isNumber()) return result.as<int>();
  return 0;
}

static void js_iconvlist(emscripten::val do_one) {
  if (!do_one.instanceof(emscripten::val::global("Function"))) {
    emscripten::val::global("TypeError").new_(std::string("The first argument of iconvlist() should be a function")).throw_();
    return;
  }
  iconvlist(on_iconvlist, &do_one);
}

static int js_errno() {
  return errno;
}

static std::string js_iconv_canonicalize(const std::string& name) {
  return iconv_canonicalize(name.c_str());
}

static int js_iconv_open(const std::string& tocode, const std::string& fromcode) {
  return reinterpret_cast<int>(iconv_open(tocode.c_str(), fromcode.c_str()));
}

static emscripten::val js_iconv(int cd, const emscripten::val& inbuf, size_t inbytesleft, size_t outbytesleft) {
  emscripten::val ArrayBuffer = emscripten::val::global("ArrayBuffer");
  emscripten::val Uint8Array = emscripten::val::global("Uint8Array");

  char** p_inbuf;
  std::string s;
  std::vector<char> buf;
  char* str;
  if (inbuf.isNull()) {
    p_inbuf = nullptr;
  } else if (inbuf.isString()) {
    s = inbuf.as<std::string>();
    buf = std::vector<char>(s.c_str(), s.c_str() + s.length());
    str = buf.data();
    p_inbuf = &str;
  } else if (inbuf.instanceof(ArrayBuffer) || ArrayBuffer["isView"](inbuf).as<bool>()) {
    buf = emscripten::convertJSArrayToNumberVector<char>(emscripten::val::global("Uint8Array").new_(inbuf["buffer"]));
    str = buf.data();
    p_inbuf = &str;
  } else {
    emscripten::val::global("TypeError").new_(std::string("Invalid inbuf")).throw_();
    return emscripten::val((size_t)-1);
  }

  size_t* p_inbytesleft;
  if (inbytesleft == 0) {
    p_inbytesleft = nullptr;
  } else {
    p_inbytesleft = &inbytesleft;
  }

  size_t total = outbytesleft;

  size_t* p_outbytesleft;
  char* outbuf;
  char* outbuforigin;
  char** p_outbuf;
  if (total == 0) {
    p_outbytesleft = nullptr;
    outbuf = nullptr;
    outbuforigin = nullptr;
    p_outbuf = nullptr;
  } else {
    p_outbytesleft = &outbytesleft;
    outbuf = new char[outbytesleft];
    memset(outbuf, 0, outbytesleft);
    outbuforigin = outbuf;
    p_outbuf = &outbuf;
  }

  size_t r = (size_t)iconv((iconv_t)cd, p_inbuf, p_inbytesleft, p_outbuf, p_outbytesleft);
  
  emscripten::val obj = emscripten::val::object();

  obj.set("result", emscripten::val(r));
  obj.set("inBytesLeft", p_inbytesleft == nullptr ? emscripten::val::null() : emscripten::val(*p_inbytesleft));
  if ((outbuforigin == nullptr) || (total == outbytesleft)) {
    obj.set("outbuf", emscripten::val::null());
    // obj.set("outString", std::string(""));
  } else {
    const size_t len = total - outbytesleft;
    emscripten::val outbufarr = Uint8Array.new_(len);
    outbufarr.call<void>("set", emscripten::val(emscripten::typed_memory_view(len, outbuforigin)));

    obj.set("outbuf", outbufarr);
    // obj.set("outString", std::string(outbuforigin));
  }
  obj.set("outBytesLeft", p_outbytesleft == nullptr ? emscripten::val::null() : emscripten::val(*p_outbytesleft));

  if (outbuforigin != nullptr) delete[] outbuforigin;
  return obj;
}

static int js_iconv_close(int cd) {
  return iconv_close(reinterpret_cast<iconv_t>(cd));
}

EMSCRIPTEN_BINDINGS(wasmiconv) {
  emscripten::constant("E2BIG", E2BIG);
  emscripten::constant("EILSEQ", EILSEQ);
  emscripten::constant("EINVAL", EINVAL);

  emscripten::function("errno", js_errno);

  emscripten::function("iconv_open", js_iconv_open);
  emscripten::function("iconv", js_iconv);
  emscripten::function("iconv_close", js_iconv_close);
  emscripten::function("iconvlist", js_iconvlist);
  emscripten::function("iconv_canonicalize", js_iconv_canonicalize);
}

}

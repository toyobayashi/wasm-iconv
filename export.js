exports.E2BIG = Module.E2BIG;
exports.EILSEQ = Module.E2BIG;
exports.EINVAL = Module.EINVAL;

Object.defineProperty(exports, 'errno', {
  configurable: true,
  enumerable: true,
  get () { return Module.errno() }
});

exports.iconv_open = Module.iconv_open;
exports.iconv = Module.iconv;
exports.iconv_close = Module.iconv_close;
exports.iconvlist = Module.iconvlist;
exports.iconv_canonicalize = Module.iconv_canonicalize;

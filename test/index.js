const iconv = typeof require === 'function' ? require('../.cgenbuild/iconv.js') : window.iconv
const init = iconv.default

init().then(mod => {
  console.log(mod)
  mod.iconvlist(function (namescount, names) {
    console.log(namescount, names)
  })
  console.log(iconv.errno)
  console.log(mod.iconv_canonicalize('wchar_ts'))
  
  const cd = mod.iconv_open("UTF-8", "UTF-16LE")
  if (cd > 0) {
    console.log(cd)
    const arr = new Uint16Array(1)
    arr[0] = 29275
    const r = mod.iconv(cd, arr, arr.byteLength, arr.byteLength * 5)
    console.log(r)
    console.log(new TextDecoder("utf-8").decode(r.outbuf))
    console.log(iconv.errno)
    mod.iconv_close(cd)
  } else {
    console.log(iconv.errno)
  }
})

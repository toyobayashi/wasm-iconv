module.exports = function (_options, { isDebug }) {

  const debugFlags = [
    '-sDISABLE_EXCEPTION_CATCHING=0',
    '-sSAFE_HEAP=1'
  ]

  const commonFlags = [
    '--bind',
    '-sALLOW_MEMORY_GROWTH=1',
    ...(isDebug ? debugFlags : [])
  ]

  return {
    project: 'iconv',
    targets: [
      {
        name: 'iconv',
        type: 'exe',
        sources: [
          './src/main.cpp'
        ],
        wrapScript: './export.js',
        compileOptions: [
          ...commonFlags
        ],
        linkOptions: [
          ...commonFlags
        ],
        libPaths: ['./deps/libiconv/lib'],
        includePaths: ['./deps/libiconv/include'],
        libs: ['./deps/libiconv/lib/libiconv.a']
      }
    ]
  }
}

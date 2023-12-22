# Zint WASM Bindings

Simple infrastructure and bindings to get the excellent [Zint](https://zint.org.uk/) barcode generation library running in the browser via WebAssembly compiled with [Emscripten](https://emscripten.org/).

Most features should be exposed by the bindings to get most barcode types and configurations to display, but not everything is exposed. The wrapper is farily thin though, so it should be straightforward to see what's going on. Pull requests welcome if you find something necesssary is missing.

# Building
Linux: run build.sh (Full build with dependencies) or build_nodeps.sh (App build only, no building dependencies)
Windows: build.bat or build_no_deps.bat
Result:
3 types of results

1. **wasmzint.js** and **wasmzint.wasm** - separate js and wasm files, can works only on the server.
2. **wasmzint_sf.js** - wasm is embedded into the js file. Works locally or on a server.
3. **wasmzint_sf_es6.js** - wasm is embedded into the es6 module. Convenient to use for the import directive

build_dependencies.sh (called by build.sh) and build_deps.bat (for windows)  handles passing proper arguments to the dependency projects and building dependencies. Dependencies are pulled in via Git submodules in the /deps directory.

# Using
1. Connection via html:
- include script into html file
```html
<script src="./wasmzint_sf.js"></script>
```
- Init wasmzint and use:
  ```javascript
  wasmzint().then(function (Module) {
  const instance = new Module.ZintWrapper()
  const out = instance.encodeAndPrint({symbology: BARCODE_EANX, scale: 1.5,
                height: 50, fgcolour: '006400', outfile: 'out.svg'},
               '123456789012+12345')
  const version = instance.version()
  })
  ```
See the example drawing to a canvas in demo.html.
  

2. Connection via import module
```javascript
   import {wasmzint} from "./wasmzint_sf_es6.js"

    ...
   try {
        zintModule = await wasmzint()
        zintInstance = new zintModule.ZintWrapper()
   }
   catch(e){
   }
```

## Functions
1. encodeAndPrint(params, data)
Call zint function ZBarcode_Encode_and_Print
2. encodeAndVector(params, data)
Call zint function ZBarcode_Encode_and_Buffer_Vector
3. encodeAndBuffer(params, data)
Call zint function ZBarcode_Encode_and_Buffer
4. barcodeName(symbol_id)
Call zint function ZBarcode_BarcodeName
5. cap(symbol_id, flags)
   Call zint function ZBarcode_Cap
6. defaultXdim(symbol_id)
   Call zint function ZBarcode_Default_Xdim
7. scaleFromXdimDp(symbol_id, x_dim_mm, dpmm, filetype)
   Call zint function ZBarcode_Scale_From_XdimDp
8. xdimDpFromScale(symbol_id, scale, x_dim_mm_or_dpmm, filetype)
   Call zint function ZBarcode_XdimDp_From_Scale
9. version()
   Call zint function ZBarcode_Version


...coming soon
 
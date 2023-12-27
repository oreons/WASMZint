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
[Example drawing to a canvas in demo.html](https://github.com/oreons/WASMZint/blob/main/demo.html).

2. Connection via import module
```javascript
   import wasmzint from "./wasmzint_sf_es6.js"

    ...
   try {
        zintModule = await wasmzint()
        zintInstance = new zintModule.ZintWrapper()
   }
   catch(e){
   }
```
[Demo vue app with embedded zint](https://github.com/oreons/ZintWebassemblyDemo/releases/tag/1.0.0)  
[Repository vue demo](https://github.com/oreons/ZintWebassemblyDemo)


## Functions
#### **params** - options for zint_symbol structure, [zint api documentation](https://zint.org.uk/manual/chapter/5)

Also added 2 parameters to the params object. These are the parameters that are passed to the zint function, see zint api ZBarcode_Encode_and_Print
- **length**
- **rotate_angle**

Null by default and should only be passed if needed.

` In these definitions length can be used to set the length of the input string. This allows the encoding of NUL (ASCII 0) characters in those symbologies which allow this. A value of 0 will disable this usage and Zint will encode data up to the first NUL character in the input string, which must be present.
  The rotate_angle value can be used to rotate the image when outputting. Valid values are 0, 90, 180 and 270.
`

**data** - input string

#### Return value:

Object with params: 
1. **result** - true or false 
2. **error** - zint error in text
3. **errorCode** - zint error number
4. file, vector, bitmap, etc -  function result

List: 
1. **_encodeAndPrint_**(params, data)
Call zint function ZBarcode_Encode_and_Print

param in zint structure **outfile**

   In this parameter is important to specify the file extension. The file name is not important.
   For example out.svg. The function will return an svg file in a binary buffer.
   
Valid extensions for zint: **.png, .gif, .bmp, .emf, .eps, .pcx, .svg, .tif**

Return **file** - binary buffer with file

2. **_encodeAndVector_**(params, data)

Call zint function ZBarcode_Encode_and_Buffer_Vector

Return **vector** - object with vector elements

3. **_encodeAndBuffer_**(params, data)

Call zint function ZBarcode_Encode_and_Buffer

Return **bitmap** - bitmap image
   **alphamap** -  array representing alpha channel

4. **_barcodeName_**(symbol_id)
 
Call zint function ZBarcode_BarcodeName

5. **_cap_**(symbol_id, flags)

Call zint function ZBarcode_Cap

6. **_defaultXdim_**(symbol_id)

Call zint function ZBarcode_Default_Xdim

7. **_scaleFromXdimDp_**(symbol_id, x_dim_mm, dpmm, filetype)

Call zint function ZBarcode_Scale_From_XdimDp

8. **_xdimDpFromScale_**(symbol_id, scale, x_dim_mm_or_dpmm, filetype)
 
Call zint function ZBarcode_XdimDp_From_Scale

9. **_version_**()
   Call zint function ZBarcode_Version
 
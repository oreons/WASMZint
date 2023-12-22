#include <emscripten/bind.h>
#include <emscripten.h>
#include <stdio.h>
#include <sys/stat.h>

#include <zint.h>

using namespace emscripten;

#define _countof(array) (sizeof(array) / sizeof(array[0]))


#define JS_C(js_obj, c_obj, name, type) \
if(js_obj.hasOwnProperty(#name)){ \
        c_obj->name = js_obj[#name].as<type>(); \
    }

#define JS_C2(js_obj, c_obj, name, type) \
if(js_obj.hasOwnProperty(#name)){ \
        c_obj.name = js_obj[#name].as<type>(); \
    }



#define JS_C_STR(js_obj, c_obj, name) \
if(js_obj.hasOwnProperty(#name)){ \
        strncpy(c_obj->name, js_obj[#name].as<std::string>().c_str(), _countof(c_obj->name)); \
    }

#define JS_C_STR2(js_obj, c_obj, name) \
if(js_obj.hasOwnProperty(#name)){ \
        strncpy(c_obj.name, js_obj[#name].as<std::string>().c_str(), _countof(c_obj.name)); \
    }


#define PROPS_ZINT(name, type) \
	JS_C(props, barcode_info, name, type)

#define PROPS_ZINT_STR(name) \
	JS_C_STR(props, barcode_info, name)


#define C_JS(c_obj, js_obj, name) \
js_obj.set(#name, c_obj->name);

#define C_JS2(c_obj, js_obj, name) \
js_obj.set(#name, c_obj.name);

#define C_JS_STR(c_obj, js_obj, name) \
js_obj.set(#name, std::string((char *)c_obj->name));

#define C_JS_STR2(c_obj, js_obj, name) \
js_obj.set(#name, std::string((char *) c_obj.name));



std::string readTextFile(const char *name){
    FILE *f = NULL;
    f = fopen(name,"rb");
    if(f==NULL) return "";
    struct stat st;
    fstat (fileno (f), &st);
    char *buf = (char*) malloc(st.st_size+1);
    int len = fread(buf, 1,st.st_size,f);
    if(len!=st.st_size) return "";
    fclose(f);
    std::string sss(buf, st.st_size);
    free(buf);
    return sss;
}


char* readFile(const char *name, int &len){
    FILE *f = NULL;
    len = 0;
    f = fopen(name,"rb");
    if(f==NULL) return NULL;
    struct stat st;
    fstat (fileno (f), &st);
    char *buf = (char*) malloc(st.st_size+1);
    int l = fread(buf, 1,st.st_size,f);
    if(l!=st.st_size) return NULL;
    fclose(f);
    len = st.st_size;
    return buf;
}

char *readFile(const std::string &name, int &len){
    return readFile(name.c_str(), len);
}


EM_JS(void, log_value, (EM_VAL val_handle), {
  let value = Emval.toValue(val_handle);
  console.log(value);
});


class ZintWrapper {
public:
  ZintWrapper(){}

  val encodeAndPrint(val props, std::string data){
    val ret = val::object();
    barcode_info = ZBarcode_Create();
    propsToBarcodeInfo(props);
    std::string file = barcode_info->outfile;
    int res = ZBarcode_Encode_and_Print(barcode_info, reinterpret_cast<const unsigned char*>(data.c_str()), _length, _rotate_angle);
    if(res!=0){
        ret = _retError(res);
    }
    else{
        int len = 0;
        char *buf = readFile(file, len);
        remove(file.c_str());
        val mem(typed_memory_view(len, buf));
        free(buf);
        ret.set("file", mem);
        ret.set("result", true);
    }
    ZBarcode_Delete(barcode_info);
    return ret;
  }

  val encodeAndVector(val props, std::string data){
    val ret = val::object();
    barcode_info = ZBarcode_Create();
    propsToBarcodeInfo(props);
    std::string file = barcode_info->outfile;
    int res = ZBarcode_Encode_and_Buffer_Vector(barcode_info, reinterpret_cast<const unsigned char*>(data.c_str()), _length, _rotate_angle);
    if(res!=0||barcode_info->vector==NULL){
        ret = _retError(res);
    }
    else{
        val vector = val::object();
        zint_vector *zintvect = barcode_info->vector;
        vector.set("width", zintvect->width);
        vector.set("height", zintvect->height);
        vector.set("rectangles", _saveRects(zintvect->rectangles));
        vector.set("hexagons", _saveHexagons(zintvect->hexagons));
        vector.set("strings", _saveStrings(zintvect->strings));
        vector.set("circles", _saveCircles(zintvect->circles));
        ret.set("vector", vector);
        ret.set("result", true);
    }
    ZBarcode_Delete(barcode_info);
    return ret;
  }

  val encodeAndBuffer(val props, std::string data){
    val ret = val::object();
    barcode_info = ZBarcode_Create();
    propsToBarcodeInfo(props);
    std::string file = barcode_info->outfile;
    int res = ZBarcode_Encode_and_Buffer(barcode_info, reinterpret_cast<const unsigned char*>(data.c_str()), _length, _rotate_angle);
    if(res!=0){
        ret = _retError(res);
    }
    else{
        ret.set("bitmap_width", barcode_info->bitmap_width);
        ret.set("bitmap_height", barcode_info->bitmap_height);
        val bitmap = val::null();
        if (barcode_info->bitmap != nullptr) {
          const auto length = barcode_info->bitmap_width * barcode_info->bitmap_height * 3;
          bitmap = val(typed_memory_view(length, barcode_info->bitmap));
        }
        ret.set("bitmap", bitmap);

        val alphamap = val::null();
        if (barcode_info->alphamap != nullptr) {
          const auto length = barcode_info->bitmap_width * barcode_info->bitmap_height;
          alphamap = val(typed_memory_view(length, barcode_info->alphamap));
        }
        ret.set("alphamap", alphamap);
        ret.set("result", true);
    }
    ZBarcode_Delete(barcode_info);
    return ret;
  }

   std::string barcodeName(int symbol_id){
    char name[40];
    int res = ZBarcode_BarcodeName(symbol_id, name);
    if(res!=0) return "";
    return std::string(name);
   }

   unsigned int cap(int symbol_id, unsigned int cap_flag){
    return ZBarcode_Cap(symbol_id, cap_flag);
   }

   float defaultXdim(int symbol_id){
        return ZBarcode_Default_Xdim(symbol_id);
   }

   float scaleFromXdimDp(int symbol_id, float x_dim_mm, float dpmm, std::string filetype){
        return ZBarcode_Scale_From_XdimDp(symbol_id, x_dim_mm, dpmm, filetype.c_str());
   }

   float xdimDpFromScale(int symbol_id, float scale, float x_dim_mm_or_dpmm, std::string filetype){
        return ZBarcode_XdimDp_From_Scale(symbol_id, scale, x_dim_mm_or_dpmm, filetype.c_str());
   }

   int version(){
        return ZBarcode_Version();
   }




private:
  zint_symbol* barcode_info = NULL;
  int _length = 0;
  int _rotate_angle = 0;

  void propsToBarcodeInfo(const val &props){
    ZBarcode_Clear(barcode_info);
    _length = 0;
    _rotate_angle = 0;
    if(props.hasOwnProperty("length")){
        _length = props["length"].as<int>();
    }
    if(props.hasOwnProperty("rotate_angle")){
        _rotate_angle = props["rotate_angle"].as<int>();
    }

    PROPS_ZINT(symbology, int)
    PROPS_ZINT(height, float)
    PROPS_ZINT(scale, float)
    PROPS_ZINT(whitespace_width, int)
    PROPS_ZINT(whitespace_height, int)
    PROPS_ZINT(border_width, int)
    PROPS_ZINT(output_options, int)
    PROPS_ZINT_STR(fgcolour)
    PROPS_ZINT_STR(bgcolour)
    PROPS_ZINT_STR(outfile)
    PROPS_ZINT_STR(primary)
    PROPS_ZINT(option_1, int)
    PROPS_ZINT(option_2, int)
    PROPS_ZINT(option_3, int)
    PROPS_ZINT(show_hrt, bool)
    PROPS_ZINT(fontsize, int)
    PROPS_ZINT(input_mode, int)
    PROPS_ZINT(eci, int)
    PROPS_ZINT(dot_size, float)
    PROPS_ZINT(guard_descent, float)
    PROPS_ZINT(warn_level, int)
    PROPS_ZINT(debug, int)
    if(props.hasOwnProperty("structapp")){
        val structapp = props["structapp"];
        JS_C2(structapp, barcode_info->structapp, count, int);
        JS_C2(structapp, barcode_info->structapp, index, int);
        JS_C_STR2(structapp, barcode_info->structapp, id);
    }
  }

    val _retError(int errorCode){
      val v = val::object();
      v.set("result", false);
      v.set("errorCode", errorCode);
      v.set("error", std::string(barcode_info->errtxt));
      return v;
    }


     val _saveRects(zint_vector_rect *rectangles){
          val result = val::array();
          for (zint_vector_rect *rect = rectangles; rect; rect = rect->next) {
              val v = val::object();
              C_JS(rect, v, x)
              C_JS(rect, v, y)
              C_JS(rect, v, height)
              C_JS(rect, v, width)
              C_JS(rect, v, colour)
              result.call<emscripten::val>("push", v);
          }
          return result;
     }

     val _saveHexagons(zint_vector_hexagon *hexagons){
          val result = val::array();
          for (zint_vector_hexagon *hexagon = hexagons; hexagon; hexagon = hexagon->next) {
              val v = val::object();
              C_JS(hexagon, v, x)
              C_JS(hexagon, v, y)
              C_JS(hexagon, v, diameter)
              C_JS(hexagon, v, rotation)
              result.call<emscripten::val>("push", v);
          }
          return result;
     }

     val _saveStrings(zint_vector_string *strings){
          val result = val::array();
          for (zint_vector_string *string = strings; string; string = string->next) {
              val v = val::object();
              C_JS(string, v, x)
              C_JS(string, v, y)
              C_JS(string, v, fsize)
              C_JS(string, v, rotation)
              C_JS(string, v, halign)
              C_JS(string, v, length)
              C_JS_STR(string, v, text)
              result.call<emscripten::val>("push", v);
          }
          return result;
     }

     val _saveCircles(zint_vector_circle *circles){
          val result = val::array();
          for (zint_vector_circle *circle = circles; circle; circle = circle->next) {
              val v = val::object();
              C_JS(circle, v, x)
              C_JS(circle, v, y)
              C_JS(circle, v, diameter)
              C_JS(circle, v, width)
              C_JS(circle, v, colour)
              result.call<emscripten::val>("push", v);
          }
          return result;
     }


};

// Binding code
EMSCRIPTEN_BINDINGS(my_class_example) {
  class_<ZintWrapper>("ZintWrapper")
    .constructor<>()
    .function("encodeAndPrint",  &ZintWrapper::encodeAndPrint)
    .function("encodeAndVector", &ZintWrapper::encodeAndVector)
    .function("encodeAndBuffer", &ZintWrapper::encodeAndBuffer)
    .function("barcodeName",     &ZintWrapper::barcodeName)
    .function("cap",             &ZintWrapper::cap)
    .function("defaultXdim",     &ZintWrapper::defaultXdim)
    .function("scaleFromXdimDp", &ZintWrapper::scaleFromXdimDp)
    .function("xdimDpFromScale", &ZintWrapper::xdimDpFromScale)
    .function("version",         &ZintWrapper::version)
    ;
    register_map<std::string, std::string>("map<string, string>");
}



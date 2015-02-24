/* Javascript bindings. */
#include <v8.h>
#include <bwe.h>

using namespace v8;

class Window;

class Screen {
  private:
    void *screen;
  public:
    void add_window(Window *w) {
      this->window_count++;
      printf("WDINWO ADDED%p\n", w);
      // Call relevant C function
    }

    // Javascript interface
    static void constructor(const FunctionCallbackInfo<Value>& args) {
      HandleScope scope(args.GetIsolate());
      Handle<Value> external;
      if (args[0]->IsExternal()) {
        external = Handle<Value>::Cast(args[0]);
      } else {
        Screen *s = new Screen();
        external = External::New(args.GetIsolate(), s);
        args.This()->SetInternalField(0, external);
      }
    }

    int window_count;
    static void set_window_count(Local<String> property, Local<Value> value,
                                 const PropertyCallbackInfo<void>& info) {
      return;
    }
    static void get_window_count(Local<String> property,
                                 const PropertyCallbackInfo<Value>& info) {
      Local<Object> self = info.Holder();
      Local<External> wrap = Local<External>::Cast(self->GetInternalField(0));
      void *ptr = wrap->Value();
      int value = static_cast<Screen*>(ptr)->window_count;
      info.GetReturnValue().Set(value);
    }

    static void add_window(const FunctionCallbackInfo<Value>&args) {
      // Get the Screen object
      Local<Object> self = args.Holder();
      Local<External> screen_wrap = Local<External>::Cast(self->GetInternalField(0));
      void *screen_ptr = screen_wrap->Value();
      Screen *s = static_cast<Screen*>(screen_ptr);

      // Get the Window argument
      Local<External> window_wrap = Local<External>::Cast(args[0]->ToObject()->GetInternalField(0));
      void *window_ptr = window_wrap->Value();
      Window *w = static_cast<Window*>(window_ptr);
      s->add_window(w);
    }
};

class Window {
  private:
  public:
    void *window;

    void focus(void) {
      // Call relevant C function.
    }

    // Javascript interface
    static void constructor(const FunctionCallbackInfo<Value>& args) {
      HandleScope scope(args.GetIsolate());
      Handle<Value> external;
      if (args[0]->IsExternal()) {
        external = Handle<Value>::Cast(args[0]);
      } else {
        Screen *s = new Screen();
        external = External::New(args.GetIsolate(), s);
        args.This()->SetInternalField(0, external);
      }
    }

    static void focus(const FunctionCallbackInfo<Value>& args) {
      // Get the Window object
      Local<Object> self = args.Holder();
      Local<External> window_wrap = Local<External>::Cast(self->GetInternalField(0));
      void *window_ptr = window_wrap->Value();
      Window *w = static_cast<Window*>(window_ptr);
      w->focus();
    }
};

Handle<String> read_file(Isolate* isolate, const char*name) {
  FILE* file = fopen(name, "rb");
  if (file == NULL) return Handle<String>();

  fseek(file, 0, SEEK_END);
  int size = ftell(file);
  rewind(file);

  char* chars = new char[size + 1];
  chars[size] = '\0';
  for (int i = 0; i < size;) {
    int read = static_cast<int>(fread(&chars[i], 1, size - i, file));
    i += read;
  }
  fclose(file);
  Handle<String> result =
      String::NewFromUtf8(isolate, chars, String::kNormalString, size);
  delete[] chars;
  return result;
}

extern "C" int initialize_javascript() {
  V8::InitializeICU();
  V8::Initialize();

  // Create a new Isolate and make it the current one.
  Isolate* isolate = Isolate::New();
  {
    Isolate::Scope isolate_scope(isolate);

    // Create a stack-allocated handle scope.
    HandleScope handle_scope(isolate);

    // Inject some cool functions.
    Local<ObjectTemplate> global_template = ObjectTemplate::New(isolate);

    // Set up the Screen type
    Handle<FunctionTemplate> screen_template = FunctionTemplate::New(isolate, Screen::constructor);
    Handle<ObjectTemplate> screen_instance_template = screen_template->InstanceTemplate();
    screen_instance_template->SetInternalFieldCount(1);
    screen_instance_template->SetAccessor(String::NewFromUtf8(isolate, "windowCount"), Screen::get_window_count, Screen::set_window_count);
    screen_instance_template->Set(String::NewFromUtf8(isolate, "addWindow"), FunctionTemplate::New(isolate, Screen::add_window));
    global_template->Set(String::NewFromUtf8(isolate, "Screen"), screen_template);

    // Set up the Window type
    Handle<FunctionTemplate> window_template = FunctionTemplate::New(isolate, Screen::constructor);
    Handle<ObjectTemplate> window_instance_template = window_template->InstanceTemplate();
    window_instance_template->SetInternalFieldCount(1);
    global_template->Set(String::NewFromUtf8(isolate, "Window"), window_template);

    // Create a new context.
    Local<Context> context = Context::New(isolate, NULL, global_template);

    // Enter the context for compiling and running the hello world script.
    Context::Scope context_scope(context);

    // Create a string containing the JavaScript source code.
    Handle<String> source = read_file(isolate, "wm.js");

    // Compile the source code.
    Local<Script> script = Script::Compile(source);
    Local<Value> result = script->Run();
    // We don't actually care about the result much.
    (void)result;

    // Set up objects.
    Handle<Object> global = context->Global();

    // Call initialization function.
    Handle<Value> value = global->Get(String::NewFromUtf8(isolate, "initWM"));
    if (value->IsFunction()) {
      Handle<Function> func = Handle<Function>::Cast(value);
      Handle<Value> args[2];
      args[0] = String::NewFromUtf8(isolate, "test");
      args[1] = String::NewFromUtf8(isolate, "ing");
      
      Handle<Value> js_result = func->Call(global, 2, args);
      String::Utf8Value utf8(js_result);
      printf("res: %s\n", *utf8);
    }

    // Run the script to get the result.

    // Convert the result to an UTF8 string and print it.
    //String::Utf8Value utf8(result);
    //printf("%s\n", *utf8);
  }

  // Dispose the isolate and tear down V8.
  isolate->Dispose();
  V8::Dispose();
  return 0;
  
}

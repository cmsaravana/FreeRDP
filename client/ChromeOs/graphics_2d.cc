// Copyright (c) 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <stdio.h>
#include <stdlib.h>

#include "ppapi/c/ppb_image_data.h"
#include "ppapi/cpp/graphics_2d.h"
#include "ppapi/cpp/image_data.h"
#include "ppapi/cpp/input_event.h"
#include "ppapi/cpp/instance.h"
#include "ppapi/cpp/module.h"
#include "ppapi/cpp/point.h"
#include "ppapi/utility/completion_callback_factory.h"

#ifdef WIN32
#undef PostMessage
// Allow 'this' in initializer list
#pragma warning(disable : 4355)
#endif

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <errno.h>
#include <stdio.h>
#include <string.h>

#include <freerdp/freerdp.h>
#include <freerdp/constants.h>
#include <freerdp/gdi/gdi.h>
#include <freerdp/client/file.h>
#include <freerdp/client/cmdline.h>
#include <freerdp/client/cliprdr.h>
#include <freerdp/channels/channels.h>

#include <winpr/crt.h>
#include <winpr/synch.h>
#include <freerdp/log.h>

#include "nacl_io/nacl_io.h"
#include <sys/mount.h>
#include <freerdp/codec/color.h>

#define TAG CLIENT_TAG("sample")

struct tf_context
{
	rdpContext _p;
};
typedef struct tf_context tfContext;

static BOOL tf_context_new(freerdp* instance, rdpContext* context)
{
	return TRUE;
}

static void tf_context_free(freerdp* instance, rdpContext* context)
{
}

static BOOL tf_begin_paint(rdpContext* context)
{
	rdpGdi* gdi = context->gdi;
	gdi->primary->hdc->hwnd->invalid->null = TRUE;
	return TRUE;
}

static BOOL tf_end_paint(rdpContext* context)
{
	rdpGdi* gdi = context->gdi;

	if (gdi->primary->hdc->hwnd->invalid->null)
		return TRUE;

	return TRUE;
}

static BOOL tf_pre_connect(freerdp* instance)
{
	rdpSettings* settings;
	settings = instance->settings;
	settings->OrderSupport[NEG_DSTBLT_INDEX] = TRUE;
	settings->OrderSupport[NEG_PATBLT_INDEX] = TRUE;
	settings->OrderSupport[NEG_SCRBLT_INDEX] = TRUE;
	settings->OrderSupport[NEG_OPAQUE_RECT_INDEX] = TRUE;
	settings->OrderSupport[NEG_DRAWNINEGRID_INDEX] = TRUE;
	settings->OrderSupport[NEG_MULTIDSTBLT_INDEX] = TRUE;
	settings->OrderSupport[NEG_MULTIPATBLT_INDEX] = TRUE;
	settings->OrderSupport[NEG_MULTISCRBLT_INDEX] = TRUE;
	settings->OrderSupport[NEG_MULTIOPAQUERECT_INDEX] = TRUE;
	settings->OrderSupport[NEG_MULTI_DRAWNINEGRID_INDEX] = TRUE;
	settings->OrderSupport[NEG_LINETO_INDEX] = TRUE;
	settings->OrderSupport[NEG_POLYLINE_INDEX] = TRUE;
	settings->OrderSupport[NEG_MEMBLT_INDEX] = TRUE;
	settings->OrderSupport[NEG_MEM3BLT_INDEX] = TRUE;
	settings->OrderSupport[NEG_SAVEBITMAP_INDEX] = TRUE;
	settings->OrderSupport[NEG_GLYPH_INDEX_INDEX] = TRUE;
	settings->OrderSupport[NEG_FAST_INDEX_INDEX] = TRUE;
	settings->OrderSupport[NEG_FAST_GLYPH_INDEX] = TRUE;
	settings->OrderSupport[NEG_POLYGON_SC_INDEX] = TRUE;
	settings->OrderSupport[NEG_POLYGON_CB_INDEX] = TRUE;
	settings->OrderSupport[NEG_ELLIPSE_SC_INDEX] = TRUE;
	settings->OrderSupport[NEG_ELLIPSE_CB_INDEX] = TRUE;
	return TRUE;
}

static BOOL tf_post_connect(freerdp* instance)
{
	if (!gdi_init(instance, PIXEL_FORMAT_BGRA32))
		return FALSE;

	instance->update->BeginPaint = tf_begin_paint;
	instance->update->EndPaint = tf_end_paint;
	return TRUE;
}

static void* tf_client_thread_proc(freerdp* instance)
{
	DWORD nCount;
	DWORD status;
	HANDLE handles[64];

	if (!freerdp_connect(instance))
	{
		WLog_ERR(TAG, "connection failure");
		return NULL;
	}

	while (!freerdp_shall_disconnect(instance))
	{
		nCount = freerdp_get_event_handles(instance->context, &handles[0], 64);

		if (nCount == 0)
		{
			WLog_ERR(TAG, "%s: freerdp_get_event_handles failed", __FUNCTION__);
			break;
		}

		status = WaitForMultipleObjects(nCount, handles, FALSE, 100);

		if (status == WAIT_FAILED)
		{
			WLog_ERR(TAG, "%s: WaitForMultipleObjects failed with %"PRIu32"", __FUNCTION__,
			         status);
			break;
		}

		if (!freerdp_check_event_handles(instance->context))
		{
			WLog_ERR(TAG, "Failed to check FreeRDP event handles");
			break;
		}
	}

	freerdp_disconnect(instance);
	ExitThread(0);
	return NULL;
}

        // Make this global to be accessible for graphics too
        freerdp* freerdp_instance;
#define CRDP_VERSION "V1.004"
// int main(int argc, char* argv[])
int freerdp_main_entry(int dWidth, int dHeight)
{
        int pargc = 6;
        char rusr[100] = "/u:Administrator";
	char rpwd[100] = "/p:wipro@123";
        char rWidth[100] = "";
        char rHeight[100] = "";
        char *pargv[6] = {"freerdp", "/v:10.201.47.37", rusr, rpwd, "", ""};

       	sprintf(rWidth, "/w:%d", dWidth);
        sprintf(rHeight, "/h:%d", dHeight);
        pargv[4] = rWidth;
        pargv[5] = rHeight;
        printf("%s():%d  Entering version:%d \n", __FUNCTION__, __LINE__, CRDP_VERSION);
        printf("%s():%d  using hardcoded username and password%d %s %s\n", __FUNCTION__, __LINE__, CRDP_VERSION, pargv[4], pargv[5]);

        int status;
        HANDLE thread;
        freerdp_instance = freerdp_new();

        if (!freerdp_instance)
        {
                WLog_ERR(TAG, "Couldn't create instance");
                printf("Couldn't create instance\n");
                exit(1);
        }
#if 1
        freerdp_instance->PreConnect = tf_pre_connect;
        freerdp_instance->PostConnect = tf_post_connect;
        freerdp_instance->ContextSize = sizeof(tfContext);
        freerdp_instance->ContextNew = tf_context_new;
        freerdp_instance->ContextFree = tf_context_free;
	freerdp_instance->VerifyCertificate = client_cli_verify_certificate;

        if (!freerdp_context_new(freerdp_instance))
        {
                WLog_ERR(TAG, "Couldn't create context");
                exit(1);
        }

        status = freerdp_client_settings_parse_command_line(freerdp_instance->settings, pargc,
                 pargv, FALSE);

        if (status < 0)
        {
                printf("%s %d freerdp_client_settings_parse_command_line Error \n", __FUNCTION__, __LINE__);
                exit(0);
        }

        if (!freerdp_client_load_addins(freerdp_instance->context->channels,
                                        freerdp_instance->settings))
        {
                printf("%s %d freerdp_client_load_addins error exempted \n", __FUNCTION__, __LINE__);
                // exit(-1);
        }

        if (!(thread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)
                                    tf_client_thread_proc, freerdp_instance, 0, NULL)))
        {
                WLog_ERR(TAG, "Failed to create client thread");
        }
        else
        {
                printf("%s %d WaitForSingleObject \n", __FUNCTION__, __LINE__);
                WaitForSingleObject(thread, INFINITE);
        }

        printf("%s %d before freerdp_context_free\n", __FUNCTION__, __LINE__);
        freerdp_context_free(freerdp_instance);
#endif
        freerdp_free(freerdp_instance);
        printf("Instance allocated and freed\n");
        printf("Exiting Main\n");
	return 0;
}

    // Update the Image buffer
    void freerdp_update_ImageData(uint32_t *data, int width, int height) {
         //  printf("Calling freerdp_update");
        BYTE *pixels =  (BYTE *)data;
	DWORD DstFormat  =  PIXEL_FORMAT_BGRA32;// PIXEL_FORMAT_RGBX32; //PIXEL_FORMAT_RGBA32;
	UINT32 nDstStep = 0;
        UINT32 x = 0;
        UINT32 y  = 0;
	rdpGdi* gdi = NULL;
        bool rc = false;

        // printf("%s %d invoking freerdp_image_copy\n", __FUNCTION__, __LINE__);
        freerdp* inst = (freerdp*)freerdp_instance;
        if (NULL != inst) {
           gdi = inst->context->gdi;
           if (NULL  != gdi) {
                // printf("%s %d invoking freerdp_image_copy\n", __FUNCTION__, __LINE__);
                rc = freerdp_image_copy(pixels, DstFormat, nDstStep, x, y, width, height,
                                        gdi->primary_buffer, gdi->dstFormat, gdi->stride, x, y,
                                        &gdi->palette, FREERDP_FLIP_NONE);
           }
        }

    }

/*------------------------------------------ */

//P lease add  tdp=>Setting conversion here


/*************************************/
namespace {

static const int kMouseRadius = 20;

uint8_t RandUint8(uint8_t min, uint8_t max) {
  uint64_t r = rand();
  uint8_t result = static_cast<uint8_t>(r * (max - min + 1) / RAND_MAX) + min;
  return result;
}

uint32_t MakeColor(uint8_t r, uint8_t g, uint8_t b) {
  uint8_t a = 255;
  PP_ImageDataFormat format = pp::ImageData::GetNativeImageDataFormat();
  if (format == PP_IMAGEDATAFORMAT_BGRA_PREMUL) {
    return (a << 24) | (r << 16) | (g << 8) | b;
  } else {
    return (a << 24) | (b << 16) | (g << 8) | r;
  }
}

}  // namespace

class Graphics2DInstance : public pp::Instance {
 public:
  explicit Graphics2DInstance(PP_Instance instance)
      : pp::Instance(instance),
        callback_factory_(this),
        mouse_down_(false),
        buffer_(NULL),
        InitScreenSet(false),
        device_scale_(1.0f) {}

  ~Graphics2DInstance() { delete[] buffer_; }

  virtual bool Init(uint32_t argc, const char* argn[], const char* argv[]) {

    /* Nacl IO has dependency on mount-menfs */
    printf("%s %d nacl_io_init_ppapi & mount-memfs in place \n", __FUNCTION__, __LINE__);
    nacl_io_init_ppapi(pp_instance(), pp::Module::Get()->get_browser_interface());
    umount("/");
    mount("", "/", "memfs", 0, "");

    RequestInputEvents(PP_INPUTEVENT_CLASS_MOUSE);
    RequestFilteringInputEvents(PP_INPUTEVENT_CLASS_KEYBOARD);

    unsigned int seed = 1;
    srand(seed);
    CreatePalette();



    return true;
  }

   void LoadLibrary() {

#if 1
            printf("s %d Invoking freerdp_main_entry x:%d y:%d\n", __FUNCTION__, __LINE__,  size_.width(), size_.height());
            freerdp_main_entry(size_.width(),  size_.height());
#endif
   }

  static void* LoadLibrariesOnWorker(void* pInst) {
    Graphics2DInstance* inst = static_cast<Graphics2DInstance*>(pInst);
    inst->LoadLibrary();
    return NULL;
  }

  virtual void DidChangeView(const pp::View& view) {
    device_scale_ = view.GetDeviceScale();
    pp::Size new_size = pp::Size(view.GetRect().width() * device_scale_,
                                 view.GetRect().height() * device_scale_);

    if (!CreateContext(new_size))
      return;

    // When flush_context_ is null, it means there is no Flush callback in
    // flight. This may have happened if the context was not created
    // successfully, or if this is the first call to DidChangeView (when the
    // module first starts). In either case, start the main loop.
    if (flush_context_.is_null()) {
       pthread_t thread;
       int rtn = pthread_create(&thread, NULL, LoadLibrariesOnWorker, this);
      MainLoop(0);
    }
  }

  virtual bool HandleInputEvent(const pp::InputEvent& event) {
    UINT16 freerdp_mouse_flag = 0;
    if (!buffer_)
      return true;
    switch (event.GetType()) {
    	case PP_INPUTEVENT_TYPE_MOUSEDOWN:
        case PP_INPUTEVENT_TYPE_MOUSEMOVE:
        case PP_INPUTEVENT_TYPE_MOUSEUP: {
      		pp::MouseInputEvent mouse_event(event);
		      mouse_ = pp::Point(mouse_event.GetPosition().x() * device_scale_,
                         mouse_event.GetPosition().y() * device_scale_);

	      switch (mouse_event.GetButton()) {
		case PP_INPUTEVENT_MOUSEBUTTON_NONE:
		freerdp_mouse_flag = PTR_FLAGS_MOVE;
		break;
		case PP_INPUTEVENT_MOUSEBUTTON_LEFT:
		freerdp_mouse_flag = PTR_FLAGS_BUTTON1;
		break;
		case PP_INPUTEVENT_MOUSEBUTTON_RIGHT:
		freerdp_mouse_flag = PTR_FLAGS_BUTTON2;
		break;
		default:
		break;
		}
	        if (event.GetType() == PP_INPUTEVENT_TYPE_MOUSEDOWN) {
       	         mouse_down_ = true; 
       		 } else if (event.GetType() == PP_INPUTEVENT_TYPE_MOUSEUP) {
               	 mouse_down_ = false;
        	}
     		if (NULL != freerdp_instance) {
         	//printf("Invoking freerdp_instance->input->MouseEvent with corresponding flags\n");
	             if (mouse_down_) { 
       	      		freerdp_mouse_flag |= PTR_FLAGS_DOWN;
             		}
		     freerdp_instance->input->MouseEvent(freerdp_instance->input, freerdp_mouse_flag,  mouse_.x(), mouse_.y());
     		}
              }
        	break;

      case PP_INPUTEVENT_TYPE_RAWKEYDOWN:
      case PP_INPUTEVENT_TYPE_KEYDOWN:
      case PP_INPUTEVENT_TYPE_KEYUP: {
      // case PP_INPUTEVENT_TYPE_CHAR: {
        pp::KeyboardInputEvent key_event(event);
        DWORD scancode; 
        scancode = GetVirtualScanCodeFromVirtualKeyCode(key_event.GetKeyCode(), 4);
        int flags = (PP_INPUTEVENT_TYPE_KEYUP == event.GetType()) ? KBD_FLAGS_RELEASE : KBD_FLAGS_DOWN;
        flags |= (scancode & KBDEXT) ? KBD_FLAGS_EXTENDED : 0;

     	if (NULL != freerdp_instance) {
       		printf("Invoking freerdp_instance->input->KeyboardEvent with corresponding flags\n");
		     freerdp_instance->input->KeyboardEvent(freerdp_instance->input, flags, scancode);
     	}
#if 0
        // std::ostringstream stream;
        stream << "Key event:"
               << " modifier:" << ModifierToString(key_event.GetModifiers())
               << " key_code:" << key_event.GetKeyCode()
               << " time:" << key_event.GetTimeStamp()
               << " text:" << key_event.GetCharacterText().DebugString();
        // PostMessage(stream.str());
#endif
        }
        break;
    }

    return true;
  }

 private:
  bool InitScreenSet;
  void CreatePalette() {
    for (int i = 0; i < 64; ++i) {
      // Black -> Red
      palette_[i] = MakeColor(i * 2, 0, 0);
      palette_[i + 64] = MakeColor(128 + i * 2, 0, 0);
      // Red -> Yellow
      palette_[i + 128] = MakeColor(255, i * 4, 0);
      // Yellow -> White
      palette_[i + 192] = MakeColor(255, 255, i * 4);
    }
  }

  bool CreateContext(const pp::Size& new_size) {
    const bool kIsAlwaysOpaque = true;
    context_ = pp::Graphics2D(this, new_size, kIsAlwaysOpaque);
    // Call SetScale before BindGraphics so the image is scaled correctly on
    // HiDPI displays.
    context_.SetScale(1.0f / device_scale_);
    if (!BindGraphics(context_)) {
      fprintf(stderr, "Unable to bind 2d context!\n");
      context_ = pp::Graphics2D();
      return false;
    }

    // Allocate a buffer of palette entries of the same size as the new context.
    buffer_ = new uint8_t[new_size.width() * new_size.height()];
    size_ = new_size;

    return true;
  }

  void Update() {
    // Old-school fire technique cribbed from
    // http://ionicsolutions.net/2011/12/30/demo-fire-effect/
    UpdateCoals();
    DrawMouse();
    UpdateFlames();
  }

  void UpdateCoals() {
    int width = size_.width();
    int height = size_.height();
    size_t span = 0;

    // Draw two rows of random values at the bottom.
    for (int y = height - 2; y < height; ++y) {
      size_t offset = y * width;
      for (int x = 0; x < width; ++x) {
        // On a random chance, draw some longer strips of brighter colors.
        if (span || RandUint8(1, 4) == 1) {
          if (!span)
            span = RandUint8(10, 20);
          buffer_[offset + x] = RandUint8(128, 255);
          span--;
        } else {
          buffer_[offset + x] = RandUint8(32, 96);
        }
      }
    }
  }

  void UpdateFlames() {
    int width = size_.width();
    int height = size_.height();
    for (int y = 1; y < height - 1; ++y) {
      size_t offset = y * width;
      for (int x = 1; x < width - 1; ++x) {
        int sum = 0;
        sum += buffer_[offset - width + x - 1];
        sum += buffer_[offset - width + x + 1];
        sum += buffer_[offset + x - 1];
        sum += buffer_[offset + x + 1];
        sum += buffer_[offset + width + x - 1];
        sum += buffer_[offset + width + x];
        sum += buffer_[offset + width + x + 1];
        buffer_[offset - width + x] = sum / 7;
      }
    }
  }

  void DrawMouse() {
 
    if (!mouse_down_)
      return;

    int width = size_.width();
    int height = size_.height();

    // Draw a circle at the mouse position.
    int radius = kMouseRadius * device_scale_;
    int cx = mouse_.x();
    int cy = mouse_.y();
    int minx = cx - radius <= 0 ? 1 : cx - radius;
    int maxx = cx + radius >= width ? width - 1 : cx + radius;
    int miny = cy - radius <= 0 ? 1 : cy - radius;
    int maxy = cy + radius >= height ? height - 1 : cy + radius;
    for (int y = miny; y < maxy; ++y) {
      for (int x = minx; x < maxx; ++x) {
        if ((x - cx) * (x - cx) + (y - cy) * (y - cy) < radius * radius)
          buffer_[y * width + x] = RandUint8(192, 255);
      }
    }
  }

  void Paint() {
    // See the comment above the call to ReplaceContents below.
    PP_ImageDataFormat format = pp::ImageData::GetNativeImageDataFormat();
    const bool kDontInitToZero = false;
    pp::ImageData image_data(this, format, size_, kDontInitToZero);

    uint32_t* data = static_cast<uint32_t*>(image_data.data());
    if (!data)
      return;

#if 0
    uint32_t num_pixels = size_.width() * size_.height();
    size_t offset = 0;
    for (uint32_t i = 0; i < num_pixels; ++i) {
      data[offset] = palette_[buffer_[offset]];
      offset++;
    
#endif 

    freerdp_update_ImageData(data,  size_.width(),  size_.height());

    // Using Graphics2D::ReplaceContents is the fastest way to update the
    // entire canvas every frame. According to the documentation:
    //
    //   Normally, calling PaintImageData() requires that the browser copy
    //   the pixels out of the image and into the graphics context's backing
    //   store. This function replaces the graphics context's backing store
    //   with the given image, avoiding the copy.
    //
    //   In the case of an animation, you will want to allocate a new image for
    //   the next frame. It is best if you wait until the flush callback has
    //   executed before allocating this bitmap. This gives the browser the
    //   option of caching the previous backing store and handing it back to
    //   you (assuming the sizes match). In the optimal case, this means no
    //   bitmaps are allocated during the animation, and the backing store and
    //   "front buffer" (which the module is painting into) are just being
    //   swapped back and forth.
    //
    context_.ReplaceContents(&image_data);
  }

  void MainLoop(int32_t) {
    if (context_.is_null()) {
      // The current Graphics2D context is null, so updating and rendering is
      // pointless. Set flush_context_ to null as well, so if we get another
      // DidChangeView call, the main loop is started again.
      flush_context_ = context_;
      return;
    }

    // Update();
    Paint();
    // Store a reference to the context that is being flushed; this ensures
    // the callback is called, even if context_ changes before the flush
    // completes.
    flush_context_ = context_;
    context_.Flush(
        callback_factory_.NewCallback(&Graphics2DInstance::MainLoop));
  }

  pp::CompletionCallbackFactory<Graphics2DInstance> callback_factory_;
  pp::Graphics2D context_;
  pp::Graphics2D flush_context_;
  pp::Size size_;
  pp::Point mouse_;
  bool mouse_down_;
  uint8_t* buffer_;
  uint32_t palette_[256];
  float device_scale_;
};

class Graphics2DModule : public pp::Module {
 public:
  Graphics2DModule() : pp::Module() {}
  virtual ~Graphics2DModule() {}

  virtual pp::Instance* CreateInstance(PP_Instance instance) {
    return new Graphics2DInstance(instance);
  }
};

namespace pp {
Module* CreateModule() { return new Graphics2DModule(); }
}  // namespace pp

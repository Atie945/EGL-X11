#include <X11/Xlib.h>
#include <EGL/egl.h>
#include <EGL/eglext.h>
#include <GL/gl.h>

int main(){

  EGLint const* attrib_list = NULL;
  EGLint major, minor, count, n, size;

  Display* x11_display = XOpenDisplay(NULL);
  if(x11_display == NULL){
      return -1;
  }

  int x11_screen = DefaultScreen(x11_display);

  Window x11_window = XCreateSimpleWindow(
      x11_display,
      RootWindow(x11_display, x11_screen),
      10, 10, 960, 480,
      1,
      BlackPixel(x11_display, x11_screen),  
      WhitePixel(x11_display, x11_screen)
  );


  XSelectInput(x11_display, x11_window, ExposureMask | KeyPressMask);
  XMapWindow(x11_display, x11_window);

  EGLDisplay* eglDisplay = eglGetDisplay(x11_display);
  if (eglDisplay == EGL_NO_DISPLAY) {
      return -1;
  }

  if (!eglInitialize(eglDisplay, &major, &minor)) {
      return -1;
  }

  eglBindAPI(EGL_OPENGL_API);
  static EGLint attribute_list[] = {
      EGL_RED_SIZE,       8,
      EGL_GREEN_SIZE,     8,
      EGL_BLUE_SIZE,      8,
      EGL_ALPHA_SIZE,     8,
      EGL_NONE
  };

  EGLConfig eglConfig;
  if (!eglChooseConfig(eglDisplay, attribute_list, &eglConfig, 1, &count)) {
      return -1;
  }


  if (0 == count || 0 == eglConfig) {
      static EGLint attribute_list2[] = {
          EGL_RED_SIZE,       8,
          EGL_GREEN_SIZE,     8,
          EGL_BLUE_SIZE,      8,
          EGL_ALPHA_SIZE,     8,
          EGL_STENCIL_SIZE,   8,
          EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
          EGL_DEPTH_SIZE, 16,
          EGL_NONE
      };

      if (!eglChooseConfig(eglDisplay, attribute_list2, &eglConfig, 1, &count)) {
          return -1;
      }
      if (0 == count || 0 == eglConfig) {
          return -1;
      }
  }

  EGLint contextAttribs[] =
  {
      EGL_CONTEXT_CLIENT_VERSION, 2,
      EGL_NONE
  };

  EGLContext* eglContext = eglCreateContext(eglDisplay, eglConfig, EGL_NO_CONTEXT, contextAttribs);
  if (eglContext == EGL_NO_CONTEXT) {
      return -1;
  }

  EGLSurface* eglSurface = eglCreateWindowSurface(eglDisplay, eglConfig, x11_window, NULL);

  if(eglSurface == EGL_NO_SURFACE) {
      int err = eglGetError();
      return -1;
  }

  EGLBoolean bret = eglMakeCurrent(eglDisplay, eglSurface, eglSurface, eglContext);
  if (EGL_TRUE != bret) {
      return -1;
  }

  while(1)
  {
        XEvent e;
        XNextEvent(x11_display, &e);

        if(e.type == Expose)
        {
            glClearColor(255, 0.0, 0.0, 0.0);
            glClear(GL_COLOR_BUFFER_BIT);
            eglSwapBuffers(eglDisplay, eglSurface );
        }

        if(e.type == KeyPress){
            break;
        }
  }

  XCloseDisplay(x11_display);
  return 0;
}

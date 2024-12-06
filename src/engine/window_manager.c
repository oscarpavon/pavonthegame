#include "window_manager.h"
#include <engine/log.h>

#include <stdio.h>

#include <engine/game.h>
#include <engine/log.h>
#include <engine/text_renderer.h>

#include <X11/Xlib.h>
#include <GL/glx.h>

#include <engine/game.h>
#include <engine/input.h>

Display                 *display;
Window                  root_window;
GLint                   attributes[] = { GLX_RGBA, GLX_DEPTH_SIZE, 24, GLX_DOUBLEBUFFER, None };
XVisualInfo             *visual_info;
Colormap                color_map;
XSetWindowAttributes    set_window_attributes;
Window                  window;
GLXContext              gl_context;
XWindowAttributes       window_attributes;


bool pe_wm_swapped = false;

#ifdef ANDROID
#include <engine/platforms/android/android.h>
#include <EGL/egl.h>
EGLDisplay display;
EGLSurface surface;
EGLContext context;
#include <GLES/gl.h>
#endif



#ifdef ANDROID
void pe_wm_egl_init(){
	// Setup OpenGL ES 2
	// http://stackoverflow.com/questions/11478957/how-do-i-create-an-opengl-es-2-context-in-a-native-activity

	const EGLint attribs[] = {
			EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT, //important
			EGL_BLUE_SIZE, 8,
			EGL_GREEN_SIZE, 8,
			EGL_RED_SIZE, 8,
			EGL_DEPTH_SIZE,8,
			EGL_NONE
	};

	EGLint attribList[] =
	{
			EGL_CONTEXT_CLIENT_VERSION, 2,
			EGL_NONE
	};

	EGLint w, h, dummy, format;
	EGLint numConfigs;
	EGLConfig config;


	display = eglGetDisplay(EGL_DEFAULT_DISPLAY);


	eglInitialize(display, 0, 0);

	/* Here, the application chooses the configuration it desires. In this
	 * sample, we have a very simplified selection process, where we pick
	 * the first EGLConfig that matches our criteria */
	eglChooseConfig(display, attribs, &config, 1, &numConfigs);

	/* EGL_NATIVE_VISUAL_ID is an attribute of the EGLConfig that is
	 * guaranteed to be accepted by ANativeWindow_setBuffersGeometry().
	 * As soon as we picked a EGLConfig, we can safely reconfigure the
	 * ANativeWindow buffers to match, using EGL_NATIVE_VISUAL_ID. */
	eglGetConfigAttrib(display, config, EGL_NATIVE_VISUAL_ID, &format);

	if(game == NULL){

		LOG("game null");
	}

	if(game->app == NULL){
		LOG("game->app null");
	}

	if(game->app->window == NULL){
		LOG("game->app->window is null");
	}
	
	ANativeWindow_setBuffersGeometry(game->app->window, 0, 0, format);
	surface = eglCreateWindowSurface(display, config, game->app->window, NULL);

	context = eglCreateContext(display, config, NULL, attribList);

	if (eglMakeCurrent(display, surface, surface, context) == EGL_FALSE) {
		LOG("Unable to eglMakeCurrent");
		return -1;
	}

	// Grab the width and height of the surface
	eglQuerySurface(display, surface, EGL_WIDTH, &w);
	eglQuerySurface(display, surface, EGL_HEIGHT, &h);
	
	// Initialize GL state.
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_FASTEST);
	glEnable(GL_CULL_FACE);
	glDisable(GL_DEPTH_TEST);

  camera_heigth_screen = h;
  camera_width_screen = w;
  window_update_viewport(w, h);
	LOG("EGL initialized");
	return 0;

}
void pe_wm_egl_context_make_current(){
	eglMakeCurrent(display, surface, surface, context);
}
/**
 * Tear down the EGL context currently associated with the display.
 */
void pe_wm_egl_end() {
	if (display != EGL_NO_DISPLAY) {
		eglMakeCurrent(display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
		if (context != EGL_NO_CONTEXT) {
			eglDestroyContext(display, context);
		}
		if (surface != EGL_NO_SURFACE) {
			eglDestroySurface(display, surface);
		}
		eglTerminate(display);
	}
	display = EGL_NO_DISPLAY;
	context = EGL_NO_CONTEXT;
	surface = EGL_NO_SURFACE;
}
#endif


void pe_wm_input_update() {

  // Draw tab bar 	& draw current tabb
  for (u8 i = 0; i < engine_windows.count; i++) {
    EngineWindow *window = array_get(&engine_windows, i);
		if (!window->initialized){
      continue;
		}

#ifdef LINUX
    // The mouse need to stay in the window for window->input call
    if (window->focus) {
      if (window->input)
        window->input();
    }
#endif

#ifdef ANDROID

		if (window->input){

      window->input();
		}
#endif
  }
}

bool pe_wm_should_close(EngineWindow* window){
	if(pe_is_window_terminate)	
		return true;
	else
    return false;
}



void pe_wm_init(){

#ifdef DESKTOP
windows_manager_init();
#endif
}


void window_update_viewport(int width, int height){
		glViewport(0,0,width,height);
#ifdef PE_FREETYPE
    text_renderer_update_pixel_size();
#endif
		camera_update_aspect_ratio(&current_window->camera);
}

void pe_wm_configure_window(EngineWindow* win){
  
	if (win == NULL) { 
		LOG("ERROR: Window not found\n");
    return;
  }
	if (win->initialized){
		LOG("Window already initialized\n");
    return;
	}

  current_window = win;


#ifdef LINUX
	pe_wm_create_x11_window();
#endif

#if defined ANDROID && defined OPENGLES
	pe_wm_egl_init();	
#endif

  win->initialized = true;
	
	LOG("Window created\n");
}


void pe_wm_window_init(EngineWindow* window){
	LOG("Initialing Window\n");
	if(window->init != NULL)
		window->init();
	window->initialized = true;

	LOG("Window init pe_wm_window_init\n");
}



void window_set_focus(EngineWindow* window){
    current_window->focus = false;
    window->focus = true;
    current_window = window;
    //LOG("Focus windows change\n");
}

void pe_wm_context_current(){

#ifdef LINUX
  glXMakeCurrent(display, window, gl_context);
#endif
#ifdef ANDROID
  pe_wm_egl_context_make_current();
#endif

}

void pe_wm_swap_buffers() {
#ifdef ANDROID
  eglSwapBuffers(display, surface);
#endif

#ifdef LINUX
	glXSwapBuffers(display, window);

#endif
}

bool is_wm_swapped(){
	if(pe_wm_swapped){
		pe_wm_swapped=false;
	}
	return true;
}

void pe_wm_events_update() {
#ifdef ANDROID
  pe_android_poll_envents();
#endif
}
void pe_wm_windows_draw() {

  for (u8 i = 0; i < engine_windows.count; i++) {
    EngineWindow *window = array_get(&engine_windows, i);
    if (!window->initialized)
      continue;
    if (pe_wm_should_close(window)) {
      window->finish();
      LOG("Window close\n");
      continue;
    }

  }
}


void windows_manager_init(){
  pe_is_window_init = true;
}

void pe_wm_make_context(EngineWindow*window){
  glXMakeCurrent(display, window, gl_context);
}

void pe_wm_create_x11_window(){

    display = XOpenDisplay(NULL);

    visual_info = glXChooseVisual(display,0,attributes);
    
    root_window = XDefaultRootWindow(display);
    
    color_map = XCreateColormap(display, root_window, visual_info->visual, AllocNone);

    set_window_attributes.colormap = color_map;
    set_window_attributes.event_mask = ExposureMask | KeyPressMask;
    
    window = XCreateWindow(display, root_window, 0, 0, 
        INIT_WINDOW_SIZE_X, INIT_WINDOW_SIZE_Y, 0,
        visual_info->depth, InputOutput, visual_info->visual,
        CWColormap | CWEventMask, &set_window_attributes);
    
    XStoreName(display, window, "pengine");
   
    XMapWindow(display, window);
    XFlush(display);
  
    gl_context = glXCreateContext(display, visual_info, NULL, GL_TRUE);


    if (pe_renderer_type == PEWMOPENGLES2) {
      glXMakeCurrent(display, window, gl_context);
    }
    
    glEnable(GL_DEPTH_TEST); 

    camera_heigth_screen = INIT_WINDOW_SIZE_Y;
    camera_width_screen = INIT_WINDOW_SIZE_X;
    window_update_viewport(INIT_WINDOW_SIZE_X, INIT_WINDOW_SIZE_Y);
}


void window_initialize_windows(){
	for(u8 i = 0; i<engine_windows.count ; i++ ){
		EngineWindow* window = array_get(&engine_windows,i);
		if(window->initialized)
			   continue;
		window->init();	
	}
}



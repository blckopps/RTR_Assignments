#include<iostream>
#include<stdio.h>
#include<stdlib.h>
#include<memory.h>

//xwindows headers
#include<X11/Xlib.h>
#include<X11/Xutil.h>
#include<X11/XKBlib.h>
#include<X11/keysym.h>

//namespace
using namespace std;

//global variables
bool bFullScreen = false;
Display *gpdisplay = NULL;
XVisualInfo *gpXVisualInfo  = NULL;
Colormap gColormap;
Window gWindow;

int giWindowWidth = 600;
int giWindowHeight = 800;

//entry point function
int main(void)
{
     //function prototype
        void CreateWindow(void);
        void Uninitialize(void);
        void ToogleFullScreen(void);
        
        //varival 
        int winWidth = giWindowWidth;
        int winHeight =  giWindowHeight;
        
        //code
        CreateWindow();
        
        //MessageLoop 
        XEvent event;
        KeySym keysym;
        
        while(1)
        {
               XNextEvent(gpdisplay, &event); 
               
               switch(event.type)
               {
                        case MapNotify:
                        
                        break;
                        
                        case KeyPress:
                                keysym = XkbKeycodeToKeysym(gpdisplay, event.xkey.keycode, 0, 0);
                                
                                switch(keysym)
                                {
                                        
                                        case XK_Escape:
                                        
                                                Uninitialize();
                                                exit(0);
                                                
                                        break;
                                        
                                        case XK_f:
                                        case XK_F:
                                        
                                                if(bFullScreen == false)
                                                {
                                                        ToogleFullScreen();
                                                        bFullScreen = true;
                                                }
                                                else
                                                {
                                                        ToogleFullScreen();
                                                        bFullScreen = false;
                                                }       
                                                        
                                        break;
                                        
                                        default:
                                        
                                        break;
                                        
                                        
                                }           
                        break;
                        
                        case ButtonPress:
                                switch(event.xbutton.button)
                                {
                                        case 1:         //LEFT MOUSE BUTTON
                                        break;
                                        
                                        case 2:         //MIDLE SCROLL BUTTON
                                        
                                        break;
                                        
                                        case 3:         //RIGHT MOUSE BUTTON
                                        
                                        break;
                                }
                        break;
                        
                        case MotionNotify:              //WM_MOUSE_MOVE
                                
                        break;
                        
                        case ConfigureNotify:
                                winWidth = event.xconfigure.width;
                                
                                winHeight = event.xconfigure.height;
                        break;
                        
                        case Expose:                    //WM_PAINT
                                
                        break;
                        
                        case DestroyNotify:
                                
                        break;
                        
                        case 33:
                                Uninitialize();
                                exit(0);                                     
                        break;
                        
                        default:
                        
                        break;
               }
        }          
        Uninitialize();
        exit(0);
        
}

///
void CreateWindow(void)
{
        //function prototypes
        void Uninitialize();
        
        //variables
        XSetWindowAttributes winattribs;
        int defaultScreen;
        int defaultDepth;
        int styleMask;
        
        //Code
        gpdisplay = XOpenDisplay(NULL);                 //NULL for defaault display
        if(gpdisplay == NULL)
        {
                printf("ERROR:UNABLE TO OPEN CONNECTION.....\n");
                Uninitialize();
                exit(1);
        }
        
        defaultScreen = XDefaultScreen(gpdisplay);
        
        defaultDepth = DefaultDepth(gpdisplay, defaultScreen);
        //>>
        gpXVisualInfo = (XVisualInfo *)malloc(sizeof(XVisualInfo));
        if(gpXVisualInfo == NULL)
        {
                printf("ERROR: UNABLE TO ALLOCATE MEMORY FOR XVisualInfo...");
                Uninitialize();
                exit(1);
        }
        XMatchVisualInfo(gpdisplay, defaultScreen, defaultDepth, TrueColor, gpXVisualInfo);
        
        if(gpXVisualInfo == NULL)
        {
                printf("ERROR: Unable get VISUAL...EXITTING...");
                Uninitialize();
                exit(1);
        }
        //>>
        winattribs.border_pixel = 0;
        winattribs.background_pixmap = 0;
        winattribs.colormap = XCreateColormap(gpdisplay,
                                              RootWindow(gpdisplay, gpXVisualInfo->screen),
                                              gpXVisualInfo->visual,
                                              AllocNone);       //dont allocate mem for color map,we saved it below line
        gColormap =   winattribs.colormap;                                   
        winattribs.background_pixel = BlackPixel(gpdisplay, defaultScreen);
        winattribs.background_pixmap = 0;
        
        winattribs.event_mask = ExposureMask | VisibilityChangeMask |
                                ButtonPressMask | KeyPressMask | PointerMotionMask |
                                StructureNotifyMask;
                                
        styleMask = CWBorderPixel | CWBackPixel | CWEventMask | CWColormap;
        
        gWindow = XCreateWindow(gpdisplay,
                                RootWindow(gpdisplay, gpXVisualInfo->screen),
                                0,
                                0,
                                giWindowWidth,
                                giWindowHeight,
                                0,
                                gpXVisualInfo->depth,
                                InputOutput,
                                gpXVisualInfo->visual,
                                styleMask,
                                &winattribs);
                                
         if(!gWindow)
         {
                printf("ERROR: XCreateWiow Failed....");
                Uninitialize();
                exit(1);                       
         }                                             
         XStoreName(gpdisplay, gWindow, "FIRST XWINDOW");
         
         Atom windowManagerDelete = XInternAtom(gpdisplay, "WM_DELETE_WINDOW",True);
         XSetWMProtocols(gpdisplay, gWindow, &windowManagerDelete, 1);
         
         XMapWindow(gpdisplay, gWindow);
}

void ToogleFullScreen(void)
{
        //variables
        Atom wm_state;
        Atom fullscreen;
        XEvent xev ={0};
        
        //code
        wm_state = XInternAtom(gpdisplay, "_NET_WM_STATE",False);
        memset(&xev, 0, sizeof(xev));
        
        xev.type = ClientMessage;
        xev.xclient.window = gWindow;
        xev.xclient.message_type = wm_state;
        xev.xclient.format = 32;
        xev.xclient.data.l[0] = bFullScreen ? 0 :1;
        
        fullscreen = XInternAtom(gpdisplay, "_NET_WM_STATE_FULLSCREEN",False);
        xev.xclient.data.l[1] = fullscreen;
        
        XSendEvent(gpdisplay,
                   RootWindow(gpdisplay, gpXVisualInfo->screen),
                   False,
                   StructureNotifyMask,
                   &xev);
}

void Uninitialize(void)
{
        if(gWindow)
        {
                XDestroyWindow(gpdisplay, gWindow);
        }
        if(gColormap)
        {
                XFreeColormap(gpdisplay, gWindow);
        }
        if(gpXVisualInfo)
        {
                free(gpXVisualInfo);
                gpXVisualInfo = NULL;
        }
        if(gpdisplay)
        {
                XCloseDisplay(gpdisplay);
                gpdisplay = NULL;
        }
}

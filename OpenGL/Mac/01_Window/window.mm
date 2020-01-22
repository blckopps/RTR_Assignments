//headers
#import <Foundation/Foundation.h>
#import <Cocoa/Cocoa.h>

#import <QuartzCore/CVDisplayLink.h>

#import <OpenGL/gl3.h>
#import <OpenGL/gl3ext.h>

//'C ' stle funtion declaration
CVReturn MyDisplayLinkCallback(CVDisplayLinkRef , const CVTimeStamp *, 
    const CVTimeStamp*, CVOptionFlags, CVOptionFlags *, void *);


//global variables
FILE * gpFile = NULL;

//interface declaration
@interface AppDelegate : NSObject <NSApplicationDelegate, NSWindowDelegate>
@end

@interface GLView : NSOpenGLView
@end

//entry point function
int main(int argc, const char* argv[])
{
    //code 
    NSAutoreleasePool *pPool = [[NSAutoreleasePool alloc]init];

    NSApp=[NSApplication sharedApplication];
    
    [NSApp setDelegate:[[AppDelegate alloc]init]];

    [NSApp run];

    [pPool release];

    return(0);
}

//interface implementation
@implementation AppDelegate
{
    @private
    NSWindow *window;
    GLView *glView;

}
- (void) applicationDidFinishLaunching:(NSNotification *) aNotification
{
    //create Log file
    NSBundle *mainBundle = [NSBundle mainBundle];
    NSString *appDirName=[mainBundle bundlePath];
    NSString *parentDirPath=[appDirName stringByDeletingLastPathComponent];
    NSString *logFileNameWithPath = [NSString stringWithFormat:@"%@/Log.txt",parentDirPath];
    const char*pszLogFileNameWithPath=[logFileNameWithPath cStringUsingEncoding:NSASCIIStringEncoding];

    gpFile = fopen(pszLogFileNameWithPath,"w");
    if(gpFile == NULL)
    {
        printf("Can't not create log file..Exiting..\n");
        [self release];
        [NSApp terminate:self];
    }
    fprintf(gpFile, "Log created....program started successfully\n");

    //Window
    NSRect win_rect;
    fprintf(gpFile,"NSMakeRect\n");
    
    win_rect=NSMakeRect(0.0, 0.0, 800.0, 600.0);

    fprintf(gpFile, "Before create window\n");
    //create simple window
    window=[[NSWindow alloc] initWithContentRect:win_rect
                                                styleMask:NSWindowStyleMaskTitled |
                     NSWindowStyleMaskClosable | NSWindowStyleMaskMiniaturizable |
                     NSWindowStyleMaskResizable
                                                  backing:NSBackingStoreBuffered
                                                    defer:NO];
    fprintf(gpFile, "ENd create window\n");
    
    [window setTitle:@"MAC OPENGL WINDOW"];
    [window center];
    
    fprintf(gpFile, "Before initWithFrame for GLView\n");
    
    glView=[[GLView alloc]initWithFrame:win_rect];

    [window setContentView:glView];
    [window setDelegate:self];
    [window makeKeyAndOrderFront:self];
    fprintf(gpFile, "End applicationDidFinishLaunching\n");
}

- (void)applicationWillTerminate:(NSNotification *)notification
{
    //code
    fprintf(gpFile,"Program terminated successfully\n");
    if(gpFile)
    {
        fclose(gpFile);
        gpFile=NULL;
    }

}

- (void)windowWillClose:(NSNotification *)notification
{
    //code
    [NSApp terminate:self];
}

- (void)dealloc
{
    //code
    [glView release];

    [window release];

    [super dealloc];
}
@end
//end of AppDelegate

@implementation GLView
{
    @private
    CVDisplayLinkRef displayLink;
}

-(id)initWithFrame:(NSRect)frame;
{
    //code
    self=[super initWithFrame:frame];

    if(self)
    {
        [[self window]setContentView:self];

        NSOpenGLPixelFormatAttribute attrs[]=
        {
            //Must use 4.1  core profile to use OpenGL 4.1
            NSOpenGLPFAOpenGLProfile,
            NSOpenGLProfileVersion4_1Core,
            //Specify display ID to assocciate the GL context with(main display for now)
            NSOpenGLPFAScreenMask, CGDisplayIDToOpenGLDisplayMask(kCGDirectMainDisplay),
            NSOpenGLPFANoRecovery,
            NSOpenGLPFAAccelerated,
            NSOpenGLPFAColorSize, 24,
            NSOpenGLPFADepthSize, 24,
            NSOpenGLPFAAlphaSize, 8,
            NSOpenGLPFADoubleBuffer,
            0 //Must
        };
    

        NSOpenGLPixelFormat *pixelFormat=[[[NSOpenGLPixelFormat alloc]
                            initWithAttributes:attrs] autorelease];

        if(pixelFormat == nil)
        {
            fprintf(gpFile, "No valid OpenGL pixel Format Is available\n");
            [self release];
            [NSApp terminate:self];
        }

        NSOpenGLContext *glContext=[[[NSOpenGLContext alloc]
                initWithFormat:pixelFormat shareContext:nil]autorelease];

        [self setPixelFormat:pixelFormat];
        
        fprintf(gpFile, "setPixelFormat success\n");
        
        [self setOpenGLContext:glContext]; //it automatically release the order context, if present , and sets newer one
        
        fprintf(gpFile,"setOpenGLContext success\n");
    }
     fprintf(gpFile,"initWithFrame return self\n");
    return(self);
}

-(CVReturn)getFrameForTime:(const CVTimeStamp *)pOutputTime
{
    //code
    NSAutoreleasePool *pool=[[NSAutoreleasePool alloc]init];

    [self drawView];

    [pool release];
    return(kCVReturnSuccess);
}

-(void)prepareOpenGL
{
    //code
    //OpenGL Info
    fprintf(gpFile,"OpenGL version: %s \n", glGetString(GL_VERSION));
    fprintf(gpFile,"OpenGL shading version: %s \n", glGetString(GL_SHADING_LANGUAGE_VERSION));

    [[self openGLContext]makeCurrentContext];

    GLint swapInt=1;    //MAC default 1 frame  
    [[self openGLContext]setValues:&swapInt forParameter:NSOpenGLCPSwapInterval];
    ///////Shaders//////////


    //setBackground Color
    glClearColor(0.0f, 0.0f, 1.0f, 1.0f);

    CVDisplayLinkCreateWithActiveCGDisplays(&displayLink);
    CVDisplayLinkSetOutputCallback(displayLink, &MyDisplayLinkCallback, self);

    CGLContextObj  cglContext=(CGLContextObj)[[self openGLContext]CGLContextObj];
    CGLPixelFormatObj cglPixelFormat = (CGLPixelFormatObj)[[self pixelFormat]CGLPixelFormatObj];

    CVDisplayLinkSetCurrentCGDisplayFromOpenGLContext(displayLink, cglContext, cglPixelFormat);

    CVDisplayLinkStart(displayLink);

}

-(void)reshape
{
    //code
    fprintf(gpFile, "In reshape\n");
    CGLLockContext((CGLContextObj)[[self openGLContext]CGLContextObj]);

    NSRect rect=[self bounds];

    GLfloat width=rect.size.width;
    GLfloat height=rect.size.height;

    if(height == 0)
    {
        height = 1;
    }

    glViewport(0, 0, (GLsizei)width, (GLsizei)height);

    CGLUnlockContext((CGLContextObj)[[self openGLContext] CGLContextObj]);
    fprintf(gpFile, "End reshape: context release\n");
}

- (void)drawRect:(NSRect)dirtyRect
{
    //code
    [self drawView];
}

- (void)drawView
{
    //code
    //fprintf(gpFile, "In drawView\n");
    [[self openGLContext]makeCurrentContext];

    CGLLockContext((CGLContextObj)[[self openGLContext]CGLContextObj]);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    CGLFlushDrawable((CGLContextObj)[[self openGLContext]CGLContextObj]);

    CGLUnlockContext((CGLContextObj)[[self openGLContext]CGLContextObj]);
    //fprintf(gpFile, "drawView end: unlocking context\n");
}

-(BOOL)acceptsFirstResponder
{
    //code
    [[self window]makeFirstResponder:self];
    return(YES);
}

-(void)keyDown:(NSEvent *)theEvent
{
    //code
    fprintf(gpFile, "In KeyDown\n");
    int key = (int)[[theEvent characters]characterAtIndex:0];
    
    fprintf(gpFile, "In Key :%d\n"+ key);
    switch(key)
    {
        case 27:
            [self release];
            [NSApp terminate:self];
        break;

        case 'F':
        case 'f':
            fprintf(gpFile, "F key pressed\n");
            [[self window]toggleFullScreen:self];
        break;

        default:

        break;
    }
}

-(void)mouseDown:(NSEvent *)theEvent
{
    //code
}

-(void)mouseDragged:(NSEvent *)theEvent
{
    //Code
}

-(void)rightMouseDown:(NSEvent *)theEvent
{

}

-(void) dealloc
{
    //code
    CVDisplayLinkStop(displayLink);
    CVDisplayLinkRelease(displayLink);

    [super dealloc];
}
@end 
CVReturn MyDisplayLinkCallback(CVDisplayLinkRef displayLink, const CVTimeStamp *pNow,
    const CVTimeStamp *pOutputTime, CVOptionFlags flagsIn, CVOptionFlags *pFlagsOut,
    void *pDisplayLinkContext)
    {
        CVReturn result = [(GLView *)pDisplayLinkContext getFrameForTime:pOutputTime];
        return(result);
    }



//headers
#import <Foundation/Foundation.h>
#import <Cocoa/Cocoa.h>

#import <QuartzCore/CVDisplayLink.h>

#import <OpenGL/gl3.h>
#import <OpenGL/gl3ext.h>
#import "vmath.h"

enum
{
    AMC_ATTRIBUTES_POSITION = 0,
    AMC_ATTRIBUTES_COLOR,
    AMC_ATTRIBUTES_NORMAL,
    AMC_ATTRIBUTES_TEXCOORD0
};

//'C ' stle funtion declaration
CVReturn MyDisplayLinkCallback(CVDisplayLinkRef , const CVTimeStamp *, 
    const CVTimeStamp*, CVOptionFlags, CVOptionFlags *, void *);


//global variables
FILE * gpFile = NULL;

float angle = 0.0f;

//OGL global 


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
    
    [window setTitle:@"MAC OPENGL"];
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
    
    //shaders object
    GLuint gVertexShaderObject_SB;
    GLuint gFragmentShaderObject_SB;
    GLuint gShaderProgramObject_SB;
    
    
    ///////vao vbo///////
    //pyramid
    GLuint vao_pyramid_SB;
    GLuint vbo_position_rectangle_SB;
    GLuint vbo_texcoord_rectangle_SB;
   
    //Cube
    GLuint vao_cube_SB;
    GLuint vbo_position_cube_SB;
    GLuint vbo_texcoord_cube_SB;

   GLuint sampler_Uniform;

    GLuint stoneTexture;
    GLuint kundaliTetxure;

    GLuint mvpUniform_SB;
    
    
    vmath::mat4 perspectiveProjectionMatrix;
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
    //************VERTEX SHADER***************************
    //Define vertex shader Object
    gVertexShaderObject_SB = glCreateShader(GL_VERTEX_SHADER);
    
    //write vertex shader code
    const GLchar* vertexShaderSourceCode =
         "#version 410 core" \
         "\n" \
         "in vec4 vPosition;" \
         "in vec2 vTexcoord;" \
         "out vec2 out_texcoord;" \
         "uniform mat4 u_mvp_matrix;" \
         "void main(void)" \
         "{" \
         "gl_Position = u_mvp_matrix * vPosition;" \
         "out_texcoord = vTexcoord;" \
         " } " ;
    //specify vertex source code to vetex shader object
    glShaderSource(gVertexShaderObject_SB,
                   1,
                   (GLchar **)&vertexShaderSourceCode,
                   NULL);
    //compile vertex shader code
    glCompileShader(gVertexShaderObject_SB);
    
    //Error checking
    GLint iShaderCompilationStatus = 0;
    GLint iInfoLength = 0;
    GLchar *szInfoLog = NULL;
    
    glGetShaderiv(gVertexShaderObject_SB,
                  GL_COMPILE_STATUS,
                  &iShaderCompilationStatus);
    
    if(iShaderCompilationStatus == GL_FALSE)
    {
        glGetShaderiv(gVertexShaderObject_SB,
                      GL_INFO_LOG_LENGTH,
                      &iInfoLength);
        if(iInfoLength > 0)
        {
            szInfoLog = (GLchar *)malloc(iInfoLength);
            
            if(szInfoLog != NULL)
            {
                GLsizei writtened;
                
                glGetShaderInfoLog(gVertexShaderObject_SB,
                                   iInfoLength,
                                   &writtened,
                                   szInfoLog);
                fprintf(gpFile,"ERROR:VERTEX SHADER:%s",szInfoLog);
                free(szInfoLog);
                [self release];
            	[NSApp terminate:self];
            }
        }
    }
    //********************FRAGMENT SHADER*****************
    gFragmentShaderObject_SB = glCreateShader(GL_FRAGMENT_SHADER);
    
    //write vertex shader code
    const GLchar * fragmentShaderSourceCode =
        "#version 410 core" \
        "\n" \
        "in vec2  out_texcoord;" \
        "uniform sampler2D u_Sampler;" \
        "out vec4 fragColor;" \
        "void main(void)" \
        "{" \
            "fragColor = texture(u_Sampler, out_texcoord);" \
        "}" ;
    //specify vertex source code to vetex shader object
    glShaderSource(gFragmentShaderObject_SB,
                   1,
                   (GLchar **)&fragmentShaderSourceCode,
                   NULL);
    //compile vertex shader code
    glCompileShader(gFragmentShaderObject_SB);
    
    //Error checking
    iShaderCompilationStatus = 0;
    iInfoLength = 0;
    szInfoLog = NULL;
    
    glGetShaderiv(gFragmentShaderObject_SB,
                  GL_COMPILE_STATUS,
                  &iShaderCompilationStatus);
    
    if(iShaderCompilationStatus == GL_FALSE)
    {
        glGetShaderiv(gFragmentShaderObject_SB,
                      GL_INFO_LOG_LENGTH,
                      &iInfoLength);
        if(iInfoLength > 0)
        {
            szInfoLog = (GLchar *)malloc(iInfoLength);
            
            if(szInfoLog != NULL)
            {
                GLsizei writtened;
                
                glGetShaderInfoLog(gFragmentShaderObject_SB,
                                   iInfoLength,
                                   &writtened,
                                   szInfoLog);
                fprintf(gpFile,"ERROR:FRAGMENT SHADER:%s",szInfoLog);
                free(szInfoLog);
                [self release];
            	[NSApp terminate:self];
            }
        }
    }
    //Create shader program object
    gShaderProgramObject_SB = glCreateProgram();
    
    //Attach vertex,fragment shader program
    glAttachShader(gShaderProgramObject_SB, gVertexShaderObject_SB);
    glAttachShader(gShaderProgramObject_SB, gFragmentShaderObject_SB);
    
    //*** PRELINKING BINDING TO VERTEX ATTRIBUTES***
    glBindAttribLocation(gShaderProgramObject_SB,
                         AMC_ATTRIBUTES_POSITION,
                         "vPosition");

    glBindAttribLocation(gShaderProgramObject_SB,
                         AMC_ATTRIBUTES_TEXCOORD0,
                         "vTexcoord");
    
    //link above shader program
    glLinkProgram(gShaderProgramObject_SB);
    
    // ***ERROR CHECKING LINKING********
    //Error checking for program linking
    GLint iShaderLinkStatus = 0;
    iInfoLength = 0;
    szInfoLog = NULL;
    
    glGetProgramiv(gShaderProgramObject_SB,
                   GL_LINK_STATUS,
                   &iShaderLinkStatus);
    if(iShaderCompilationStatus == GL_FALSE)
    {
        glGetProgramiv(gShaderProgramObject_SB,
                       GL_INFO_LOG_LENGTH,
                       &iInfoLength);
        if(iInfoLength > 0)
        {
            szInfoLog = (GLchar *)malloc(iInfoLength);
            
            if (szInfoLog != NULL)
            {
                GLsizei wr;
                
                glGetShaderInfoLog(gShaderProgramObject_SB,
                                   iInfoLength,
                                   &wr,
                                   szInfoLog);
                
                fprintf(gpFile,"ERROR:SHADER LINK:%s",szInfoLog);
                free(szInfoLog);
                [self release];
            	[NSApp terminate:self];
            }
            
        }
    }
    
    ///***POST LINKING GETTING UNIFORMS**
    mvpUniform_SB = glGetUniformLocation(gShaderProgramObject_SB,
                                      "u_mvp_matrix");
    
    sampler_Uniform = glGetUniformLocation(gShaderProgramObject_SB,
                            "u_Sampler");
    //create vao_SB
    //                                    "u_mvp_matrix");
    //Load texture
    stoneTexture = [self loadTextureFromBMPFile:"Stone.bmp"];

    kundaliTetxure = [self loadTextureFromBMPFile:"Kundali.bmp"];
    
    fprintf(gpFile,"Post link success!!\n");

    const GLfloat pyramidVertices[] =
                                    {
                                        //1st       
                                        0.0f, 1.0f, 0.0f,
                                        -1.0f, -1.0f, 1.0f,             
                                        1.0f, -1.0f, 1.0f,
                                        //2nd
                    
                                        0.0f, 1.0f, 0.0f,
                                        1.0f, -1.0f, 1.0f,
                                        1.0f, -1.0f, -1.0f,             
                                        //3rd
                                        0.0f, 1.0f, 0.0f,               
                                        1.0f, -1.0f, -1.0f,             
                                        -1.0f, -1.0f, -1.0f,
                                        //4th
                                        0.0f, 1.0f, 0.0f,               
                                        -1.0f, -1.0f, -1.0f,
                                        -1.0f, -1.0f, 1.0f
                                    };

    const GLfloat stoneTexcooeds [] =
                                    {
                                            0.5f, 1.0f,
                                            0.0f, 0.0f,
                                            1.0f, 0.0f,

                                            0.5f, 1.0f,
                                            1.0f, 0.0f,
                                            0.0f, 0.0f,

                                            0.5f, 1.0f,
                                            1.0f, 0.0f,
                                            0.0f, 0.0f,

                                            0.5f, 1.0f,
                                            0.0f, 0.0f,
                                            1.0f, 0.0f
                                    };

    const GLfloat cube_vertices[] =
                                    {
                                        //TOP
                                         1.0f,1.0f,-1.0f,
                                         -1.0f,1.0f,-1.0f,
                                         -1.0f,1.0f,1.0f,                   
                                         1.0f,1.0f,1.0f,                    
                                        //bottom
     
                                         1.0f,-1.0f,-1.0f,
                                         -1.0f,-1.0f,-1.0f,             
                                         -1.0f,-1.0f,1.0f,
                                         1.0f, -1.0f, 1.0f,
                                        ////Front
     
                                         1.0f,1.0f,1.0f,                    
                                         -1.0f,1.0f,1.0f,               
                                         -1.0f,-1.0f,1.0f,              
                                         1.0f,-1.0f,1.0f,
                                        //back
     
                                         1.0f,1.0f,-1.0f,               
                                         -1.0f,1.0f,-1.0f,              
                                         -1.0f,-1.0f,-1.0f,             
                                         1.0f,-1.0f,-1.0f,

                                        //Right
     
                                         1.0f,1.0f,-1.0f,               
                                         1.0f,1.0f,1.0f,                    
                                         1.0f,-1.0f,1.0f,               
                                         1.0f,-1.0f,-1.0f,

                                        //left
     
                                         -1.0f,1.0f,1.0f,                   
                                         -1.0f,1.0f,-1.0f,                  
                                         -1.0f,-1.0f,-1.0f,                 
                                         -1.0f,-1.0f,1.0f
                                    };

    const GLfloat cube_texture_vertex[]=
                                        {
                                            0.0f, 1.0f,
                                            0.0f, 0.0f,
                                            1.0f, 0.0f,
                                            1.0f, 1.0f,

                                            1.0f, 1.0f,
                                            0.0f, 1.0f,
                                            0.0f, 0.0f,
                                            1.0f, 0.0f,

                                            0.0f, 0.0f,
                                            1.0f, 0.0f,
                                            1.0f, 1.0f,
                                            0.0f, 1.0f,

                                            1.0f, 0.0f,
                                            1.0f, 1.0f,
                                            0.0f, 1.0f,
                                            0.0f, 0.0f,

                                            1.0f, 0.0f,
                                            1.0f, 1.0f,
                                            0.0f, 1.0f,
                                            0.0f, 0.0f,

                                            0.0f, 0.0f,
                                            1.0f, 0.0f,
                                            1.0f, 1.0f,
                                            0.0f, 1.0f
                                            
                                        };
    

    //Create vao and vbo for CUBE
    //START:
         glGenVertexArrays(1, &vao_cube_SB);
    
        glBindVertexArray(vao_cube_SB);

        glGenBuffers(1, &vbo_position_cube_SB);    //vbo position attach cube

        glBindBuffer(GL_ARRAY_BUFFER,
                     vbo_position_cube_SB);

        glBufferData(GL_ARRAY_BUFFER,
                     sizeof(cube_vertices),
                     cube_vertices,
                     GL_STATIC_DRAW);

        glVertexAttribPointer(AMC_ATTRIBUTES_POSITION,
                                3,
                                GL_FLOAT,
                                GL_FALSE,
                                0,
                                NULL);

        glEnableVertexAttribArray(AMC_ATTRIBUTES_POSITION);

        glBindBuffer(GL_ARRAY_BUFFER, 0);       //Unbind vbo position py
        
        //texture
        glGenBuffers(1, &vbo_texcoord_cube_SB);

        glBindBuffer(GL_ARRAY_BUFFER, vbo_texcoord_cube_SB);

        glBufferData(GL_ARRAY_BUFFER,
                     sizeof(cube_texture_vertex),
                     cube_texture_vertex,
                     GL_STATIC_DRAW);

        glVertexAttribPointer(AMC_ATTRIBUTES_TEXCOORD0,
                                2,
                                GL_FLOAT,
                                GL_FALSE,
                                0,
                                NULL);

        glEnableVertexAttribArray(AMC_ATTRIBUTES_TEXCOORD0);

        glBindBuffer(GL_ARRAY_BUFFER, 0);

        glBindVertexArray(0);                               
    //END:
    //create vao for pyramid....
    //START:
    glGenVertexArrays(1, &vao_pyramid_SB);
    

    glBindVertexArray(vao_pyramid_SB);  //vao triangle bind
    

    glGenBuffers(1, &vbo_position_rectangle_SB);

    glBindBuffer(GL_ARRAY_BUFFER,
                vbo_position_rectangle_SB);

    glBufferData(GL_ARRAY_BUFFER,
                 sizeof(pyramidVertices),
                 pyramidVertices,
                 GL_STATIC_DRAW);

    glVertexAttribPointer(AMC_ATTRIBUTES_POSITION,
                            3,
                            GL_FLOAT,
                            GL_FALSE,
                            0,
                            NULL
                            );

    glEnableVertexAttribArray(AMC_ATTRIBUTES_POSITION);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    ////color vbo triangle
    glGenBuffers(1, &vbo_texcoord_rectangle_SB);

    glBindBuffer(GL_ARRAY_BUFFER,
                 vbo_texcoord_rectangle_SB);

    glBufferData(GL_ARRAY_BUFFER,
                sizeof(stoneTexcooeds),
                stoneTexcooeds,
                GL_STATIC_DRAW);

    glVertexAttribPointer(AMC_ATTRIBUTES_TEXCOORD0,
                            2,
                            GL_FLOAT,
                            GL_FALSE,
                            0,
                            NULL);

    glEnableVertexAttribArray(AMC_ATTRIBUTES_TEXCOORD0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glBindVertexArray(0);                       //vao pyramid unbind...
    ////END
    
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);

    //setBackground Color
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClearDepth(1.0f);
    
     glEnable(GL_TEXTURE_2D);
    perspectiveProjectionMatrix = vmath::mat4::identity();

    //End OGL
    CVDisplayLinkCreateWithActiveCGDisplays(&displayLink);
    CVDisplayLinkSetOutputCallback(displayLink, &MyDisplayLinkCallback, self);

    CGLContextObj  cglContext=(CGLContextObj)[[self openGLContext]CGLContextObj];
    CGLPixelFormatObj cglPixelFormat = (CGLPixelFormatObj)[[self pixelFormat]CGLPixelFormatObj];

    CVDisplayLinkSetCurrentCGDisplayFromOpenGLContext(displayLink, cglContext, cglPixelFormat);

    CVDisplayLinkStart(displayLink);

}
//load texture
- (GLuint)loadTextureFromBMPFile:(const char *)texFileName
{
    fprintf(gpFile, "load texture start...\n");

    NSBundle *mainBundle = [NSBundle mainBundle];
    fprintf(gpFile, "load texture mainBundle\n");

    NSString *appDirName=[mainBundle bundlePath];
    fprintf(gpFile, "load texture appDirName\n");

    NSString *parentDirPath=[appDirName stringByDeletingLastPathComponent];
    fprintf(gpFile, "load texture parentDirPath\n");

    NSString *textureFileNameWithPath = [NSString stringWithFormat:@"%@/%s",parentDirPath,texFileName];

    fprintf(gpFile, "After getting bmp path\n");

    NSImage *bmpImage = [[NSImage alloc]initWithContentsOfFile:textureFileNameWithPath];

    if(!bmpImage)
    {
        fprintf(gpFile, "load texture: bmpImage failed\n");

        NSLog(@"can not find %@", textureFileNameWithPath);
        return(0);
    }
    fprintf(gpFile, "load texture: after initWithContentOfFile \n");
    //convert NSImage to CGImage(CG for core graphics)
    CGImageRef cgImage = [bmpImage CGImageForProposedRect:nil context:nil hints:nil];

    fprintf(gpFile, "load texture: after CGImageRef \n");

    int w = (int)CGImageGetWidth(cgImage);
    int h = (int)CGImageGetHeight(cgImage);

    CFDataRef imageData = CGDataProviderCopyData(CGImageGetDataProvider(cgImage));
    void* pixels = (void*)CFDataGetBytePtr(imageData);

    GLuint bmpTexture;
    glGenTextures(1, &bmpTexture);

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);  //set 1 rather than 4 for better performance

    glBindTexture(GL_TEXTURE_2D, bmpTexture);

    glTexParameteri(GL_TEXTURE_2D,
            GL_TEXTURE_MAG_FILTER,
            GL_LINEAR);

    glTexParameteri(GL_TEXTURE_2D,
            GL_TEXTURE_MIN_FILTER,
            GL_LINEAR_MIPMAP_LINEAR);

    glTexImage2D(GL_TEXTURE_2D,
                        0,
                        GL_RGBA,
                        w,
                        h,
                        0,
                        GL_RGBA,
                        GL_UNSIGNED_BYTE,
                        pixels);

    glGenerateMipmap(GL_TEXTURE_2D);

    CFRelease(imageData);
    fprintf(gpFile, "load texture successed\n");
    return(bmpTexture);
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

    perspectiveProjectionMatrix = vmath::perspective(45.0f,width/height, 0.1f, 100.0f);

    
    
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

    glUseProgram(gShaderProgramObject_SB);

    vmath::mat4 modelViewMatrix;
     vmath::mat4 modelViewProjectionMatrix;
     vmath::mat4 translateMatrix;
     vmath::mat4 rotationMatrix;
     //mat4 rotationMatrix;
    
    //make identity matrix
     modelViewMatrix = vmath::mat4::identity();
     modelViewProjectionMatrix = vmath::mat4::identity();
     translateMatrix = vmath::mat4::identity();
     rotationMatrix = vmath::mat4::identity();
     //rotationMatrix = mat4::identity();

     //Do transformations...
     modelViewMatrix = vmath::translate(-2.0f, 0.0f, -6.0f);
     rotationMatrix = vmath::rotate(angle, 0.0f, 1.0f, 0.0f);

     modelViewProjectionMatrix = perspectiveProjectionMatrix * modelViewMatrix;

     modelViewProjectionMatrix = modelViewProjectionMatrix * rotationMatrix;

     glUniformMatrix4fv(mvpUniform_SB,
                        1,
                        GL_FALSE,
                        modelViewProjectionMatrix);
    
     glActiveTexture(GL_TEXTURE0);

     glBindTexture(GL_TEXTURE_2D, stoneTexture);

     glUniform1i(sampler_Uniform,
                0);

     glBindVertexArray(vao_pyramid_SB);

     glDrawArrays(GL_TRIANGLE_FAN,
                    0,
                    12);

     glBindVertexArray(0);


     //////////////DRAW CUBE/////////////////
     modelViewMatrix = vmath::mat4::identity();
     modelViewProjectionMatrix = vmath::mat4::identity();
     rotationMatrix = vmath::mat4::identity();

     modelViewMatrix = vmath::translate(2.0f, 0.0f, -6.5f);
     rotationMatrix = vmath::rotate(angle, 0.0f, 0.0f, 1.0f);
     rotationMatrix = rotationMatrix * vmath::rotate(angle, 0.0f, 1.0f, 0.0f);
     rotationMatrix = rotationMatrix * vmath::rotate(angle, 1.0f, 0.0f, 0.0f);

     modelViewProjectionMatrix = perspectiveProjectionMatrix * modelViewMatrix;

     modelViewProjectionMatrix = modelViewProjectionMatrix * rotationMatrix;

     glUniformMatrix4fv(mvpUniform_SB,
                        1,
                        GL_FALSE,
                        modelViewProjectionMatrix);

     glActiveTexture(GL_TEXTURE0);

     glBindTexture(GL_TEXTURE_2D, kundaliTetxure);

     glUniform1i(sampler_Uniform,
                0);
     //
     glBindVertexArray(vao_cube_SB);

     glDrawArrays(GL_TRIANGLE_FAN,
                    0,
                    4);

      glDrawArrays(GL_TRIANGLE_FAN,
                    4,
                    4);

      glDrawArrays(GL_TRIANGLE_FAN,
                    8,
                    4);

      glDrawArrays(GL_TRIANGLE_FAN,
                    12,
                    4);

     glDrawArrays(GL_TRIANGLE_FAN,
                    16,
                    4);

     glDrawArrays(GL_TRIANGLE_FAN,
                    20,
                    4);

     glBindVertexArray(0);

     glUseProgram(0);
    //swaping buffers
    CGLFlushDrawable((CGLContextObj)[[self openGLContext]CGLContextObj]);

    CGLUnlockContext((CGLContextObj)[[self openGLContext]CGLContextObj]);
    //fprintf(gpFile, "drawView end: unlocking context\n");
    angle += 0.1f;
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
    
    //
    if (vbo_position_rectangle_SB)
    {
        glDeleteBuffers(1, &vbo_position_rectangle_SB);
        vbo_position_rectangle_SB = 0;
    }
    if(vbo_texcoord_rectangle_SB)
    {
        glDeleteBuffers(1, &vbo_texcoord_rectangle_SB);
        vbo_texcoord_rectangle_SB = 0;
    }
    if (vao_pyramid_SB)
    {
        glDeleteVertexArrays(1, &vao_pyramid_SB);
        vao_pyramid_SB = 0;
    }
    //vertex shader
    glDetachShader(gShaderProgramObject_SB,gVertexShaderObject_SB);
    glDeleteShader(gVertexShaderObject_SB);
    gVertexShaderObject_SB = 0;
    //fragment
    glDetachShader(gShaderProgramObject_SB,gFragmentShaderObject_SB);
    glDeleteShader(gFragmentShaderObject_SB);
    gFragmentShaderObject_SB = 0;
    //shader object
    glDeleteProgram(gShaderProgramObject_SB);
    
    gShaderProgramObject_SB = 0;
    //
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



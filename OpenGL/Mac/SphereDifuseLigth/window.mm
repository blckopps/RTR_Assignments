
#import<Foundation/Foundation.h>
#import<Cocoa/Cocoa.h>

#import<QuartzCore/CVDisplayLink.h>

#import<OpenGL/gl3.h>
#import<OpenGL/gl3ext.h>
#import "vmath.h"
#import "Sphere.h"
enum
{
    AMC_ATTRIBUTE_POSITION=0,
    AMC_ATTRIBUTE_COLOR,
    AMC_ATTRIBUTE_NORMAL,
    AMC_ATTRIBUTE_TEXCOORD0
};

CVReturn MyDisplayLinkCallback(CVDisplayLinkRef, const CVTimeStamp *,const CVTimeStamp *,CVOptionFlags,CVOptionFlags *,void *);

FILE *gpFile=NULL;
Sphere *sphere=[Sphere alloc];
@interface AppDelegate : NSObject <NSApplicationDelegate, NSWindowDelegate>
@end

@interface GLView : NSOpenGLView
@end

int main(int argc, const char * argv[])
{
    NSAutoreleasePool *pPool=[[NSAutoreleasePool alloc]init];
    
    NSApp=[NSApplication sharedApplication];
    [NSApp setDelegate:[[AppDelegate alloc]init]];
    
    [NSApp run];
    [pPool release];
    return(0);
}

@implementation AppDelegate
{
@private
    NSWindow *window;
    GLView *glView;
}

- (void)applicationDidFinishLaunching:(NSNotification *)aNotification
{
    
    
    // log file
    
    NSBundle *mainBundle=[NSBundle mainBundle];
    NSString *appDirName=[mainBundle bundlePath];
    NSString *parentDirPath=[appDirName stringByDeletingLastPathComponent];
    NSString *logFileNameWithPath=[NSString stringWithFormat:@"%@/Log.txt",parentDirPath];
    const char *pszLogFileNameWithPath=[logFileNameWithPath cStringUsingEncoding:NSASCIIStringEncoding];
    gpFile=fopen(pszLogFileNameWithPath,"w");
    
    if(gpFile==NULL)
    {
        printf("Cannot create log file\n");
        [self release];
        [NSApp terminate:self];
    }
    
    fprintf(gpFile,"Program is started successfully");
    
    
    NSRect win_rect;
    win_rect=NSMakeRect(0.0,0.0,800.0,600.0);
    
    window=[[NSWindow alloc] initWithContentRect:win_rect
                                       styleMask:NSWindowStyleMaskTitled |
            NSWindowStyleMaskClosable | NSWindowStyleMaskMiniaturizable |
            NSWindowStyleMaskResizable
                                         backing:NSBackingStoreBuffered
                                           defer:NO];
    [window setTitle:@"macOS Diffuse Light on Sphere"];
    [window center];
    
    glView=[[GLView alloc]initWithFrame:win_rect];
    
    [window setContentView:glView];
    [window setDelegate:self];
    [window makeKeyAndOrderFront:self];
}

-(void)applicationWillTerminate:(NSNotification *)notification
{
    fprintf(gpFile,"Program is terminated successfully");
    
    if(gpFile)
    {
        fclose(gpFile);
        gpFile=NULL;
    }
    
    //code
}

-(void)windowWillClose:(NSNotification *)notification
{
    [NSApp terminate:self];
}

-(void)dealloc
{
    [glView release];
    [window release];
    [super dealloc];
}
@end

@implementation GLView
{
@private
    CVDisplayLinkRef displayLink;
    
    GLuint gVertexShaderObject;
    GLuint gFragmentShaderObject;
    GLuint gShaderProgramObject;
    
    GLuint vao_rectangle;
    GLuint vbo_position_rectangle;
    GLuint vbo_normal_rectangle;
    GLuint mvUniform;
    GLuint pUniform;
    GLuint ldUniform;
    GLuint kdUniform;
    GLuint lightPositionUniform;
    GLuint isLkeyPressedUniform;
    vmath::mat4 perspectiveProjectionMatrix;
    vmath::mat4 modelViewMatrix;
    
    GLfloat angle_rect;
    bool gbIsLighting;
    bool gbIsAnimation;
    
    GLuint vao_sphere;
    GLuint vbo_sphere_position;
    GLuint vbo_sphere_normal;
    GLuint vbo_sphere_element;
    
    float sphere_vertices[1146];
    float sphere_normals[1146];
    float sphere_textures[746];
    short sphere_elements[2280];
    
    int numVertices;
    int numElements;
    
}

-(id)initWithFrame:(NSRect)frame;
{
    self=[super initWithFrame:frame];
    
    if(self)
    {
        [[self window]setContentView:self];
        
        NSOpenGLPixelFormatAttribute attrs[]=
        {
            NSOpenGLPFAOpenGLProfile,
            NSOpenGLProfileVersion4_1Core,
            NSOpenGLPFAScreenMask,CGDisplayIDToOpenGLDisplayMask(kCGDirectMainDisplay),
            NSOpenGLPFANoRecovery,
            NSOpenGLPFAAccelerated,
            NSOpenGLPFAColorSize,24,
            NSOpenGLPFADepthSize,24,
            NSOpenGLPFAAlphaSize,8,
            NSOpenGLPFADoubleBuffer,
            0
        };//last 0 is must
        
        NSOpenGLPixelFormat *pixelFormat=[[[NSOpenGLPixelFormat alloc]initWithAttributes:attrs]autorelease];
        
        if(pixelFormat==nil)
        {
            fprintf(gpFile,"\nNo Valid OpenGL Pixel Format Is Available");
            [self release];
            [NSApp terminate:self];
        }
        
        NSOpenGLContext *glContext=[[[NSOpenGLContext alloc]initWithFormat:pixelFormat shareContext:nil]autorelease];
        
        [self setPixelFormat:pixelFormat];
        
        [self setOpenGLContext:glContext];
        
    }
    return(self);
}


-(CVReturn)getFrameForTime:(const CVTimeStamp *)pOutputTime

{
    NSAutoreleasePool *pool=[[NSAutoreleasePool alloc]init];
    [self drawView];
    [pool release];
    
    return(kCVReturnSuccess);
}


-(void)prepareOpenGL
{
    
    angle_rect = 0.0f;
    gbIsLighting = false;
    gbIsAnimation = false;
    
    [sphere getSphereVertexData:sphere_vertices andArray1:sphere_normals andArray2:sphere_textures andArray3:sphere_elements];
    
    
    numVertices=[sphere getNumberOfSphereVertices];
    numElements=[sphere getNumberOfSphereElements];
    fprintf(gpFile,"OpenGL Version : %s\n",glGetString(GL_VERSION));
    fprintf(gpFile,"GLSL Version : %s\n",glGetString(GL_SHADING_LANGUAGE_VERSION));
    
    [[self openGLContext]makeCurrentContext];
    
    GLint swapInt=1;
    [[self openGLContext]setValues:&swapInt forParameter:NSOpenGLCPSwapInterval];
    
    //Vertex Shader
    gVertexShaderObject = glCreateShader(GL_VERTEX_SHADER);
    
    //write vertex shader code
    const GLchar *vertexShaderSourceCode =
    "#version 330 core" \
    "\n" \
    "in vec4 vPosition;" \
    "in vec3 vNormal;" \
    "uniform mat4 u_mv_matrix;" \
    "uniform mat4 u_p_matrix;" \
    "uniform int u_LkeyIsPressed;" \
    "uniform vec3 u_ld;" \
    "uniform vec3 u_kd;" \
    "uniform vec4 u_lightPosition;" \
    "out vec3 diffuseColor;" \
    "void main(void)" \
    "{" \
    "    if(u_LkeyIsPressed==1)" \
    "    {" \
    "        vec4 eyeCoordinate=u_mv_matrix * vPosition;" \
    "        mat3 normalMatrix= mat3(transpose(inverse(u_mv_matrix)));" \
    "        vec3 tNormal=normalize(normalMatrix*vNormal);" \
    "        vec3 s=normalize(vec3(u_lightPosition.xyz-eyeCoordinate.xyz));" \
    "        diffuseColor=u_ld*u_kd*dot(s,tNormal);" \
    "    }" \
    "gl_Position=u_p_matrix *u_mv_matrix* vPosition;" \
    "}";
    
    //specify above source code to vertexShaderObject
    glShaderSource(gVertexShaderObject, 1,
                   (const GLchar**)&vertexShaderSourceCode,
                   NULL);
    glCompileShader(gVertexShaderObject);
    GLint iShaderCompileStatus = 0;
    GLint iInfoLogLength = 0;
    GLchar *szInfoLog = NULL;
    
    glGetShaderiv(gVertexShaderObject,
                  GL_COMPILE_STATUS,
                  &iShaderCompileStatus);
    fprintf(gpFile,"Value of ivertex shader compile Status is : %d", iShaderCompileStatus);
    if (iShaderCompileStatus == GL_FALSE)
    {
        glGetShaderiv(gVertexShaderObject,
                      GL_INFO_LOG_LENGTH,
                      &iInfoLogLength);
        if (iInfoLogLength > 0)
        {
            szInfoLog = (GLchar*)malloc(iInfoLogLength);
            if (szInfoLog != NULL)
            {
                GLsizei writtened;
                glGetShaderInfoLog(gVertexShaderObject,
                                   iInfoLogLength,
                                   &writtened,
                                   szInfoLog);
                fprintf(gpFile,"Error in compiling vertex Shader : %hs", szInfoLog);
                free(szInfoLog);
                
                [self dealloc];
            }
        }
    }
    
    //define Fragment shader object
    gFragmentShaderObject = glCreateShader(GL_FRAGMENT_SHADER);
    
    //write fragment shader code
    const GLchar *fragmentShaderSourceCode =
    "#version 330 core" \
    "\n" \
    "in vec3 diffuseColor;" \
    "uniform int u_LkeyIsPressed;" \
    "out vec4 FragColor;" \
    "void main(void)" \
    "{" \
    "    if(u_LkeyIsPressed==1)" \
    "    {" \
    "        FragColor=vec4(diffuseColor,1.0);" \
    "    }" \
    "    else" \
    "    {" \
    "        FragColor=vec4(1.0,1.0,1.0,1.0);" \
    "    }" \
    "}";
    
    //specify above source code to FragmentShaderObject
    glShaderSource(gFragmentShaderObject, 1,
                   (const GLchar**)&fragmentShaderSourceCode,
                   NULL);
    glCompileShader(gFragmentShaderObject);
    
    iShaderCompileStatus = 0;
    iInfoLogLength = 0;
    szInfoLog = NULL;
    
    glGetShaderiv(gFragmentShaderObject,
                  GL_COMPILE_STATUS,
                  &iShaderCompileStatus);
    fprintf(gpFile,"Value of ifragment shader compile Status is : %d", iShaderCompileStatus);
    if (iShaderCompileStatus == GL_FALSE)
    {
        glGetShaderiv(gFragmentShaderObject,
                      GL_INFO_LOG_LENGTH,
                      &iInfoLogLength);
        if (iInfoLogLength > 0)
        {
            szInfoLog = (GLchar*)malloc(iInfoLogLength);
            if (szInfoLog != NULL)
            {
                GLsizei writtened;
                glGetShaderInfoLog(gFragmentShaderObject,
                                   iInfoLogLength,
                                   &writtened,
                                   szInfoLog);
                fprintf(gpFile,"Error in compiling Fragment Shader : %hs", szInfoLog);
                free(szInfoLog);
                [self dealloc];
                //DestroyWindow(ghHwnd);
            }
        }
    }
    
    //create Shader program object
    gShaderProgramObject = glCreateProgram();
    
    //attach vertex/fragment shaders
    glAttachShader(gShaderProgramObject,
                   gVertexShaderObject);
    glAttachShader(gShaderProgramObject,
                   gFragmentShaderObject);
    
    //prelinking binding to vertex attributes
    glBindAttribLocation(gShaderProgramObject,AMC_ATTRIBUTE_POSITION,"vPosition");
    glBindAttribLocation(gShaderProgramObject, AMC_ATTRIBUTE_NORMAL, "vNormal");
    
    //Link the shader program
    fprintf(gpFile,"attach comp\n");
    glLinkProgram(gShaderProgramObject);
    //ErrorCheck for linking
    
    GLint iProgramLinkStatus = 0;
    iInfoLogLength = 0;
    szInfoLog = NULL;
    
    glGetProgramiv(gShaderProgramObject,
                   GL_LINK_STATUS,
                   &iProgramLinkStatus);
    fprintf(gpFile,"Value of iProgramLinkStatus is : %d", iProgramLinkStatus);
    if (iProgramLinkStatus == GL_FALSE)
    {
        glGetProgramiv(gShaderProgramObject,
                       GL_INFO_LOG_LENGTH,
                       &iInfoLogLength);
        if (iInfoLogLength > 0)
        {
            szInfoLog = (GLchar*)malloc(iInfoLogLength);
            if (szInfoLog != NULL)
            {
                GLsizei writtened;
                glGetProgramInfoLog(gShaderProgramObject,
                                    iInfoLogLength,
                                    &writtened,
                                    szInfoLog);
                fprintf(gpFile,"Error in Linking Shader : %hs", szInfoLog);
                free(szInfoLog);
                [self dealloc];
                //DestroyWindow(ghHwnd);
            }
        }
    }
    
    //post linking retrieving uniform location
    mvUniform = glGetUniformLocation(gShaderProgramObject, "u_mv_matrix");
    pUniform = glGetUniformLocation(gShaderProgramObject, "u_p_matrix");
    
    ldUniform = glGetUniformLocation(gShaderProgramObject, "u_ld");
    kdUniform = glGetUniformLocation(gShaderProgramObject, "u_kd");
    lightPositionUniform = glGetUniformLocation(gShaderProgramObject, "u_lightPosition");
    isLkeyPressedUniform = glGetUniformLocation(gShaderProgramObject, "u_LkeyIsPressed");
    
    glGenVertexArrays(1, &vao_sphere);
    glBindVertexArray(vao_sphere);
    glGenBuffers(1, &vbo_sphere_position);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_sphere_position);
    glBufferData(GL_ARRAY_BUFFER, sizeof(sphere_vertices), sphere_vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(AMC_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(AMC_ATTRIBUTE_POSITION);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    
    
    glGenBuffers(1, &vbo_sphere_normal);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_sphere_normal);
    glBufferData(GL_ARRAY_BUFFER, sizeof(sphere_normals), sphere_normals, GL_STATIC_DRAW);
    glVertexAttribPointer(AMC_ATTRIBUTE_NORMAL, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(AMC_ATTRIBUTE_NORMAL);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    
    glGenBuffers(1, &vbo_sphere_element);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_sphere_element);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(sphere_elements), sphere_elements, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    
    glBindVertexArray(0);
    /////////////////////////////////////////////////////////
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClearDepth(1.0f);
    perspectiveProjectionMatrix = vmath::mat4::identity();
    modelViewMatrix = vmath::mat4::identity();
    [self reshape];
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    
    
    CVDisplayLinkCreateWithActiveCGDisplays(&displayLink);
    CVDisplayLinkSetOutputCallback(displayLink,&MyDisplayLinkCallback,self);
    CGLContextObj cglContext=(CGLContextObj)[[self openGLContext]CGLContextObj];
    CGLPixelFormatObj cglPixelFormat=(CGLPixelFormatObj)[[self pixelFormat]CGLPixelFormatObj];
    CVDisplayLinkSetCurrentCGDisplayFromOpenGLContext(displayLink,cglContext,cglPixelFormat);
    CVDisplayLinkStart(displayLink);
}


-(void)reshape
{
    CGLLockContext((CGLContextObj)[[self openGLContext]CGLContextObj]);
    
    NSRect rect=[self bounds];
    
    GLfloat width=rect.size.width;
    GLfloat height=rect.size.height;
    
    if(height==0)
        height=1;
    
    glViewport(0,0,(GLsizei)width,(GLsizei)height);
    perspectiveProjectionMatrix=vmath::perspective(45.0f,
                                                   (GLfloat)width / (GLfloat)height,
                                                   0.1f,
                                                   100.0f);
    CGLUnlockContext((CGLContextObj)[[self openGLContext] CGLContextObj]);
}

-(void)drawRect:(NSRect)dirtyRect
{
    [self drawView];
}


-(void)drawView
{
    [[self openGLContext]makeCurrentContext];
    CGLLockContext((CGLContextObj)[[self openGLContext]CGLContextObj]);
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
    
    glUseProgram(gShaderProgramObject);
    
    //declaration of matrices
    vmath::mat4 translationMatrix;
    vmath::mat4 rotationMatrix;
    
    //modelViewProjectionMatrix = mat4::identity();
    translationMatrix = vmath::mat4::identity();
    rotationMatrix = vmath::mat4::identity();
    translationMatrix = vmath::translate(0.0f, 0.0f, -3.0f);
    modelViewMatrix = translationMatrix;
    
    if (gbIsLighting == true)
    {
        glUniform1i(isLkeyPressedUniform, 1);
        glUniform3f(ldUniform, 1.0f, 1.0f, 1.0f);
        glUniform3f(kdUniform, 0.5f, 0.5f, 0.5f);
        glUniform4f(lightPositionUniform, 0.0f, 0.0f, 0.0f, 1.0f);
    }
    else
    {
        glUniform1i(isLkeyPressedUniform, 0);
    }
    
    glUniformMatrix4fv(pUniform, 1, GL_FALSE, perspectiveProjectionMatrix);
    glUniformMatrix4fv(mvUniform, 1, GL_FALSE, modelViewMatrix);
    
    //similarly bind with textures if any
    
    glBindVertexArray(vao_sphere);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_sphere_element);
    glDrawElements(GL_TRIANGLES, numElements, GL_UNSIGNED_SHORT, 0);
    glBindVertexArray(0);
    //unuse program
    glUseProgram(0);
    
    
    CGLFlushDrawable((CGLContextObj)[[self openGLContext]CGLContextObj]);
    
    if (gbIsAnimation == true)
    {
        angle_rect += 0.5f;
        if (angle_rect >= 360.0f)
            angle_rect = 0;
    }
    CGLUnlockContext((CGLContextObj)[[self openGLContext]CGLContextObj]);
    
}

-(BOOL)acceptsFirstResponder
{
    [[self window]makeFirstResponder:self];
    return(YES);
    
}

-(void)keyDown:(NSEvent *)theEvent
{
    int key=(int)[[theEvent characters]characterAtIndex:0];
    switch(key)
    {
        case 27:
            [self release];
            [NSApp terminate:self];
            break;
            
        case 'F':
        case 'f':
            [[self window]toggleFullScreen:self];
            break;
        case 'L':case 'l':
            if (gbIsLighting == false)
            {
                gbIsLighting = true;
            }
            else
            {
                gbIsLighting = false;
                //glUniform1i(isLkeyPressedUniform, 0);
            }
            break;
        case 'a':case 'A':
            if (gbIsAnimation == false)
            {
                gbIsAnimation = true;
            }
            else
            {
                gbIsAnimation = false;
            }
            break;
        default:
            break;
    }
    
}

-(void)mouseDown:(NSEvent *)theEvent
{
    
}

-(void)mouseDragged:(NSEvent *)theEvent
{
    
}

-(void)rightMouseDown:(NSEvent *)theEvent
{
    
}

-(void)dealloc
{
    
    if (vbo_sphere_normal)
    {
        glDeleteBuffers(1, &vbo_sphere_normal);
        vbo_sphere_normal = 0;
    }
    if (vbo_sphere_element)
    {
        glDeleteBuffers(1, &vbo_sphere_element);
        vbo_sphere_element = 0;
    }
    
    if (vbo_sphere_position)
    {
        glDeleteBuffers(1, &vbo_sphere_position);
        vbo_sphere_position = 0;
    }
    
    
    if (vao_sphere)
    {
        glDeleteVertexArrays(1, &vao_sphere);
        vao_sphere = 0;
    }
    
    glUseProgram(gShaderProgramObject);
    glDetachShader(gShaderProgramObject, GL_FRAGMENT_SHADER);
    glDetachShader(gShaderProgramObject, GL_VERTEX_SHADER);
    glDeleteShader(gFragmentShaderObject);
    gFragmentShaderObject = 0;
    glDeleteShader(gVertexShaderObject);
    gVertexShaderObject = 0;
    glDeleteProgram(gShaderProgramObject);
    gShaderProgramObject = 0;
    glUseProgram(0);
    
    if (gpFile)
    {
        fprintf(gpFile, "Log File is Closed\n");
        fclose(gpFile);
        gpFile = NULL;
    }
    
    
    CVDisplayLinkStop(displayLink);
    CVDisplayLinkRelease(displayLink);
    
    
    
    [super dealloc];
}

@end

CVReturn MyDisplayLinkCallback(CVDisplayLinkRef displayLink, const CVTimeStamp *pNow, const CVTimeStamp *pOutputTime,CVOptionFlags flagsIn,CVOptionFlags *pFlagsOut, void *pDisplayLinkContext)
{
    CVReturn result=[(GLView *)pDisplayLinkContext getFrameForTime:pOutputTime];
    return(result);
}

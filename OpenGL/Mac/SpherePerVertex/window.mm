
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
    [window setTitle:@"PV Light"];
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
    
    GLuint modelMatrixUniform;
    GLuint viewMatrixUniform;
    GLuint projectionMatrixUniform;
    
    GLuint isLKeyIsPressedUniforms;
    
    GLuint laUniform;
    GLuint ldUniform;
    GLuint lsUniform;
    
    GLuint kaUniform;
    GLuint kdUniform;
    GLuint ksUniform;
    
    GLuint shininessUniform;
    
    GLuint lightPositionUniform;
    
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
    
    /////////
    //
    float lightAmbient[4] ;
    float lightDifuse[4] ;
    float lightSpecular[4];
    
    //material array
    float materialAmbient[4];
    float materialDifuse[4] ;
    float materialSpecular[4];
    
    float lightPosition[4] ;
    
    float materialShininess ;
    
    
}

-(id)initWithFrame:(NSRect)frame;
{
    
    //
    /////////////////////////////////////////////////////////////
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
    
    ///////
    lightAmbient[0] = 0.0f;
    lightAmbient[1] = 0.0f;
    lightAmbient[2] = 0.0f;
    lightAmbient[3] = 0.0f;
    
    lightDifuse[0] = 1.0f;
    lightDifuse[1] = 1.0f;
    lightDifuse[2] = 1.0f;
    lightDifuse[3] = 1.0f;
    
    lightSpecular[0] = 1.0f;
    lightSpecular[1] = 1.0f;
    lightSpecular[2] = 1.0f;
    lightSpecular[3] = 1.0f;
    
    //material array
    
    materialAmbient[0] = 0.0f;
    materialAmbient[1] = 0.0f;
    materialAmbient[2] = 0.0f;
    materialAmbient[3] = 0.0f;
    
    materialDifuse[0] = 1.0f;
    materialDifuse[1] = 1.0f;
    materialDifuse[2] = 1.0f;
    materialDifuse[3] = 1.0f;
    
    materialSpecular[0] = 1.0f;
    materialSpecular[1] = 1.0f;
    materialSpecular[2] = 1.0f;
    materialSpecular[3] = 1.0f;
    
    lightPosition[0] = 100.0f;
    lightPosition[1] = 100.0f;
    lightPosition[2] = 100.0f;
    lightPosition[3] = 1.0f;
    
    materialShininess = 50.0f;
    
    
    
    
    
    
    ///////
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
    "uniform mat4 u_model_matrix;" \
    "uniform mat4 u_view_matrix;" \
    "uniform mat4 u_projection_matrix;" \
    "uniform int islkeypressed;" \
    "uniform vec3 u_la;" \
    "uniform vec3 u_ld;" \
    "uniform vec3 u_ls;" \
    "uniform vec3 u_ka;" \
    "uniform vec3 u_kd;" \
    "uniform vec3 u_ks;" \
    "uniform float u_shininess;" \
    "uniform vec4 u_light_position;" \
    "out vec3 phong_ads_light;" \
    "void main(void)" \
    "{" \
    "if(islkeypressed == 1)" \
    "{" \
    "vec4 eye_coordinates = u_view_matrix *  u_model_matrix  * vPosition;" \
    "vec3 tnorm = normalize(mat3(u_view_matrix * u_model_matrix) * vNormal);" \
    "vec3 light_direction = normalize(vec3(u_light_position - eye_coordinates));" \
    "float tn_dot_ldirection = max(dot(light_direction, tnorm), 0);" \
    "vec3 reflection_vector = reflect(-light_direction, tnorm);" \
    "vec3 viewer_vector = normalize(vec3(-eye_coordinates));" \
    "vec3 ambient = u_la * u_ka;" \
    "vec3 difuse = u_ld * u_kd * tn_dot_ldirection;" \
    "vec3 specular = u_ls * u_ks * pow(max(dot(reflection_vector,viewer_vector),0),u_shininess);" \
    "phong_ads_light = ambient + difuse + specular;" \
    "}" \
    "else" \
    "{" \
    "phong_ads_light = vec3(1.0, 1.0, 1.0);" \
    "}" \
    "gl_Position = u_projection_matrix * u_view_matrix * u_model_matrix * vPosition;" \
    " } ";
    
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
    "out vec4 fragColor;" \
    "uniform int islkeypressed;" \
    "in vec3 phong_ads_light;" \
    "void main(void)" \
    "{" \
    "if(islkeypressed == 1)" \
    "{" \
    "fragColor = vec4(phong_ads_light, 1.0);" \
    "}" \
    "else" \
    "{" \
    "fragColor = vec4(phong_ads_light, 1.0);" \
    "}" \
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
    
    //////////////////////////////post linking retrieving uniform location///////////////////////////
    
    isLKeyIsPressedUniforms = glGetUniformLocation(gShaderProgramObject, "islkeypressed");
    
    laUniform = glGetUniformLocation(gShaderProgramObject, "u_la");
    ldUniform = glGetUniformLocation(gShaderProgramObject, "u_ld");
    lsUniform = glGetUniformLocation(gShaderProgramObject, "u_ls");
    
    kaUniform = glGetUniformLocation(gShaderProgramObject, "u_ka");
    kdUniform = glGetUniformLocation(gShaderProgramObject, "u_kd");
    ksUniform = glGetUniformLocation(gShaderProgramObject, "u_ks");
    
    shininessUniform = glGetUniformLocation(gShaderProgramObject, "u_shininess");
    
    lightPositionUniform = glGetUniformLocation(gShaderProgramObject, "u_light_position");
    
    //Matrix
    
    modelMatrixUniform = glGetUniformLocation(gShaderProgramObject, "u_model_matrix");
    
    viewMatrixUniform = glGetUniformLocation(gShaderProgramObject, "u_view_matrix");
    
    projectionMatrixUniform = glGetUniformLocation(gShaderProgramObject, "u_projection_matrix");
    
    ////////////////////////////////VAO ANS VBO////////////////////////////////////////
    
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
    vmath::mat4 modelMatrix;
    vmath::mat4 viewMatrix;
    
    modelMatrix = vmath::mat4::identity();
    viewMatrix = vmath::mat4::identity();
    translationMatrix = vmath::mat4::identity();
    rotationMatrix = vmath::mat4::identity();
    
    translationMatrix = vmath::translate(0.0f, 0.0f, -3.0f);
    
    modelMatrix = translationMatrix;
    
    if (gbIsLighting == true)
    {
        glUniform1i(isLKeyIsPressedUniforms, 1);
        
        glUniform1f(shininessUniform, materialShininess);
        
        glUniform3fv(laUniform, 1, lightAmbient);
        glUniform3fv(ldUniform, 1, lightDifuse);
        glUniform3fv(lsUniform, 1, lightSpecular);
        
        glUniform3fv(kaUniform, 1, materialAmbient);
        glUniform3fv(kdUniform, 1, materialDifuse);
        glUniform3fv(ksUniform, 1, materialSpecular);
        
        glUniform4fv(lightPositionUniform, 1, lightPosition);
    }
    else
    {
        glUniform1i(isLKeyIsPressedUniforms, 0);
    }
    
    glUniformMatrix4fv(modelMatrixUniform,
                       1,
                       GL_FALSE,
                       modelMatrix
                       );
    
    glUniformMatrix4fv(viewMatrixUniform,
                       1,
                       GL_FALSE,
                       viewMatrix
                       );
    
    glUniformMatrix4fv(projectionMatrixUniform,
                       1,
                       GL_FALSE,
                       perspectiveProjectionMatrix);
    
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

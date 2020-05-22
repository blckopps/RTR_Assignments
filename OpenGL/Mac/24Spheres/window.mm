


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
    [window setTitle:@"24 SPHERES"];
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
    
    GLuint laUniform_Red;
    
    GLuint ldUniform_Red;
    
    GLuint lsUniform_Red;
    
    GLuint kaUniform;
    GLuint kdUniform;
    GLuint ksUniform;
    
    GLuint shininessUniform;
    
    GLuint lightPositionUniform_Red;
    
    GLuint shaderSwitchUniform;
    ///////////////////////
    vmath::mat4 perspectiveProjectionMatrix;
    vmath::mat4 modelViewMatrix;
    
    GLfloat angle;
    int rotation ;
    GLfloat gHeight, gWidth;
    bool gbIsLighting;
    bool gbIsAnimation;
    bool perVertex;
    bool perFragmnet;
    
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
    float lightAmbient_Red[4] ;
    float lightDifuse_Red[4] ;
    float lightSpecular_Red[4];
    
    //material array
    float materialAmbient[4];
    float materialDifuse[4] ;
    float materialSpecular[4];
    
    float lightPosition_Red[4] ;
    
    float materialShininess ;
    
}

-(id)initWithFrame:(NSRect)frame;
{
    
    
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
    
    angle = 0.0f;
    rotation = 0;
    gbIsLighting = false;
    gbIsAnimation = false;
    perVertex = true;
    perFragmnet = false;
    
    ///////
    ///////Ambient/////////////
    lightAmbient_Red[0] = 0.0f;
    lightAmbient_Red[1] = 0.0f;
    lightAmbient_Red[2] = 0.0f;
    lightAmbient_Red[3] = 0.0f;
    
    
    ///////////////Diffuse/////////////
    lightDifuse_Red[0] = 1.0f;
    lightDifuse_Red[1] = 1.0f;
    lightDifuse_Red[2] = 1.0f;
    lightDifuse_Red[3] = 1.0f;
    
    ////////Specular/////////////////
    lightSpecular_Red[0] = 1.0f;
    lightSpecular_Red[1] = 1.0f;
    lightSpecular_Red[2] = 1.0f;
    lightSpecular_Red[3] = 1.0f;
    
    
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
    
    lightPosition_Red[0] = 100.0f;
    
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
    "uniform int shaderSwitch;" \
    
    "uniform vec3 u_la_Red;" \
    "uniform vec3 u_ld_Red;" \
    "uniform vec3 u_ls_Red;" \
    
    "uniform vec3 u_ka;" \
    "uniform vec3 u_kd;" \
    "uniform vec3 u_ks;" \
    
    "uniform float u_shininess;" \
    
    "uniform vec4 u_light_position_Red;" \
    
    "out vec3 phong_ads_light;" \
    
    "out vec3 tnorm;" \
    
    "out vec3 light_direction_Red;" \
    
    "out vec3 viewer_vector;" \
    "void main(void)" \
    "{" \
    "if(islkeypressed == 1)" \
    "{" \
    "if(shaderSwitch == 1)" \
    "{" \
    "vec4 eye_coordinates = u_view_matrix *  u_model_matrix  * vPosition;" \
    
    "vec3 tnorm = normalize(mat3(u_view_matrix * u_model_matrix) * vNormal);" \
    
    "vec3 light_direction_Red = normalize(vec3(u_light_position_Red - eye_coordinates));" \
    
    "float tn_dot_ldirection_Red  = max(dot(light_direction_Red , tnorm), 0);" \
    
    "vec3 reflection_vector_Red  = reflect(-light_direction_Red , tnorm);" \
    
    "vec3 viewer_vector = normalize(vec3(-eye_coordinates));" \
    
    "vec3 ambient_Red   = u_la_Red * u_ka;" \
    
    "vec3 difuse_Red   = u_ld_Red   * u_kd * tn_dot_ldirection_Red  ;" \
    
    "vec3 specular_Red   = u_ls_Red   * u_ks * pow(max(dot(reflection_vector_Red  ,viewer_vector),0),u_shininess);" \
    
    "vec3 redLight = ambient_Red + difuse_Red + specular_Red;" \
    
    "phong_ads_light = redLight;" \
    "}" \
    "else" \
    "{" \
    "vec4 eye_coordinates = u_view_matrix *  u_model_matrix  * vPosition;" \
    "tnorm = mat3(u_view_matrix * u_model_matrix) * vNormal;" \
    "light_direction_Red = vec3(u_light_position_Red - eye_coordinates);" \
    "viewer_vector = vec3(-eye_coordinates.xyz);" \
    "}"\
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
    "uniform int shaderSwitch;" \
    //
    "uniform vec3 u_la_Red;" \
    "uniform vec3 u_ld_Red;" \
    "uniform vec3 u_ls_Red;" \
    
    //material
    "uniform vec3 u_ka;" \
    "uniform vec3 u_kd;" \
    "uniform vec3 u_ks;" \
    "uniform float u_shininess;" \
    
    
    "in vec3 phong_ads_light;" \
    "in vec3 tnorm;" \
    
    "in vec3 light_direction_Red;" \
    
    "in vec3 viewer_vector;" \
    
    "void main(void)" \
    "{" \
    "if(islkeypressed == 1)" \
    "{" \
    
    "if(shaderSwitch == 1)" \
    "{" \
    "fragColor = vec4(phong_ads_light, 1.0);" \
    "}" \
    "else" \
    "{" \
    "vec3 tnorm_normalized = normalize(tnorm);" \
    
    "vec3 light_direction_normalized_Red   = normalize(light_direction_Red);" \
    
    "vec3 viewer_vector_normalized = normalize(viewer_vector);" \
    
    "float tn_dot_ldirection_Red   = max(dot(light_direction_normalized_Red  , tnorm_normalized), 0);" \
    
    "vec3 reflection_vector_Red   = reflect(-light_direction_normalized_Red  , tnorm_normalized);" \
    
    "vec3 ambient_Red   = u_la_Red   * u_ka;" \
    
    "vec3 difuse_Red   = u_ld_Red   * u_kd * tn_dot_ldirection_Red  ;" \
    
    "vec3 specular_Red   = u_ls_Red   * u_ks * pow(max(dot(reflection_vector_Red  ,viewer_vector_normalized),0),u_shininess);" \
    
    "vec3 redLight = ambient_Red + difuse_Red + specular_Red;" \
    
    "fragColor = vec4(redLight ,1.0);" \
    "}" \
    
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
    
    shaderSwitchUniform = glGetUniformLocation(gShaderProgramObject,"shaderSwitch" );
    
    laUniform_Red    = glGetUniformLocation(gShaderProgramObject, "u_la_Red");
    
    ldUniform_Red   = glGetUniformLocation(gShaderProgramObject, "u_ld_Red");
    
    lsUniform_Red     = glGetUniformLocation(gShaderProgramObject, "u_ls_Red");
    
    kaUniform = glGetUniformLocation(gShaderProgramObject, "u_ka");
    kdUniform = glGetUniformLocation(gShaderProgramObject, "u_kd");
    ksUniform = glGetUniformLocation(gShaderProgramObject, "u_ks");
    
    shininessUniform = glGetUniformLocation(gShaderProgramObject, "u_shininess");
    
    lightPositionUniform_Red   = glGetUniformLocation(gShaderProgramObject, "u_light_position_Red");
    
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
    
    gHeight = height;
    gWidth = width;
    
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
        if(perVertex)
        {
            glUniform1i(shaderSwitchUniform, 1);
        }
        else
        {
            glUniform1i(shaderSwitchUniform, 0);
        }
        
        glUniform1i(isLKeyIsPressedUniforms, 1);
        
        //glUniform1f(shininessUniform, materialShininess);
        
        glUniform3fv(laUniform_Red, 1, lightAmbient_Red);
        glUniform3fv(ldUniform_Red, 1, lightDifuse_Red);
        glUniform3fv(lsUniform_Red, 1, lightSpecular_Red);
        
        
        // glUniform3fv(kaUniform, 1, materialAmbient);
        // glUniform3fv(kdUniform, 1, materialDifuse);
        // glUniform3fv(ksUniform, 1, materialSpecular);
        
        glUniform4fv(lightPositionUniform_Red, 1, lightPosition_Red);
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
    
    GLuint xPos = 0;
    GLuint yPos = gHeight - gHeight / 6;
    int i = 1;
    /////////////Draw Sphers/////////////////////////
    
    
    
    
    materialAmbient[0] = 0.0215f;
    materialAmbient[1] = 0.1745f;
    materialAmbient[2] = 0.0215f;
    materialAmbient[3] = 1.0f  ;
    
    materialDifuse[0] = 0.07568f;
    materialDifuse[1] = 0.61424f;
    materialDifuse[2] = 0.07568f;
    materialDifuse[3] = 1.0f;
    
    materialSpecular[0] = 0.633f;
    materialSpecular[1] = 0.727811f;
    materialSpecular[2] = 0.633f;
    materialSpecular[3] = 1.0f;
    
    materialShininess = 0.6f*128.0f;
    
    glViewport(xPos, yPos, (GLsizei)gWidth / 4, (GLsizei)gHeight / 6);
    
    
    
    if (gbIsLighting == true)
    {
        
        glUniform1f(shininessUniform, materialShininess);
        
        
        glUniform3fv(kaUniform, 1, materialAmbient);
        glUniform3fv(kdUniform, 1, materialDifuse);
        glUniform3fv(ksUniform, 1, materialSpecular);
        
        
    }
    else
    {
        glUniform1i(isLKeyIsPressedUniforms, 0);
    }
    
    
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_sphere_element);
    glDrawElements(GL_TRIANGLES, numElements, GL_UNSIGNED_SHORT, 0);
    
    
    //1th        2nd on first col       ->
    
    
    if (i % 6 == 0)
    {
        xPos += gWidth / 4;
        yPos = gHeight - gHeight / 6;
    }
    else
    {
        yPos -= gHeight / 6;
    }
    i++;
    
    materialAmbient[0] = 0.135f;
    materialAmbient[1] = 0.2225f;
    materialAmbient[2] = 0.1575f;
    materialAmbient[3] = 1.0f;
    
    materialDifuse[0] = 0.54f;
    materialDifuse[1] = 0.89f;
    materialDifuse[2] = 0.63f;
    materialDifuse[3] = 1.0f;
    
    materialSpecular[0] = 0.316228f;
    materialSpecular[1] = 0.316228f;
    materialSpecular[2] = 0.316228f;
    materialSpecular[3] = 1.0f;
    
    materialShininess = 0.1f *128.0f;
    
    glViewport(xPos, yPos, (GLsizei)gWidth / 4, (GLsizei)gHeight / 6);
    
    if (gbIsLighting == true)
    {
        
        glUniform1f(shininessUniform, materialShininess);
        
        
        glUniform3fv(kaUniform, 1, materialAmbient);
        glUniform3fv(kdUniform, 1, materialDifuse);
        glUniform3fv(ksUniform, 1, materialSpecular);
        
        
    }
    else
    {
        glUniform1i(isLKeyIsPressedUniforms, 0);
    }
    
    
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_sphere_element);
    glDrawElements(GL_TRIANGLES, numElements, GL_UNSIGNED_SHORT, 0);
    
    
    
    //2th               ->
    if (i % 6 == 0)
    {
        xPos += gWidth / 4;
        yPos = gHeight - gHeight / 6;
    }
    else
    {
        yPos -= gHeight / 6;
    }
    i++;
    
    materialAmbient[0] = 0.05375f;
    materialAmbient[1] = 0.05f;
    materialAmbient[2] = 0.06625f;
    materialAmbient[3] = 1.0f;
    
    materialDifuse[0] = 0.18275f;
    materialDifuse[1] = 0.17f;
    materialDifuse[2] = 0.22525f;
    materialDifuse[3] = 1.0f;
    
    materialSpecular[0] = 0.332741f;
    materialSpecular[1] = 0.328634f;
    materialSpecular[2] = 0.346435f;
    materialSpecular[3] = 1.0f;
    
    materialShininess = 0.3f*128.0f;
    
    glViewport(xPos, yPos, (GLsizei)gWidth / 4, (GLsizei)gHeight / 6);
    
    if (gbIsLighting == true)
    {
        
        glUniform1f(shininessUniform, materialShininess);
        
        
        glUniform3fv(kaUniform, 1, materialAmbient);
        glUniform3fv(kdUniform, 1, materialDifuse);
        glUniform3fv(ksUniform, 1, materialSpecular);
        
        
    }
    else
    {
        glUniform1i(isLKeyIsPressedUniforms, 0);
    }
    
    
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_sphere_element);
    glDrawElements(GL_TRIANGLES, numElements, GL_UNSIGNED_SHORT, 0);
    
    
    
    //3th        4th on 1st colmn       ->
    if (i % 6 == 0)
    {
        xPos += gWidth / 4;
        yPos = gHeight - gHeight / 6;
    }
    else
    {
        yPos -= gHeight / 6;
    }
    i++;
    
    materialAmbient[0] = 0.25f;
    materialAmbient[1] = 0.20725f;
    materialAmbient[2] = 0.20725f;
    materialAmbient[3] = 1.0f;
    
    materialDifuse[0] = 1.0f;
    materialDifuse[1] = 0.829f;
    materialDifuse[2] = 0.829f;
    materialDifuse[3] = 1.0f;
    
    materialSpecular[0] = 0.296648f;
    materialSpecular[1] = 0.296648f;
    materialSpecular[2] = 0.296648f;
    materialSpecular[3] = 1.0f;
    
    materialShininess = 0.88f*128.0f;
    
    glViewport(xPos, yPos, (GLsizei)gWidth / 4, (GLsizei)gHeight / 6);
    
    if (gbIsLighting == true)
    {
        
        glUniform1f(shininessUniform, materialShininess);
        
        
        glUniform3fv(kaUniform, 1, materialAmbient);
        glUniform3fv(kdUniform, 1, materialDifuse);
        glUniform3fv(ksUniform, 1, materialSpecular);
        
        
    }
    else
    {
        glUniform1i(isLKeyIsPressedUniforms, 0);
    }
    
    
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_sphere_element);
    glDrawElements(GL_TRIANGLES, numElements, GL_UNSIGNED_SHORT, 0);
    
    
    
    //4th               5th on 1st colmn->
    
    if (i % 6 == 0)
    {
        xPos += gWidth / 4;
        yPos = gHeight - gHeight / 6;
    }
    else
    {
        yPos -= gHeight / 6;
    }
    i++;
    
    materialAmbient[0] = 0.1745f;
    materialAmbient[1] = 0.01175f;
    materialAmbient[2] = 0.01175f;
    materialAmbient[3] = 1.0f;
    
    materialDifuse[0] = 0.61424f;
    materialDifuse[1] = 0.04136f;
    materialDifuse[2] = 0.04136f;
    materialDifuse[3] = 1.0f;
    
    materialSpecular[0] = 0.727811f;
    materialSpecular[1] = 0.626959f;
    materialSpecular[2] = 0.626959f;
    materialSpecular[3] = 1.0f;
    
    materialShininess = 0.6f * 128.0f;
    
    glViewport(xPos, yPos, (GLsizei)gWidth / 4, (GLsizei)gHeight / 6);
    
    if (gbIsLighting == true)
    {
        
        glUniform1f(shininessUniform, materialShininess);
        
        
        glUniform3fv(kaUniform, 1, materialAmbient);
        glUniform3fv(kdUniform, 1, materialDifuse);
        glUniform3fv(ksUniform, 1, materialSpecular);
        
        
    }
    else
    {
        glUniform1i(isLKeyIsPressedUniforms, 0);
    }
    
    
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_sphere_element);
    glDrawElements(GL_TRIANGLES, numElements, GL_UNSIGNED_SHORT, 0);
    
    
    //5th    6th on 1st col           ->
    
    if (i % 6 == 0)
    {
        xPos += gWidth / 4;
        yPos = gHeight - gHeight / 6;
    }
    else
    {
        yPos -= gHeight / 6;
    }
    i++;
    
    materialAmbient[0] = 0.1f;
    materialAmbient[1] = 0.18725f;
    materialAmbient[2] = 0.1745f;
    materialAmbient[3] = 1.0f;
    
    materialDifuse[0] = 0.396f;
    materialDifuse[1] = 0.74151f;
    materialDifuse[2] = 0.69102f;
    materialDifuse[3] = 1.0f;
    
    materialSpecular[0] = 0.297254f;
    materialSpecular[1] = 0.30829f;
    materialSpecular[2] = 0.30829f;
    materialSpecular[3] = 1.0f;
    
    materialShininess = 0.1f*128.0f;
    
    glViewport(xPos, yPos, (GLsizei)gWidth / 4, (GLsizei)gHeight / 6);
    
    if (gbIsLighting == true)
    {
        
        glUniform1f(shininessUniform, materialShininess);
        
        
        glUniform3fv(kaUniform, 1, materialAmbient);
        glUniform3fv(kdUniform, 1, materialDifuse);
        glUniform3fv(ksUniform, 1, materialSpecular);
        
        
    }
    else
    {
        glUniform1i(isLKeyIsPressedUniforms, 0);
    }
    
    
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_sphere_element);
    glDrawElements(GL_TRIANGLES, numElements, GL_UNSIGNED_SHORT, 0);
    
    
    ////6th            1st on 2nd colmn   ->
    if (i % 6 == 0)
    {
        xPos += gWidth / 4;
        yPos = gHeight - gHeight / 6;
    }
    else
    {
        yPos -= gHeight / 6;
    }
    i++;
    
    materialAmbient[0] = 0.329412f;
    materialAmbient[1] = 0.223529f;
    materialAmbient[2] = 0.027451f;
    materialAmbient[3] = 1.0f;
    
    materialDifuse[0] = 0.780392f;
    materialDifuse[1] = 0.568627f;
    materialDifuse[2] = 0.113725f;
    materialDifuse[3] = 1.0f;
    
    materialSpecular[0] = 0.992157f;
    materialSpecular[1] = 0.941176f;
    materialSpecular[2] = 0.807843f;
    materialSpecular[3] = 1.0f;
    
    materialShininess = 0.21794872f*128.0f;
    
    
    glViewport(xPos, yPos, (GLsizei)gWidth / 4, (GLsizei)gHeight / 6);
    
    if (gbIsLighting == true)
    {
        
        glUniform1f(shininessUniform, materialShininess);
        
        
        glUniform3fv(kaUniform, 1, materialAmbient);
        glUniform3fv(kdUniform, 1, materialDifuse);
        glUniform3fv(ksUniform, 1, materialSpecular);
        
        
    }
    else
    {
        glUniform1i(isLKeyIsPressedUniforms, 0);
    }
    
    
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_sphere_element);
    glDrawElements(GL_TRIANGLES, numElements, GL_UNSIGNED_SHORT, 0);
    
    
    //7th 2nd on 2nd colm  ->
    if (i % 6 == 0)
    {
        xPos += gWidth / 4;
        yPos = gHeight - gHeight / 6;
    }
    else
    {
        yPos -= gHeight / 6;
    }
    i++;
    
    materialAmbient[0] = 0.2125f;
    materialAmbient[1] = 0.1275f;
    materialAmbient[2] = 0.054f;
    materialAmbient[3] = 1.0f;
    
    materialDifuse[0] = 0.714f;
    materialDifuse[1] = 0.4284f;
    materialDifuse[2] = 0.18144f;
    materialDifuse[3] = 1.0f;
    
    materialSpecular[0] = 0.393548f;
    materialSpecular[1] = 0.271906f;
    materialSpecular[2] = 0.166721f;
    materialSpecular[3] = 1.0f;
    
    materialShininess = 0.2f*128.0f;
    
    glViewport(xPos, yPos, (GLsizei)gWidth / 4, (GLsizei)gHeight / 6);
    
    if (gbIsLighting == true)
    {
        
        glUniform1f(shininessUniform, materialShininess);
        
        
        glUniform3fv(kaUniform, 1, materialAmbient);
        glUniform3fv(kdUniform, 1, materialDifuse);
        glUniform3fv(ksUniform, 1, materialSpecular);
        
        
    }
    else
    {
        glUniform1i(isLKeyIsPressedUniforms, 0);
    }
    
    
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_sphere_element);
    glDrawElements(GL_TRIANGLES, numElements, GL_UNSIGNED_SHORT, 0);
    
    
    //8th   3rd on 2nd colm->
    if (i % 6 == 0)
    {
        xPos += gWidth / 4;
        yPos = gHeight - gHeight / 6;
    }
    else
    {
        yPos -= gHeight / 6;
    }
    i++;
    
    materialAmbient[0] = 0.25f;
    materialAmbient[1] = 0.25f;
    materialAmbient[2] = 0.25f;
    materialAmbient[3] = 1.0f;
    
    materialDifuse[0] = 0.4f;
    materialDifuse[1] = 0.4f;
    materialDifuse[2] = 0.4f;
    materialDifuse[3] = 1.0f;
    
    materialSpecular[0] = 0.774597f;
    materialSpecular[1] = 0.774597f;
    materialSpecular[2] = 0.774597f;
    materialSpecular[3] = 1.0f;
    
    materialShininess = 0.6f*128.0f;
    
    glViewport(xPos, yPos, (GLsizei)gWidth / 4, (GLsizei)gHeight / 6);
    
    if (gbIsLighting == true)
    {
        
        glUniform1f(shininessUniform, materialShininess);
        
        
        glUniform3fv(kaUniform, 1, materialAmbient);
        glUniform3fv(kdUniform, 1, materialDifuse);
        glUniform3fv(ksUniform, 1, materialSpecular);
        
        
    }
    else
    {
        glUniform1i(isLKeyIsPressedUniforms, 0);
    }
    
    
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_sphere_element);
    glDrawElements(GL_TRIANGLES, numElements, GL_UNSIGNED_SHORT, 0);
    
    
    //9th 2nd col 4th sphre
    if (i % 6 == 0)
    {
        xPos += gWidth / 4;
        yPos = gHeight - gHeight / 6;
    }
    else
    {
        yPos -= gHeight / 6;
    }
    i++;
    
    materialAmbient[0] = 0.19125f;
    materialAmbient[1] = 0.0735f;
    materialAmbient[2] = 0.0225f;
    materialAmbient[3] = 1.0f;
    
    materialDifuse[0] = 0.7038f;
    materialDifuse[1] = 0.27048f;
    materialDifuse[2] = 0.0828f;
    materialDifuse[3] = 1.0f;
    
    materialSpecular[0] = 0.256777f;
    materialSpecular[1] = 0.137622f;
    materialSpecular[2] = 0.086014f;
    materialSpecular[3] = 1.0f;
    
    materialShininess = 0.1f*128.0f;
    
    glViewport(xPos, yPos, (GLsizei)gWidth / 4, (GLsizei)gHeight / 6);
    
    if (gbIsLighting == true)
    {
        
        glUniform1f(shininessUniform, materialShininess);
        
        
        glUniform3fv(kaUniform, 1, materialAmbient);
        glUniform3fv(kdUniform, 1, materialDifuse);
        glUniform3fv(ksUniform, 1, materialSpecular);
        
        
    }
    else
    {
        glUniform1i(isLKeyIsPressedUniforms, 0);
    }
    
    
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_sphere_element);
    glDrawElements(GL_TRIANGLES, numElements, GL_UNSIGNED_SHORT, 0);
    
    
    //10th 2nd col 5th sphere
    if (i % 6 == 0)
    {
        xPos += gWidth / 4;
        yPos = gHeight - gHeight / 6;
    }
    else
    {
        yPos -= gHeight / 6;
    }
    i++;
    
    materialAmbient[0] = 0.24725f;
    materialAmbient[1] = 0.1995f;
    materialAmbient[2] = 0.0745f;
    materialAmbient[3] = 1.0f;
    
    materialDifuse[0] = 0.75164f;
    materialDifuse[1] = 0.60648f;
    materialDifuse[2] = 0.22648f;
    materialDifuse[3] = 1.0f;
    
    materialSpecular[0] = 0.628281f;
    materialSpecular[1] = 0.555802f;
    materialSpecular[2] = 0.366065f;
    materialSpecular[3] = 1.0f;
    
    materialShininess = 0.4f*128.0f;
    
    glViewport(xPos, yPos, (GLsizei)gWidth / 4, (GLsizei)gHeight / 6);
    
    if (gbIsLighting == true)
    {
        
        glUniform1f(shininessUniform, materialShininess);
        
        
        glUniform3fv(kaUniform, 1, materialAmbient);
        glUniform3fv(kdUniform, 1, materialDifuse);
        glUniform3fv(ksUniform, 1, materialSpecular);
        
        
    }
    else
    {
        glUniform1i(isLKeyIsPressedUniforms, 0);
    }
    
    
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_sphere_element);
    glDrawElements(GL_TRIANGLES, numElements, GL_UNSIGNED_SHORT, 0);
    
    
    //11th 2nd col 6th sphere
    if (i % 6 == 0)
    {
        xPos += gWidth / 4;
        yPos = gHeight - gHeight / 6;
    }
    else
    {
        yPos -= gHeight / 6;
    }
    i++;
    
    materialAmbient[0] = 0.19225f;
    materialAmbient[1] = 0.19225f;
    materialAmbient[2] = 0.19225f;
    materialAmbient[3] = 1.0f;
    
    materialDifuse[0] = 0.50754f;
    materialDifuse[1] = 0.50754f;
    materialDifuse[2] = 0.50754f;
    materialDifuse[3] = 1.0f;
    
    materialSpecular[0] = 0.508273f;
    materialSpecular[1] = 0.508273f;
    materialSpecular[2] = 0.508273f;
    materialSpecular[3] = 1.0f;
    
    materialShininess = 0.4f*128.0f;
    
    glViewport(xPos, yPos, (GLsizei)gWidth / 4, (GLsizei)gHeight / 6);
    
    if (gbIsLighting == true)
    {
        
        glUniform1f(shininessUniform, materialShininess);
        
        
        glUniform3fv(kaUniform, 1, materialAmbient);
        glUniform3fv(kdUniform, 1, materialDifuse);
        glUniform3fv(ksUniform, 1, materialSpecular);
        
        
    }
    else
    {
        glUniform1i(isLKeyIsPressedUniforms, 0);
    }
    
    
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_sphere_element);
    glDrawElements(GL_TRIANGLES, numElements, GL_UNSIGNED_SHORT, 0);
    
    
    
    //12th 3rd colm 1
    if (i % 6 == 0)
    {
        xPos += gWidth / 4;
        yPos = gHeight - gHeight / 6;
    }
    else
    {
        yPos -= gHeight / 6;
    }
    i++;
    
    materialAmbient[0] = 0.0f;
    materialAmbient[1] = 0.0f;
    materialAmbient[2] = 0.0f;
    materialAmbient[3] = 1.0f;
    
    materialDifuse[0] = 0.01f;
    materialDifuse[1] = 0.01f;
    materialDifuse[2] = 0.01f;
    materialDifuse[3] = 1.0f;
    
    materialSpecular[0] = 0.50f;
    materialSpecular[1] = 0.50f;
    materialSpecular[2] = 0.50f;
    materialSpecular[3] = 1.0f;
    
    materialShininess = 0.25f*128.0f;
    
    glViewport(xPos, yPos, (GLsizei)gWidth / 4, (GLsizei)gHeight / 6);
    
    if (gbIsLighting == true)
    {
        
        glUniform1f(shininessUniform, materialShininess);
        
        
        glUniform3fv(kaUniform, 1, materialAmbient);
        glUniform3fv(kdUniform, 1, materialDifuse);
        glUniform3fv(ksUniform, 1, materialSpecular);
        
        
    }
    else
    {
        glUniform1i(isLKeyIsPressedUniforms, 0);
    }
    
    
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_sphere_element);
    glDrawElements(GL_TRIANGLES, numElements, GL_UNSIGNED_SHORT, 0);
    
    
    //13th 3rd colm 2
    
    if (i % 6 == 0)
    {
        xPos += gWidth / 4;
        yPos = gHeight - gHeight / 6;
    }
    else
    {
        yPos -= gHeight / 6;
    }
    i++;
    
    materialAmbient[0] = 0.0f;
    materialAmbient[1] = 0.1f;
    materialAmbient[2] = 0.06f;
    materialAmbient[3] = 1.0f;
    
    materialDifuse[0] = 0.0f;
    materialDifuse[1] = 0.50980392f;
    materialDifuse[2] = 0.50980392f;
    materialDifuse[3] = 1.0f;
    
    materialSpecular[0] = 0.50196078f;
    materialSpecular[1] = 0.50196078f;
    materialSpecular[2] = 0.50196078f;
    materialSpecular[3] = 1.0f;
    
    materialShininess = 0.25f*128.0f;
    
    glViewport(xPos, yPos, (GLsizei)gWidth / 4, (GLsizei)gHeight / 6);
    
    if (gbIsLighting == true)
    {
        
        glUniform1f(shininessUniform, materialShininess);
        
        
        glUniform3fv(kaUniform, 1, materialAmbient);
        glUniform3fv(kdUniform, 1, materialDifuse);
        glUniform3fv(ksUniform, 1, materialSpecular);
        
        
    }
    else
    {
        glUniform1i(isLKeyIsPressedUniforms, 0);
    }
    
    
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_sphere_element);
    glDrawElements(GL_TRIANGLES, numElements, GL_UNSIGNED_SHORT, 0);
    
    
    //14th 3rd colm 3
    
    if (i % 6 == 0)
    {
        xPos += gWidth / 4;
        yPos = gHeight - gHeight / 6;
    }
    else
    {
        yPos -= gHeight / 6;
    }
    i++;
    
    
    materialAmbient[0] = 0.0f;
    materialAmbient[1] = 0.0f;
    materialAmbient[2] = 0.0f;
    materialAmbient[3] = 1.0f;
    
    materialDifuse[0] = 0.1f;
    materialDifuse[1] = 0.35f;
    materialDifuse[2] = 0.1f;
    materialDifuse[3] = 1.0f;
    
    materialSpecular[0] = 0.45f;
    materialSpecular[1] = 0.55f;
    materialSpecular[2] = 0.45f;
    materialSpecular[3] = 1.0f;
    
    materialShininess = 0.25f*128.0f;
    
    glViewport(xPos, yPos, (GLsizei)gWidth / 4, (GLsizei)gHeight / 6);
    
    if (gbIsLighting == true)
    {
        
        glUniform1f(shininessUniform, materialShininess);
        
        
        glUniform3fv(kaUniform, 1, materialAmbient);
        glUniform3fv(kdUniform, 1, materialDifuse);
        glUniform3fv(ksUniform, 1, materialSpecular);
        
        
    }
    else
    {
        glUniform1i(isLKeyIsPressedUniforms, 0);
    }
    
    
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_sphere_element);
    glDrawElements(GL_TRIANGLES, numElements, GL_UNSIGNED_SHORT, 0);
    
    
    //15th 3rd colm 4
    
    if (i % 6 == 0)
    {
        xPos += gWidth / 4;
        yPos = gHeight - gHeight / 6;
    }
    else
    {
        yPos -= gHeight / 6;
    }
    i++;
    
    
    materialAmbient[0] = 0.0f;
    materialAmbient[1] = 0.0f;
    materialAmbient[2] = 0.0f;
    materialAmbient[3] = 1.0f;
    
    materialDifuse[0] = 0.5f;
    materialDifuse[1] = 0.0f;
    materialDifuse[2] = 0.0f;
    materialDifuse[3] = 1.0f;
    
    materialSpecular[0] = 0.7f;
    materialSpecular[1] = 0.6f;
    materialSpecular[2] = 0.6f;
    materialSpecular[3] = 1.0f;
    
    materialShininess = 0.25f*128.0f;
    
    glViewport(xPos, yPos, (GLsizei)gWidth / 4, (GLsizei)gHeight / 6);
    
    if (gbIsLighting == true)
    {
        
        glUniform1f(shininessUniform, materialShininess);
        
        
        glUniform3fv(kaUniform, 1, materialAmbient);
        glUniform3fv(kdUniform, 1, materialDifuse);
        glUniform3fv(ksUniform, 1, materialSpecular);
        
        
    }
    else
    {
        glUniform1i(isLKeyIsPressedUniforms, 0);
    }
    
    
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_sphere_element);
    glDrawElements(GL_TRIANGLES, numElements, GL_UNSIGNED_SHORT, 0);
    
    
    //16th 3rd colm 5
    if (i % 6 == 0)
    {
        xPos += gWidth / 4;
        yPos = gHeight - gHeight / 6;
    }
    else
    {
        yPos -= gHeight / 6;
    }
    i++;
    
    materialAmbient[0] = 0.0f;
    materialAmbient[1] = 0.0f;
    materialAmbient[2] = 0.0f;
    materialAmbient[3] = 1.0f;
    
    materialDifuse[0] = 0.55f;
    materialDifuse[1] = 0.55f;
    materialDifuse[2] = 0.55f;
    materialDifuse[3] = 1.0f;
    
    materialSpecular[0] = 0.70f;
    materialSpecular[1] = 0.70f;
    materialSpecular[2] = 0.70f;
    materialSpecular[3] = 1.0f;
    
    materialShininess = 0.25f*128.0f;
    
    glViewport(xPos, yPos, (GLsizei)gWidth / 4, (GLsizei)gHeight / 6);
    
    if (gbIsLighting == true)
    {
        
        glUniform1f(shininessUniform, materialShininess);
        
        
        glUniform3fv(kaUniform, 1, materialAmbient);
        glUniform3fv(kdUniform, 1, materialDifuse);
        glUniform3fv(ksUniform, 1, materialSpecular);
        
        
    }
    else
    {
        glUniform1i(isLKeyIsPressedUniforms, 0);
    }
    
    
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_sphere_element);
    glDrawElements(GL_TRIANGLES, numElements, GL_UNSIGNED_SHORT, 0);
    
    
    
    //17th 3rd colm 6
    if (i % 6 == 0)
    {
        xPos += gWidth / 4;
        yPos = gHeight - gHeight / 6;
    }
    else
    {
        yPos -= gHeight / 6;
    }
    i++;
    
    materialAmbient[0] = 0.0f;
    materialAmbient[1] = 0.0f;
    materialAmbient[2] = 0.0f;
    materialAmbient[3] = 1.0f;
    
    materialDifuse[0] = 0.5f;
    materialDifuse[1] = 0.5f;
    materialDifuse[2] = 0.0f;
    materialDifuse[3] = 1.0f;
    
    materialSpecular[0] = 0.60f;
    materialSpecular[1] = 0.60f;
    materialSpecular[2] = 0.50f;
    materialSpecular[3] = 1.0f;
    
    materialShininess = 0.25f*128.0f;
    
    glViewport(xPos, yPos, (GLsizei)gWidth / 4, (GLsizei)gHeight / 6);
    
    if (gbIsLighting == true)
    {
        
        glUniform1f(shininessUniform, materialShininess);
        
        
        glUniform3fv(kaUniform, 1, materialAmbient);
        glUniform3fv(kdUniform, 1, materialDifuse);
        glUniform3fv(ksUniform, 1, materialSpecular);
        
        
    }
    else
    {
        glUniform1i(isLKeyIsPressedUniforms, 0);
    }
    
    
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_sphere_element);
    glDrawElements(GL_TRIANGLES, numElements, GL_UNSIGNED_SHORT, 0);
    
    
    //18th 4th colm 1
    
    if (i % 6 == 0)
    {
        xPos += gWidth / 4;
        yPos = gHeight - gHeight / 6;
    }
    else
    {
        yPos -= gHeight / 6;
    }
    i++;
    
    materialAmbient[0] = 0.02f;
    materialAmbient[1] = 0.02f;
    materialAmbient[2] = 0.02f;
    materialAmbient[3] = 1.0f;
    
    materialDifuse[0] = 0.01f;
    materialDifuse[1] = 0.01f;
    materialDifuse[2] = 0.01f;
    materialDifuse[3] = 1.0f;
    
    materialSpecular[0] = 0.4f;
    materialSpecular[1] = 0.4f;
    materialSpecular[2] = 0.4f;
    materialSpecular[3] = 1.0f;
    
    materialShininess = 0.078125f*128.0f;
    
    glViewport(xPos, yPos, (GLsizei)gWidth / 4, (GLsizei)gHeight / 6);
    
    if (gbIsLighting == true)
    {
        
        glUniform1f(shininessUniform, materialShininess);
        
        
        glUniform3fv(kaUniform, 1, materialAmbient);
        glUniform3fv(kdUniform, 1, materialDifuse);
        glUniform3fv(ksUniform, 1, materialSpecular);
        
        
    }
    else
    {
        glUniform1i(isLKeyIsPressedUniforms, 0);
    }
    
    
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_sphere_element);
    glDrawElements(GL_TRIANGLES, numElements, GL_UNSIGNED_SHORT, 0);
    
    
    //19th 4th colm 2
    if (i % 6 == 0)
    {
        xPos += gWidth / 4;
        yPos = gHeight - gHeight / 6;
    }
    else
    {
        yPos -= gHeight / 6;
    }
    i++;
    
    materialAmbient[0] = 0.0f;
    materialAmbient[1] = 0.05f;
    materialAmbient[2] = 0.05f;
    materialAmbient[3] = 1.0f;
    
    materialDifuse[0] = 0.4f;
    materialDifuse[1] = 0.5f;
    materialDifuse[2] = 0.5f;
    materialDifuse[3] = 1.0f;
    
    materialSpecular[0] = 0.04f;
    materialSpecular[1] = 0.7f;
    materialSpecular[2] = 0.7f;
    materialSpecular[3] = 1.0f;
    
    materialShininess = 0.078125f*128.0f;
    
    glViewport(xPos, yPos, (GLsizei)gWidth / 4, (GLsizei)gHeight / 6);
    
    if (gbIsLighting == true)
    {
        
        glUniform1f(shininessUniform, materialShininess);
        
        
        glUniform3fv(kaUniform, 1, materialAmbient);
        glUniform3fv(kdUniform, 1, materialDifuse);
        glUniform3fv(ksUniform, 1, materialSpecular);
        
        
    }
    else
    {
        glUniform1i(isLKeyIsPressedUniforms, 0);
    }
    
    
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_sphere_element);
    glDrawElements(GL_TRIANGLES, numElements, GL_UNSIGNED_SHORT, 0);
    
    
    //20th 4th col 3
    if (i % 6 == 0)
    {
        xPos += gWidth / 4;
        yPos = gHeight - gHeight / 6;
    }
    else
    {
        yPos -= gHeight / 6;
    }
    i++;
    
    materialAmbient[0] = 0.0f;
    materialAmbient[1] = 0.05f;
    materialAmbient[2] = 0.0f;
    materialAmbient[3] = 1.0f;
    
    materialDifuse[0] = 0.4f;
    materialDifuse[1] = 0.5f;
    materialDifuse[2] = 0.4f;
    materialDifuse[3] = 1.0f;
    
    materialSpecular[0] = 0.04f;
    materialSpecular[1] = 0.7f;
    materialSpecular[2] = 0.04f;
    materialSpecular[3] = 1.0f;
    
    materialShininess = 0.078125f*128.0f;
    
    glViewport(xPos, yPos, (GLsizei)gWidth / 4, (GLsizei)gHeight / 6);
    
    if (gbIsLighting == true)
    {
        
        glUniform1f(shininessUniform, materialShininess);
        
        
        glUniform3fv(kaUniform, 1, materialAmbient);
        glUniform3fv(kdUniform, 1, materialDifuse);
        glUniform3fv(ksUniform, 1, materialSpecular);
        
        
    }
    else
    {
        glUniform1i(isLKeyIsPressedUniforms, 0);
    }
    
    
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_sphere_element);
    glDrawElements(GL_TRIANGLES, numElements, GL_UNSIGNED_SHORT, 0);
    
    
    //21th 4th col 4
    if (i % 6 == 0)
    {
        xPos += gWidth / 4;
        yPos = gHeight - gHeight / 6;
    }
    else
    {
        yPos -= gHeight / 6;
    }
    i++;
    
    materialAmbient[0] = 0.05f;
    materialAmbient[1] = 0.0f;
    materialAmbient[2] = 0.0f;
    materialAmbient[3] = 1.0f;
    
    materialDifuse[0] = 0.5f;
    materialDifuse[1] = 0.4f;
    materialDifuse[2] = 0.4f;
    materialDifuse[3] = 1.0f;
    
    materialSpecular[0] = 0.7f;
    materialSpecular[1] = 0.04f;
    materialSpecular[2] = 0.04f;
    materialSpecular[3] = 1.0f;
    
    materialShininess = 0.078125f*128.0f;
    
    glViewport(xPos, yPos, (GLsizei)gWidth / 4, (GLsizei)gHeight / 6);
    
    if (gbIsLighting == true)
    {
        
        glUniform1f(shininessUniform, materialShininess);
        
        
        glUniform3fv(kaUniform, 1, materialAmbient);
        glUniform3fv(kdUniform, 1, materialDifuse);
        glUniform3fv(ksUniform, 1, materialSpecular);
        
        
    }
    else
    {
        glUniform1i(isLKeyIsPressedUniforms, 0);
    }
    
    
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_sphere_element);
    glDrawElements(GL_TRIANGLES, numElements, GL_UNSIGNED_SHORT, 0);
    
    
    //22th 4th col 5th
    if (i % 6 == 0)
    {
        xPos += gWidth / 4;
        yPos = gHeight - gHeight / 6;
    }
    else
    {
        yPos -= gHeight / 6;
    }
    i++;
    
    materialAmbient[0] = 0.5f;
    materialAmbient[1] = 0.5f;
    materialAmbient[2] = 0.5f;
    materialAmbient[3] = 1.0f;
    
    materialDifuse[0] = 0.5f;
    materialDifuse[1] = 0.5f;
    materialDifuse[2] = 0.5f;
    materialDifuse[3] = 1.0f;
    
    materialSpecular[0] = 0.7f;
    materialSpecular[1] = 0.7f;
    materialSpecular[2] = 0.7f;
    materialSpecular[3] = 1.0f;
    
    materialShininess = 0.078125f*128.0f;
    
    glViewport(xPos, yPos, (GLsizei)gWidth / 4, (GLsizei)gHeight / 6);
    
    if (gbIsLighting == true)
    {
        
        glUniform1f(shininessUniform, materialShininess);
        
        
        glUniform3fv(kaUniform, 1, materialAmbient);
        glUniform3fv(kdUniform, 1, materialDifuse);
        glUniform3fv(ksUniform, 1, materialSpecular);
        
        
    }
    else
    {
        glUniform1i(isLKeyIsPressedUniforms, 0);
    }
    
    
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_sphere_element);
    glDrawElements(GL_TRIANGLES, numElements, GL_UNSIGNED_SHORT, 0);
    
    
    //23th 4th col 6th
    if (i % 6 == 0)
    {
        xPos += gWidth / 4;
        yPos = gHeight - gHeight / 6;
    }
    else
    {
        yPos -= gHeight / 6;
    }
    i++;
    
    materialAmbient[0] = 0.05f;
    materialAmbient[1] = 0.05f;
    materialAmbient[2] = 0.0f;
    materialAmbient[3] = 1.0f;
    
    materialDifuse[0] = 0.5f;
    materialDifuse[1] = 0.5f;
    materialDifuse[2] = 0.4f;
    materialDifuse[3] = 1.0f;
    
    materialSpecular[0] = 0.7f;
    materialSpecular[1] = 0.7f;
    materialSpecular[2] = 0.7f;
    materialSpecular[3] = 1.0f;
    
    materialShininess = 0.078125f*128.0f;
    
    glViewport(xPos, yPos, (GLsizei)gWidth / 4, (GLsizei)gHeight / 6);
    
    if (gbIsLighting == true)
    {
        
        glUniform1f(shininessUniform, materialShininess);
        
        
        glUniform3fv(kaUniform, 1, materialAmbient);
        glUniform3fv(kdUniform, 1, materialDifuse);
        glUniform3fv(ksUniform, 1, materialSpecular);
        
        
    }
    else
    {
        glUniform1i(isLKeyIsPressedUniforms, 0);
    }
    
    
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_sphere_element);
    glDrawElements(GL_TRIANGLES, numElements, GL_UNSIGNED_SHORT, 0);
    
    
    
    
    glBindVertexArray(0);
    
    
    //unuse program
    glUseProgram(0);
    
    
    CGLFlushDrawable((CGLContextObj)[[self openGLContext]CGLContextObj]);
    
    if (gbIsAnimation == true)
    {
        angle += 0.01f;
        if (angle >= 360.0f)
            angle = 0;
        
        //X Rotation
        if(rotation == 0)
        {
            lightPosition_Red[0] = 0.0f;
            lightPosition_Red[1] = 100.0f*sin(angle);
            lightPosition_Red[2] = 100.0f *cos(angle);
            lightPosition_Red[3] = 1.0f;
        }
        // Y Rotation
        if(rotation == 1)
        {
            lightPosition_Red[0] = 100.0f*cos(angle);
            lightPosition_Red[1] = 0.0f;
            lightPosition_Red[2] = 100.0f*sin(angle);
            lightPosition_Red[3] = 1.0f;
        }
        // Z Rotation
        if(rotation ==2)
        {
            lightPosition_Red[0] = 100 * cos(angle);
            lightPosition_Red[1] = 100 * sin(angle);
            lightPosition_Red[2] = 0.0f;
            lightPosition_Red[3] = 1.0f;
        }
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
        case 'q':
        case 'Q':
            [self release];
            [NSApp terminate:self];
            break;
            
        case 27:        //escape
            [[self window]toggleFullScreen:self];
            break;
            
        case 'F':
        case 'f':
            perFragmnet = true;
            perVertex = false;
            break;
            
        case 'X':
        case 'x':
            rotation = 0;
            break;
            
        case 'y':
        case 'Y':
            rotation = 1;
            break;
            
        case 'Z':
        case 'z':
            rotation = 2;
            break;
            
        case 'v':
        case 'V':
            perFragmnet = false;
            perVertex = true;
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

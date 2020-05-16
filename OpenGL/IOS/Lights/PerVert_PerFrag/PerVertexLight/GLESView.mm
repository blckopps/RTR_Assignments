



#import <OpenGLES/ES3/gl.h>
#import <OpenGLES/ES3/glext.h>

#import "vmath.h"
#import "GLESView.h"
#import "Sphere.h"

enum
{
    AMC_ATTRIBUTES_POSITION=0,
    AMC_ATTRIBUTES_COLOR,
    AMC_ATTRIBUTES_NORMAL,
    AMC_ATTRIBUTES_TEXCOORD0,
};

@implementation GLESView
{
    EAGLContext *eaglContext;
    
    GLuint defaultFrameBuffer;
    GLuint colorRenderBuffer;
    GLuint depthRenderBuffer;
    
    id displayLink;
    NSInteger animationFrameInterval;
    BOOL isAnimating;
    
    GLuint gVertexShaderObject;
    GLuint gFragmentShaderObject;
    GLuint gShaderProgramObject;
    
    GLuint gVertexShaderObject_PF;
    GLuint gFragmentShaderObject_PF;
    GLuint gShaderProgramObject_PF;
    
    GLuint gModelMatrixUniform,gViewMatrixUniform,gProjectionMatrixUniform;
    GLuint gLightPositionUniform;
    GLuint gLKeyPressedUniform;
    
    GLuint La_uniform;
    GLuint Ld_Uniform;
    GLuint Ls_Uniform;
    
    GLuint Ka_Uniform;
    GLuint Kd_Uniform;
    GLuint Ks_Uniform;
    GLuint Material_shininess_uniform;
    
    //per FRAG
    GLuint gModelMatrixUniform_PF,gViewMatrixUniform_PF,gProjectionMatrixUniform_PF;
    GLuint gLightPositionUniform_PF;
    GLuint gLKeyPressedUniform_PF;
    
    GLuint La_uniform_PF;
    GLuint Ld_Uniform_PF;
    GLuint Ls_Uniform_PF;
    
    GLuint Ka_Uniform_PF;
    GLuint Kd_Uniform_PF;
    GLuint Ks_Uniform_PF;
    GLuint Material_shininess_uniform_PF;
    
    
    
    float sphere_vertices[1146];
    float shpere_normals[1146];
    float sphere_textures[764];
    unsigned short shpere_elements[2280];
    
    int gNumVertices;
    int gNumElements;
    
    GLuint gVao_sphere;
    GLuint gVbo_sphere_position;
    GLuint gVbo_sphere_normal;
    GLuint gVbo_sphere_elements;
    
    GLfloat lightAmbient[4];
    GLfloat lightDiffuse[4];
    GLfloat lightSpecular[4];
    GLfloat lightPosition[4];
    
    GLfloat material_Ambient[4];
    GLfloat material_Diffuse[4];
    GLfloat material_Specular[4];
    GLfloat material_shininess;
    
    BOOL gbLight;
    BOOL gPerVertex ;
    BOOL gPerFragment;
    
    vmath::mat4 projrctionMatrix;
    
    Sphere *sphere;
    
    GLfloat angle_sphere;
    
}

-(id) initWithFrame:(CGRect)frame
{
    self = [super initWithFrame:frame];
    
    if (self )
    {
        CAEAGLLayer *eaglLayer = (CAEAGLLayer *)super.layer;
        eaglLayer.opaque=YES;
        eaglLayer.drawableProperties=[ NSDictionary dictionaryWithObjectsAndKeys:[NSNumber numberWithBool:FALSE],kEAGLDrawablePropertyRetainedBacking,kEAGLColorFormatRGBA8,kEAGLDrawablePropertyColorFormat ,nil ];
        
        eaglContext = [[EAGLContext alloc] initWithAPI:kEAGLRenderingAPIOpenGLES3];
        if (eaglContext==nil)
        {
            [self release];
            return nil;
        }
        [EAGLContext setCurrentContext:eaglContext];
        
        /* glGenFramebuffers(1, &defaultFrameBuffer);
         glBindFramebuffer(GL_FRAMEBUFFER, defaultFrameBuffer);
         
         glGenRenderbuffers(1, &colorRenderBuffer);
         glBindBuffer(GL_RENDERBUFFER, colorRenderBuffer);
         */
        
        glGenFramebuffers(1,&defaultFrameBuffer);
        glGenRenderbuffers(1,&colorRenderBuffer);
        glBindFramebuffer(GL_FRAMEBUFFER,defaultFrameBuffer);
        glBindRenderbuffer(GL_RENDERBUFFER,colorRenderBuffer);
        
        [eaglContext renderbufferStorage:GL_RENDERBUFFER fromDrawable:eaglLayer];
        
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, colorRenderBuffer);
        
        GLint backingWidth;
        GLint backingHeight;
        
        glGetRenderbufferParameteriv(GL_RENDERBUFFER, GL_RENDERBUFFER_WIDTH, &backingWidth);
        glGetRenderbufferParameteriv(GL_RENDERBUFFER, GL_RENDERBUFFER_HEIGHT, &backingHeight);
        
        glGenRenderbuffers(1, &depthRenderBuffer);
        glBindRenderbuffer(GL_RENDERBUFFER, depthRenderBuffer);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT16, backingWidth, backingHeight);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthRenderBuffer);
        
        if (glCheckFramebufferStatus(GL_FRAMEBUFFER)!=GL_FRAMEBUFFER_COMPLETE)
        {
            printf("Failed to Create Complete FrameBuffer Object %x\n",glCheckFramebufferStatus(GL_FRAMEBUFFER));
            glDeleteFramebuffers(1, &defaultFrameBuffer);
            glDeleteRenderbuffers(1, &colorRenderBuffer);
            glDeleteRenderbuffers(1, &depthRenderBuffer);
            [self release];
            return nil;
        }
        
        printf("Renderer : %s |\n GL Version : %s |\n GLSL Version : %s \n",glGetString(GL_RENDERER),glGetString(GL_VERSION),glGetString(GL_SHADING_LANGUAGE_VERSION));
        
        isAnimating=NO;
        animationFrameInterval=60;
        
        lightAmbient[0] = 0.0f;
        lightAmbient[1] = 0.0f;
        lightAmbient[2] = 0.0f;
        lightAmbient[3] = 1.0f;
        
        lightDiffuse[0]= 1.0f;
        lightDiffuse[1]= 1.0f;
        lightDiffuse[2]= 1.0f;
        lightDiffuse[3]= 1.0f;
        
        lightSpecular[0] = 1.0f;
        lightSpecular[1] = 1.0f;
        lightSpecular[2] = 1.0f;
        lightSpecular[3] = 1.0f;
        
        lightPosition[0] = 50.0f;
        lightPosition[1] = 50.0f;
        lightPosition[2] = 100.0f;
        lightPosition[3] = 1.0f;
        
        material_Ambient[0] = 0.0f;
        material_Ambient[1] = 0.0f;
        material_Ambient[2] = 0.0f;
        material_Ambient[3] = 1.0f;
        
        material_Diffuse[0]= 1.0f;
        material_Diffuse[1]= 1.0f;
        material_Diffuse[2]= 1.0f;
        material_Diffuse[3]= 1.0f;
        
        material_Specular[0] = 1.0f;
        material_Specular[1] = 1.0f;
        material_Specular[2] = 1.0f;
        material_Specular[3] = 1.0f;
        
        material_shininess = 50.0f;
        angle_sphere = 0.0f;
        
        
        sphere = [[Sphere alloc]init];
        //vertex shader
        gVertexShaderObject = glCreateShader(GL_VERTEX_SHADER);
        
        const GLchar *vertexShaderSourceCode =
        "#version 300 es" \
        "\n" \
        "in vec4 vPosition;" \
        "in vec3 vNormal;" \
        "uniform mat4 u_model_matrix;" \
        "uniform mat4 u_view_matrix;" \
        "uniform mat4 u_projection_matrix;" \
        "uniform int u_LKeyPressed;" \
        "uniform vec3 u_La;" \
        "uniform vec3 u_Ld;" \
        "uniform vec3 u_Ls;" \
        "uniform vec3 u_Ka;" \
        "uniform vec3 u_Kd;" \
        "uniform vec3 u_Ks;" \
        "uniform vec4 u_light_position;" \
        "uniform float u_material_shininess;" \
        "out vec3 phong_ads_color;" \
        "void main (void)" \
        "{" \
        "if(u_LKeyPressed == 1)" \
        "{" \
        "vec4 eyeCoordinates =u_view_matrix * u_model_matrix * vPosition;" \
        "vec3 transformed_normals = normalize(mat3(u_view_matrix * u_model_matrix)*vNormal);" \
        "vec3 light_direction = normalize(vec3(u_light_position) - eyeCoordinates.xyz);" \
        "float tn_dot_ld = max(dot(transformed_normals,light_direction),0.0);" \
        "vec3 ambient = u_La * u_Ka;" \
        "vec3 diffuse = u_Ld * u_Kd * tn_dot_ld;" \
        "vec3 reflection_vector = reflect(-light_direction , transformed_normals);" \
        "vec3 viewer_vector = normalize(-eyeCoordinates.xyz);" \
        "vec3 specular = u_Ls * u_Ks * pow(max(dot(reflection_vector,viewer_vector),0.0),u_material_shininess);" \
        "phong_ads_color = ambient + diffuse + specular; "\
        "}" \
        "else" \
        "{" \
        " phong_ads_color = vec3(1.0,1.0,1.0);"
        "}" \
        "gl_Position = u_projection_matrix * u_view_matrix * u_model_matrix *vPosition;" \
        "}";
        
        glShaderSource(gVertexShaderObject,1,(const GLchar **)&vertexShaderSourceCode,NULL);
        glCompileShader(gVertexShaderObject);
        
        GLint iInfoLogLength = 0;
        GLint iShaderCompiledStatus = 0;
        char * szInfoLog = NULL;
        glGetShaderiv(gVertexShaderObject,GL_COMPILE_STATUS,&iShaderCompiledStatus);
        if(iShaderCompiledStatus ==GL_FALSE)
        {
            glGetShaderiv(gVertexShaderObject,GL_INFO_LOG_LENGTH,&iInfoLogLength);
            if(iInfoLogLength > 0)
            {
                szInfoLog = (char *)malloc(iInfoLogLength);
                if(szInfoLog != NULL)
                {
                    GLsizei written;
                    glGetShaderInfoLog(gVertexShaderObject,iInfoLogLength,&written,szInfoLog);
                    printf("Vertex Shader Compilation Log: %s\n",szInfoLog);
                    free(szInfoLog);
                    [self release];
                    
                }
            }
        }
        
        //fragment shader
        gFragmentShaderObject = glCreateShader(GL_FRAGMENT_SHADER);
        
        
        const GLchar * gFragmentShaderSourceCode =
        "#version 300 es" \
        "\n" \
        "precision highp float;"\
        "in vec3 phong_ads_color;"
        "out vec4 FragColor;" \
        "void main (void)" \
        "{" \
        "FragColor = vec4(phong_ads_color,1.0);" \
        "}";
        
        glShaderSource(gFragmentShaderObject,1,(const GLchar **)&gFragmentShaderSourceCode,NULL);
        glCompileShader(gFragmentShaderObject);
        
        glGetShaderiv(gFragmentShaderObject,GL_COMPILE_STATUS,&iShaderCompiledStatus);
        if(iShaderCompiledStatus ==GL_FALSE)
        {
            glGetShaderiv(gFragmentShaderObject,GL_INFO_LOG_LENGTH,&iInfoLogLength);
            if(iInfoLogLength > 0)
            {
                szInfoLog = (char *)malloc(iInfoLogLength);
                if(szInfoLog != NULL)
                {
                    GLsizei written;
                    glGetShaderInfoLog(gFragmentShaderObject,iInfoLogLength,&written,szInfoLog);
                    printf("Fragment Shader Compilation Log: %s\n",szInfoLog);
                    free(szInfoLog);
                    [self release];
                    
                }
            }
        }
        
        //////////////////////////////////Per Fragment//////////////////////////////////////////////
        
        const GLchar *vertexShaderSourceCode_PF =
        "#version 300 es" \
        "\n" \
        "in vec4 vPosition;"    \
        "in vec3 vNormal;"  \
        "uniform mat4 u_model_matrix;"  \
        "uniform mat4 u_view_matrix;"   \
        "uniform mat4 u_projection_matrix;" \
        "uniform mediump int u_lighting_enabled;"   \
        "uniform vec4 u_Light_Position;"    \
        "out vec3 transformed_normals;" \
        "out vec3 light_direction;" \
        "out vec3 viewer_vector;"   \
        "void main (void)"  \
        "{" \
        "if(u_lighting_enabled == 1)"   \
        "{" \
        "vec4 eye_cordinates = u_view_matrix * u_model_matrix * vPosition;" \
        "transformed_normals = mat3(u_view_matrix * u_model_matrix) * vNormal;" \
        "light_direction = vec3(u_Light_Position) - eye_cordinates.xyz;"    \
        "viewer_vector = -eye_cordinates.xyz;"  \
        "}" \
        "gl_Position = u_projection_matrix * u_view_matrix * u_model_matrix * vPosition;"   \
        "}";
        
        gVertexShaderObject_PF = glCreateShader(GL_VERTEX_SHADER);
        
        glShaderSource(gVertexShaderObject_PF, 1, (const GLchar **)&vertexShaderSourceCode_PF, NULL);
        
        glCompileShader(gVertexShaderObject_PF);
        
        glGetShaderiv(gVertexShaderObject_PF,GL_COMPILE_STATUS,&iShaderCompiledStatus);
        
        if(iShaderCompiledStatus ==GL_FALSE)
        {
            glGetShaderiv(gVertexShaderObject_PF,GL_INFO_LOG_LENGTH,&iInfoLogLength);
            if(iInfoLogLength > 0)
            {
                szInfoLog = (char *)malloc(iInfoLogLength);
                if(szInfoLog != NULL)
                {
                    GLsizei written;
                    glGetShaderInfoLog(gVertexShaderObject_PF,iInfoLogLength,&written,szInfoLog);
                    printf("Vertex Shader PV Compilation Log: %s\n",szInfoLog);
                    free(szInfoLog);
                    [self release];
                    
                }
            }
        }
        
        /////////////Fragment shader Per frag/////////////////
        gFragmentShaderObject_PF = glCreateShader(GL_FRAGMENT_SHADER);
        
        
        const GLchar * gFragmentShaderSourceCode_PF =
        "#version 300 es" \
        "\n" \
        "precision highp float;"\
        "in vec3 transformed_normals;"    \
        "in vec3 light_direction;"    \
        "in vec3 viewer_vector;"    \
        "out vec4 FragColor;"    \
        "uniform vec3 u_La;"    \
        "uniform vec3 u_Ld;"    \
        "uniform vec3 u_Ls;"    \
        "uniform vec3 u_Ka;"    \
        "uniform vec3 u_Kd;"    \
        "uniform vec3 u_Ks;"    \
        "uniform float u_Material_Shininess;"    \
        "uniform int u_lighting_enabled;"    \
        "void main (void)" \
        "{"    \
        "vec3 phong_ads_color;"    \
        "if(u_lighting_enabled == 1)"    \
        "{"    \
        "vec3 normalized_transformed_normals = normalize(transformed_normals);"    \
        "vec3 normalized_light_direction = normalize(light_direction);"    \
        "vec3 normalized_viewer_vector = normalize(viewer_vector);"    \
        "vec3 ambient = u_La * u_Ka ;"    \
        "float tn_dot_ld = max(dot(normalized_transformed_normals,normalized_light_direction),0.0);"    \
        "vec3 diffuse = u_Ld * u_Kd * tn_dot_ld;"    \
        "vec3 reflection_vector =  reflect(-normalized_light_direction,normalized_transformed_normals);"\
        "vec3 specular = u_Ks * u_Ls * pow(max(dot(reflection_vector,normalized_viewer_vector),0.0),u_Material_Shininess);" \
        "phong_ads_color = ambient + diffuse + specular ;"    \
        "}"    \
        "else"    \
        "{"    \
        "phong_ads_color = vec3(1.0,1.0,1.0);"    \
        "}"    \
        "FragColor = vec4(phong_ads_color,1.0);"    \
        "}";
        
        glShaderSource(gFragmentShaderObject_PF,1,(const GLchar **)&gFragmentShaderSourceCode_PF,NULL);
        glCompileShader(gFragmentShaderObject_PF);
        
        glGetShaderiv(gFragmentShaderObject_PF,GL_COMPILE_STATUS,&iShaderCompiledStatus);
        if(iShaderCompiledStatus ==GL_FALSE)
        {
            glGetShaderiv(gFragmentShaderObject_PF,GL_INFO_LOG_LENGTH,&iInfoLogLength);
            if(iInfoLogLength > 0)
            {
                szInfoLog = (char *)malloc(iInfoLogLength);
                if(szInfoLog != NULL)
                {
                    GLsizei written;
                    glGetShaderInfoLog(gFragmentShaderObject_PF,iInfoLogLength,&written,szInfoLog);
                    printf("Fragment Shader PF Compilation Log: %s\n",szInfoLog);
                    free(szInfoLog);
                    [self release];
                    
                }
            }
        }
        
        
        
        ////////////////////////////////shader program//////////////////////////
        
        gShaderProgramObject = glCreateProgram();
        glAttachShader(gShaderProgramObject,gVertexShaderObject);
        glAttachShader(gShaderProgramObject,gFragmentShaderObject);
        
        //per Frag
        gShaderProgramObject_PF = glCreateProgram();
        glAttachShader(gShaderProgramObject_PF,gVertexShaderObject_PF);
        glAttachShader(gShaderProgramObject_PF,gFragmentShaderObject_PF);
        
        //bind in variables before linking
        glBindAttribLocation(gShaderProgramObject,AMC_ATTRIBUTES_POSITION,"vPosition");
        glBindAttribLocation(gShaderProgramObject,AMC_ATTRIBUTES_NORMAL,"vNormal");
        
        glBindAttribLocation(gShaderProgramObject_PF,AMC_ATTRIBUTES_POSITION,"vPosition");
        glBindAttribLocation(gShaderProgramObject_PF,AMC_ATTRIBUTES_NORMAL,"vNormal");
        
        glLinkProgram(gShaderProgramObject);
        GLint iShaderProgramLinkStatus = 0;
        glGetProgramiv(gShaderProgramObject,GL_LINK_STATUS,&iShaderProgramLinkStatus);
        if(iShaderProgramLinkStatus == GL_FALSE)
        {
            glGetProgramiv(gShaderProgramObject,GL_INFO_LOG_LENGTH,&iInfoLogLength);
            if(iInfoLogLength > 0)
            {
                szInfoLog = (char *)malloc(iInfoLogLength);
                if(szInfoLog != NULL)
                {
                    GLsizei written;
                    glGetProgramInfoLog(gShaderProgramObject,iInfoLogLength,&written,szInfoLog);
                    printf("Shader Program  Link Log: %s\n",szInfoLog);
                    free(szInfoLog);
                    [self release];
                }
            }
        }
        
        glLinkProgram(gShaderProgramObject_PF);
        iShaderProgramLinkStatus = 0;
        glGetProgramiv(gShaderProgramObject_PF,GL_LINK_STATUS,&iShaderProgramLinkStatus);
        if(iShaderProgramLinkStatus == GL_FALSE)
        {
            glGetProgramiv(gShaderProgramObject_PF,GL_INFO_LOG_LENGTH,&iInfoLogLength);
            if(iInfoLogLength > 0)
            {
                szInfoLog = (char *)malloc(iInfoLogLength);
                if(szInfoLog != NULL)
                {
                    GLsizei written;
                    glGetProgramInfoLog(gShaderProgramObject_PF,iInfoLogLength,&written,szInfoLog);
                    printf("Shader Program PF Link Log: %s\n",szInfoLog);
                    free(szInfoLog);
                    [self release];
                }
            }
        }
        
        ///////////////////////////////////get uniform location////////////////////////////
        
        gModelMatrixUniform = glGetUniformLocation(gShaderProgramObject, "u_model_matrix");
        gViewMatrixUniform = glGetUniformLocation(gShaderProgramObject, "u_view_matrix");
        gProjectionMatrixUniform = glGetUniformLocation(gShaderProgramObject, "u_projection_matrix");
        
        Ka_Uniform = glGetUniformLocation(gShaderProgramObject, "u_Ka");
        Kd_Uniform = glGetUniformLocation(gShaderProgramObject, "u_Kd");
        Ks_Uniform = glGetUniformLocation(gShaderProgramObject, "u_Ks");
        Material_shininess_uniform = glGetUniformLocation(gShaderProgramObject, "u_material_shininess");
        
        gLightPositionUniform = glGetUniformLocation(gShaderProgramObject, "u_light_position");
        Ld_Uniform = glGetUniformLocation(gShaderProgramObject, "u_Ld");
        La_uniform = glGetUniformLocation(gShaderProgramObject, "u_La");
        Ls_Uniform = glGetUniformLocation(gShaderProgramObject, "u_Ls");
        gLKeyPressedUniform = glGetUniformLocation(gShaderProgramObject, "u_LKeyPressed");
        
        ///////PER FRGAMENT///////////////////////////////////////////////////////////////////
        
        gModelMatrixUniform_PF = glGetUniformLocation(gShaderProgramObject_PF, "u_model_matrix");
        gViewMatrixUniform_PF = glGetUniformLocation(gShaderProgramObject_PF, "u_view_matrix");
        gProjectionMatrixUniform_PF = glGetUniformLocation(gShaderProgramObject_PF, "u_projection_matrix");
        
        Ka_Uniform_PF = glGetUniformLocation(gShaderProgramObject_PF, "u_Ka");
        Kd_Uniform_PF = glGetUniformLocation(gShaderProgramObject_PF, "u_Kd");
        Ks_Uniform_PF = glGetUniformLocation(gShaderProgramObject_PF, "u_Ks");
        Material_shininess_uniform_PF = glGetUniformLocation(gShaderProgramObject_PF, "u_Material_Shininess");
        
        gLightPositionUniform_PF = glGetUniformLocation(gShaderProgramObject_PF, "u_Light_Position");
        Ld_Uniform_PF = glGetUniformLocation(gShaderProgramObject_PF, "u_Ld");
        La_uniform_PF = glGetUniformLocation(gShaderProgramObject_PF, "u_La");
        Ls_Uniform_PF = glGetUniformLocation(gShaderProgramObject_PF, "u_Ls");
        gLKeyPressedUniform_PF = glGetUniformLocation(gShaderProgramObject_PF, "u_lighting_enabled");
        
        
        
        
        
        
        
        
        
        
        ////////////////////SPHERE////////////////////////////////////
        
        [sphere getSphereVertexDataWithPosition:sphere_vertices withNormals:shpere_normals withTexCoords:sphere_textures andElements:shpere_elements];
        gNumVertices = [sphere getNumberOfSphereVertices];
        gNumElements = [sphere getNumberOfSphereElements];
        
        glGenVertexArrays(1, &gVao_sphere);
        glBindVertexArray(gVao_sphere);
        //vertices
        glGenBuffers(1, &gVbo_sphere_position);
        glBindBuffer(GL_ARRAY_BUFFER, gVbo_sphere_position);
        glBufferData(GL_ARRAY_BUFFER, sizeof(sphere_vertices), sphere_vertices, GL_STATIC_DRAW);
        
        glVertexAttribPointer(AMC_ATTRIBUTES_POSITION, 3, GL_FLOAT, GL_FALSE, 0, NULL);
        
        glEnableVertexAttribArray(AMC_ATTRIBUTES_POSITION);
        
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        
        //normals
        glGenBuffers(1, &gVbo_sphere_normal);
        glBindBuffer(GL_ARRAY_BUFFER, gVbo_sphere_normal);
        glBufferData(GL_ARRAY_BUFFER, sizeof(shpere_normals), shpere_normals, GL_STATIC_DRAW);
        
        glVertexAttribPointer(AMC_ATTRIBUTES_NORMAL, 3, GL_FLOAT, GL_FALSE, 0, NULL);
        
        glEnableVertexAttribArray(AMC_ATTRIBUTES_NORMAL);
        
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        
        //elements
        glGenBuffers(1, &gVbo_sphere_elements);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gVbo_sphere_elements);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(shpere_elements), shpere_elements, GL_STATIC_DRAW);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
        
        
        glBindVertexArray(0);
        
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LEQUAL);
        glEnable(GL_CULL_FACE);
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        
        
        gbLight = NO;
        gPerVertex = YES;
        
        //Gesture Recognition Code
        //Tap gesture code
        UITapGestureRecognizer *singleTapGestureRecognizer = [ [ UITapGestureRecognizer alloc ] initWithTarget:self action:@selector(onSingleTap:)] ;
        [singleTapGestureRecognizer setNumberOfTapsRequired:1]; // 1 touch for 1 fingure
        [singleTapGestureRecognizer setDelegate:self];
        [self addGestureRecognizer:singleTapGestureRecognizer];
        
        // Double Tap Gesture Code
        UITapGestureRecognizer *doubleTapGestureRecognizer = [[UITapGestureRecognizer alloc]initWithTarget:self action:@selector(onDoubleTap:)];
        [doubleTapGestureRecognizer setNumberOfTapsRequired:2];
        [doubleTapGestureRecognizer setNumberOfTouchesRequired:1]; // 1 touch of fingure
        [doubleTapGestureRecognizer setDelegate:self];
        [self addGestureRecognizer:doubleTapGestureRecognizer];
        
        [singleTapGestureRecognizer requireGestureRecognizerToFail:doubleTapGestureRecognizer];
        
        //swipe gesture
        UISwipeGestureRecognizer *swipGestureRecognizer = [ [UISwipeGestureRecognizer alloc]initWithTarget:self action:@selector(onSwipe:) ] ;
        [self addGestureRecognizer:swipGestureRecognizer];
        
        // long Pres Gesture
        UILongPressGestureRecognizer *longPressGestureRecognizer = [ [UILongPressGestureRecognizer alloc]initWithTarget:self action:@selector(onLongPress:) ];
        
        [self addGestureRecognizer:longPressGestureRecognizer];
        
        
    }
    
    return (self);
    
}


+(Class)layerClass
{
    return ([CAEAGLLayer class]);
}

-(void)drawView:(id)sender
{
    [EAGLContext setCurrentContext:eaglContext];
    glBindFramebuffer(GL_FRAMEBUFFER, defaultFrameBuffer);
    
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT|GL_STENCIL_BUFFER_BIT);
    
    vmath::mat4 modelMatrix = vmath::mat4::identity();
    vmath::mat4 viewMatrix = vmath::mat4::identity();
    vmath::mat4 rotationMatrix = vmath::mat4::identity();
    
    modelMatrix = vmath::translate(0.0f,0.0f,-2.0f);
    rotationMatrix = vmath::rotate(angle_sphere, 0.0f, 1.0f, 0.0f);
    
    modelMatrix = modelMatrix * rotationMatrix;
    
    glUniformMatrix4fv(gModelMatrixUniform, 1, GL_FALSE, modelMatrix);
    glUniformMatrix4fv(gViewMatrixUniform, 1, GL_FALSE, viewMatrix);
    glUniformMatrix4fv(gProjectionMatrixUniform, 1, GL_FALSE, projrctionMatrix);
    
    if(gbLight == YES)
    {
        if(gPerVertex == YES)
        {
            glUseProgram(gShaderProgramObject);
            
            glUniform1i(gLKeyPressedUniform,1);
            
            //light properties
            glUniform3fv(La_uniform,1,lightAmbient);
            glUniform3fv(Ld_Uniform,1,lightDiffuse);
            glUniform3fv(Ls_Uniform,1,lightSpecular);
            glUniform4fv(gLightPositionUniform,1,lightPosition);
            
            glUniform3fv(Ka_Uniform,1,material_Ambient);
            glUniform3fv(Kd_Uniform,1,material_Diffuse);
            glUniform3fv(Ks_Uniform,1,material_Specular);
            glUniform1f(Material_shininess_uniform,material_shininess);
            
            glUniformMatrix4fv(gModelMatrixUniform, 1, GL_FALSE, modelMatrix);
            glUniformMatrix4fv(gViewMatrixUniform, 1, GL_FALSE, viewMatrix);
            glUniformMatrix4fv(gProjectionMatrixUniform, 1, GL_FALSE, projrctionMatrix);
            
        }
        else
        {
            glUseProgram(gShaderProgramObject_PF);
            
            glUniform1i(gLKeyPressedUniform_PF,1);
            
            //light properties
            glUniform3fv(La_uniform_PF,1,lightAmbient);
            glUniform3fv(Ld_Uniform_PF,1,lightDiffuse);
            glUniform3fv(Ls_Uniform_PF,1,lightSpecular);
            glUniform4fv(gLightPositionUniform_PF,1,lightPosition);
            
            glUniform3fv(Ka_Uniform_PF,1,material_Ambient);
            glUniform3fv(Kd_Uniform_PF,1,material_Diffuse);
            glUniform3fv(Ks_Uniform_PF,1,material_Specular);
            glUniform1f(Material_shininess_uniform_PF,material_shininess);
            
            glUniformMatrix4fv(gModelMatrixUniform_PF, 1, GL_FALSE, modelMatrix);
            glUniformMatrix4fv(gViewMatrixUniform_PF, 1, GL_FALSE, viewMatrix);
            glUniformMatrix4fv(gProjectionMatrixUniform_PF, 1, GL_FALSE, projrctionMatrix);
        }
        
    }
    else
    {
        glUniform1i(gLKeyPressedUniform,0);
        glUniform1i(gLKeyPressedUniform_PF,0);
    }
    
    
    
    
    glBindVertexArray(gVao_sphere);
    
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,gVbo_sphere_elements);
    glDrawElements(GL_TRIANGLES, gNumElements,GL_UNSIGNED_SHORT, 0);
    
    
    glBindVertexArray(0);
    
    
    glUseProgram(0);
    
    glBindRenderbuffer(GL_RENDERBUFFER, colorRenderBuffer);
    [eaglContext presentRenderbuffer:GL_RENDERBUFFER];
    
    [self updateGeometry];
}

-(void)updateGeometry
{
    angle_sphere = angle_sphere + 0.1f;
    
    if(angle_sphere >= 360.0f)
        angle_sphere = 0.0f;
    
}

-(void)layoutSubviews
{
    GLint width;
    GLint height;
    
    glBindRenderbuffer(GL_RENDERBUFFER, colorRenderBuffer);
    [eaglContext renderbufferStorage:GL_RENDERBUFFER fromDrawable:(CAEAGLLayer *)self.layer];
    
    glGetRenderbufferParameteriv(GL_RENDERBUFFER, GL_RENDERBUFFER_WIDTH, &width);
    glGetRenderbufferParameteriv(GL_RENDERBUFFER, GL_RENDERBUFFER_HEIGHT, &height);
    
    glGenRenderbuffers(1, &depthRenderBuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, depthRenderBuffer);
    
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT16, width, height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthRenderBuffer);
    
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER)!=GL_FRAMEBUFFER_COMPLETE)
    {
        printf("Failed to Create Complete FrameBuffer Object %x\n",glCheckFramebufferStatus(GL_FRAMEBUFFER));
    }
    
    glViewport(0,0,(GLsizei)width,(GLsizei)height);
    
    projrctionMatrix =vmath::perspective(45.0f,(GLfloat)width/(GLfloat)height,0.1f,100.0f);
    [self drawView:nil];
}

-(void)startAnimation
{
    if (!isAnimating)
    {
        displayLink = [NSClassFromString(@"CADisplayLink") displayLinkWithTarget:self selector:@selector(drawView:)];
        [displayLink setPreferredFramesPerSecond:animationFrameInterval];
        [displayLink addToRunLoop:[NSRunLoop currentRunLoop] forMode:NSDefaultRunLoopMode];
        
        isAnimating=YES;
    }
}

-(void)stopAnimation
{
    if (isAnimating)
    {
        [displayLink invalidate];
        displayLink=nil;
        
        isAnimating=NO;
    }
}

-(BOOL) acceptsFirstResponder
{
    return YES;
}

-(void)touchesBegane :(NSSet *)touches withEvent:(UIEvent *)UIEvent
{
    
    
}


-(void) onLongPress: (UIGestureRecognizer *)gesture
{
    
}

-(void) onSwipe: (UIGestureRecognizer *)gesture
{
    
    [self release];
    exit(0);
}

-(void) onDoubleTap: (UITapGestureRecognizer *)gesture
{
    if(gPerVertex == NO)
    {
        gPerVertex = YES;
        gPerFragment = NO;
    }
    else
    {
        gPerVertex = NO;
        gPerFragment = YES;
    }
    
}

-(void) onSingleTap: (UITapGestureRecognizer *)gesture
{
    if(gbLight == NO)
    {
        gbLight = YES;
    }
    else
    {
        gbLight = NO;
    }
    
    
}

-(void)dealloc
{
    if (depthRenderBuffer)
    {
        glDeleteRenderbuffers(1, &depthRenderBuffer);
        depthRenderBuffer=0;
    }
    
    if (colorRenderBuffer)
    {
        glDeleteRenderbuffers(1, &colorRenderBuffer);
        colorRenderBuffer=0;
    }
    
    if (defaultFrameBuffer)
    {
        glDeleteRenderbuffers(1, &defaultFrameBuffer);
        defaultFrameBuffer=0;
    }
    
    if ([EAGLContext currentContext]==eaglContext)
    {
        [EAGLContext setCurrentContext:nil];
    }
    [eaglContext release];
    eaglContext=nil;
    
    [super dealloc];
}

@end

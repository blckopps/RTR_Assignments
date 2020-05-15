


#import <OpenGLES/ES3/gl.h>
#import <OpenGLES/ES3/glext.h>

#import "vmath.h"
#import "GLESView.h"
#import "Sphere.h"

enum
{
    AMC_ATTRIBUTE_VERTEX=0,
    AMC_ATTRIBUTE_COLOR,
    AMC_ATTRIBUTE_NORMAL,
    AMC_ATTRIBUTE_TEXTURE0,
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
    
    GLuint vertexShaderObject;
    GLuint fragmentShaderObject;
    GLuint shaderProgramObject;
    
    
    GLuint vertexArrayObject_Sphere;
    GLuint vertexBufferObject_Sphere_Position;
    GLuint vertexBufferObject_Sphere_Element;
    GLuint vertexBufferObject_Sphere_Normal;
    
    float sphere_vertices[1146];
    float sphere_normals[1146];
    float sphere_textures[764];
    unsigned short sphere_elements[2280];
    
    //GLuint uniform_ModelViewProjection;
    
    GLfloat sphere_angle;
    
    GLuint Uniform_ModelMatrix,Uniform_ViewMatrix, Uniform_ProjectionMatrix;
    GLuint Uniform_La,Uniform_Ld, Uniform_Ls,Uniform_LightPosition;
    GLuint Uniform_Ka,Uniform_Kd,Uniform_Ks,Uniform_Material_shininess;
    
    GLuint Uniform_LightEnabled;
    
    vmath::mat4 perspectiveProjectionMatrix;
    
    BOOL bIsDoubleTapDone;// Animation
    BOOL bIsSingleTapDone;// Light
    
    Sphere *mySphere;
    
    GLuint iNumVertices, iNumElements;
    GLfloat light_Ambient[4];
    GLfloat light_Diffuse[4];
    GLfloat light_Specular[4] ;
    GLfloat light_Position[4] ;
    
    
    
    GLfloat material_Ambient[4] ;
    GLfloat material_Diffuse[4] ;
    GLfloat material_Specular[4];
    GLfloat material_shininess ;
    
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
        
        light_Ambient[0] = 0.0f    ;
        light_Ambient[1] = 0.0f    ;
        light_Ambient[2] = 0.0f    ;
        light_Ambient[3] = 1.0f    ;
        
        light_Diffuse[0] = 1.0f    ;
        light_Diffuse[1] = 1.0f    ;
        light_Diffuse[2] = 1.0f    ;
        light_Diffuse[3] = 1.0f    ;
        
        light_Specular[0] = 1.0f;
        light_Specular[1] = 1.0f;
        light_Specular[2] = 1.0f;
        light_Specular[3] = 1.0f;
        
        light_Position[0] = 100.0f;
        light_Position[1] = 100.0f;
        light_Position[2] = 100.0f;
        light_Position[3] = 1.0f;
        
        material_Ambient[0] = 0.0f    ;
        material_Ambient[1] = 0.0f    ;
        material_Ambient[2] = 0.0f    ;
        material_Ambient[3] = 1.0f    ;
        
        material_Diffuse[0] = 1.0f    ;
        material_Diffuse[1] = 1.0f    ;
        material_Diffuse[2] = 1.0f    ;
        material_Diffuse[3] = 1.0f    ;
        
        material_Specular[0] = 1.0f;
        material_Specular[1] = 1.0f;
        material_Specular[2] = 1.0f;
        material_Specular[3] = 1.0f;
        
        material_shininess = 50.0f;
        
        bIsDoubleTapDone=NO;
        bIsSingleTapDone=NO;
        
        mySphere = [[Sphere alloc]init];
        //vertex shader
        vertexShaderObject = glCreateShader(GL_VERTEX_SHADER);
        
        const GLchar *vertexShaderSourceCode =
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
        glShaderSource(vertexShaderObject,1,(const GLchar **)&vertexShaderSourceCode,NULL);
        glCompileShader(vertexShaderObject);
        
        GLint iInfoLogLength = 0;
        GLint iShaderCompiledStatus = 0;
        char * szInfoLog = NULL;
        glGetShaderiv(vertexShaderObject,GL_COMPILE_STATUS,&iShaderCompiledStatus);
        if(iShaderCompiledStatus ==GL_FALSE)
        {
            glGetShaderiv(vertexShaderObject,GL_INFO_LOG_LENGTH,&iInfoLogLength);
            if(iInfoLogLength > 0)
            {
                szInfoLog = (char *)malloc(iInfoLogLength);
                if(szInfoLog != NULL)
                {
                    GLsizei written;
                    glGetShaderInfoLog(vertexShaderObject,iInfoLogLength,&written,szInfoLog);
                    printf("Vertex Shader Compilation Log: %s\n",szInfoLog);
                    free(szInfoLog);
                    [self release];
                    
                }
            }
        }
        
        //fragment shader
        fragmentShaderObject = glCreateShader(GL_FRAGMENT_SHADER);
        
        
        const GLchar * gFragmentShaderSourceCode =
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
        
        glShaderSource(fragmentShaderObject,1,(const GLchar **)&gFragmentShaderSourceCode,NULL);
        glCompileShader(fragmentShaderObject);
        
        glGetShaderiv(fragmentShaderObject,GL_COMPILE_STATUS,&iShaderCompiledStatus);
        if(iShaderCompiledStatus ==GL_FALSE)
        {
            glGetShaderiv(fragmentShaderObject,GL_INFO_LOG_LENGTH,&iInfoLogLength);
            if(iInfoLogLength > 0)
            {
                szInfoLog = (char *)malloc(iInfoLogLength);
                if(szInfoLog != NULL)
                {
                    GLsizei written;
                    glGetShaderInfoLog(fragmentShaderObject,iInfoLogLength,&written,szInfoLog);
                    printf("Fragment Shader Compilation Log: %s\n",szInfoLog);
                    free(szInfoLog);
                    [self release];
                    
                }
            }
        }
        
        //shader program
        
        shaderProgramObject = glCreateProgram();
        glAttachShader(shaderProgramObject,vertexShaderObject);
        glAttachShader(shaderProgramObject,fragmentShaderObject);
        
        //bind in variables before linking
        glBindAttribLocation(shaderProgramObject,AMC_ATTRIBUTE_VERTEX,"vPosition");
        glBindAttribLocation(shaderProgramObject,AMC_ATTRIBUTE_NORMAL,"vNormal");
        
        
        glLinkProgram(shaderProgramObject);
        GLint iShaderProgramLinkStatus = 0;
        glGetProgramiv(shaderProgramObject,GL_LINK_STATUS,&iShaderProgramLinkStatus);
        if(iShaderProgramLinkStatus == GL_FALSE)
        {
            glGetProgramiv(shaderProgramObject,GL_INFO_LOG_LENGTH,&iInfoLogLength);
            if(iInfoLogLength > 0)
            {
                szInfoLog = (char *)malloc(iInfoLogLength);
                if(szInfoLog != NULL)
                {
                    GLsizei written;
                    glGetProgramInfoLog(shaderProgramObject,iInfoLogLength,&written,szInfoLog);
                    printf("Shader Program  Link Log: %s\n",szInfoLog);
                    free(szInfoLog);
                    [self release];
                }
            }
        }
        
        //get uniform location
        Uniform_ModelMatrix = glGetUniformLocation(shaderProgramObject, "u_model_matrix");
        Uniform_ViewMatrix = glGetUniformLocation(shaderProgramObject, "u_view_matrix");
        Uniform_ProjectionMatrix = glGetUniformLocation(shaderProgramObject, "u_projection_matrix");
        
        Uniform_Ka = glGetUniformLocation(shaderProgramObject, "u_Ka");
        Uniform_Kd = glGetUniformLocation(shaderProgramObject, "u_Kd");
        Uniform_Ks = glGetUniformLocation(shaderProgramObject, "u_Ks");
        Uniform_Material_shininess = glGetUniformLocation(shaderProgramObject, "u_Material_Shininess");
        
        Uniform_LightPosition = glGetUniformLocation(shaderProgramObject, "u_Light_Position");
        Uniform_Ld = glGetUniformLocation(shaderProgramObject, "u_Ld");
        Uniform_La = glGetUniformLocation(shaderProgramObject, "u_La");
        Uniform_Ls = glGetUniformLocation(shaderProgramObject, "u_Ls");
        Uniform_LightEnabled = glGetUniformLocation(shaderProgramObject, "u_lighting_enabled");
        
        [mySphere getSphereVertexDataWithPosition:sphere_vertices withNormals:sphere_normals withTexCoords:sphere_textures andElements:sphere_elements];
        iNumVertices = [mySphere getNumberOfSphereVertices ];
        iNumElements = [mySphere getNumberOfSphereElements ];
        
        glGenVertexArrays(1, &vertexArrayObject_Sphere);
        glBindVertexArray(vertexArrayObject_Sphere);
        //vertices
        glGenBuffers(1, &vertexBufferObject_Sphere_Position);
        glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObject_Sphere_Position);
        glBufferData(GL_ARRAY_BUFFER, sizeof(sphere_vertices), sphere_vertices, GL_STATIC_DRAW);
        
        glVertexAttribPointer(AMC_ATTRIBUTE_VERTEX, 3, GL_FLOAT, GL_FALSE, 0, NULL);
        
        glEnableVertexAttribArray(AMC_ATTRIBUTE_VERTEX);
        
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        
        //normals
        glGenBuffers(1, &vertexBufferObject_Sphere_Normal);
        glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObject_Sphere_Normal);
        glBufferData(GL_ARRAY_BUFFER, sizeof(sphere_normals), sphere_normals, GL_STATIC_DRAW);
        glVertexAttribPointer(AMC_ATTRIBUTE_NORMAL, 3, GL_FLOAT, GL_FALSE, 0, NULL);
        glEnableVertexAttribArray(AMC_ATTRIBUTE_NORMAL);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        
        //elements
        glGenBuffers(1, &vertexBufferObject_Sphere_Element);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vertexBufferObject_Sphere_Element);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(sphere_elements), sphere_elements, GL_STATIC_DRAW);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
        
        
        glBindVertexArray(0);
        
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LEQUAL);
        glEnable(GL_CULL_FACE);
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        
        
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

/*
 // Only override drawRect: if you perform custom drawing.
 // An empty implementation adversely affects performance during animation.
 - (void)drawRect:(CGRect)rect {
 // Drawing code
 }
 */

+(Class)layerClass
{
    return ([CAEAGLLayer class]);
}

-(void)drawView:(id)sender
{
    [EAGLContext setCurrentContext:eaglContext];
    glBindFramebuffer(GL_FRAMEBUFFER, defaultFrameBuffer);
    
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
    
    glUseProgram(shaderProgramObject);
    
    
    if (bIsSingleTapDone == YES)
    {
        
        glUniform1i(Uniform_LightEnabled, 1);
        
        glUniform3fv(Uniform_La,1,light_Ambient);
        glUniform3fv(Uniform_Ld, 1, light_Diffuse);
        glUniform3fv(Uniform_Ls, 1, light_Specular);
        glUniform4fv(Uniform_LightPosition, 1, light_Position);
        
        glUniform3fv(Uniform_Ka, 1, material_Ambient);
        glUniform3fv(Uniform_Kd, 1, material_Diffuse);
        glUniform3fv(Uniform_Ks, 1, material_Specular);
        glUniform1f(Uniform_Material_shininess, material_shininess);
        
    }
    else
    {
        glUniform1i(Uniform_LightEnabled, 0);
    }
    
    
    vmath::mat4 modelMatrix = vmath::mat4::identity();
    vmath::mat4 viewMatrix = vmath::mat4::identity();
    vmath::mat4 rotationMatrix = vmath::mat4::identity();
    
    modelMatrix = vmath::translate(0.0f, 0.0f, -2.0f);
    rotationMatrix = vmath::rotate(sphere_angle,0.0f,1.0f,0.0f);
    modelMatrix = modelMatrix * rotationMatrix  ;
    //vmath::mat4 modelViewProjectionMatrix = g_PerspectiveProjectionMatrix * modelViewMatrix;
    //glUniformMatrix4fv(g_Uniform_ModelViewProjection, 1, GL_FALSE, modelViewProjectionMatrix);
    
    glUniformMatrix4fv(Uniform_ModelMatrix,1,GL_FALSE, modelMatrix);
    glUniformMatrix4fv(Uniform_ViewMatrix, 1, GL_FALSE, viewMatrix);
    glUniformMatrix4fv(Uniform_ProjectionMatrix,1, GL_FALSE, perspectiveProjectionMatrix);
    
    glBindVertexArray(vertexArrayObject_Sphere);
    
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vertexBufferObject_Sphere_Element);
    glDrawElements(GL_TRIANGLES, iNumElements, GL_UNSIGNED_SHORT, 0);
    
    glBindVertexArray(0);
    
    glUseProgram(0);
    
    glBindRenderbuffer(GL_RENDERBUFFER, colorRenderBuffer);
    [eaglContext presentRenderbuffer:GL_RENDERBUFFER];
    
    if (bIsDoubleTapDone==YES)
        [self updateGeometry];
    
    
}

-(void)updateGeometry
{
    sphere_angle = sphere_angle + 0.5;
    if (sphere_angle>360.0f)
    {
        sphere_angle = 0.0f;
    }
    
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
    
    perspectiveProjectionMatrix = vmath::perspective(45.0f, (GLfloat)width/(GLfloat)height, 0.1f, 100.0f);
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
    
    //centralText=@" 'touchesBegane' Event Occured ";
    //[ self setNeedsDisplay ];// akin to InvalidateRect()
    
}


-(void) onLongPress: (UIGestureRecognizer *)gesture
{
    //centralText=@" 'OnLongPress' Event Occured ";
    //[ self setNeedsDisplay ];// akin to InvalidateRect()
}

-(void) onSwipe: (UIGestureRecognizer *)gesture
{
    //centralText=@" 'OnSwipe' Event Occured ";
    //[ self setNeedsDisplay ];// akin to InvalidateRect()
    [self release];
    exit(0);
}

-(void) onDoubleTap: (UITapGestureRecognizer *)gesture
{
    //centralText=@" 'OnDoubleTap' Event Occured ";
    //[ self setNeedsDisplay ];// akin to InvalidateRect()
    
    if (bIsDoubleTapDone==NO)
    {
        bIsDoubleTapDone = YES;
    }
    else
    {
        bIsDoubleTapDone = NO;
    }
    
    
}

-(void) onSingleTap: (UITapGestureRecognizer *)gesture
{
    
    //bIsSingleTapDone
    
    if (bIsSingleTapDone==NO)
    {
        bIsSingleTapDone = YES;
    }
    else
    {
        bIsSingleTapDone = NO;
    }
    printf("hello");
    //centralText=@" 'OnSingleTap' Event Occured ";
    //[ self setNeedsDisplay ];// akin to InvalidateRect()
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


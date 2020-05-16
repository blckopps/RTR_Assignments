

#import <OpenGLES/ES3/gl.h>
#import <OpenGLES/ES3/glext.h>

#import "vmath.h"
#import "GLESView.h"


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
    
    
    GLuint vao_Pyramid;
    GLuint vbo_Position_Pyramid;
    GLuint vbo_Element_Pyramid;
    GLuint vbo_Normal_Pyramid;
    
    
    GLfloat Pyramid_angle;
    
    GLuint Uniform_ModelMatrix;
    GLuint Uniform_ViewMatrix;
    GLuint Uniform_ProjectionMatrix;
    
    
    GLuint Uniform_La,Uniform_Ld_red, Uniform_Ls_red,Uniform_LightPosition_red;
    GLuint Uniform_Ld_green, Uniform_Ls_green,Uniform_LightPosition_green;
    
    GLuint Uniform_Ka,Uniform_Kd,Uniform_Ks,Uniform_Material_shininess;
    
    GLuint Uniform_LightEnabled,Uniform_ShaderToggle;
    
    vmath::mat4 perspectiveProjectionMatrix;
    
    BOOL bIsDoubleTapDone;
    BOOL bIsSingleTapDone;
    BOOL bShaderToggleFlag;
    
    
    GLfloat light_Ambient[4];
    
    GLfloat light_Diffuse_Red[4];
    GLfloat light_Specular_Red[4] ;
    GLfloat light_Position_Red[4] ;
    
    GLfloat light_Diffuse_Green[4];
    GLfloat light_Specular_Green[4] ;
    GLfloat light_Position_Green[4] ;
    
    
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
        
        
        
        isAnimating=NO;
        animationFrameInterval=60;
        
        light_Ambient[0] = 0.0f;
        light_Ambient[1] = 0.0f;
        light_Ambient[2] = 0.0f;
        light_Ambient[3] = 1.0f;
        
        light_Diffuse_Red[0] = 1.0f;
        light_Diffuse_Red[1] = 0.0f;
        light_Diffuse_Red[2] = 0.0f;
        light_Diffuse_Red[3] = 1.0f;
        
        light_Specular_Red[0] = 1.0f;
        light_Specular_Red[1] = 0.0f;
        light_Specular_Red[2] = 0.0f;
        light_Specular_Red[3] = 1.0f;
        
        light_Position_Red[0] = 100.0f;
        light_Position_Red[1] = 100.0f;
        light_Position_Red[2] = 100.0f;
        light_Position_Red[3] = 1.0f;
        
        light_Diffuse_Green[0] = 0.0f;
        light_Diffuse_Green[1] = 0.0f;
        light_Diffuse_Green[2] = 1.0f;
        light_Diffuse_Green[3] = 1.0f;
        
        light_Specular_Green[0] = 0.0f;
        light_Specular_Green[1] = 0.0f;
        light_Specular_Green[2] = 1.0f;
        light_Specular_Green[3] = 1.0f;
        
        light_Position_Green[0] = -100.0f;
        light_Position_Green[1] = 100.0f;
        light_Position_Green[2] = 100.0f;
        light_Position_Green[3] = 1.0f;
        
        material_Ambient[0] = 0.0f;
        material_Ambient[1] = 0.0f;
        material_Ambient[2] = 0.0f;
        material_Ambient[3] = 1.0f;
        
        material_Diffuse[0] = 1.0f;
        material_Diffuse[1] = 1.0f;
        material_Diffuse[2] = 1.0f;
        material_Diffuse[3] = 1.0f;
        
        material_Specular[0] = 1.0f;
        material_Specular[1] = 1.0f;
        material_Specular[2] = 1.0f;
        material_Specular[3] = 1.0f;
        
        material_shininess = 50.0f;
        
        bIsDoubleTapDone=NO;
        bIsSingleTapDone=NO;
        bShaderToggleFlag=NO;
        
        //vertex shader
        vertexShaderObject = glCreateShader(GL_VERTEX_SHADER);
        
        const GLchar *vertexShaderSourceCode =
        "#version 300 es" \
        "\n" \
        "in vec4 vPosition;" \
        "in vec3 vNormal;" \
        "uniform mat4 u_model_matrix;" \
        "uniform mat4 u_view_matrix;" \
        "uniform mat4 u_projection_matrix;" \
        "uniform mediump int u_lighting_enabled;" \
        "uniform vec4 u_light_Position_red;" \
        "uniform vec4 u_light_Position_green;" \
        "uniform mediump int u_toggle_shader;" \
        "uniform vec3 u_La;" \
        "uniform vec3 u_Ld_red;" \
        "uniform vec3 u_Ls_red;" \
        "uniform vec3 u_Ld_green;" \
        "uniform vec3 u_Ls_green;" \
        "uniform vec3 u_Ka;" \
        "uniform vec3 u_Kd;" \
        "uniform vec3 u_Ks;" \
        "uniform float u_material_shininess;" \
        "out vec3 transformed_normals;" \
        "out vec3 light_direction_red;" \
        "out vec3 light_direction_green;" \
        "out vec3 viewer_vector;" \
        "out vec3 phong_ads_color_vertex;" \
        "void main(void)" \
        "{" \
        "if(u_lighting_enabled==1)" \
        "{" \
        "vec4 eye_coordinates = u_view_matrix*u_model_matrix*vPosition;" \
        "transformed_normals = mat3(u_view_matrix*u_model_matrix)*vNormal;" \
        "light_direction_red = vec3(u_light_Position_red)-eye_coordinates.xyz;" \
        "light_direction_green = vec3(u_light_Position_green)-eye_coordinates.xyz;" \
        "viewer_vector = -eye_coordinates.xyz;" \
        "if(u_toggle_shader == 1)" \
        "{" \
        "vec3 normalized_transformed_normals = normalize(transformed_normals);" \
        "vec3 normalized_light_direction_red = normalize(light_direction_red);" \
        "vec3 normalized_light_direction_green = normalize(light_direction_green);" \
        "vec3 normalized_viewer_vector = normalize(viewer_vector);" \
        "vec3 ambient = u_La * u_Ka ;" \
        "float tn_dot_ld_red = max(dot(normalized_transformed_normals,normalized_light_direction_red),0.0);" \
        "float tn_dot_ld_green = max(dot(normalized_transformed_normals,normalized_light_direction_green),0.0);" \
        "vec3 diffuse = (u_Ld_red * u_Kd * tn_dot_ld_red) + (u_Ld_green * u_Kd * tn_dot_ld_green) ;" \
        "vec3 reflection_vector_red = reflect(-normalized_light_direction_red,normalized_transformed_normals);" \
        "vec3 reflection_vector_green = reflect(-normalized_light_direction_green,normalized_transformed_normals);" \
        "vec3 specular = (u_Ls_red * u_Ks * pow(max(dot(reflection_vector_red,normalized_viewer_vector),0.0),u_material_shininess)) + (u_Ls_green * u_Ks * pow(max(dot(reflection_vector_green,normalized_viewer_vector),0.0),u_material_shininess));" \
        "phong_ads_color_vertex = ambient + diffuse + specular;" \
        "}" \
        "}" \
        "gl_Position = u_projection_matrix*u_view_matrix*u_model_matrix*vPosition;" \
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
        "in vec3 transformed_normals;" \
        "in vec3 light_direction_red;" \
        "in vec3 light_direction_green;" \
        "in vec3 viewer_vector;" \
        "in vec3 phong_ads_color_vertex;" \
        "out vec4 FragColor;" \
        "uniform vec3 u_La;" \
        "uniform vec3 u_Ld_red;" \
        "uniform vec3 u_Ls_red;" \
        "uniform vec3 u_Ld_green;" \
        "uniform vec3 u_Ls_green;" \
        "uniform vec3 u_Ka;" \
        "uniform vec3 u_Kd;" \
        "uniform vec3 u_Ks;" \
        "uniform float u_material_shininess;" \
        "uniform int u_lighting_enabled;" \
        "uniform int u_toggle_shader;" \
        "void main(void)" \
        "{" \
        "vec3 phong_ads_color;" \
        "if(u_lighting_enabled == 1)" \
        "{" \
        "if(u_toggle_shader == 0)" \
        "{" \
        "vec3 normalized_transformed_normals = normalize(transformed_normals);" \
        "vec3 normalized_light_direction_red = normalize(light_direction_red);" \
        "vec3 normalized_light_direction_green = normalize(light_direction_green);" \
        "vec3 normalized_viewer_vector = normalize(viewer_vector);" \
        "float tn_dot_ld_red = max(dot(normalized_transformed_normals,normalized_light_direction_red),0.0);" \
        "float tn_dot_ld_green = max(dot(normalized_transformed_normals,normalized_light_direction_green),0.0);" \
        "vec3 reflection_vector_red = reflect(-normalized_light_direction_red,normalized_transformed_normals);" \
        "vec3 reflection_vector_green = reflect(-normalized_light_direction_green,normalized_transformed_normals);" \
        "vec3 ambient = u_La * u_Ka ;" \
        "vec3 diffuse = (u_Ld_red * u_Kd * tn_dot_ld_red) + (u_Ld_green * u_Kd * tn_dot_ld_green) ;" \
        "vec3 specular = (u_Ls_red * u_Ks * pow(max(dot(reflection_vector_red,normalized_viewer_vector),0.0),u_material_shininess)) + (u_Ls_green * u_Ks * pow(max(dot(reflection_vector_green,normalized_viewer_vector),0.0),u_material_shininess)) ;" \
        "phong_ads_color = ambient + diffuse + specular;" \
        "}" \
        "else" \
        "{"\
        "phong_ads_color = phong_ads_color_vertex;" \
        "}" \
        "}" \
        "else" \
        "{" \
        "phong_ads_color = vec3(1.0,1.0,1.0);" \
        "}" \
        "FragColor = vec4(phong_ads_color,1.0);" \
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
        Uniform_Material_shininess = glGetUniformLocation(shaderProgramObject, "u_material_shininess");
        
        Uniform_LightPosition_red = glGetUniformLocation(shaderProgramObject, "u_light_Position_red");
        Uniform_LightPosition_green = glGetUniformLocation(shaderProgramObject, "u_light_Position_green");
        
        Uniform_La = glGetUniformLocation(shaderProgramObject, "u_La");
        
        Uniform_Ld_red = glGetUniformLocation(shaderProgramObject, "u_Ld_red");
        Uniform_Ls_red = glGetUniformLocation(shaderProgramObject, "u_Ls_red");
        
        Uniform_Ld_green = glGetUniformLocation(shaderProgramObject, "u_Ld_green");
        Uniform_Ls_green = glGetUniformLocation(shaderProgramObject, "u_Ls_green");
        
        Uniform_LightEnabled = glGetUniformLocation(shaderProgramObject, "u_lighting_enabled");
        Uniform_ShaderToggle = glGetUniformLocation(shaderProgramObject, "u_toggle_shader");
        
        
        // vertices, colors, shader attribs, vbo_Position_Pyramid, vao_Pyramid initializations
        //// Pyramid Section Start
        const GLfloat pyramidVertices[] =
        {
            0.0f, 1.0f, 0.0f,
            -1.0f, -1.0f, 1.0f,
            1.0f, -1.0f, 1.0f,
            
            0.0f, 1.0f, 0.0f,
            1.0f, -1.0f, 1.0f,
            1.0f, -1.0f, -1.0f ,
            
            0.0f, 1.0f, 0.0f ,
            -1.0f, -1.0f, -1.0f,
            -1.0f, -1.0f, 1.0f,
            
            0.0f, 1.0f, 0.0f ,
            1.0f, -1.0f, -1.0f ,
            -1.0f, -1.0f, -1.0f
        };
        const GLfloat pyramidNormalsCords[] =
        {
            0.0f,0.447214f,0.894427f,
            0.0f,0.447214f,0.894427f,
            0.0f,0.447214f,0.894427f,
            0.894427f,0.447214f,0.0f,
            0.894427f,0.447214f,0.0f,
            0.894427f,0.447214f,0.0f,
            0.0f,0.447214f,-0.894427f,
            0.0f,0.447214f,-0.894427f,
            0.0f,0.447214f,-0.894427f,
            -0.894427f,0.447214f,0.0f,
            -0.894427f,0.447214f,0.0f,
            -0.894427f,0.447214f,0.0f
        };
        
        glGenVertexArrays(1,&vao_Pyramid);
        glBindVertexArray(vao_Pyramid);
        
        // position vbo
        glGenBuffers(1, &vbo_Position_Pyramid);
        glBindBuffer(GL_ARRAY_BUFFER, vbo_Position_Pyramid);
        glBufferData(GL_ARRAY_BUFFER, sizeof(pyramidVertices), pyramidVertices, GL_STATIC_DRAW);
        
        glVertexAttribPointer(AMC_ATTRIBUTE_VERTEX, 3, GL_FLOAT, GL_FALSE, 0, NULL);
        
        glEnableVertexAttribArray(AMC_ATTRIBUTE_VERTEX);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        
        // normal vbo
        glGenBuffers(1, &vbo_Normal_Pyramid);
        glBindBuffer(GL_ARRAY_BUFFER, vbo_Normal_Pyramid);
        glBufferData(GL_ARRAY_BUFFER, sizeof(pyramidNormalsCords), pyramidNormalsCords, GL_STATIC_DRAW);
        glVertexAttribPointer(AMC_ATTRIBUTE_NORMAL, 3, GL_FLOAT, GL_FALSE, 0, NULL);
        glEnableVertexAttribArray(AMC_ATTRIBUTE_NORMAL);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        
        glBindVertexArray(0);
        
        
        
        
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LEQUAL);
        glEnable(GL_CULL_FACE);
        glClearColor(0.25f, 0.25f, 0.25f, 1.0f);
        
        
        //Gesture Recognition Code
        //Tap gesture code
        UITapGestureRecognizer *singleTapGestureRecognizer = [ [ UITapGestureRecognizer alloc ] initWithTarget:self action:@selector(onSingleTap:)] ;
        [singleTapGestureRecognizer setNumberOfTapsRequired:1];
        [singleTapGestureRecognizer setDelegate:self];
        [self addGestureRecognizer:singleTapGestureRecognizer];
        
        // Double Tap Gesture Code
        UITapGestureRecognizer *doubleTapGestureRecognizer = [[UITapGestureRecognizer alloc]initWithTarget:self action:@selector(onDoubleTap:)];
        [doubleTapGestureRecognizer setNumberOfTapsRequired:2];
        [doubleTapGestureRecognizer setNumberOfTouchesRequired:1];
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
    
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
    
    glUseProgram(shaderProgramObject);
    
    
    if (bIsSingleTapDone == YES)
    {
        glUniform1i(Uniform_LightEnabled, 1);
        
        if (bShaderToggleFlag == YES)
            glUniform1i(Uniform_ShaderToggle, 1);
        else
            glUniform1i(Uniform_ShaderToggle, 0);
        
        glUniform3fv(Uniform_La,1,light_Ambient);
        glUniform3fv(Uniform_Ld_red, 1, light_Diffuse_Red);
        glUniform3fv(Uniform_Ls_red, 1, light_Specular_Red);
        glUniform4fv(Uniform_LightPosition_red, 1, light_Position_Red);
        
        glUniform3fv(Uniform_Ld_green, 1, light_Diffuse_Green);
        glUniform3fv(Uniform_Ls_green, 1, light_Specular_Green);
        glUniform4fv(Uniform_LightPosition_green, 1, light_Position_Green);
        
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
    
    modelMatrix = vmath::translate(0.0f, 0.0f, -6.0f);
    rotationMatrix = vmath::rotate(Pyramid_angle,0.0f,1.0f,0.0f);
    modelMatrix = modelMatrix * rotationMatrix  ;
    
    
    glUniformMatrix4fv(Uniform_ModelMatrix,1,GL_FALSE, modelMatrix);
    glUniformMatrix4fv(Uniform_ViewMatrix, 1, GL_FALSE, viewMatrix);
    glUniformMatrix4fv(Uniform_ProjectionMatrix,1, GL_FALSE, perspectiveProjectionMatrix);
    
    glBindVertexArray(vao_Pyramid);
    glDrawArrays(GL_TRIANGLES, 0, 12);
    glBindVertexArray(0);
    
    glUseProgram(0);
    
    glBindRenderbuffer(GL_RENDERBUFFER, colorRenderBuffer);
    [eaglContext presentRenderbuffer:GL_RENDERBUFFER];
    
    if (bIsDoubleTapDone==YES)
        [self updateGeometry];
    
    
}

-(void)updateGeometry
{
    Pyramid_angle = Pyramid_angle + 0.5;
    if (Pyramid_angle>360.0f)
    {
        Pyramid_angle = 0.0f;
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
    
    
    
}


-(void) onLongPress: (UIGestureRecognizer *)gesture
{
    
    if(bShaderToggleFlag==NO)
        bShaderToggleFlag=YES;
    else
        bShaderToggleFlag=NO;
    
    printf("In onLongPress \n");
}

-(void) onSwipe: (UIGestureRecognizer *)gesture
{
    
    [self release];
    exit(0);
}

-(void) onDoubleTap: (UITapGestureRecognizer *)gesture
{
    
    
    if (bIsDoubleTapDone==NO)
    {
        bIsDoubleTapDone = YES;
    }
    else
    {
        bIsDoubleTapDone = NO;
    }
    
    printf("In onDoubleTap \n");
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




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
    
    
    GLuint vao_Sphere;
    GLuint vbo_Sphere_Position;
    GLuint vao_Sphere_Element;
    GLuint vao_Sphere_Normal;
    
    float sphere_vertices[1146];
    float sphere_normals[1146];
    float sphere_textures[764];
    unsigned short sphere_elements[2280];
    
    
    GLfloat sphere_angle;
    
    GLuint Uniform_ModelMatrix;
    GLuint Uniform_ViewMatrix;
    GLuint Uniform_ProjectionMatrix;
    
    GLuint Uniform_La,Uniform_Ld, Uniform_Ls,Uniform_light_Position;
    GLuint Uniform_Ka,Uniform_Kd,Uniform_Ks,Uniform_Material_shininess;
    
    GLuint Uniform_LightEnabled,Uniform_ShaderToggle;
    
    vmath::mat4 perspectiveProjectionMatrix;
    
    
    BOOL bIsSingleTapDone;// Light
    BOOL bShaderToggleFlag;// Toggle Flag
    
    Sphere *mySphere;
    
    GLuint iNumVertices, iNumElements;
    GLfloat light_Ambient[4];
    GLfloat light_Diffuse[4];
    GLfloat light_Specular[4] ;
    GLfloat light_Position[4] ;
    
    GLuint iLightRotation;
    
    GLfloat material_Ambient[4] ;
    GLfloat material_Diffuse[4] ;
    GLfloat material_Specular[4];
    GLfloat material_shininess ;
    
    GLfloat fPresentWidth;
    GLfloat fPresentHeight;
    
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
        
        printf("GL Version : %s |\n GLSL Version : %s \n",glGetString(GL_VERSION),glGetString(GL_SHADING_LANGUAGE_VERSION));
        
        isAnimating=NO;
        animationFrameInterval=60;
        
        iLightRotation = 3;
        
        light_Ambient[0] = 0.0f;
        light_Ambient[1] = 0.0f;
        light_Ambient[2] = 0.0f;
        light_Ambient[3] = 1.0f;
        
        light_Diffuse[0] = 1.0f;
        light_Diffuse[1] = 1.0f;
        light_Diffuse[2] = 1.0f;
        light_Diffuse[3] = 1.0f;
        
        light_Specular[0] = 1.0f;
        light_Specular[1] = 1.0f;
        light_Specular[2] = 1.0f;
        light_Specular[3] = 1.0f;
        
        light_Position[0] = 100.0f;
        light_Position[1] = 100.0f;
        light_Position[2] = 100.0f;
        light_Position[3] = 1.0f;
        
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
        
        
        bIsSingleTapDone=NO;
        bShaderToggleFlag=NO;
        
        mySphere = [[Sphere alloc]init];
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
        "uniform vec4 u_light_position;" \
        "uniform mediump int u_toggle_shader;" \
        "uniform vec3 u_La;" \
        "uniform vec3 u_Ld;" \
        "uniform vec3 u_Ls;" \
        "uniform vec3 u_Ka;" \
        "uniform vec3 u_Kd;" \
        "uniform vec3 u_Ks;" \
        "uniform float u_material_shininess;" \
        "out vec3 transformed_normals;" \
        "out vec3 light_direction;" \
        "out vec3 viewer_vector;" \
        "out vec3 phong_ads_color_vertex;" \
        "void main(void)" \
        "{" \
        "if(u_lighting_enabled==1)" \
        "{" \
        "vec4 eye_coordinates = u_view_matrix*u_model_matrix*vPosition;" \
        "transformed_normals = mat3(u_view_matrix*u_model_matrix)*vNormal;" \
        "light_direction = vec3(u_light_position)-eye_coordinates.xyz;" \
        "viewer_vector = -eye_coordinates.xyz;" \
        "if(u_toggle_shader == 1)" \
        "{" \
        "vec3 normalized_transformed_normals = normalize(transformed_normals);" \
        "vec3 normalized_light_direction = normalize(light_direction);" \
        "vec3 normalized_viewer_vector = normalize(viewer_vector);" \
        "vec3 ambient = u_La * u_Ka ;" \
        "float tn_dot_ld = max(dot(normalized_transformed_normals,normalized_light_direction),0.0);" \
        "vec3 diffuse = u_Ld * u_Kd * tn_dot_ld ;" \
        "vec3 reflection_vector = reflect(-normalized_light_direction,normalized_transformed_normals);" \
        "vec3 specular = u_Ls * u_Ks * pow(max(dot(reflection_vector,normalized_viewer_vector),0.0),u_material_shininess);" \
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
        "in vec3 light_direction;" \
        "in vec3 viewer_vector;" \
        "in vec3 phong_ads_color_vertex;" \
        "out vec4 FragColor;" \
        "uniform vec3 u_La;" \
        "uniform vec3 u_Ld;" \
        "uniform vec3 u_Ls;" \
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
        "vec3 normalized_light_direction = normalize(light_direction);" \
        "vec3 normalized_viewer_vector = normalize(viewer_vector);" \
        "float tn_dot_ld = max(dot(normalized_transformed_normals,normalized_light_direction),0.0);" \
        "vec3 reflection_vector = reflect(-normalized_light_direction,normalized_transformed_normals);" \
        "vec3 ambient = u_La * u_Ka ;" \
        "vec3 diffuse = u_Ld * u_Kd * tn_dot_ld ;" \
        "vec3 specular = u_Ls * u_Ks * pow(max(dot(reflection_vector,normalized_viewer_vector),0.0),u_material_shininess) ;" \
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
        
        Uniform_light_Position = glGetUniformLocation(shaderProgramObject, "u_light_position");
        Uniform_Ld = glGetUniformLocation(shaderProgramObject, "u_Ld");
        Uniform_La = glGetUniformLocation(shaderProgramObject, "u_La");
        Uniform_Ls = glGetUniformLocation(shaderProgramObject, "u_Ls");
        Uniform_LightEnabled = glGetUniformLocation(shaderProgramObject, "u_lighting_enabled");
        Uniform_ShaderToggle = glGetUniformLocation(shaderProgramObject, "u_toggle_shader");
        
        
        [mySphere getSphereVertexDataWithPosition:sphere_vertices withNormals:sphere_normals withTexCoords:sphere_textures andElements:sphere_elements];
        iNumVertices = [mySphere getNumberOfSphereVertices ];
        iNumElements = [mySphere getNumberOfSphereElements ];
        
        glGenVertexArrays(1, &vao_Sphere);
        glBindVertexArray(vao_Sphere);
        //vertices
        glGenBuffers(1, &vbo_Sphere_Position);
        glBindBuffer(GL_ARRAY_BUFFER, vbo_Sphere_Position);
        glBufferData(GL_ARRAY_BUFFER, sizeof(sphere_vertices), sphere_vertices, GL_STATIC_DRAW);
        
        glVertexAttribPointer(AMC_ATTRIBUTE_VERTEX, 3, GL_FLOAT, GL_FALSE, 0, NULL);
        
        glEnableVertexAttribArray(AMC_ATTRIBUTE_VERTEX);
        
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        
        //normals
        glGenBuffers(1, &vao_Sphere_Normal);
        glBindBuffer(GL_ARRAY_BUFFER, vao_Sphere_Normal);
        glBufferData(GL_ARRAY_BUFFER, sizeof(sphere_normals), sphere_normals, GL_STATIC_DRAW);
        glVertexAttribPointer(AMC_ATTRIBUTE_NORMAL, 3, GL_FLOAT, GL_FALSE, 0, NULL);
        glEnableVertexAttribArray(AMC_ATTRIBUTE_NORMAL);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        
        //elements
        glGenBuffers(1, &vao_Sphere_Element);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vao_Sphere_Element);
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



+(Class)layerClass
{
    return ([CAEAGLLayer class]);
}

-(void)drawView:(id)sender
{
    [EAGLContext setCurrentContext:eaglContext];
    glBindFramebuffer(GL_FRAMEBUFFER, defaultFrameBuffer);
    
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
    
    //Use Shader Program Object
    glUseProgram(shaderProgramObject);
    float present_ratio = (float)fPresentWidth /(float)fPresentHeight;
    vmath::mat4 modelMatrix = vmath::mat4::identity();
    vmath::mat4 viewMatrix = vmath::mat4::identity();
    
    modelMatrix = vmath::translate(0.0f, 0.0f, 0.0f);
    
    
    glUniformMatrix4fv(Uniform_ModelMatrix, 1, GL_FALSE, modelMatrix);
    glUniformMatrix4fv(Uniform_ViewMatrix, 1, GL_FALSE, viewMatrix);
    glUniformMatrix4fv(Uniform_ProjectionMatrix, 1, GL_FALSE, perspectiveProjectionMatrix);
    
    glBindVertexArray(vao_Sphere);
    
    
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vao_Sphere_Element);
    
    // Sphere 1,1
    glViewport(50,250,(int)(present_ratio*3*16.0f),(int)(present_ratio*3*9.0f));
    //modelMatrix = vmath::translate(-5.5f, 5.0f, -16.0f);
    modelMatrix = vmath::translate(0.0f, 0.0f, -1.3f);
    [self MaterialC1S1];
    glUniformMatrix4fv(Uniform_ModelMatrix, 1, GL_FALSE, modelMatrix);
    glUniformMatrix4fv(Uniform_ViewMatrix, 1, GL_FALSE, viewMatrix);
    glUniformMatrix4fv(Uniform_ProjectionMatrix, 1, GL_FALSE, perspectiveProjectionMatrix);
    glDrawElements(GL_TRIANGLES, iNumElements, GL_UNSIGNED_SHORT, 0);
    
    // Sphere 1,2
    glViewport(3*50,250,(int)(present_ratio*3*16.0f),(int)(present_ratio*3*9.0f));
    modelMatrix = vmath::translate(0.0f, 0.0f, -1.3f);
    [self MaterialC1S2];
    glUniformMatrix4fv(Uniform_ModelMatrix, 1, GL_FALSE, modelMatrix);
    glUniformMatrix4fv(Uniform_ViewMatrix, 1, GL_FALSE, viewMatrix);
    glUniformMatrix4fv(Uniform_ProjectionMatrix, 1, GL_FALSE, perspectiveProjectionMatrix);
    glDrawElements(GL_TRIANGLES, iNumElements, GL_UNSIGNED_SHORT, 0);
    
    // Sphere 1,3
    glViewport(5*50,250,(int)(present_ratio*3*16.0f),(int)(present_ratio*3*9.0f));
    modelMatrix = vmath::translate(0.0f, 0.0f, -1.3f);
    [self MaterialC1S3];
    glUniformMatrix4fv(Uniform_ModelMatrix, 1, GL_FALSE, modelMatrix);
    glUniformMatrix4fv(Uniform_ViewMatrix, 1, GL_FALSE, viewMatrix);
    glUniformMatrix4fv(Uniform_ProjectionMatrix, 1, GL_FALSE, perspectiveProjectionMatrix);
    glDrawElements(GL_TRIANGLES, iNumElements, GL_UNSIGNED_SHORT, 0);
    
    // Sphere 1,4
    glViewport(7*50,250,(int)(present_ratio*3*16.0f),(int)(present_ratio*3*9.0f));
    modelMatrix = vmath::translate(0.0f, 0.0f, -1.3f);
    [self MaterialC1S4];
    glUniformMatrix4fv(Uniform_ModelMatrix, 1, GL_FALSE, modelMatrix);
    glUniformMatrix4fv(Uniform_ViewMatrix, 1, GL_FALSE, viewMatrix);
    glUniformMatrix4fv(Uniform_ProjectionMatrix, 1, GL_FALSE, perspectiveProjectionMatrix);
    glDrawElements(GL_TRIANGLES, iNumElements, GL_UNSIGNED_SHORT, 0);
    
    // Sphere 1,5
    glViewport(9*50,250,(int)(present_ratio*3*16.0f),(int)(present_ratio*3*9.0f));
    modelMatrix = vmath::translate(0.0f, 0.0f, -1.3f);
    [self MaterialC1S5];
    glUniformMatrix4fv(Uniform_ModelMatrix, 1, GL_FALSE, modelMatrix);
    glUniformMatrix4fv(Uniform_ViewMatrix, 1, GL_FALSE, viewMatrix);
    glUniformMatrix4fv(Uniform_ProjectionMatrix, 1, GL_FALSE, perspectiveProjectionMatrix);
    glDrawElements(GL_TRIANGLES, iNumElements, GL_UNSIGNED_SHORT, 0);
    // Sphere 1,6
    glViewport(11*50,250,(int)(present_ratio*3*16.0f),(int)(present_ratio*3*9.0f));
    modelMatrix = vmath::translate(0.0f, 0.0f, -1.3f);
    [self MaterialC1S6];
    glUniformMatrix4fv(Uniform_ModelMatrix, 1, GL_FALSE, modelMatrix);
    glUniformMatrix4fv(Uniform_ViewMatrix, 1, GL_FALSE, viewMatrix);
    glUniformMatrix4fv(Uniform_ProjectionMatrix, 1, GL_FALSE, perspectiveProjectionMatrix);
    glDrawElements(GL_TRIANGLES, iNumElements, GL_UNSIGNED_SHORT, 0);
    
    // Sphere 2,1
    glViewport(50,180,(int)(present_ratio*3*16.0f),(int)(present_ratio*3*9.0f));
    modelMatrix = vmath::translate(0.0f, 0.0f, -1.3f);
    [self MaterialC2S1];
    glUniformMatrix4fv(Uniform_ModelMatrix, 1, GL_FALSE, modelMatrix);
    glUniformMatrix4fv(Uniform_ViewMatrix, 1, GL_FALSE, viewMatrix);
    glUniformMatrix4fv(Uniform_ProjectionMatrix, 1, GL_FALSE, perspectiveProjectionMatrix);
    glDrawElements(GL_TRIANGLES, iNumElements, GL_UNSIGNED_SHORT, 0);
    
    // Sphere 2,2
    glViewport(3*50,180,(int)(present_ratio*3*16.0f),(int)(present_ratio*3*9.0f));
    modelMatrix = vmath::translate(0.0f, 0.0f, -1.3f);
    [self MaterialC2S2];
    glUniformMatrix4fv(Uniform_ModelMatrix, 1, GL_FALSE, modelMatrix);
    glUniformMatrix4fv(Uniform_ViewMatrix, 1, GL_FALSE, viewMatrix);
    glUniformMatrix4fv(Uniform_ProjectionMatrix, 1, GL_FALSE, perspectiveProjectionMatrix);
    glDrawElements(GL_TRIANGLES, iNumElements, GL_UNSIGNED_SHORT, 0);
    
    // Sphere 2,3
    glViewport(5*50,180,(int)(present_ratio*3*16.0f),(int)(present_ratio*3*9.0f));
    modelMatrix = vmath::translate(0.0f, 0.0f, -1.3f);
    [self MaterialC2S3];
    glUniformMatrix4fv(Uniform_ModelMatrix, 1, GL_FALSE, modelMatrix);
    glUniformMatrix4fv(Uniform_ViewMatrix, 1, GL_FALSE, viewMatrix);
    glUniformMatrix4fv(Uniform_ProjectionMatrix, 1, GL_FALSE, perspectiveProjectionMatrix);
    glDrawElements(GL_TRIANGLES, iNumElements, GL_UNSIGNED_SHORT, 0);
    
    // Sphere 2,4
    glViewport(7*50,180,(int)(present_ratio*3*16.0f),(int)(present_ratio*3*9.0f));
    modelMatrix = vmath::translate(0.0f, 0.0f, -1.3f);
    [self MaterialC2S4];
    glUniformMatrix4fv(Uniform_ModelMatrix, 1, GL_FALSE, modelMatrix);
    glUniformMatrix4fv(Uniform_ViewMatrix, 1, GL_FALSE, viewMatrix);
    glUniformMatrix4fv(Uniform_ProjectionMatrix, 1, GL_FALSE, perspectiveProjectionMatrix);
    glDrawElements(GL_TRIANGLES, iNumElements, GL_UNSIGNED_SHORT, 0);
    
    // Sphere 2,5
    glViewport(9*50,180,(int)(present_ratio*3*16.0f),(int)(present_ratio*3*9.0f));
    modelMatrix = vmath::translate(0.0f, 0.0f, -1.3f);
    [self MaterialC2S5];
    glUniformMatrix4fv(Uniform_ModelMatrix, 1, GL_FALSE, modelMatrix);
    glUniformMatrix4fv(Uniform_ViewMatrix, 1, GL_FALSE, viewMatrix);
    glUniformMatrix4fv(Uniform_ProjectionMatrix, 1, GL_FALSE, perspectiveProjectionMatrix);
    glDrawElements(GL_TRIANGLES, iNumElements, GL_UNSIGNED_SHORT, 0);
    
    // Sphere 2,6
    glViewport(11*50,180,(int)(present_ratio*3*16.0f),(int)(present_ratio*3*9.0f));
    modelMatrix = vmath::translate(0.0f, 0.0f, -1.3f);
    [self MaterialC2S6];
    glUniformMatrix4fv(Uniform_ModelMatrix, 1, GL_FALSE, modelMatrix);
    glUniformMatrix4fv(Uniform_ViewMatrix, 1, GL_FALSE, viewMatrix);
    glUniformMatrix4fv(Uniform_ProjectionMatrix, 1, GL_FALSE, perspectiveProjectionMatrix);
    glDrawElements(GL_TRIANGLES, iNumElements, GL_UNSIGNED_SHORT, 0);
    
    
    // Sphere 3,1
    glViewport(50,100,(int)(present_ratio*3*16.0f),(int)(present_ratio*3*9.0f));
    modelMatrix = vmath::translate(0.0f, 0.0f, -1.3f);
    [self MaterialC3S1];
    glUniformMatrix4fv(Uniform_ModelMatrix, 1, GL_FALSE, modelMatrix);
    glUniformMatrix4fv(Uniform_ViewMatrix, 1, GL_FALSE, viewMatrix);
    glUniformMatrix4fv(Uniform_ProjectionMatrix, 1, GL_FALSE, perspectiveProjectionMatrix);
    glDrawElements(GL_TRIANGLES, iNumElements, GL_UNSIGNED_SHORT, 0);
    
    
    // Sphere 3,2
    glViewport(3*50,100,(int)(present_ratio*3*16.0f),(int)(present_ratio*3*9.0f));
    modelMatrix = vmath::translate(0.0f, 0.0f, -1.3f);
    [self MaterialC3S2];
    glUniformMatrix4fv(Uniform_ModelMatrix, 1, GL_FALSE, modelMatrix);
    glUniformMatrix4fv(Uniform_ViewMatrix, 1, GL_FALSE, viewMatrix);
    glUniformMatrix4fv(Uniform_ProjectionMatrix, 1, GL_FALSE, perspectiveProjectionMatrix);
    glDrawElements(GL_TRIANGLES, iNumElements, GL_UNSIGNED_SHORT, 0);
    
    // Sphere 3,3
    glViewport(5*50,100,(int)(present_ratio*3*16.0f),(int)(present_ratio*3*9.0f));
    modelMatrix = vmath::translate(0.0f, 0.0f, -1.3f);
    [self MaterialC3S3];
    glUniformMatrix4fv(Uniform_ModelMatrix, 1, GL_FALSE, modelMatrix);
    glUniformMatrix4fv(Uniform_ViewMatrix, 1, GL_FALSE, viewMatrix);
    glUniformMatrix4fv(Uniform_ProjectionMatrix, 1, GL_FALSE, perspectiveProjectionMatrix);
    glDrawElements(GL_TRIANGLES, iNumElements, GL_UNSIGNED_SHORT, 0);
    
    // Sphere 3,4
    glViewport(7*50,100,(int)(present_ratio*3*16.0f),(int)(present_ratio*3*9.0f));
    modelMatrix = vmath::translate(0.0f, 0.0f, -1.3f);
    [self MaterialC3S4];
    glUniformMatrix4fv(Uniform_ModelMatrix, 1, GL_FALSE, modelMatrix);
    glUniformMatrix4fv(Uniform_ViewMatrix, 1, GL_FALSE, viewMatrix);
    glUniformMatrix4fv(Uniform_ProjectionMatrix, 1, GL_FALSE, perspectiveProjectionMatrix);
    glDrawElements(GL_TRIANGLES, iNumElements, GL_UNSIGNED_SHORT, 0);
    
    // Sphere 3,5
    glViewport(9*50,100,(int)(present_ratio*3*16.0f),(int)(present_ratio*3*9.0f));
    modelMatrix = vmath::translate(0.0f, 0.0f, -1.3f);
    [self MaterialC3S5];
    glUniformMatrix4fv(Uniform_ModelMatrix, 1, GL_FALSE, modelMatrix);
    glUniformMatrix4fv(Uniform_ViewMatrix, 1, GL_FALSE, viewMatrix);
    glUniformMatrix4fv(Uniform_ProjectionMatrix, 1, GL_FALSE, perspectiveProjectionMatrix);
    glDrawElements(GL_TRIANGLES, iNumElements, GL_UNSIGNED_SHORT, 0);
    
    // Sphere 3,6
    glViewport(11*50,100,(int)(present_ratio*3*16.0f),(int)(present_ratio*3*9.0f));
    modelMatrix = vmath::translate(0.0f, 0.0f, -1.3f);
    [self MaterialC3S6];
    glUniformMatrix4fv(Uniform_ModelMatrix, 1, GL_FALSE, modelMatrix);
    glUniformMatrix4fv(Uniform_ViewMatrix, 1, GL_FALSE, viewMatrix);
    glUniformMatrix4fv(Uniform_ProjectionMatrix, 1, GL_FALSE, perspectiveProjectionMatrix);
    glDrawElements(GL_TRIANGLES, iNumElements, GL_UNSIGNED_SHORT, 0);
    
    
    // Sphere 4,1
    glViewport(50,30,(int)(present_ratio*3*16.0f),(int)(present_ratio*3*9.0f));
    modelMatrix = vmath::translate(0.0f, 0.0f, -1.3f);
    [self MaterialC4S1];
    glUniformMatrix4fv(Uniform_ModelMatrix, 1, GL_FALSE, modelMatrix);
    glUniformMatrix4fv(Uniform_ViewMatrix, 1, GL_FALSE, viewMatrix);
    glUniformMatrix4fv(Uniform_ProjectionMatrix, 1, GL_FALSE, perspectiveProjectionMatrix);
    glDrawElements(GL_TRIANGLES, iNumElements, GL_UNSIGNED_SHORT, 0);
    
    // Sphere 4,2
    glViewport(3*50,30,(int)(present_ratio*3*16.0f),(int)(present_ratio*3*9.0f));
    modelMatrix = vmath::translate(0.0f, 0.0f, -1.3f);
    [self MaterialC4S2];
    glUniformMatrix4fv(Uniform_ModelMatrix, 1, GL_FALSE, modelMatrix);
    glUniformMatrix4fv(Uniform_ViewMatrix, 1, GL_FALSE, viewMatrix);
    glUniformMatrix4fv(Uniform_ProjectionMatrix, 1, GL_FALSE, perspectiveProjectionMatrix);
    glDrawElements(GL_TRIANGLES, iNumElements, GL_UNSIGNED_SHORT, 0);
    // Sphere 4,3
    glViewport(5*50,30,(int)(present_ratio*3*16.0f),(int)(present_ratio*3*9.0f));
    modelMatrix = vmath::translate(0.0f, 0.0f, -1.3f);
    [self MaterialC4S3];
    glUniformMatrix4fv(Uniform_ModelMatrix, 1, GL_FALSE, modelMatrix);
    glUniformMatrix4fv(Uniform_ViewMatrix, 1, GL_FALSE, viewMatrix);
    glUniformMatrix4fv(Uniform_ProjectionMatrix, 1, GL_FALSE, perspectiveProjectionMatrix);
    glDrawElements(GL_TRIANGLES, iNumElements, GL_UNSIGNED_SHORT, 0);
    // Sphere 4,4
    glViewport(7*50,30,(int)(present_ratio*3*16.0f),(int)(present_ratio*3*9.0f));
    modelMatrix = vmath::translate(0.0f, 0.0f, -1.3f);
    [self MaterialC4S4];
    glUniformMatrix4fv(Uniform_ModelMatrix, 1, GL_FALSE, modelMatrix);
    glUniformMatrix4fv(Uniform_ViewMatrix, 1, GL_FALSE, viewMatrix);
    glUniformMatrix4fv(Uniform_ProjectionMatrix, 1, GL_FALSE, perspectiveProjectionMatrix);
    glDrawElements(GL_TRIANGLES, iNumElements, GL_UNSIGNED_SHORT, 0);
    // Sphere 4,5
    glViewport(9*50,30,(int)(present_ratio*3*16.0f),(int)(present_ratio*3*9.0f));
    modelMatrix = vmath::translate(0.0f, 0.0f, -1.3f);
    [self MaterialC4S5];
    glUniformMatrix4fv(Uniform_ModelMatrix, 1, GL_FALSE, modelMatrix);
    glUniformMatrix4fv(Uniform_ViewMatrix, 1, GL_FALSE, viewMatrix);
    glUniformMatrix4fv(Uniform_ProjectionMatrix, 1, GL_FALSE, perspectiveProjectionMatrix);
    glDrawElements(GL_TRIANGLES, iNumElements, GL_UNSIGNED_SHORT, 0);
    // Sphere 4,6
    glViewport(11*50,30,(int)(present_ratio*3*16.0f),(int)(present_ratio*3*9.0f));
    modelMatrix = vmath::translate(0.0f, 0.0f, -1.3f);
    [self MaterialC4S6];
    glUniformMatrix4fv(Uniform_ModelMatrix, 1, GL_FALSE, modelMatrix);
    glUniformMatrix4fv(Uniform_ViewMatrix, 1, GL_FALSE, viewMatrix);
    glUniformMatrix4fv(Uniform_ProjectionMatrix, 1, GL_FALSE, perspectiveProjectionMatrix);
    glDrawElements(GL_TRIANGLES, iNumElements, GL_UNSIGNED_SHORT, 0);
    
    glBindVertexArray(0);
    
    glUseProgram(0);
    
    glBindRenderbuffer(GL_RENDERBUFFER, colorRenderBuffer);
    [eaglContext presentRenderbuffer:GL_RENDERBUFFER];
    
    
    [self updateGeometry];
    
    
}

-(void)updateGeometry
{
    sphere_angle = sphere_angle + 0.05;
    if (sphere_angle>360.0f)
    {
        sphere_angle = 0.0f;
    }
    
    switch (iLightRotation)
    {
        case 0: // x Direction
            light_Position[0] = 0.0f;
            light_Position[1] = 100.0f*(cos(M_2_PI *sphere_angle) - sin(M_2_PI *sphere_angle));
            light_Position[2] = 100.0f * (sin(M_2_PI *sphere_angle) + cos(M_2_PI *sphere_angle));
            light_Position[3] = 1.0f;
            break;
        case 1: // Y Direction
            light_Position[0] = 100.0f * (cos((M_2_PI * sphere_angle)) + sin(M_2_PI *sphere_angle));
            light_Position[1] = 0.0f;
            light_Position[2] = 100.0f *(cos((M_2_PI *sphere_angle)) - sin((M_2_PI * sphere_angle)));
            light_Position[3] = 1.0f;
            break;
        case 2:
            light_Position[0] = 100.0f * ((cos((M_2_PI *sphere_angle)) + sin((M_2_PI *sphere_angle))));
            light_Position[1] = 100.0f * (cos((M_2_PI *sphere_angle)) - sin((M_2_PI *sphere_angle)));
            light_Position[2] = 0.0f;
            light_Position[3] = 1.0f;
            break;
            
        default:
            light_Position[0] = 0.0f ;
            light_Position[1] = 0.0f ;
            light_Position[2] = 0.0f ;
            light_Position[3] = 1.0f;
            break;
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
    
    fPresentWidth = width;
    fPresentHeight = height;
    
    perspectiveProjectionMatrix = vmath::perspective(45.0f, (GLfloat)width/(GLfloat)height, 0.1f, 100.0f);
    [self drawView:nil];
}

-(void) MaterialC1S1
{
    // Emerald
    material_Ambient[0] =  0.0215f ;
    material_Ambient[1] =  0.1745f ;
    material_Ambient[2] =  0.0215f ;
    material_Ambient[3] =  1.0f ;
    
    material_Diffuse[0] =  0.07568f ;
    material_Diffuse[1] =  0.61424f ;
    material_Diffuse[2] =  0.07568f ;
    material_Diffuse[3] =  1.0f ;
    
    material_Specular[0] =  0.633f ;
    material_Specular[1] =  0.727811f ;
    material_Specular[2] =  0.63f ;
    material_Specular[3] =  1.0f ;
    
    material_shininess = 0.6f*128.0f;
    if (bIsSingleTapDone == true)
    {
        glUniform1i(Uniform_LightEnabled, 1);
        
        glUniform3fv(Uniform_La, 1, light_Ambient);
        glUniform3fv(Uniform_Ld, 1, light_Diffuse);
        glUniform3fv(Uniform_Ls, 1, light_Specular);
        glUniform4fv(Uniform_light_Position, 1, light_Position);
        
        
        glUniform3fv(Uniform_Ka, 1, material_Ambient);
        glUniform3fv(Uniform_Kd, 1, material_Diffuse);
        glUniform3fv(Uniform_Ks, 1, material_Specular);
        glUniform1f(Uniform_Material_shininess, material_shininess);
    }
    else
    {
        glUniform1i(Uniform_LightEnabled, 0);
    }
}

-(void) MaterialC1S2
{
    //jade
    material_Ambient[0] =  0.135f ;
    material_Ambient[1] =  0.2225f ;
    material_Ambient[2] =  0.1575f ;
    material_Ambient[3] =  1.0f ;
    
    material_Diffuse[0] =  0.54f ;
    material_Diffuse[1] =  0.89f ;
    material_Diffuse[2] =  0.63f ;
    material_Diffuse[3] =  1.0f ;
    
    material_Specular[0] =  0.316228f ;
    material_Specular[1] =  0.316228f ;
    material_Specular[2] =  0.316228f ;
    material_Specular[3] =  1.0f ;
    
    material_shininess = 0.1f*128.0f;
    if (bIsSingleTapDone == true)
    {
        glUniform1i(Uniform_LightEnabled, 1);
        
        glUniform3fv(Uniform_La, 1, light_Ambient);
        glUniform3fv(Uniform_Ld, 1, light_Diffuse);
        glUniform3fv(Uniform_Ls, 1, light_Specular);
        glUniform4fv(Uniform_light_Position, 1, light_Position);
        
        
        glUniform3fv(Uniform_Ka, 1, material_Ambient);
        glUniform3fv(Uniform_Kd, 1, material_Diffuse);
        glUniform3fv(Uniform_Ks, 1, material_Specular);
        glUniform1f(Uniform_Material_shininess, material_shininess);
    }
    else
    {
        glUniform1i(Uniform_LightEnabled, 0);
    }
}

-(void) MaterialC1S3
{
    //obsidian
    material_Ambient[0] =  0.05375f ;
    material_Ambient[1] =  0.05f ;
    material_Ambient[2] =  0.06625f ;
    material_Ambient[3] =  1.0f ;
    
    material_Diffuse[0] =  0.18275f ;
    material_Diffuse[1] =  0.17f ;
    material_Diffuse[2] =  0.22525f ;
    material_Diffuse[3] =  1.0f ;
    
    material_Specular[0] =  0.332741f ;
    material_Specular[1] =  0.328634f ;
    material_Specular[2] =  0.346435f ;
    material_Specular[3] =  1.0f;
    
    material_shininess = 0.3*128.0f;
    if (bIsSingleTapDone == true)
    {
        glUniform1i(Uniform_LightEnabled, 1);
        
        glUniform3fv(Uniform_La, 1, light_Ambient);
        glUniform3fv(Uniform_Ld, 1, light_Diffuse);
        glUniform3fv(Uniform_Ls, 1, light_Specular);
        glUniform4fv(Uniform_light_Position, 1, light_Position);
        
        
        glUniform3fv(Uniform_Ka, 1, material_Ambient);
        glUniform3fv(Uniform_Kd, 1, material_Diffuse);
        glUniform3fv(Uniform_Ks, 1, material_Specular);
        glUniform1f(Uniform_Material_shininess, material_shininess);
    }
    else
    {
        glUniform1i(Uniform_LightEnabled, 0);
    }
    
}

-(void) MaterialC1S4
{
    // pearl
    material_Ambient[0] =  0.25f ;
    material_Ambient[1] =  0.20725f ;
    material_Ambient[2] =  0.20725f ;
    material_Ambient[3] =  1.0f ;
    
    material_Diffuse[0] =  1.0f ;
    material_Diffuse[1] =  0.829f ;
    material_Diffuse[2] =  0.829f ;
    material_Diffuse[3] =  1.0f ;
    
    material_Specular[0] =  0.296648f ;
    material_Specular[1] =  0.296648f ;
    material_Specular[2] =  0.296648f ;
    material_Specular[3] =  1.0f ;
    
    material_shininess = 0.88*128.0f;
    if (bIsSingleTapDone == true)
    {
        glUniform1i(Uniform_LightEnabled, 1);
        
        glUniform3fv(Uniform_La, 1, light_Ambient);
        glUniform3fv(Uniform_Ld, 1, light_Diffuse);
        glUniform3fv(Uniform_Ls, 1, light_Specular);
        glUniform4fv(Uniform_light_Position, 1, light_Position);
        
        
        glUniform3fv(Uniform_Ka, 1, material_Ambient);
        glUniform3fv(Uniform_Kd, 1, material_Diffuse);
        glUniform3fv(Uniform_Ks, 1, material_Specular);
        glUniform1f(Uniform_Material_shininess, material_shininess);
    }
    else
    {
        glUniform1i(Uniform_LightEnabled, 0);
    }
}

-(void) MaterialC1S5
{
    // ruby
    material_Ambient[0] =  0.1745f ;
    material_Ambient[1] =  0.01175f ;
    material_Ambient[2] =  0.01175f ;
    material_Ambient[3] =  1.0f ;
    
    material_Diffuse[0] =  0.61424f ;
    material_Diffuse[1] =  0.04136f ;
    material_Diffuse[2] =  0.04136f ;
    material_Diffuse[3] =  1.0f ;
    
    material_Specular[0] =  0.727811f ;
    material_Specular[1] =  0.626959f ;
    material_Specular[2] =  0.626959f ;
    material_Specular[3] =  1.0f ;
    
    material_shininess = 0.6*128.0f;
    
    if (bIsSingleTapDone == true)
    {
        glUniform1i(Uniform_LightEnabled, 1);
        
        glUniform3fv(Uniform_La, 1, light_Ambient);
        glUniform3fv(Uniform_Ld, 1, light_Diffuse);
        glUniform3fv(Uniform_Ls, 1, light_Specular);
        glUniform4fv(Uniform_light_Position, 1, light_Position);
        
        
        glUniform3fv(Uniform_Ka, 1, material_Ambient);
        glUniform3fv(Uniform_Kd, 1, material_Diffuse);
        glUniform3fv(Uniform_Ks, 1, material_Specular);
        glUniform1f(Uniform_Material_shininess, material_shininess);
    }
    else
    {
        glUniform1i(Uniform_LightEnabled, 0);
    }
    
}

-(void) MaterialC1S6
{
    // turquoise
    material_Ambient[0] =  0.1f ;
    material_Ambient[1] =  0.18725f ;
    material_Ambient[2] =  0.1745f ;
    material_Ambient[3] =  1.0f ;
    
    material_Diffuse[0] =  0.396f ;
    material_Diffuse[1] =  0.74151f ;
    material_Diffuse[2] =  0.69102f ;
    material_Diffuse[3] =  1.0f ;
    
    material_Specular[0] =  0.297254f ;
    material_Specular[1] =  0.30829f ;
    material_Specular[2] =  0.306678f ;
    material_Specular[3] =  1.0f ;
    
    material_shininess = 0.1*128.0f;
    
    if (bIsSingleTapDone == true)
    {
        glUniform1i(Uniform_LightEnabled, 1);
        
        glUniform3fv(Uniform_La, 1, light_Ambient);
        glUniform3fv(Uniform_Ld, 1, light_Diffuse);
        glUniform3fv(Uniform_Ls, 1, light_Specular);
        glUniform4fv(Uniform_light_Position, 1, light_Position);
        
        
        glUniform3fv(Uniform_Ka, 1, material_Ambient);
        glUniform3fv(Uniform_Kd, 1, material_Diffuse);
        glUniform3fv(Uniform_Ks, 1, material_Specular);
        glUniform1f(Uniform_Material_shininess, material_shininess);
    }
    else
    {
        glUniform1i(Uniform_LightEnabled, 0);
    }
    
}

-(void) MaterialC2S1
{
    //brass
    material_Ambient[0] =  0.329412f ;
    material_Ambient[1] =  0.223529f ;
    material_Ambient[2] =  0.027451f ;
    material_Ambient[3] =  1.0f ;
    
    material_Diffuse[0] =  0.780392f ;
    material_Diffuse[1] =  0.568627f ;
    material_Diffuse[2] =  0.113725f ;
    material_Diffuse[3] =  1.0f ;
    
    material_Specular[0] =  0.992157f ;
    material_Specular[1] =  0.941176f ;
    material_Specular[2] =  0.807843f ;
    material_Specular[3] =  1.0f ;
    
    material_shininess = 0.21794872f*128.0f;
    
    if (bIsSingleTapDone == true)
    {
        glUniform1i(Uniform_LightEnabled, 1);
        
        glUniform3fv(Uniform_La, 1, light_Ambient);
        glUniform3fv(Uniform_Ld, 1, light_Diffuse);
        glUniform3fv(Uniform_Ls, 1, light_Specular);
        glUniform4fv(Uniform_light_Position, 1, light_Position);
        
        
        glUniform3fv(Uniform_Ka, 1, material_Ambient);
        glUniform3fv(Uniform_Kd, 1, material_Diffuse);
        glUniform3fv(Uniform_Ks, 1, material_Specular);
        glUniform1f(Uniform_Material_shininess, material_shininess);
    }
    else
    {
        glUniform1i(Uniform_LightEnabled, 0);
    }
    
}

-(void) MaterialC2S2
{
    //bronze
    material_Ambient[0] =  0.2125f ;
    material_Ambient[1] =  0.1275f ;
    material_Ambient[2] =  0.054f ;
    material_Ambient[3] =  1.0f ;
    
    material_Diffuse[0] =  0.714f ;
    material_Diffuse[1] =  0.4284f ;
    material_Diffuse[2] =  0.18144f ;
    material_Diffuse[3] =  1.0f ;
    
    material_Specular[0] =  0.393548f ;
    material_Specular[1] =  0.271906f ;
    material_Specular[2] =  0.166721f ;
    material_Specular[3] =  1.0f ;
    
    material_shininess = 0.2*128.0f;
    
    if (bIsSingleTapDone == true)
    {
        glUniform1i(Uniform_LightEnabled, 1);
        
        glUniform3fv(Uniform_La, 1, light_Ambient);
        glUniform3fv(Uniform_Ld, 1, light_Diffuse);
        glUniform3fv(Uniform_Ls, 1, light_Specular);
        glUniform4fv(Uniform_light_Position, 1, light_Position);
        
        
        glUniform3fv(Uniform_Ka, 1, material_Ambient);
        glUniform3fv(Uniform_Kd, 1, material_Diffuse);
        glUniform3fv(Uniform_Ks, 1, material_Specular);
        glUniform1f(Uniform_Material_shininess, material_shininess);
    }
    else
    {
        glUniform1i(Uniform_LightEnabled, 0);
    }
    
}

-(void) MaterialC2S3
{
    //chrome
    material_Ambient[0] =  0.25f ;
    material_Ambient[1] =  0.25f ;
    material_Ambient[2] =  0.25f ;
    material_Ambient[3] =  1.0f ;
    
    material_Diffuse[0] =  0.4f ;
    material_Diffuse[1] =  0.4f ;
    material_Diffuse[2] =  0.4f ;
    material_Diffuse[3] =  1.0f ;
    
    material_Specular[0] =  0.774597f ;
    material_Specular[1] =  0.774597f ;
    material_Specular[2] =  0.774597f ;
    material_Specular[3] =  1.0f ;
    
    material_shininess = 0.6*128.0f;
    
    if (bIsSingleTapDone == true)
    {
        glUniform1i(Uniform_LightEnabled, 1);
        
        glUniform3fv(Uniform_La, 1, light_Ambient);
        glUniform3fv(Uniform_Ld, 1, light_Diffuse);
        glUniform3fv(Uniform_Ls, 1, light_Specular);
        glUniform4fv(Uniform_light_Position, 1, light_Position);
        
        
        glUniform3fv(Uniform_Ka, 1, material_Ambient);
        glUniform3fv(Uniform_Kd, 1, material_Diffuse);
        glUniform3fv(Uniform_Ks, 1, material_Specular);
        glUniform1f(Uniform_Material_shininess, material_shininess);
    }
    else
    {
        glUniform1i(Uniform_LightEnabled, 0);
    }
    
}

-(void) MaterialC2S4
{
    //copper
    material_Ambient[0] =  0.19125f ;
    material_Ambient[1] =  0.0735f ;
    material_Ambient[2] =  0.0225f ;
    material_Ambient[3] =  1.0f ;
    
    material_Diffuse[0] =  0.7038f ;
    material_Diffuse[1] =  0.27048f ;
    material_Diffuse[2] =  0.0828f ;
    material_Diffuse[3] =  1.0f ;
    
    material_Specular[0] =  0.256777f ;
    material_Specular[1] =  0.137622f ;
    material_Specular[2] =  0.086014f ;
    material_Specular[3] =  1.0f ;
    
    material_shininess = 0.1*128.0f;
    
    if (bIsSingleTapDone == true)
    {
        glUniform1i(Uniform_LightEnabled, 1);
        
        glUniform3fv(Uniform_La, 1, light_Ambient);
        glUniform3fv(Uniform_Ld, 1, light_Diffuse);
        glUniform3fv(Uniform_Ls, 1, light_Specular);
        glUniform4fv(Uniform_light_Position, 1, light_Position);
        
        
        glUniform3fv(Uniform_Ka, 1, material_Ambient);
        glUniform3fv(Uniform_Kd, 1, material_Diffuse);
        glUniform3fv(Uniform_Ks, 1, material_Specular);
        glUniform1f(Uniform_Material_shininess, material_shininess);
    }
    else
    {
        glUniform1i(Uniform_LightEnabled, 0);
    }
    
}

-(void) MaterialC2S5
{
    //gold
    material_Ambient[0] =  0.24725f ;
    material_Ambient[1] =  0.1995f ;
    material_Ambient[2] =  0.0745f ;
    material_Ambient[3] =  1.0f ;
    
    material_Diffuse[0] =  0.75164f ;
    material_Diffuse[1] =  0.60648f ;
    material_Diffuse[2] =  0.22648f ;
    material_Diffuse[3] =  1.0f ;
    
    material_Specular[0] =  0.628281f ;
    material_Specular[1] =  0.555802f ;
    material_Specular[2] =  0.366065f ;
    material_Specular[3] =  1.0f ;
    
    material_shininess = 0.4*128.0f;
    
    if (bIsSingleTapDone == true)
    {
        glUniform1i(Uniform_LightEnabled, 1);
        
        glUniform3fv(Uniform_La, 1, light_Ambient);
        glUniform3fv(Uniform_Ld, 1, light_Diffuse);
        glUniform3fv(Uniform_Ls, 1, light_Specular);
        glUniform4fv(Uniform_light_Position, 1, light_Position);
        
        
        glUniform3fv(Uniform_Ka, 1, material_Ambient);
        glUniform3fv(Uniform_Kd, 1, material_Diffuse);
        glUniform3fv(Uniform_Ks, 1, material_Specular);
        glUniform1f(Uniform_Material_shininess, material_shininess);
    }
    else
    {
        glUniform1i(Uniform_LightEnabled, 0);
    }
    
}

-(void) MaterialC2S6
{
    //silver
    material_Ambient[0] =  0.19225f ;
    material_Ambient[1] =  0.19225f ;
    material_Ambient[2] =  0.19225f ;
    material_Ambient[3] =  1.0f ;
    
    material_Diffuse[0] =  0.50754f ;
    material_Diffuse[1] =  0.50754f ;
    material_Diffuse[2] =  0.50754f ;
    material_Diffuse[3] =  1.0f ;
    
    material_Specular[0] =  0.508273f ;
    material_Specular[1] =  0.508273f ;
    material_Specular[2] =  0.508273f ;
    material_Specular[3] =  1.0f ;
    
    material_shininess = 0.4*128.0f;
    
    if (bIsSingleTapDone == true)
    {
        glUniform1i(Uniform_LightEnabled, 1);
        
        glUniform3fv(Uniform_La, 1, light_Ambient);
        glUniform3fv(Uniform_Ld, 1, light_Diffuse);
        glUniform3fv(Uniform_Ls, 1, light_Specular);
        glUniform4fv(Uniform_light_Position, 1, light_Position);
        
        
        glUniform3fv(Uniform_Ka, 1, material_Ambient);
        glUniform3fv(Uniform_Kd, 1, material_Diffuse);
        glUniform3fv(Uniform_Ks, 1, material_Specular);
        glUniform1f(Uniform_Material_shininess, material_shininess);
    }
    else
    {
        glUniform1i(Uniform_LightEnabled, 0);
    }
    
}

-(void) MaterialC3S1
{
    // black
    material_Ambient[0] =  0.0f ;
    material_Ambient[1] =  0.0f ;
    material_Ambient[2] =  0.0f ;
    material_Ambient[3] =  1.0f ;
    
    material_Diffuse[0] =  0.01f ;
    material_Diffuse[1] =  0.01f ;
    material_Diffuse[2] =  0.01f ;
    material_Diffuse[3] =  1.0f ;
    
    material_Specular[0] =  0.50f ;
    material_Specular[1] =  0.50f ;
    material_Specular[2] =  0.50f ;
    material_Specular[3] =  1.0f ;
    
    material_shininess = 0.25f*128.0f;
    
    if (bIsSingleTapDone == true)
    {
        glUniform1i(Uniform_LightEnabled, 1);
        
        glUniform3fv(Uniform_La, 1, light_Ambient);
        glUniform3fv(Uniform_Ld, 1, light_Diffuse);
        glUniform3fv(Uniform_Ls, 1, light_Specular);
        glUniform4fv(Uniform_light_Position, 1, light_Position);
        
        
        glUniform3fv(Uniform_Ka, 1, material_Ambient);
        glUniform3fv(Uniform_Kd, 1, material_Diffuse);
        glUniform3fv(Uniform_Ks, 1, material_Specular);
        glUniform1f(Uniform_Material_shininess, material_shininess);
    }
    else
    {
        glUniform1i(Uniform_LightEnabled, 0);
    }
    
}

-(void) MaterialC3S2
{
    // cyan
    material_Ambient[0] = 0.0f ;
    material_Ambient[1] =  0.1f ;
    material_Ambient[2] =  0.06f ;
    material_Ambient[3] =  1.0f ;
    
    material_Diffuse[0] =  0.0f ;
    material_Diffuse[1] =  0.50980392f ;
    material_Diffuse[2] =  0.50980392f ;
    material_Diffuse[3] =  1.0f ;
    
    material_Specular[0] =  0.50196078f ;
    material_Specular[1] =  0.50196078f ;
    material_Specular[2] =  0.50196078f ;
    material_Specular[3] =  1.0f ;
    
    material_shininess = 0.25f*128.0f;
    
    if (bIsSingleTapDone == true)
    {
        glUniform1i(Uniform_LightEnabled, 1);
        
        glUniform3fv(Uniform_La, 1, light_Ambient);
        glUniform3fv(Uniform_Ld, 1, light_Diffuse);
        glUniform3fv(Uniform_Ls, 1, light_Specular);
        glUniform4fv(Uniform_light_Position, 1, light_Position);
        
        
        glUniform3fv(Uniform_Ka, 1, material_Ambient);
        glUniform3fv(Uniform_Kd, 1, material_Diffuse);
        glUniform3fv(Uniform_Ks, 1, material_Specular);
        glUniform1f(Uniform_Material_shininess, material_shininess);
    }
    else
    {
        glUniform1i(Uniform_LightEnabled, 0);
    }
    
}

-(void) MaterialC3S3
{
    // green
    material_Ambient[0] =  0.0f ;
    material_Ambient[1] =  0.0f ;
    material_Ambient[2] =  0.0f ;
    material_Ambient[3] =  1.0f ;
    
    material_Diffuse[0] =  0.1f ;
    material_Diffuse[1] =  0.35f ;
    material_Diffuse[2] =  0.1f ;
    material_Diffuse[3] =  1.0f ;
    
    material_Specular[0] =  0.45f ;
    material_Specular[1] =  0.55f ;
    material_Specular[2] =  0.45f ;
    material_Specular[3] =  1.0f ;
    
    material_shininess = 0.25f*128.0f;
    
    if (bIsSingleTapDone == true)
    {
        glUniform1i(Uniform_LightEnabled, 1);
        
        glUniform3fv(Uniform_La, 1, light_Ambient);
        glUniform3fv(Uniform_Ld, 1, light_Diffuse);
        glUniform3fv(Uniform_Ls, 1, light_Specular);
        glUniform4fv(Uniform_light_Position, 1, light_Position);
        
        
        glUniform3fv(Uniform_Ka, 1, material_Ambient);
        glUniform3fv(Uniform_Kd, 1, material_Diffuse);
        glUniform3fv(Uniform_Ks, 1, material_Specular);
        glUniform1f(Uniform_Material_shininess, material_shininess);
    }
    else
    {
        glUniform1i(Uniform_LightEnabled, 0);
    }
    
}

-(void) MaterialC3S4
{
    // red
    material_Ambient[0] =  0.0f ;
    material_Ambient[1] =  0.0f ;
    material_Ambient[2] =  0.0f ;
    material_Ambient[3] =  1.0f ;
    
    material_Diffuse[0] =  0.5f ;
    material_Diffuse[1] =  0.0f ;
    material_Diffuse[2] =  0.0f ;
    material_Diffuse[3] =  1.0f ;
    
    material_Specular[0] =  0.7f ;
    material_Specular[1] =  0.6f ;
    material_Specular[2] =  0.6f ;
    material_Specular[3] =  1.0f ;
    
    material_shininess = 0.25f*128.0f;
    
    if (bIsSingleTapDone == true)
    {
        glUniform1i(Uniform_LightEnabled, 1);
        
        glUniform3fv(Uniform_La, 1, light_Ambient);
        glUniform3fv(Uniform_Ld, 1, light_Diffuse);
        glUniform3fv(Uniform_Ls, 1, light_Specular);
        glUniform4fv(Uniform_light_Position, 1, light_Position);
        
        
        glUniform3fv(Uniform_Ka, 1, material_Ambient);
        glUniform3fv(Uniform_Kd, 1, material_Diffuse);
        glUniform3fv(Uniform_Ks, 1, material_Specular);
        glUniform1f(Uniform_Material_shininess, material_shininess);
    }
    else
    {
        glUniform1i(Uniform_LightEnabled, 0);
    }
    
}

-(void) MaterialC3S5
{
    // white
    material_Ambient[0] =  0.0f ;
    material_Ambient[1] =  0.0f ;
    material_Ambient[2] =  0.0f ;
    material_Ambient[3] =  1.0f ;
    
    material_Diffuse[0] =  0.55f ;
    material_Diffuse[1] =  0.55f ;
    material_Diffuse[2] =  0.55f ;
    material_Diffuse[3] =  1.0f ;
    
    material_Specular[0] =  0.70f ;
    material_Specular[1] =  0.70f ;
    material_Specular[2] =  0.70f ;
    material_Specular[3] =  1.0f ;
    
    material_shininess = 0.25f*128.0f;
    
    if (bIsSingleTapDone == true)
    {
        glUniform1i(Uniform_LightEnabled, 1);
        
        glUniform3fv(Uniform_La, 1, light_Ambient);
        glUniform3fv(Uniform_Ld, 1, light_Diffuse);
        glUniform3fv(Uniform_Ls, 1, light_Specular);
        glUniform4fv(Uniform_light_Position, 1, light_Position);
        
        
        glUniform3fv(Uniform_Ka, 1, material_Ambient);
        glUniform3fv(Uniform_Kd, 1, material_Diffuse);
        glUniform3fv(Uniform_Ks, 1, material_Specular);
        glUniform1f(Uniform_Material_shininess, material_shininess);
    }
    else
    {
        glUniform1i(Uniform_LightEnabled, 0);
    }
    
}

-(void) MaterialC3S6
{
    // yellow plastic
    material_Ambient[0] =  0.0f ;
    material_Ambient[1] =  0.0f ;
    material_Ambient[2] =  0.0f ;
    material_Ambient[3] =  1.0f ;
    
    material_Diffuse[0] =  0.5f ;
    material_Diffuse[1] =  0.5f ;
    material_Diffuse[2] =  0.0f ;
    material_Diffuse[3] =  1.0f ;
    
    material_Specular[0] =  0.60f ;
    material_Specular[1] =  0.60f ;
    material_Specular[2] =  0.50f ;
    material_Specular[3] =  1.0f ;
    
    material_shininess = 0.25f*128.0f;
    
    if (bIsSingleTapDone == true)
    {
        glUniform1i(Uniform_LightEnabled, 1);
        
        glUniform3fv(Uniform_La, 1, light_Ambient);
        glUniform3fv(Uniform_Ld, 1, light_Diffuse);
        glUniform3fv(Uniform_Ls, 1, light_Specular);
        glUniform4fv(Uniform_light_Position, 1, light_Position);
        
        
        glUniform3fv(Uniform_Ka, 1, material_Ambient);
        glUniform3fv(Uniform_Kd, 1, material_Diffuse);
        glUniform3fv(Uniform_Ks, 1, material_Specular);
        glUniform1f(Uniform_Material_shininess, material_shininess);
    }
    else
    {
        glUniform1i(Uniform_LightEnabled, 0);
    }
    
}

-(void) MaterialC4S1
{
    // black
    material_Ambient[0] = 0.02f;
    material_Ambient[1] = 0.02f;
    material_Ambient[2] = 0.02f;
    material_Ambient[3] = 1.0f;
    
    material_Diffuse[0] = 0.01f;
    material_Diffuse[1] = 0.01f;
    material_Diffuse[2] = 0.01f;
    material_Diffuse[3] = 1.0f;
    
    material_Specular[0] = 0.40f;
    material_Specular[1] = 0.40f;
    material_Specular[2] = 0.40f;
    material_Specular[3] = 1.0f;
    
    material_shininess = 0.078125f*128.0f;
    
    if (bIsSingleTapDone == true)
    {
        glUniform1i(Uniform_LightEnabled, 1);
        
        glUniform3fv(Uniform_La, 1, light_Ambient);
        glUniform3fv(Uniform_Ld, 1, light_Diffuse);
        glUniform3fv(Uniform_Ls, 1, light_Specular);
        glUniform4fv(Uniform_light_Position, 1, light_Position);
        
        
        glUniform3fv(Uniform_Ka, 1, material_Ambient);
        glUniform3fv(Uniform_Kd, 1, material_Diffuse);
        glUniform3fv(Uniform_Ks, 1, material_Specular);
        glUniform1f(Uniform_Material_shininess, material_shininess);
    }
    else
    {
        glUniform1i(Uniform_LightEnabled, 0);
    }
    
}

-(void) MaterialC4S2
{
    // cyan
    material_Ambient[0] = 0.0f;
    material_Ambient[1] = 0.05f;
    material_Ambient[2] = 0.05f;
    material_Ambient[3] = 1.0f;
    
    material_Diffuse[0] = 0.04f;
    material_Diffuse[1] = 0.05f;
    material_Diffuse[2] = 0.05f;
    material_Diffuse[3] = 1.0f;
    
    material_Specular[0] = 0.04f;
    material_Specular[1] = 0.7f;
    material_Specular[2] = 0.7f;
    material_Specular[3] = 1.0f;
    
    material_shininess = 0.078125f*128.0f;
    
    if (bIsSingleTapDone == true)
    {
        glUniform1i(Uniform_LightEnabled, 1);
        
        glUniform3fv(Uniform_La, 1, light_Ambient);
        glUniform3fv(Uniform_Ld, 1, light_Diffuse);
        glUniform3fv(Uniform_Ls, 1, light_Specular);
        glUniform4fv(Uniform_light_Position, 1, light_Position);
        
        
        glUniform3fv(Uniform_Ka, 1, material_Ambient);
        glUniform3fv(Uniform_Kd, 1, material_Diffuse);
        glUniform3fv(Uniform_Ks, 1, material_Specular);
        glUniform1f(Uniform_Material_shininess, material_shininess);
    }
    else
    {
        glUniform1i(Uniform_LightEnabled, 0);
    }
    
}

-(void) MaterialC4S3
{
    // green
    material_Ambient[0] = 0.0f;
    material_Ambient[1] = 0.05f;
    material_Ambient[2] = 0.0f;
    material_Ambient[3] = 1.0f;
    
    material_Diffuse[0] = 0.4f;
    material_Diffuse[1] = 0.5f;
    material_Diffuse[2] = 0.5f;
    material_Diffuse[3] = 1.0f;
    
    material_Specular[0] = 0.04f;
    material_Specular[1] = 0.7f;
    material_Specular[2] = 0.04f;
    material_Specular[3] = 1.0f;
    
    material_shininess = 0.078125f*128.0f;
    
    if (bIsSingleTapDone == true)
    {
        glUniform1i(Uniform_LightEnabled, 1);
        
        glUniform3fv(Uniform_La, 1, light_Ambient);
        glUniform3fv(Uniform_Ld, 1, light_Diffuse);
        glUniform3fv(Uniform_Ls, 1, light_Specular);
        glUniform4fv(Uniform_light_Position, 1, light_Position);
        
        
        glUniform3fv(Uniform_Ka, 1, material_Ambient);
        glUniform3fv(Uniform_Kd, 1, material_Diffuse);
        glUniform3fv(Uniform_Ks, 1, material_Specular);
        glUniform1f(Uniform_Material_shininess, material_shininess);
    }
    else
    {
        glUniform1i(Uniform_LightEnabled, 0);
    }
    
}

-(void) MaterialC4S4
{
    // red
    material_Ambient[0] = 0.05f;
    material_Ambient[1] = 0.0f;
    material_Ambient[2] = 0.0f;
    material_Ambient[3] = 1.0f;
    
    material_Diffuse[0] = 0.5f;
    material_Diffuse[1] = 0.4f;
    material_Diffuse[2] = 0.4f;
    material_Diffuse[3] = 1.0f;
    
    material_Specular[0] = 0.7f;
    material_Specular[1] = 0.04f;
    material_Specular[2] = 0.04f;
    material_Specular[3] = 1.0f;
    
    material_shininess = 0.078125f*128.0f;
    
    if (bIsSingleTapDone == true)
    {
        glUniform1i(Uniform_LightEnabled, 1);
        
        glUniform3fv(Uniform_La, 1, light_Ambient);
        glUniform3fv(Uniform_Ld, 1, light_Diffuse);
        glUniform3fv(Uniform_Ls, 1, light_Specular);
        glUniform4fv(Uniform_light_Position, 1, light_Position);
        
        
        glUniform3fv(Uniform_Ka, 1, material_Ambient);
        glUniform3fv(Uniform_Kd, 1, material_Diffuse);
        glUniform3fv(Uniform_Ks, 1, material_Specular);
        glUniform1f(Uniform_Material_shininess, material_shininess);
    }
    else
    {
        glUniform1i(Uniform_LightEnabled, 0);
    }
    
}

-(void) MaterialC4S5
{
    // white
    material_Ambient[0] = 0.05f;
    material_Ambient[1] = 0.05f;
    material_Ambient[2] = 0.05f;
    material_Ambient[3] = 1.0f;
    
    material_Diffuse[0] = 0.5f;
    material_Diffuse[1] = 0.5f;
    material_Diffuse[2] = 0.5f;
    material_Diffuse[3] = 1.0f;
    
    material_Specular[0] = 0.7f;
    material_Specular[1] = 0.7f;
    material_Specular[2] = 0.7f;
    material_Specular[3] = 1.0f;
    
    material_shininess = 0.078125f*128.0f;
    
    if (bIsSingleTapDone == true)
    {
        glUniform1i(Uniform_LightEnabled, 1);
        
        glUniform3fv(Uniform_La, 1, light_Ambient);
        glUniform3fv(Uniform_Ld, 1, light_Diffuse);
        glUniform3fv(Uniform_Ls, 1, light_Specular);
        glUniform4fv(Uniform_light_Position, 1, light_Position);
        
        
        glUniform3fv(Uniform_Ka, 1, material_Ambient);
        glUniform3fv(Uniform_Kd, 1, material_Diffuse);
        glUniform3fv(Uniform_Ks, 1, material_Specular);
        glUniform1f(Uniform_Material_shininess, material_shininess);
    }
    else
    {
        glUniform1i(Uniform_LightEnabled, 0);
    }
    
}

-(void) MaterialC4S6
{
    // yellow rubber
    material_Ambient[0] = 0.05f;
    material_Ambient[1] = 0.05f;
    material_Ambient[2] = 0.0f;
    material_Ambient[3] = 1.0f;
    
    material_Diffuse[0] = 0.5f;
    material_Diffuse[1] = 0.5f;
    material_Diffuse[2] = 0.4f;
    material_Diffuse[3] = 1.0f;
    
    material_Specular[0] = 0.7f;
    material_Specular[1] = 0.7f;
    material_Specular[2] = 0.04f;
    material_Specular[3] = 1.0f;
    
    material_shininess = 0.078125f*128.0f;
    
    if (bIsSingleTapDone == true)
    {
        glUniform1i(Uniform_LightEnabled, 1);
        
        glUniform3fv(Uniform_La, 1, light_Ambient);
        glUniform3fv(Uniform_Ld, 1, light_Diffuse);
        glUniform3fv(Uniform_Ls, 1, light_Specular);
        glUniform4fv(Uniform_light_Position, 1, light_Position);
        
        
        glUniform3fv(Uniform_Ka, 1, material_Ambient);
        glUniform3fv(Uniform_Kd, 1, material_Diffuse);
        glUniform3fv(Uniform_Ks, 1, material_Specular);
        glUniform1f(Uniform_Material_shininess, material_shininess);
    }
    else
    {
        glUniform1i(Uniform_LightEnabled, 0);
    }
    
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
    if(bShaderToggleFlag==NO)
        bShaderToggleFlag=YES;
    else
        bShaderToggleFlag=NO;
    
    printf("In onLongPress \n");
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
    
    
    iLightRotation++;
    if(iLightRotation>=3)
        iLightRotation=0;
    
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
    
    
    
    
    printf("In OnSingleTap \n");
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


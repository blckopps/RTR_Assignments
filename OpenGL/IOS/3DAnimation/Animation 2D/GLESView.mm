


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
    GLuint gShaderProgramObject;
    
    GLuint vao_Cube;
    
    GLuint vbo_Cube_Position;
    GLuint vbo_Cube_Color;
    
    GLuint vao_pyramid;
    
    GLuint vbo_pyramid_pos;
    GLuint vbo_pyramid_color;
    
    GLuint Uniform_MVP;
    
    GLfloat cube_angle;
    GLfloat pyramid_angle;
    
    vmath::mat4 perspectiveProjrctionMatrix;
    
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
        // Shader Information End
        
        
        
        //***** Vertex Shader *****
        //Create Shader
        vertexShaderObject = glCreateShader( GL_VERTEX_SHADER );
        
        // give source code to shader
        const GLchar *vertexShaderSourceCode = "#version 300 es"    \
        "\n" \
        "in vec4 vPosition;" \
        "in vec4 vColor;" \
        "out vec4 out_Color;" \
        "uniform mat4 u_mvp_matrix;" \
        "void main(void)" \
        "{"    \
        "gl_Position = u_mvp_matrix *vPosition ;"    \
        "out_Color = vColor;" \
        "}";
        
        glShaderSource( vertexShaderObject, 1, (const GLchar**) &vertexShaderSourceCode, NULL );
        
        // Compile Source Code
        glCompileShader( vertexShaderObject );
        GLint iInfoLogLength = 0;
        GLint iShaderCompileStatus = 0;
        
        char *szInfoLog = NULL;
        glGetShaderiv( vertexShaderObject, GL_COMPILE_STATUS, &iShaderCompileStatus );
        if (iShaderCompileStatus == GL_FALSE)
        {
            glGetShaderiv( vertexShaderObject, GL_INFO_LOG_LENGTH, &iInfoLogLength );
            if (iInfoLogLength > 0)
            {
                szInfoLog = (char*) malloc( iInfoLogLength );
                if (szInfoLog != NULL)
                {
                    GLsizei written;
                    glGetShaderInfoLog(vertexShaderObject,iInfoLogLength,&written,szInfoLog);
                    printf("Vertex Shader Compilation Log: %s\n",szInfoLog);
                    free(szInfoLog);
                    [self release];
                }
                
            }
            
        }
        
        fragmentShaderObject= glCreateShader(GL_FRAGMENT_SHADER);
        
        
        const GLchar * gFragmentShaderSourceCode =
        "#version 300 es" \
        "\n" \
        "precision highp float;"\
        "out vec4 FragColor;" \
        "in vec4 out_Color;" \
        "void main (void)" \
        "{"    \
        "FragColor = vec4(out_Color);" \
        "}";
        glShaderSource(fragmentShaderObject,1,(const GLchar **)&gFragmentShaderSourceCode,NULL);
        glCompileShader(fragmentShaderObject);
        
        glGetShaderiv(fragmentShaderObject,GL_COMPILE_STATUS,&iShaderCompileStatus);
        if(iShaderCompileStatus ==GL_FALSE)
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
        
        gShaderProgramObject= glCreateProgram();
        glAttachShader(gShaderProgramObject,vertexShaderObject);
        glAttachShader(gShaderProgramObject,fragmentShaderObject);
        
        //bind in variables before linking
        glBindAttribLocation(gShaderProgramObject,AMC_ATTRIBUTE_VERTEX,"vPosition");
        glBindAttribLocation(gShaderProgramObject,AMC_ATTRIBUTE_COLOR,"vColor");
        
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
        
        
        //get uniform location
        Uniform_MVP = glGetUniformLocation(gShaderProgramObject, "u_mvp_matrix");
        
        // **** Verttices, Colors, Shader Attribs, Vbo, Vao Initializations ****
        const GLfloat pyramid_vertices[]=
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
        
        const GLfloat pyramid_color [] =
        {
            1.0f, 0.0f, 0.0f,
            0.0f, 1.0f, 0.0f,
            0.0f, 0.0f, 1.0f,
            
            1.0f, 0.0f, 0.0f,
            0.0f, 1.0f, 0.0f,
            0.0f, 0.0f, 1.0f,
            
            1.0f, 0.0f, 0.0f,
            0.0f, 1.0f, 0.0f,
            0.0f, 0.0f, 1.0f,
            
            1.0f, 0.0f, 0.0f,
            0.0f, 1.0f, 0.0f,
            0.0f, 0.0f, 1.0f
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
        
        
        const GLfloat cube_color[] =
        {
            1.0f, 0.0f, 0.0f,
            1.0f, 0.0f, 0.0f,
            1.0f, 0.0f, 0.0f,
            1.0f, 0.0f, 0.0f,
            
            0.0f, 1.0f, 0.0f,
            0.0f, 1.0f, 0.0f,
            0.0f, 1.0f, 0.0f,
            0.0f, 1.0f, 0.0f,
            
            0.0f, 0.0f, 1.0f,
            0.0f, 0.0f, 1.0f,
            0.0f, 0.0f, 1.0f,
            0.0f, 0.0f, 1.0f,
            
            1.0f, 1.0f, 0.0f,
            1.0f, 1.0f, 0.0f,
            1.0f, 1.0f, 0.0f,
            1.0f, 1.0f, 0.0f,
            
            1.0f, 0.0f, 1.0f,
            1.0f, 0.0f, 1.0f,
            1.0f, 0.0f, 1.0f,
            1.0f, 0.0f, 1.0f,
            
            0.0f, 1.0f, 1.0f,
            0.0f, 1.0f, 1.0f,
            0.0f, 1.0f, 1.0f,
            0.0f, 1.0f, 1.0f,
        };
        
        // vao creation and binding
        glGenVertexArrays( 1, &vao_Cube );
        glBindVertexArray( vao_Cube );
        
        //vbo creation and binding
        glGenBuffers( 1, &vbo_Cube_Position );
        glBindBuffer( GL_ARRAY_BUFFER, vbo_Cube_Position );
        glBufferData( GL_ARRAY_BUFFER, sizeof( cube_vertices ), cube_vertices, GL_STATIC_DRAW );
        glVertexAttribPointer( AMC_ATTRIBUTE_VERTEX, 3, GL_FLOAT, GL_FALSE, 0, NULL );
        glEnableVertexAttribArray( AMC_ATTRIBUTE_VERTEX );
        glBindBuffer( GL_ARRAY_BUFFER, 0 );
        
        //Colors
        glGenBuffers(1,&vbo_Cube_Color);
        glBindBuffer(GL_ARRAY_BUFFER,vbo_Cube_Color);
        glBufferData(GL_ARRAY_BUFFER,sizeof(cube_color),cube_color,GL_STATIC_DRAW);
        glVertexAttribPointer(AMC_ATTRIBUTE_COLOR,3,GL_FLOAT,GL_FALSE,0,NULL);
        glEnableVertexAttribArray(AMC_ATTRIBUTE_COLOR);
        glBindBuffer(GL_ARRAY_BUFFER,0);
        
        glBindVertexArray(0);
        
        //////////////pyramid////////////
        
        glGenVertexArrays(1,&vao_pyramid);
        glBindVertexArray(vao_pyramid);
        
        glGenBuffers(1, &vbo_pyramid_pos);
        glBindBuffer(GL_ARRAY_BUFFER, vbo_pyramid_pos);
        glBufferData(GL_ARRAY_BUFFER, sizeof(pyramid_vertices), pyramid_vertices, GL_STATIC_DRAW);
        glVertexAttribPointer(AMC_ATTRIBUTE_VERTEX, 3, GL_FLOAT, GL_FALSE, 0, NULL);
        glEnableVertexAttribArray(AMC_ATTRIBUTE_VERTEX);
        
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        
        //Color
        glGenBuffers(1, &vbo_pyramid_color);
        glBindBuffer(GL_ARRAY_BUFFER, vbo_pyramid_color);
        glBufferData(GL_ARRAY_BUFFER, sizeof(pyramid_color), pyramid_color, GL_STATIC_DRAW);
        glVertexAttribPointer(AMC_ATTRIBUTE_COLOR, 3, GL_FLOAT, GL_FALSE, 0, NULL);
        glEnableVertexAttribArray(AMC_ATTRIBUTE_COLOR);
        
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        
        glBindVertexArray(0);
        
        
        
        //glEnable(GL_DEPTH_TEST);
        //glDepthFunc(GL_LEQUAL);
        //glEnable(GL_CULL_FACE);
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LEQUAL);
        pyramid_angle = 360.0f;
        cube_angle = 0.0f;
        
        perspectiveProjrctionMatrix = vmath::mat4::identity();
        
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
    
    glUseProgram(gShaderProgramObject);
    
    // Triangle Section
    vmath::mat4 modelViewMatrix = vmath::mat4::identity();
    vmath::mat4 rotationMatrix = vmath::mat4::identity();
    modelViewMatrix = vmath::translate(-1.8f, 0.0f, -6.0f);
    rotationMatrix = vmath::rotate(cube_angle,1.0f,0.0f,0.0f);
    rotationMatrix *= vmath::rotate(cube_angle,0.0f,1.0f,0.0f);
    rotationMatrix *= vmath::rotate(cube_angle,0.0f,0.0f,1.0f);
    
    modelViewMatrix = modelViewMatrix * rotationMatrix;
    vmath::mat4 modelViewProjectionMatrix = perspectiveProjrctionMatrix * modelViewMatrix;
    
    glUniformMatrix4fv(Uniform_MVP, 1, GL_FALSE, modelViewProjectionMatrix);
    
    
    glBindVertexArray(vao_Cube);
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
    
    // pyramid
    
    modelViewMatrix = vmath::mat4::identity();
    modelViewProjectionMatrix = vmath::mat4::identity();
    rotationMatrix = vmath::mat4::identity();
    modelViewMatrix = vmath::translate(1.8f, 0.0f, -6.0f);
    rotationMatrix = vmath::rotate(cube_angle,0.0f,1.0f,0.0f);
    modelViewMatrix = modelViewMatrix * rotationMatrix;
    modelViewProjectionMatrix = perspectiveProjrctionMatrix * modelViewMatrix;
    
    glUniformMatrix4fv(Uniform_MVP, 1, GL_FALSE, modelViewProjectionMatrix);
    
    
    glBindVertexArray(vao_pyramid);
    
    glDrawArrays(GL_TRIANGLES,    //what to draw from given array
                 0,                //from where to start
                 12);
    
    glBindVertexArray(0);
    
    glUseProgram(0);
    
    glBindRenderbuffer(GL_RENDERBUFFER, colorRenderBuffer);
    [eaglContext presentRenderbuffer:GL_RENDERBUFFER];
    
    [self updateGeometry];
}

-(void)updateGeometry
{
    cube_angle = cube_angle + 0.5f;
    if (cube_angle >= 360.0f)
    {
        cube_angle=0.0f;
    }
    
    pyramid_angle = pyramid_angle - 0.5f;
    if (pyramid_angle <= 0.0f)
    {
        pyramid_angle=360.0f;
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
    
    GLfloat fWidth = (GLfloat)width;
    GLfloat fHeight = (GLfloat)height;
    
    perspectiveProjrctionMatrix=vmath::perspective(45.0f, fWidth/fHeight, 0.1f, 100.0f);
    
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
}

-(void) onSingleTap: (UITapGestureRecognizer *)gesture
{
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

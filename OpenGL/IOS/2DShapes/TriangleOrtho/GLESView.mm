


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
    
    GLuint iVertexShaderObject;
    GLuint iFragmentShaderObject;
    GLuint iShaderProgramObject;
    
    GLuint iVertexArrayObject_tri;
    GLuint iVertexArrayObject_rect;
    
    GLuint iVertexBufferObject_tri;
    GLuint iVertexBufferObject_rect;
    
    GLuint Uniform_MVP;
    
    
    vmath::mat4 perspectiveProjectionMatrix;
    
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
        // Shader Information End
        
        
        
        //***** Vertex Shader *****
        //Create Shader
        iVertexShaderObject = glCreateShader( GL_VERTEX_SHADER );
        
        // give source code to shader
        const GLchar *vertexShaderSourceCode = "#version 300 es"    \
        "\n" \
        "in vec4 vPosition;"    \
        "uniform mat4 u_mvp_matrix;"    \
        "void main (void)"    \
        "{"    \
        "gl_Position = u_mvp_matrix * vPosition;"    \
        "}";
        
        glShaderSource( iVertexShaderObject, 1, (const GLchar**) &vertexShaderSourceCode, NULL );
        
        // Compile Source Code
        glCompileShader( iVertexShaderObject );
        GLint iInfoLogLength = 0;
        GLint iShaderCompileStatus = 0;
        
        char *szInfoLog = NULL;
        glGetShaderiv( iVertexShaderObject, GL_COMPILE_STATUS, &iShaderCompileStatus );
        if (iShaderCompileStatus == GL_FALSE)
        {
            glGetShaderiv( iVertexShaderObject, GL_INFO_LOG_LENGTH, &iInfoLogLength );
            if (iInfoLogLength > 0)
            {
                szInfoLog = (char*) malloc( iInfoLogLength );
                if (szInfoLog != NULL)
                {
                    GLsizei written;
                    glGetShaderInfoLog(iVertexShaderObject,iInfoLogLength,&written,szInfoLog);
                    printf("Vertex Shader Compilation Log: %s\n",szInfoLog);
                    free(szInfoLog);
                    [self release];
                }
                
            }
            
        }
        
        iFragmentShaderObject= glCreateShader(GL_FRAGMENT_SHADER);
        
        
        const GLchar * gFragmentShaderSourceCode =
        "#version 300 es" \
        "\n" \
        "precision highp float;"\
        "out vec4 FragColor;" \
        "void main (void)" \
        "{" \
        "FragColor = vec4(1.0,1.0,1.0,1.0);" \
        "}";
        
        glShaderSource(iFragmentShaderObject,1,(const GLchar **)&gFragmentShaderSourceCode,NULL);
        glCompileShader(iFragmentShaderObject);
        
        glGetShaderiv(iFragmentShaderObject,GL_COMPILE_STATUS,&iShaderCompileStatus);
        if(iFragmentShaderObject ==GL_FALSE)
        {
            glGetShaderiv(iFragmentShaderObject,GL_INFO_LOG_LENGTH,&iInfoLogLength);
            if(iInfoLogLength > 0)
            {
                szInfoLog = (char *)malloc(iInfoLogLength);
                if(szInfoLog != NULL)
                {
                    GLsizei written;
                    glGetShaderInfoLog(iFragmentShaderObject,iInfoLogLength,&written,szInfoLog);
                    printf("Fragment Shader Compilation Log: %s\n",szInfoLog);
                    free(szInfoLog);
                    [self release];
                    
                }
            }
        }
        
        iShaderProgramObject= glCreateProgram();
        glAttachShader(iShaderProgramObject,iVertexShaderObject);
        glAttachShader(iShaderProgramObject,iFragmentShaderObject);
        
        //bind in variables before linking
        glBindAttribLocation(iShaderProgramObject,AMC_ATTRIBUTE_VERTEX,"vPosition");
        
        
        glLinkProgram(iShaderProgramObject);
        GLint iShaderProgramLinkStatus = 0;
        glGetProgramiv(iShaderProgramObject,GL_LINK_STATUS,&iShaderProgramLinkStatus);
        if(iShaderProgramLinkStatus == GL_FALSE)
        {
            glGetProgramiv(iShaderProgramObject,GL_INFO_LOG_LENGTH,&iInfoLogLength);
            if(iInfoLogLength > 0)
            {
                szInfoLog = (char *)malloc(iInfoLogLength);
                if(szInfoLog != NULL)
                {
                    GLsizei written;
                    glGetProgramInfoLog(iShaderProgramObject,iInfoLogLength,&written,szInfoLog);
                    printf("Shader Program  Link Log: %s\n",szInfoLog);
                    free(szInfoLog);
                    [self release];
                }
            }
        }
        
        
        //get uniform location
        Uniform_MVP = glGetUniformLocation(iShaderProgramObject, "u_mvp_matrix");
        
        // **** Verttices, Colors, Shader Attribs, Vbo, Vao Initializations ****
        
        
        const GLfloat triangleVertices[]=
        {
            0.0f,  1.0f,  0.0f,
            -1.0f, -1.0f, 0.0f,
            1.0f, -1.0f, 0.0f
        };
        
        const GLfloat rectangleVertices[]=
        {
            1.0f, 1.0f, 0.0f,
            -1.0f, 1.0f, 0.0f,
            -1.0f, -1.0f, 0.0f,
            1.0f, -1.0f, 0.0f
        };
        
        // /////////////////////////////vao creation /////////////////////////
        
        //Create Vao
        glGenBuffers( 1, &iVertexArrayObject_tri );
        glBindVertexArray( iVertexArrayObject_tri );
        
        ////////////////////////////////////********vbo creation and binding********///////////
        
        glGenBuffers( 1, &iVertexBufferObject_tri );
        glBindBuffer( GL_ARRAY_BUFFER, iVertexBufferObject_tri );
        glBufferData( GL_ARRAY_BUFFER, sizeof( triangleVertices ), triangleVertices, GL_STATIC_DRAW );
        
        glVertexAttribPointer( AMC_ATTRIBUTE_VERTEX, 3, GL_FLOAT, GL_FALSE, 0, NULL );
        glEnableVertexAttribArray( AMC_ATTRIBUTE_VERTEX );
        
        glBindBuffer( GL_ARRAY_BUFFER, 0 );
        
        glBindVertexArray( 0 );
        
        ///////////Rectangle////////////
        
        glGenBuffers( 1, &iVertexArrayObject_rect);
        glBindVertexArray( iVertexArrayObject_rect );
        
        //vbo creation and binding
        glGenBuffers( 1, &iVertexBufferObject_rect);
        glBindBuffer( GL_ARRAY_BUFFER, iVertexBufferObject_rect );
        glBufferData( GL_ARRAY_BUFFER, sizeof( rectangleVertices ), rectangleVertices, GL_STATIC_DRAW );
        
        glVertexAttribPointer( AMC_ATTRIBUTE_VERTEX, 3, GL_FLOAT, GL_FALSE, 0, NULL );
        glEnableVertexAttribArray( AMC_ATTRIBUTE_VERTEX );
        
        glBindBuffer( GL_ARRAY_BUFFER, 0 );
        
        glBindVertexArray( 0 );
        
        
        
        
        
        
        //////////////////Vao binding end///////////////////
        //glEnable(GL_DEPTH_TEST);
        //glDepthFunc(GL_LEQUAL);
        glEnable(GL_CULL_FACE);
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        
        perspectiveProjectionMatrix = vmath::mat4::identity();
        
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
    
    glUseProgram(iShaderProgramObject);
    
    vmath::mat4 modelViewMatrix = vmath::mat4::identity();
    vmath:: mat4 translateMatrix = vmath::mat4::identity();
    vmath::mat4 modelViewProjectionMatrix = vmath::mat4::identity();
    
    //DO Mat cal
    modelViewMatrix= vmath::translate(2.0f, 0.0f, -4.0f);
    
    modelViewProjectionMatrix = perspectiveProjectionMatrix * modelViewMatrix;
    
    glUniformMatrix4fv( Uniform_MVP, 1, GL_FALSE, modelViewProjectionMatrix );
    
    glBindVertexArray( iVertexArrayObject_tri);
    
    
    
    glDrawArrays( GL_TRIANGLES, 0, 3 );
    
    glBindVertexArray( 0 );
    
    ///Draw Rectangle
    
    modelViewMatrix = vmath::mat4::identity();
    translateMatrix = vmath::mat4::identity();
    modelViewProjectionMatrix= vmath::mat4::identity();
    
    //DO Mat cal
    modelViewMatrix= vmath::translate(-1.5f, 0.0f, -4.0f);
    
    modelViewProjectionMatrix = perspectiveProjectionMatrix * modelViewMatrix;
    
    glUniformMatrix4fv( Uniform_MVP, 1, GL_FALSE, modelViewProjectionMatrix );
    
    glBindVertexArray( iVertexArrayObject_rect);
    
    
    
    glDrawArrays( GL_TRIANGLE_FAN, 0, 4 );
    
    glBindVertexArray( 0 );
    
    
    glUseProgram(0);
    
    glBindRenderbuffer(GL_RENDERBUFFER, colorRenderBuffer);
    [eaglContext presentRenderbuffer:GL_RENDERBUFFER];
    
    [self updateGeometry];
}

-(void)updateGeometry
{
    
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
    
    if(fHeight == 0)
    {
        fHeight = 1;
    }
    glViewport(0, 0, (GLsizei)fWidth,(GLsizei)fHeight);
    
    perspectiveProjectionMatrix = vmath::perspective(45.0f,(GLfloat)width/(GLfloat)height, 0.1f, 100.0f);
    
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
    
}

-(void) onSwipe: (UIGestureRecognizer *)gesture
{
    
    [self release];
    exit(0);
}

-(void) onDoubleTap: (UITapGestureRecognizer *)gesture
{
    
}

-(void) onSingleTap: (UITapGestureRecognizer *)gesture
{
    
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

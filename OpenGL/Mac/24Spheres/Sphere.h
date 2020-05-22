
@interface Sphere:NSObject

-(void)getSphereVertexData:(float[]) spherePositionCoords andArray1:(float[]) sphereNormalCoords andArray2:(float[]) sphereTexCoords andArray3:(short[]) sphereElements;

-(int) getNumberOfSphereVertices;

-(int) getNumberOfSphereElements;

@end

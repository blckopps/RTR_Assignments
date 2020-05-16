
#ifndef Sphere_h
#define Sphere_h

#import <Foundation/Foundation.h>
#import <math.h>

@interface Sphere : NSObject


-(BOOL)isFoundIdenticalWithValue1: (float) val1  withValue2:(float)val2  andAcceptableDifference:(float)diff ;
-(void) normalizeTheVector:(float[3]) vector;
-(void) addTriangleWithVertex:(float[3][3])single_vertex  withNormal:(float[3][3])single_normal andTexCoord:(float[3][2]) single_texture;
-(void) processSphereData;
-(void) getSphereVertexDataWithPosition:(float*)spherePositionCoords withNormals:(float*)sphereNormalCoords withTexCoords:(float*) sphereTexCoords andElements:(unsigned short*)sphereElements;
-(int) getNumberOfSphereVertices;
-(int) getNumberOfSphereElements;
@end

#endif /* Sphere_h */

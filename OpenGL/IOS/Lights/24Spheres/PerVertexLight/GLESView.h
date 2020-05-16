//
//  GLESView.h
//  BlueWindow
//
//  Created by Samarth Mabrukar on 22/06/18.
//

#import <UIKit/UIKit.h>

@interface GLESView : UIView <UIGestureRecognizerDelegate>
-(void)stopAnimation;
-(void)startAnimation;
-(void)updateGeometry;
@end

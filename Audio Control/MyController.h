/* MyController */

#import <Cocoa/Cocoa.h>

@interface MyController : NSObject
{
    IBOutlet id applicationTable;

    NSArray * _applications;
}

// simple accessors

- (NSArray *) applications;
- (void) setApplications: (NSArray *)newApplications;

@end

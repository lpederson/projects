#import "MyController.h"

@implementation MyController

- (id) init
{
    if (self = [super init])
    {
        _applications = [[NSMutableArray alloc] init];
        _applications = [[NSWorkspace sharedWorkspace]runningApplications];
        
    }
    return self;
}

- (void) dealloc
{
    [_applications release];
    
    [super dealloc];
}

- (NSArray *) applications
{
    return _applications;
}

- (void) setApplications: (NSArray *)newApplications
{
    if (_applications != newApplications)
    {
        [_applications autorelease];
        _applications = [[NSMutableArray alloc] initWithArray: newApplications];
    }
}



@end

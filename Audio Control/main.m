//
//  main.m
//  Audio Control
//
//  Created by Luke Pederson on 7/26/12.
//  Copyright (c) 2012 Luke Pederson. All rights reserved.
//

#import <Cocoa/Cocoa.h>

#import <AppleScriptObjC/AppleScriptObjC.h>

int main(int argc, char *argv[])
{
    [[NSBundle mainBundle] loadAppleScriptObjectiveCScripts];
    return NSApplicationMain(argc, (const char **)argv);
}


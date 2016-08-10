//
//  RACSubject.h
//  ReactiveCocoa
//
//  Created by Josh Abernathy on 3/9/12.
//  Copyright (c) 2012 GitHub, Inc. All rights reserved.
//

#import "RACSignal.h"
#import "RACSubscriber.h"

/// A subject can be thought of as a signal that you can manually control by
/// sending next, completed, and error.
///
/// They're most helpful in bridging the non-RAC world to RAC, since they let you
/// manually control the sending of events.
/********************************************
 
 
 
 Subject是“可变”的。
 Subject是非RAC到RAC的一个桥梁。
 Subject可以附加行为，例如RACReplaySubject具备为未来订阅者缓冲事件的能力。
 
 
 
 *******************************************/
@interface RACSubject : RACSignal <RACSubscriber>

/// Returns a new subject.
+ (instancetype)subject;

@end

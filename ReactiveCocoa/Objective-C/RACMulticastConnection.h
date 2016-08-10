//
//  RACMulticastConnection.h
//  ReactiveCocoa
//
//  Created by Josh Abernathy on 4/11/12.
//  Copyright (c) 2012 GitHub, Inc. All rights reserved.
//

#import <Foundation/Foundation.h>

@class RACDisposable;
@class RACSignal;

/// A multicast connection encapsulates the idea of sharing one subscription to a
/// signal to many subscribers. This is most often needed if the subscription to
/// the underlying signal involves side-effects or shouldn't be called more than
/// once.
///
/// The multicasted signal is only subscribed to when
/// -[RACMulticastConnection connect] is called. Until that happens, no values
/// will be sent on `signal`. See -[RACMulticastConnection autoconnect] for how
/// -[RACMulticastConnection connect] can be called automatically.
///
/// Note that you shouldn't create RACMulticastConnection manually. Instead use
/// -[RACSignal publish] or -[RACSignal multicast:].
/***************************
关键用中文说明：你不能手动的去创建RACMulticastConnection，而需要通过RACSignal publish] 或者[RACSignal multicast:]来进行创建；在下文中我们可以发现publish又去调用了multicast函数
example
RACMulticastConnection *connection = [[RACSignal createSignal:^RACDisposable *(id<RACSubscriber> subscriber) {
	[[RACScheduler mainThreadScheduler] afterDelay:1 schedule:^{
		[subscriber sendNext:@1];
	}];
	return nil;
}] publish];
 只有当[RACMulticastConnection connect]被调用的时候，多发信号（multicasted signal）才能被订阅。
 上述例子只有当
 [connection connect];完成才有信号发出。。。
 针对本格例子可以跟踪publish来看起完成了什么
 - (RACMulticastConnection *)publish {
	RACSubject *subject = [[RACSubject subject] setNameWithFormat:@"[%@] -publish", self.name];
	RACMulticastConnection *connection = [self multicast:subject];
	return connection;
 }

 可以看到publish的主要工作是在函数[self multicast:subject];中完成的，继续跟踪函数multicast.可以看得到是：这里又去调用RACMulticastConnection去完成initWithSourceSignal来实现对RACMulticastConnection的初始化继续跟踪RACMulticastConnection的initWithSourceSignal
 
 - (RACMulticastConnection *)multicast:(RACSubject *)subject {
	[subject setNameWithFormat:@"[%@] -multicast: %@", self.name, subject.name];
	RACMulticastConnection *connection = [[RACMulticastConnection alloc] initWithSourceSignal:self subject:subject];
	return connection;
 }
 
 
 继续
 - (id)initWithSourceSignal:(RACSignal *)source subject:(RACSubject *)subject {
	NSCParameterAssert(source != nil);
	NSCParameterAssert(subject != nil);
 
	self = [super init];
	if (self == nil) return nil;
 
	_sourceSignal = source;
	_serialDisposable = [[RACSerialDisposable alloc] init];
	_signal = subject;
	
	return self;
 }
 这里把创建的在publish中创建的subject复制给_signal而sourceSignal 则是创建的信号。
 
 - (RACDisposable *)connect {
	BOOL shouldConnect = OSAtomicCompareAndSwap32Barrier(0, 1, &_hasConnected);
 
	if (shouldConnect) {
 self.serialDisposable.disposable = [self.sourceSignal subscribe:_signal];
	}
 
	return self.serialDisposable;
 }
则是通过源的信号去subscribe订阅_signal.
 
 
 
 
 
 
 当RACSignal类的实例调用- (RACMulticastConnection *)multicast:(RACSubject *)subject时，以self和subject作为构造参数创建一个RACMulticastConnection实例。
 RACMulticastConnection构造的时候，保存source和subject作为成员变量，创建一个RACSerialDisposable对象，用于取消订阅。
 当RACMulticastConnection类的实例调用- (RACDisposable *)connect这个方法的时候，判断是否是第一次。如果是的话用_signal这个成员变量来订阅sourceSignal之后返回self.serialDisposable；否则直接返回self.serialDisposable。这里面订阅sourceSignal是重点。
 RACMulticastConnection的signal只读属性，就是一个热信号，订阅这个热信号就避免了各种副作用的问题。它会在- (RACDisposable *)connect第一次调用后，根据sourceSignal的订阅结果来传递事件。
 想要确保第一次订阅就能成功订阅sourceSignal，可以使用- (RACSignal *)autoconnect这个方法，它保证了第一个订阅者触发sourceSignal的订阅，也保证了当返回的信号所有订阅者都关闭连接后sourceSignal被正确关闭连接。
 
 
 
 
 
 
 
 
 
********************************/
@interface RACMulticastConnection : NSObject

/// The multicasted signal.
@property (nonatomic, strong, readonly) RACSignal *signal;

/// Connect to the underlying signal by subscribing to it. Calling this multiple
/// times does nothing but return the existing connection's disposable.
///
/// Returns the disposable for the subscription to the multicasted signal.
- (RACDisposable *)connect;

/// Connects to the underlying signal when the returned signal is first
/// subscribed to, and disposes of the subscription to the multicasted signal
/// when the returned signal has no subscribers.
///
/// If new subscribers show up after being disposed, they'll subscribe and then
/// be immediately disposed of. The returned signal will never re-connect to the
/// multicasted signal.
///
/// Returns the autoconnecting signal.
- (RACSignal *)autoconnect;

@end

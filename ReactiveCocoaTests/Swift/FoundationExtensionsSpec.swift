//
//  FoundationExtensionsSpec.swift
//  ReactiveCocoa
//
//  Created by Neil Pankey on 5/22/15.
//  Copyright (c) 2015 GitHub. All rights reserved.
//

import Result
import Nimble
import Quick
import ReactiveCocoa

class FoundationExtensionsSpec: QuickSpec {
	override func spec() {
		describe("NSNotificationCenter.rac_notifications") {

			it("should send notifications on the producer") {
				let center = NSNotificationCenter.defaultCenter()
				let producer = center.rac_notifications("rac_notifications_test")

				var notif: NSNotification? = nil
				let disposable = producer.startWithNext { notif = $0 }

				center.postNotificationName("some_other_notification", object: nil)
				expect(notif).to(beNil())

				center.postNotificationName("rac_notifications_test", object: nil)
				expect(notif?.name) == "rac_notifications_test"

				notif = nil
				disposable.dispose()

				center.postNotificationName("rac_notifications_test", object: nil)
				expect(notif).to(beNil())
			}
		}
	}
}

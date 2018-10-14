/*
 * Appcelerator Titanium Mobile
 * Copyright (c) 2011-Present by Appcelerator, Inc. All Rights Reserved.
 * Licensed under the terms of the Apache Public License
 * Please see the LICENSE included with this distribution for details.
 */
/* eslint-env mocha */
/* global Ti, Titanium */
/* eslint no-unused-expressions: "off" */
'use strict';
var should = require('./utilities/assertions'),
	utilities = require('./utilities/utilities');

describe('Titanium.UI.TextField', function () {
	var win;

	afterEach(function () {
		if (win) {
			win.close();
		}
		win = null;
	});

	it('apiName', function () {
		var textField = Ti.UI.createTextField({
			value: 'this is some text'
		});
		should(textField).have.readOnlyProperty('apiName').which.is.a.String;
		should(textField.apiName).be.eql('Ti.UI.TextField');
	});

	it('value', function () {
		var textfield = Ti.UI.createTextField({
			value: 'this is some text'
		});
		should(textfield.value).be.a.String;
		should(textfield.getValue).be.a.Function;
		should(textfield.value).eql('this is some text');
		should(textfield.getValue()).eql('this is some text');
		textfield.value = 'other text';
		should(textfield.value).eql('other text');
		should(textfield.getValue()).eql('other text');
	});

	// Skip on Windows Phone since not available, yet
	it.windowsMissing('padding', function () {
		var textfield = Ti.UI.createTextField({
			value: 'this is some text',
			padding: {
				left: 20,
				right: 20
			}
		});
		should(textfield.padding).be.a.Object;
		should(textfield.getPadding).be.a.Function;
		should(textfield.setPadding).be.a.Function;

		should(textfield.padding.left).eql(20);
		should(textfield.padding.right).eql(20);
		should(textfield.getPadding().left).eql(20);
		should(textfield.getPadding().right).eql(20);

		textfield.setPadding({
			left: 10,
			right: 10
		});

		should(textfield.padding.left).eql(10);
		should(textfield.padding.right).eql(10);
		should(textfield.getPadding().left).eql(10);
		should(textfield.getPadding().right).eql(10);
	});

	it('textAlign', function () {
		var textfield = Ti.UI.createTextField({
			value: 'this is some text',
			textAlign: Titanium.UI.TEXT_ALIGNMENT_CENTER
		});
		// FIXME Parity issue!
		if (utilities.isAndroid()) {
			should(textfield.textAlign).be.a.String;
		} else {
			should(textfield.textAlign).be.a.Number;
		}
		should(textfield.getTextAlign).be.a.Function;
		should(textfield.textAlign).eql(Titanium.UI.TEXT_ALIGNMENT_CENTER);
		should(textfield.getTextAlign()).eql(Titanium.UI.TEXT_ALIGNMENT_CENTER);
		textfield.textAlign = Titanium.UI.TEXT_ALIGNMENT_RIGHT;
		should(textfield.textAlign).eql(Titanium.UI.TEXT_ALIGNMENT_RIGHT);
		should(textfield.getTextAlign()).eql(Titanium.UI.TEXT_ALIGNMENT_RIGHT);
	});

	it('verticalAlign', function () {
		var textfield = Ti.UI.createTextField({
			value: 'this is some text',
			verticalAlign: Titanium.UI.TEXT_VERTICAL_ALIGNMENT_BOTTOM
		});
		// FIXME Parity issue!
		if (utilities.isAndroid()) {
			should(textfield.verticalAlign).be.a.String;
		} else {
			should(textfield.verticalAlign).be.a.Number;
		}
		should(textfield.getVerticalAlign).be.a.Function;
		should(textfield.verticalAlign).eql(Titanium.UI.TEXT_VERTICAL_ALIGNMENT_BOTTOM);
		should(textfield.getVerticalAlign()).eql(Titanium.UI.TEXT_VERTICAL_ALIGNMENT_BOTTOM);
		textfield.verticalAlign = Titanium.UI.TEXT_VERTICAL_ALIGNMENT_TOP;
		should(textfield.verticalAlign).eql(Titanium.UI.TEXT_VERTICAL_ALIGNMENT_TOP);
		should(textfield.getVerticalAlign()).eql(Titanium.UI.TEXT_VERTICAL_ALIGNMENT_TOP);
	});

	// FIXME Defaults to undefined on Android. Docs say default is false
	it.androidBroken('passwordMask', function () {
		var text = 'this is some text',
			textfield = Ti.UI.createTextField({
				value: text
			});
		// passwordMask should default to false
		should(textfield.passwordMask).be.false; // undefined on Android
		textfield.passwordMask = true;
		should(textfield.passwordMask).be.true;
		// it should have same text before
		should(textfield.value).be.eql(text);
	});

	// TODO Add tests for:
	// autocapitalize
	// autocorrect
	// borderStyle
	// clearonEdit
	// color
	// editable
	// enableReturnKey
	// font
	// keyboardType
	// maxLength
	// returnKeyType
	// selection
	// suppressReturn

	it('hintText', function () {
		var textfield = Ti.UI.createTextField({
			hintText: 'Enter E-Mail ...'
		});
		should(textfield.getHintText).be.a.Function;
		should(textfield.hintText).eql('Enter E-Mail ...');
		should(textfield.getHintText()).eql('Enter E-Mail ...');
		textfield.hintText = 'Enter Name ...';
		should(textfield.hintText).eql('Enter Name ...');
		should(textfield.getHintText()).eql('Enter Name ...');
	});

	it.windowsMissing('hintTextColor', function () {
		var textfield = Ti.UI.createTextField({
			hintText: 'Enter E-Mail ...',
			hintTextColor: 'red'
		});
		should(textfield.getHintTextColor).be.a.Function;
		should(textfield.hintTextColor).eql('red');
		should(textfield.getHintTextColor()).eql('red');
		textfield.hintTextColor = 'blue';
		should(textfield.hintTextColor).eql('blue');
		should(textfield.getHintTextColor()).eql('blue');
	});

	it.android('hintType', function () {
		var textfield = Ti.UI.createTextField({
			hintText: 'Enter E-Mail ...',
			hintType: Ti.UI.HINT_TYPE_ANIMATED
		});
		should(textfield.getHintType).be.a.Function;
		should(textfield.hintType).eql(Ti.UI.HINT_TYPE_ANIMATED);
		should(textfield.getHintType()).eql(Ti.UI.HINT_TYPE_ANIMATED);
		textfield.hintType = Ti.UI.HINT_TYPE_STATIC;
		should(textfield.hintType).eql(Ti.UI.HINT_TYPE_STATIC);
		should(textfield.getHintType()).eql(Ti.UI.HINT_TYPE_STATIC);
	});

	// FIXME win.width is undefined on Android and iOS here. Test needs to be rewritten. Likely need to use postlayout to get values?
	// FIXME Windows Desktop gives: expected '' to be above 100
	it.allBroken('width', function (finish) {
		var textfield;
		this.timeout(5000);
		textfield = Ti.UI.createTextField({
			value: 'Lorem ipsum dolor sit amet, consectetur adipiscing elit. Donec nec ullamcorper massa, eget tempor sapien. Phasellus nisi metus, tempus a magna nec, ultricies rutrum lacus. Aliquam sit amet augue suscipit, dignissim tellus eu, consectetur elit. Praesent ligula velit, blandit vel urna sit amet, suscipit euismod nunc.',
			width: Ti.UI.SIZE
		});
		win = Ti.UI.createWindow({
			backgroundColor: '#ddd'
		});
		win.add(textfield);
		win.addEventListener('focus', function () {
			try {
				should(win.width).be.greaterThan(100);
				should(textfield.width).not.be.greaterThan(win.width);
				return finish();
			} catch (err) {
				finish(err);
			}
		});
		win.open();
	});

	// FIXME Intermittently failing on Android on build machine, I think due to test timeout
	it.androidBroken('height', function (finish) {
		var textfield,
			bgView;
		this.timeout(5000);
		textfield = Ti.UI.createTextField({
			value: 'Lorem ipsum dolor sit amet, consectetur adipiscing elit. Donec nec ullamcorper massa, eget tempor sapien. Phasellus nisi metus, tempus a magna nec, ultricies rutrum lacus. Aliquam sit amet augue suscipit, dignissim tellus eu, consectetur elit. Praesent ligula velit, blandit vel urna sit amet, suscipit euismod nunc.',
			width: Ti.UI.SIZE,
			height: Ti.UI.SIZE,
			color: 'black'
		});
		bgView = Ti.UI.createView({
			width: 200,
			height: 100,
			backgroundColor: 'red'
		});
		win = Ti.UI.createWindow({
			backgroundColor: '#eee'
		});
		bgView.add(textfield);
		win.add(bgView);

		win.addEventListener('focus', function () {
			try {
				should(bgView.height).be.eql(100);
				should(textfield.height).not.be.greaterThan(100);
				return finish();
			} catch (err) {
				finish(err);
			}
		});
		win.open();
	});

	// Tests adding and removing a TextField's focus.
	it.ios('focus-blur', function (finish) {
		var textField;
		this.timeout(5000);
		win = Ti.UI.createWindow({ layout: 'vertical' });

		// First TextField is needed to receive default focus on startup
		// and to receive focus when second TextField has lost focus.
		textField = Ti.UI.createTextField({
			width: Ti.UI.FILL,
			height: Ti.UI.SIZE,
		});
		win.add(textField);

		// Second TextField is used to test focus/blur handling.
		textField = Ti.UI.createTextField({
			width: Ti.UI.FILL,
			height: Ti.UI.SIZE,
		});
		textField.addEventListener('focus', function () {
			// Focus has been received. Now test removing focus.
			setTimeout(function () {
				textField.blur();
			}, 500);
		});
		textField.addEventListener('blur', function () {
			// Focus has been lost. The test was finished successfully. (Timeout means failure.)
			finish();
		});
		win.add(textField);

		// Start the test when the window has been opened.
		win.addEventListener('postlayout', function () {
			setTimeout(function () {
				textField.focus();
			}, 500);
		});
		win.open();
	});

	// TextField must not receive focus by default upon opening a window.
	it('focus-win-open', function (finish) {
		var textField;
		this.timeout(5000);

		win = Ti.UI.createWindow();
		textField = Ti.UI.createTextField();
		textField.addEventListener('focus', function () {
			// This should never happen.
			finish(new Error('TextField wrongly received focus on open.'));
		});
		win.add(textField);
		win.addEventListener('postlayout', function () {
			// If we made it this far, assume TextField did not receive focus.
			finish();
		});
		win.open();
	});

	// The "focus" and "blur" events are not supposed to bubble up the view hierarchy.
	// Windows ticket TIMOB-26177
	it.windowsBroken('focus-blur-bubbles', function (finish) {
		var textField;
		this.timeout(5000);

		win = Ti.UI.createWindow();
		textField = Ti.UI.createTextField();
		textField.addEventListener('focus', function (e) {
			should(e.bubbles).be.eql(false);
			textField.blur();
		});
		textField.addEventListener('blur', function (e) {
			should(e.bubbles).be.eql(false);
			finish();
		});
		win.add(textField);
		win.addEventListener('open', function () {
			textField.focus();
		});
		win.open();
	});

	it.ios('.passwordRules', function () {
		var textField = Ti.UI.createTextField({
			passwordMask: true,
			passwordRules: 'required: upper; required: lower; required: digit; max-consecutive: 2'
		});
		should(textField.passwordRules).equal('required: upper; required: lower; required: digit; max-consecutive: 2');
	});
});

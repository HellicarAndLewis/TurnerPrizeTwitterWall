/*
 *  TwitterMessage.cpp
 *  TPTW
 *
 *  Created by Joel Gethin Lewis on 01/10/2010.
 *  Copyright 2010 Hellicar&Lewis. All rights reserved.
 *
 */

#include "TwitterMessage.h"

TwitterMessage::TwitterMessage(){
	messageWidth = DEFAULT_TWIITER_MESSAGE_WIDTH; //need a real size otherwise the copy constructor fucks everything by trying to make an image with negative width or height
	messageHeight = DEFAULT_TWIITER_MESSAGE_HEIGHT;
	
	dateTimeImage.allocate(messageWidth, messageHeight, OF_IMAGE_COLOR_ALPHA); //ditto above
	contentImage.allocate(messageWidth, messageHeight, OF_IMAGE_COLOR_ALPHA);	
	
	messageXPos = 0;
	messageYPos = 0;
	
	tweetID = "-1";
	content = "no content yet";
	dateTime = "no time yet";
	userName = "no user yet";
	
	isLive = true;
	fadingIn = true;
	onScreen = false;
	
	pango = NULL;	
	
	dateTimeContext = NULL;
	dateTimeLayout = NULL;
	fdForDateTime = NULL;
	contentContext = NULL;
	contentLayout = NULL;
	fdForContent = NULL;
	
	inited = false;
}

	//----------------------------------------------------------
TwitterMessage::~TwitterMessage(){
	clear();
}	

	// default copy overwriting (for = or std::vector)
TwitterMessage::TwitterMessage(const TwitterMessage& tm){		
	clear();
	clone(tm);		
}

	// copying:
void TwitterMessage::clone(const TwitterMessage &tm){
	messageWidth = tm.messageWidth;
	messageHeight = tm.messageHeight;
	
	dateTimeImage.clear();
	contentImage.clear();
	
	dateTimeImage.resize(tm.dateTimeImage.width, tm.dateTimeImage.height);
	contentImage.resize(tm.contentImage.width, tm.contentImage.height);
	
	dateTimeImage.clone(tm.dateTimeImage);
	contentImage.clone(tm.contentImage);	
	
	messageXPos = tm.messageXPos;
	messageYPos = tm.messageYPos;
	
	tweetID = string(tm.tweetID);
	content = string(tm.content);
	dateTime = string(tm.dateTime);
	userName = string(tm.userName);
	
	isLive = tm.isLive;
	fadingIn = tm.fadingIn;
	onScreen = tm.onScreen;
	
	pango = tm.pango;
	
	inited = tm.inited;
}	

void TwitterMessage::clear(){
	messageWidth = 10; //need a real size otherwise the copy constructor fucks everything by trying to make an image with negative width or height
	messageHeight = 10;
	messageXPos = 0;
	messageYPos = 0;
	
	tweetID = "-1";
	content = "no content yet";
	dateTime = "no time yet";
	userName = "no user yet";
	
	isLive = true;
	fadingIn = true;	
	onScreen = false;
	
	dateTimeImage.clear();
	contentImage.clear();
	
	pango = NULL;	
	
//	dateTimeContext = NULL;
//	dateTimeLayout = NULL;
//	fdForDateTime = NULL;
//	contentContext = NULL;
//	contentLayout = NULL;
//	fdForContent = NULL; //TODO: neaten this into a sensible state of affairs - what about pango proper way of doing? just trying to kill memory leaks atm
	
	inited = false;
}

void TwitterMessage::init(string _tweetID, string someUser, string someContent, string someDateTime, ofxPango* _pango, bool _isLive, unsigned int _duration, unsigned int _delay, bool _onScreen){
	messageWidth = DEFAULT_TWIITER_MESSAGE_WIDTH;
	
	isLive = _isLive;
	duration = _duration;
	delay = _delay;
	tweetID = _tweetID;
	userName = someUser;
	onScreen = _onScreen;
	
	pango = _pango;
	
	if(isLive){
		
	}else{
			//its selected, so just fade in and out
		fadingIn = true;			
			//reset it
		tween.setParameters(easingsine,ofxTween::easeOut,0,100,duration,0);				
	}
	
	content = someContent;
	dateTime = someDateTime;
	
	dateTimeContext = pango->createContextWithSurface(DEFAULT_TWIITER_MESSAGE_WIDTH, DEFAULT_TWIITER_MESSAGE_HEIGHT);
	
		//dateTimeContext->color4f(129.f/255.f, 130.f/255.f, 133.f/255.f, 1.f);
		//prev grey, now going to black
	dateTimeContext->color4f(0.f, 0.f, 0.f, 1.f);
	dateTimeContext->paint();
	
	dateTimeLayout = dateTimeContext->createPangoLayout();
	dateTimeLayout->setText(dateTime);
	dateTimeLayout->setWidth(DEFAULT_TWIITER_MESSAGE_WIDTH);
	dateTimeLayout->setAlignLeft();
	dateTimeContext->color4f(1.f, 1.f, 1.f, 1.f);
	
	pango_layout_set_spacing(dateTimeLayout->pa_layout, 12.5f*PANGO_SCALE); //25px line spacing for date time
	
	fdForDateTime = new ofxPCPangoFontDescription();
	fdForDateTime->createFromString("TateMonoTwo 14px");
	fdForDateTime->setWeight(PANGO_WEIGHT_NORMAL); //or "regular"
	dateTimeLayout->setFontDescription(*fdForDateTime);
	dateTimeLayout->show();
	
	int pixelWidth, pixelHeight;
	
	pango_layout_get_pixel_size(dateTimeLayout->pa_layout, &pixelWidth, &pixelHeight);
	
		//cout << "Date time pixel size is " << pixelWidth << ", " << pixelHeight << endl;
	
	if(isLive){
		pixelHeight += 5;
	}else{
		pixelHeight += 8;	
	}
	
	dateTimeImage.resize(messageWidth, pixelHeight);
	setImageFromPangoData(&dateTimeImage, dateTimeContext->getSurface()->getData());
	
	contentContext = pango->createContextWithSurface(DEFAULT_TWIITER_MESSAGE_WIDTH, DEFAULT_TWIITER_MESSAGE_HEIGHT);
	contentContext->color4f(0.f, 0.f, 0.f, 1.f);
	contentContext->paint();
	
	contentLayout = contentContext->createPangoLayout();
	contentLayout->setText(content);
	contentLayout->setWidth(DEFAULT_TWIITER_MESSAGE_WIDTH);
	contentLayout->setAlignLeft();
	contentContext->color4f(1.f, 1.f, 1.f, 1.f);
	
	fdForContent = new ofxPCPangoFontDescription();
	
	if(isLive){
		pango_layout_set_spacing(contentLayout->pa_layout, 7*PANGO_SCALE); //24px line spacing for content			
		fdForContent->createFromString("Tate New 18px"); // ultra thin seems to look more right? hmm
		fdForContent->setWeight(PANGO_WEIGHT_THIN);
	}else{
		pango_layout_set_spacing(contentLayout->pa_layout, 3.5f*PANGO_SCALE); //24px line spacing for content
		fdForContent->createFromString("Tate New 25px"); // ultra thin seems to look more right? hmm
														 //fdForContent->setWeight(PANGO_WEIGHT_THIN);
	}
	
	contentLayout->setFontDescription(*fdForContent);
	contentLayout->show();
	
	pango_layout_get_pixel_size(contentLayout->pa_layout, &pixelWidth, &pixelHeight);
	
		//cout << "Content time pixel size is " << pixelWidth << ", " << pixelHeight << endl;
	
	pixelHeight += 37; //37 pixels of space below the last line...
	
	contentImage.resize(messageWidth, pixelHeight);
	setImageFromPangoData(&contentImage, contentContext->getSurface()->getData());
	
	messageHeight = dateTimeImage.height + contentImage.height;
	
	inited = true;
}

void TwitterMessage::update(){
	if(isLive){
			//move it
		
	}else{		
		if(tween.isCompleted()){
				//its selected, so just fade in and out
			if(fadingIn){
					//fade out with delay
				tween.setParameters(easingsine,ofxTween::easeOut,100,0,duration,delay);
				fadingIn = false;
			}else{
					//fade in straight away
				tween.setParameters(easingsine,ofxTween::easeIn,0,100,duration,0);// 0 delay on fade in
				fadingIn = true;
			}
		}
	}	
}

void TwitterMessage::draw(float x, float y, float w, float h, ofImage* iconImage){
	if(isLive){	
		ofSetColor(255, 255, 255, 255);
		dateTimeImage.draw(x, messageYPos);
		iconImage->draw(x-20.f, messageYPos+3.f);
		contentImage.draw(x, messageYPos+dateTimeImage.height);				
	}else{
		ofSetColor(255, 255, 255, ((float)tween.update()/100.f)*255.f);
		dateTimeImage.draw(x,y);
		contentImage.draw(x,y+dateTimeImage.height+5);			
	}
}

void TwitterMessage::draw(float x, float y, ofImage* iconImage){
	draw(x, y, messageWidth, messageHeight, iconImage);
}

float TwitterMessage::getHeight(){
	return messageHeight;
}
float TwitterMessage::getWidth(){
	return messageWidth;
}	

void TwitterMessage::updateText(string someContent, string someDateTime, string someID, string someUser){
	ofPushStyle();
	ofEnableAlphaBlending();
	ofFill();
	ofSetColor(255, 255, 255, 255); //full white?
	
	tweetID = someID;
	content = someContent;
	dateTime = someDateTime;	
	userName = someUser;
	
		//		ofxPCContext* dateTimeContext = _pango->createContextWithSurface(340.f, 900.f);
	
		//dateTimeContext->color4f(129.f/255.f, 130.f/255.f, 133.f/255.f, 1.f);
		//prev grey, now going to black
	dateTimeContext->color4f(0.f, 0.f, 0.f, 1.f);
	dateTimeContext->paint();
	
	dateTimeLayout->setText(dateTime);
	dateTimeLayout->setWidth(DEFAULT_TWIITER_MESSAGE_WIDTH);
	dateTimeLayout->setAlignLeft();
	dateTimeContext->color4f(1.f, 1.f, 1.f, 1.f);
	
	pango_layout_set_spacing(dateTimeLayout->pa_layout, 12.5f*PANGO_SCALE); //25px line spacing for date time
	
	dateTimeLayout->show();
	
	int pixelWidth, pixelHeight;
	
	pango_layout_get_pixel_size(dateTimeLayout->pa_layout, &pixelWidth, &pixelHeight);
	
		//cout << "Date time pixel size is " << pixelWidth << ", " << pixelHeight << endl;
	
	if(isLive){
		pixelHeight += 5;
	}else{
		pixelHeight += 8;	
	}
	
	dateTimeImage.resize(messageWidth, pixelHeight);
	setImageFromPangoData(&dateTimeImage, dateTimeContext->getSurface()->getData());
	
	contentContext->color4f(0.f, 0.f, 0.f, 1.f);
	contentContext->paint();
	
	contentLayout->setText(content);
	contentLayout->setWidth(DEFAULT_TWIITER_MESSAGE_WIDTH);
	contentLayout->setAlignLeft();
	
	contentContext->color4f(1.f, 1.f, 1.f, 1.f);
	
	contentLayout->show();
	
	pango_layout_get_pixel_size(contentLayout->pa_layout, &pixelWidth, &pixelHeight);
	
		//cout << "Content time pixel size is " << pixelWidth << ", " << pixelHeight << endl;
	
	pixelHeight += 37; //37 pixels of space below the last line...
	
	contentImage.resize(messageWidth, pixelHeight);
	setImageFromPangoData(&contentImage, contentContext->getSurface()->getData());
	
	messageHeight = dateTimeImage.height + contentImage.height;			
	
	ofDisableAlphaBlending();
	ofNoFill();
	ofPopStyle();		
}

void TwitterMessage::setImageFromPangoData(ofImage* theImage, uint32_t* thePangoData){
	
	int newWidth = theImage->width;
	int newHeight = theImage->height; // 
	
	int pixels = newWidth*newHeight; // we assume RGBA for now!
	
	unsigned char* new_data = new unsigned char[pixels * 4];
	
	for(int i = 0; i < newWidth; ++i) {
		for(int j = 0; j < newHeight; ++j) {
			int pos = (j * newWidth) + i;
			int dest = (j * newWidth*4) + i * 4;
			uint32_t p = thePangoData[pos];
			int r,g,b,a;
			a = (unsigned char)((p & 0xFF000000) >> 24);
			r = (unsigned char)((p & 0x00FF0000) >> 16);
			g = (unsigned char)((p & 0x0000FF00) >> 8);
			b = (unsigned char)((p & 0x000000FF));
			new_data[dest] = r;
			new_data[dest+1] = g;
			new_data[dest+2] = b;
			new_data[dest+3] = a;
		}
	}
	
	theImage->setFromPixels(new_data, newWidth, newHeight, OF_IMAGE_COLOR_ALPHA, true);
	
	delete[] new_data; //clean up!
}


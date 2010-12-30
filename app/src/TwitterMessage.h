#pragma once

#include "ofMain.h"

#include "ofxPango.h"

	//ofxTween
#include "ofxTween.h"

#define DEFAULT_TWIITER_MESSAGE_WIDTH 340
#define DEFAULT_TWIITER_MESSAGE_HEIGHT 900

class TwitterMessage{
public:
	
	TwitterMessage();
	~TwitterMessage();
	
		// default copy overwriting (for = or std::vector)
	TwitterMessage(const TwitterMessage& tm);
	
	TwitterMessage::TwitterMessage& operator= (const TwitterMessage& tm){
		clone(tm);
		return *this;		
	}		
	
		// copying:
	void clone(const TwitterMessage &tm);
	void clear();
	
	void init(string _tweetID, string someUser, string someContent, string someDateTime, 
			  ofxPango* _pango, bool _isLive, unsigned int _duration, unsigned int _delay, bool _onScreen);
	
	void update();
	
	void draw(float x, float y, float w, float h, ofImage* iconImage);
	
	void draw(float x, float y, ofImage* iconImage);
	
	float getHeight();
	float getWidth();
	
	void updateText(string someContent, string someDateTime, string someID, string someUser);	
	void setImageFromPangoData(ofImage* theImage, uint32_t* thePangoData);
	
public:
	float messageWidth, messageHeight, messageXPos, messageYPos;
		  
	string content;
	string dateTime;
	
	ofImage dateTimeImage;
	ofImage contentImage;
	
	bool isLive;
	
	ofxTween tween;
	
	ofxEasingSine	easingsine;	

	unsigned int delay;
	unsigned int duration;
	
	bool fadingIn;
	string tweetID;
	string userName;
	
	bool onScreen;
	
	ofxPango* pango;
	
	ofxPCContext* dateTimeContext;
	ofxPCPangoLayout* dateTimeLayout;
	ofxPCPangoFontDescription* fdForDateTime;
	ofxPCContext* contentContext;
	ofxPCPangoLayout* contentLayout;
	ofxPCPangoFontDescription* fdForContent;
	
	bool inited;
};



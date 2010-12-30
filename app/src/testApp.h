#ifndef _TEST_APP
#define _TEST_APP

#include "ofMain.h"

	//ofxTween
#include "ofxTween.h"

	//Twitter Message class
#include "TwitterMessage.h"

#include "ofxPango.h"

#define NUMBER_OF_LIVE_MESSAGES 20
#define TOP_BUFFER_ZONE_HEIGHT 500
#define LIVE_MESSAGE_OFFSCREEN -900

#define NUMBER_OF_SELECTED_MESSAGES 10

#include "ofxTwitter.h"
#include "ofxXmlSettings.h"

	//for easy tokenisation of the twitter string
#include "Poco/StringTokenizer.h"
#include <iostream>
#include <queue>
#include "Poco/String.h"

using Poco::StringTokenizer;

class testApp : public ofBaseApp{

	public:

		void setup();
		void update();
		void draw();

		void keyPressed  (int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void resized(int w, int h);
	
		//new methods
	void drawMaskBox(); // draws a box over the top of the live comments so that the edges look tidy...
	void drawStaticElements(); //draws the title texts and a vertical line
	
	float totalHeightOfDisplayMessages(); //returns the total height of the current display vector in pixels	
	float yPositionOfHighestMessage(); //returns the y postion of the highest currently displayed message
	bool isMessageCurrent(string _tweetID); //is the tweetID in our current vector?
	bool isMessageDisplayed(string _tweetID); //is the tweetID in our display vector?	
	bool isMessageInSelected(string _tweetID); //is the tweetID in our selected vector?
	int getSafeIndexFromDisplay(); //get a safe index that isn't onscreen

		//ofxTween
	float length_float;
	
	ofxTween tweenlinear;
	ofxTween tweensine;
	
	ofxEasingLinear 	easinglinear;
	ofxEasingSine	easingsine;	
	
	TwitterMessage selectedMessage;
	
	ofImage backgroundImage;
	
	ofxPango* pango;
	
	TwitterMessage * display[NUMBER_OF_LIVE_MESSAGES];	

		//vector <TwitterMessage> display;
	vector <ofxTweet> current;
	
	float messageSpeed;
	
	float prevFrameTime;
	
	int tweetIDCounter;	
	
		//ofxTwitter
	
		// from todds simple twitter example
	
	void twitterResponse(TwitterResponseArgs &e);
	
		// OF Twitter
	ofxTwitter			twitter;
	ofxTwitterProfile	profile;
	queue <ofxTweet>	tweets;	
	
		//new JGL
	void processTweet(ofxTweet tweet); //processes a tweet by splitting it up via tokeniszeTwitterMessage and then runs the relevant commands if there are
									   // more than one
	
	void popAndProcessATweet();	
	
	vector<string> tokeniseString(string inputString, string deliminators=" "); //defaults to the " " deliminator
	
	vector<string> tokeniseTwitterMessage(ofxTweet tweet); //breaks up a tweet message string around " " 's for processing	
	
	string cleanUpDateFromRT(string dateToClean);
	string cleanUpDateFromLive(string dateToClean); //different formats annoyingly
	
	
	
		//hellowall
	
	bool bGetNewResponse;
	
	float timeOfLastNewParticle;
	
	float timeBetweenTwitterUpdateAttempts;
	
	float timeOfLastAttemptedTwitterUpdate;	
	
	string lastText;
	string lastScreenName;	
	
	bool bGetSelectedMessages;
	
	vector <ofxTweet> selectedMessages;
	
	bool bShowMouse;
	
	ofImage twitterIcon;
	ofImage mobileIcon;
};

#endif

#include "testApp.h"

extern "C" {
#include "macGlutfix.h"
}


//--------------------------------------------------------------
void testApp::setup(){	
	
	ofSetVerticalSync(true);
	ofSetFrameRate(30);
		//ofEnableSmoothing();
	
    ofEnableAlphaBlending();
	
	twitterIcon.loadImage("tweetIcon.png");
	mobileIcon.loadImage("textIcon.png");
	
	backgroundImage.loadImage("twitter_animation_diagram_background.png");
	
    ofBackground(0,0,0);

	ofSetColor(255,255,255);
	ofNoFill();
	
	pango = new ofxPango();

	tweetIDCounter = 0;
	
	string selectedMessageContent = "Temporary Message, downloading latest Twitter Information.";
	string selectedMessageDateTime = "Now";
	string selectedMessageUser = "joelgethinlewis";
	bool selectedOnScreen = true;
	
	selectedMessage.init(ofToString(tweetIDCounter), selectedMessageUser, selectedMessageContent, selectedMessageDateTime, pango, false, 2000, 10000, selectedOnScreen);
	
	tweetIDCounter++;
	
	messageSpeed = 30.f; //move 30 pixels every second
	
	prevFrameTime = ofGetElapsedTimef();

		//setting them up for real now...
	for(int i = 0; i < NUMBER_OF_LIVE_MESSAGES; i++){
		TwitterMessage temp;
		
		string liveMessageContent = "Temporary Message, awaiting new messages from Twitter.";
		string liveMessageDateTime = "00:00";	
		string liveMessageUser = "joelgethinlewis";
		bool onScreen = false;
		
//		temp.init(ofToString(tweetIDCounter), liveMessageUser, liveMessageContent, liveMessageDateTime, pango, true, -1, -1, onScreen);
//		
//		display.push_back(temp);
//		
//		display.back().init(ofToString(tweetIDCounter), liveMessageUser, liveMessageContent, liveMessageDateTime, pango, true, -1, -1, onScreen);//TODO: fix this dirtyness, shouldn't have to
//			//copy constructor should take care of this
//		
//		display.back().messageYPos = LIVE_MESSAGE_OFFSCREEN; //arbitary
//		display.back().onScreen = false; //making sure....
		
		display[i] = new TwitterMessage();
		
		display[i]->init(ofToString(tweetIDCounter), liveMessageUser, liveMessageContent, liveMessageDateTime, pango, true, -1, -1, onScreen);//TODO: fix this dirtyness, shouldn't have to
			//copy constructor should take care of this, fuck vectors
		
		display[i]->messageYPos = LIVE_MESSAGE_OFFSCREEN; //arbitary
		display[i]->onScreen = false; //making sure....		
		
		tweetIDCounter++;
	}
	
		//from todds simple twitter example
		// regester for twitter events
	ofAddListener(twitter.TWITTER_RESPONSE, this, &testApp::twitterResponse);
	twitter.allocate();
	
	/*
	 You can load tweets via number and page
	 I am getting a error if I try to call 
	 twitter twice like:
	 
	 twitter.getUserTweets("vanderlin", 20, 1);
	 twitter.getUserTweets("vanderlin", 20, 2);
	 
	 you need to wait for twitter to be done.
	 
	 There is also a twitter profile call
	 but im not done with that :)
	 
	 to use:
	 You need ofxThread
	 you need the altered version of ofxXMLSettings
	 I put it in the src/ofxTwitter should be good to go
	 */
		//twitter.getUserTweets("thehellowall", 20, 1);
		//twitter.getUserMentions();
	
	timeBetweenTwitterUpdateAttempts = 30.f;
	timeOfLastAttemptedTwitterUpdate = ofGetElapsedTimef();
	bGetSelectedMessages = true; //start by trying to get retweeted/selected messages...
	bGetNewResponse = true;	//start by getting a response	
	
	lastText = "";
	lastScreenName = "";
	
		//cout << "End of setup" << endl;
	
	ofHideCursor();
	bShowMouse = false;
}

//--------------------------------------------------------------
void testApp::update(){
		//move every message by distance to move, based on messageSpeed
	
	float currentTime = ofGetElapsedTimef();
	
	float elapsedTime = currentTime - prevFrameTime;
	
	float distanceToMove = messageSpeed*elapsedTime; //number of pixels to move this frame...
	
	distanceToMove = roundf(distanceToMove);
	
	for(int i=0; i < NUMBER_OF_LIVE_MESSAGES; i++){
			//update all the positions
		if(display[i]->onScreen){
			display[i]->messageYPos += distanceToMove;			
		}
	}
	
	//from hellowall
	float now = ofGetElapsedTimef();
	
	popAndProcessATweet();
	
	if((now-timeOfLastAttemptedTwitterUpdate) > timeBetweenTwitterUpdateAttempts){
		bGetNewResponse = true;
	}
	
	if(bGetNewResponse){
		if(bGetSelectedMessages){
				//get the 10 latest retweets by TurnerPrize2010 (selected messages)
			twitter.getUserReTweets();
			bGetSelectedMessages = false;
		}else{
				//get the 20 latest tweets with the hashtag #TurnerPrize2010
			twitter.getHashTags();
			bGetSelectedMessages = true; //just alternating for now
		}
		
		bGetNewResponse = false;
		timeOfLastAttemptedTwitterUpdate = now;
	}	
	
		//from JGL rendering code
	
		//selected messages now
	
	if((selectedMessage.tween.isCompleted()) && (selectedMessage.fadingIn != true) && (selectedMessages.size() >= 2)){ //got to have two or more messages
			//pick a random element of the selectedmessages vector, and use it
		int randomIndexToSelected = ofRandom(0, selectedMessages.size()-1);
		
		string idOfRandom = selectedMessages[randomIndexToSelected].id;
		
		while(idOfRandom == selectedMessage.tweetID){
			randomIndexToSelected = ofRandom(0, selectedMessages.size()-1);
			idOfRandom = selectedMessages[randomIndexToSelected].id;
		}
		
		string selectedMessageContent = selectedMessages[randomIndexToSelected].text;
		string selectedMessageDateTime = selectedMessages[randomIndexToSelected].created_at;
		string selectedMessageID = selectedMessages[randomIndexToSelected].id;
		string selectedMessageUser = selectedMessages[randomIndexToSelected].screen_name;
		
		selectedMessage.updateText(selectedMessageContent, selectedMessageDateTime, selectedMessageID, selectedMessageUser); 		
	}

	selectedMessage.update();
	
		//if any have moved off the bottom of the screen, remove them from the display vector
	
//	for(int i = (display.size()-1) ; i >= 0; i--){
//			//kill them if they are off the bottom of screen
//		if(display[i].onScreen && (display[i].messageYPos > ofGetHeight())){
//			display[i].onScreen = false; //so it can have it's text updated....
//			display[i].messageYPos = LIVE_MESSAGE_OFFSCREEN; //arbitary amount
//		}
//	}
	
	for(int i = 0 ; i < NUMBER_OF_LIVE_MESSAGES; i++){
			//kill them if they are off the bottom of screen
		if(display[i]->onScreen && (display[i]->messageYPos > ofGetHeight())){
			display[i]->onScreen = false; //so it can have it's text updated....
			display[i]->messageYPos = LIVE_MESSAGE_OFFSCREEN; //arbitary amount
		}
	}	
	
		//make sure there are sufficient messages on and above screen to keep the illusion of a continuous scroll
	
	float totalHeight = totalHeightOfDisplayMessages();	
	
	if(current.size() > 0){ //at the beginning, current may be empty, not really any more
			//was while, now if
		
		if(totalHeight < (ofGetHeight()+TOP_BUFFER_ZONE_HEIGHT)){ //keep adding to the display list to make sure that no gaps appear at the top of the display
																	 //lets add another
			int randomMessageIndex = (int)ofRandom(0.f, current.size()-1.f);
			
			bool isAlreadyDisplayed = isMessageDisplayed(current[randomMessageIndex].id);

			int messageIndexNotOnScreen = getSafeIndexFromDisplay(); //returns -1 if can't find a safe index
			
			if(!isAlreadyDisplayed && messageIndexNotOnScreen != -1){ //so its not on screen AND we have an empty index to change
				string liveMessageContent = current[randomMessageIndex].text;
				string liveMessageDateTime = current[randomMessageIndex].created_at;
				string liveMessageID = current[randomMessageIndex].id;
				string liveMessageUser = current[randomMessageIndex].screen_name;
				bool isLive = true;
				int duration = -1;
				int delay = -1;
				bool isOnScreen = true;
				
					//	void init(string _tweetID, string someUser, string someContent, string someDateTime, 
					//ofxPango* _pango, bool _isLive, unsigned int _duration, unsigned int _delay, bool _onScreen);
				
					//where do we add it?
				float highestPosition = yPositionOfHighestMessage();
				
				if(display[messageIndexNotOnScreen]->inited){
					display[messageIndexNotOnScreen]->updateText(liveMessageContent, liveMessageDateTime, 
																liveMessageID, liveMessageUser); //screen_name NOT name
				}else{
					display[messageIndexNotOnScreen]->init(liveMessageID, liveMessageUser, 
														  liveMessageContent, liveMessageDateTime, 
														  pango, isLive, duration, delay, isOnScreen);//TODO: fix this dirtyness, shouldn't have to
					cout << "Initing - why?" << endl;
				}
				
				display[messageIndexNotOnScreen]->messageYPos = highestPosition - display[messageIndexNotOnScreen]->messageHeight;
				
				display[messageIndexNotOnScreen]->onScreen = true;
				
				totalHeight = totalHeightOfDisplayMessages();
			}
		}		
	}
	
	prevFrameTime = currentTime;
}


//--------------------------------------------------------------
void testApp::draw(){
	ofPushStyle();
	ofEnableAlphaBlending();	

	for(int i=0; i < NUMBER_OF_LIVE_MESSAGES; i++){
		
		if(display[i]->userName == "TurnerPrize_SMS"){
				//if the tweet is from turnerprize_sms, then it's been submitted by text, so they can retweet properly.....
			display[i]->draw(98.f,0, &mobileIcon); // draw ignores y position, but takes x
		}else{
				//its a tweet from a live user
			display[i]->draw(98.f,0, &twitterIcon); // draw ignores y position, but takes x
		}
	}
	
	selectedMessage.draw(512+72,(768/2.f)-100, &twitterIcon); //twitter icon isn't used...
	
	drawStaticElements();	
	
	ofDisableAlphaBlending();
	ofPopStyle();
	
		// are we loading
	if(twitter.isLoading()) {
//		ofSetColor(255, (ofGetFrameNum()%2)*255, 255);
//		ofDrawBitmapString("Loading...", 100, 15);	
	}		
}

void testApp::drawStaticElements(){
	ofPushStyle();
	
	ofSetColor(255, 255, 255);
	ofFill();	
	
	backgroundImage.draw(0,0,1024,768);
	
	ofPopStyle();	
}

//--------------------------------------------------------------
void testApp::keyPressed  (int key){	
	switch (key){
		case 'f':{
			ofToggleFullscreen();
			break;
		}
		case 't':{
			break;
		}
		case 'm':{
			if(bShowMouse){
				ofHideCursor();
				bShowMouse = false;
			}else {
				ofShowCursor();
				bShowMouse = true;
			} 
			break;				
		}		
		case 'n':{
			break;
		}
		case '+':{
			messageSpeed = messageSpeed + 10.f;	
				//cout << "New Message Speed: " << messageSpeed << endl;
			break;
		}
		case '-':{
			if(messageSpeed > 10.f){
				messageSpeed = messageSpeed - 10.f;
					//cout << "New Message Speed: " << messageSpeed << endl;
			}
			break;
		}
	}	
}

//--------------------------------------------------------------
void testApp::keyReleased(int key){

}

//--------------------------------------------------------------
void testApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void testApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button){
}

//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::resized(int w, int h){

}

	//--------------------------------------------------------------
	//returns the total height of the current display vector in pixels
float testApp::totalHeightOfDisplayMessages(){
	float _totalHeight = 0.f;
	
	for(int i=0; i < NUMBER_OF_LIVE_MESSAGES; i++){
			//add them all up
		if(display[i]->onScreen){
			_totalHeight += display[i]->messageHeight;			
		}
	}
	
	return _totalHeight;
} 

float testApp::yPositionOfHighestMessage(){
	float _highestPosition = ofGetHeight();
	
	for(int i=0; i < NUMBER_OF_LIVE_MESSAGES; i++){
		if(display[i]->onScreen){
			float heightOfCurrentMessage = display[i]->messageYPos;
				//which is the highest? I.E. smallest height value over all...)
			if(heightOfCurrentMessage < _highestPosition){
				_highestPosition = heightOfCurrentMessage;
			}			
		}
	}
	
	return _highestPosition;
}

bool testApp::isMessageCurrent(string _tweetID){
	bool _isCurrent = false;
	
	for(int i = 0; i < current.size(); i++){
		if(_tweetID == current[i].id){
			_isCurrent = true;
		}
	}	
	
	return _isCurrent;
}

bool testApp::isMessageDisplayed(string _tweetID){
	bool _isDisplayed = false;
	
	for(int i = 0; i < NUMBER_OF_LIVE_MESSAGES; i++){
		if((_tweetID == display[i]->tweetID) && display[i]->onScreen){ //so it's onscreen and the same ID
			_isDisplayed = true;
		}
	}	
	
	return _isDisplayed;
}

bool testApp::isMessageInSelected(string _tweetID){
	bool _isInSelected = false;
	
	for(int i = 0; i < selectedMessages.size(); i++){
		if(_tweetID == selectedMessages[i].id){
			_isInSelected = true;
		}
	}	
	
	return _isInSelected;
}

	//hello wall below
	//--------------------------------------------------------------
void testApp::twitterResponse(TwitterResponseArgs &e) {
	
	cout << "Got a new twitter response in testApp of type " << e.type << endl;
	
	switch (e.type) {
		case TWITTER_TYPE_STATUS:
				//cout << "New status Response - why are these occuring?" << endl;
			break;
		case TWITTER_TYPE_USER:
				//cout << "New User Response - why are these occuring?" << endl;
			break;
		case TWITTER_TYPE_MENTIONS:
				//cout << "New Mention Response - why are these occuring?" << endl;
			break;
		case TWITTER_TYPE_RETWEETS:
			cout << "New Retweet Response!!!!! We have: " << e.tweets.size() << " new tweets" << endl;
			for(int i=0; i< e.tweets.size(); i++) {
				cout << "Tweet recieved but not processed: " << e.tweets[i].text.c_str() << endl;
				tweets.push(e.tweets[i]);
			}
			break;
		case TWITTER_TYPE_HASHTAG:
			cout << "New Hashtag Response!!!!!" << e.tweets.size() << " new tweets" << endl;
			for(int i=0; i< e.tweets.size(); i++) {
				cout << "Tweet recieved but not processed: " << e.tweets[i].text.c_str() << endl;
				tweets.push(e.tweets[i]);
			}
			break;				
	}
}

void testApp::processTweet(ofxTweet tweet){
	
	if(tweet.type == TWITTER_TYPE_HASHTAG){
			//then its a search for a mention of "#TurnerPrize"
			//so add it to the vector of live messages
		
		tweet.created_at = cleanUpDateFromLive(tweet.created_at);
		
		bool isNewMessage = !isMessageCurrent(tweet.id); 
		
			//cout << "PROCESS TWEET: Could have added TWITTER_TYPE_HASHTAG with message: " << newLiveMessageContent << ", from user: " << newLiveMessageUser << "at: " << newLiveMessageDateTime 
			//<< ", with ID " << newLiveMessageID << endl;
		
		if(isNewMessage){
				//cout << "It's new, so adding it to the current vector" << endl;
			int sizeOfCurrent = current.size();			
			
			while(sizeOfCurrent >= NUMBER_OF_LIVE_MESSAGES){
				current.erase(current.begin());	//make sure the size is correct	
				
				sizeOfCurrent = current.size();//make sure its updated
			}
			
			current.push_back(tweet);		
		}else{
				//cout << "Not adding it to the current vector as it's already in" << endl;
		}
	}else if(tweet.type == TWITTER_TYPE_RETWEETS){
		
		tweet.created_at = cleanUpDateFromRT(tweet.created_at);
		
		bool isInSelected = isMessageInSelected(tweet.id);
		
		if(!isInSelected){
				//if it isn't already in, add it to the vector, popping off if necessary
			int sizeOfSelected = selectedMessages.size();
			
			if(sizeOfSelected < NUMBER_OF_SELECTED_MESSAGES){
					//then we can just push it
				selectedMessages.push_back(tweet);
			}else{
				while(sizeOfSelected >= NUMBER_OF_SELECTED_MESSAGES){
						//while it's too big
					selectedMessages.erase(selectedMessages.begin());
											//selectedMessages.pop_back();	//make sure the size is correct	NO because this is the youngest message
					sizeOfSelected = selectedMessages.size();
				}
					//now we can push back safely
				selectedMessages.push_back(tweet);
			}
		}
	}
	
	
}

void testApp::popAndProcessATweet(){
	if(!tweets.empty()){ //as long as we some to process
		ofxTweet nextToProcess = tweets.front();
			//cout << "About to process the tweet: " << nextToProcess.text << endl;
		processTweet(nextToProcess);
		tweets.pop(); //we've processed it so we can get rid of it
	} 
}

vector<string> testApp::tokeniseString(string inputString, string deliminators){
	vector<string> splitUp;
	
	StringTokenizer tokenizer(inputString, deliminators, StringTokenizer::TOK_TRIM);
	
	for (StringTokenizer::Iterator it = tokenizer.begin(); it != tokenizer.end(); ++it)
	{
			//std::cout << *it << std::endl;
		splitUp.push_back(*it);
	}	
	
	return splitUp;
}

vector<string> testApp::tokeniseTwitterMessage(ofxTweet tweet){
	vector<string> splitUp;
	
	splitUp = tokeniseString(tweet.text, " "); //break it up around the spaces
	
	return splitUp;
}

string testApp::cleanUpDateFromLive(string dateToClean){
	
		//format for live is Tue, 28 Sep 2010 11:42:33 +0000
	
		//cout << "format for live is Tue, 28 Sep 2010 11:42:33 +0000" << ".What I got for LIVE: " << dateToClean << endl;
	
	string dateToChange;
	
	vector<string> split = tokeniseString(dateToClean, " ");
	
	vector<string> splitTime = tokeniseString(split[4], ":");
	
	split[4] = splitTime[0] + ":" + splitTime[1];
	
	dateToChange = split[0] + " " + split[1] + " " + split[2] + " " + split[4];  //TODO: no hardcode! match instead...
	
	return dateToChange;
}

string testApp::cleanUpDateFromRT(string dateToClean){
	
		//format for RT is: Wed Aug 11 15:49:03 +0000 2010
	
		//cout << "format for RT is: Wed Aug 11 15:49:03 +0000 2010" << ".What I got for RT: " << dateToClean << endl;	
	
	string dateToChange;
	
	vector<string> split = tokeniseString(dateToClean, " ");
	
	vector<string> splitTime = tokeniseString(split[3], ":");
	
	split[3] = splitTime[0] + ":" + splitTime[1];
	
	dateToChange = split[0] + ", " + split[2] + " " + split[1] + " " + split[3]; //swapping 2 and 1 to get to the identical format...
	
	//TODO: no hardcode! match instead...
	
	return dateToChange;
}

int testApp::getSafeIndexFromDisplay(){
	int safeIndex = -1;
	
	for(int i = 0; i < NUMBER_OF_LIVE_MESSAGES; i++){
		if(!display[i]->onScreen){
			safeIndex = i;
		}
	}	
	
	return safeIndex;	
}


	//
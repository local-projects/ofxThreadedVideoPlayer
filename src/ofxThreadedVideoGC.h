//
//  ofxThreadedVideoGC.h
//  BaseApp
//
//  Created by Oriol Ferrer Mesià on 13/05/14.
//
//

#pragma once

#include "ofMain.h"

class ofxThreadedVideoGC : public ofThread{

public:

	static ofxThreadedVideoGC* instance();
	void addToGarbageQueue(ofAVFoundationPlayer*);


private:

	ofxThreadedVideoGC(){}; //use instance()!
	static ofxThreadedVideoGC*	singleton;

	vector<ofAVFoundationPlayer*> videosPendingDeletion;

	void threadedFunction();
};
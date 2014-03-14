//
//  ofxThreadedVideoPlayer.cpp
//  BaseApp
//
//  Created by Oriol Ferrer Mesià on 04/03/14.
//
//

#include "ofxThreadedVideoPlayer.h"

ofxThreadedVideoPlayer::ofxThreadedVideoPlayer(){
	loadNow = playNow = stopNow = false;
	player = NULL;
	loaded = false;
	loopMode = OF_LOOP_NORMAL;
	needToNotifyDelegate = false;
}

void ofxThreadedVideoPlayer::loadVideo(string path){
	videopPath = path;
	loadNow = true;
	if(!isThreadRunning()) startThread();
}

void ofxThreadedVideoPlayer::play(){
	playNow = true;
	if(!isThreadRunning()) startThread();
}

void ofxThreadedVideoPlayer::stop(){
	stopNow = true;
}

void ofxThreadedVideoPlayer::setLoopMode(ofLoopType loop){
	loopMode = loop;
	if(player){
		player->setLoopState(loop);
	}
}





void ofxThreadedVideoPlayer::threadedFunction(){

	while(isThreadRunning()){

		if (loadNow){	//////////////////////////// LOAD
			lock();
				if(player){
					player->stop();
					delete player;
				}
				player = new ofxAVFVideoPlayerExtension();
				loaded = player->loadMovie(videopPath);
				needToNotifyDelegate = true;
				player->setLoopState(loopMode);
			unlock();
			loadNow = false;
		}

		if (stopNow){	/////////////////////////// STOP
			if(loaded){
				lock();
				player->setPaused(true);
				unlock();
			}else{
				cout << "can't stop playing before we load a movie!" << endl;
			}
			stopNow = false;
			stopThread();
		}

		if (playNow){	///////////////////////////// PLAY
			if(loaded){
				lock();
				player->setPaused(false);
				player->play();
				unlock();
			}else{
				cout << "can't play before we load a movie!" << endl;
			}
			playNow = false;
		}

		if(player){
			if(player->isReallyLoaded()){
				lock();
				player->update();
				unlock();
			}
		}

		ofSleepMillis(3); //mm todo!

		if (player){
			if(player->getIsMovieDone() && loopMode == OF_LOOP_NONE){
				//lock();
				//player->stop();
				//unlock();
				stopThread();
			}
		}
	}
}

bool ofxThreadedVideoPlayer::hasFinished(){
	bool ret = false;
	lock();
	ret = player->getIsMovieDone();
	unlock();
	return ret;
}

void ofxThreadedVideoPlayer::draw(float x, float y, float w, float h){
	if(player && loaded){
		if(player->isLoaded()){
			lock();
				player->draw(x,y, w, h);
			unlock();
		}
	}
}

void ofxThreadedVideoPlayer::draw(float x, float y, bool drawDebug){
	if(player && loaded){

		lock();
		bool reallyLoaded = player->isReallyLoaded();
		ofTexture * tex = player->getTexture();

		if( reallyLoaded && tex){

				if(needToNotifyDelegate){ //notify our delegate from the main therad, just in case (draw() always called from main thread)
					ofxThreadedVideoPlayerStatus status;
					status.path = videopPath;
					status.player = this;
					ofNotifyEvent( videoIsReadyEvent, status, this );
					needToNotifyDelegate = false;
				}
				tex->draw(x,y, player->getWidth(), player->getHeight() ); //doing this instead if drawing the player directly to avoid 2 textureUpdate calls (one to see if texture is there, one to draw)

		}
		unlock();

		if(drawDebug){
			string debug =	"isThreadRunning: " + ofToString(isThreadRunning()) + "\n" +
			"loadNow: " + ofToString(loadNow) + "\n" +
			"playNow: " + ofToString(playNow) + "\n" +
			"stopNow: " + ofToString(stopNow) + "\n" +
			"hasFinished: " + ofToString(hasFinished()) + "\n" +
			"position: " + ofToString(getPosition()) + "\n" +
			"loop: " + string(loopMode == OF_LOOP_NONE ? "NO" : "YES") + "\n"
			;

			ofDrawBitmapString(debug, x + 25, y + 55);
		}
	}
}



float ofxThreadedVideoPlayer::getWidth(){
	if(player){
		return player->getWidth();
	}
	return 0;
}


float ofxThreadedVideoPlayer::getHeight(){
	if(player){
		return player->getHeight();
	}
	return 0;
}


void ofxThreadedVideoPlayer::setPosition(float percent){
	if(player){
		lock();
		player->stop();
		player->play();
		player->setPosition( ofClamp(percent,0.0f,1.0f) );
		unlock();
		if(!isThreadRunning()) startThread();
	}
}

float ofxThreadedVideoPlayer::getPosition(){
	if(player){
		lock();
		float p = player->getPosition();
		unlock();
		return p;
	}
	return 0;
}


float ofxThreadedVideoPlayer::getDuration(){
	if(player){
		lock();
		float d = player->getDuration();
		unlock();
		return d;
	}
	return 0;
}
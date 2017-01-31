#include "ofApp.h"

int curBeat;

//--------------------------------------------------------------
void ofApp::setup(){
    ofBackground(0);
    ofSetVerticalSync(true);
    ofSetFrameRate(60);
    
    hotHand.setup();
    hotHand.startBPM();
    axis = hotHand.getAxisPointer();
    
    curBeat = 0;
    ofAddListener(hotHand.evTap, this, &ofApp::onTap);
}

void ofApp::onTap(){

    curBeat++;
    cout<<"curBeat "<<curBeat<<endl;
    if(curBeat>=4){
        int lastBPM = hotHand.getBPM();
        hotHand.resetBPM(lastBPM);
        curBeat=0;
    }
}

//--------------------------------------------------------------
void ofApp::update(){
    hotHand.update();
}

//--------------------------------------------------------------
void ofApp::draw(){
    ofPushMatrix();
    ofTranslate(ofGetWidth()/2, ofGetHeight()/2);
    ofRotate(ofGetElapsedTimef()*5, 0, 1, 0);
    ofScale(1+axis->x, 1+axis->y, 1+axis->z);
    float sc = 1+(hotHand.getBPMsin()/4);
    ofScale(sc, sc, sc);
    box.drawWireframe();
    ofPopMatrix();
    
    hotHand.drawDebug();
    
    ofPushMatrix();
    ofTranslate(20, ofGetHeight()/2);
    ofPushStyle();
    ofSetColor(255, 0, 0, 100);
    ofDrawRectangle(0, 0, 20, 100*axis->x);
    ofSetColor(0, 255, 0, 100);
    ofTranslate(40, 0);
    ofDrawRectangle(0, 0, 20, 100*axis->y);
    ofSetColor(0, 0, 255, 100);
    ofTranslate(40, 0);
    ofDrawRectangle(0, 0, 20, 100*axis->z);
    if(hotHand.getBPMMode()==ofxHotHand::BPM_AXIS_FUSION){
        ofSetColor(255, 255, 0, 100);
        ofTranslate(40, 0);
        ofDrawRectangle(0, 0, 20, 100*axis->length());
        ofSetColor(ofColor::red);
        float avg = hotHand.aFusion.getAverage()*100;
        float thr = hotHand.aFusion.getThreshold()*100;
        ofDrawLine(-5,avg,25,avg);
        ofDrawLine(35,thr,65,thr);
        ofSetColor(255,0,255,100);
        ofTranslate(40, 0);
        ofDrawRectangle(0, 0, 20, 100*hotHand.aFusion.getDeviation());//(axis->length()-hotHand.aFusion.getAverage()));
    }
    ofPopStyle();
    ofPopMatrix();
    
    ofDrawBitmapString(ofToString(ofGetFrameRate()), 20, 300);
    
    //cout<<"axis "<<*axis<<endl;
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    if(key=='b'){
        if(hotHand.getBPMMode()==ofxHotHand::BPM_AXIS_FUSION){
            hotHand.setBPMMode(ofxHotHand::BPM_AXIS_THRESH);
        }else{
            hotHand.setBPMMode(ofxHotHand::BPM_AXIS_FUSION);
        }
    }
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}

//
//  ofxHotHand.cpp
//
//  Created by enrico<nausea>viola on 7/5/16.
//
//

#include "ofxHotHand.h"

ofxHotHand::ofxHotHand(){
    midiIn = new ofxMidiIn();
    midiIn->addListener(this);
    hasListener = true;
    //this is the default control setting
    setMidiAxisControls(MIDI_CONTROL_CHANGE, 7, 8, 9);
    setMinMaxVals();
    bBPM = false;
    setBPMMode(ofxHotHand::BPM_AXIS_THRESH);
}

ofxHotHand::~ofxHotHand(){
    close();
}

void ofxHotHand::destroyMidi(){
    if(bDestroyMidi){
        if(midiIn!=NULL){
            delete midiIn;
            midiIn = NULL;
        }
    }
}

void ofxHotHand::close(){
    if(hasListener)midiIn->removeListener(this);
    destroyMidi();
}

void ofxHotHand::linkMidi(ofxMidiIn *_midiIn){
    destroyMidiOnCLose(true);
    close();
    midiIn = _midiIn;
    destroyMidiOnCLose(false);
}

void ofxHotHand::setMidiAxisControls(MidiStatus _status, int ctrX, int ctrY, int ctrZ){
    midiX.set(_status, ctrX);
    midiY.set(_status, ctrY);
    midiZ.set(_status, ctrZ);
}

void ofxHotHand::setBPMMode(ofxHotHand::BPMMode m){
    if(bpmMode==ofxHotHand::BPM_AXIS_FUSION){
        ofRemoveListener(aFusion.evHigh, this, &ofxHotHand::onHigher);
        ofRemoveListener(aFusion.evHigh, this, &ofxHotHand::onLower);
    }
    bpmMode = m;
    if(bpmMode==ofxHotHand::BPM_AXIS_FUSION){
        aFusion.reset();
        aFusion.addSample(axis.length());
        ofAddListener(aFusion.evHigh, this, &ofxHotHand::onHigher);
        ofAddListener(aFusion.evHigh, this, &ofxHotHand::onLower);
    }
}

void ofxHotHand::startBPM(){
    if(bBPM)return;
    bpmTapper.startFresh();
    bBPM = true;
}

void ofxHotHand::stopBPM(){
    if(!bBPM)return;
    bBPM = false;
}


bool ofxHotHand::setup(){
    vector<string> avPorts = midiIn->getPortList();
    for(int i=0;i<avPorts.size();i++){
        size_t found = ofStringTimesInString(avPorts[i], "Hot Hand");
        if((int)found>0){
            midiIn->openPort(i);
            resetAxis();
            cout<<"ofxHotHand::setup: "<<avPorts[i]<<" connected on port "<<i<<endl;
            return true;
        }
    }
    ofLogError("ofxHotHand::setup","cannot find a Hot Hand device");
    return false;
}

void ofxHotHand::update(){
    if(bBPM){
        bpmTapper.update();
    }
}

void ofxHotHand::drawDebug(){
    ofPushStyle();
    ofDrawBitmapString("Acceleration: ["+ofToString(axis.x,2)+", "+ofToString(axis.y,2)+", "+ofToString(axis.z,2)+"]", 20, 20);
    if(bBPM){
        ofDrawBitmapString("BPM: "+ofToString(bpmTapper.bpm()), 20, 35);
    }
    ofPopStyle();
}

bool ofxHotHand::isAxis(ofxMidiMessage &msg, ofxHotHand::MidiAxis &axis){
    return (msg.status==axis.status && msg.control==axis.control);
}

float ofxHotHand::midi2Axis(int midiVal){
    return ofMap((float)midiVal, minMidiVal, maxMidiVal, -1, 1);
}

void ofxHotHand::newMidiMessage(ofxMidiMessage &msg){
    if(isAxis(msg, midiX)){
        axis.x = midi2Axis(msg.value);
    }else if(isAxis(msg, midiY)){
        axis.y = midi2Axis(msg.value);
    }else if(isAxis(msg, midiZ)){
        axis.z = midi2Axis(msg.value);
    }
    checkTapping(msg);
}

void ofxHotHand::checkTapping(ofxMidiMessage &msg){
    if(!bBPM)return;
    switch (bpmMode) {
        case ofxHotHand::BPM_AXIS_THRESH:
            if(msg.status==MIDI_NOTE_OFF){
                tap();
            }
            break;
        case ofxHotHand::BPM_AXIS_FUSION:
            aFusion.addSample(axis.length());
            break;
            
        default:
            break;
    }
}

void ofxHotHand::onHigher(){
    //tap();
    //cout<<"HIGH"<<endl;
}

void ofxHotHand::onLower(){
    //tap();
    //cout<<"LOW"<<endl;
}
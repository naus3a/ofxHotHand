//
//  ofxHotHand.h
//
//  Created by enrico<nausea>viola on 7/5/16.
//
//

#pragma once
#include "ofxMidi.h"
#include "MSABPMTapper.h"
#include "ofxCircularBuffer.h"

class ofxHotHand : public ofxMidiListener{
public:
    class AxisFusion{
    public:
        inline AxisFusion(){
            thr = 0.1;
            reset();
        }
        
        inline void reset(){
            accHistory.setSize(1000);
            bEnoughSamples = false;
            curNSamples = 0;
            avg = 0;
            bBelow = true;
        }
        
        inline void addSample(float s){
            accHistory.setValue(s);
            accHistory.posUp();
            
            calcAvg();
            calcDeviation(s);
            checkThreshold();
        }
        
        inline void setThreshold(float _thr){thr=_thr;}
        
        inline float getAverage(){return avg;}
        inline float getDeviation(){return dev;}
        inline float getThreshold(){return thr;}
        
        ofEvent<void> evHigh;
        ofEvent<void> evLow;
    protected:
        inline void checkThreshold(){
            bool nowBelow = (dev<=thr);
            if(!nowBelow && bBelow){
                cout<<"CIPPA "<<ofGetElapsedTimef()<<endl;
            }
            bBelow = nowBelow;
        }
        
        inline void calcDeviation(float _s){
            dev = _s-avg;
        }
        
        inline void calcAvg(){
            if(!bEnoughSamples){
                curNSamples++;
                calcPartWinAvg();
                if(curNSamples>=accHistory.getSize()){
                    bEnoughSamples=true;
                }
            }else{
                calcWinAvg();
            }
        }
        
        inline void calcPartWinAvg(){
            avg = 0;
            for(int i=0;i<curNSamples;i++){
                avg+=accHistory.getValue(i);
            }
            avg/=curNSamples;
        }
        
        inline void calcWinAvg(){
            avg = 0;
            for(int i=0;i<accHistory.getSize();i++){
                ofxCircularBuffer<float>::BufferIterator it = accHistory.nthFromBegin(i);
                avg += (*it);
            }
            avg/=accHistory.getSize();
        }
        
        ofxCircularBuffer<float> accHistory;
        float avg;
        float dev;
        float thr;
        int curNSamples;
        bool bEnoughSamples;
        bool bBelow;
    };
    
    class MidiAxis{
    public:
        void set(MidiStatus _status, int _ctrl){
            status=_status;
            control = _ctrl;
        }
        MidiStatus status;
        int control;
    };
    
    enum BPMMode{
        BPM_AXIS_THRESH,
        BPM_AXIS_FUSION
    };
    
    ofxHotHand();
    ~ofxHotHand();
    void setBPMMode(BPMMode m);
    inline void close();
    inline void destroyMidiOnCLose(bool b){bDestroyMidi=b;}
    void linkMidi(ofxMidiIn * _midiIn);
    void setMidiAxisControls(MidiStatus _status, int xtrX, int ctrY, int ctrZ);
    inline void setMinMaxVals(float _min=0, float _max=127){minMidiVal=_min;maxMidiVal=_max;}
    
    bool setup();
    void update();
    void drawDebug();
    
    inline void resetAxis(){axis.set(1,1,1);}
    inline float getX(){return axis.x;}
    inline float getY(){return axis.y;}
    inline float getZ(){return axis.z;}
    inline ofVec3f getAxisVector(){return axis;}
    inline ofVec3f * getAxisPointer(){return &axis;}
    
    inline ofxMidiIn * getMidi(){return midiIn;}
    static bool isAxis(ofxMidiMessage & msg, MidiAxis & axis);
    float midi2Axis(int midiVal);
    
    void startBPM();
    void stopBPM();
    inline void toggleBPM(){bBPM?stopBPM():startBPM();}
    inline void resetBPM(){bpmTapper.startFresh();}
    inline void resetBPM(int _bpm){bpmTapper.startFresh(); bpmTapper.setBpm(_bpm);}
    inline void tap(){bpmTapper.tap();ofNotifyEvent(evTap, this);}
    inline float getBPMsin(){return bpmTapper.sin1;}
    inline float getBPM(){return bpmTapper.bpm();}
    inline BPMMode getBPMMode(){return bpmMode;}
    
    MidiAxis midiX;
    MidiAxis midiY;
    MidiAxis midiZ;
    MidiAxis midiBPM;
    
    ofEvent<void> evTap;
    AxisFusion aFusion;
protected:
    void destroyMidi();
    void newMidiMessage(ofxMidiMessage & msg);
    void checkTapping(ofxMidiMessage & msg);
    void onHigher();
    void onLower();
    
    ofxMidiIn * midiIn;
    msa::BPMTapper bpmTapper;
    
    ofVec3f axis;
    float minMidiVal;
    float maxMidiVal;
    BPMMode bpmMode;
    bool bDestroyMidi;
    bool hasListener;
    bool bBPM;
};
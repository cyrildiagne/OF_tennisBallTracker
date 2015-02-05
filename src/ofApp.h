#pragma once

#include "ofMain.h"
#include "ofxCv.h"
#include "ofxUI.h"
#include "BallMan.h"

//#define USE_CAMERA

class ofApp : public ofBaseApp{

public:
    void setup();
    void exit();
    void update();
    void draw();
    void keyPressed(int key);
    void mousePressed(int x, int y, int button);
    
#ifdef USE_CAMERA
    ofVideoGrabber camera;
#else
    ofVideoPlayer camera;
#endif
    
    BallMan perso;
    
    cv::Mat image;
    
    ofxCv::TrackingColorMode trackingColorMode;
    ofColor targetColor;
    
    float rescale;
    
    cv::Mat resizedFrame, hsvFrame;
    cv::Mat threshFrame, morphFrame;
    
    float minHue;
    float minSaturation;
    float minValue;
    
    float maxHue;
    float maxSaturation;
    float maxValue;
    
    int morphSize;
    
    ofxUICanvas * gui;
    bool bDebug;
    
    ofImage hsvOf, threshOf, morphOf;
    
    vector<cv::Vec3f> circles;
};

#include "ofApp.h"

void ofApp::setup(){
    ofSetFrameRate(60);
    ofSetVerticalSync(true);
    ofBackground(0);
    
#ifdef USE_CAMERA
    camera.setup(1280, 720);
#else
    camera.load("test.mov");
    camera.setVolume(0);
    camera.play();
    camera.setLoopState(OF_LOOP_NORMAL);
#endif
    
    rescale = 0.25;
    
    ofxCv::allocate(threshFrame, 1280*rescale, 720*rescale, CV_8U);
    
    hsvOf.allocate(1280*rescale, 720*rescale, OF_IMAGE_COLOR);
    threshOf.allocate(1280*rescale, 720*rescale, OF_IMAGE_GRAYSCALE);
    morphOf.allocate(1280*rescale, 720*rescale, OF_IMAGE_GRAYSCALE);
    
    bDebug = false;
    
    gui = new ofxUICanvas();
    gui->addLabel("HSV Min");
    gui->addSlider("minHue", 0, 180, &minHue);
    gui->addSlider("minSaturation", 0, 255, &minSaturation);
    gui->addSlider("minValue", 0, 255, &minValue);
    gui->addLabel("HSV Max");
    gui->addSlider("maxHue", 0, 180, &maxHue);
    gui->addSlider("maxSaturation", 0, 255, &maxSaturation);
    gui->addSlider("maxValue", 0, 255, &maxValue);
    gui->addLabel("Morph. Transformation");
    gui->addIntSlider("morphSize", 1, 10, &morphSize);
    gui->autoSizeToFitWidgets();
#ifdef USE_CAMERA
    gui->loadSettings("settings.xml");
#else
    gui->loadSettings("settings_debug.xml");
#endif
    gui->setVisible(bDebug);
    
    perso.setup();
}

void ofApp::exit(){
#ifdef USE_CAMERA
    gui->saveSettings("settings.xml");
#else
    gui->saveSettings("settings_debug.xml");
#endif
}



void ofApp::update(){
    
    ofSetWindowTitle(ofToString(ofGetFrameRate(), 1)+"fps");
    
    camera.update();
    
    if (camera.isFrameNew()) {

        cv::Mat im = ofxCv::toCv(camera);
        ofxCv::resize(im, resizedFrame, rescale, rescale);

        // hsv threshold
        
        cv::cvtColor(resizedFrame, hsvFrame, CV_BGR2HSV);
        
        cv::Scalar hsv_min  = cv::Scalar(minHue, minSaturation, minValue);
        cv::Scalar hsv_max  = cv::Scalar(maxHue, maxSaturation, maxValue);
        
        cv::inRange(hsvFrame, hsv_min, hsv_max, threshFrame);
        
        // erode / dilate morph
        
        int morph_elem = 2; // 0: Rect - 1: Cross - 2: Ellipse
        float msize = morphSize;
        cv::Mat element1 = getStructuringElement( morph_elem, cv::Size( 2*msize+1, 2*msize+1 ), cv::Point( msize, msize ) );
        cv::morphologyEx(threshFrame, morphFrame, cv::MORPH_OPEN, element1);
        
        cv::Mat element2 = getStructuringElement( morph_elem, cv::Size( 2*msize + 1, 2*msize+1 ), cv::Point( msize, msize ) );
        cv::morphologyEx(morphFrame, morphFrame, cv::MORPH_CLOSE, element2);
        
        
        // Reduce the noise so we avoid false circle detection
//        cv::GaussianBlur( morphFrame, morphFrame, cv::Size(9, 9), 2, 2 );
        
        // hough transform
        
        circles.clear();
        cv::HoughCircles( morphFrame, circles, CV_HOUGH_GRADIENT, 1, 20, 255, 1, 0, 0 );

        // update of images
        
        ofxCv::toOf(hsvFrame, hsvOf.getPixels());
        hsvOf.update();
        
        ofxCv::toOf(threshFrame, threshOf.getPixels());
        threshOf.update();
        
        ofxCv::toOf(morphFrame, morphOf.getPixels());
        morphOf.update();
        
        // update our character
        if(circles.size()){
            perso.set(circles[0][0]/rescale, circles[0][1]/rescale, circles[0][2]/rescale);
        }
    }
    
    perso.update();
}


void ofApp::draw(){
    
    ofPushMatrix();
    ofTranslate(ofGetWidth(), 0);
    ofScale(-1, 1);
    
    if(camera.getTexture().isAllocated()) {
        
        ofSetColor(255);
        
        camera.draw(0, 0);
//        ofPopMatrix();
        
        if(bDebug) {
            int w = 320;
            int h = 180;
            hsvOf.draw(0, 0, w, h);
            threshOf.draw(0, h, w, h);
            morphOf.draw(0, h*2, w, h);
            
            ofPushStyle();
            ofSetColor(255, 0, 0);
            ofNoFill();
            for (int i=0; i<circles.size(); i++) {
                ofDrawCircle(circles[i][0]/rescale, circles[i][1]/rescale, circles[i][2]/rescale);
            }
            ofPopStyle();
        }
    }
    perso.draw();
    ofPopMatrix();
}

void ofApp::keyPressed(int key){
    switch(key) {
        case ' ':
#ifndef USE_CAMERA
            camera.setPaused(!camera.isPaused());
#endif
            break;
        case 'd':
            bDebug = !bDebug;
            gui->setVisible(bDebug);
            break;
    }
}

void ofApp::mousePressed(int x, int y, int button){
    targetColor = camera.getPixels().getColor(x, y);
}
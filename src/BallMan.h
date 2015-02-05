//
//  BallMan.h
//  tennisBallTracker
//
//  Created by kikko on 04/02/15.
//
//

#pragma once

#include "ofMain.h"




#pragma mark VerletPoint -

class VerletPoint : public ofVec2f {
public:
    VerletPoint(bool isPinned=false):isPinned(isPinned){}
    
    ofVec2f prev;
    bool isPinned;
};




#pragma mark Leg -

class Leg {
    
public:
    
    Leg();
    ~Leg() {}
    
    void setup(int numSegments=15);
    void update(ofVec2f pos);
    void draw();
    
    ofVec2f origin;
    vector<VerletPoint> pts;
    float gravity;
    float segmentLength;
    float segmentAngle;
    float stiffness;
    
private:
    void applyUnitaryVerletIntegration(VerletPoint & p, float t);
    void applyUnitaryDistanceRelaxation(VerletPoint & p, const VerletPoint & from);
    
    ofMesh mesh;
};





#pragma mark BallMan -

class BallMan {
    
public:
    
    BallMan();
    ~BallMan();
    
    void setup();
    void update();
    void draw();
    
    void set(float x, float y, float radius);
    
private:
    
    ofShader shader;
    
    ofVec2f position, destPosition, velocity;
    float radius, destRadius;
    
    Leg leftArm;
    Leg rightArm;
    Leg leftLeg;
    Leg rightLeg;
};

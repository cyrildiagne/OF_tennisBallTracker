//
//  BallMan.cpp
//  tennisBallTracker
//
//  Created by kikko on 04/02/15.
//
//

#include "BallMan.h"


#pragma mark VerletPoint -

//VerletPoint




#pragma mark Leg -


Leg::Leg():gravity(0.0),segmentLength(5),segmentAngle(PI*0.5),stiffness(0.01){}

void Leg::setup(int numSegments){
    pts.push_back(VerletPoint(true));
    mesh.setMode(OF_PRIMITIVE_LINE_STRIP);
    
    for (int i=0; i<numSegments; i++) {
        pts.push_back(VerletPoint());
        mesh.addVertex(ofVec2f());
        mesh.addIndex(mesh.getNumVertices()-1);
    }
}

void Leg::update(ofVec2f pos){
    origin = pos;
    pts[0].set(origin);
    
    
    float t = ofGetElapsedTimef();
    
    // apply verlet integration
    ofVec2f prev;
    for (auto & p : pts) {
        prev.set(p);
        if(!p.isPinned) applyUnitaryVerletIntegration(p, t);
        p.prev.set(prev);
    }
    
    // apply relaxation
    int relaxIterations = 10;
    for (int r=0; r<relaxIterations; r++) {
        for (int i=1; i<pts.size(); i++) {
            VerletPoint & p = pts[i];
            if(!p.isPinned)
                applyUnitaryDistanceRelaxation(p, pts[i-1]);
        }
        for (int i=pts.size()-2; i>0; i--) {
            VerletPoint & p = pts[i];
            if(!p.isPinned)
                applyUnitaryDistanceRelaxation(p, pts[i+1]);
        }
    }
    
    int i=0;
    for (auto & p : pts) {
        mesh.setVertex(i, p);
        i++;
    }
}

void Leg::draw(){
//    for (int i=0; i<pts.size()-1; i++) {
//        ofDrawLine(pts[i], pts[i+1]);
//    }
    mesh.draw();
}

void Leg::applyUnitaryVerletIntegration(VerletPoint & p, float t){
    p.x = 2 * p.x - p.prev.x;
    p.y = 2 * p.y - p.prev.y + gravity * t;
}

void Leg::applyUnitaryDistanceRelaxation(VerletPoint & p, const VerletPoint & from){
    

    ofVec2f d = ofVec2f(p.x - from.x, p.y - from.y);
    float dstFrom = d.length();
    
    if (dstFrom > segmentLength && dstFrom != 0) {
        p -= (dstFrom - segmentLength) * (d / dstFrom) * 0.5;
    }
    
    // TODO: fix this - not correct
    float dA = p.angleRad(from);
    ofVec2f anchor = from + ofVec2f(segmentLength, 0).rotateRad(segmentAngle);
    p += (anchor-p) * stiffness;
}





#pragma mark BallMan -




BallMan::BallMan():radius(0)
{}

BallMan::~BallMan()
{}

void BallMan::setup(){
    leftLeg.setup(20);
    rightLeg.setup(20);
    leftArm.setup();
    rightArm.setup();
    
    shader.load("shaders/lines.vert", "shaders/lines.frag", "shaders/lines.geom");
    shader.begin();
    shader.setUniform1f("brightness", 0);
    shader.setUniform1f("thickness", 5);
    shader.end();
}

void BallMan::set(float x, float y, float _radius){
    destPosition.set(x, y);
    destRadius = _radius;
}

void BallMan::update(){
    velocity = (destPosition - position) * 0.5;
    position += velocity;
    radius += (destRadius - radius) * 0.5;
    
//    if(velocity.length() > 5) {
//        leftLeg.segmentAngle = rightLeg.segmentAngle = velocity.angleRad(ofVec2f(1,0)) - PI;
//        leftArm.segmentAngle = rightArm.segmentAngle = velocity.angleRad(ofVec2f(1,0)) - PI;
//    } else {
        leftLeg.segmentAngle = rightLeg.segmentAngle = PI*0.5;
        leftArm.segmentAngle = rightArm.segmentAngle = PI*0.5;
//    }
    
    leftArm.update( ofVec2f(position.x-radius, position.y) );
    rightArm.update( ofVec2f(position.x+radius, position.y) );
    
    ofVec2f r(radius, 0);
    r.rotate(60);
    leftLeg.update( ofVec2f(position.x-r.x, position.y+r.y) );
    rightLeg.update( ofVec2f(position.x+r.x, position.y+r.y) );
}

void BallMan::draw(){
    ofPushStyle();
    ofSetColor(0, 0, 0);
    
    ofFill();
    ofVec2f r(radius*0.75, 0);
    r.rotate(-50);
    float eyeR = 15.f / 200 * radius;
    ofDrawCircle(position.x+r.x, position.y+r.y, eyeR);
    ofDrawCircle(position.x-r.x, position.y+r.y, eyeR);
    
    
    ofNoFill();
    
    shader.begin();
    {
        float mouthWidth = radius * 0.5;
        ofDrawLine(position.x-mouthWidth*0.5, position.y-radius*0.35, position.x+mouthWidth*0.5, position.y-radius*0.35);
        leftArm.draw();
        rightArm.draw();
        leftLeg.draw();
        rightLeg.draw();
    }
    shader.end();
    
    ofPopStyle();
}
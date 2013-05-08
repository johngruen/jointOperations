//
//  Nodes.cpp
//  finalMkSix
//
//  Created by John Gruen on 4/28/13.
//
//

#include "Nodes.h"
#include "testApp.h"

Nodes::Nodes(int i_, string name_, bool isDraw_, bool isCursor_, bool isCam_, ofColor c_) {
    index = i_;
    name = name_;
    isDraw = isDraw_;
    isCursor = isCursor_;
    isCam = isCam_;
    c = c_;
    bubblesInit();
}

//captures and pushes
void Nodes::captureAndPush(vector<double> temp) {
    deWiggle(temp);
    push(1000);
}

//just captures
void Nodes::capture(vector<double> temp) {
    deWiggle(temp);
}

//pushes current frame to points vector, trims vector
void Nodes::push(int num) {
    if (easedFrame.x != 0 || easedFrame.y != 0 || easedFrame.z != 0 ) {
        points.push_back(easedFrame);
    }
    if (points.size() > num) points.erase(points.begin());
    
}

//applies easing to each frame as it comes in
void Nodes::deWiggle(vector<double>temp){
    frame.set(temp[0],temp[1],temp[2]);
    float dx = frame.x-pFrame.x;
    float dy = frame.y-pFrame.y;
    float dz = frame.z-pFrame.z;
    easedFrame.x += dx*cameraEasing;
    easedFrame.y += dy*cameraEasing;
    easedFrame.z += dz*cameraEasing;
    pFrame.set(easedFrame);
}

//DRAW
void Nodes::draw() {
    capMesh.clear();
    ofMesh mesh;
	mesh.setMode(OF_PRIMITIVE_TRIANGLE_STRIP);
	for(int i = 1; i < points.size(); i++){
        //find this point and the next point
		ofVec3f thisPoint = points[i-1];
		ofVec3f nextPoint = points[i];
        
		//get the direction from one to the next.
		//the ribbon should fan out from this direction
		ofVec3f direction = (nextPoint - thisPoint);
        
		//get the distance from one point to the next
		float distance = direction.length();
        
		//get the normalized direction. normalized vectors always have a length of one
		//and are really useful for representing directions as opposed to something with length
		ofVec3f unitDirection = direction.normalized();
        
		//find both directions to the left and to the right
		ofVec3f toTheLeft = unitDirection.getRotated(-90, ofVec3f(0,0,1));
		ofVec3f toTheRight = unitDirection.getRotated(90, ofVec3f(0,0,1));
        
		//use the map function to determine the distance.
		//the longer the distance, the narrower the line.
		//this makes it look a bit like brush strokes
		float thickness = ofMap(distance, 0, 60, 30, 2, true);
        
		//calculate the points to the left and to the right
		//by extending the current point in the direction of left/right by the length
		ofVec3f leftPoint = thisPoint+toTheLeft*thickness;
		ofVec3f rightPoint = thisPoint+toTheRight*thickness;
        
		//add these points to the triangle strip
		mesh.addVertex(ofVec3f(leftPoint.x, leftPoint.y, leftPoint.z));
		mesh.addVertex(ofVec3f(rightPoint.x, rightPoint.y, rightPoint.z));
        
    }
    ofEnableAlphaBlending();
    ofSetColor(c,225);
    capMesh = mesh;
    capMesh.draw();
    ofDisableAlphaBlending();
    bubblesMove();
    bubblesDraw();
    
    
}



//DEBUG BOX
void Nodes::debugBox() {
    ofSetColor(0);
    ofNoFill();
    ofBox(easedFrame.x,easedFrame.y,easedFrame.z,20);    
}

//EACH NODE DRAWS RANDOM BUBBLES...THIS MAKES IT HAPPEN
void Nodes::bubblesInit() {
    for(int i = 0; i < 100; i++) {
        bubbleState[i] = 0;
    }
}

void Nodes::bubblesMove() {
    for(int i = 0; i < 100; i++) {
        if (bubbleState[i] == 0) {
            if (points.size() > 100) {
                int p = points.size();
                int j = int (ofRandom(p-1));
                bubbleSeed[i] = points[j];
                bubbleMaxSize[i] = 5 + ofRandom(20);
                bubbleSize[i] = 0;
                bubbleGrowth[i] = .001+ofRandom(.08);
                bubbleTravel[i] = .1+ofRandom(.3);
                bubbleState[i] = 1;
            }
        }
        else if (bubbleState[i] == 1) {
            if (bubbleSize[i] < bubbleMaxSize[i]) {
                bubbleSize[i]+=bubbleGrowth[i];
                bubbleSeed[i].y+=bubbleTravel[i];
            } else {
                bubbleState[i] = 0;
            }
        }
    }
}

void Nodes::bubblesDraw() {
    for(int i = 0; i < 100; i++) {
        if (bubbleState[i] == 1) {
            ofEnableAlphaBlending();
            ofSetColor(100,100);
            ofFill();
            ofEllipse(bubbleSeed[i].x,bubbleSeed[i].y,bubbleSeed[i].z,bubbleSize[i],bubbleSize[i]);
            ofEndShape();
            ofDisableAlphaBlending();
        }
    }
}

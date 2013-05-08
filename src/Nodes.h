//
//  Nodes.h
//  finalMkThree
//
//  Created by John Gruen on 4/22/13.
//
//

#pragma once

#include "ofMain.h"

class Nodes {
    
    public:
    
    //CONSTRUCTOR---------------------------------------------------------------------------
    Nodes(int,string,bool,bool,bool,ofColor); 
    int index;//index in node array
    string name;//what is it called
    bool isCam;//is it the camera
    bool isCursor;//is it the cursor
    bool isDraw;
    ofColor c;//color
  
    
    //STREAM HANDLING---------------------------------------------------------------------------
    void captureAndPush(vector<double>); //capture new frame,
    void capture(vector<double>); //capture new frame,
    void deWiggle(vector<double>); //averages camAverage each frame
    void push(int);//add to points vector
    
    //points vector interpreted into mesh
    vector<ofVec3f> points;
    
    //capture of current frame from synapseStreamer
    ofVec3f frame;
    ofVec3f pFrame = ofVec3f(0,0,0);
    ofVec3f easedFrame = ofVec3f(0,0,0);
    //coefficent of easing
    float cameraEasing = .3;
    
    //DRAW---------------------------------------------------------------------------
    ofMesh capMesh;
    void draw();
    
    
    //DEBUG-----------------------------------------------------------------------------
    void debugBox(); //ball shows current position
    
    //BUBBLES---------------------------------------------------------------------------
    ofVec3f bubbleSeed[100];
    int bubbleFrame[100];
    float bubbleTravel[100];
    float bubbleSize[100];
    float bubbleMaxSize[100];
    float bubbleGrowth[100];
    int bubbleState[100];
    
    void bubblesInit();
    void bubblesMove();
    void bubblesDraw();

};
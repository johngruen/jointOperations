//
//  Toggles.h
//  finalMkSix
//
//  Created by John Gruen on 4/30/13.
//
//

#pragma once

#include "ofMain.h"


class Toggles {
    
    public:
    
    Toggles(string,float,float,float,float,int,int,ofTrueTypeFont);
    string name;
    float xPos;
    float yPos;
    float width;
    float height;
    int state;
    int type;
    ofRectangle sensorRect;

    float toggleTimer;
    
    ofTrueTypeFont swiss;
    
    void drawTypeZero(ofVec2f,ofVec2f);
    void drawTypeOne(ofVec2f,ofVec2f);
    
    bool listenForCrossRight(ofVec2f,ofVec2f);
    bool listenForCrossLeft(ofVec2f,ofVec2f);



};
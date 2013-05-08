//
//  Toggles.cpp
//  finalMkSix
//
//  Created by John Gruen on 4/30/13.
//
//

#include "Toggles.h"
#include "testApp.h"


Toggles::Toggles(string n, float x, float y, float w, float h, int s, int t, ofTrueTypeFont sw) {
    name = ofToUpper(n);
    
    xPos = x;
    yPos = y;
    width = w;
    height = h;
    state = s;
    type = t;
    swiss = sw;
};

void Toggles::drawTypeOne(ofVec2f selector, ofVec2f center) {
    ofFill();
    
    if(listenForCrossLeft(selector,center)) {
        ofSetColor(200);
        ofRect(xPos,yPos,width,height);
    } else {
        ofSetColor(255);
        ofRect(xPos,yPos,width,height);
    }
    
    if (state == 0) {
        toggleTimer = 0;
        if(listenForCrossLeft(selector,center)) state = 1;
    } else if (state == 1) {
        toggleTimer++;
        ofFill();
        ofEnableAlphaBlending();
        ofSetColor(255,29,37,180);
        float w = ofMap(toggleTimer,0,60,0,width);
        ofRect(xPos,yPos,w,height);
        ofDisableAlphaBlending();
        if(listenForCrossLeft(selector,center)) {
            if(toggleTimer >= 60) {
                state = 2;
                toggleTimer = 60;
            }
            } else {
                state = 0;
                toggleTimer = 0;
            }
    } else if (state == 2) {
        ofSetColor(255,29,37);
        ofRect(xPos,yPos,width,height);
    }

    ofSetColor(0);
    swiss.drawString(name,xPos+5,yPos+height-10);

}



void Toggles::drawTypeZero(ofVec2f selector, ofVec2f center) {
    ofFill();
    
    if(listenForCrossRight(selector,center)) {
        ofSetColor(200);
       ofRect(xPos,yPos,width,height); 
    } else {
        ofSetColor(255);
        ofRect(xPos,yPos,width,height);
    }
    if (state == 0) {
        //ofNoFill();
        //ofSetColor(0);
        //ofRect(xPos,yPos,width,height);
        toggleTimer = 0;
        if(listenForCrossRight(selector,center)) state = 1;
    } else if (state == 1) {
        toggleTimer++;
        ofFill();
        ofEnableAlphaBlending();
        ofSetColor(122,201,67,180);
        float w = ofMap(toggleTimer,0,30,0,width);
        ofRect(xPos,yPos,w,height);
        ofDisableAlphaBlending();
        //ofNoFill();
        //ofSetColor(0);
        //ofRect(xPos,yPos,width,height);
        if(listenForCrossRight(selector,center)) {
            if(toggleTimer >= 30) {
                state = 3;
                toggleTimer = 30;
            }
            } else {
                state = 0;
                toggleTimer = 0;
            }
    } else if (state == 3) {
        ofFill();
        ofSetColor(122,201,67);
        ofRect(xPos,yPos,width,height);
        //ofNoFill();
        //ofSetColor(0);
        //ofRect(xPos,yPos,width,height);
        if(!listenForCrossRight(selector,center)) state = 4;
    }  else if (state == 4) {
        ofFill();
        ofSetColor(122,201,67);
        ofRect(xPos,yPos,width,height);
        //ofNoFill();
        //ofSetColor(0);
        //ofRect(xPos,yPos,width,height);
        if(listenForCrossRight(selector,center)) state = 5;
    } else if( state == 5) {
        toggleTimer--;
        ofFill();
        ofEnableAlphaBlending();
        ofSetColor(122,201,67,180);
        float w = ofMap(toggleTimer,30,0,width,0);
        ofRect(xPos,yPos,w,height);
        ofDisableAlphaBlending();
        //ofNoFill();
        //ofSetColor(0);
        //ofRect(xPos,yPos,width,height);
        if(listenForCrossRight(selector,center)) {
            if(toggleTimer <= 0) {
                state = 6;
                toggleTimer = 0;
            }
        } else {
            state = 4;
            toggleTimer = 30;
        }
    } else if (state == 6) {
        //ofNoFill();
        //ofSetColor(0);
        //ofRect(xPos,yPos,width,height);
        toggleTimer = 0;
        if(!listenForCrossRight(selector,center)) state = 0;
    }
    ofSetColor(0);
    swiss.drawString(name,xPos+5,yPos+height-10);

    }
    


bool Toggles::listenForCrossRight(ofVec2f selector, ofVec2f center) {
    sensorRect.set(xPos+width,yPos,2,height + .2*height);
    ofPoint a;
    a.set(selector.x,selector.y,0);
    ofPoint b;
    b.set(center.x,center.y);
    return sensorRect.intersects(a,b);
    
}

bool Toggles::listenForCrossLeft(ofVec2f selector, ofVec2f center) {
    sensorRect.set(xPos,yPos,2,height + .2*height);
    ofPoint a;
    a.set(selector.x,selector.y,0);
    ofPoint b;
    b.set(center.x,center.y);
    return sensorRect.intersects(a,b);
    
}
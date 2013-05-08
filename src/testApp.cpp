#include "testApp.h"

//--------------------------------------------------------------
void testApp::setup(){
    
    setReferencePoints(5000,500);
    
    synapseStreamer.openSynapseConnection();
    synapseStreamer.trackAllJoints(true);
    
    initializeNodes();
    
    ofEnableSmoothing();
    ofSetFrameRate(30);
    
    swiss.loadFont("swiss.ttf",24,true,true);
    swiss.setLineHeight(ofGetHeight()*.3);
    swiss.setLetterSpacing(1);
    
    initializeToggles();
    ofEnableSmoothing();

    camInstructions.loadImage("images/camera.png");
    camInstructions.setAnchorPoint(camInstructions.getWidth(),0);
    penInstructions.loadImage("images/pens.png");
    instructions.loadImage("images/instructions.png");
    
    shader.load("shaders/noise.vert", "shaders/noise.frag");
    ofSetFullscreen(true);
}

//--------------------------------------------------------------
void testApp::update(){
    synapseStreamer.parseIncomingMessages();
    if ( synapseStreamer.getNewMessage() ) {
        receiving = true;
        receivingCounter = 0;
        if(selectionState == 1)setCaptures();
        else if (selectionState == 0) setCapturesAndPush();
    } else {
        receivingCounter++;
        if (receivingCounter > 5){
            receiving = false;
            selectionState = 1;
            for(int i = 0; i < 14; i ++) {
                nodes[i].points.clear();
                
            }
            pastColors.clear();
            pastMeshes.clear();
            penToggles[1].state = 4;
            penToggles[6].state = 4;
            penToggles[1].toggleTimer = 30;
            penToggles[6].toggleTimer = 30;
        }
    }
   

}

//--------------------------------------------------------------
void testApp::draw(){
     resizeToggles();
    screenCenter.set(ofGetWidth()/2,ofGetHeight()/2);
    ofBackgroundGradient(0xFFFFFF,0xF9EFD0,OF_GRADIENT_CIRCULAR);
    if(!receiving) {
        ofSetColor(255);
        instructions.draw(0,0,ofGetWidth(),ofGetHeight());
    }
    stateMachine();
}

void testApp::drawPastMeshes() {
    ofEnableAlphaBlending();
    for(int i = 0; i < pastMeshes.size();i++) {
        ofSetColor(pastColors[i],160);
        pastMeshes[i].draw();
    }
 
    ofDisableAlphaBlending();
}

void testApp::clearAllNodes() {
    if(nodes[1].easedFrame.z > nodes[7].easedFrame.z+100 && nodes[6].easedFrame.z > nodes[7].easedFrame.z+100) {
        if (clearingTimer > 60) {
            for(int i = 0; i < 14; i ++) {
                nodes[i].points.clear();
                              
            }
            pastColors.clear();
            pastMeshes.clear();
            
        } else {
            clearingTimer++;
        }
    } else {
        clearingTimer = 0;
    }
}

void testApp::stateMachine() {
    if (receiving) {
    if(selectionState == 0) {
        ofPushMatrix();
        setTranslationCenterForDraw(-200);
        shader.begin();
        //we want to pass in some varrying values to animate our type / color
        shader.setUniform1f("timeValX", ofGetElapsedTimef() * 0.001 );
        shader.setUniform1f("timeValY", -ofGetElapsedTimef() * 0.0018 );
        drawPastMeshes();
        drawNodes();
        shader.end();
        ofEnableAlphaBlending();
        
        for(int i = 0; i < 14; i++) {
            if(nodes[i].isDraw) {
                ofSetColor(122,201,67,150);
                ofEllipse(nodes[i].easedFrame.x,nodes[i].easedFrame.y,nodes[i].easedFrame.z,30,30);
            } 
        }
        //debuggery
        if (showReferenceGrid) drawReferenceGrid(2500);
        if (showDebugLines) drawDebugLines();
        if (showDebugBoxes) drawDebugBoxes();
        ofPopMatrix();
        ofSetColor(255,175,0);
        ofFill();
        ofEllipse(screenCenter.x+85,screenCenter.y,selectionTimer*6,selectionTimer*6);
        ofEllipse(screenCenter.x-85,screenCenter.y,selectionTimer*6,selectionTimer*6);
        ofSetColor(255,0,0);
        ofEllipse(screenCenter.x,screenCenter.y,clearingTimer*3,clearingTimer*3);
        ofNoFill();
        clearAllNodes();

        newCamIndex = -1;
        if (switchtoSelector()) {
           selectionState = 1;
           justSelected = true;
        }
    } else if(selectionState == 1) {
        if(justSelected) {
        //we're going t o flush all active lines + colors into a vector
            for(int i = 0; i < 14; i++) {
                if(nodes[i].isDraw) {
                    pastMeshes.push_back(nodes[i].capMesh);
                    pastColors.push_back(nodes[i].c);
                    if (pastMeshes.size() > 50) pastMeshes.erase(pastMeshes.begin());
                    if (pastColors.size() > 50) pastColors.erase(pastColors.begin());
                    
                    
                }
                nodes[i].points.clear();
            }
            
        }
        ofPushMatrix();
        setTranslationCenterForDraw(-200);
        shader.begin();
        //we want to pass in some varrying values to animate our type / color
        shader.setUniform1f("timeValX", ofGetElapsedTimef() * 0.001 );
        shader.setUniform1f("timeValY", -ofGetElapsedTimef() * 0.0018 );
        drawPastMeshes();
        shader.end();
        //debuggery
        if (showReferenceGrid) drawReferenceGrid(2500);
        if (showDebugLines) drawDebugLines();
        if (showDebugBoxes) drawDebugBoxes();
        ofPopMatrix();
        ofEnableAlphaBlending();
        ofFill();
        ofSetColor(200,220);
        ofRect(0,0,ofGetWidth(),ofGetHeight());
        drawFlatLines();
        drawToggles();
        
        for(int i = 0; i < 14; i++) {

            
            if (camToggles[i].state == 2) {
                ofLogNotice() << i;
                newCamIndex = i;
                for(int j = 0; j < 14; j++) {
                    camToggles[j].state = 0;
                    nodes[j].isCam = false;
                }
                break;
            }
        }
        if(newCamIndex != -1) {
            int t = 0;
            for(int i = 0; i < 14; i++) {
                nodes[i].isDraw = false;
                
                if(penToggles[i].state == 4) {
                    t++;
                    nodes[i].isDraw = true;
                    nodes[i].c = ofColor(ofRandom(255),ofRandom(255),ofRandom(255));
                }
            }
            if (t == 0) {
                penToggles[1].state = 4;
                nodes[1].isDraw = true;
                nodes[1].c = ofColor(ofRandom(255),ofRandom(255),ofRandom(255));
                penToggles[6].state = 4;
                nodes[6].isDraw = true;
                nodes[6].c = ofColor(ofRandom(255),ofRandom(255),ofRandom(255));
                
            }

            nodes[newCamIndex].isCam = true;
            selectionState = 0;
        }

        justSelected = false;
    }
}
}
void testApp::manualFlush() {
    for(int i = 0; i < 14; i ++) {
        nodes[i].points.clear();
        
    }
    pastColors.clear();
    pastMeshes.clear();
    penToggles[1].state = 4;
    penToggles[6].state = 4;
    penToggles[1].toggleTimer = 30;
    penToggles[6].toggleTimer = 30;
}

bool testApp::switchtoSelector() {
    float d = nodes[1].easedFrame.distance(nodes[6].easedFrame);
    if (d < 175 && nodes[1].easedFrame.y > nodes[0].easedFrame.y) {
        selectionTimer++;
        ofSetColor(0);
        ofFill();
        
        if (selectionTimer >= 30) {
            selectionTimer = 0;
            return true;
        } else return false;
    } else {
        selectionTimer = 0;
        return false;
    }
}

//--------------------------------------------------------------
void testApp::initializeNodes() {
    nodes.reserve(14);
    nodes.push_back(Nodes(0,"head",false,false,true,ofColor(ofRandom(255),ofRandom(255),ofRandom(255))));
    nodes.push_back(Nodes(1,"leftHand",true,true,false,ofColor(ofRandom(255),ofRandom(255),ofRandom(255))));
    nodes.push_back(Nodes(2,"leftElbow",true,false,false,ofColor(ofRandom(255),ofRandom(255),ofRandom(255))));
    nodes.push_back(Nodes(3,"leftShoulder",false,false,false,ofColor(ofRandom(255),ofRandom(255),ofRandom(255))));
    nodes.push_back(Nodes(4,"rightShoulder",true,false,false,ofColor(ofRandom(255),ofRandom(255),ofRandom(255))));
    nodes.push_back(Nodes(5,"rightElbow",true,false,false,ofColor(ofRandom(255),ofRandom(255),ofRandom(255))));
    nodes.push_back(Nodes(6,"rightHand",true,true,false,ofColor(ofRandom(255),ofRandom(255),ofRandom(255))));
    nodes.push_back(Nodes(7,"torso",true,false,false,ofColor(ofRandom(255),ofRandom(255),ofRandom(255))));
    nodes.push_back(Nodes(8,"leftHip",true,false,false,ofColor(ofRandom(255),ofRandom(255),ofRandom(255))));
    nodes.push_back(Nodes(9,"leftKnee",true,false,false,ofColor(ofRandom(255),ofRandom(255),ofRandom(255))));
    nodes.push_back(Nodes(10,"leftFoot",true,false,false,ofColor(ofRandom(255),ofRandom(255),ofRandom(255))));
    nodes.push_back(Nodes(11,"rightKnee",true,false,false,ofColor(ofRandom(255),ofRandom(255),ofRandom(255))));
    nodes.push_back(Nodes(12,"rightFoot",true,false,false,ofColor(ofRandom(255),ofRandom(255),ofRandom(255))));
    nodes.push_back(Nodes(13,"rightHip",true,false,false,ofColor(ofRandom(255),ofRandom(255),ofRandom(255))));
}

//--------------------------------------------------------------
void testApp::initializeToggles() {
    penToggles.reserve(14);
    camToggles.reserve(14);
    float w = ofGetWidth();
    float h = ofGetHeight();
    for (int i = 0; i < 14; i++) {
        penToggles.push_back(Toggles(jointNames[i],.015*w, 015*h + (i*.07*h) ,.2*w,.06*h,0,0,swiss));
        camToggles.push_back(Toggles(jointNames[i],0.785*w,.015*h + (i*.07*h),.2*w,.06*h,0,1,swiss));
    }
}

void testApp::drawToggles() {
    for (int i = 0; i < 14; i++) {
        penToggles[i].drawTypeZero(selectorLeft,screenCenter);
        camToggles[i].drawTypeOne(selectorLeft,screenCenter);
    }
}

void testApp::resizeToggles() {
    float w = ofGetWidth();
    float h = ofGetHeight();
    for (int i = 0; i < 14; i++) {
        penToggles[i].xPos = .015*w;
        penToggles[i].yPos = .01*h + (i*.07*h);
        penToggles[i].width = .2*w;
        penToggles[i].height =.06*h;
        camToggles[i].xPos = .785*w;
        camToggles[i].yPos = .01*h + (i*.07*h);
        camToggles[i].width = .2*w;
        camToggles[i].height =.06*h;
        
    }
}

void testApp::toggleCrossings() {
    for (int i = 0; i < 14; i++) {
        //toggles[i].listenForCross(selectorLeft,screenCenter);
    }
}

//--------------------------------------------------------------
void testApp::setCaptures() {
    nodes[0].capture(synapseStreamer.getHeadJoint());
    nodes[1].capture(synapseStreamer.getLeftHandJoint());
    nodes[2].capture(synapseStreamer.getLeftElbowJoint());
    nodes[3].capture(synapseStreamer.getLeftShoulderJoint());
    nodes[4].capture(synapseStreamer.getRightShoulderJoint());
    nodes[5].capture(synapseStreamer.getRightElbowJoint());
    nodes[6].capture(synapseStreamer.getRightHandJoint());
    nodes[7].capture(synapseStreamer.getTorsoJoint());
    nodes[8].capture(synapseStreamer.getLeftHipJoint());
    nodes[9].capture(synapseStreamer.getLeftKneeJoint());
    nodes[10].capture(synapseStreamer.getLeftFootJoint());
    nodes[11].capture(synapseStreamer.getRightHipJoint());
    nodes[12].capture(synapseStreamer.getRightKneeJoint());
    nodes[13].capture(synapseStreamer.getRightFootJoint());
}

//--------------------------------------------------------------
void testApp::setCapturesAndPush() {
    nodes[0].captureAndPush(synapseStreamer.getHeadJoint());
    nodes[1].captureAndPush(synapseStreamer.getLeftHandJoint());
    nodes[2].captureAndPush(synapseStreamer.getLeftElbowJoint());
    nodes[3].captureAndPush(synapseStreamer.getLeftShoulderJoint());
    nodes[4].captureAndPush(synapseStreamer.getRightShoulderJoint());
    nodes[5].captureAndPush(synapseStreamer.getRightElbowJoint());
    nodes[6].captureAndPush(synapseStreamer.getRightHandJoint());
    nodes[7].captureAndPush(synapseStreamer.getTorsoJoint());
    nodes[8].captureAndPush(synapseStreamer.getLeftHipJoint());
    nodes[9].captureAndPush(synapseStreamer.getLeftKneeJoint());
    nodes[10].captureAndPush(synapseStreamer.getLeftFootJoint());
    nodes[11].captureAndPush(synapseStreamer.getRightHipJoint());
    nodes[12].captureAndPush(synapseStreamer.getRightKneeJoint());
    nodes[13].captureAndPush(synapseStreamer.getRightFootJoint());
}

//--------------------------------------------------------------
void testApp::drawFlatLines() {
     float w = ofGetWidth();
    float h = ofGetHeight();
    ofFill();
    ofSetColor(255);
    ofEllipse(screenCenter.x,screenCenter.y,.39*w,.39*w);
    
    ofVec2f x(1,0);
    ofVec2f y(0,1);
    
    arrowLeft.set(nodes[1].easedFrame.x-nodes[2].easedFrame.x,nodes[1].easedFrame.y-nodes[2].easedFrame.y);
    ofVec2f v(1,0);
    arrowLeft.normalize();
    
    arrowLeft.y = -arrowLeft.y;
    float angle = arrowLeft.angle(x);
    selectorLeft.set(arrowLeft.x,arrowLeft.y);
    selectorLeft = selectorLeft *  5000;
    arrowLeft = arrowLeft * 200;
    arrowLeft.map(screenCenter,x,y);
    selectorLeft.map(screenCenter,x,y);
    
    arrowRight.set(nodes[6].easedFrame.x-nodes[5].easedFrame.x,nodes[6].easedFrame.y-nodes[5].easedFrame.y);
    arrowRight.normalize();
    arrowRight.y = -arrowRight.y;
    
    selectorRight.set(arrowRight.x,arrowRight.y);
    selectorRight = selectorRight * 5000;
    arrowRight = arrowRight * 200;
    arrowRight.map(screenCenter,x,y);
    selectorRight.map(screenCenter,x,y);
    //float angle = arrowLeft.angle(screenCenter);

    ofFill();
    ofSetColor(0);
    ofEllipse(screenCenter.x,screenCenter.y,60,60);
    ofPushMatrix();
    
    ofTranslate(screenCenter.x,screenCenter.y);
    ofScale(-1,1);
    ofRotate(angle);
    ofRect(-w*.15,-30,w*.15,60);
    ofTriangle(-w*.15,-30,-w*.15,30,-w*.19,0);
    ofPopMatrix();
    
    ofSetLineWidth(3);
    ofLine(screenCenter.x,screenCenter.y,selectorLeft.x,selectorLeft.y);
    //ofLine(screenCenter.x,screenCenter.y,selectorRight.x,selectorRight.y);
    ofSetColor(0);
    ofLine(screenCenter.x,screenCenter.y,arrowLeft.x,arrowLeft.y);
    //ofLine(screenCenter.x,screenCenter.y,arrowRight.x,arrowRight.y);
    ofSetHexColor(0xF0F0f0);
    ofRect(0,0,w*.215,ofGetHeight());
    ofRect(.785*w,0,w*.215,ofGetHeight());
    ofEnableAlphaBlending();
    penInstructions.draw(.25*w,.05*h);
    camInstructions.draw(.75*w,h-215);
    ofDisableAlphaBlending();
    
}

//--------------------------------------------------------------
void testApp::setTranslationCenterForDraw(int zOffset) {
    ofTranslate(ofGetWidth()/2,ofGetHeight()/2,zOffset);
    ofScale(1,-1,1);
    for(int i = 0; i < nodes.size(); i++) {
        if(nodes[i].isCam == true) {
            cam.set(nodes[i].easedFrame);
            cam = -cam;
            ofTranslate(cam.x,cam.y,cam.z);
            break;
        }
    }
}

void testApp::setTranslationCenterForSelect(int zOffset) {
    ofTranslate(ofGetWidth()/2,ofGetHeight()/2,zOffset);
    ofScale(1,-1,1);
    cam.set(nodes[7].easedFrame);
    cam = -cam;
    ofTranslate(cam.x,cam.y,cam.z);
               
}

//--------------------------------------------------------------
void testApp::drawNodes() {
    for(int i = 0; i < nodes.size(); i++) {
        if(nodes[i].isDraw) nodes[i].draw();
    }
}

//--------------------------------------------------------------
void testApp::drawDebugBoxes() {
    for(int i = 0; i < nodes.size(); i++) {
        nodes[i].debugBox();
    }
}

//--------------------------------------------------------------
void testApp::drawDebugLines() {
    ofSetColor(0);
    ofVec3f m;
    m.set(nodes[3].easedFrame);
    m.middle(nodes[4].easedFrame);
    ofLine(nodes[0].easedFrame.x,nodes[0].easedFrame.y,nodes[0].easedFrame.z,m.x,m.y,m.z);
    ofLine(nodes[1].easedFrame.x,nodes[1].easedFrame.y,nodes[1].easedFrame.z,nodes[2].easedFrame.x,nodes[2].easedFrame.y,nodes[2].easedFrame.z);
    ofLine(nodes[3].easedFrame.x,nodes[3].easedFrame.y,nodes[3].easedFrame.z,nodes[2].easedFrame.x,nodes[2].easedFrame.y,nodes[2].easedFrame.z);
    ofLine(nodes[3].easedFrame.x,nodes[3].easedFrame.y,nodes[3].easedFrame.z,nodes[4].easedFrame.x,nodes[4].easedFrame.y,nodes[4].easedFrame.z);
    ofLine(nodes[5].easedFrame.x,nodes[5].easedFrame.y,nodes[5].easedFrame.z,nodes[4].easedFrame.x,nodes[4].easedFrame.y,nodes[4].easedFrame.z);
    ofLine(nodes[5].easedFrame.x,nodes[5].easedFrame.y,nodes[5].easedFrame.z,nodes[6].easedFrame.x,nodes[6].easedFrame.y,nodes[6].easedFrame.z);
    
    ofLine(nodes[7].easedFrame.x,nodes[7].easedFrame.y,nodes[7].easedFrame.z,nodes[4].easedFrame.x,nodes[4].easedFrame.y,nodes[4].easedFrame.z);
    ofLine(nodes[3].easedFrame.x,nodes[3].easedFrame.y,nodes[3].easedFrame.z,nodes[7].easedFrame.x,nodes[7].easedFrame.y,nodes[7].easedFrame.z);
    
    ofLine(nodes[7].easedFrame.x,nodes[7].easedFrame.y,nodes[7].easedFrame.z,nodes[8].easedFrame.x,nodes[8].easedFrame.y,nodes[8].easedFrame.z);
    ofLine(nodes[9].easedFrame.x,nodes[9].easedFrame.y,nodes[9].easedFrame.z,nodes[8].easedFrame.x,nodes[8].easedFrame.y,nodes[8].easedFrame.z);
    ofLine(nodes[9].easedFrame.x,nodes[9].easedFrame.y,nodes[9].easedFrame.z,nodes[10].easedFrame.x,nodes[10].easedFrame.y,nodes[10].easedFrame.z);
    
    ofLine(nodes[7].easedFrame.x,nodes[7].easedFrame.y,nodes[7].easedFrame.z,nodes[11].easedFrame.x,nodes[11].easedFrame.y,nodes[11].easedFrame.z);
    ofLine(nodes[12].easedFrame.x,nodes[12].easedFrame.y,nodes[12].easedFrame.z,nodes[11].easedFrame.x,nodes[11].easedFrame.y,nodes[11].easedFrame.z);
    ofLine(nodes[12].easedFrame.x,nodes[12].easedFrame.y,nodes[12].easedFrame.z,nodes[13].easedFrame.x,nodes[13].easedFrame.y,nodes[13].easedFrame.z);
    ofLine(nodes[8].easedFrame.x,nodes[8].easedFrame.y,nodes[8].easedFrame.z,nodes[11].easedFrame.x,nodes[11].easedFrame.y,nodes[11].easedFrame.z);
}

//--------------------------------------------------------------
void testApp::drawReferenceGrid(int minMax) {
    ofSetLineWidth(3);
    ofSetColor(255,0,0);
    ofLine(0,0,0,minMax,0,0);
    ofSetColor(0,255,0);
    ofLine(0,0,0,0,minMax,0);
    ofSetColor(0,0,255);
    ofLine(0,0,0,0,0,minMax);
    ofSetLineWidth(1);
    ofSetColor(255,0,0);
    ofLine(0,0,0,-minMax,0,0);
    ofSetColor(0,255,0);
    ofLine(0,0,0,0,-minMax,0);
    ofSetColor(0,0,255);
    ofLine(0,0,0,0,0,-minMax);
    ofSetColor(0);
    referencePoints.drawVertices();
}

//--------------------------------------------------------------
void testApp::setReferencePoints(int size, int step) {
    ofVec3f t;
    for(int x = -size; x < size; x+=step){
        for(int y = -size; y < size; y+=step){
            for(int z = -size; z < size; z+=step){
                t.set(x,y,z);
                referencePoints.addVertex(t);
            }
        }
    }
    
}

//--------------------------------------------------------------
void testApp::keyPressed(int key){
    ofLogNotice() << key;
    switch (key)
    {
        case 116:
            if (showReferenceGrid) {
                showReferenceGrid = false;
            } else {
                showReferenceGrid = true;
            }
            break;
        case 115:
            if (showDebugLines) {
                showDebugLines = false;
            } else {
                showDebugLines = true;
            }
            break;
        case 98:
            if (showDebugBoxes) {
                showDebugBoxes = false;
            } else {
                showDebugBoxes = true;
            }
            break;
        case 109:
            manualFlush();
            break;
        default:
            break;
    }


}

//--------------------------------------------------------------
void testApp::keyReleased(int key){

}

//--------------------------------------------------------------
void testApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void testApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void testApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void testApp::dragEvent(ofDragInfo dragInfo){ 

}
#pragma once

#include "ofMain.h"
#include "SynapseStreamer.h"
#include "Nodes.h"
#include "Toggles.h"


class testApp : public ofBaseApp{

	public:
		void setup();
		void update();
		void draw();

		void keyPressed  (int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);
    
        //Synapse streamer
        SynapseStreamer synapseStreamer;
    
        //Reference and Debugging
        void setReferencePoints(int, int);
        void drawReferenceGrid(int);
        void drawDebugBoxes();
        void drawDebugLines();
        ofMesh referencePoints;
        bool showReferenceGrid = false;
        bool showDebugBoxes = false;
        bool showDebugLines = false;
        ofVec2f screenCenter;
    
        //nodes for every joint
        vector<Nodes> nodes;
        void initializeNodes();
    
        //tests for receiving data from synapseStreamer
        bool receiving = false;
        int receivingCounter = 0;
    
        //test for whether we're in selecting or drawing state
        bool selecting = false;
        void setCaptures();
        void setCapturesAndPush();
    
        //CAMERA
        ofVec3f cam;
        void setTranslationCenterForDraw(int zOffset);
        void setTranslationCenterForSelect(int zOffset);
    
        //DRAW ACTIVE NODES
        void drawNodes();
    
        //2D PRESSED
        void drawFlatLines();
    ofVec2f arrowLeft;
    ofVec2f arrowRight;
    ofVec2f selectorLeft;
    ofVec2f selectorRight;
    
        ofTrueTypeFont swiss;
        string jointNames [14] = {"head","l. hand","l. elbow","l. shoulder","r. shoulder","r. elbow", "r. hand",
        "torso","l. hip","l. knee","l. foot","r. hip", "r. knee","r. foot"};
    
        //toggles
        vector<Toggles> penToggles;
        vector<Toggles> camToggles;
        //Toggles exit;
        void initializeToggles();
        void resizeToggles();
        void drawToggles();
        void toggleCrossings();
    
    //Images
    ofImage camInstructions;
    ofImage penInstructions;
    ofImage instructions;
    
    //Shaders
    ofShader shader;
    
    vector<ofMesh> pastMeshes;
    vector<ofColor>pastColors;
    void drawPastMeshes();
    
    void clearAllNodes();
    int clearingTimer = 0;
    
    //selection delegation & passback
    void stateMachine();
    int selectionState = 1;
    int selectionTimer = 0;
    int newCamIndex = -1;
    bool justSelected = true;
    
    bool switchtoSelector();
    
    void manualFlush();
};

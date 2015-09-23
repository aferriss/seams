#pragma once

#include "ofMain.h"

class testApp : public ofBaseApp{

	public:
		void setup();
		void update();
		void draw();

		void keyPressed(int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);
        void removeSeam(ofTexture srcTex);
        void convertGrayscale(ofPixels pixels);
        int getPixelLoc(ofPixels ppixels, int row, int col, int width);
        void findSeam();
        
        void calcGradient(ofTexture texture);
    
        vector<int> findMinSeam(vector<int> SeamTable, int minCol, int width, int height);
        int getNextMinCol(vector<int> rowArray, int col);
        void getEnergyMap(ofTexture tex);
        int fastMin(int x, int y);
    
    int w, h;
    float time;
    vector<int> seamTable;
    vector<int> allSeams;
    vector<int> seam;
    vector<vector<int> > totalSeams;
    
    
    ofImage img, greyImg, newImg;
    ofTexture tex;
    ofPixels pixels, greyPix, origPix;
    
    ofPixels np;
    ofImage ni;
    
    ofFbo fbo, energyFbo, rowFbo;
    ofShader baseShader, sobelShader, energyShader;
    ofVideoGrabber cam;
    bool findAll, drawSeam;
    
    int numSeamsRemoved;
};

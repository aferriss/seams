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
        void removeSeam(ofTexture &srcTex);
        void removeHSeam(ofTexture &srcTex);
        void addSeam(ofTexture &srcTex);
        //void convertGrayscale(ofPixels pixels);
        int getPixelLoc(ofPixels const &ppixels, int row, int col, int width);
        void findSeam();
        void findHorSeam();
    
    
        vector<int> findMinSeam(vector<int> const &SeamTable, int minCol, int width, int height);
        vector<int> findMinSeamH(vector<int> const &SeamTable, int minCol, int width, int height);
        int getNextMinCol(vector<int> const &rowArray, int col);
        void getEnergyMap(ofTexture &tex);
        int fastMin(int x, int y);
    
    int w, h;
    float time;
    vector<int> seamTable;
    vector<int> hSeamTable;
    
    vector<int> allSeams;
    
    vector<int> seam;
    vector<int> hSeam;
    vector<vector<int> > totalSeams;
    
    
    ofImage img, greyImg, newImg;
    ofTexture tex;
    ofPixels pixels, greyPix, origPix;
    ofPixels newPixels;
    ofPixels np;
    ofImage ni;
    
    ofFbo fbo, energyFbo, rowFbo;
    ofShader baseShader, sobelShader, energyShader;
    int newWidth, newHeight;
    int startingWidth, startingHeight;
    ofVideoGrabber cam;
    bool findAll, drawSeam;
    
    int numSeamsRemoved, numRowsRemoved;
    int numSeamsAdded;
    
};

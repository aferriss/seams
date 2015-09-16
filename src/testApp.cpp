#include "testApp.h"



//--------------------------------------------------------------
void testApp::setup(){

    sobelShader.load("base.vert", "sobel.frag");
    energyShader.load("base.vert", "energy.frag");
    
    img.loadImage("male.jpg");
    
    w = img.width;
    h = img.height;
    
    fbo.allocate(img.width, img.height, GL_RGBA);
    pixels.allocate(img.width, img.height, OF_PIXELS_RGBA);
    greyPix.allocate(img.width, img.height, OF_PIXELS_RGBA);
    
    pixels = img.getPixelsRef();
    if(img.bAllocated()){
     //   convertGrayscale(pixels);
    }
    
    
    greyImg.allocate(img.width, img.height, OF_IMAGE_COLOR);
    greyImg.setFromPixels(greyPix);
    
    ofSetWindowShape(img.width, img.height);
    
    
    ofSetBackgroundAuto(false);
    
    seamTable.resize(img.width * img.height);
    seam.resize(img.height);
    
    for (int i = 0; i<seamTable.size(); i++) {
        seamTable[i] = 255;
    }
    
    
}

//--------------------------------------------------------------
void testApp::update(){
   // if(row == 0){
   //     cam.update();
   // }
}



//--------------------------------------------------------------
void testApp::draw(){
    
    if(ofGetFrameNum() == 0){
    fbo.begin();
        sobelShader.begin();
            sobelShader.setUniformTexture("srcTex", img.getTextureReference(), 0);
            sobelShader.setUniform2f("step", 1.0, 1.0);
            sobelShader.setUniform1f("time", ofGetFrameNum());
            img.draw(0, 0);
        sobelShader.end();
    fbo.end();
    
        fbo.readToPixels(pixels);
        
        findSeam();
        
    fbo.begin();
        img.draw(0,0);
        
        ofColor c = ofColor(255,0,0);
        ofSetColor(c);
        unsigned char *newPix = img.getPixels();
        vector<unsigned char> newPixVec;
        
        for(int i =0; i< w*h; i++){
            newPixVec.push_back(newPix[i]);
        }
        
        newPixVec.erase(newPixVec.begin() + (10));
        
        for(int numSeams = 0; numSeams< totalSeams.size(); numSeams++){
            vector<int> curSeam = totalSeams[numSeams];
            
            for(int row = 0; row < img.height; row++){
                
                int colToRemove = curSeam[row];
                
                for(int col = 0; col < img.width; col++){
                    if(col  == colToRemove){
                        //ofRect(col, row, 1, 1);
                       
                    }
                }
            }
        }
        
        ofSetColor(255);
        
        fbo.readToPixels(pixels);
        ofSaveImage(pixels, ofGetTimestampString()+".png");
    fbo.end();
    }
    
    fbo.draw(0,0);
}
//--------------------------------------------------------------
void testApp::convertGrayscale(ofPixels ppixels){
    for(int i = 0; i < ppixels.size(); i+=4){
        int r = ppixels[i];
        int g = ppixels[i+1];
        int b = ppixels[i+2];
        
        //greyPix[i] = greyPix[i+1] = greyPix[i+2] = floor(0.2126*r + 0.7152*g + 0.0722*b);
        //greyPix[i+3] = 255;
    }
}
//--------------------------------------------------------------

int testApp::fastMin(int x, int y){
    return  y + ((x - y) & ((x - y) >>
                            (sizeof(int) * CHAR_BIT - 1)));
}
//--------------------------------------------------------------

void testApp::findSeam(){
    
    unsigned char *fboPix = pixels.getPixels();
    
    for(int y = 0; y < img.width; y++){
        seamTable[y] = fboPix[y*4];//getPixelLoc(pixels, 0, y, img.width);
    }
    
    for(int i = 1; i< img.height; i++){
        for(int j = 0; j< img.width; j++){
            int rowOffset = (i - 1) * img.width;
            int tl = seamTable[rowOffset + j -1];
            int t = seamTable[rowOffset + j];
            int tr = seamTable[rowOffset + j + 1];
            
            int loc = (i * img.width+j)*4;
            int seamLoc = (i*w+j);
            
            if(j == 0){ //if left edge
                seamTable[seamLoc] = fboPix[loc] + fastMin(t, tr);
            } else if( j == (img.width -1)){ //or right edge
                seamTable[seamLoc] = fboPix[loc] + fastMin(tl, t);
            } else{ //not on the edges
                
                //if everything the same choose random
                if(fboPix[loc] + tl == fboPix[loc] + t && fboPix[loc] + t == fboPix[loc] + tr ){
                    vector<int> choices;
                    choices.push_back(tl);
                    choices.push_back(t);
                    choices.push_back(tr);
                    
                    int randChoice = floor(ofRandom(3));
                    
                    seamTable[seamLoc] = fboPix[loc] + choices[randChoice];
                    
                } else{
                 
                    seamTable[seamLoc] = fboPix[loc] + fastMin(fastMin(tl, t), tr);
                }
            }
        }
    }
    
    
    int minCol = 0;
    vector<int> zeros;
    
    for(int j = 0; j<img.width; j++){
            zeros.push_back(j);
    }
    
    //vector<vector<int> > TOTALSEAMS;
    vector<int> foundSeam(h);
    
    for(int i = 0; i<zeros.size(); i++){
        foundSeam = findMinSeam(seamTable, zeros[i], img.width, img.height);
        totalSeams.push_back(foundSeam);
    }
    
    //return TOTALSEAMS;
    //return findMinSeam(seamTable, w/2, img.width, img.height);
    //return seamTable;
}
//--------------------------------------------------------------
vector<int> testApp::findMinSeam(vector<int> SeamTable, int minCol, int width, int height){
    //seam.clear();
    
    for(int i = img.height -1; i> 0; i--){
        seam[i] = minCol;
        vector<int>::const_iterator first = SeamTable.begin() + ((i-1)*img.width);
        vector<int>::const_iterator last = SeamTable.begin() + (i*img.width);
        vector<int> rowArray(first, last);
        minCol = getNextMinCol(rowArray, minCol);
    }
    
    seam[0] = minCol;
    
    return seam;
}
//--------------------------------------------------------------
int testApp::getNextMinCol(vector<int> rowArray, int col){
    if(col == 0){
        if(rowArray[col] < rowArray[col + 1]){
            return col;
        } else {
            return col + 1;
        }
    } else if( col == rowArray.size() -1){
        if(rowArray[col -1] < rowArray[col]){
            return col -1;
        } else{
            return col;
        }
    } else{
        
        if(rowArray[col-1] == rowArray[col] && rowArray[col] == rowArray[col+1] ){
            //if everything is the same choose randomly
            vector<int> choices;
            choices.push_back(col-1);
            choices.push_back(col);
            choices.push_back(col+1);
            int randChoice = floor(ofRandom(3));
            return choices[randChoice];
        }
        else if((rowArray[col -1] < rowArray[col]) && (rowArray[col -1] < rowArray[col + 1])){
            return col -1;
        } else if((rowArray[col] < rowArray[col - 1]) && (rowArray[col] < rowArray[col+1])){
            return col;
        } else{
            return col + 1;
        }
    }
}

//--------------------------------------------------------------
int testApp::getPixelLoc(ofPixels ppixels, int row, int col, int srcWidth){
    return ppixels[(row*srcWidth+col) * 4];
}

//--------------------------------------------------------------
void testApp::keyPressed(int key){

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

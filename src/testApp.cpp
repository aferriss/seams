#include "testApp.h"



//--------------------------------------------------------------
void testApp::setup(){

    sobelShader.load("base.vert", "sobel.frag");
    energyShader.load("base.vert", "energy.frag");
    

    img.loadImage("ss6.png");

    cout<<ofToString(img.getPixelsRef().getNumChannels())<<endl;
    w = img.width;
    h = img.height;
    
    fbo.allocate(img.width, img.height, GL_RGBA);
    pixels.allocate(img.width, img.height, OF_PIXELS_RGBA);
    origPix.allocate(img.width, img.height, OF_PIXELS_RGBA);
    greyPix.allocate(img.width, img.height, OF_PIXELS_RGBA);
    
    
    pixels = img.getPixelsRef();
    origPix = pixels;
    
    
    newImg.setUseTexture(true);
    
    ofSetWindowShape(img.width, img.height);
    
    
    ofSetBackgroundAuto(false);
    
    seamTable.resize(img.width * img.height);
    seam.resize(img.height);
    
    cout<<ofToString(seamTable.size())<<endl;
    
    for (int i = 0; i<seamTable.size(); i++) {
        seamTable[i] = 255;
    }
    
    findAll = false;
    drawSeam = false;
    numSeamsRemoved = 1;
    newWidth = img.width;
    
}

//--------------------------------------------------------------
void testApp::update(){
   // if(row == 0){
   //     cam.update();
   // }
    
    ofSetWindowTitle(ofToString(ofGetFrameRate()));
}



//--------------------------------------------------------------
void testApp::draw(){
    int count = 0;
    if(ofGetFrameNum() <= img.width - 50 && img.isAllocated()){
        fbo.allocate(newImg.width, newImg.height, GL_RGBA);

        fbo.begin();
            ofClear(0);
        fbo.end();
        
        if(ofGetFrameNum() == 0){
            getEnergyMap(img.getTextureReference());
        } else {
            getEnergyMap(newImg.getTextureReference());
        }
        
        ////////////sloooow
        fbo.readToPixels(pixels);
        ////////////
        
        
        findSeam();
        
        
        if(ofGetFrameNum() == 0){
            removeSeam(img.getTextureReference());
        } else{
            removeSeam(newImg.getTextureReference());
        }
        
        newImg.draw(0,0);
        
        
        pixels = newImg.getPixelsRef();
        //ofSaveImage(pixels, ofGetTimestampString()+".png");
        
    }
    
    ofClear(0);
    
    newImg.draw(floor((numSeamsRemoved-1)*0.5),0);
    
    
}
//--------------------------------------------------------------
void testApp::removeSeam(ofTexture &srcTex){
    
    fbo.begin();
        srcTex.draw(0, 0);
    fbo.end();
    
    int newOldWidth = img.width - numSeamsRemoved + 1;
    newWidth = img.width - numSeamsRemoved ;
    
    ofPixels pixelsToRemove;
    pixelsToRemove.allocate(newOldWidth, img.height, 4);
    fbo.readToPixels(pixelsToRemove);
    

    
    //ofPixels newPixels;
    newPixels.allocate(newWidth, h, 4);
    
    auto pixelsToRemovePtr = pixelsToRemove.getPixels();
    auto newPixelsPtr = newPixels.getPixels();
   
    
    //for(int numSeams = 0; numSeams< totalSeams.size(); numSeams++){
        vector<int> &curSeam = totalSeams[totalSeams.size()-1];
        
        for(int row = 0; row < img.height; row++){
            
            int colToRemove = curSeam[row];
            
            for(int col = 0; col < newOldWidth; col++){
                
                int newPixelIndex = (row* newWidth + col)*4;
                int oldPixelIndex = (row* newOldWidth + col)*4;
                
                /*
                if(col  == colToRemove && drawSeam){
                    ofColor c = ofColor(255,0,0);
                    ofSetColor(c);
                    ofRect(col, row, 1, 1);
                    ofSetColor(255);
                }
                */
                
                
                if(col < colToRemove){
                    newPixelsPtr[newPixelIndex] = pixelsToRemovePtr[oldPixelIndex];
                    newPixelsPtr[newPixelIndex+1] = pixelsToRemovePtr[oldPixelIndex+1];
                    newPixelsPtr[newPixelIndex+2] = pixelsToRemovePtr[oldPixelIndex+2];
                    newPixelsPtr[newPixelIndex+3] = pixelsToRemovePtr[oldPixelIndex+3];
                    
                    
                } else if (col > colToRemove){
                    newPixelIndex -= 4;
                    newPixelsPtr[newPixelIndex] = pixelsToRemovePtr[oldPixelIndex];
                    newPixelsPtr[newPixelIndex+1] = pixelsToRemovePtr[oldPixelIndex+1];
                    newPixelsPtr[newPixelIndex+2] = pixelsToRemovePtr[oldPixelIndex+2];
                    newPixelsPtr[newPixelIndex+3] = pixelsToRemovePtr[oldPixelIndex+3];
                }
            }
        }
    //}
    
    newImg.allocate(newWidth, h, OF_IMAGE_COLOR_ALPHA);
    newImg.setFromPixels(newPixels);
    numSeamsRemoved ++;

}
//--------------------------------------------------------------
int testApp::fastMin(int x, int y){
    return  y + ((x - y) & ((x - y) >>
                            (sizeof(int) * CHAR_BIT - 1)));
}
//--------------------------------------------------------------
void testApp::getEnergyMap(ofTexture &etex){
    fbo.begin();
        sobelShader.begin();
        sobelShader.setUniformTexture("srcTex", etex, 0);
        sobelShader.setUniform2f("step", 1.0, 1.0);
        sobelShader.setUniform1f("time", ofGetFrameNum());
            etex.draw(0, 0);
        sobelShader.end();
    fbo.end();

}
//--------------------------------------------------------------

void testApp::findSeam(){
    
    unsigned char *fboPix = pixels.getPixels();
    
    //fill the last row
    for(int y = 0; y < newWidth; y++){
        seamTable[y] = fboPix[y*4];//getPixelLoc(pixels, 0, y, img.width);
    }
    
    for(int i = 1; i< img.height; i++){
        for(int j = 0; j< newWidth; j++){
            int rowOffset = (i - 1) * newWidth;
            //int tl = seamTable[rowOffset + j -1];
            int t = seamTable[rowOffset + j];
            //int tr = seamTable[rowOffset + j + 1];
            
            int loc = (i * newWidth + j )*4;
            int seamLoc = ( i * newWidth + j );
            
            if(j == 0){ //if left edge
                seamTable[seamLoc] = fboPix[loc] + fastMin(t, seamTable[rowOffset + j + 1]);
            } else if( j == (newWidth -1)){ //or right edge
                seamTable[seamLoc] = fboPix[loc] ;//
                seamTable[seamLoc] += fastMin(seamTable[rowOffset + j -1], t);
            } else{ //not on the edges
                /*
                //if everything the same choose random
                if(fboPix[loc] + seamTable[rowOffset + j -1] == fboPix[loc] + t && fboPix[loc] + t == fboPix[loc] + seamTable[rowOffset + j + 1] ){
                    vector<int> choices;
                    choices.push_back(seamTable[rowOffset + j -1]);
                    choices.push_back(t);
                    choices.push_back(seamTable[rowOffset + j + 1]);
                    
                    int randChoice = floor(ofRandom(3));
                    
                    seamTable[seamLoc] = fboPix[loc] + choices[randChoice];
                    
                } else{
                 */
                seamTable[seamLoc] = fboPix[loc];
                seamTable[seamLoc] += fastMin(fastMin(seamTable[rowOffset + j -1], t), seamTable[rowOffset + j + 1]);
                //}
            }
        }
    }
    
    
    int minCol = 0;
    vector<int> zeros;
    
    for(int j = 0; j<newWidth; j++){
            zeros.push_back(j);
        
        if( getPixelLoc(pixels, img.height -1, j, newWidth) < getPixelLoc(pixels, img.height -1 , minCol, newWidth)){
            minCol = j;
        }
    }
    
    //vector<vector<int> > TOTALSEAMS;
    vector<int> foundSeam(h);
    
    if(findAll){
        for(int i = 0; i<zeros.size(); i++){
            foundSeam = findMinSeam(seamTable, zeros[i], newWidth, img.height);
            totalSeams.push_back(foundSeam);
        }
    } else{
        cout<<"Min seam = "+ofToString(minCol)<<endl;
        foundSeam = findMinSeam(seamTable, minCol, newWidth, img.height);
        totalSeams.push_back(foundSeam);
    }
}
//--------------------------------------------------------------
vector<int> testApp::findMinSeam(vector<int> const &SeamTable, int minCol, int width, int height){
    //seam.clear();
    
    for(int i = img.height -1; i> 0; i--){
        seam[i] = minCol;
        vector<int>::const_iterator first = SeamTable.begin() + ((i-1)*(newWidth));
        vector<int>::const_iterator last = SeamTable.begin() + (i*(newWidth));
        vector<int> rowArray(first, last);
        minCol = getNextMinCol(rowArray, minCol);
    }
    
    seam[0] = minCol;
    
    return seam;
}
//--------------------------------------------------------------
int testApp::getNextMinCol(vector<int> const &rowArray, int col){
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
        /*
        if(rowArray[col-1] == rowArray[col] && rowArray[col] == rowArray[col+1] ){
            //if everything is the same choose randomly
            vector<int> choices;
            choices.push_back(col-1);
            choices.push_back(col);
            choices.push_back(col+1);
            int randChoice = floor(ofRandom(3));
            return choices[randChoice];
        }
        else */
        if((rowArray[col -1] < rowArray[col]) && (rowArray[col -1] < rowArray[col + 1])){
            return col -1;
        } else if((rowArray[col] < rowArray[col - 1]) && (rowArray[col] < rowArray[col+1])){
            return col;
        } else{
            return col + 1;
        }
    }
}

//--------------------------------------------------------------
int testApp::getPixelLoc(ofPixels const &ppixels, int row, int col, int srcWidth){
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

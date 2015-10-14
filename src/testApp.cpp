#include "testApp.h"



//--------------------------------------------------------------
void testApp::setup(){

    sobelShader.load("base.vert", "sobel.frag");
    energyShader.load("base.vert", "energy.frag");
    


    img.loadImage("faces/barraco.png");
    img.rotate90(0);
    

    cout<<ofToString(img.getPixelsRef().getNumChannels())<<endl;
    w = img.width;
    h = img.height;
    
    fbo.allocate(img.width, img.height, GL_RGBA);
    
    int maxSize = max(img.width, img.height);
    displayFbo.allocate(maxSize,maxSize, GL_RGBA);
    centerFbo.allocate(1920,1080, GL_RGBA);
    
    pixels.allocate(img.width, img.height, OF_PIXELS_RGBA);
    newImg.allocate(img.width, img.height, OF_IMAGE_COLOR_ALPHA);
    
    pixels = img.getPixelsRef();
    
    
    
    newImg.setUseTexture(true);
    
    ofSetWindowShape(img.width, img.width);
    
    
    ofSetBackgroundAuto(false);
    
    seamTable.resize(img.width * img.height);
    hSeamTable.resize(img.width * img.height);
    
    seam.resize(img.height);
    hSeam.resize(img.width);

    
    
    //cout<<ofToString(seamTable.size())<<endl;
    
    for (int i = 0; i<seamTable.size() - 1; i++) {
        seamTable[i] = 255;
        hSeamTable[i] = 255;
    }
    
    findAll = false;
    drawSeam = false;
    numSeamsRemoved = 1;
    numRowsRemoved = 1;
    
    numSeamsAdded = 1;
    numRowsAdded = 1;
    newWidth = img.width;
    newHeight = img.height;
    
    startingWidth = img.width;
    startingHeight = img.height;
    
    newImg.getTextureReference().setTextureMinMagFilter(GL_NEAREST, GL_NEAREST);
    fbo.getTextureReference().setTextureMinMagFilter(GL_NEAREST, GL_NEAREST);
    ofSetMinMagFilters(GL_NEAREST, GL_NEAREST);
    
    sideWays = false;
    hasTurned = false;
    
    
    doRemove = true;
    doAdd = false;
    
    
    
}

//--------------------------------------------------------------
void testApp::update(){
    ofSetWindowTitle(ofToString(ofGetFrameRate()));
    
    
        if(ofGetKeyPressed('s')){
        
        
        //doAdd = false;
        
        //ofPixels savePix;
        //savePix.allocate(newImg.width, newImg.height, OF_PIXELS_RGBA);
        ofSaveImage(newImg.getPixelsRef(), ofGetTimestampString()+".png");
        
        //doAdd = true;
    }
}



//--------------------------------------------------------------
void testApp::draw(){
    
    if(ofGetFrameNum() == 0){
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
            //findHorSeam();
        if(ofGetFrameNum() == 0){
            removeSeam(img.getTextureReference());
            //removeHSeam(img.getTextureReference());
        } else{
            removeSeam(newImg.getTextureReference());
            //removeHSeam(newImg.getTextureReference());
        }

        pixels = newImg.getPixelsRef();
    }
    
    if(ofGetFrameNum() <= img.width *2 - 500){
        doRemove = true;
    } else {
        doRemove = false;
        doAdd = true;
    }
    
    
    if(doRemove){
        flipAndSetupSizes();
        remove1Seam();
    }
    
    if(doAdd){
        flipAndSetupSizes();
        add1Seam();
    }
     
    ofClear(0);

    if(!sideWays){
    displayFbo.begin();
        ofClear(0);
            newImg.draw(0,0, displayFbo.getWidth(), displayFbo.getWidth());
    displayFbo.end();
    } else if(sideWays){
        displayFbo.begin();
        ofClear(0);
        ofPushMatrix();
            ofRotate(-90, 0, 0, 1);
            int ww = newImg.width;
        //cout<<ofToString(newImg.height)<<endl;
            //newImg.draw(-ww, 0);
            newImg.draw(-1080, 0, displayFbo.getWidth(), displayFbo.getWidth() );
        ofPopMatrix();
        displayFbo.end();
    }
    
   // if(!sideWays){
        //displayFbo.draw(0,0);
   /* } else if(sideWays){
        ofPushMatrix();
        ofRotate(-90, 0, 0, 1);
        displayFbo.draw(-newImg.width,0);
        ofPopMatrix();
    }*/
    
    centerFbo.begin();
        ofClear(0,0,0,255);
        //displayFbo.draw(displayFbo.getWidth()/2 - newImg.width/2 , displayFbo.getHeight()/2 - newImg.height/2);
       // displayFbo.draw(0,0, centerFbo.getWidth()+(centerFbo.getWidth()-newImg.width), centerFbo.getHeight()+ (centerFbo.getHeight()-newImg.height));
        displayFbo.draw(centerFbo.getWidth()/2 - displayFbo.getWidth()/2,0);
    centerFbo.end();
    
    centerFbo.draw(0,0);
    
    //newImg.draw(0,0);
    //newImg.draw(floor((numSeamsRemoved-1)*0.5), floor((numRowsRemoved -1)*0.5));
    //newImg.draw(0,floor((numRowsRemoved -1)*0.5));
    
    //fbo.draw(0,0);
    if(ofGetFrameNum() < 3324){
    ofPixels centerPix;
    centerFbo.readToPixels(centerPix);
    ofSaveImage(centerPix, "savedFrames/newObama/obama"+ofToString(ofGetFrameNum())+".png");
    }
    
}
//--------------------------------------------------------------
void testApp::removeSeam(ofTexture &srcTex){
    if(!sideWays){
        newWidth = startingWidth - numSeamsRemoved ;
        if(hasTurned ){
            startingHeight = img.height - numRowsRemoved +1;
        }
    } else {
        newWidth = startingWidth - numRowsRemoved ;
        startingHeight = img.width - numSeamsRemoved + 1;
    }

    ofPixels pixelsToRemove;

    pixelsToRemove.allocate(newImg.width, startingHeight, OF_PIXELS_RGBA);
    srcTex.readToPixels(pixelsToRemove);
    
    newPixels.allocate(newWidth, startingHeight, OF_PIXELS_RGBA);
    
    auto pixelsToRemovePtr = pixelsToRemove.getPixels();
    auto newPixelsPtr = newPixels.getPixels();
   
    
        vector<int> &curSeam = sideWays ? totalSeamsH[totalSeamsH.size()-1] : totalSeams[totalSeams.size()-1];
    

        for(int row = 0; row < startingHeight; row++){
            
            int colToRemove = curSeam[row];
            
            for(int col = 0; col < newImg.width; col++){
                

                int newPixelIndex = (row * newWidth + col)*4;
                int oldPixelIndex = (row * newImg.width + col)*4;
                
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
                
                
                /*
                if(col == colToRemove){
                    newPixelsPtr[newPixelIndex] = int(ofRandom(256));//pixelsToRemovePtr[oldPixelIndex];
                    newPixelsPtr[newPixelIndex+1] = int(ofRandom(256));//pixelsToRemovePtr[oldPixelIndex+1];
                    newPixelsPtr[newPixelIndex+2] = int(ofRandom(256));//pixelsToRemovePtr[oldPixelIndex+2];
                    newPixelsPtr[newPixelIndex+3] = 255;//pixelsToRemovePtr[oldPixelIndex+3];
                } else{
                    newPixelsPtr[newPixelIndex] = pixelsToRemovePtr[oldPixelIndex];
                    newPixelsPtr[newPixelIndex+1] = pixelsToRemovePtr[oldPixelIndex+1];
                    newPixelsPtr[newPixelIndex+2] = pixelsToRemovePtr[oldPixelIndex+2];
                    newPixelsPtr[newPixelIndex+3] = pixelsToRemovePtr[oldPixelIndex+3];
                }
                */
                
            }
        }

    

    newImg.allocate(newWidth, startingHeight, OF_IMAGE_COLOR_ALPHA);
    //newImg.allocate(img.width, img.height, OF_IMAGE_COLOR_ALPHA);
    newImg.setFromPixels(newPixels);
    if(!sideWays){
        numSeamsRemoved ++;
    } else{
        numRowsRemoved ++;
    }

}
// int seamIndex = 0;
//--------------------------------------------------------------
void testApp::addSeam(ofTexture &srcTex){
    
    
    if(!sideWays){
        if (hasTurned) {
            startingHeight = newImg.height;//img.height - numRowsRemoved;
        } else{
            startingHeight = img.height;
        }
        
    } else{
        startingHeight = newImg.height;//img.width - numSeamsRemoved;
        //cout<<ofToString(startingHeight)<<endl;
    }


    
    fbo.begin();
        srcTex.draw(0, 0);
    fbo.end();
    
    int newOldWidth = newImg.width;
    newWidth = newImg.width + 1 ;
    
    ofPixels pixelsToRemove;
    pixelsToRemove.allocate(newOldWidth, startingHeight, 4);
    fbo.readToPixels(pixelsToRemove);
    
    
    
    //ofPixels newPixels;
    newPixels.allocate(newWidth, startingHeight, OF_PIXELS_RGBA);

    
    auto pixelsToRemovePtr = pixelsToRemove.getPixels();
    auto newPixelsPtr = newPixels.getPixels();
   
    

    int seamIndex = sideWays ? totalSeamsH.size() - numRowsAdded : totalSeams.size() - numSeamsAdded;
    if(seamIndex<0 && !sideWays){
        seamIndex+=totalSeams.size();
        numSeamsAdded = 1;
    } else if(seamIndex<0 && sideWays){
        seamIndex+=totalSeamsH.size();
        numRowsAdded = 1;
    }
    //seamIndex = (seamIndex +1 ) % totalSeams.size();
    
        vector<int> &curSeam = sideWays ? totalSeamsH[seamIndex] : totalSeams[seamIndex];
        //cout<<ofToString(seamIndex)<<endl;

        for(int row = 0; row < startingHeight; row++){
            
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
                
                
                if(col <= colToRemove){
                    newPixelsPtr[newPixelIndex] = pixelsToRemovePtr[oldPixelIndex];
                    newPixelsPtr[newPixelIndex+1] = pixelsToRemovePtr[oldPixelIndex+1];
                    newPixelsPtr[newPixelIndex+2] = pixelsToRemovePtr[oldPixelIndex+2];
                    newPixelsPtr[newPixelIndex+3] = 255;//pixelsToRemovePtr[oldPixelIndex+3];
                    
                    
                } else if (col > colToRemove){
                    newPixelIndex += 4;

                    newPixelsPtr[newPixelIndex] = pixelsToRemovePtr[oldPixelIndex];
                    newPixelsPtr[newPixelIndex+1] = pixelsToRemovePtr[oldPixelIndex+1];
                    newPixelsPtr[newPixelIndex+2] = pixelsToRemovePtr[oldPixelIndex+2];
                    newPixelsPtr[newPixelIndex+3] = pixelsToRemovePtr[oldPixelIndex+3];
                }
                
                /*
                if(col == colToRemove ){
                    //newPixelIndex +=4;
                    newPixelsPtr[newPixelIndex] = 255;//(pixelsToRemovePtr[oldPixelIndex] + pixelsToRemovePtr[oldPixelIndex+8])/2;
                    newPixelsPtr[newPixelIndex+1] = 0;//(pixelsToRemovePtr[oldPixelIndex+1] + pixelsToRemovePtr[oldPixelIndex+9])/2;
                    newPixelsPtr[newPixelIndex+2] = 0;//(pixelsToRemovePtr[oldPixelIndex+2] + pixelsToRemovePtr[oldPixelIndex+10] )/2;
                    newPixelsPtr[newPixelIndex+3] = 255;//(pixelsToRemovePtr[oldPixelIndex+3] + pixelsToRemovePtr[oldPixelIndex+11])/2;
                }
                
                if(col == colToRemove+1 ){
                    //newPixelIndex +=4;
                    newPixelsPtr[newPixelIndex] = 0;//(pixelsToRemovePtr[oldPixelIndex] + pixelsToRemovePtr[oldPixelIndex+8])/2;
                    newPixelsPtr[newPixelIndex+1] = 255;//(pixelsToRemovePtr[oldPixelIndex+1] + pixelsToRemovePtr[oldPixelIndex+9])/2;
                    newPixelsPtr[newPixelIndex+2] = 0;//(pixelsToRemovePtr[oldPixelIndex+2] + pixelsToRemovePtr[oldPixelIndex+10] )/2;
                    newPixelsPtr[newPixelIndex+3] = 255;//(pixelsToRemovePtr[oldPixelIndex+3] + pixelsToRemovePtr[oldPixelIndex+11])/2;
                }
                
                if(col == colToRemove-1 ){
                    //newPixelIndex +=4;
                    newPixelsPtr[newPixelIndex] = 0;//(pixelsToRemovePtr[oldPixelIndex] + pixelsToRemovePtr[oldPixelIndex+8])/2;
                    newPixelsPtr[newPixelIndex+1] = 0;//(pixelsToRemovePtr[oldPixelIndex+1] + pixelsToRemovePtr[oldPixelIndex+9])/2;
                    newPixelsPtr[newPixelIndex+2] = 255;//(pixelsToRemovePtr[oldPixelIndex+2] + pixelsToRemovePtr[oldPixelIndex+10] )/2;
                    newPixelsPtr[newPixelIndex+3] = 255;//(pixelsToRemovePtr[oldPixelIndex+3] + pixelsToRemovePtr[oldPixelIndex+11])/2;
                }
                */
                if(col == colToRemove ){
                    newPixelIndex +=4;
                    //int randPos = int(ofRandom(6));
                    //int fours [] = {4, 8, 12, 16, 20, 24};
                    
                    newPixelsPtr[newPixelIndex] = 0;//(pixelsToRemovePtr[oldPixelIndex] + pixelsToRemovePtr[oldPixelIndex+4])/2;
                    newPixelsPtr[newPixelIndex+1] = 0;//(pixelsToRemovePtr[oldPixelIndex+1] + pixelsToRemovePtr[oldPixelIndex+5])/2;
                    newPixelsPtr[newPixelIndex+2] = 0;//(pixelsToRemovePtr[oldPixelIndex+2] + pixelsToRemovePtr[oldPixelIndex+6] )/2;
                    newPixelsPtr[newPixelIndex+3] = 255;//(pixelsToRemovePtr[oldPixelIndex+3] + pixelsToRemovePtr[oldPixelIndex+7])/2;
                }
                
            }
        }
    
    
    newImg.allocate(newWidth, startingHeight, OF_IMAGE_COLOR_ALPHA);
    newImg.setFromPixels(newPixels);
    if(!sideWays){
        numSeamsRemoved --;
        numSeamsAdded++;
    } else if(sideWays){
        numRowsRemoved --;
        numRowsAdded ++;
        
    }
    
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
    
    if(!sideWays){
        if (hasTurned) {
            startingHeight = newImg.height;// img.height - numRowsRemoved;
            seamSize = newImg.height;//img.height - numRowsRemoved;
            
        } else{
            startingHeight = img.height;
            seamSize = img.height;
        }
        
    } else if(sideWays){
        startingHeight = newImg.height;//img.width - numSeamsRemoved;
        //cout<<ofToString(startingHeight)<<endl;
        seamSize = newImg.height;
    }

    //fill the last row
    for(int y = 0; y < newWidth; y++){
            seamTable[y] = fboPix[y*4];//getPixelLoc(pixels, 0, y, img.width);
    }
    

    for(int i = 1; i< startingHeight; i++){
        for(int j = 0; j< newImg.width; j++){
            int rowOffset = (i - 1) * newImg.width;
            //int tl = seamTable[rowOffset + j -1];
            int t = seamTable[rowOffset + j];
            //int tr = seamTable[rowOffset + j + 1];
            
            int loc = (i * newImg.width + j )*4;
            int seamLoc = ( i * newImg.width + j );
            
            if(j == 0){ //if left edge
                seamTable[seamLoc] = fboPix[loc] + fastMin(t, seamTable[rowOffset + j + 1]);
            } else if( j == (newImg.width -1)){ //or right edge
                seamTable[seamLoc] = fboPix[loc] ;//
                seamTable[seamLoc] += fastMin(seamTable[rowOffset + j -1], t);
            } else{ //not on the edges

                seamTable[seamLoc] = fboPix[loc];
                seamTable[seamLoc] += fastMin(fastMin(seamTable[rowOffset + j -1], t), seamTable[rowOffset + j + 1]);
                
            }
        }
    }
    
    
    int minCol = 0;
    
    for(int j = 0; j<newWidth; j++){
        if( getPixelLoc(pixels, newImg.height -1, j, newWidth) < getPixelLoc(pixels, newImg.height -1 , minCol, newWidth)){
            minCol = j;
            //cout<<ofToString(int(pixels[minCol]))<<endl;
        }
    }
    
    vector<int> foundSeam(seamSize);
  
        //cout<<"Min seam = "+ofToString(int(pixels[minCol]))<<endl;
        foundSeam = findMinSeam(seamTable, minCol, newWidth, newImg.height);

    if(!sideWays){
        totalSeams.push_back(foundSeam);
    } else if(sideWays){
        totalSeamsH.push_back(foundSeam);
    }
    

}
//--------------------------------------------------------------
vector<int> testApp::findMinSeamH(vector<int> const &SeamTable, int minCol, int width, int height){

    //seam.clear();
    
    for(int i = img.width -1; i> 0; i--){
        hSeam[i] = minCol;
        vector<int>::const_iterator first = SeamTable.begin() + ((i-1)*(newImg.height));
        vector<int>::const_iterator last = SeamTable.begin() + (i*(newImg.height));
        vector<int> rowArray(first, last);
        minCol = getNextMinColH(rowArray, minCol);
        //cout<<ofToString(rowArray[minCol])<<endl;
    }
    
    hSeam[0] = minCol;
    
    return hSeam;
}

//--------------------------------------------------------------
vector<int> testApp::findMinSeam(vector<int> const &SeamTable, int minCol, int width, int height){
    //seam.clear();
    
    for(int i = newImg.height -1; i> 0; i--){
        seam[i] = minCol;
        vector<int>::const_iterator first = SeamTable.begin() + ((i-1)*(newImg.width));
        vector<int>::const_iterator last = SeamTable.begin() + (i*(newImg.width));
        vector<int> rowArray(first, last);
        minCol = getNextMinCol(rowArray, minCol);
        //cout<<ofToString(rowArray[minCol])<<endl;
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



//--------------------------------------------------------------


int testApp::getNextMinColH(vector<int> const &rowArray, int col){
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
    if(key == 'a'){
        fbo.allocate(newImg.width, newImg.height, GL_RGBA);


        fbo.begin();
            ofClear(0);
        fbo.end();
        
        //getEnergyMap(newImg.getTextureReference());
        
        
        ////////////sloooow
        //fbo.readToPixels(pixels);
        ////////////
        
        
        //findSeam();
        
        
  
        addSeam(newImg.getTextureReference());
    
        
        newImg.draw(0,0);
        
        
        pixels = newImg.getPixelsRef();

    }
    
    if(key == 'r'){
        fbo.allocate(newImg.width, newImg.height, GL_RGBA);


        fbo.begin();
            ofClear(0);
        fbo.end();
        
        getEnergyMap(newImg.getTextureReference());
        
        
        ////////////sloooow
        fbo.readToPixels(pixels);
        ////////////
        
        
        findSeam();
        
        
  
        removeSeam(newImg.getTextureReference());
    
        
        //newImg.draw(0,0);
        
        
        pixels = newImg.getPixelsRef();

    }
    
    if(key == 't'){
        hasTurned = true;
        sideWays = !sideWays;
        if(sideWays){
            newImg.rotate90(1);
        } else if(!sideWays){
            newImg.rotate90(-1);
        }
        
        //cout<<ofToString(newImg.width)<<endl;
        seam.resize(newImg.height);
        fbo.allocate(newImg.width, newImg.height, GL_RGBA);
        pixels.allocate(newImg.width, newImg.height, OF_PIXELS_RGBA);
        //newWidth = newImg.width;
        //newHeight = newImg.height;
        if(sideWays){
            startingWidth = img.height;
            startingHeight = img.width;
        } else{
            startingWidth = img.width;
            startingHeight = img.height;
        }
        
        pixels = newImg.getPixelsRef();
    }
    
    if (key == 'd') {
        flipAndSetupSizes();
        //remove1Seam();
        remove1Seam();
    }
    
    if (key == 'f') {
        flipAndSetupSizes();
        //remove1Seam();
        add1Seam();
    }
    
    if(key == 'q'){
        doRemove = !doRemove;
        doAdd = !doAdd;
    }

}
//--------------------------------------------------------------
void testApp::remove1Seam(){
        fbo.allocate(newImg.width, newImg.height, GL_RGBA);


        fbo.begin();
            ofClear(0);
        fbo.end();
        
        getEnergyMap(newImg.getTextureReference());
        
        
        ////////////sloooow
        fbo.readToPixels(pixels);
        ////////////
        
        
        findSeam();
        
        
  
        removeSeam(newImg.getTextureReference());
    
        
        //newImg.draw(0,0);
        
        
        pixels = newImg.getPixelsRef();
}
//--------------------------------------------------------------
void testApp::flipAndSetupSizes(){
        hasTurned = true;
        sideWays = !sideWays;
        if(sideWays){
            newImg.rotate90(1);
        } else if(!sideWays){
            newImg.rotate90(-1);
        }
        
        //cout<<ofToString(newImg.width)<<endl;
        seam.resize(newImg.height);
        fbo.allocate(newImg.width, newImg.height, GL_RGBA);
        pixels.allocate(newImg.width, newImg.height, OF_PIXELS_RGBA);
        //newWidth = newImg.width;
        //newHeight = newImg.height;
        if(sideWays){
            startingWidth = img.height;
            startingHeight = img.width;
        } else{
            startingWidth = img.width;
            startingHeight = img.height;
        }
        
        pixels = newImg.getPixelsRef();
}
//--------------------------------------------------------------
void testApp::add1Seam(){
            fbo.allocate(newImg.width, newImg.height, GL_RGBA);


        fbo.begin();
            ofClear(0);
        fbo.end();
        
        //getEnergyMap(newImg.getTextureReference());
        
        
        ////////////sloooow
        //fbo.readToPixels(pixels);
        ////////////
        
        
        //findSeam();
        
        
  
        addSeam(newImg.getTextureReference());
    
        
        newImg.draw(0,0);
        
        
        pixels = newImg.getPixelsRef();
    
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

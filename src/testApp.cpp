#include "testApp.h"



//--------------------------------------------------------------
void testApp::setup(){

    sobelShader.load("base.vert", "sobel.frag");
    energyShader.load("base.vert", "energy.frag");
    


    img.loadImage("forest2.png");
    img.rotate90(0);
    

    cout<<ofToString(img.getPixelsRef().getNumChannels())<<endl;
    w = img.width;
    h = img.height;
    
    fbo.allocate(img.width, img.height, GL_RGBA);
    
    int maxSize = max(img.width, img.height);
    displayFbo.allocate(maxSize,maxSize, GL_RGBA);
    
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
    newWidth = img.width;
    newHeight = img.height;
    
    startingWidth = img.width;
    startingHeight = img.height;
    
    newImg.getTextureReference().setTextureMinMagFilter(GL_NEAREST, GL_NEAREST);
    fbo.getTextureReference().setTextureMinMagFilter(GL_NEAREST, GL_NEAREST);
    ofSetMinMagFilters(GL_NEAREST, GL_NEAREST);
    
    sideWays = false;
    hasTurned = false;
    
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
    
    /*
    if(ofGetFrameNum() % (img.width - 100) == 0){
        
        //fbo.allocate(img.width, img.height, GL_RGBA);
        
     
        newImg.getTextureReference().setTextureMinMagFilter(GL_NEAREST, GL_NEAREST);
        fbo.getTextureReference().setTextureMinMagFilter(GL_NEAREST, GL_NEAREST);
        ofSetMinMagFilters(GL_NEAREST, GL_NEAREST);
        img.getTextureReference().setTextureMinMagFilter(GL_NEAREST, GL_NEAREST);
     
        
        //fbo.begin();
        //    newImg.draw(0,0,img.width, img.height);
        //fbo.end();
        
        
        //fbo.readToPixels(pixels);
        //newImg.allocate(img.width, img.height, OF_IMAGE_COLOR_ALPHA);


        //newImg.setFromPixels(pixels);
        //numSeamsRemoved = 1;
         
    }
     */
    
    if(ofGetFrameNum() % 2 == 0 && ofGetFrameNum() >= 4){
        //newImg.rotate90(1);
        //img.rotate90(1);
        /*
        cout<<ofToString(newImg.width)<<endl;
        newHeight = newImg.height;
        newWidth  = newImg.width;
        pixels.allocate(newImg.width, newImg.height, OF_PIXELS_RGBA);
        int temp = startingWidth;
        startingWidth = startingHeight;
        startingHeight = temp;
        */
    } else{
        
        //newImg.rotate90(-1);
        //img.rotate90(-1);
        /*
        newHeight = newImg.height;
        newWidth  = newImg.width;
        pixels.allocate(newImg.width, newImg.height, OF_PIXELS_RGBA);
        
        startingHeight = img.width;
        startingWidth = img.height;
         */
    }
    
    
    if(ofGetFrameNum() <=200 && numSeamsRemoved < img.width-1){
        //fbo.allocate(newImg.width, newImg.height, GL_RGBA);

        //newImg.setFromPixels(pixels);
        //numSeamsRemoved = 1;
         
    }
    
    /*
    if(ofGetFrameNum() % 2 == 0 && ofGetFrameNum() >= 4){
        newImg.rotate90(1);
        cout<<ofToString(newImg.width)<<endl;
        newHeight = newImg.height;
        newWidth  = newImg.width;
        pixels.allocate(newImg.width, newImg.height, OF_PIXELS_RGBA);
        int temp = startingWidth;
        startingWidth = startingHeight;
        startingHeight = temp;
        
    } else{
        newImg.rotate90(-1);
        newHeight = newImg.height;
        newWidth  = newImg.width;
        pixels.allocate(newImg.width, newImg.height, OF_PIXELS_RGBA);
        
        startingHeight = img.width;
        startingWidth = img.height;
    }
    */
    
    if(ofGetFrameNum() <= 100){
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

        

        /*
        if(ofGetFrameNum() % 2 == 0){
            findSeam();
            //findHorSeam();
        } else{
            //findHorSeam();
            findSeam();
        }
        
        if(ofGetFrameNum() == 0){
            if(ofGetFrameNum() %2 == 0){
                removeSeam(img.getTextureReference());
                //removeHSeam(img.getTextureReference());
            } else{
                //removeHSeam(img.getTextureReference());
                removeSeam(img.getTextureReference());
            }
        } else{
            if(ofGetFrameNum() %2 == 0){
                removeSeam(newImg.getTextureReference());
                //removeHSeam(newImg.getTextureReference());
            } else{
                //removeHSeam(newImg.getTextureReference());
                removeSeam(newImg.getTextureReference());
            }
        }
         */
        
        
     
        
        
        

        pixels = newImg.getPixelsRef();
    } //else if(ofGetFrameNum() < min(img.width, img.height)-100){
//        flipAndSetupSizes();
//        remove1Seam();
//    }
     
    ofClear(0);

/*
    fbo.begin();
    ofClear(0);
    if(ofGetFrameNum()%1 == 0){
        //ofPushMatrix();
            //ofTranslate(ofGetWidth()/2, ofGetHeight()/2);
            //ofRotate(-90, 0, 0, 1);
            //newImg.draw(-ofGetWidth()/2,-ofGetHeight()/2);
        //ofPopMatrix();
        newImg.draw(0,0);
 
    } else{

        newImg.draw(0,0);
        
    }
    fbo.end();

    */
    /*
        ofPushMatrix();
            ofTranslate(ofGetWidth()/2, ofGetHeight()/2);
            ofRotate(-90, 0, 0, 1);
            newImg.draw(-ofGetWidth()/2,-ofGetHeight()/2);
        ofPopMatrix();
    */
        //ofSaveImage(pixels, ofGetTimestampString()+".png");
        
    //}
    
    displayFbo.begin();
        ofClear(0);
            newImg.draw(0,0);
    displayFbo.end();
    
        //ofTranslate(newImg.width/2, newImg.height/2);
    if(!sideWays){
        displayFbo.draw(0,0);
    } else if(sideWays){
        //ofTranslate(-newImg.width/2, -newImg.height/2);
        ofPushMatrix();
        ofRotate(-90, 0, 0, 1);
        displayFbo.draw(-newImg.width,0);
        ofPopMatrix();
    }
    //newImg.draw(0,0);
    //newImg.draw(floor((numSeamsRemoved-1)*0.5), floor((numRowsRemoved -1)*0.5));
    //newImg.draw(0,floor((numRowsRemoved -1)*0.5));
    
}
//--------------------------------------------------------------
void testApp::removeHSeam(ofTexture &srcTex){
    
    newHeight = startingWidth - numRowsRemoved;
    
    ofPixels pixelsToRemove;
    pixelsToRemove.allocate(newImg.width, newImg.height, OF_PIXELS_RGBA);
    srcTex.readToPixels(pixelsToRemove);
    
    newPixels.allocate(newImg.width, newImg.height, OF_PIXELS_RGBA);
    
    auto pixelsToRemovePtr = pixelsToRemove.getPixels();
    auto newPixelsPtr = newPixels.getPixels();
    
    vector<int> &curSeam = totalSeams[totalSeams.size() -1];
    
    
    
    for(int row = 0; row < newImg.width; row++){
        int rowToRemove = curSeam[row];
        for(int col = 0; col < newImg.height; col++){
            
            
            int newPixelIndex = (col * newImg.width + row)*4;
            int oldPixelIndex = (col * newImg.width + row)*4;
            
            
            if(col  > rowToRemove){
                newPixelsPtr[newPixelIndex] = pixelsToRemovePtr[oldPixelIndex];
                newPixelsPtr[newPixelIndex+1] = pixelsToRemovePtr[oldPixelIndex+1];
                newPixelsPtr[newPixelIndex+2] = pixelsToRemovePtr[oldPixelIndex+2];
                newPixelsPtr[newPixelIndex+3] = pixelsToRemovePtr[oldPixelIndex+3];
                
            } /*else if(col > rowToRemove){
                newPixelIndex -= 4*newImg.width;
                newPixelsPtr[newPixelIndex] = pixelsToRemovePtr[oldPixelIndex];
                newPixelsPtr[newPixelIndex+1] = pixelsToRemovePtr[oldPixelIndex+1];
                newPixelsPtr[newPixelIndex+2] = pixelsToRemovePtr[oldPixelIndex+2];
                newPixelsPtr[newPixelIndex+3] = pixelsToRemovePtr[oldPixelIndex+3];
            }
            */
            
            /*
            if(col == rowToRemove){
                newPixelsPtr[newPixelIndex] = 255;//pixelsToRemovePtr[oldPixelIndex];
                newPixelsPtr[newPixelIndex+1] = 255;//pixelsToRemovePtr[oldPixelIndex+1];
                newPixelsPtr[newPixelIndex+2] = 255;//pixelsToRemovePtr[oldPixelIndex+2];
                newPixelsPtr[newPixelIndex+3] = 255;//pixelsToRemovePtr[oldPixelIndex+3];
            } else if(col != rowToRemove){
                newPixelsPtr[newPixelIndex] = pixelsToRemovePtr[oldPixelIndex];
                newPixelsPtr[newPixelIndex+1] = pixelsToRemovePtr[oldPixelIndex+1];
                newPixelsPtr[newPixelIndex+2] = pixelsToRemovePtr[oldPixelIndex+2];
                newPixelsPtr[newPixelIndex+3] = pixelsToRemovePtr[oldPixelIndex+3];
            }
            */
        }
    }
    
    newImg.allocate(newImg.width, newImg.height, OF_IMAGE_COLOR_ALPHA);
    newImg.setFromPixels(newPixels);
    numRowsRemoved++;
    
    
    //newImg.draw(floor((numSeamsRemoved-1)*0.5), floor((numRowsRemoved -1)*0.5));
    //newImg.draw(0,floor((numRowsRemoved -1)*0.5));
    
}

//--------------------------------------------------------------
void testApp::removeSeam(ofTexture &srcTex){
    if(!sideWays){
        newWidth = startingWidth - numSeamsRemoved ;
        if(hasTurned ){
            startingHeight = img.height - numRowsRemoved +1;
        }
    } else{
        newWidth = startingWidth - numRowsRemoved ;
        startingHeight = img.width - numSeamsRemoved + 1;
    }

    ofPixels pixelsToRemove;

    pixelsToRemove.allocate(newImg.width, startingHeight, OF_PIXELS_RGBA);
    srcTex.readToPixels(pixelsToRemove);
    
    newPixels.allocate(newWidth, startingHeight, OF_PIXELS_RGBA);
    
    auto pixelsToRemovePtr = pixelsToRemove.getPixels();
    auto newPixelsPtr = newPixels.getPixels();
   
    
    //for(int numSeams = 0; numSeams< totalSeams.size(); numSeams++){
        vector<int> &curSeam = totalSeams[totalSeams.size()-1];
        

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
    //}
    

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
    


    
    fbo.begin();
        srcTex.draw(0, 0);
    fbo.end();
    
    int newOldWidth = newImg.width;
    newWidth = newImg.width + 1 ;
    
    ofPixels pixelsToRemove;
    pixelsToRemove.allocate(newOldWidth, img.height, 4);
    fbo.readToPixels(pixelsToRemove);
    
    
    
    //ofPixels newPixels;
    newPixels.allocate(newWidth, h, OF_PIXELS_RGBA);

    
    auto pixelsToRemovePtr = pixelsToRemove.getPixels();
    auto newPixelsPtr = newPixels.getPixels();
   
    
    //for(int numSeams = 0; numSeams< totalSeams.size(); numSeams++){

   int seamIndex = totalSeams.size() - numSeamsAdded;
    if(seamIndex<0){
        seamIndex+=totalSeams.size();
        numSeamsAdded = 1;
    }
    //seamIndex = (seamIndex +1 ) % totalSeams.size();
    
        vector<int> &curSeam = totalSeams[seamIndex];
        //cout<<ofToString(seamIndex)<<endl;

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
                    
                    newPixelsPtr[newPixelIndex] = (pixelsToRemovePtr[oldPixelIndex] + pixelsToRemovePtr[oldPixelIndex+4])/2;
                    newPixelsPtr[newPixelIndex+1] = (pixelsToRemovePtr[oldPixelIndex+1] + pixelsToRemovePtr[oldPixelIndex+5])/2;
                    newPixelsPtr[newPixelIndex+2] = (pixelsToRemovePtr[oldPixelIndex+2] + pixelsToRemovePtr[oldPixelIndex+6] )/2;
                    newPixelsPtr[newPixelIndex+3] = 255;//(pixelsToRemovePtr[oldPixelIndex+3] + pixelsToRemovePtr[oldPixelIndex+7])/2;
                }
                
            }
        }
    //}
    
    newImg.allocate(newWidth, h, OF_IMAGE_COLOR_ALPHA);
    newImg.setFromPixels(newPixels);
    numSeamsRemoved --;
    numSeamsAdded++;
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
void testApp::findHorSeam(){
    unsigned char *fboPix = pixels.getPixels();
    
    for(int x = 0; x<newImg.height; x++){
        hSeamTable[x*newImg.width] = fboPix[x*newImg.width*4];
    }
    

    
    for(int y = 0; y<newImg.height; y++){
       for(int x = 1; x< newWidth; x++){
            
            //int r = hSeamTable[colOffset + 1];
            
            int loc = ( y * newWidth + x)*4;
            int seamLoc = (y * newWidth + x);
            int colOffset = seamLoc - 1;
            
            
            if(y == 0){
                hSeamTable[seamLoc] = fboPix[loc] + fastMin(hSeamTable[colOffset], hSeamTable[colOffset + newImg.width]);
            } else if(y == (newImg.height -1)){
                hSeamTable[seamLoc] = fboPix[loc];
                hSeamTable[seamLoc] += fastMin(hSeamTable[colOffset - newImg.width], hSeamTable[colOffset]);
            } else{
                hSeamTable[seamLoc] = fboPix[loc];
                hSeamTable[seamLoc] += fastMin(fastMin(hSeamTable[colOffset - newImg.width], hSeamTable[colOffset]), hSeamTable[colOffset + newImg.width]);
            }
        }
    }
    

    
    int minRow = 0;
    //vector<int> zeros;
    
    for(int x = 0; x< newImg.height; x++){
        //zeros.push_back(x);
        if(getPixelLoc(pixels, x, newImg.width -1, newImg.width) < getPixelLoc(pixels, minRow, newImg.width-1, newImg.width) ){
            minRow = x;
            //cout<<ofToString(int(pixels[minRow]))<<endl;
        }
    }
    
    
    
    vector<int> foundSeam(startingWidth);
    foundSeam = findMinSeamH(hSeamTable, minRow, newImg.width, newImg.height);
    totalSeams.push_back(foundSeam);
}
//--------------------------------------------------------------

void testApp::findSeam(){
    
    unsigned char *fboPix = pixels.getPixels();
    
    //cout<<ofToString(pixels.getNumChannels())<<endl;
    if(!sideWays){
        if (hasTurned) {
            startingHeight = img.height - numRowsRemoved;
        } else{
            startingHeight = img.height;
        }
        
    } else{
        startingHeight = newImg.width;//img.width - numSeamsRemoved;
        //cout<<ofToString(startingHeight)<<endl;
    }

    //fill the last row
    for(int y = 0; y < newWidth; y++){
            seamTable[y] = fboPix[y*4];//getPixelLoc(pixels, 0, y, img.width);
    }
    
    if(!sideWays){
        
    }
    

    for(int i = 1; i< startingHeight; i++){
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

                seamTable[seamLoc] = fboPix[loc];
                seamTable[seamLoc] += fastMin(fastMin(seamTable[rowOffset + j -1], t), seamTable[rowOffset + j + 1]);
                
            }
        }
    }
    
    
    int minCol = 0;
    vector<int> zeros;
    
    for(int j = 0; j<newWidth; j++){
            zeros.push_back(j);
        //if( pixels[( (newImg.height-1) * newWidth + j)*4] < pixels[( (newImg.height-1) *newWidth + minCol)*4]){
        if( getPixelLoc(pixels, newImg.height -1, j, newWidth) < getPixelLoc(pixels, newImg.height -1 , minCol, newWidth)){
            minCol = j;
            //cout<<ofToString(int(pixels[minCol]))<<endl;
        }
    }
    
    //vector<vector<int> > TOTALSEAMS;
    vector<int> foundSeam(h);
    
    if(findAll){
        for(int i = 0; i<zeros.size(); i++){
            foundSeam = findMinSeam(seamTable, zeros[i], newWidth, newImg.height);
            totalSeams.push_back(foundSeam);
        }
    } else{
        //cout<<"Min seam = "+ofToString(int(pixels[minCol]))<<endl;
        foundSeam = findMinSeam(seamTable, minCol, newWidth, newImg.height);


        totalSeams.push_back(foundSeam);
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

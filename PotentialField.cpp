//
//  PotentialField.cpp
//  EmoFlux
//
//  Created by GILL PARK on 11/5/14.
//
//

#include "PotentialField.h"


PotentialField::PotentialField(){
}
PotentialField::PotentialField(int res, int w, int h){
    resolution = res;
    width = w;
    height = h;
    testID =-1;
    pDebug = false;
    //cout <<"cells size : "<<cells.size()<<"\n";
}
PotentialField::~PotentialField(){}

void PotentialField::setup(){
    
    cols = width/resolution;
    rows = height/resolution;
    int numCells = cols * rows;

    
    for(int x=0; x < cols; x++){
        for(int y=0; y < rows; y++){
            int id = y * cols +x;
            shared_ptr<Tile> tempCell(new Tile(x,y));
            shared_ptr<Tile> Cell(new Tile(x,y));
            tempCell->size = resolution;
            tempCell->id = id;
            temp_cells.push_back(tempCell);
            
            Cell->size = resolution;
            Cell->id = id;
            Cell->direction = ofVec2f(0,0);
            cells.push_back(Cell);
        }
    }
    
    cout<<"cell size : "<<cells.size()<<" temp_cell_size : "<<temp_cells.size()<<"\n";
    
}
void PotentialField::update(){
    //update particles

    
    for(int j = cells.size()-1; j >= 0; j--){
        if(cells[j]->particles.size()<20)
        cells[j]->update();
        for(int i= cells[j]->particles.size()-1; i>=0 ; i--){
            
            cells[j]->particles[i]->run();
            
            float X = int(ofClamp(cells[j]->particles[i]->location.x/resolution,0,cols-1));
            float Y = int(ofClamp(cells[j]->particles[i]->location.y/resolution,0,rows-1));
            int lookup = Y * cols + X;
            ofVec2f Current_loc = cells[lookup]->direction;
            cells[j]->particles[i]->follow(Current_loc);
            
            if(cells[j]->particles[i]->dead){
                cells[j]->particles.erase(cells[j]->particles.begin()+i);
                
            }
        }
        //calculateVecs(j);
    }
    
}
void PotentialField::mReleased(){
    
    
    column = int(ofClamp(ofGetMouseX()/resolution,0,cols-1));
    row = int(ofClamp(ofGetMouseY()/resolution,0,rows-1));
    int test_cell =  row * cols +column;
    
    int ran = ofRandom(-10,10);
    temp_cells[test_cell]->cost=ran; //add val into cell
    temp_cells[test_cell]->isGoal = true;
    testID = test_cell;
    
    calculateField(testID); //start calculate field
    
    for(int i=0; i < cells.size(); i++){
        if(cells[i]->cost==0 && !temp_cells[i]->isPassible) //
            cells[i]->cost = temp_cells[i]->cost;
        cells[i]->isPassible = temp_cells[i]->isPassible;
        cells[i]->isGoal = temp_cells[i]->isGoal;///
        cells[i]->direction = temp_cells[i]->direction;///
  
        if(cells[i]->cost!=0&&!cells[i]->isPassible&&!temp_cells[i]->isPassible){
            cells[i]->cost =(cells[i]->cost+ temp_cells[i]->cost)*0.5;
           //TODO: VETOR SUM??
        }
        
        temp_cells[i]->reset_val();
    }
    for(int i=0; i<cells.size(); i++)calculateVecs(i);
    
    
}

void PotentialField::calculateVecs(int _id){
    
    int _x = _id%cols;
    int _y = _id/cols;
    int _column = int(ofClamp(_x,0,cols-1));
    int _row = int(ofClamp(_y,0,rows-1));
    
    int N,E,S,W;
    N = (_row-1) * cols + _column;
    S = (_row+1) * cols + _column;
    E = _row * cols + _column+1;
    W = _row * cols + _column-1;
    
    float vecX,vecY;
    if(_x<=0){vecX = cells[E]->cost - 0;}
    if(_x>=cols-1){vecX = 0 - cells[W]->cost;}
    if(_y <= 0){vecY = cells[S]->cost - 0;}
    if(_y >= rows-1){vecY = 0 - cells[N]->cost;}
    if(_x>0&&_x<cols-1){
        
        vecX = cells[_id]->direction.x + (cells[E]->cost - cells[W]->cost);
    }
    if(_y>0&&_y<rows-1){
        vecY = cells[_id]->direction.y + (cells[S]->cost - cells[N]->cost);
    }
    
    cells[_id]->direction.set(vecX,vecY);
}

void PotentialField::findNeighbors(int _x, int _y){
    int _column = int(ofClamp(_x,0,cols-1));
    int _row = int(ofClamp(_y,0,rows-1));
    int testingID = _row * cols + _column;
    
    int N,E,S,W;
    N = (_row-1) * cols + _column;
    E = _row * cols + _column+1;
    S = (_row+1) * cols + _column;
    W = _row * cols + _column-1;
    
    
    if(temp_cells[N]->isPassible){
        if(temp_cells[testingID]->cost>0){ //when the input val is positive
            temp_cells[N]->cost = temp_cells[testingID]->cost-1;
            temp_cells[N]->direction = ofVec2f(-1.5,0);

        }
        if(temp_cells[testingID]->cost<0){ //when the input val is negative
            temp_cells[N]->cost = temp_cells[testingID]->cost+1;
            temp_cells[N]->direction = ofVec2f(1.5,0);
        }
       
        testList.push_back(N);
    }
    
    if(temp_cells[E]->isPassible){
        if(temp_cells[testingID]->cost>0){ //when the input val is positive
            temp_cells[E]->cost = temp_cells[testingID]->cost-1;
            temp_cells[E]->direction = ofVec2f(0,-1.5);
        }
        if(temp_cells[testingID]->cost<0){ //when the input val is negative
            temp_cells[E]->cost = temp_cells[testingID]->cost+1;
            temp_cells[E]->direction = ofVec2f(0,1.5);
        }
        temp_cells[E]->direction = ofVec2f(0,-1.5);
        testList.push_back(E);
    }
    
    if(temp_cells[S]->isPassible){
        if(temp_cells[testingID]->cost>0){ //when the input val is positive
            temp_cells[S]->cost = temp_cells[testingID]->cost-1;
            temp_cells[S]->direction = ofVec2f(1.5,0);
        }
        if(temp_cells[testingID]->cost<0){ //when the input val is negative
            temp_cells[S]->cost = temp_cells[testingID]->cost+1;
            temp_cells[S]->direction = ofVec2f(-1.5,0);
        }
        testList.push_back(S);
    }
    
    
    if(temp_cells[W]->isPassible){
        if(temp_cells[testingID]->cost>0){ //when the input val is positive
            temp_cells[W]->cost = temp_cells[testingID]->cost-1;
            temp_cells[W]->direction = ofVec2f(0,1.5);
        }
        if(temp_cells[testingID]->cost<0){ //when the input val is negative
            temp_cells[W]->cost = temp_cells[testingID]->cost+1;
            temp_cells[W]->direction = ofVec2f(0,-1.5);
        }
        testList.push_back(W);
    }
    
}
void PotentialField::calculateField(int _id){
    
    testList.push_back(_id);
    
    while(testList.size()){
        testList.unique(); //get only unigue numbers
        int targetN = testList.front();
        testList.pop_front(); //delete target one
        int CurrentX = targetN%cols;
        int CurrentY = targetN/cols;
        //cout << "X : "<<CurrentX<<" Y :"<<CurrentY<<"\n";
        if(temp_cells[targetN]->isPassible){
            temp_cells[targetN]->isPassible = false;
            if(CurrentY<=0||CurrentX<=0||CurrentY>=rows-1||CurrentX>=cols-1)continue;
            if(temp_cells[targetN]->cost != 0){
                findNeighbors(CurrentX, CurrentY);
            }
            
        }
    }
    
}

void PotentialField::drawVectors(){
    
    for(int x=0; x < cols; x++){
        for(int y=0; y < rows; y++){
            int id = y * cols +x;
            float nX,nY;
            nX = x*resolution+resolution*0.5;
            nY = y*resolution+resolution*0.5;
            //ofLine(nX, nY, nX+10, nY+10);
            ofPushMatrix();
            ofTranslate(nX, nY);
            ofSetColor(255,0,255);
            if(cells[id]->cost==0)
                ofRect(0,0,2,2);
            ofSetColor(255);
            
            ofLine(0, 0, cells[id]->direction.x*5, cells[id]->direction.y*5);
            ofPopMatrix();
        }
    }
    
}

void PotentialField::draw(){
    
    for(int x=0; x < cols; x++){
        for(int y=0; y < rows; y++){
            int id = y * cols +x;
            cells[id]->draw(x*resolution,y*resolution);
        }
    }
    drawVectors();

    for(int j=0; j<cells.size(); j++){
        for(int i=0; i < cells[j]->particles.size(); i++){
            
            //cells[j]->particles[i]->draw();
        }
    }
    ofSetColor(0, 0, 0,100);
    ofFill();
    ofRect(0,0,width,height);
    
}


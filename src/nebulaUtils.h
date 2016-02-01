#pragma once

namespace nebula {
   class Utils {
   public:
     static ofVec2f polToCar(ofVec2f pol){
       return ofVec2f(pol.x * cos(pol.y), pol.x * sin(pol.y));
     }
     static ofVec2f carToPol(ofVec2f car){
       return ofVec2f(car.length(), car.angleRad(ofVec2f(1,0)));
     }

     static ofVec2f carToPol(float x, float y){
       return carToPol(ofVec2f(x,y));
     }

     static ofVec2f polToCar(float r, float t){
       return polToCar(ofVec2f(r,t));
     }
   };
}
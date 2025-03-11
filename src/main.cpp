#include <Motor.h>

void setup(){

  // Initialize serial monitor
  Serial.begin(115200);
  
  encoderSetup();
  motorSetup();
  mpuSetup();
  ultrasonicSetup();

  currentTime = millis();

  Serial.println("Done setup..");

  delay(1000);

  while(1){;}
}


void loop(){

  if(getDistance(FRONT) > 2){
    if(!isMoving){
      // Turning function
      if(isTurnLeft || isTurnRight || isUTurn){
        if(isTurnRight){
          // Turn
          turnRight();
          // Move to next block after turning
          moveForwardAfterTurn();
          isTurnRight = false;
        }else if(isTurnLeft){
          // Turn
          turnLeft();
          // Move to next block after turning
          moveForwardAfterTurn();
          isTurnLeft = false;
        }else if(isUTurn){
          // U-Turn
          uTurn();
          // Move to next block after U-Turn
          moveForwardAfterTurn();
          isUTurn = false;
        }
      }
      else{
        // Move forward if no flag is set
        moveForward();
        isMoving = true;
        resetDistance();
      }
    }else{
      float ultrasonicResult = getDistance(FRONT);
      // Condition for precisely move between blocks
      if((getMovingDistance() > 30 && int(ultrasonicResult) % 27 <= 4) || ultrasonicResult < 6){
        Serial.print(getMovingDistance());
        Serial.print("  " + String(int(ultrasonicResult) % 27));
        Serial.print("  " + String(ultrasonicResult));
        Serial.println();
        stop();
        isMoving = false;
        isReachPoint = true;
      }else{
        //Moving forward + Align
        if(millis() - current > 30){
          distance[0] = getDistance(LEFT);
          distance[1] = getDistance(RIGHT);

          Serial.print("FRONT: " + String(ultrasonicResult) + " LEFT: " + String(distance[0]) + " RIGHT: " + String(distance[1]) + " ");

          if(distance[0] < mazeWidth && distance[1] < mazeWidth){ // No branch
            if(distance[0] - distance[1] < -3){
              Serial.println("Align Right");
              alignRight();
            }else if(distance[0] - distance[1] > 3){
              Serial.println("Align Left");
              alignLeft();
            }else{
              Serial.println("Move Forward");
              moveForward();
            }
          }else if(distance[0] < mazeWidth && distance[1] > mazeWidth){ // Branch at Rught
            if(distance[0] < 6){
              Serial.println("Align Right");
              alignRight();
            }else if(distance[0] > 8){
              Serial.println("Align Left");
              alignLeft();
            }else{
              Serial.println("Move Forward");
              moveForward();
            }
          }else if(distance[0] > mazeWidth && distance[1] < mazeWidth){ // Branch at Left
            if(distance[1] < 6){
              Serial.println("Align Left");
              alignLeft();
            }else if(distance[1] > 8){
              Serial.println("Align Right");
              alignRight();
            }else{
              Serial.println("Move Forward");
              moveForward();
            }
          }else{ // Branches at Left and Right
            update();
            Serial.print(String(angle) + " ");
            if(angle < 0){
              if(angle - targetAngle < -10){
                  Serial.println("Align Left");
                  alignLeft();
              }else if(angle - targetAngle > 10){
                  Serial.println("Align Right");
                  alignRight();
              }else{
                  Serial.println("Move Forward");
                  moveForward();
              }
            }else{
              if(angle - targetAngle > 5){
                  Serial.println("Align Right");
                  alignRight();
              }else if(angle - targetAngle  < -5){
                  Serial.println("Align Left");
                  alignLeft();
              }else{
                  Serial.println("Move Forward");
                  moveForward();
              }
            }
          }
          // Update timer
          current = millis();
        }
      }
    }
  }else{ // This usually will not happens
    Serial.println(String(pulsesLeft - pulsesRight));
    Serial.println(String(pulsesLeft + pulsesRight));
    Serial.println(String(getMovingDistance()));
    stop();
    while(1){;}
  }

  // Run this block after reach the middle of the block to detect posible branches for next movement (Here is where the turning flag is raised)
  if(isReachPoint){

    // Based on the branches, undergo DFS to determine whether the robot need to turn or go straight

    isMoving = false;
    isReachPoint = false;

    delay(500);

    currentTime = millis();
    
  }
}
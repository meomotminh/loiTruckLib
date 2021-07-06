
#include <can.h>
#include <iterator>
#include <vector>
#include <map>
#include <Arduino.h>
#include <SPI.h>
#include <mcp2515.h>
#include <LiquidCrystal_I2C.h>
#include <Servo.h>
#include <loiTruck.h>

LiquidCrystal_I2C lcd(0x27, 20, 4);
Servo servoLenken;

// VAR for Poti and Servo
int val = 0;
int maxPot = 0;
int minPot = 1000;
int starting = 0;
int servoWrite = 0;

// VAR for joystick
int VRx = A1;
int VRy = A2;
int SW = 2;
int xPos;
int yPos;
int mapX;
int mapY;
int SW_state = 0;
int last_SW_state = 0;
int last_saved_mapX;
int last_saved_mapY = 0;
bool flag_Interrupt = false;

// VAR for Potentialmeter
int potPin = A0;

// VAR for MCP2515
MCP2515 mcp2515(53);

// Global VAR
struct can_frame req;
struct can_frame res;
__u8 test_data[8];
can_frame checkList[24];
int count = 0;

// VAR for loiTruck
LOITRUCK loiTruck = LOITRUCK(MODE_HAPPY, servoLenken);

void isr(){     
  if (loiTruck._runState == STATE_RUN){
    flag_Interrupt = true;     
  }
  
}

void setup()
{
    Serial.begin(9600);

    // Configure INT1            
    attachInterrupt(digitalPinToInterrupt(2),isr, FALLING);

    // Poti configuration
    starting = millis();
    // LCD configuration
    lcd.init();
    lcd.backlight();
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Start Calibration!");
    Serial.println("Start Calibration!");
    
    while (millis() - starting < 10000){
      val = analogRead(potPin);
    
      if (val > maxPot){
        maxPot = val;
      } else if (val < minPot){
        minPot = val;  
      }    
    }
    

    Serial.print("min val : "); Serial.println(minPot);
    Serial.print("max val : "); Serial.println(maxPot);
    String tempstr = String(minPot,HEX);
    __u8 firstByte = strtoul(tempstr.substring(0, 2).c_str(),NULL,16);
    __u8 secondByte = strtoul(tempstr.substring(2).c_str(),NULL,16);
    
    loiTruck.loiTruck_Lenken_Min_0 = firstByte;
    loiTruck.loiTruck_Lenken_Min_1 = secondByte;

    tempstr = String(maxPot,HEX);
    firstByte = strtoul(tempstr.substring(0, 2).c_str(),NULL,16);
    secondByte = strtoul(tempstr.substring(2).c_str(),NULL,16);
    
    loiTruck.loiTruck_Lenken_Max_0 = 0xFF;
    loiTruck.loiTruck_Lenken_Max_1 = 0xFF;    
    
    

    // JOYSTICK configuration
    pinMode(VRx, INPUT);
    pinMode(VRy, INPUT);
    pinMode(SW, INPUT_PULLUP);

    // Servo Configuration
    loiTruck._servo.attach(3);

    // Fake Heartbeat
    __u8 temp0[] = {0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0};
    __u8 temp1[] = {0x0,0x46,0x0,0x0,0x0,0x0,0x0,0x0};
    __u8 temp2[] = {0x0,0x0,0x0,0x0,0x0,0x3D,0x0,0x0};
    __u8 temp3[] = {0x0,0x0,0x0,0x0,0x0,0x02,0x0,0x0};
    __u8 temp4[] = {0x3,0x0,0x0,0x0,0x0,0x00,0x0,0x0};
    __u8 temp5[] = {0x5,0x0,0x0,0x0,0x0,0x00,0x0,0x0};
    __u8 temp6[] = {0x49,0x01,0x2E,0x74,0x0,0x00,0x0,0x0};
    __u8 temp7[] = {0x23,0x40,0x53,0x01,0x7C,0x00,0x00,0x10};
    __u8 temp8[] = {0x23,0x40,0x53,0x01,0x00,0x00,0x00,0x00};

  
  for (int i = 0; i<23; i++)
  {
    switch (i){
      case 0: checkList[i] = create_CAN_frame(0x80,0, temp0); break;
      case 1: checkList[i] = create_CAN_frame(0x20E,8, temp1); break;
      case 2: checkList[i] = create_CAN_frame(0x30E,8, temp2); break;
      case 3: checkList[i] = create_CAN_frame(0x21D,8, temp0); break;
      case 4: checkList[i] = create_CAN_frame(0x31D,8,temp0); break;
      case 5: checkList[i] = create_CAN_frame(0x18D,8, temp3); break;
      case 6: checkList[i] = create_CAN_frame(0x70D,1, temp0); break;
      case 7: checkList[i] = create_CAN_frame(0x203,8, temp4); break;
      case 8: checkList[i] = create_CAN_frame(0x701,1, temp5); break;
      case 9: checkList[i] = create_CAN_frame(0x80,0, temp0); break;
      case 10: checkList[i] = create_CAN_frame(0x20E,8, temp1); break;
      case 11: checkList[i] = create_CAN_frame(0x183,0, temp0); break;
      case 12: checkList[i] = create_CAN_frame(0x30E,8, temp2); break;
      case 13: checkList[i] = create_CAN_frame(0x21D,8, temp0); break;
      case 14: checkList[i] = create_CAN_frame(0x18D, 8, temp3); break;
      case 15: checkList[i] = create_CAN_frame(0x31D,8,temp0); break;
      case 16: checkList[i] = create_CAN_frame(0x704,1,temp5); break;
      case 17: checkList[i] = create_CAN_frame(0x80,0,temp0); break;
      case 18: checkList[i] = create_CAN_frame(0x181,8,temp6); break;
      case 19: checkList[i] = create_CAN_frame(0x20E,8,temp1); break;
      case 20: checkList[i] = create_CAN_frame(0x30E,8,temp2); break;
      case 21: checkList[i] = create_CAN_frame(0x603,8,temp7); break;
      case 22: checkList[i] = create_CAN_frame(0x583,8,temp8); break;
      case 23: checkList[i] = create_CAN_frame(0x703,1,temp5); break;     
    }
                
  }

    // Configure mcp2515
    mcp2515.reset();
    mcp2515.setBitrate(CAN_250KBPS, MCP_8MHZ);
    mcp2515.setOneShotMode();

    
         
    loiTruck.create_CAN_Segmented_map();
       
    // PRINT CAN_Segmented_Map
    Serial.print("Segmented Map size : ");
    Serial.println(loiTruck.CAN_Segmented_Map.size());
    std::map<String, String>::iterator it2;
    for (it2 = loiTruck.CAN_Segmented_Map.begin(); it2 != loiTruck.CAN_Segmented_Map.end(); ++it2){
      Serial.print(it2->first + "\t");
      Serial.println(it2->second);  
    }

    loiTruck.display_LCD(lcd);
}




void loop()
{                   
      if ((loiTruck._runState != STATE_RUN) || (flag_Interrupt)){
            xPos = analogRead(VRx);
            yPos = analogRead(VRy);
            
            // debouncing button ?? 
                        
              SW_state = digitalRead(SW);
              mapX = map(xPos, 0, 1023, -512, 512);
              mapY = map(yPos, 0, 1023, -512, 512);

              if (((abs(mapX) > 500) && (abs(last_saved_mapX) < 500)) || ((abs(mapY)> 500) && (abs(last_saved_mapY) < 500)) || ((SW_state == 0) && (last_SW_state == 1)))
              {               
                loiTruck.modify_after_joystick(mapX, mapY, SW_state, lcd);
                loiTruck.display_LCD(lcd);
              }
              flag_Interrupt = false;  
              last_SW_state = SW_state;
              last_saved_mapX = mapX;
              last_saved_mapY = mapY;                                  
      }
     

      // always actual servo
      loiTruck.actuate_servo(minPot, maxPot);
                  
      
      // only read mcp when in run state

      if (loiTruck._runState == STATE_RUN){
        if ((mcp2515.readMessage(&req) == MCP2515::ERROR_OK)) {
          count++;

          if(count > 5){count = 0;}
      
          // CHECK IF INTEREST
          if (check_COB_ID_range(req)){
            Serial.print("REQUEST "); Serial.println(count);
            Serial.println(create_str_from_frame(req));
          
            // CHECK SEGMENTED
            if (loiTruck.check_Segmented(req)){
                res = loiTruck.get_Segmented_Response(req, lcd);
            } else {
              // Expedited Response
              res = loiTruck.get_Expedited_Response(req, lcd);
            }
            
            //delay(50);
            if (loiTruck.ignore){              
              Serial.print("RESPONSE "); Serial.println(count);
              Serial.println("***ignore***");                        
              loiTruck.ignore = false;
            } else {              
              mcp2515.sendMessage(&res);              
              Serial.print("RESPONSE "); Serial.println(count);
              Serial.println(create_str_from_frame(res));                                      
            }
            
          }
        }
      
        // Send Fake Heartbeat Messages

        for (int i = 0; i<24; i++)
        {
           mcp2515.sendMessage(&checkList[i]);  
           delay(1);
        }
             
      }
      
      
      

    //delay(50);
}

// loiTruck.cpp : This file contains the 'main' function. Program execution begins and ends there.
//


#include <iterator>
#include <stdint.h>
#include <vector>
#include <map>
#include <Arduino.h>
#include <SPI.h>
#include <mcp2515.h>
#include <LiquidCrystal_I2C.h>
#include <Servo.h>
#include "loiTruck.h"


// Global Vector for data
//std::vector<std::vector<__u8>> req_arr;
//std::vector<std::vector<__u8>> res_arr; 


// Global String
String dic_req_str;
String dic_res_str;

// clear lcd line
void clearLCDLine(int line, LiquidCrystal_I2C lcd)
{
    lcd.setCursor(0, line);
    for (int i = 0; i < 20; i++)
    {
        lcd.print(" ");
    }
}


// check if it is in concerning range 0x601 -> 0x67F
bool check_COB_ID_range(can_frame req)
{    
    if (req.can_id >= 0x601 && req.can_id <= 0x67F)
    {
        return true;
    } else return false;
}

// create map index & subindex to the corresponding index in the LOOK UP TABLE
bool LOITRUCK::create_map()
{   
    std::map<int, int> temp;
    int i = 0;
    
    // Same map for all fahrzeug
            temp.insert(std::pair<int, int>(20001,i++)); // 0 
            temp.insert(std::pair<int, int>(20003,i++)); // 1
            temp.insert(std::pair<int, int>(20011,i++)); // 2
            
            temp.insert(std::pair<int, int>(20021,i++));  // Seri nummer 3
            
            temp.insert(std::pair<int, int>(20022,i++));  // 4

            temp.insert(std::pair<int, int>(20201,i++));  // betriebszeit 5
            temp.insert(std::pair<int, int>(20202,i++)); // 6
            temp.insert(std::pair<int, int>(21002,i++));  // 7
            temp.insert(std::pair<int, int>(21006,i++));  // 8
            temp.insert(std::pair<int, int>(21012,i++));  // 9
            temp.insert(std::pair<int, int>(21016,i++));  // 10
            temp.insert(std::pair<int, int>(21022,i++));  // 11
            temp.insert(std::pair<int, int>(21026,i++));  // 12
            temp.insert(std::pair<int, int>(21032,i++));  // 13
            temp.insert(std::pair<int, int>(21036,i++));  // 14
            temp.insert(std::pair<int, int>(21042,i++));  // 15
            temp.insert(std::pair<int, int>(21061,i++));  // 16
            temp.insert(std::pair<int, int>(22002,i++));  // 17
            temp.insert(std::pair<int, int>(22006,i++));  // 18
            temp.insert(std::pair<int, int>(22012,i++));  // 19
            temp.insert(std::pair<int, int>(22016,i++)); // 20       

            temp.insert(std::pair<int, int>(24132,i++)); //---- Lenkkorrektur ----21
            temp.insert(std::pair<int, int>(29232,i++)); //---- Bandagenentspannung ---22
            temp.insert(std::pair<int, int>(24142,i++)); //---- Lenkübersetzung ---23
            temp.insert(std::pair<int, int>(24612,i++)); //---- SollStatus ---    24
            temp.insert(std::pair<int, int>(24052,i++)); //---- SollLenkwinkel Link --- 25   
            temp.insert(std::pair<int, int>(24022,i++)); //---- IstLenkwinkel Link ---  26
            
            temp.insert(std::pair<int, int>(20012,i++)); //---- Logbuch sollindex --- 27   
            temp.insert(std::pair<int, int>(20013,i++)); //---- Logbuch istindex ---   28 
            temp.insert(std::pair<int, int>(24112,i++)); //---- Kennlinie ---    29

            temp.insert(std::pair<int, int>(24057,i++)); //---- SollLenkwinkel Link 07 ---30    
            temp.insert(std::pair<int, int>(24602,i++)); //---- IstStatus 02 --- 31  
            
            temp.insert(std::pair<int, int>(24042,i++)); //---- SollLenkwinkel Recht ---- 32
            temp.insert(std::pair<int, int>(24012,i++)); //---- IstLenkwinkel Recht ---- 33
            
            temp.insert(std::pair<int, int>(24032,i++)); //---- SollLenkwinkel Null ----  34
            
            
            temp.insert(std::pair<int, int>(24002,i++)); //---- IstLenkwinkel Null non teach ---- 35 
            temp.insert(std::pair<int, int>(24037,i++)); //---- Min IstLenkwinkel Null teach ---- 36
            temp.insert(std::pair<int, int>(24047,i++)); //---- Min IstLenkwinkel Recht teach ---- 37
            
            
            temp.insert(std::pair<int, int>(24053,i++)); //---- Min value lenken --- 38           
            temp.insert(std::pair<int, int>(24043,i++)); //---- Min value lenken --- 39
            temp.insert(std::pair<int, int>(24033,i++)); //---- Min value lenken --- 40
            temp.insert(std::pair<int, int>(24054,i++)); //---- Max value lenken --- 41
            temp.insert(std::pair<int, int>(24044,i++)); //---- Max value lenken --- 42
            temp.insert(std::pair<int, int>(24034,i++)); //---- Max value lenken --- 43

            temp.insert(std::pair<int, int>(24027,i++)); //---- Ist Teach-In Link--- 44
            temp.insert(std::pair<int, int>(24017,i++)); //---- Ist Teach-In Recht---45 
            temp.insert(std::pair<int, int>(24007,i++)); //---- Ist Teach-In Null---46
            
            temp.insert(std::pair<int, int>(10101,i++)); //---- Write SAVE---47
            temp.insert(std::pair<int, int>(20123,i++)); //---- Write LOAD---48    TEST 201213 too big
            temp.insert(std::pair<int, int>(24058,i++)); //---- save link --- 49           
            temp.insert(std::pair<int, int>(24048,i++)); //---- save recht --- 50
            temp.insert(std::pair<int, int>(24038,i++)); //---- save null --- 51

            temp.insert(std::pair<int, int>(24055,i++)); //---- standard --- 52           
            temp.insert(std::pair<int, int>(24045,i++)); //---- standard --- 53
            temp.insert(std::pair<int, int>(24035,i++)); //---- standard --- 54

            temp.insert(std::pair<int, int>(24056,i++)); //---- werk --- 55           
            temp.insert(std::pair<int, int>(24046,i++)); //---- werk --- 56
            temp.insert(std::pair<int, int>(24036,i++)); //---- werk --- 57

            temp.insert(std::pair<int, int>(24003,i++)); //---- IstLenkwinkel Null non teach ---- 58
            temp.insert(std::pair<int, int>(24013,i++)); //---- Min IstLenkwinkel Null teach ---- 59
            temp.insert(std::pair<int, int>(24023,i++)); //---- Min IstLenkwinkel Recht teach ---- 60
    
            
            temp.insert(std::pair<int, int>(24004,i++)); //---- IstLenkwinkel Null non teach ---- 61
            temp.insert(std::pair<int, int>(24014,i++)); //---- Min IstLenkwinkel Null teach ---- 62
            temp.insert(std::pair<int, int>(24024,i++)); //---- Min IstLenkwinkel Recht teach ---- 63
            
    this->CAN_Expedited_Map.clear();
    this->CAN_Expedited_Map = temp;

    // create Fahrzeug name map
    this->create_segmented_res_Fahrzeug_Name();    
    
    // display segmented array
    Serial.print("Detail Segmented Fahrzeug Name size : ");
    Serial.println(sizeof(this->segmented_res_Fahrzeug_Name)/sizeof(this->segmented_res_Fahrzeug_Name[0]));
    for (int i = 0; i<5; i++)
    {
      Serial.println(this->segmented_res_Fahrzeug_Name[i]);  
    }
    
    //Serial.println("Created map");
    // PRINT CAN_Expedited_Map
    Serial.print("Expedited Map size : ");
    Serial.println(this->CAN_Expedited_Map.size());
      
    std::map<int, int>::iterator it;
    for (it = this->CAN_Expedited_Map.begin(); it != this->CAN_Expedited_Map.end(); ++it){
        Serial.print(it->first);Serial.print("-->");Serial.println(it->second);       
    }        

    
    return true; 
}



// Calculate COB_ID
__u16 prepare_ID(__u16 ID_req)
{
    __u16 node_ID = ID_req & 0x07f;
    return (node_ID | 0x580);
}

// calculate Command_ID
__u8 LOITRUCK::prepare_Command_ID(can_frame req, bool end_msg)
{
    __u8 command_ID = req.data[0] & 0xf0;  // filter out n,e,struct MyStruct
    __u16 index_ID = (req.data[2] << 8) | (req.data[1]);
    __u8 subindex_ID = req.data[3];

    Serial.print("just_save"); Serial.println(this->_just_Save);
    // only in ADT & apply all or ADT & write command
    if (this->_runMode == MODE_ADT){
        if ((this->_runMode_Apply == ALL) || ((command_ID == 0x20) && (this->_runMode_Apply == WRITE_REQ)) ){
            command_ID = 0x80; // ABORT        
        } 
        if ((command_ID == 0x40) && (this->_runMode_Apply == WRITE_CHECK) && this->_just_Save){
            command_ID = 0x80; // ABORT    
            this->_just_Save = false;              
        }  
    }      
    if (command_ID != 0x80)      
    {
        switch (command_ID) {
        case 0x20: // initiate domain download            
            command_ID = 0x60;  // Server reply                        
            break;
        case 0x00:  // download domain segment toggle = 0
            command_ID |= 0x20;  // or to remain toggle bit
            break;
        case 0x40:  // initiate domain upload
            if (req.data[2] == 0x24 && subindex_ID == 0x02)
            {
                command_ID |= 0x0B;    // follow Noris excel   
            } else {
                command_ID |= 0x03;  // n = 0; e = s = 1
            }
            
            break;        
        case 0x60:  // upload domain segment with toggle bit = 0
            if (end_msg) {command_ID = 0x01;} else {command_ID = 0x00;}
            break;
        case 0x70:  // upload domain segment with toggle bit = 1
            if (end_msg) {command_ID = 0x11;} else {command_ID = 0x10;}
            break;
        case 0xc0:  // initiate block download
            command_ID = 0xA0;
            break;
        default : // Unknown
            command_ID = 0x00;
            break;
        }    
    }

    // IGNORE or NOT
    if (this->_runMode == MODE_IGNORE){
            if ((this->_runMode_Apply == ALL) || ((command_ID == 0x20) && (this->_runMode_Apply == WRITE_REQ))){
                this->ignore = true;
            } else if ((command_ID == 0x40) && (this->_just_Save)){
                this->ignore = true;                                
            }
    }   

    // DELAY FINALLY
    if ((this->_runMode_Apply == ALL) || ((command_ID == 0x20) && (this->_runMode_Apply == WRITE_REQ))){
        delay(this->_runMode_Delay);
    } else if ((command_ID == 0x40) && (this->_just_Save)){
        delay(this->_runMode_Delay);   
        this->_just_Save = false;               
    }    


    return command_ID;
}

bool LOITRUCK::modify_after_joystick(int mapx, int mapy, int clicked, LiquidCrystal_I2C lcd)
{   
    if (clicked == 0){
        // CLICKED
        switch (this->_runState)
        {
            case STATE_WELCOME:
                if (this->_mousePos == 2){
                    this->_runState = STATE_SELECT_TRUCK;
                } else if (this->_mousePos == 3){
                    this->_runState = STATE_DEMO;
                    this->display_LCD(lcd);
                    this->demo_test();    
                }                
                break;
            case STATE_SELECT_TRUCK:
                this->_runState = STATE_SELECT_MODE;
                this->selected_Truck = this->_mousePos;
                this->create_map();
                break;
            case STATE_SELECT_MODE:
                this->_runState = STATE_CONFIG;
                this->_runMode = RUN_MODE(this->_mousePos);
                break;    
            case STATE_CONFIG:                
                switch (this->_mousePos) {
                    case 1:// line 1 Range
                        this->_runState = CONFIG_SELECT_RANGE;
                        break;
                    case 2:// line 2 Delay
                        this->_runState = CONFIG_SELECT_DELAY;
                        break;
                    case 3:// line 3 Run
                        this->_runState = STATE_RUN;                                        
                        break;
                    default:
                        break;
                }
                break;            
            // press in config state to confirm and back to state_config          
            case STATE_RUN:
                // if go back and click
                if (mapx > 400){
                    this->_runState = STATE_CONFIG;
                } else {
                    switch (this->_runMode){
                    case MODE_HAPPY:
                        this->_runMode = MODE_ADT;
                        break;
                    case MODE_ADT:
                        this->_runMode = MODE_UNHAPPY;
                        break;
                    case MODE_UNHAPPY:
                        this->_runMode = MODE_IGNORE;
                        break;
                    case MODE_IGNORE:
                        this->_runMode = MODE_HAPPY;
                        break;
                    default:
                        break;
                    }    
                }
                
                break;
            default:
                break;
        }
    }

    if (mapx > 400){
        
        // GO BACK
        switch (this->_runState)
        {
            case STATE_WELCOME:
                break;
            case STATE_SELECT_TRUCK:
                this->_runState = STATE_WELCOME;
                this->_mousePos = 2;
                break;
            case STATE_SELECT_MODE:
                this->_runState = STATE_SELECT_TRUCK;
                this->_mousePos = 2;
                break;            
            case STATE_CONFIG:
                this->_runState = STATE_SELECT_MODE;
                this->_mousePos = 2;
                break;    
            case STATE_DEMO:
                this->_runState = STATE_WELCOME;
                this->_mousePos = 2;
                break;     
            case CONFIG_SELECT_DELAY:
                this->_runState = STATE_CONFIG;
                break;
            case CONFIG_SELECT_RANGE:
                this->_runState = STATE_CONFIG;
                break;                       
            default:
                break;
        }
    }

    if (mapy < -100)
    {
        // GO DOWN
        switch (this->_runState)
        {
            case CONFIG_SELECT_RANGE:
                // Change between WRITE ONLY and ALL
                if (this->_runMode_Apply != WRITE_REQ){
                    this->_runMode_Apply = APPLY_RANGE(this->_runMode_Apply - 1);
                }                
                break;
            case CONFIG_SELECT_DELAY:
                // Decrease delay by 100
                if (this->_runMode_Delay >= 100){this->_runMode_Delay -= 100;}                
                break;            
            default:
                if (this->_mousePos < 3) {this->_mousePos++;}
                break;
        }
                
                
    } else if (mapy > 100)
    {
        // GO UP
        switch (this->_runState)
        {
            case CONFIG_SELECT_RANGE:
                // Change between WRITE ONLY and ALL
                if (this->_runMode_Apply != ALL){
                    this->_runMode_Apply = APPLY_RANGE(this->_runMode_Apply + 1);
                }  
                break;
            case CONFIG_SELECT_DELAY:
                // Decrease delay by 100
                this->_runMode_Delay += 100;               
                break;            
            default:
                if (this->_mousePos > 0){this->_mousePos--;}
                break;
        }
        
    }

    return true;
}

answer LOITRUCK::prepare_Answer(can_frame req, int indx_subindx, LiquidCrystal_I2C lcd)
{
    answer to_Return;
    // Find in the map
    std::map<int, int>::iterator it;
    
    it = this->CAN_Expedited_Map.find(indx_subindx);
    int indx_in_res_table;

    __u8 command_id = req.data[0] & 0xf0;

    //Serial.println(indx_subindx);
    
    // DEFAULT VALUE
    to_Return.data0 = 0x00;
    to_Return.data1 = 0x00;
    to_Return.data2 = 0x00;
    to_Return.data3 = 0x00;

    


    // MODE 
    // if (happy or apply to write only) and not ignore
    if (((this->_runMode == MODE_HAPPY) || (this->_runMode_Apply != ALL)) && (!this->ignore)){
        
            // if found in map    
            if (it != this->CAN_Expedited_Map.end())
            {
                indx_in_res_table = (it->second);                            
                //---------------- WEIRD BEHAVIOR (without switch not run properly)--------------------------
                switch (indx_in_res_table){
                    case 0: // res_2000_01_1
                        to_Return.data0 = pgm_read_byte(*(&res_table_1[indx_in_res_table]));
                        to_Return.data1 = pgm_read_byte(*(&res_table_1[indx_in_res_table])+1);
                        to_Return.data2 = pgm_read_byte(*(&res_table_1[indx_in_res_table])+2);
                        to_Return.data3 = pgm_read_byte(*(&res_table_1[indx_in_res_table])+3);
                        break;
                    case 1: // res_2000_03_1
                        to_Return.data0 = pgm_read_byte(*(&(res_table_1[indx_in_res_table])));
                        to_Return.data1 = pgm_read_byte(*(&res_table_1[indx_in_res_table])+1);
                        to_Return.data2 = pgm_read_byte(*(&res_table_1[indx_in_res_table])+2);
                        to_Return.data3 = pgm_read_byte(*(&res_table_1[indx_in_res_table])+3);
                        break;
                    case 2: // res_2001_01_1
                        to_Return.data0 = pgm_read_byte(*(&(res_table_1[indx_in_res_table])));
                        to_Return.data1 = pgm_read_byte(*(&res_table_1[indx_in_res_table])+1);
                        to_Return.data2 = pgm_read_byte(*(&res_table_1[indx_in_res_table])+2);
                        to_Return.data3 = pgm_read_byte(*(&res_table_1[indx_in_res_table])+3);
                        break;
                    case 3:  // Seri nummer res_2002_01_1
                        to_Return.data0 = this->loiTruck_Seri0;
                        to_Return.data1 = this->loiTruck_Seri1;
                        to_Return.data2 = this->loiTruck_Seri2;
                        to_Return.data3 = this->loiTruck_Seri3;
                        break;
                    case 4: // res_2002_02_1
                        to_Return.data0 = pgm_read_byte(*(&(res_table_1[indx_in_res_table])));
                        to_Return.data1 = pgm_read_byte(*(&res_table_1[indx_in_res_table])+1);
                        to_Return.data2 = pgm_read_byte(*(&res_table_1[indx_in_res_table])+2);
                        to_Return.data3 = pgm_read_byte(*(&res_table_1[indx_in_res_table])+3);
                        break;
                    case 5:  // betriebszeit res_2020_01_1
                        to_Return.data0 = this->loiTruck_Zeit0;
                        to_Return.data1 = this->loiTruck_Zeit1;
                        to_Return.data2 = 0x00;
                        to_Return.data3 = 0x00;
                        break;
                    case 6: // res_2020_02_1
                        to_Return.data0 = pgm_read_byte(*(&(res_table_1[indx_in_res_table])));
                        to_Return.data1 = pgm_read_byte(*(&res_table_1[indx_in_res_table])+1);
                        to_Return.data2 = pgm_read_byte(*(&res_table_1[indx_in_res_table])+2);
                        to_Return.data3 = pgm_read_byte(*(&res_table_1[indx_in_res_table])+3);
                        break;
                    case 7: // res_2100_02_1
                        to_Return.data0 = pgm_read_byte(*(&(res_table_1[indx_in_res_table])));
                        to_Return.data1 = pgm_read_byte(*(&res_table_1[indx_in_res_table])+1);
                        to_Return.data2 = pgm_read_byte(*(&res_table_1[indx_in_res_table])+2);
                        to_Return.data3 = pgm_read_byte(*(&res_table_1[indx_in_res_table])+3);
                        break;
                    case 8: // res_2100_06_1
                        to_Return.data0 = pgm_read_byte(*(&(res_table_1[indx_in_res_table])));
                        to_Return.data1 = pgm_read_byte(*(&res_table_1[indx_in_res_table])+1);
                        to_Return.data2 = pgm_read_byte(*(&res_table_1[indx_in_res_table])+2);
                        to_Return.data3 = pgm_read_byte(*(&res_table_1[indx_in_res_table])+3);
                        break;
                    case 9: // res_2101_02_1
                        to_Return.data0 = pgm_read_byte(*(&(res_table_1[indx_in_res_table])));
                        to_Return.data1 = pgm_read_byte(*(&res_table_1[indx_in_res_table])+1);
                        to_Return.data2 = pgm_read_byte(*(&res_table_1[indx_in_res_table])+2);
                        to_Return.data3 = pgm_read_byte(*(&res_table_1[indx_in_res_table])+3);
                        break;
                    case 10: // res_2101_06_1
                        to_Return.data0 = pgm_read_byte(*(&(res_table_1[indx_in_res_table])));
                        to_Return.data1 = pgm_read_byte(*(&res_table_1[indx_in_res_table])+1);
                        to_Return.data2 = pgm_read_byte(*(&res_table_1[indx_in_res_table])+2);
                        to_Return.data3 = pgm_read_byte(*(&res_table_1[indx_in_res_table])+3);
                        break;
                    case 11: // res_2102_02_1
                        to_Return.data0 = pgm_read_byte(*(&(res_table_1[indx_in_res_table])));
                        to_Return.data1 = pgm_read_byte(*(&res_table_1[indx_in_res_table])+1);
                        to_Return.data2 = pgm_read_byte(*(&res_table_1[indx_in_res_table])+2);
                        to_Return.data3 = pgm_read_byte(*(&res_table_1[indx_in_res_table])+3);
                        break;
                    case 12: // res_2102_06_1
                        to_Return.data0 = pgm_read_byte(*(&(res_table_1[indx_in_res_table])));
                        to_Return.data1 = pgm_read_byte(*(&res_table_1[indx_in_res_table])+1);
                        to_Return.data2 = pgm_read_byte(*(&res_table_1[indx_in_res_table])+2);
                        to_Return.data3 = pgm_read_byte(*(&res_table_1[indx_in_res_table])+3);
                        break;
                    case 13: // res_2103_02_1
                        to_Return.data0 = pgm_read_byte(*(&(res_table_1[indx_in_res_table])));
                        to_Return.data1 = pgm_read_byte(*(&res_table_1[indx_in_res_table])+1);
                        to_Return.data2 = pgm_read_byte(*(&res_table_1[indx_in_res_table])+2);
                        to_Return.data3 = pgm_read_byte(*(&res_table_1[indx_in_res_table])+3);
                        break;
                    case 14: // res_2103_06_1
                        to_Return.data0 = pgm_read_byte(*(&(res_table_1[indx_in_res_table])));
                        to_Return.data1 = pgm_read_byte(*(&res_table_1[indx_in_res_table])+1);
                        to_Return.data2 = pgm_read_byte(*(&res_table_1[indx_in_res_table])+2);
                        to_Return.data3 = pgm_read_byte(*(&res_table_1[indx_in_res_table])+3);
                        break;
                    case 15: // res_2104_02_1
                        to_Return.data0 = pgm_read_byte(*(&(res_table_1[indx_in_res_table])));
                        to_Return.data1 = pgm_read_byte(*(&res_table_1[indx_in_res_table])+1);
                        to_Return.data2 = pgm_read_byte(*(&res_table_1[indx_in_res_table])+2);
                        to_Return.data3 = pgm_read_byte(*(&res_table_1[indx_in_res_table])+3);
                        break;
                    case 16: // res_2106_06_1
                        to_Return.data0 = pgm_read_byte(*(&(res_table_1[indx_in_res_table])));
                        to_Return.data1 = pgm_read_byte(*(&res_table_1[indx_in_res_table])+1);
                        to_Return.data2 = pgm_read_byte(*(&res_table_1[indx_in_res_table])+2);
                        to_Return.data3 = pgm_read_byte(*(&res_table_1[indx_in_res_table])+3);
                        break;
                    case 17: // res_2200_02_1
                        to_Return.data0 = pgm_read_byte(*(&(res_table_1[indx_in_res_table])));
                        to_Return.data1 = pgm_read_byte(*(&res_table_1[indx_in_res_table])+1);
                        to_Return.data2 = pgm_read_byte(*(&res_table_1[indx_in_res_table])+2);
                        to_Return.data3 = pgm_read_byte(*(&res_table_1[indx_in_res_table])+3);
                        break;
                    
                    case 18: // res_2200_06_1
                        to_Return.data0 = pgm_read_byte(*(&(res_table_1[indx_in_res_table])));
                        to_Return.data1 = pgm_read_byte(*(&res_table_1[indx_in_res_table])+1);
                        to_Return.data2 = pgm_read_byte(*(&res_table_1[indx_in_res_table])+2);
                        to_Return.data3 = pgm_read_byte(*(&res_table_1[indx_in_res_table])+3);
                        break; 
                    case 19: // res_2201_02_1
                        to_Return.data0 = pgm_read_byte(*(&(res_table_1[indx_in_res_table])));
                        to_Return.data1 = pgm_read_byte(*(&res_table_1[indx_in_res_table])+1);
                        to_Return.data2 = pgm_read_byte(*(&res_table_1[indx_in_res_table])+2);
                        to_Return.data3 = pgm_read_byte(*(&res_table_1[indx_in_res_table])+3);
                        break;
                        
                    case 20: // res_2201_06_1
                        to_Return.data0 = pgm_read_byte(*(&(res_table_1[indx_in_res_table])));
                        to_Return.data1 = pgm_read_byte(*(&res_table_1[indx_in_res_table])+1);
                        to_Return.data2 = pgm_read_byte(*(&res_table_1[indx_in_res_table])+2);
                        to_Return.data3 = pgm_read_byte(*(&res_table_1[indx_in_res_table])+3);
                        break;

                    case 21: //---- Lenkkorrektur 2413---- 
                        to_Return.data0 = this->loiTruck_Lenken_Korrektur;
                        to_Return.data1 = 0x00;
                        to_Return.data2 = 0x00;
                        to_Return.data3 = 0x00;
                        break;
                    case 22: //---- Bandagenentspannung 2923---
                        to_Return.data0 = this->loiTruck_Lenken_Zeit_Einfall;
                        to_Return.data1 = 0x00;
                        to_Return.data2 = 0x00;
                        to_Return.data3 = 0x00;
                        break;
                    case 23: //---- Lenkübersetzung 2414---
                        to_Return.data0 = this->loiTruck_Lenken_Ubersetzung;
                        to_Return.data1 = 0x00;
                        to_Return.data2 = 0x00;
                        to_Return.data3 = 0x00;
                        break;
                    case 24: //---- Status 2461---  
                        //Serial.println("Ask Sollwert LenkenStatus 2461");                      
                        
                        to_Return.data0 = this->loiTruck_Lenken_Soll_Status_0;                         
                        to_Return.data1 = this->loiTruck_Lenken_Soll_Status_1;                        
                        to_Return.data2 = 0x00;
                        to_Return.data3 = 0x00;                       
                        break;
                    case 25: //---- Sollwert 2405 02---  
                        //Serial.println("Ask Sollwert Link 2405");    
                        // 10s TimeOut
                        
                        to_Return.data0 = this->loiTruck_Lenken_SollLenkwinkel_Link_0;                         
                        to_Return.data1 = this->loiTruck_Lenken_SollLenkwinkel_Link_1;
                        to_Return.data2 = 0x00;
                        to_Return.data3 = 0x00;
                        break;
                    case 26: //---- Status 2402---
                        //Serial.println("Ask Istwert Link 2402");                        
                        to_Return.data0 = this->loiTruck_Lenken_IstLenkwinkel_Link_0;                         
                        to_Return.data1 = this->loiTruck_Lenken_IstLenkwinkel_Link_1;   
                        to_Return.data2 = 0x00;
                        to_Return.data3 = 0x00;
                        break;
                    case 27: //---- Logbuch sollindex 2001 02---
                        break;

                    case 28: //---- Status ist index 2001 03---
                        to_Return.data0 = this->loiTruck_Logbuch_SavedIndx;                         
                        to_Return.data1 = 0x00;   
                        to_Return.data2 = 0x00;
                        to_Return.data3 = 0x00;
                        break;

                    case 29: //---- Kennlinie 2411 02---
                        //Serial.println("Ask Kennlinie 2411");                        
                        to_Return.data0 = this->loiTruck_Lenken_Kennlinie;                         
                        to_Return.data1 = 0x00;   
                        to_Return.data2 = 0x00;
                        to_Return.data3 = 0x00;
                        break;
                    
                    case 30: //---- SollLenkwinkel Link 2405 07---
                        //Serial.println("Ask Solllenkwinkel Link 2405");                        
                        to_Return.data0 = this->loiTruck_Lenken_SollLenkwinkel_Link_0;                         
                        to_Return.data1 = this->loiTruck_Lenken_SollLenkwinkel_Link_1;
                                               
                        to_Return.data2 = 0x00;
                        to_Return.data3 = 0x00;                  
                        break;
                    
                    case 31: //---- IstLenkwinkel 2460 02---
                        //Serial.println("Ask Istwert LenkenStatus 2460");      
                        if (this->_ist_teach_In == true){
                            //long time_pass = millis() - this->_last_saved_time;
                            this->_count_teach_In++;
                            if (this->_count_teach_In < 10){
                                //this->loiTruck_Lenken_IstLenkwinkel_Link_0 = this->loiTruck_Lenken_SollLenkwinkel_Link_0 - this->_count_teach_In;
                                
                                             
                            } else if ((this->_count_teach_In == 20) && (this->loiTruck_Lenken_Ist_Status_0 == 0x00)){
                                this->loiTruck_Lenken_Ist_Status_0 = 0x22;
                                this->loiTruck_Lenken_Ist_Status_1 = 0x22;   
                                //this->loiTruck_Lenken_IstLenkwinkel_Link_0 = this->loiTruck_Lenken_SollLenkwinkel_Link_0;
                                //this->loiTruck_Lenken_IstLenkwinkel_Link_1 = this->loiTruck_Lenken_SollLenkwinkel_Link_1;
                                
                                //this->_last_saved_time = millis();                                                                                                                                                                   
                                this->_servo.write(60);

                            } else if ((this->_count_teach_In > 20) && (this->_count_teach_In < 30)){
                                //this->loiTruck_Lenken_IstLenkwinkel_Recht_0 = this->loiTruck_Lenken_SollLenkwinkel_Recht_0 - random(0,10);
                                
                                
                            } else if ((this->_count_teach_In == 40)  && (this->loiTruck_Lenken_Ist_Status_0 == 0x22)){
                                this->loiTruck_Lenken_Ist_Status_0 = 0x66;
                                this->loiTruck_Lenken_Ist_Status_1 = 0x66;   
                                //this->loiTruck_Lenken_IstLenkwinkel_Recht_0 = this->loiTruck_Lenken_SollLenkwinkel_Recht_0;
                                //this->loiTruck_Lenken_IstLenkwinkel_Recht_1 = this->loiTruck_Lenken_SollLenkwinkel_Recht_1;

                                this->_servo.write(90);
                            }
                            else if ((this->_count_teach_In > 40) && (this->_count_teach_In < 50)){
                                //this->loiTruck_Lenken_IstLenkwinkel_Null_0 = this->loiTruck_Lenken_SollLenkwinkel_Null_0 - random(0,10);                                                                                                                                                                                                         
                            } else if ((this->_count_teach_In == 50)  && (this->loiTruck_Lenken_Ist_Status_0 == 0x66)){
                                this->loiTruck_Lenken_Ist_Status_0 = 0x77;
                                this->loiTruck_Lenken_Ist_Status_1 = 0x77;
                                
                                //this->loiTruck_Lenken_IstLenkwinkel_Null_0 = this->loiTruck_Lenken_SollLenkwinkel_Null_0;
                                //this->loiTruck_Lenken_IstLenkwinkel_Null_1 = this->loiTruck_Lenken_SollLenkwinkel_Null_1;

                                this->_servo.write(120);

                                this->_count_teach_In = 0;
                                this->_ist_teach_In = false;
                            }                               
                        }

                        to_Return.data0 = this->loiTruck_Lenken_Ist_Status_0;                         
                        to_Return.data1 = this->loiTruck_Lenken_Ist_Status_1;                        
                        to_Return.data2 = 0x00;
                        to_Return.data3 = 0x00;                  
                        break;

                    case 32: //---- SollLenkwinkel Recht 2404 02---
                        //Serial.println("Ask SollLenkwinkel Recht 2404");                        
                        to_Return.data0 = this->loiTruck_Lenken_SollLenkwinkel_Recht_0;                         
                        to_Return.data1 = this->loiTruck_Lenken_SollLenkwinkel_Recht_1;
                        to_Return.data2 = 0x00;
                        to_Return.data3 = 0x00;                  
                        break;

                    case 33: //---- IstLenkwinkel Recht 2401 02---
                        //Serial.println("Ask IstLenkwinkel Recht 2401");                        
                        to_Return.data0 = this->loiTruck_Lenken_IstLenkwinkel_Recht_0;                         
                        to_Return.data1 = this->loiTruck_Lenken_IstLenkwinkel_Recht_1;
                        to_Return.data2 = 0x00;
                        to_Return.data3 = 0x00;                  
                        break;

                    case 34: //---- SollLenkwinkel Null 2403 02---
                        //Serial.println("Ask SollLenkwinkel Null 2403");                        
                        to_Return.data0 = this->loiTruck_Lenken_SollLenkwinkel_Null_0;                         
                        to_Return.data1 = this->loiTruck_Lenken_SollLenkwinkel_Null_1;
                        to_Return.data2 = 0x00;
                        to_Return.data3 = 0x00;                  
                        break;

                    case 35: //---- IstLenkwinkel Null 2400 02---
                        //Serial.println("Ask IstLenkwinkel Null 2400");                        
                        to_Return.data0 = this->loiTruck_Lenken_IstLenkwinkel_Null_0;                         
                        to_Return.data1 = this->loiTruck_Lenken_IstLenkwinkel_Null_1;
                        to_Return.data2 = 0x00;
                        to_Return.data3 = 0x00;                  
                        break;

                    case 36: //---- Poti IstLenkwinkel Null teach---24037
                        //Serial.println("Ask Poti IstLenkwinkel Null");
                        to_Return.data0 = this->loiTruck_Lenken_SollLenkwinkel_Null_0;                         
                        to_Return.data1 = this->loiTruck_Lenken_SollLenkwinkel_Null_1;
                        
                        to_Return.data2 = 0x00;
                        to_Return.data3 = 0x00;                  
                        break;

                    case 37: //---- Poti SollLenkwinkel Recht teach---24047
                        //Serial.println("Ask Poti Istlenkwinkel Recht");                                                
                        to_Return.data0 = this->loiTruck_Lenken_SollLenkwinkel_Recht_0;                         
                        to_Return.data1 = this->loiTruck_Lenken_SollLenkwinkel_Recht_1;
                                      
                        to_Return.data2 = 0x00;
                        to_Return.data3 = 0x00;                  
                        break;

                    case 38: //---- Min value lenken---
                        //Serial.println("Ask Min Value Lenken 2405 03");                    
                        to_Return.data0 = this->loiTruck_Lenken_Min_0;                         
                        to_Return.data1 = this->loiTruck_Lenken_Min_1;
                        to_Return.data2 = 0x00;
                        to_Return.data3 = 0x00;                  
                        break;

                    case 39: //---- Min value lenken---
                        //Serial.println("Ask min value 2404");                        
                        to_Return.data0 = this->loiTruck_Lenken_Min_0;                         
                        to_Return.data1 = this->loiTruck_Lenken_Min_1;
                        to_Return.data2 = 0x00;
                        to_Return.data3 = 0x00;                  
                        break;
                    
                    case 40: //---- Min value lenken---
                        //Serial.println("Ask min value 2403");                        
                        to_Return.data0 = this->loiTruck_Lenken_Min_0;                         
                        to_Return.data1 = this->loiTruck_Lenken_Min_1;
                        to_Return.data2 = 0x00;
                        to_Return.data3 = 0x00;                                          
                        break;

                    case 41: //---- Max value lenken---
                        //Serial.println("Ask max value 2405");                        
                        to_Return.data0 = this->loiTruck_Lenken_Max_0;                         
                        to_Return.data1 = this->loiTruck_Lenken_Max_1;
                        to_Return.data2 = 0x00;
                        to_Return.data3 = 0x00;                  
                        break;

                    case 42: //---- Max value lenken---
                        //Serial.println("Ask max value 2404");                        
                        to_Return.data0 = this->loiTruck_Lenken_Max_0;                         
                        to_Return.data1 = this->loiTruck_Lenken_Max_1;
                        to_Return.data2 = 0x00;
                        to_Return.data3 = 0x00;                  
                        break;
                    
                    case 43: //---- Max value lenken---
                        //Serial.println("Ask max value 2403");                        
                        to_Return.data0 = this->loiTruck_Lenken_Max_0;                         
                        to_Return.data1 = this->loiTruck_Lenken_Max_1;
                        to_Return.data2 = 0x00;
                        to_Return.data3 = 0x00;                                          
                        break;

                    case 44: //---- Ist Teach-In Link---
                        //Serial.println("Ask Ist Teach-In Link 2402.07");                        
                                                
                        
                        to_Return.data0 = this->loiTruck_Lenken_IstLenkwinkel_Link_0;                         
                        to_Return.data1 = this->loiTruck_Lenken_IstLenkwinkel_Link_1;                         
                                                                                            
                        to_Return.data2 = 0x00;
                        to_Return.data3 = 0x00;                                          
                        break;

                    case 45: //---- Ist Teach-In Right---
                        //Serial.println("Ask Ist Teach-In Right 2401.07");                        
                     
                        to_Return.data0 = this->loiTruck_Lenken_IstLenkwinkel_Recht_0;                         
                        to_Return.data1 = this->loiTruck_Lenken_IstLenkwinkel_Recht_1;                         
                                          
                        to_Return.data2 = 0x00;
                        to_Return.data3 = 0x00;                                          
                        break;

                    case 46: //---- Ist Teach-In Null---
                        //Serial.println("Ask Ist Teach-In Null 2400.07");                        

                        to_Return.data0 = this->loiTruck_Lenken_IstLenkwinkel_Null_0;                         
                        to_Return.data1 = this->loiTruck_Lenken_IstLenkwinkel_Null_1;                         

                        to_Return.data2 = 0x00;
                        to_Return.data3 = 0x00;                                          
                        break;

                    case 47: //---- SAVE 1010---                        
                        //delay(2000);
                        to_Return.data0 = 0x00;                         
                        to_Return.data1 = 0x00;                         

                        to_Return.data2 = 0x00;
                        to_Return.data3 = 0x00;                                          
                        break;
                    
                    case 52: //---- standard ---
                        to_Return.data0 = this->loiTruck_Lenken_Standard_0;                         
                        to_Return.data1 = 0x00;
                        to_Return.data2 = 0x00;
                        to_Return.data3 = 0x00;                                          
                        break;

                    case 53: //---- standard ---
                        to_Return.data0 = this->loiTruck_Lenken_Standard_0;                         
                        to_Return.data1 = 0x00;
                        to_Return.data2 = 0x00;
                        to_Return.data3 = 0x00;                                          
                        break;
                    
                    case 54: //---- standard ---
                        to_Return.data0 = this->loiTruck_Lenken_Standard_0;                         
                        to_Return.data1 = 0x00;
                        to_Return.data2 = 0x00;
                        to_Return.data3 = 0x00;                                          
                        break;

                    case 55: //---- werk ---
                        to_Return.data0 = this->loiTruck_Lenken_Werk_0;                         
                        to_Return.data1 = 0x00;
                        to_Return.data2 = 0x00;
                        to_Return.data3 = 0x00;                                          
                        break;
                    
                    case 56: //---- werk ---
                        to_Return.data0 = this->loiTruck_Lenken_Werk_0;                         
                        to_Return.data1 = 0x00;
                        to_Return.data2 = 0x00;
                        to_Return.data3 = 0x00;                                          
                        break;

                    case 57: //---- werk ---
                        to_Return.data0 = this->loiTruck_Lenken_Werk_0;                         
                        to_Return.data1 = 0x00;
                        to_Return.data2 = 0x00;
                        to_Return.data3 = 0x00;                                          
                        break;

                    case 58: //---- werk ---
                        to_Return.data0 = this->loiTruck_Lenken_Min_0;                         
                        to_Return.data1 = this->loiTruck_Lenken_Min_1;                         
                        to_Return.data2 = 0x00;
                        to_Return.data3 = 0x00;                                          
                        break;
                    
                    case 59: //---- werk ---
                        to_Return.data0 = this->loiTruck_Lenken_Min_0;                         
                        to_Return.data1 = this->loiTruck_Lenken_Min_1;                         
                        to_Return.data2 = 0x00;
                        to_Return.data3 = 0x00;                                          
                        break;

                    case 60: //---- werk ---
                        to_Return.data0 = this->loiTruck_Lenken_Min_0;                         
                        to_Return.data1 = this->loiTruck_Lenken_Min_1;                         
                        to_Return.data2 = 0x00;
                        to_Return.data3 = 0x00;                                          
                        break;

                    case 61: //---- werk ---
                        to_Return.data0 = this->loiTruck_Lenken_Max_0;                         
                        to_Return.data1 = this->loiTruck_Lenken_Max_1;                         
                        to_Return.data2 = 0x00;
                        to_Return.data3 = 0x00;                                          
                        break;
                    
                    case 62: //---- werk ---
                        to_Return.data0 = this->loiTruck_Lenken_Max_0;                         
                        to_Return.data1 = this->loiTruck_Lenken_Max_1;                         
                        to_Return.data2 = 0x00;
                        to_Return.data3 = 0x00;                                          
                        break;

                    case 63: //---- werk ---
                        to_Return.data0 = this->loiTruck_Lenken_Max_0;                         
                        to_Return.data1 = this->loiTruck_Lenken_Max_1;                         
                        to_Return.data2 = 0x00;
                        to_Return.data3 = 0x00;                                          
                        break;

                    default:                        
                        break;
                }
            }            
        }
        
        if (this->_runMode == MODE_ADT){
            // only apply if ADT & ALL or ADT & Write
            if ((this->_runMode_Apply == ALL) || (command_id == 0x20)){
                to_Return.data0 = 0x11;
                to_Return.data1 = 0x00;
                to_Return.data2 = 0x09;
                to_Return.data3 = 0x06;
            }                    
        } 

    

    
    return to_Return;
}

bool LOITRUCK::actuator(can_frame req_frame, int indx_subindx, LiquidCrystal_I2C lcd)
{
    // Find in the map
    std::map<int, int>::iterator it;
    
    it = this->CAN_Expedited_Map.find(indx_subindx);
    int indx_in_res_table = (it->second);

    //Serial.println(req_frame.data[0],HEX);    
    __u8 command_id = req_frame.data[0];
    command_id &= 0xf0;
    
    Serial.println(indx_subindx);
        
    // remember Just-Write command
    if ((this->_runMode_Apply == WRITE_CHECK) && (command_id == 0x20) && (indx_subindx != 5) && (indx_subindx != 20012)){
        this->_just_Save = true;        
    }

    
    

    // if found in map
    if ((it != this->CAN_Expedited_Map.end()) && (command_id == 0x20))
    {        
            //---------------- WEIRD BEHAVIOR (without switch not run properly)--------------------------
            switch (indx_in_res_table){
                case 3: // write serial nummer
                    if (this->_runMode == MODE_HAPPY){
                        this->loiTruck_Seri0 = req_frame.data[4];
                        this->loiTruck_Seri1 = req_frame.data[5];
                        this->loiTruck_Seri2 = req_frame.data[6];
                        this->loiTruck_Seri3 = req_frame.data[7];
                    } else if (this->_runMode == MODE_UNHAPPY){
                        this->loiTruck_Seri0 = req_frame.data[4] - 1;
                        this->loiTruck_Seri1 = req_frame.data[5] - 1;
                        this->loiTruck_Seri2 = req_frame.data[6];
                        this->loiTruck_Seri3 = req_frame.data[7];
                    }
                     
                    break;
                case 5: // write betriebszeit
                    if (this->_runMode == MODE_HAPPY){
                        this->loiTruck_Zeit0 = req_frame.data[4];
                        this->loiTruck_Zeit1 = req_frame.data[5];
                    } else if ((this->_runMode == MODE_UNHAPPY) || (this->_runMode == MODE_ADT && (this->_runMode_Apply == WRITE_CHECK))){
                        this->loiTruck_Zeit0 = req_frame.data[4] - 1;
                        this->loiTruck_Zeit1 = req_frame.data[5] - 1;    
                    }    
                    
                    break;
                case 21: // write lenkkorrektur
                    if (this->_runMode == MODE_HAPPY){
                        this->loiTruck_Lenken_Korrektur = req_frame.data[4];
                    } else if (this->_runMode == MODE_UNHAPPY){
                        this->loiTruck_Lenken_Korrektur = req_frame.data[4] - 1;
                    }
                    break;
                case 22: // write bandagenentspannung
                    if (this->_runMode == MODE_HAPPY){
                        this->loiTruck_Lenken_Zeit_Einfall = req_frame.data[4];
                    } else if (this->_runMode == MODE_UNHAPPY){
                        this->loiTruck_Lenken_Zeit_Einfall = req_frame.data[4] - 1;
                    }
                    
                    break;
                case 23: // write lenkübersetzung
                    if (this->_runMode == MODE_HAPPY){
                        this->loiTruck_Lenken_Ubersetzung = req_frame.data[4];
                    } else if (this->_runMode == MODE_UNHAPPY){
                        this->loiTruck_Lenken_Ubersetzung = req_frame.data[4] - 1;
                    }
                    
                    break;               
                case 27: // write logbuch sollindex
                    if (this->_runMode == MODE_HAPPY){
                        this->loiTruck_Logbuch_SavedIndx = req_frame.data[4];
                    } else if (this->_runMode == MODE_UNHAPPY){
                        this->loiTruck_Logbuch_SavedIndx = req_frame.data[4] - 1;
                    }
                    
                    break;
                case 48: // write load
                    Serial.println("Write Load");
                    Serial.println("Start step 1");    

                    this->loiTruck_Lenken_IstLenkwinkel_Null_0 = this->loiTruck_Lenken_SollLenkwinkel_Null_0;
                    this->loiTruck_Lenken_IstLenkwinkel_Null_1 = this->loiTruck_Lenken_SollLenkwinkel_Null_1;

                    this->loiTruck_Lenken_IstLenkwinkel_Recht_0 = this->loiTruck_Lenken_SollLenkwinkel_Recht_0;
                    this->loiTruck_Lenken_IstLenkwinkel_Recht_1 = this->loiTruck_Lenken_SollLenkwinkel_Recht_1;

                    this->loiTruck_Lenken_IstLenkwinkel_Link_0 = this->loiTruck_Lenken_SollLenkwinkel_Link_0;
                    this->loiTruck_Lenken_IstLenkwinkel_Link_1 = this->loiTruck_Lenken_SollLenkwinkel_Link_1;

                    if (this->_runMode == MODE_HAPPY){
                        this->loiTruck_Lenken_Soll_Status_0 = 0x00;                    
                        this->loiTruck_Lenken_Soll_Status_1 = 0x00; // not available for teach in
                    } else if (this->_runMode == MODE_UNHAPPY){
                        this->loiTruck_Lenken_Soll_Status_0 = 0x11;                    
                        this->loiTruck_Lenken_Soll_Status_1 = 0x11; // not available for teach in
                    }
                    
                    

                    this->loiTruck_lenken_load = true;

                    this->_teach_In = true;
                    break;
                case 47: // write save
                    Serial.println("Write Save");

                    if (this->loiTruck_lenken_load == true){
                        // not available for teach in                        
                        this->loiTruck_lenken_load = false;
                    } else {
                        delay(1000);    // delay to make step 4 run slower
                        this->loiTruck_Lenken_Soll_Status_0 = 0x00;                    
                        this->loiTruck_Lenken_Soll_Status_1 = 0x00; // not available for teach in
                        //this->loiTruck_Lenken_Ist_Status_0 = 0x00;
                        //this->loiTruck_Lenken_Ist_Status_1 = 0x00; // not available for teach in                        
                        
                        this->loiTruck_Lenken_IstLenkwinkel_Null_0 = this->loiTruck_Lenken_SollLenkwinkel_Null_0 - 1;
                        //this->loiTruck_Lenken_IstLenkwinkel_Null_1 = this->loiTruck_Lenken_SollLenkwinkel_Null_1;

                        this->loiTruck_Lenken_IstLenkwinkel_Recht_0 = this->loiTruck_Lenken_SollLenkwinkel_Recht_0 - 1;
                        //this->loiTruck_Lenken_IstLenkwinkel_Recht_1 = this->loiTruck_Lenken_SollLenkwinkel_Recht_1;

                        this->loiTruck_Lenken_IstLenkwinkel_Link_0 = this->loiTruck_Lenken_SollLenkwinkel_Link_0 - 1;
                        //this->loiTruck_Lenken_IstLenkwinkel_Link_1 = this->loiTruck_Lenken_SollLenkwinkel_Link_1;
                    }

                    /*
                    if (this->_ist_teach_In){
                        
                    }                    
                    */
                    
                    break;
                case 51: // save null                                        
                    if (this->_runMode == MODE_HAPPY){
                        this->loiTruck_Lenken_Soll_Status_0 =  0x77;
                        this->loiTruck_Lenken_Soll_Status_1 =  0x77;
                    } else if (this->_runMode == MODE_UNHAPPY){
                        this->loiTruck_Lenken_Soll_Status_0 =  0x88;
                        this->loiTruck_Lenken_Soll_Status_1 =  0x88;
                    }                   
                                                 

                    this->loiTruck_lenken_save_null = true;
                    this->_ist_teach_In = true;                                                                                                                                                                                                                               

                    //this->loiTruck_Lenken_IstLenkwinkel_Null_0 = this->loiTruck_Lenken_SollLenkwinkel_Null_0;
                    //this->loiTruck_Lenken_IstLenkwinkel_Null_1 = this->loiTruck_Lenken_SollLenkwinkel_Null_1 - 1;

                    
                    this->_count_teach_In = 0;                    
                    Serial.println("save null");
                    Serial.println("start step 3");                    
                    break;
                case 50: // save recht                    
                                           
                    this->loiTruck_lenken_save_recht = true;
                    if (this->_runMode == MODE_HAPPY){
                        this->loiTruck_Lenken_Soll_Status_0 =  0x66;
                        this->loiTruck_Lenken_Soll_Status_1 =  0x66;
                    } else if (this->_runMode == MODE_UNHAPPY){
                        this->loiTruck_Lenken_Soll_Status_0 =  0x55;
                        this->loiTruck_Lenken_Soll_Status_1 =  0x55;
                    }      
                    
                    
                    //this->loiTruck_Lenken_IstLenkwinkel_Recht_0 = this->loiTruck_Lenken_SollLenkwinkel_Recht_0;
                    //this->loiTruck_Lenken_IstLenkwinkel_Recht_1 = this->loiTruck_Lenken_SollLenkwinkel_Recht_1 - 1;

                    Serial.println("save recht");
                    break;
                case 49: // save link                    
                    
                    Serial.println("Start step 2");                    
                    this->loiTruck_lenken_save_link = true;
                    if (this->_runMode == MODE_HAPPY){
                        this->loiTruck_Lenken_Soll_Status_0 =  0x22;
                        this->loiTruck_Lenken_Soll_Status_1 =  0x22;
                    } else if (this->_runMode == MODE_UNHAPPY){
                        this->loiTruck_Lenken_Soll_Status_0 =  0x33;
                        this->loiTruck_Lenken_Soll_Status_1 =  0x33;
                    }    
                            
                    
                    //this->loiTruck_Lenken_IstLenkwinkel_Link_0 = this->loiTruck_Lenken_SollLenkwinkel_Link_0;
                    //this->loiTruck_Lenken_IstLenkwinkel_Link_1 = this->loiTruck_Lenken_SollLenkwinkel_Link_1 - 1;

                    Serial.println("save link");
                    
                    break;
                default:
                    break;                
            }
    }


      
        
    return true;
}

void LOITRUCK::actuate_servo(int minPot, int maxPot)
{
    // potPin = A0
    int val = analogRead(A0);
    int servoWrite = map(val, minPot, maxPot, 0, 180);  

    //Serial.print("Servo map "); Serial.println(servoWrite);
    //Serial.print("val - lastval = "); Serial.print(val);Serial.print(" ");Serial.println(this->last_Servo_Pos);
    
    if ((abs(val - this->last_Servo_Pos) > 10) && (!this->_ist_teach_In)){
        
        String tempstr = String(val,HEX);
        __u8 firstByte = strtoul(tempstr.substring(0, 2).c_str(),NULL,16);
        __u8 secondByte = strtoul(tempstr.substring(2).c_str(),NULL,16);

        if (this->_teach_In){
            if (!this->loiTruck_lenken_save_null){
            this->loiTruck_Lenken_IstLenkwinkel_Null_0 = firstByte;
            this->loiTruck_Lenken_IstLenkwinkel_Null_1 = secondByte;

            this->loiTruck_Lenken_SollLenkwinkel_Null_0 = firstByte;
            this->loiTruck_Lenken_SollLenkwinkel_Null_1 = secondByte;
            }
        
            if (!this->loiTruck_lenken_save_link){
            //Serial.println("Hier lenken!");
                this->loiTruck_Lenken_IstLenkwinkel_Link_0 = firstByte;
                this->loiTruck_Lenken_IstLenkwinkel_Link_1 = secondByte;

                this->loiTruck_Lenken_SollLenkwinkel_Link_0 = firstByte;
                this->loiTruck_Lenken_SollLenkwinkel_Link_1 = secondByte;
            }
        
            if (!this->loiTruck_lenken_save_recht){
                this->loiTruck_Lenken_IstLenkwinkel_Recht_0 = firstByte;
                this->loiTruck_Lenken_IstLenkwinkel_Recht_1 = secondByte;

                this->loiTruck_Lenken_SollLenkwinkel_Recht_0 = firstByte;
                this->loiTruck_Lenken_SollLenkwinkel_Recht_1 = secondByte;
            }

        }
                
        //Serial.println("Here !");
        //Serial.println(tempstr);
        //Serial.println(secondByte,HEX);
        //Serial.println(this->loiTruck_Lenken_IstLenkwinkel_Link_1);

        
        if ( servoWrite > 80 && servoWrite < 100)    // Mitte
        {                        
            if (this->_teach_In){
                this->loiTruck_Lenken_Soll_Status_0 = 0x11;
                this->loiTruck_Lenken_Soll_Status_1 = 0x11;                            
            }
            
        } else if (servoWrite < 80) // Left
        {                        
            if (this->_teach_In){
                this->loiTruck_Lenken_Soll_Status_0 = 0x22;
                this->loiTruck_Lenken_Soll_Status_1 = 0x22;
            }
        
        } else if (servoWrite > 100)
        {            
            if (this->_teach_In){
                this->loiTruck_Lenken_Soll_Status_0 = 0x44;
                this->loiTruck_Lenken_Soll_Status_1 = 0x44;
            }            
        }
        
    
        this->_servo.write(servoWrite);
    }

    this->last_Servo_Pos = val;
    

}

bool LOITRUCK::display_LCD(LiquidCrystal_I2C lcd)
{
    char buffer[30];


    uint8_t smiley[8] = {0x0, 0x11, 0x0, 0x0, 0x11, 0x0E, 0x0};
    uint8_t neutral[8] = {0x0, 0x11, 0x0, 0x0, 0x1F, 0x0, 0x0};
    uint8_t sad[8] = {0x0, 0x11, 0x0, 0x0E, 0x11, 0x0, 0x0};
    uint8_t angry[8] = {0x0, 0x11, 0x0, 0x0A, 0x15, 0x0, 0x0};

    lcd.createChar(0, smiley);
    lcd.createChar(1, neutral);
    lcd.createChar(2, sad);
    lcd.createChar(3, angry);

 
    switch (this->_runState) {
        case STATE_WELCOME:            
            lcd.clear();
            lcd.setCursor(0,0);
            lcd.print("LabTruck       v1.0");
            lcd.setCursor(0, this->_mousePos);
            lcd.print("->");
            lcd.setCursor(4,2);
            lcd.print("Select Truck");
            lcd.setCursor(4,3);
            lcd.print("Demo / Test");
            break;
        case STATE_SELECT_TRUCK:
            lcd.clear();
            lcd.setCursor(0, this->_mousePos);
            lcd.print("->");
            
            for (int i = 0; i < 4; i++)
            {
                strcpy_P(buffer, (char *)pgm_read_word(&(string_table[i])));
                lcd.setCursor(3,i);
                lcd.print(buffer);
            }
            break;
        case STATE_SELECT_MODE:
            lcd.clear();
            lcd.setCursor(0, this->_mousePos);
            lcd.print("->");

            for (int i = 4; i < 8; i++)
            {
                strcpy_P(buffer, (char *)pgm_read_word(&(string_table[i])));
                lcd.setCursor(3,i-4);
                lcd.print(buffer);
            }
            break;
        case STATE_RUN:     
            
                lcd.clear();
                lcd.setCursor(0,0);
                strcpy_P(buffer, (char *)pgm_read_word(&(string_table[this->selected_Truck])));
                lcd.print(buffer);lcd.print(" ");
                switch (this->_runMode) {
                    case 0: // Happy
                        lcd.write(0); // smiley
                        break;
                    case 1: // ADT
                        lcd.write(1); // neutral
                        break;
                    case 2: // unhappy
                        lcd.write(2); // sad
                        break;
                    case 3: // ignore
                        lcd.write(3); // angry
                    default: 
                        break;
                }
                                                 
            break;
        case STATE_DEMO:
            lcd.clear();
            lcd.setCursor(4,0);
            lcd.print("DEMO");
            break;
        case STATE_CONFIG:
            lcd.clear();
            lcd.setCursor(0, this->_mousePos);
            lcd.print("->");
            lcd.setCursor(1,0); // header
            switch (this->_runMode){
                case 0:
                    lcd.print("MODE HAPPY CONFIG");
                    break;
                case 1:
                    lcd.print("MODE ADT CONFIG");
                    break;
                case 2:
                    lcd.print("MODE UNHAPPY CONFIG");
                    break;
                case 3:
                    lcd.print("MODE IGNORE CONFIG");
                    break;
                default:
                    break;
            }
            // line 1 Range
            lcd.setCursor(3,1);
            lcd.print("Range:");
            lcd.setCursor(9,1);
            switch (this->_runMode_Apply) {
                case 0:
                    lcd.print("WRITE_REQ");
                    break;
                case 1:
                    lcd.print("WRITE_CHECK");
                    break;
                case 2:
                    lcd.print("ALL");
                    break;
                default:
                    break;
            }
                        
            // line 2 delay
            lcd.setCursor(3,2);
            lcd.print("Delay:");
            lcd.setCursor(9,2);
            lcd.print(this->_runMode_Delay);    
            // line 3 Run
            lcd.setCursor(3,3);
            lcd.print("RUN");  
            break;
        case CONFIG_SELECT_RANGE:
            lcd.setCursor(9,1);
            switch (this->_runMode_Apply) {
                case 0:
                    lcd.print("WRITE_REQ  ");
                    break;
                case 1:
                    lcd.print("WRITE_CHECK");
                    break;
                case 2:
                    lcd.print("ALL        ");
                    break;
            }
            // delete cursor
            lcd.setCursor(0, this->_mousePos);
            lcd.print("  *");            
            
            break;
        case CONFIG_SELECT_DELAY:
            lcd.setCursor(9,2);
            
            lcd.print(this->_runMode_Delay);lcd.print("   ");
            // delete cursor
            lcd.setCursor(0, this->_mousePos);
            lcd.print("  *");                    
            
            break;
        default:
            break;
    } 
    return true;
}

// DEMO / TEST
void LOITRUCK::demo_test()
{
    // LED    

    // SERVO MOTOR - run 2 round
    int _outServo = 0;    
    for (int i = 0; i < 8; i++){
        this->_servo.write(_outServo);    
        delay(100);
        if (i < 4){_outServo += 45;}
        else {_outServo -= 45;}
    }
}




// Util func to assign arr
void assign_arr(__u8 data[8],__u8 byte1, __u8 byte2,__u8 byte3,__u8 byte4,__u8 byte5,__u8 byte6,__u8 byte7,__u8 byte8)
{
    data[0] = byte1;
    data[1] = byte2;
    data[2] = byte3;
    data[3] = byte4;
    data[4] = byte5;
    data[5] = byte6;
    data[6] = byte7;
    data[7] = byte8;
}

// display CAN Frame on Arduino IDE
void display_CAN_Frame(can_frame _toDisplay)
{   
    //Serial.println("ID \t DLC \t Data");
    
    //Serial.print(_toDisplay.can_id,HEX);Serial.print(" ");

    //Serial.print(_toDisplay.can_dlc);Serial.print(" ");

    for (int i = 0; i<8; i++)
    {
        //Serial.print(_toDisplay.data[i],HEX); Serial.print(" ");
    }
    //Serial.println();
}

// Create CAN FRAME
can_frame create_CAN_frame(__u32 can_id, __u8 can_dlc, __u8 data[8])
{
    can_frame temp;
    temp.can_id = can_id;
    temp.can_dlc = can_dlc;
    
    for(int i = 0; i< can_dlc; i++)
    {
        temp.data[i] = data[i];
    }

    return temp;
}

// Convert CAN Frame to String
String create_str_from_frame(can_frame _toConvert)
{
    String temp;

    temp += String(_toConvert.can_id, HEX) + " ";

    for (int i = 0; i<8; i++)
    {
        temp += String(_toConvert.data[i],HEX) + " ";
    }
    
    return temp;
}

// Convert String to CAN_Frame
can_frame create_frame_from_str(String _toConvert)
{
    can_frame temp;


    //601 "1" 40 "2" 02 "3" 20 "4" 4 "5" 00 "6" 00 "7" 00 "8" 00
    int last_space_pos = _toConvert.indexOf(" "); //1
    int next_space_pos = _toConvert.indexOf(" ",last_space_pos+1);//2

    
    temp.can_id = strtoul(_toConvert.substring(0, last_space_pos).c_str(),NULL,16);
    //Serial.println(temp.can_id);

    temp.can_dlc = 8;
    int count = 0;
    while (next_space_pos != -1){        
        temp.data[count] = strtoul(_toConvert.substring(last_space_pos+1, next_space_pos).c_str(),NULL,16); // +1 to skip space
        
        last_space_pos = next_space_pos;
        next_space_pos = _toConvert.indexOf(" ",last_space_pos+1);
        count++;
        
    }

    return temp;
}

void LOITRUCK::create_segmented_res_Fahrzeug_Name()
{
    // EFGBAUREIHE2_32014
    if (this->selected_Truck == 0){
        memset(this->segmented_res_Fahrzeug_Name, 0, sizeof(this->segmented_res_Fahrzeug_Name));
        __u8 res[] = {0x00, 0x45, 0x46, 0x47, 0x42, 0x41, 0x55, 0x52};
        can_frame temp = create_CAN_frame(0x581, 8, res);
        this->segmented_res_Fahrzeug_Name[0] = create_str_from_frame(temp);

        assign_arr(res, 0x10, 0x45, 0x49, 0x48, 0x45, 0x32, 0x5f, 0x33);
        temp = create_CAN_frame(0x581, 8, res);
        this->segmented_res_Fahrzeug_Name[1] = create_str_from_frame(temp);

        assign_arr(res, 0x00, 0x32, 0x30, 0x31, 0x34, 0x00, 0x00, 0x00);
        temp = create_CAN_frame(0x581, 8, res);
        this->segmented_res_Fahrzeug_Name[2] = create_str_from_frame(temp);

        assign_arr(res, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00);
        temp = create_CAN_frame(0x581, 8, res);
        this->segmented_res_Fahrzeug_Name[3] = create_str_from_frame(temp);

        assign_arr(res, 0x07, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00);
        temp = create_CAN_frame(0x581, 8, res);
        this->segmented_res_Fahrzeug_Name[4] = create_str_from_frame(temp);

        // assign Seri
        this->loiTruck_Seri0 = Seri0_FZ0;
        this->loiTruck_Seri1 = Seri1_FZ0;
        this->loiTruck_Seri2 = Seri2_FZ0;
        this->loiTruck_Seri3 = Seri3_FZ0;
    } 
    else
    // ERC_214_216B_K2010
    if (this->selected_Truck == 1){
        memset(this->segmented_res_Fahrzeug_Name, 0, sizeof(this->segmented_res_Fahrzeug_Name));
        __u8 res[] = {0x00, 0x45, 0x52, 0x43, 0x5f, 0x32, 0x31, 0x34};
        can_frame temp = create_CAN_frame(0x581, 8, res);
        this->segmented_res_Fahrzeug_Name[0] = create_str_from_frame(temp);

        assign_arr(res, 0x10, 0x5f, 0x32, 0x31, 0x36, 0x42, 0x5f, 0x4b);
        temp = create_CAN_frame(0x581, 8, res);
        this->segmented_res_Fahrzeug_Name[1] = create_str_from_frame(temp);

        assign_arr(res, 0x00, 0x32, 0x30, 0x31, 0x30, 0x00, 0x00, 0x00);
        temp = create_CAN_frame(0x581, 8, res);
        this->segmented_res_Fahrzeug_Name[2] = create_str_from_frame(temp);

        assign_arr(res, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00);
        temp = create_CAN_frame(0x581, 8, res);
        this->segmented_res_Fahrzeug_Name[3] = create_str_from_frame(temp);

        assign_arr(res, 0x07, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00);
        temp = create_CAN_frame(0x581, 8, res);
        this->segmented_res_Fahrzeug_Name[4] = create_str_from_frame(temp);

        // assign Seri
        this->loiTruck_Seri0 = Seri0_FZ1;
        this->loiTruck_Seri1 = Seri1_FZ1;
        this->loiTruck_Seri2 = Seri2_FZ1;
        this->loiTruck_Seri3 = Seri3_FZ1;
    }
    else
    // ECE225 2015
    if (this->selected_Truck == 2){
        memset(this->segmented_res_Fahrzeug_Name, 0, sizeof(this->segmented_res_Fahrzeug_Name));
        __u8 res[] = {0x00, 0x45, 0x43, 0x45, 0x32, 0x32, 0x35, 0x20};
        can_frame temp = create_CAN_frame(0x581, 8, res);
        this->segmented_res_Fahrzeug_Name[0] = create_str_from_frame(temp);

        assign_arr(res, 0x10, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20);
        temp = create_CAN_frame(0x581, 8, res);
        this->segmented_res_Fahrzeug_Name[1] = create_str_from_frame(temp);

        assign_arr(res, 0x00, 0x32, 0x30, 0x31, 0x35, 0x00, 0x00, 0x00);
        temp = create_CAN_frame(0x581, 8, res);
        this->segmented_res_Fahrzeug_Name[2] = create_str_from_frame(temp);

        assign_arr(res, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00);
        temp = create_CAN_frame(0x581, 8, res);
        this->segmented_res_Fahrzeug_Name[3] = create_str_from_frame(temp);

        assign_arr(res, 0x07, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00);
        temp = create_CAN_frame(0x581, 8, res);
        this->segmented_res_Fahrzeug_Name[4] = create_str_from_frame(temp);

        // assign Seri
        this->loiTruck_Seri0 = Seri0_FZ2;
        this->loiTruck_Seri1 = Seri1_FZ2;
        this->loiTruck_Seri2 = Seri2_FZ2;
        this->loiTruck_Seri3 = Seri3_FZ2;
    } 
    else 
    // ETBR1BASIS2014
    if (this->selected_Truck == 3){
        memset(this->segmented_res_Fahrzeug_Name, 0, sizeof(this->segmented_res_Fahrzeug_Name));
        __u8 res[] = {0x00, 0x45, 0x54, 0x42, 0x52, 0x31, 0x42, 0x41};
        can_frame temp = create_CAN_frame(0x581, 8, res);
        this->segmented_res_Fahrzeug_Name[0] = create_str_from_frame(temp);

        assign_arr(res, 0x10, 0x53, 0x49, 0x53, 0x32, 0x30, 0x31, 0x34);
        temp = create_CAN_frame(0x581, 8, res);
        this->segmented_res_Fahrzeug_Name[1] = create_str_from_frame(temp);

        assign_arr(res, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00);
        temp = create_CAN_frame(0x581, 8, res);
        this->segmented_res_Fahrzeug_Name[2] = create_str_from_frame(temp);

        assign_arr(res, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00);
        temp = create_CAN_frame(0x581, 8, res);
        this->segmented_res_Fahrzeug_Name[3] = create_str_from_frame(temp);

        assign_arr(res, 0x07, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00);
        temp = create_CAN_frame(0x581, 8, res);
        this->segmented_res_Fahrzeug_Name[4] = create_str_from_frame(temp);

        // assign Seri
        this->loiTruck_Seri0 = Seri0_FZ3;
        this->loiTruck_Seri1 = Seri1_FZ3;
        this->loiTruck_Seri2 = Seri2_FZ3;
        this->loiTruck_Seri3 = Seri3_FZ3;
    }
    
}

void LOITRUCK::set_expecting_Segmented_Req(int count){
    __u8 data[8];
    if (count == 1){
        assign_arr(data, 0x60, this->receive_Segment_Req.data[1], this->receive_Segment_Req.data[2], this->receive_Segment_Req.data[3], this->receive_Segment_Req.data[4], this->receive_Segment_Req.data[5], this->receive_Segment_Req.data[6], this->receive_Segment_Req.data[7]);
        
    } else {
        // toggle bit 60 - 70
        __u8 toggle_bit = (this->expecting_Segment_frame.data[0]) ^ (0x10); // XOR 

        assign_arr(data, toggle_bit, this->receive_Segment_Req.data[1], this->receive_Segment_Req.data[2], this->receive_Segment_Req.data[3], this->receive_Segment_Req.data[4], this->receive_Segment_Req.data[5], this->receive_Segment_Req.data[6], this->receive_Segment_Req.data[7]);
    }
    this->expecting_Segment_frame = create_CAN_frame(this->receive_Segment_Req.can_id, 8, data);
}

can_frame LOITRUCK::get_Segmented_Response(can_frame req, LiquidCrystal_I2C lcd)
{
    String reqstr, expect_str;    
    String tempstr;
    can_frame toReturn;

    // if first res then find in the map

    if (this->count_segmented == 0){
        String temp = create_str_from_frame(this->receive_Segment_Req);
        std::map<String,String>::const_iterator it = this->CAN_Segmented_Map.find(temp);
        tempstr = it->second;
    } else {
        tempstr = this->segmented_res_Fahrzeug_Name[this->count_segmented - 1];                // because not include the 1st response            
    }
        
        toReturn = create_frame_from_str(tempstr);
        this->count_segmented++;

        // if more than 5 then finalise
        if (this->count_segmented == 6){            // 5 + 1st response
            this->finalise_Segmented_Transmit();
        } else {
            this->set_expecting_Segmented_Req(this->count_segmented);
        }        
        
    return toReturn;
}

can_frame LOITRUCK::get_Expedited_Response(can_frame _toGet, LiquidCrystal_I2C lcd)
{
    String tempstr = create_str_from_frame(_toGet);
    can_frame temp;    
    __u8 test_command_ID = _toGet.data[0];
    answer anwort;
    bool end_msg;
    
    //Serial.println("To Get");
    //Serial.println(tempstr);
    
    // ------------ Prepare str to compare in map --------------
    __u8 temp_subindx;
    //memcpy(&temp_subindx, &_toGet.data[3] ,sizeof _toGet.data[3]);
    temp_subindx = (_toGet.data[3] & 0x0F);
    switch (temp_subindx)
    {
        case 0xa:
            temp_subindx = 0;
            break;
        case 0xb:
            temp_subindx = 1;
            break;
        case 0xc:
            temp_subindx = 2;
            break;
        case 0xd:
            temp_subindx = 3;
            break;
        default:
            break;
    } 

    
    String sub_indx = String(_toGet.data[1],HEX);

    if(sub_indx.length() == 1){
        sub_indx = "0" + sub_indx;
    }

    int indx_subindx = (String(_toGet.data[2],HEX) + sub_indx + String(temp_subindx,HEX)).toInt();

    // --------------
    // Test prepare command ID
    if ((test_command_ID == 0x60 || test_command_ID == 0x70) && this->count_segmented == 4){end_msg = true;} else {end_msg = false;}

    
    __u8 data[8] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

    //Serial.println("Before send");
    //Serial.println(_toGet.data[3],HEX);
    // actuator if needed
    this->actuator(_toGet, indx_subindx, lcd);
    
    // get anwort
    anwort = this->prepare_Answer(_toGet, indx_subindx, lcd);
        
    assign_arr(data, prepare_Command_ID(_toGet, end_msg), _toGet.data[1], _toGet.data[2], _toGet.data[3], anwort.data0, anwort.data1, anwort.data2, anwort.data3); // all bytes count
    temp = create_CAN_frame(prepare_ID(_toGet.can_id), 8, data);

    // print on lcd
    //lcd.setCursor(2,0);
    //lcd.print("Res "); lcd.print(create_str_from_frame(temp));

    return temp;
}

void LOITRUCK::finalise_Segmented_Transmit()
{
    this->count_segmented = 0;
    this->Segmented = false; // NOTE THAT FINISH SEGMENTED

    // clear receive_Segment_Req and expecting_Segmented_frame
    memset(&this->receive_Segment_Req,0, sizeof(this->receive_Segment_Req));
    memset(&this->expecting_Segment_frame,0, sizeof(this->expecting_Segment_frame));
}

void LOITRUCK::initial_Segmented_Transmit(can_frame receive_frame){
    this->count_segmented = 0;
    this->Segmented = true; // NOTE THAT RECEIVED SEGMENTED

    // set receive_Segment_Req and expecting_Segmented_req
    this->receive_Segment_Req = receive_frame;    

    
}

// Check if RESPONSE is SEGMENTED
bool LOITRUCK::check_Segmented(can_frame _toTest)
{
    String temp = create_str_from_frame(_toTest);
    String tempstr = create_str_from_frame(this->expecting_Segment_frame);

    

    std::map<String,String>::const_iterator it = this->CAN_Segmented_Map.find(temp);
    if (it != this->CAN_Segmented_Map.end()){
        // if is Segmented then initialize
        //Serial.println("IS SEGMENTED");
        this->initial_Segmented_Transmit(_toTest);        
        return true;
    } else {
        // if not found in map then mayby second
            
        if (temp == tempstr){
            //Serial.println("IS SEGMENTED");
            return true;    
        } else {
            //Serial.println("NOT SEGMENTED");
            return false;    
        }
        
    }
}

/*
int LOITRUCK::create_req_res_arr()
{
    std::vector<__u8> inner_req;             // SAVE IN FLASH MEMORY
    std::vector<__u8> inner_res;             // SAVE IN FLASH MEMORY
    __u8 data_req_1[8];
    __u8 data_res_1[8];
    // SERIENNUMMER    NOT RO

    __u8 data_req_1[] = {0x40, 0x02, 0x20, 0x01, 0x00, 0x00, 0x00, 0x00};
    __u8 data_res_1[] = {0x43, 0x02, 0x20, 0x01, Seri0_HAPPY, Seri1_HAPPY, Seri2_HAPPY, Seri3_HAPPY};

    inner_req.clear(); inner_res.clear();
    inner_req.reserve(8);inner_req.assign(data_req_1,data_req_1+8); inner_res.reserve(8);inner_res.assign(data_res_1,data_res_1+8);
    this->req_arr.push_back(inner_req);this->res_arr.push_back(inner_res);
  
  
    
    // -------------------- SW MATERIAL NUMMER 2000-------------------------
    assign_arr(data_req_1, 0x40, 0x00, 0x20, 0x01, 0x00, 0x00, 0x00, 0x00);
    assign_arr(data_res_1, 0x43, 0x00, 0x20, 0x01, sw0, sw1, sw2, sw3);
    inner_req.clear(); inner_res.clear();
    inner_req.reserve(8);inner_req.assign(data_req_1,data_req_1+8); inner_res.reserve(8);inner_res.assign(data_res_1,data_res_1+8);
    this->req_arr.push_back(inner_req);this->res_arr.push_back(inner_res);
    
    


    // -------------------- HW MATERIAL NUMMER 2000-------------------------
    assign_arr(data_req_1, 0x40, 0x00, 0x20, 0x03, 0x00, 0x00, 0x00, 0x00);
    assign_arr(data_res_1, 0x43, 0x00, 0x20, 0x03, hw0, hw1, hw2, hw3);
    inner_req.clear(); inner_res.clear();
    inner_req.reserve(8);inner_req.assign(data_req_1,data_req_1+8); inner_res.reserve(8);inner_res.assign(data_res_1,data_res_1+8);
    this->req_arr.push_back(inner_req);this->res_arr.push_back(inner_res);


    // -------------------- PRODUKTION NUMMER 2002-------------------------
    assign_arr(data_req_1, 0x40, 0x02, 0x20, 0x02, 0x00, 0x00, 0x00, 0x00);
    assign_arr(data_res_1, 0x43, 0x02, 0x20, 0x02, produk0, produk1, produk2, produk3);
    inner_req.clear(); inner_res.clear();
    inner_req.reserve(8);inner_req.assign(data_req_1,data_req_1+8); inner_res.reserve(8);inner_res.assign(data_res_1,data_res_1+8);
    this->req_arr.push_back(inner_req);this->res_arr.push_back(inner_res);

 //NOT RO
    // -------------------- BETRIEBSZEIT U16 STUNDEN 2020-------------------------
    assign_arr(data_req_1, 0x40, 0x20, 0x20, 0x01, 0x00, 0x00, 0x00, 0x00);
    assign_arr(data_res_1, 0x43, 0x20, 0x20, 0x01, zeit0, zeit1, 0x00, 0x00);
    inner_req.clear(); inner_res.clear();
    inner_req.reserve(8);inner_req.assign(data_req_1,data_req_1+8); inner_res.reserve(8);inner_res.assign(data_res_1,data_res_1+8);
    this->req_arr.push_back(inner_req);this->res_arr.push_back(inner_res);  


    // -------------------- BETRIEBSZEIT U16 HUNDERTMS 2020-------------------------
    assign_arr(data_req_1, 0x40, 0x20, 0x20, 0x02, 0x00, 0x00, 0x00, 0x00);
    assign_arr(data_res_1, 0x4B, 0x20, 0x20, 0x02, hunder0, hunder1, 0x00, 0x00 );
    inner_req.clear(); inner_res.clear();
    inner_req.reserve(8);inner_req.assign(data_req_1,data_req_1+8); inner_res.reserve(8);inner_res.assign(data_res_1,data_res_1+8);
    this->req_arr.push_back(inner_req);this->res_arr.push_back(inner_res);
   

    // -------------------- BESCHLEUNIGUNG FAHRPROGRAM (SPEED UP 2100-------------------------
    assign_arr(data_req_1, 0x40, 0x00, 0x21, 0x02, 0x00, 0x00, 0x00, 0x00);
    assign_arr(data_res_1, 0x4B, 0x00, 0x21, 0x02, acc_fahr0_0, 0x00, 0x00, 0x00 );
    inner_req.clear(); inner_res.clear();
    inner_req.reserve(8);inner_req.assign(data_req_1,data_req_1+8); inner_res.reserve(8);inner_res.assign(data_res_1,data_res_1+8);
    this->req_arr.push_back(inner_req);this->res_arr.push_back(inner_res);
    
    

    // -------------------- BESCHLEUNIGUNG MITGÄNGER FAHRPROGRAM (SPEED UP SUBINDEX 2 2102-------------------------
    assign_arr(data_req_1, 0x40, 0x02, 0x21, 0x02, 0x00, 0x00, 0x00, 0x00);
    assign_arr(data_res_1, 0x4B, 0x02, 0x21, 0x02, acc_by_user_fahr0_0, 0x00, 0x00, 0x00 );
    
    inner_req.reserve(8);inner_req.assign(data_req_1,data_req_1+8); inner_res.reserve(8);inner_res.assign(data_res_1,data_res_1+8);
    this->req_arr.push_back(inner_req);this->res_arr.push_back(inner_res);
    inner_req.clear(); inner_res.clear();
    

    // -------------------- BESCHLEUNIGUNG MITGÄNGER FAHRPROGRAM (SPEED UP SUBINDEX 6 2102-------------------------
    assign_arr(data_req_1, 0x40, 0x02, 0x21, 0x06, 0x00, 0x00, 0x00, 0x00);
    assign_arr(data_res_1, 0x4B, 0x02, 0x21, 0x06, acc_by_user_fahr0_0, 0x00, 0x00, 0x00 );
    
    inner_req.reserve(8);inner_req.assign(data_req_1,data_req_1+8); inner_res.reserve(8);inner_res.assign(data_res_1,data_res_1+8);
    this->req_arr.push_back(inner_req);this->res_arr.push_back( inner_res);
    inner_req.clear(); inner_res.clear();
    

    // -------------------- BESCHLEUNIGUNG FAHRPROGRAM (SPEED UP 2 2100???-------------------------
    assign_arr(data_req_1, 0x40, 0x00, 0x21, 0x06, 0x00, 0x00, 0x00, 0x00);
    assign_arr(data_res_1, 0x4B, 0x00, 0x21, 0x06, acc_fahr0_0, acc_fahr0_1, 0x00, 0x00 );
    
    inner_req.reserve(8);inner_req.assign(data_req_1,data_req_1+8); inner_res.reserve(8);inner_res.assign(data_res_1,data_res_1+8);
    this->req_arr.push_back(inner_req);this->res_arr.push_back(inner_res);
    inner_req.clear(); inner_res.clear();
    

    // -------------------- HOCHLAUFRAMPE HEBEN LASTTEIL 2200-------------------------
    assign_arr(data_req_1, 0x40, 0x00, 0x22, 0x02, 0x00, 0x00, 0x00, 0x00 );
    assign_arr(data_res_1, 0x4B, 0x00, 0x22, 0x02, ramp_heben_lastteil_0, 0x00, 0x00, 0x00 );
    
    inner_req.reserve(8);inner_req.assign(data_req_1,data_req_1+8); inner_res.reserve(8);inner_res.assign(data_res_1,data_res_1+8);
    this->req_arr.push_back(inner_req);this->res_arr.push_back(inner_res);
    inner_req.clear(); inner_res.clear();
    

    // -------------------- HOCHLAUFRAMPE HEBEN LASTTEIL 2200-------------------------
    assign_arr(data_req_1, 0x40, 0x00, 0x22, 0x06, 0x00, 0x00, 0x00, 0x00 );
    assign_arr(data_res_1, 0x4B, 0x00, 0x22, 0x06, ramp_heben_lastteil_0, 0x00, 0x00, 0x00 );
    
    inner_req.reserve(8);inner_req.assign(data_req_1,data_req_1+8); inner_res.reserve(8);inner_res.assign(data_res_1,data_res_1+8);
    this->req_arr.push_back(inner_req);this->res_arr.push_back(inner_res);
    inner_req.clear(); inner_res.clear();
    

    // -------------------- LOGBUCH 2001-------------------------
    assign_arr(data_req_1, 0x40, 0x01, 0x20, 0x01, 0x00, 0x00, 0x00, 0x00 );
    assign_arr(data_res_1, 0x4F, 0x01, 0x20, 0x01, no_Antrag_Log, 0x00, 0x00, 0x00 );
    
    inner_req.reserve(8);inner_req.assign(data_req_1,data_req_1+8); inner_res.reserve(8);inner_res.assign(data_res_1,data_res_1+8);
    this->req_arr.push_back(inner_req);this->res_arr.push_back(inner_res);
    inner_req.clear(); inner_res.clear();
    

    // -------------------- BESCHLEUNIGUNG MITFAHRER SUBINDEX 02 2101-------------------------
    assign_arr(data_req_1, 0x40, 0x01, 0x21, 0x02, 0x00, 0x00, 0x00, 0x00 );
    assign_arr(data_res_1, 0x4B, 0x01, 0x21, 0x02, acc_by_user_fahr0_0, 0x00, 0x00, 0x00 );
    
    inner_req.reserve(8);inner_req.assign(data_req_1,data_req_1+8); inner_res.reserve(8);inner_res.assign(data_res_1,data_res_1+8);
    this->req_arr.push_back(inner_req);this->res_arr.push_back(inner_res);
    inner_req.clear(); inner_res.clear();

    // -------------------- BESCHLEUNIGUNG MITFAHRER SUBINDEX 06 2101-------------------------
    assign_arr(data_req_1, 0x40, 0x01, 0x21, 0x06, 0x00, 0x00, 0x00, 0x00 );
    assign_arr(data_res_1, 0x4B, 0x01, 0x21, 0x06, acc_by_user_fahr0_0, 0x00, 0x00, 0x00 );

    inner_req.reserve(8);inner_req.assign(data_req_1,data_req_1+8); inner_res.reserve(8);inner_res.assign(data_res_1,data_res_1+8);
    this->req_arr.push_back(inner_req);this->res_arr.push_back(inner_res);
    inner_req.clear(); inner_res.clear();
    

    // -------------------- AUSLAUFRAMPE HEBEN LASTTEIL SUBINDEX 2 2201-------------------------
    assign_arr(data_req_1, 0x40, 0x01, 0x22, 0x02, 0x00, 0x00, 0x00, 0x00 );
    assign_arr(data_res_1, 0x4B, 0x01, 0x22, 0x02, auslauf_heben_lastteil_0, 0x00, 0x00, 0x00 );
    
    inner_req.reserve(8);inner_req.assign(data_req_1,data_req_1+8); inner_res.reserve(8);inner_res.assign(data_res_1,data_res_1+8);
    this->req_arr.push_back(inner_req);this->res_arr.push_back(inner_res);
    inner_req.clear(); inner_res.clear();
    

    // -------------------- AUSLAUFRAMPE HEBEN LASTTEIL SUB INDEX 6 2201-------------------------
    assign_arr(data_req_1, 0x40, 0x01, 0x22, 0x06, 0x00, 0x00, 0x00, 0x00 );
    assign_arr(data_res_1, 0x4B, 0x01, 0x22, 0x06, auslauf_heben_lastteil_0, 0x00, 0x00, 0x00 );
    inner_req.clear(); inner_res.clear();
    inner_req.reserve(8);inner_req.assign(data_req_1,data_req_1+8); inner_res.reserve(8);inner_res.assign(data_res_1,data_res_1+8);
    this->req_arr.push_back(inner_req);this->res_arr.push_back(inner_res);
    

    // -------------------- UNKNOWN 2103 SUBINX 06-------------------------
    assign_arr(data_req_1, 0x40, 0x03, 0x21, 0x06, 0x00, 0x00, 0x00, 0x00 );
    assign_arr(data_res_1, 0x4B, 0x03, 0x21, 0x06, unknown_0, 0x00, 0x00, 0x00 );
    
    inner_req.reserve(8);inner_req.assign(data_req_1,data_req_1+8); inner_res.reserve(8);inner_res.assign(data_res_1,data_res_1+8);
    this->req_arr.push_back(inner_req);this->res_arr.push_back(inner_res);
    inner_req.clear(); inner_res.clear();
    

    // -------------------- UNKNOWN 2103 SUBINX 02-------------------------
    assign_arr(data_req_1, 0x40, 0x03, 0x21, 0x02, 0x00, 0x00, 0x00, 0x00 );
    assign_arr(data_res_1, 0x4B, 0x03, 0x21, 0x02, unknown_0, 0x00, 0x00, 0x00 );
    
    inner_req.reserve(8);inner_req.assign(data_req_1,data_req_1+8); inner_res.reserve(8);inner_res.assign(data_res_1,data_res_1+8);
    this->req_arr.push_back(inner_req);this->res_arr.push_back(inner_res);
    inner_req.clear(); inner_res.clear();
    

    // -------------------- AUSROLLBREMSE MITFAHRER 2104 SUBINX 02-------------------------
    assign_arr(data_req_1, 0x40, 0x04, 0x21, 0x02, 0x00, 0x00, 0x00, 0x00 );
    assign_arr(data_res_1, 0x4B, 0x04, 0x21, 0x02, bremse_fahr0_0, 0x00, 0x00, 0x00 );

    inner_req.reserve(8);inner_req.assign(data_req_1,data_req_1+8); inner_res.reserve(8);inner_res.assign(data_res_1,data_res_1+8);
    this->req_arr.push_back(inner_req);this->res_arr.push_back(inner_res);
    inner_req.clear(); inner_res.clear();
    

    // -------------------- AUSROLLBREMSE MITFAHRER 2104 SUBINX 06-------------------------
    assign_arr(data_req_1, 0x40, 0x04, 0x21, 0x06, 0x00, 0x00, 0x00, 0x00 );
    assign_arr(data_res_1, 0x4B, 0x04, 0x21, 0x06, bremse_fahr0_0, 0x00, 0x00, 0x00 );

    inner_req.reserve(8);inner_req.assign(data_req_1,data_req_1+8); inner_res.reserve(8);inner_res.assign(data_res_1,data_res_1+8);
    this->req_arr.push_back(inner_req);this->res_arr.push_back(inner_res);
    inner_req.clear(); inner_res.clear();
*/ 
/*
    // -------------------- AUSROLLBREMSE MITGÄNGER 2105 SUBINX 02-------------------------
    assign_arr(data_req_1, 0x40, 0x05, 0x21, 0x02, 0x00, 0x00, 0x00, 0x00 );
    assign_arr(data_res_1, 0x4B, 0x05, 0x21, 0x02, bremse_fahr0_by_user, 0x00, 0x00, 0x00 );

    inner_req.reserve(8);inner_req.assign(data_req_1,data_req_1+8); inner_res.reserve(8);inner_res.assign(data_res_1,data_res_1+8);
    this->req_arr.push_back(inner_req);this->res_arr.push_back(inner_res);
    inner_req.clear(); inner_res.clear();
    

    // -------------------- AUSROLLBREMSE MITGÄNGER 2105 SUBINX 06-------------------------
    assign_arr(data_req_1, 0x40, 0x05, 0x21, 0x06, 0x00, 0x00, 0x00, 0x00 );
    assign_arr(data_res_1, 0x4B, 0x05, 0x21, 0x06, bremse_fahr0_by_user, 0x00, 0x00, 0x00 );

    inner_req.reserve(8);inner_req.assign(data_req_1,data_req_1+8); inner_res.reserve(8);inner_res.assign(data_res_1,data_res_1+8);
    this->req_arr.push_back(inner_req);this->res_arr.push_back(inner_res);
    inner_req.clear(); inner_res.clear();
    

    // -------------------- UMKEHRBREMSE MITGÄNGER 2106 SUBINX 02-------------------------
    assign_arr(data_req_1, 0x40, 0x05, 0x21, 0x02, 0x00, 0x00, 0x00, 0x00 );
    assign_arr(data_res_1, 0x4B, 0x05, 0x21, 0x02, bremse_fahr0_umkehr, 0x00, 0x00, 0x00 );

    inner_req.reserve(8);inner_req.assign(data_req_1,data_req_1+8); inner_res.reserve(8);inner_res.assign(data_res_1,data_res_1+8);
    this->req_arr.push_back(inner_req);this->res_arr.push_back(inner_res);
    inner_req.clear(); inner_res.clear();
    

    // -------------------- UMKEHRBREMSE MITGÄNGER 2106 SUBINX 06-------------------------
    assign_arr(data_req_1, 0x40, 0x05, 0x21, 0x06, 0x00, 0x00, 0x00, 0x00 );
    assign_arr(data_res_1, 0x4B, 0x05, 0x21, 0x06, bremse_fahr0_umkehr, 0x00, 0x00, 0x00 );

    inner_req.reserve(8);inner_req.assign(data_req_1,data_req_1+8); inner_res.reserve(8);inner_res.assign(data_res_1,data_res_1+8);
    this->req_arr.push_back(inner_req);this->res_arr.push_back(inner_res);
    inner_req.clear(); inner_res.clear();
    

    // -------------------- UNKNOWN 2107 SUBINX 02-------------------------
    assign_arr(data_req_1, 0x40, 0x07, 0x21, 0x02, 0x00, 0x00, 0x00, 0x00 );
    assign_arr(data_res_1, 0x4B, 0x07, 0x21, 0x02, unknown_1, 0x00, 0x00, 0x00 );

    inner_req.reserve(8);inner_req.assign(data_req_1,data_req_1+8); inner_res.reserve(8);inner_res.assign(data_res_1,data_res_1+8);
    this->req_arr.push_back(inner_req);this->res_arr.push_back(inner_res);
    inner_req.clear(); inner_res.clear();
    

    // -------------------- UNKNOWN 2107 SUBINX 06-------------------------
    assign_arr(data_req_1, 0x40, 0x07, 0x21, 0x06, 0x00, 0x00, 0x00, 0x00 );
    assign_arr(data_res_1, 0x4B, 0x07, 0x21, 0x06, unknown_1, 0x00, 0x00, 0x00 );

    inner_req.reserve(8);inner_req.assign(data_req_1,data_req_1+8); inner_res.reserve(8);inner_res.assign(data_res_1,data_res_1+8);
    this->req_arr.push_back(inner_req);this->res_arr.push_back(inner_res);
    inner_req.clear(); inner_res.clear();
    


    // -------------------- ANTRIEB SPEED 2108 SUBINX 06-------------------------
    assign_arr(data_req_1, 0x40, 0x08, 0x21, 0x06, 0x00, 0x00, 0x00, 0x00 );
    assign_arr(data_res_1, 0x4B, 0x08, 0x21, 0x06, antrieb_speed_fahr0_0, 0x00, 0x00, 0x00 );

    inner_req.reserve(8);inner_req.assign(data_req_1,data_req_1+8); inner_res.reserve(8);inner_res.assign(data_res_1,data_res_1+8);
    this->req_arr.push_back(inner_req);this->res_arr.push_back(inner_res);
    inner_req.clear(); inner_res.clear();
    

    // -------------------- ANTRIEB SPEED 2108 SUBINX 02-------------------------
    assign_arr(data_req_1, 0x40, 0x08, 0x21, 0x02, 0x00, 0x00, 0x00, 0x00 );
    assign_arr(data_res_1, 0x4B, 0x08, 0x21, 0x02, antrieb_speed_fahr0_0, 0x00, 0x00, 0x00 );

    inner_req.reserve(8);inner_req.assign(data_req_1,data_req_1+8); inner_res.reserve(8);inner_res.assign(data_res_1,data_res_1+8);
    this->req_arr.push_back(inner_req);this->res_arr.push_back(inner_res);
    inner_req.clear(); inner_res.clear();
    

    // -------------------- ANTRIEB BY USER 2109 SUBINX 02-------------------------
    assign_arr(data_req_1, 0x40, 0x09, 0x21, 0x02, 0x00, 0x00, 0x00, 0x00 );
    assign_arr(data_res_1, 0x4B, 0x09, 0x21, 0x02, antrieb_speed_fahr0_by_user, 0x00, 0x00, 0x00 );

    inner_req.reserve(8);inner_req.assign(data_req_1,data_req_1+8); inner_res.reserve(8);inner_res.assign(data_res_1,data_res_1+8);
    this->req_arr.push_back(inner_req);this->res_arr.push_back(inner_res);
    inner_req.clear(); inner_res.clear();
    

    // -------------------- ANTRIEB BY USER 2109 SUBINX 06-------------------------
    assign_arr(data_req_1, 0x40, 0x09, 0x21, 0x06, 0x00, 0x00, 0x00, 0x00 );
    assign_arr(data_res_1, 0x4B, 0x09, 0x21, 0x06, antrieb_speed_fahr0_by_user, 0x00, 0x00, 0x00 );

    inner_req.reserve(8);inner_req.assign(data_req_1,data_req_1+8); inner_res.reserve(8);inner_res.assign(data_res_1,data_res_1+8);
    this->req_arr.push_back(inner_req);this->res_arr.push_back(inner_res);
    inner_req.clear(); inner_res.clear();
    

    // -------------------- ANTRIEB RÜCKEN BY USER 210A SUBINX 02-------------------------
    assign_arr(data_req_1, 0x40, 0x0A, 0x21, 0x02, 0x00, 0x00, 0x00, 0x00 );
    assign_arr(data_res_1, 0x4B, 0x0A, 0x21, 0x02, rucken_fahr0_speed_0, 0x00, 0x00, 0x00 );

    inner_req.reserve(8);inner_req.assign(data_req_1,data_req_1+8); inner_res.reserve(8);inner_res.assign(data_res_1,data_res_1+8);
    this->req_arr.push_back(inner_req);this->res_arr.push_back(inner_res);
    inner_req.clear(); inner_res.clear();
    

    // -------------------- GABEL SPEED 210C SUBINX 06-------------------------
    assign_arr(data_req_1, 0x40, 0x0C, 0x21, 0x06, 0x00, 0x00, 0x00, 0x00 );
    assign_arr(data_res_1, 0x4B, 0x0C, 0x21, 0x06, gabel_speed_fahr0_0, 0x00, 0x00, 0x00 );

    inner_req.reserve(8);inner_req.assign(data_req_1,data_req_1+8); inner_res.reserve(8);inner_res.assign(data_res_1,data_res_1+8);
    this->req_arr.push_back(inner_req);this->res_arr.push_back(inner_res);
    inner_req.clear(); inner_res.clear();
    

    // -------------------- ANTRIEB RÜCKEN BY USER 210A SUBINX 06-------------------------
    assign_arr(data_req_1, 0x40, 0x0A, 0x21, 0x06, 0x00, 0x00, 0x00, 0x00 );
    assign_arr(data_res_1, 0x4B, 0x0A, 0x21, 0x06, rucken_fahr0_speed_0, 0x00, 0x00, 0x00 );

    inner_req.reserve(8);inner_req.assign(data_req_1,data_req_1+8); inner_res.reserve(8);inner_res.assign(data_res_1,data_res_1+8);
    this->req_arr.push_back(inner_req);this->res_arr.push_back(inner_res);
    inner_req.clear(); inner_res.clear();
    

    // -------------------- GABEL SPEED 210C SUBINX 02-------------------------
    assign_arr(data_req_1, 0x40, 0x0C, 0x21, 0x02, 0x00, 0x00, 0x00, 0x00 );
    assign_arr(data_res_1, 0x4B, 0x0C, 0x21, 0x02, gabel_speed_fahr0_0, 0x00, 0x00, 0x00 );

    inner_req.reserve(8);inner_req.assign(data_req_1,data_req_1+8); inner_res.reserve(8);inner_res.assign(data_res_1,data_res_1+8);
    this->req_arr.push_back(inner_req);this->res_arr.push_back(inner_res);
    inner_req.clear(); inner_res.clear();
    

    // -------------------- GABEL BY USER 210D SUBINX 02-------------------------
    assign_arr(data_req_1, 0x40, 0x0D, 0x21, 0x02, 0x00, 0x00, 0x00, 0x00 );
    assign_arr(data_res_1, 0x4B, 0x0D, 0x21, 0x02, gabel_speed_fahr0_by_user, 0x00, 0x00, 0x00 );

    inner_req.reserve(8);inner_req.assign(data_req_1,data_req_1+8); inner_res.reserve(8);inner_res.assign(data_res_1,data_res_1+8);
    this->req_arr.push_back(inner_req);this->res_arr.push_back(inner_res);
    inner_req.clear(); inner_res.clear();
    

    // -------------------- GABEL BY USER 210D SUBINX 06-------------------------
    assign_arr(data_req_1, 0x40, 0x0D, 0x21, 0x06, 0x00, 0x00, 0x00, 0x00 );
    assign_arr(data_res_1, 0x4B, 0x0D, 0x21, 0x06, gabel_speed_fahr0_by_user, 0x00, 0x00, 0x00 );

    inner_req.reserve(8);inner_req.assign(data_req_1,data_req_1+8); inner_res.reserve(8);inner_res.assign(data_res_1,data_res_1+8);
    this->req_arr.push_back(inner_req);this->res_arr.push_back(inner_res);
    inner_req.clear(); inner_res.clear();
    

    // -------------------- GABEL RÜCKEN 210E SUBINX 02-------------------------
    assign_arr(data_req_1, 0x40, 0x0E, 0x21, 0x02, 0x00, 0x00, 0x00, 0x00 );
    assign_arr(data_res_1, 0x4B, 0x0E, 0x21, 0x02, rucken_gabel_speed_fahr0_0, 0x00, 0x00, 0x00 );

    inner_req.reserve(8);inner_req.assign(data_req_1,data_req_1+8); inner_res.reserve(8);inner_res.assign(data_res_1,data_res_1+8);
    this->req_arr.push_back(inner_req);this->res_arr.push_back(inner_res);
    inner_req.clear(); inner_res.clear();
    

    // -------------------- GABEL RÜCKEN 210E SUBINX 06-------------------------
    assign_arr(data_req_1, 0x40, 0x0E, 0x21, 0x06, 0x00, 0x00, 0x00, 0x00 );
    assign_arr(data_res_1, 0x4B, 0x0E, 0x21, 0x06, rucken_gabel_speed_fahr0_0, 0x00, 0x00, 0x00 );

    inner_req.reserve(8);inner_req.assign(data_req_1,data_req_1+8); inner_res.reserve(8);inner_res.assign(data_res_1,data_res_1+8);
    this->req_arr.push_back(inner_req);this->res_arr.push_back(inner_res);
    inner_req.clear(); inner_res.clear();
    

    // -------------------- BESCHLEUNIGUNG 2110 FAHRPROGRAM 2 SUBINX 02-------------------------
    assign_arr(data_req_1, 0x40, 0x10, 0x21, 0x02, 0x00, 0x00, 0x00, 0x00 );
    assign_arr(data_res_1, 0x4B, 0x10, 0x21, 0x02, acc_fahr1_0, 0x00, 0x00, 0x00 );

    inner_req.reserve(8);inner_req.assign(data_req_1,data_req_1+8); inner_res.reserve(8);inner_res.assign(data_res_1,data_res_1+8);
    this->req_arr.push_back(inner_req);this->res_arr.push_back(inner_res);
    inner_req.clear(); inner_res.clear();
    

    // -------------------- BESCHLEUNIGUNG 2110 FAHRPROGRAM 2 SUBINX 06-------------------------
    assign_arr(data_req_1, 0x40, 0x10, 0x21, 0x06, 0x00, 0x00, 0x00, 0x00 );
    assign_arr(data_res_1, 0x4B, 0x10, 0x21, 0x06, acc_fahr1_0, 0x00, 0x00, 0x00 );

    inner_req.reserve(8);inner_req.assign(data_req_1,data_req_1+8); inner_res.reserve(8);inner_res.assign(data_res_1,data_res_1+8);
    this->req_arr.push_back(inner_req);this->res_arr.push_back(inner_res);
    inner_req.clear(); inner_res.clear();
    

    // -------------------- BESCHLEUNIGUNG BY USER 2111 FAHRPROGRAM 2 SUBINX 02-------------------------
    assign_arr(data_req_1, 0x40, 0x11, 0x21, 0x02, 0x00, 0x00, 0x00, 0x00 );
    assign_arr(data_res_1, 0x4B, 0x11, 0x21, 0x02, acc_by_user_fahr1_0, 0x00, 0x00, 0x00 );

    inner_req.reserve(8);inner_req.assign(data_req_1,data_req_1+8); inner_res.reserve(8);inner_res.assign(data_res_1,data_res_1+8);
    this->req_arr.push_back(inner_req);this->res_arr.push_back(inner_res);
    inner_req.clear(); inner_res.clear();
    

    // -------------------- BESCHLEUNIGUNG BY USER 2111 FAHRPROGRAM 2 SUBINX 06-------------------------
    assign_arr(data_req_1, 0x40, 0x11, 0x21, 0x06, 0x00, 0x00, 0x00, 0x00 );
    assign_arr(data_res_1, 0x4B, 0x11, 0x21, 0x06, acc_by_user_fahr1_0, 0x00, 0x00, 0x00 );

    inner_req.reserve(8);inner_req.assign(data_req_1,data_req_1+8); inner_res.reserve(8);inner_res.assign(data_res_1,data_res_1+8);
    this->req_arr.push_back(inner_req);this->res_arr.push_back(inner_res);
    inner_req.clear(); inner_res.clear();
    

    // -------------------- BESCHLEUNIGUNG RÜCKEN BY USER 2112 FAHRPROGRAM 2 SUBINX 02-------------------------
    assign_arr(data_req_1, 0x40, 0x12, 0x21, 0x02, 0x00, 0x00, 0x00, 0x00 );
    assign_arr(data_res_1, 0x4B, 0x12, 0x21, 0x02, acc_by_user_fahr1_0, 0x00, 0x00, 0x00 );

    inner_req.reserve(8);inner_req.assign(data_req_1,data_req_1+8); inner_res.reserve(8);inner_res.assign(data_res_1,data_res_1+8);
    this->req_arr.push_back(inner_req);this->res_arr.push_back(inner_res);
    inner_req.clear(); inner_res.clear();
    

    // -------------------- BESCHLEUNIGUNG RÜCKEN BY USER 2112 FAHRPROGRAM 2 SUBINX 02-------------------------
    assign_arr(data_req_1, 0x40, 0x12, 0x21, 0x06, 0x00, 0x00, 0x00, 0x00 );
    assign_arr(data_res_1, 0x4B, 0x12, 0x21, 0x06, acc_by_user_fahr1_0, 0x00, 0x00, 0x00 );

    inner_req.reserve(8);inner_req.assign(data_req_1,data_req_1+8); inner_res.reserve(8);inner_res.assign(data_res_1,data_res_1+8);
    this->req_arr.push_back(inner_req);this->res_arr.push_back(inner_res);
    inner_req.clear(); inner_res.clear();
    

    // -------------------- KENNLINIE 2411 SUBINDX 02-------------------------
    assign_arr(data_req_1, 0x40, 0x11, 0x21, 0x02, 0x00, 0x00, 0x00, 0x00 );
    assign_arr(data_res_1, 0x4B, 0x11, 0x21, 0x02, kenn_0, 0x00, 0x00, 0x00 );

    inner_req.reserve(8);inner_req.assign(data_req_1,data_req_1+8); inner_res.reserve(8);inner_res.assign(data_res_1,data_res_1+8);
    this->req_arr.push_back(inner_req);this->res_arr.push_back(inner_res);
    inner_req.clear(); inner_res.clear();
    

    // -------------------- KENNLINIE 2411 SUBINDEX 06-------------------------
    assign_arr(data_req_1, 0x40, 0x11, 0x21, 0x06, 0x00, 0x00, 0x00, 0x00 );
    assign_arr(data_res_1, 0x4B, 0x11, 0x21, 0x06, kenn_0, 0x00, 0x00, 0x00 );

    inner_req.reserve(8);inner_req.assign(data_req_1,data_req_1+8); inner_res.reserve(8);inner_res.assign(data_res_1,data_res_1+8);
    this->req_arr.push_back(inner_req);this->res_arr.push_back(inner_res);
    inner_req.clear(); inner_res.clear();
    

    // -------------------- KENNLINIE 2411-------------------------
    assign_arr(data_req_1, 0x40, 0x11, 0x21, 0x06, 0x00, 0x00, 0x00, 0x00 );
    assign_arr(data_res_1, 0x4B, 0x11, 0x21, 0x06, kenn_0, 0x00, 0x00, 0x00 );

    inner_req.reserve(8);inner_req.assign(data_req_1,data_req_1+8); inner_res.reserve(8);inner_res.assign(data_res_1,data_res_1+8);
    this->req_arr.push_back(inner_req);this->res_arr.push_back(inner_res);
    inner_req.clear(); inner_res.clear();
    

    // -------------------- EINHEITEN UMSCHALTUNG 2612 SUBINDEX 2-------------------------
    assign_arr(data_req_1, 0x40, 0x12, 0x26, 0x02, 0x00, 0x00, 0x00, 0x00 );
    assign_arr(data_res_1, 0x4B, 0x12, 0x26, 0x02, umschalt_0, 0x00, 0x00, 0x00 );

    inner_req.reserve(8);inner_req.assign(data_req_1,data_req_1+8); inner_res.reserve(8);inner_res.assign(data_res_1,data_res_1+8);
    this->req_arr.push_back(inner_req);this->res_arr.push_back(inner_res);
    inner_req.clear(); inner_res.clear();
    

    // -------------------- EINHEITEN UMSCHALTUNG 2612 SUBINDEX 6-------------------------
    assign_arr(data_req_1, 0x40, 0x12, 0x26, 0x06, 0x00, 0x00, 0x00, 0x00 );
    assign_arr(data_res_1, 0x4B, 0x12, 0x26, 0x06, umschalt_0, 0x00, 0x00, 0x00 );

    inner_req.reserve(8);inner_req.assign(data_req_1,data_req_1+8); inner_res.reserve(8);inner_res.assign(data_res_1,data_res_1+8);
    this->req_arr.push_back(inner_req);this->res_arr.push_back(inner_res);
    inner_req.clear(); inner_res.clear();
    

    // -------------------- LENKKORREKTUR 2413 SUBINDEX 2-------------------------
    assign_arr(data_req_1, 0x40, 0x13, 0x24, 0x02, 0x00, 0x00, 0x00, 0x00 );
    assign_arr(data_res_1, 0x4B, 0x13, 0x24, 0x02, lenken_korrektur_0, 0x00, 0x00, 0x00 );

    inner_req.reserve(8);inner_req.assign(data_req_1,data_req_1+8); inner_res.reserve(8);inner_res.assign(data_res_1,data_res_1+8);
    this->req_arr.push_back(inner_req);this->res_arr.push_back(inner_res);
    inner_req.clear(); inner_res.clear();
    

    // -------------------- LENKKORREKTUR 2413 SUBINDEX 6-------------------------
    assign_arr(data_req_1, 0x40, 0x13, 0x24, 0x06, 0x00, 0x00, 0x00, 0x00 );
    assign_arr(data_res_1, 0x4B, 0x13, 0x24, 0x06, lenken_korrektur_0, 0x00, 0x00, 0x00 );

    inner_req.reserve(8);inner_req.assign(data_req_1,data_req_1+8); inner_res.reserve(8);inner_res.assign(data_res_1,data_res_1+8);
    this->req_arr.push_back(inner_req);this->res_arr.push_back(inner_res);
    inner_req.clear(); inner_res.clear();
    

    // -------------------- AUSROLLBREMSE MITFAHRER FAHRPROGRAM 2 2114 SUBINDEX 2-------------------------
    assign_arr(data_req_1, 0x40, 0x14, 0x21, 0x02, 0x00, 0x00, 0x00, 0x00 );
    assign_arr(data_res_1, 0x4B, 0x14, 0x21, 0x02, bremse_fahr1_0, 0x00, 0x00, 0x00 );

    inner_req.reserve(8);inner_req.assign(data_req_1,data_req_1+8); inner_res.reserve(8);inner_res.assign(data_res_1,data_res_1+8);
    this->req_arr.push_back(inner_req);this->res_arr.push_back(inner_res);
    inner_req.clear(); inner_res.clear();
    

    // -------------------- AUSROLLBREMSE MITFAHRER FAHRPROGRAM 2 2114 SUBINDEX 2-------------------------
    assign_arr(data_req_1, 0x40, 0x14, 0x21, 0x02, 0x00, 0x00, 0x00, 0x00 );
    assign_arr(data_res_1, 0x4B, 0x14, 0x21, 0x02, bremse_fahr1_0, 0x00, 0x00, 0x00 );

    inner_req.reserve(8);inner_req.assign(data_req_1,data_req_1+8); inner_res.reserve(8);inner_res.assign(data_res_1,data_res_1+8);
    this->req_arr.push_back(inner_req);this->res_arr.push_back(inner_res);
    inner_req.clear(); inner_res.clear();
    

    // -------------------- LENKENUBERSETZUNG 2414 SUBINDX 02-------------------------
    assign_arr(data_req_1, 0x40, 0x14, 0x24, 0x02, 0x00, 0x00, 0x00, 0x00 );
    assign_arr(data_res_1, 0x4B, 0x14, 0x24, 0x02, lenken_ubersetzung, 0x00, 0x00, 0x00 );

    inner_req.reserve(8);inner_req.assign(data_req_1,data_req_1+8); inner_res.reserve(8);inner_res.assign(data_res_1,data_res_1+8);
    this->req_arr.push_back(inner_req);this->res_arr.push_back(inner_res);
    inner_req.clear(); inner_res.clear();
    

    // -------------------- LENKENUBERSETZUNG 2414 SUBINDX 02-------------------------
    assign_arr(data_req_1, 0x40, 0x14, 0x24, 0x06, 0x00, 0x00, 0x00, 0x00 );
    assign_arr(data_res_1, 0x4B, 0x14, 0x24, 0x06, lenken_ubersetzung, 0x00, 0x00, 0x00 );

    inner_req.reserve(8);inner_req.assign(data_req_1,data_req_1+8); inner_res.reserve(8);inner_res.assign(data_res_1,data_res_1+8);
    this->req_arr.push_back(inner_req);this->res_arr.push_back(inner_res);
    inner_req.clear(); inner_res.clear();
    

    // -------------------- AUSROLLBREMSE MITGÄNGER FAHRPROGRAM 2 2115 SUBINDX 02-------------------------
    assign_arr(data_req_1, 0x40, 0x15, 0x21, 0x02, 0x00, 0x00, 0x00, 0x00 );
    assign_arr(data_res_1, 0x4B, 0x15, 0x21, 0x02, bremse_fahr1_by_user, 0x00, 0x00, 0x00 );

    inner_req.reserve(8);inner_req.assign(data_req_1,data_req_1+8); inner_res.reserve(8);inner_res.assign(data_res_1,data_res_1+8);
    this->req_arr.push_back(inner_req);this->res_arr.push_back(inner_res);
    inner_req.clear(); inner_res.clear();
    

    // -------------------- AUSROLLBREMSE MITGÄNGER FAHRPROGRAM 2 2115 SUBINDX 06-------------------------
    assign_arr(data_req_1, 0x40, 0x15, 0x21, 0x06, 0x00, 0x00, 0x00, 0x00 );
    assign_arr(data_res_1, 0x4B, 0x15, 0x21, 0x06, bremse_fahr1_by_user, 0x00, 0x00, 0x00 );

    inner_req.reserve(8);inner_req.assign(data_req_1,data_req_1+8);  inner_res.reserve(8);inner_res.assign(data_res_1,data_res_1+8);
    this->req_arr.push_back(inner_req);this->res_arr.push_back(inner_res);
    inner_req.clear(); inner_res.clear();
    

    // -------------------- MAXIMALGEGSCHWINDIGKEIT ANTRIEBSRICHTUNG IM FP2 2118 SUBINDEX 2-------------------------
    assign_arr(data_req_1, 0x40, 0x18, 0x21, 0x02, 0x00, 0x00, 0x00, 0x00 );
    assign_arr(data_res_1, 0x4B, 0x18, 0x21, 0x02, antrieb_speed_fahr1_0, 0x00, 0x00, 0x00 );

    inner_req.reserve(8);inner_req.assign(data_req_1,data_req_1+8); inner_res.reserve(8);inner_res.assign(data_res_1,data_res_1+8);
    this->req_arr.push_back(inner_req);this->res_arr.push_back(inner_res);
    inner_req.clear(); inner_res.clear();
    

    // -------------------- MAXIMALGEGSCHWINDIGKEIT ANTRIEBSRICHTUNG IM FP2 2118 SUBINDEX 6-------------------------
    assign_arr(data_req_1, 0x40, 0x18, 0x21, 0x06, 0x00, 0x00, 0x00, 0x00 );
    assign_arr(data_res_1, 0x4B, 0x18, 0x21, 0x06, antrieb_speed_fahr1_0, 0x00, 0x00, 0x00 );

    inner_req.reserve(8);inner_req.assign(data_req_1,data_req_1+8); inner_res.reserve(8);inner_res.assign(data_res_1,data_res_1+8);
    this->req_arr.push_back(inner_req);this->res_arr.push_back(inner_res);
    inner_req.clear(); inner_res.clear();
    

    // -------------------- HOCHLAUFRAMPE HEBEN PLATFORM 2218 SUBINDX 2-------------------------
    assign_arr(data_req_1, 0x40, 0x18, 0x22, 0x02, 0x00, 0x00, 0x00, 0x00 );
    assign_arr(data_res_1, 0x4B, 0x18, 0x22, 0x02, ramp_heben_0, 0x00, 0x00, 0x00 );

    inner_req.reserve(8);inner_req.assign(data_req_1,data_req_1+8); inner_res.reserve(8);inner_res.assign(data_res_1,data_res_1+8);
    this->req_arr.push_back(inner_req);this->res_arr.push_back(inner_res);
    inner_req.clear(); inner_res.clear();
    

    // -------------------- HOCHLAUFRAMPE HEBEN PLATFORM 2218 SUBINDX 6-------------------------
    assign_arr(data_req_1, 0x40, 0x18, 0x22, 0x06, 0x00, 0x00, 0x00, 0x00 );
    assign_arr(data_res_1, 0x4B, 0x18, 0x22, 0x026, ramp_heben_0, 0x00, 0x00, 0x00 );

    inner_req.reserve(8);inner_req.assign(data_req_1,data_req_1+8); inner_res.reserve(8);inner_res.assign(data_res_1,data_res_1+8);
    this->req_arr.push_back(inner_req);this->res_arr.push_back(inner_res);
    inner_req.clear(); inner_res.clear();
    

    // -------------------- MITGÄNGERGESCHWINDIGKEIT ANTRIEB BY USER IN FP2 2119 SUBINDX 2-------------------------
    assign_arr(data_req_1, 0x40, 0x19, 0x21, 0x02, 0x00, 0x00, 0x00, 0x00 );
    assign_arr(data_res_1, 0x4B, 0x19, 0x21, 0x02, antrieb_speed_fahr1_by_user, 0x00, 0x00, 0x00 );

    inner_req.reserve(8);inner_req.assign(data_req_1,data_req_1+8); inner_res.reserve(8);inner_res.assign(data_res_1,data_res_1+8);
    this->req_arr.push_back(inner_req);this->res_arr.push_back(inner_res);
    inner_req.clear(); inner_res.clear();
    

    // -------------------- MITGÄNGERGESCHWINDIGKEIT ANTRIEB BY USER IN FP2 2119 SUBINDX 6-------------------------
    assign_arr(data_req_1, 0x40, 0x19, 0x21, 0x06, 0x00, 0x00, 0x00, 0x00 );
    assign_arr(data_res_1, 0x4B, 0x19, 0x21, 0x06, antrieb_speed_fahr1_by_user, 0x00, 0x00, 0x00 );

    inner_req.reserve(8);inner_req.assign(data_req_1,data_req_1+8); inner_res.reserve(8);inner_res.assign(data_res_1,data_res_1+8);
    this->req_arr.push_back(inner_req);this->res_arr.push_back(inner_res);
    inner_req.clear(); inner_res.clear();
    

    // -------------------- AUSLAUFRAMPE HEBEN 2219 SUBINDX 2-------------------------
    assign_arr(data_req_1, 0x40, 0x19, 0x22, 0x02, 0x00, 0x00, 0x00, 0x00 );
    assign_arr(data_res_1, 0x4B, 0x19, 0x22, 0x02, auslauf_heben_0, 0x00, 0x00, 0x00 );

    inner_req.reserve(8);inner_req.assign(data_req_1,data_req_1+8); inner_res.reserve(8);inner_res.assign(data_res_1,data_res_1+8);
    this->req_arr.push_back(inner_req);this->res_arr.push_back(inner_res);
    inner_req.clear(); inner_res.clear();
    

    // -------------------- AUSLAUFRAMPE HEBEN 2219 SUBINDX 6-------------------------
    assign_arr(data_req_1, 0x40, 0x19, 0x22, 0x06, 0x00, 0x00, 0x00, 0x00 );
    assign_arr(data_res_1, 0x4B, 0x19, 0x22, 0x06, auslauf_heben_0, 0x00, 0x00, 0x00 );

    inner_req.reserve(8);inner_req.assign(data_req_1,data_req_1+8); inner_res.reserve(8);inner_res.assign(data_res_1,data_res_1+8);
    this->req_arr.push_back(inner_req);this->res_arr.push_back(inner_res);
    inner_req.clear(); inner_res.clear();
    

    // -------------------- ANTRIEB RÜCKEN BY USER GESCHWINDIGKEIT FP2 211A SUBINDX 2-------------------------
    assign_arr(data_req_1, 0x40, 0x1A, 0x21, 0x02, 0x00, 0x00, 0x00, 0x00 );
    assign_arr(data_res_1, 0x4B, 0x1A, 0x21, 0x02, antrieb_speed_fahr1_by_user, 0x00, 0x00, 0x00 );

    inner_req.reserve(8);inner_req.assign(data_req_1,data_req_1+8); inner_res.reserve(8);inner_res.assign(data_res_1,data_res_1+8);
    this->req_arr.push_back(inner_req);this->res_arr.push_back(inner_res);
    inner_req.clear(); inner_res.clear();
  

    // -------------------- ANTRIEB RÜCKEN BY USER GESCHWINDIGKEIT FP2 211A SUBINDX 6-------------------------
    assign_arr(data_req_1, 0x40, 0x1A, 0x21, 0x06, 0x00, 0x00, 0x00, 0x00 );
    assign_arr(data_res_1, 0x4B, 0x1A, 0x21, 0x06, antrieb_speed_fahr1_by_user, 0x00, 0x00, 0x00 );

    inner_req.reserve(8);inner_req.assign(data_req_1,data_req_1+8); inner_res.reserve(8);inner_res.assign(data_res_1,data_res_1+8);
    this->req_arr.push_back(inner_req);this->res_arr.push_back(inner_res);
    inner_req.clear(); inner_res.clear();

    // -------------------- GABEL GESCHWINDIGKEIT FP2 211C SUBINDX 2-------------------------
    assign_arr(data_req_1, 0x40, 0x1C, 0x21, 0x02, 0x00, 0x00, 0x00, 0x00 );
    assign_arr(data_res_1, 0x4B, 0x1C, 0x21, 0x02, gabel_speed_fahr1_0, 0x00, 0x00, 0x00 );

    inner_req.reserve(8);inner_req.assign(data_req_1,data_req_1+8); inner_res.reserve(8);inner_res.assign(data_res_1,data_res_1+8);
    this->req_arr.push_back(inner_req);this->res_arr.push_back(inner_res);
    inner_req.clear(); inner_res.clear();
        
  
    // -------------------- GABEL GESCHWINDIGKEIT FP2 211C SUBINDX 6-------------------------
    assign_arr(data_req_1, 0x40, 0x1C, 0x21, 0x06, 0x00, 0x00, 0x00, 0x00 );
    assign_arr(data_res_1, 0x4B, 0x1C, 0x21, 0x06, gabel_speed_fahr1_0, 0x00, 0x00, 0x00 );

    inner_req.reserve(8);inner_req.assign(data_req_1,data_req_1+8); inner_res.reserve(8);inner_res.assign(data_res_1,data_res_1+8);
    this->req_arr.push_back(inner_req);this->res_arr.push_back(inner_res);
    inner_req.clear(); inner_res.clear();
    

    // -------------------- GABEL BY USER GESCHWINDIGKEIT FP2 211D SUBINDX 2-------------------------
    assign_arr(data_req_1, 0x40, 0x1D, 0x21, 0x02, 0x00, 0x00, 0x00, 0x00 );
    assign_arr(data_res_1, 0x4B, 0x1D, 0x21, 0x02, gabel_speed_fahr1_by_user, 0x00, 0x00, 0x00 );

    inner_req.reserve(8);inner_req.assign(data_req_1,data_req_1+8); inner_res.reserve(8);inner_res.assign(data_res_1,data_res_1+8);
    this->req_arr.push_back(inner_req);this->res_arr.push_back(inner_res);
    inner_req.clear(); inner_res.clear();
    

    // -------------------- GABEL BY USER GESCHWINDIGKEIT FP2 211D SUBINDX 6-------------------------
    assign_arr(data_req_1, 0x40, 0x1D, 0x21, 0x06, 0x00, 0x00, 0x00, 0x00 );
    assign_arr(data_res_1, 0x4B, 0x1D, 0x21, 0x06, gabel_speed_fahr1_by_user, 0x00, 0x00, 0x00 );

    inner_req.reserve(8);inner_req.assign(data_req_1,data_req_1+8); inner_res.reserve(8);inner_res.assign(data_res_1,data_res_1+8);
    this->req_arr.push_back(inner_req);this->res_arr.push_back(inner_res);
    inner_req.clear(); inner_res.clear();
    

    // -------------------- GABEL RÜCKEN BY USER GESCHWINDIGKEIT FP2 211E SUBINDX 2-------------------------
    assign_arr(data_req_1, 0x40, 0x1E, 0x21, 0x02, 0x00, 0x00, 0x00, 0x00 );
    assign_arr(data_res_1, 0x4B, 0x1E, 0x21, 0x02, rucken_gabel_speed_fahr1_0, 0x00, 0x00, 0x00 );

    inner_req.reserve(8);inner_req.assign(data_req_1,data_req_1+8); inner_res.reserve(8);inner_res.assign(data_res_1,data_res_1+8);
    this->req_arr.push_back(inner_req);this->res_arr.push_back(inner_res);
    inner_req.clear(); inner_res.clear();
    

    // -------------------- GABEL RÜCKEN BY USER GESCHWINDIGKEIT FP2 211E SUBINDX 6-------------------------
    assign_arr(data_req_1, 0x40, 0x1E, 0x21, 0x06, 0x00, 0x00, 0x00, 0x00 );
    assign_arr(data_res_1, 0x4B, 0x1E, 0x21, 0x06, rucken_gabel_speed_fahr1_0, 0x00, 0x00, 0x00 );

    inner_req.reserve(8);inner_req.assign(data_req_1,data_req_1+8); inner_res.reserve(8);inner_res.assign(data_res_1,data_res_1+8);
    this->req_arr.push_back(inner_req);this->res_arr.push_back(inner_res);
    inner_req.clear(); inner_res.clear();
    

    // -------------------- BESCHLEUNIGUNG FP3 SUBINDX 2-------------------------
    assign_arr(data_req_1, 0x40, 0x21, 0x20, 0x02, 0x00, 0x00, 0x00, 0x00 );
    assign_arr(data_res_1, 0x4B, 0x21, 0x20, 0x02, acc_fahr2_0, 0x00, 0x00, 0x00 );

    inner_req.reserve(8);inner_req.assign(data_req_1,data_req_1+8); inner_res.reserve(8);inner_res.assign(data_res_1,data_res_1+8);
    this->req_arr.push_back(inner_req);this->res_arr.push_back(inner_res);
    inner_req.clear(); inner_res.clear();
    

    // -------------------- BESCHLEUNIGUNG FP3 SUBINDX 6-------------------------
    assign_arr(data_req_1, 0x40, 0x21, 0x20, 0x06, 0x00, 0x00, 0x00, 0x00 );
    assign_arr(data_res_1, 0x4B, 0x21, 0x20, 0x06, acc_fahr2_0, 0x00, 0x00, 0x00 );

    inner_req.reserve(8);inner_req.assign(data_req_1,data_req_1+8); inner_res.reserve(8);inner_res.assign(data_res_1,data_res_1+8);
    this->req_arr.push_back(inner_req);this->res_arr.push_back(inner_res);
    inner_req.clear(); inner_res.clear();
    

    // -------------------- FAHRZEUGTYP 2029 SUBINDX 2-------------------------
    assign_arr(data_req_1, 0x40, 0x29, 0x20, 0x02, 0x00, 0x00, 0x00, 0x00 );
    assign_arr(data_res_1, 0x4B, 0x29, 0x20, 0x02, truck_type, 0x00, 0x00, 0x00 );

    inner_req.reserve(8);inner_req.assign(data_req_1,data_req_1+8); inner_res.reserve(8);inner_res.assign(data_res_1,data_res_1+8);
    this->req_arr.push_back(inner_req);this->res_arr.push_back(inner_res);
    inner_req.clear(); inner_res.clear();
    

    // -------------------- FAHRZEUGTYP 2029 SUBINDX 6-------------------------
    assign_arr(data_req_1, 0x40, 0x29, 0x20, 0x06, 0x00, 0x00, 0x00, 0x00 );
    assign_arr(data_res_1, 0x4B, 0x29, 0x20, 0x06, truck_type, 0x00, 0x00, 0x00 );

    inner_req.reserve(8);inner_req.assign(data_req_1,data_req_1+8); inner_res.reserve(8);inner_res.assign(data_res_1,data_res_1+8);
    this->req_arr.push_back(inner_req);this->res_arr.push_back(inner_res);
    inner_req.clear(); inner_res.clear();
    

    // -------------------- BESCHLEUNIGUNG BY USER FP3 2121 SUBINDX 2-------------------------
    assign_arr(data_req_1, 0x40, 0x21, 0x21, 0x02, 0x00, 0x00, 0x00, 0x00 );
    assign_arr(data_res_1, 0x4B, 0x21, 0x21, 0x02, acc_by_user_fahr2_0, 0x00, 0x00, 0x00 );

    inner_req.reserve(8);inner_req.assign(data_req_1,data_req_1+8); inner_res.reserve(8);inner_res.assign(data_res_1,data_res_1+8);
    this->req_arr.push_back(inner_req);this->res_arr.push_back(inner_res);
    inner_req.clear(); inner_res.clear();
    

    // -------------------- BESCHLEUNIGUNG BY USER FP3 2121 SUBINDX 6-------------------------
    assign_arr(data_req_1, 0x40, 0x21, 0x21, 0x02, 0x00, 0x00, 0x00, 0x00 );
    assign_arr(data_res_1, 0x4B, 0x21, 0x21, 0x02, acc_by_user_fahr2_0, 0x00, 0x00, 0x00 );

    inner_req.reserve(8);inner_req.assign(data_req_1,data_req_1+8); inner_res.reserve(8);inner_res.assign(data_res_1,data_res_1+8);
    this->req_arr.push_back(inner_req);this->res_arr.push_back(inner_res);
    inner_req.clear(); inner_res.clear();
    

    // -------------------- BESCHLEUNIGUNG RUCKEN FP3 2122 SUBINDX 2-------------------------
    assign_arr(data_req_1, 0x40, 0x22, 0x21, 0x02, 0x00, 0x00, 0x00, 0x00 );
    assign_arr(data_res_1, 0x4B, 0x22, 0x21, 0x02, acc_rucken_farh2_0, 0x00, 0x00, 0x00 );

    inner_req.reserve(8);inner_req.assign(data_req_1,data_req_1+8); inner_res.reserve(8);inner_res.assign(data_res_1,data_res_1+8);
    this->req_arr.push_back(inner_req);this->res_arr.push_back(inner_res);
    inner_req.clear(); inner_res.clear();
    

    // -------------------- BESCHLEUNIGUNG RUCKEN FP3 2122 SUBINDX 6-------------------------
    assign_arr(data_req_1, 0x40, 0x22, 0x21, 0x06, 0x00, 0x00, 0x00, 0x00 );
    assign_arr(data_res_1, 0x4B, 0x22, 0x21, 0x06, acc_rucken_farh2_0, 0x00, 0x00, 0x00 );

    inner_req.reserve(8);inner_req.assign(data_req_1,data_req_1+8); inner_res.reserve(8);inner_res.assign(data_res_1,data_res_1+8);
    this->req_arr.push_back(inner_req);this->res_arr.push_back(inner_res);
    inner_req.clear(); inner_res.clear();
    

    // -------------------- VERRIEGELUNG 2922 SUBINDX 2-------------------------
    assign_arr(data_req_1, 0x40, 0x22, 0x29, 0x02, 0x00, 0x00, 0x00, 0x00 );
    assign_arr(data_res_1, 0x4B, 0x22, 0x29, 0x02, verrie_0, 0x00, 0x00, 0x00 );

    inner_req.reserve(8);inner_req.assign(data_req_1,data_req_1+8); inner_res.reserve(8);inner_res.assign(data_res_1,data_res_1+8);
    this->req_arr.push_back(inner_req);this->res_arr.push_back(inner_res);
    inner_req.clear(); inner_res.clear();
    

    // -------------------- VERRIEGELUNG 2922 SUBINDX 6-------------------------
    assign_arr(data_req_1, 0x40, 0x22, 0x29, 0x06, 0x00, 0x00, 0x00, 0x00 );
    assign_arr(data_res_1, 0x4B, 0x22, 0x29, 0x06, verrie_1, 0x00, 0x00, 0x00 );

    inner_req.reserve(8);inner_req.assign(data_req_1,data_req_1+8); inner_res.reserve(8);inner_res.assign(data_res_1,data_res_1+8);
    this->req_arr.push_back(inner_req);this->res_arr.push_back(inner_res);
    inner_req.clear(); inner_res.clear();
    

    // -------------------- ZEIT EINFALL BREMSE UND BANDAGENENTSPANNUNG 2923 SUBINDX 2-------------------------
    assign_arr(data_req_1, 0x40, 0x23, 0x29, 0x02, 0x00, 0x00, 0x00, 0x00 );
    assign_arr(data_res_1, 0x4B, 0x23, 0x29, 0x02, zeit_bremse_0, 0x00, 0x00, 0x00 );

    inner_req.reserve(8);inner_req.assign(data_req_1,data_req_1+8); inner_res.reserve(8);inner_res.assign(data_res_1,data_res_1+8);
    this->req_arr.push_back(inner_req);this->res_arr.push_back(inner_res);
    inner_req.clear(); inner_res.clear();
    

    // -------------------- ZEIT EINFALL BREMSE UND BANDAGENENTSPANNUNG 2923 SUBINDX 6-------------------------
    assign_arr(data_req_1, 0x40, 0x23, 0x29, 0x06, 0x00, 0x00, 0x00, 0x00 );
    assign_arr(data_res_1, 0x4B, 0x23, 0x29, 0x06, zeit_bremse_0, 0x00, 0x00, 0x00 );

    inner_req.reserve(8);inner_req.assign(data_req_1,data_req_1+8); inner_res.reserve(8);inner_res.assign(data_res_1,data_res_1+8);
    this->req_arr.push_back(inner_req);this->res_arr.push_back(inner_res);
    inner_req.clear(); inner_res.clear();
    

    // -------------------- AUSROLLBREMSE MITFAHRER FP3 2124 SUBINDX 2-------------------------
    assign_arr(data_req_1, 0x40, 0x24, 0x21, 0x02, 0x00, 0x00, 0x00, 0x00 );
    assign_arr(data_res_1, 0x4B, 0x24, 0x21, 0x02, bremse_fahr2_0, 0x00, 0x00, 0x00 );

    inner_req.reserve(8);inner_req.assign(data_req_1,data_req_1+8); inner_res.reserve(8);inner_res.assign(data_res_1,data_res_1+8);
    this->req_arr.push_back(inner_req);this->res_arr.push_back(inner_res);
    inner_req.clear(); inner_res.clear();
    

    // -------------------- AUSROLLBREMSE MITFAHRER FP3 2124 SUBINDX 6-------------------------
    assign_arr(data_req_1, 0x40, 0x24, 0x21, 0x06, 0x00, 0x00, 0x00, 0x00 );
    assign_arr(data_res_1, 0x4B, 0x24, 0x21, 0x06, bremse_fahr2_0, 0x00, 0x00, 0x00 );

    inner_req.reserve(8);inner_req.assign(data_req_1,data_req_1+8); inner_res.reserve(8);inner_res.assign(data_res_1,data_res_1+8);
    this->req_arr.push_back(inner_req);this->res_arr.push_back(inner_res);
    inner_req.clear(); inner_res.clear();
    

    // -------------------- ECO VORGABE 2924 SUBINDX 2-------------------------
    assign_arr(data_req_1, 0x40, 0x24, 0x29, 0x02, 0x00, 0x00, 0x00, 0x00 );
    assign_arr(data_res_1, 0x4B, 0x24, 0x29, 0x02, eco_0, 0x00, 0x00, 0x00 );

    inner_req.reserve(8);inner_req.assign(data_req_1,data_req_1+8); inner_res.reserve(8);inner_res.assign(data_res_1,data_res_1+8);
    this->req_arr.push_back(inner_req);this->res_arr.push_back(inner_res);
    inner_req.clear(); inner_res.clear();
    

    // -------------------- ECO VORGABE 2924 SUBINDX 6-------------------------
    assign_arr(data_req_1, 0x40, 0x24, 0x29, 0x06, 0x00, 0x00, 0x00, 0x00 );
    assign_arr(data_res_1, 0x4B, 0x24, 0x29, 0x06, eco_0, 0x00, 0x00, 0x00 );

    inner_req.reserve(8);inner_req.assign(data_req_1,data_req_1+8); inner_res.reserve(8);inner_res.assign(data_res_1,data_res_1+8);
    this->req_arr.push_back(inner_req);this->res_arr.push_back(inner_res);
    inner_req.clear(); inner_res.clear();
    

    // -------------------- AUSROLLBREMSE BY USER FP3 2125 SUNINDX 2-------------------------
    assign_arr(data_req_1, 0x40, 0x25, 0x21, 0x02, 0x00, 0x00, 0x00, 0x00 );
    assign_arr(data_res_1, 0x4B, 0x25, 0x21, 0x02, bremse_fahr2_by_user, 0x00, 0x00, 0x00 );

    inner_req.reserve(8);inner_req.assign(data_req_1,data_req_1+8); inner_res.reserve(8);inner_res.assign(data_res_1,data_res_1+8);
    this->req_arr.push_back(inner_req);this->res_arr.push_back(inner_res);
    inner_req.clear(); inner_res.clear();
    


    // -------------------- AUSROLLBREMSE BY USER FP3 2125 SUNINDX 6-------------------------
    assign_arr(data_req_1, 0x40, 0x25, 0x21, 0x06, 0x00, 0x00, 0x00, 0x00 );
    assign_arr(data_res_1, 0x4B, 0x25, 0x21, 0x06, bremse_fahr2_by_user, 0x00, 0x00, 0x00 );

    inner_req.reserve(8);inner_req.assign(data_req_1,data_req_1+8); inner_res.reserve(8);inner_res.assign(data_res_1,data_res_1+8);
    this->req_arr.push_back(inner_req);this->res_arr.push_back(inner_res);
    inner_req.clear(); inner_res.clear();
    

    // -------------------- MAXIMALGESCHWINDIGKEIT ANTRIEBSRICHTUNG FP3 2128 SUNINDX 2-------------------------
    assign_arr(data_req_1, 0x40, 0x28, 0x21, 0x02, 0x00, 0x00, 0x00, 0x00 );
    assign_arr(data_res_1, 0x4B, 0x28, 0x21, 0x02, antrieb_speed_fahr2_0, 0x00, 0x00, 0x00 );

    inner_req.reserve(8);inner_req.assign(data_req_1,data_req_1+8); inner_res.reserve(8);inner_res.assign(data_res_1,data_res_1+8);
    this->req_arr.push_back(inner_req);this->res_arr.push_back(inner_res);
    inner_req.clear(); inner_res.clear();
    


    // -------------------- MAXIMALGESCHWINDIGKEIT ANTRIEBSRICHTUNG FP3 2128 SUNINDX 6-------------------------
    assign_arr(data_req_1, 0x40, 0x28, 0x21, 0x02, 0x00, 0x00, 0x00, 0x00 );
    assign_arr(data_res_1, 0x4B, 0x28, 0x21, 0x02, antrieb_speed_fahr2_0_indx6, 0x00, 0x00, 0x00 );

    inner_req.reserve(8);inner_req.assign(data_req_1,data_req_1+8); inner_res.reserve(8);inner_res.assign(data_res_1,data_res_1+8);
    this->req_arr.push_back(inner_req);this->res_arr.push_back(inner_res);
    inner_req.clear(); inner_res.clear();
    

    // -------------------- GESCHWINDIGKEIT BY USER ANTRIEBSRICHTUNG FP3 2129 SUNINDX 2-------------------------
    assign_arr(data_req_1, 0x40, 0x29, 0x21, 0x02, 0x00, 0x00, 0x00, 0x00 );
    assign_arr(data_res_1, 0x4B, 0x29, 0x21, 0x02, antrieb_speed_fahr2_by_user, 0x00, 0x00, 0x00 );

    inner_req.reserve(8);inner_req.assign(data_req_1,data_req_1+8); inner_res.reserve(8);inner_res.assign(data_res_1,data_res_1+8);
    this->req_arr.push_back(inner_req);this->res_arr.push_back(inner_res);
    inner_req.clear(); inner_res.clear();
    

    // -------------------- GESCHWINDIGKEIT BY USER ANTRIEBSRICHTUNG FP3 2129 SUNINDX 6-------------------------
    assign_arr(data_req_1, 0x40, 0x29, 0x21, 0x06, 0x00, 0x00, 0x00, 0x00 );
    assign_arr(data_res_1, 0x4B, 0x29, 0x21, 0x06, antrieb_speed_fahr2_by_user, 0x00, 0x00, 0x00 );

    inner_req.reserve(8);inner_req.assign(data_req_1,data_req_1+8); inner_res.reserve(8);inner_res.assign(data_res_1,data_res_1+8);
    this->req_arr.push_back(inner_req);this->res_arr.push_back(inner_res);
    inner_req.clear(); inner_res.clear();
    

    // -------------------- GESCHWINDIGKEIT RÜCKEN ANTRIEBSRICHTUNG FP3 212A SUNINDX 2-------------------------
    assign_arr(data_req_1, 0x40, 0x2A, 0x21, 0x02, 0x00, 0x00, 0x00, 0x00 );
    assign_arr(data_res_1, 0x4B, 0x2A, 0x21, 0x02, rucken_fahr2_speed_0, 0x00, 0x00, 0x00 );

    inner_req.reserve(8);inner_req.assign(data_req_1,data_req_1+8); inner_res.reserve(8);inner_res.assign(data_res_1,data_res_1+8);
    this->req_arr.push_back(inner_req);this->res_arr.push_back(inner_res);
    inner_req.clear(); inner_res.clear();
    


    // -------------------- GESCHWINDIGKEIT RÜCKEN ANTRIEBSRICHTUNG FP3 212A SUNINDX 2-------------------------
    assign_arr(data_req_1, 0x40, 0x2A, 0x21, 0x06, 0x00, 0x00, 0x00, 0x00 );
    assign_arr(data_res_1, 0x4B, 0x2A, 0x21, 0x06, rucken_fahr2_speed_0, 0x00, 0x00, 0x00 );

    inner_req.reserve(8);inner_req.assign(data_req_1,data_req_1+8); inner_res.reserve(8);inner_res.assign(data_res_1,data_res_1+8);
    this->req_arr.push_back(inner_req);this->res_arr.push_back(inner_res);
    inner_req.clear(); inner_res.clear();
    

    // -------------------- UBERWACHUNGSZEIT BEDIENERPOSITION 292B SUNINDX 2-------------------------
    assign_arr(data_req_1, 0x40, 0x2B, 0x29, 0x02, 0x00, 0x00, 0x00, 0x00 );
    assign_arr(data_res_1, 0x4B, 0x2B, 0x29, 0x02, wakeup_0, 0x00, 0x00, 0x00 );

    inner_req.reserve(8);inner_req.assign(data_req_1,data_req_1+8); inner_res.reserve(8);inner_res.assign(data_res_1,data_res_1+8);
    this->req_arr.push_back(inner_req);this->res_arr.push_back(inner_res);
    inner_req.clear(); inner_res.clear();
    


    // -------------------- UBERWACHUNGSZEIT BEDIENERPOSITION 292B SUNINDX 6-------------------------
    assign_arr(data_req_1, 0x40, 0x2B, 0x29, 0x06, 0x00, 0x00, 0x00, 0x00 );
    assign_arr(data_res_1, 0x4B, 0x2B, 0x29, 0x06, wakeup_0, 0x00, 0x00, 0x00 );

    inner_req.reserve(8);inner_req.assign(data_req_1,data_req_1+8); inner_res.reserve(8);inner_res.assign(data_res_1,data_res_1+8);
    this->req_arr.push_back(inner_req);this->res_arr.push_back(inner_res);
    inner_req.clear(); inner_res.clear();
   

    // -------------------- MAXIMALGESCHWINDIGKEIT GABEL FP3 212C SUNINDX 2-------------------------
    assign_arr(data_req_1, 0x40, 0x2C, 0x21, 0x02, 0x00, 0x00, 0x00, 0x00 );
    assign_arr(data_res_1, 0x4B, 0x2C, 0x21, 0x02, gabel_speed_fahr2_0, 0x00, 0x00, 0x00 );

    inner_req.reserve(8);inner_req.assign(data_req_1,data_req_1+8); inner_res.reserve(8);inner_res.assign(data_res_1,data_res_1+8);
    this->req_arr.push_back(inner_req);this->res_arr.push_back(inner_res);
    inner_req.clear(); inner_res.clear();
    

    // -------------------- MAXIMALGESCHWINDIGKEIT GABEL FP3 212C SUNINDX 6-------------------------
    assign_arr(data_req_1, 0x40, 0x2C, 0x21, 0x02, 0x00, 0x00, 0x00, 0x00 );
    assign_arr(data_res_1, 0x4B, 0x2C, 0x21, 0x02, gabel_speed_fahr2_0_subindex6, 0x00, 0x00, 0x00 );

    inner_req.reserve(8);inner_req.assign(data_req_1,data_req_1+8); inner_res.reserve(8);inner_res.assign(data_res_1,data_res_1+8);
    this->req_arr.push_back(inner_req);this->res_arr.push_back(inner_res);
    inner_req.clear(); inner_res.clear();
    


    // -------------------- GESCHWINDIGKEIT BY USER GABEL FP3 212D SUNINDX 2-------------------------
    assign_arr(data_req_1, 0x40, 0x2D, 0x21, 0x02, 0x00, 0x00, 0x00, 0x00 );
    assign_arr(data_res_1, 0x4B, 0x2D, 0x21, 0x02, gabel_speed_fahr2_by_user, 0x00, 0x00, 0x00 );

    inner_req.reserve(8);inner_req.assign(data_req_1,data_req_1+8); inner_res.reserve(8);inner_res.assign(data_res_1,data_res_1+8);
    this->req_arr.push_back(inner_req);this->res_arr.push_back(inner_res);
    inner_req.clear(); inner_res.clear();
    

    
    // -------------------- GESCHWINDIGKEIT BY USER GABEL FP3 212D SUNINDX 6-------------------------
    assign_arr(data_req_1, 0x40, 0x2D, 0x21, 0x06, 0x00, 0x00, 0x00, 0x00 );
    assign_arr(data_res_1, 0x4B, 0x2D, 0x21, 0x06, gabel_speed_fahr2_by_user, 0x00, 0x00, 0x00 );

    inner_req.reserve(8);inner_req.assign(data_req_1,data_req_1+8); inner_res.reserve(8);inner_res.assign(data_res_1,data_res_1+8);
    this->req_arr.push_back(inner_req);this->res_arr.push_back(inner_res);
    inner_req.clear(); inner_res.clear();
    


    // -------------------- GESCHWINDIGKEIT RÜCKEN GABEL FP3 212E SUNINDX 2-------------------------
    assign_arr(data_req_1, 0x40, 0x2E, 0x21, 0x02, 0x00, 0x00, 0x00, 0x00 );
    assign_arr(data_res_1, 0x4B, 0x2E, 0x21, 0x02,rucken_gabel_speed_fahr2_0, 0x00, 0x00, 0x00 );

    inner_req.reserve(8);inner_req.assign(data_req_1,data_req_1+8); inner_res.reserve(8);inner_res.assign(data_res_1,data_res_1+8);
    this->req_arr.push_back(inner_req);this->res_arr.push_back(inner_res);
    inner_req.clear(); inner_res.clear();
    

    // -------------------- GESCHWINDIGKEIT RÜCKEN GABEL FP3 212E SUNINDX 6-------------------------
    assign_arr(data_req_1, 0x40, 0x2E, 0x21, 0x06, 0x00, 0x00, 0x00, 0x00 );
    assign_arr(data_res_1, 0x4B, 0x2E, 0x21, 0x06,rucken_gabel_speed_fahr2_0, 0x00, 0x00, 0x00 );

    inner_req.reserve(8);inner_req.assign(data_req_1,data_req_1+8); inner_res.reserve(8);inner_res.assign(data_res_1,data_res_1+8);
    this->req_arr.push_back(inner_req);this->res_arr.push_back(inner_res);
    inner_req.clear(); inner_res.clear();
    

 

    // -------------------- BATTERIELADEZUSTAND 2030-------------------------
    assign_arr(data_req_1, 0x40, 0x30, 0x20, 0x00, 0x00, 0x00, 0x00, 0x00 );
    assign_arr(data_res_1, 0x4F, 0x30, 0x20, 0x00, percent0, 0x00, 0x00, 0x00 );

    inner_req.reserve(8);inner_req.assign(data_req_1,data_req_1+8); inner_res.reserve(8);inner_res.assign(data_res_1,data_res_1+8);
    this->req_arr.push_back(inner_req);this->res_arr.push_back(inner_res);
    inner_req.clear(); inner_res.clear();
    



    // -------------------- HOCHLAUFRAMPE SENKEN LASTTEIL 2330 SUBINDX 02-------------------------
    assign_arr(data_req_1, 0x40, 0x30, 0x23, 0x02, 0x00, 0x00, 0x00, 0x00 );
    assign_arr(data_res_1, 0x4B, 0x30, 0x23, 0x02, ramp_senken_lastteil_0, 0x00, 0x00, 0x00 );

    inner_req.reserve(8);inner_req.assign(data_req_1,data_req_1+8); inner_res.reserve(8);inner_res.assign(data_res_1,data_res_1+8);
    this->req_arr.push_back(inner_req);this->res_arr.push_back(inner_res);
    inner_req.clear(); inner_res.clear();
    

  
    // -------------------- HOCHLAUFRAMPE SENKEN LASTTEIL 2330 SUBINDX 06-------------------------
    assign_arr(data_req_1, 0x40, 0x30, 0x23, 0x06, 0x00, 0x00, 0x00, 0x00 );
    assign_arr(data_res_1, 0x4B, 0x30, 0x23, 0x06, ramp_senken_lastteil_0, 0x00, 0x00, 0x00 );

    inner_req.reserve(8);inner_req.assign(data_req_1,data_req_1+8); inner_res.reserve(8);inner_res.assign(data_res_1,data_res_1+8);
    this->req_arr.push_back(inner_req);this->res_arr.push_back(inner_res);
    inner_req.clear(); inner_res.clear();
    

   

    // -------------------- AUSLAUFRAMPE SENKEN LASTTEIL 2331 SUBINDX 02-------------------------
    assign_arr(data_req_1, 0x40, 0x31, 0x23, 0x02, 0x00, 0x00, 0x00, 0x00 );
    assign_arr(data_res_1, 0x4B, 0x31, 0x23, 0x02, auslauf_senken_lastteil_0, 0x00, 0x00, 0x00 );

    inner_req.reserve(8);inner_req.assign(data_req_1,data_req_1+8); inner_res.reserve(8);inner_res.assign(data_res_1,data_res_1+8);
    this->req_arr.push_back(inner_req);this->res_arr.push_back(inner_res);
    inner_req.clear(); inner_res.clear();
   

   

    // -------------------- AUSLAUFRAMPE SENKEN LASTTEIL 2331 SUBINDX 06-------------------------
    assign_arr(data_req_1, 0x40, 0x31, 0x23, 0x06, 0x00, 0x00, 0x00, 0x00 );
    assign_arr(data_res_1, 0x4B, 0x31, 0x23, 0x06, auslauf_senken_lastteil_0, 0x00, 0x00, 0x00 );

    inner_req.reserve(8);inner_req.assign(data_req_1,data_req_1+8); inner_res.reserve(8);inner_res.assign(data_res_1,data_res_1+8);
    this->req_arr.push_back(inner_req);this->res_arr.push_back(inner_res);
    inner_req.clear(); inner_res.clear();
  



    // -------------------- HUB OPTION 2233 SUBINDX 02-------------------------
    assign_arr(data_req_1, 0x40, 0x33, 0x22, 0x02, 0x00, 0x00, 0x00, 0x00 );
    assign_arr(data_res_1, 0x4B, 0x33, 0x22, 0x02, hub_option_0, 0x00, 0x00, 0x00 );

    inner_req.reserve(8);inner_req.assign(data_req_1,data_req_1+8); inner_res.reserve(8);inner_res.assign(data_res_1,data_res_1+8);
    this->req_arr.push_back(inner_req);this->res_arr.push_back(inner_res);
    inner_req.clear(); inner_res.clear();
    
   

    // -------------------- HUB OPTION 2233 SUBINDX 06-------------------------
    assign_arr(data_req_1, 0x40, 0x33, 0x22, 0x06, 0x00, 0x00, 0x00, 0x00 );
    assign_arr(data_res_1, 0x4B, 0x33, 0x22, 0x06, hub_option_0, 0x00, 0x00, 0x00 );

    inner_req.reserve(8);inner_req.assign(data_req_1,data_req_1+8); inner_res.reserve(8);inner_res.assign(data_res_1,data_res_1+8);
    this->req_arr.push_back(inner_req);this->res_arr.push_back(inner_res);
    inner_req.clear(); inner_res.clear();
    

   

    // -------------------- HOCHLAUFRAMPE SENKEN PLATFORM 2334 SUBINDX 02-------------------------
    assign_arr(data_req_1, 0x40, 0x34, 0x23, 0x02, 0x00, 0x00, 0x00, 0x00 );
    assign_arr(data_res_1, 0x4B, 0x34, 0x23, 0x02, ramp_senken_platform, 0x00, 0x00, 0x00 );

    inner_req.reserve(8);inner_req.assign(data_req_1,data_req_1+8); inner_res.reserve(8);inner_res.assign(data_res_1,data_res_1+8);
    this->req_arr.push_back(inner_req);this->res_arr.push_back(inner_res);
    inner_req.clear(); inner_res.clear();
    

 

    // -------------------- HOCHLAUFRAMPE SENKEN PLATFORM 2334 SUBINDX 06-------------------------
    assign_arr(data_req_1, 0x40, 0x34, 0x23, 0x06, 0x00, 0x00, 0x00, 0x00 );
    assign_arr(data_res_1, 0x4B, 0x34, 0x23, 0x06 , ramp_senken_platform, 0x00, 0x00, 0x00 );

    inner_req.reserve(8);inner_req.assign(data_req_1,data_req_1+8); inner_res.reserve(8);inner_res.assign(data_res_1,data_res_1+8);
    this->req_arr.push_back(inner_req);this->res_arr.push_back(inner_res);
    inner_req.clear(); inner_res.clear();
    

  

    // -------------------- AUSLAUFRAMPE SENKEN PLATFORM 2335 SUBINDX 02-------------------------
    assign_arr(data_req_1, 0x40, 0x35, 0x23, 0x02, 0x00, 0x00, 0x00, 0x00 );
    assign_arr(data_res_1, 0x4B, 0x35, 0x23, 0x02 , auslauf_senken_platform, 0x00, 0x00, 0x00 );

    inner_req.reserve(8);inner_req.assign(data_req_1,data_req_1+8); inner_res.reserve(8);inner_res.assign(data_res_1,data_res_1+8);
    this->req_arr.push_back(inner_req);this->res_arr.push_back(inner_res);
    inner_req.clear(); inner_res.clear();
    

   


    // -------------------- AUSLAUFRAMPE SENKEN PLATFORM 2335 SUBINDX 06-------------------------
    assign_arr(data_req_1, 0x40, 0x35, 0x23, 0x06, 0x00, 0x00, 0x00, 0x00 );
    assign_arr(data_res_1, 0x4B, 0x35, 0x23, 0x06 , auslauf_senken_platform, 0x00, 0x00, 0x00 );

    inner_req.reserve(8);inner_req.assign(data_req_1,data_req_1+8); inner_res.reserve(8);inner_res.assign(data_res_1,data_res_1+8);
    this->req_arr.push_back(inner_req);this->res_arr.push_back(inner_res);
    inner_req.clear(); inner_res.clear();
    


    // -------------------- OPTION ZUGANGSMODUL 2960 SUBINDX 02-------------------------
    assign_arr(data_req_1, 0x40, 0x60, 0x29, 0x02, 0x00, 0x00, 0x00, 0x00 );
    assign_arr(data_res_1, 0x4B, 0x60, 0x29, 0x02, zugang_0, 0x00, 0x00, 0x00 );

    inner_req.reserve(8);inner_req.assign(data_req_1,data_req_1+8); inner_res.reserve(8);inner_res.assign(data_res_1,data_res_1+8);
    this->req_arr.push_back(inner_req);this->res_arr.push_back(inner_res);
    inner_req.clear(); inner_res.clear();
    
   


    // -------------------- OPTION ZUGANGSMODUL 2960 SUBINDX 06-------------------------
    assign_arr(data_req_1, 0x40, 0x60, 0x29, 0x06, 0x00, 0x00, 0x00, 0x00 );
    assign_arr(data_res_1, 0x4B, 0x60, 0x29, 0x06, zugang_0, 0x00, 0x00, 0x00 );

    inner_req.reserve(8);inner_req.assign(data_req_1,data_req_1+8); inner_res.reserve(8);inner_res.assign(data_res_1,data_res_1+8);
    this->req_arr.push_back(inner_req);this->res_arr.push_back(inner_res);
    inner_req.clear(); inner_res.clear();
    

   
    // -------------------- BATTERIE TYP 2561 SUBINDX 02-------------------------
    assign_arr(data_req_1, 0x40, 0x61, 0x25, 0x02, 0x00, 0x00, 0x00, 0x00 );
    assign_arr(data_res_1, 0x4B, 0x61, 0x25, 0x02, batterie_typ, 0x00, 0x00, 0x00 );

    inner_req.reserve(8);inner_req.assign(data_req_1,data_req_1+8); inner_res.reserve(8);inner_res.assign(data_res_1,data_res_1+8);
    this->req_arr.push_back(inner_req);this->res_arr.push_back(inner_res);
    inner_req.clear(); inner_res.clear();
   

    


    // -------------------- BATTERIE TYP 2561 SUBINDX 06-------------------------
    assign_arr(data_req_1, 0x40, 0x61, 0x25, 0x06, 0x00, 0x00, 0x00, 0x00 );
    assign_arr(data_res_1, 0x4B, 0x61, 0x25, 0x06, batterie_typ, 0x00, 0x00, 0x00 );

    inner_req.reserve(8);inner_req.assign(data_req_1,data_req_1+8); inner_res.reserve(8);inner_res.assign(data_res_1,data_res_1+8);
    this->req_arr.push_back(inner_req);this->res_arr.push_back(inner_res);
    inner_req.clear(); inner_res.clear();
  

   

    // -------------------- BATTERIE CAPACITY 2562 SUBINDX 02-------------------------
    assign_arr(data_req_1, 0x40, 0x62, 0x25, 0x02, 0x00, 0x00, 0x00, 0x00 );
    assign_arr(data_res_1, 0x4B, 0x62, 0x25, 0x02, batterie_kapazitat_0, batterie_kapazitat_1, 0x00, 0x00 );

    inner_req.reserve(8);inner_req.assign(data_req_1,data_req_1+8); inner_res.reserve(8);inner_res.assign(data_res_1,data_res_1+8);
    this->req_arr.push_back(inner_req);this->res_arr.push_back(inner_res);
    inner_req.clear(); inner_res.clear();
  

    

    // -------------------- BATTERIE CAPACITY 2562 SUBINDX 06-------------------------
    assign_arr(data_req_1, 0x40, 0x62, 0x25, 0x06, 0x00, 0x00, 0x00, 0x00 );
    assign_arr(data_res_1, 0x4B, 0x62, 0x25, 0x06, batterie_kapazitat_0, batterie_kapazitat_1, 0x00, 0x00 );

    inner_req.reserve(8);inner_req.assign(data_req_1,data_req_1+8); inner_res.reserve(8);inner_res.assign(data_res_1,data_res_1+8);
    this->req_arr.push_back(inner_req);this->res_arr.push_back(inner_res);
    inner_req.clear(); inner_res.clear();
    */

//}

String create_str(std::vector<__u8> data, __u16 can_id, int can_dlc)
{
  String s;
  std::vector<__u8>::const_iterator it2;
  s.reserve(25);
  
  s += String(can_id, HEX) + " ";

  for (it2 = data.begin(); it2 != data.end(); it2++)
  {
    s += String(*it2,HEX) + " ";
  }

  return s;
}

/*
void LOITRUCK::create_CAN_Expedited_map()
{
    std::vector<std::vector<__u8>>::const_iterator outer_it, outer_it2;
    
    outer_it2 = this->res_arr.begin();
    for (outer_it = this->req_arr.begin(); outer_it != this->req_arr.end(); outer_it++){
      // Anzeige 643
      this->CAN_Expedited_Map.insert(std::pair<String,String>(create_str(*outer_it, 0x643, 8), create_str(*outer_it2, prepare_ID(0x643), 8)));  
      // Master 641
      this->CAN_Expedited_Map.insert(std::pair<String,String>(create_str(*outer_it, 0x641, 8), create_str(*outer_it2, prepare_ID(0x641), 8)));  
      // Lenkung 644
      this->CAN_Expedited_Map.insert(std::pair<String,String>(create_str(*outer_it, 0x644, 8), create_str(*outer_it2, prepare_ID(0x644), 8)));  

      outer_it2++;
    }
}
*/

void LOITRUCK::create_CAN_Segmented_map()
{
    this->CAN_Segmented_Map.clear();
    
    __u8 req_arr[] = {0x40, 0x02, 0x20, 0x04, 0x00, 0x00, 0x00, 0x00};
    __u8 res_arr[] = {0x41, 0x02, 0x20, 0x04, 0x20, 0x00, 0x00, 0x00};
    can_frame res = create_CAN_frame(0x601, 8, req_arr);
    can_frame req = create_CAN_frame(0x581, 8, res_arr);
    this->CAN_Segmented_Map.insert(std::pair<String,String>(create_str_from_frame(res),create_str_from_frame(req)));

}


/*
int LOITRUCK::create_req_res_arr(std::vector<std::vector<uint8_t>> & req_arr, std::vector<std::vector<uint8_t>> & res_arr {
    

    req_arr.clear(;
    res_arr.clear(;

    // -------------------- SERIEN NUMMER 2002-------------------------
    __u8 data_req_6[] = { 0x40, 0x02, 0x20, 0x01, 0x00, 0x00, 0x00, 0x00 };
    __u8 data_res_6[] = { 0x43, 0x02, 0x20, 0x01, Seri0_HAPPY, Seri1_HAPPY, Seri2_HAPPY, Seri3_HAPPY };
    
    // -------------------- SW MATERIAL NUMMER 2000-------------------------
    __u8 data_req_6[] = { 0x40, 0x00, 0x20, 0x01, 0x00, 0x00, 0x00, 0x00 };
    __u8 data_res_6[] = { 0x43, 0x00, 0x20, 0x01, sw0, sw1, sw2, sw3 };

    // -------------------- HW MATERIAL NUMMER 2000-------------------------
    __u8 data_req_6[] = { 0x40, 0x00, 0x20, 0x03, 0x00, 0x00, 0x00, 0x00 };
    __u8 data_res_6[] = { 0x43, 0x00, 0x20, 0x03, hw0, hw1, hw2, hw3 };

    // -------------------- PRODUKTION NUMMER 2002-------------------------
    __u8 data_req_6[] = { 0x40, 0x02, 0x20, 0x02, 0x00, 0x00, 0x00, 0x00 };
    __u8 data_res_6[] = { 0x43, 0x02, 0x20, 0x02, produk0, produk1, produk2, produk3 };

    // -------------------- BETRIEBSZEIT U16 STUNDEN 2020-------------------------
    __u8 data_req_6[] = { 0x40, 0x20, 0x20, 0x01, 0x00, 0x00, 0x00, 0x00 };
    __u8 data_res_6[] = { 0x4B, 0x20, 0x20, 0x01, zeit0, zeit1, 0x00, 0x00 };

    // -------------------- BETRIEBSZEIT U16 HUNDERTMS 2020-------------------------
    __u8 data_req_6[] = { 0x40, 0x20, 0x20, 0x02, 0x00, 0x00, 0x00, 0x00 };
    __u8 data_res_6[] = { 0x4B, 0x20, 0x20, 0x02, hunder0, hunder1, 0x00, 0x00 };

    // -------------------- BESCHLEUNIGUNG FAHRPROGRAM (SPEED UP 2100-------------------------
    __u8 data_req_6[] = { 0x40, 0x00, 0x21, 0x02, 0x00, 0x00, 0x00, 0x00 };
    __u8 data_res_6[] = { 0x4B, 0x00, 0x21, 0x02, acc_fahr0_0, 0x00, 0x00, 0x00 };

    // -------------------- BESCHLEUNIGUNG MITGÄNGER FAHRPROGRAM (SPEED UP SUBINDEX 2 2102-------------------------
    __u8 data_req_6[] = { 0x40, 0x02, 0x21, 0x02, 0x00, 0x00, 0x00, 0x00 };
    __u8 data_res_6[] = { 0x4B, 0x02, 0x21, 0x02, acc_by_user_fahr0_0, 0x00, 0x00, 0x00 };

    // -------------------- BESCHLEUNIGUNG MITGÄNGER FAHRPROGRAM (SPEED UP SUBINDEX 6 2102-------------------------
    __u8 data_req_6[] = { 0x40, 0x02, 0x21, 0x06, 0x00, 0x00, 0x00, 0x00 };
    __u8 data_res_6[] = { 0x4B, 0x02, 0x21, 0x06, acc_by_user_fahr0_0, 0x00, 0x00, 0x00 };

    // -------------------- BESCHLEUNIGUNG FAHRPROGRAM (SPEED UP 2 2100???-------------------------
    __u8 data_req_6[] = { 0x40, 0x00, 0x21, 0x06, 0x00, 0x00, 0x00, 0x00 };
    __u8 data_res_6[] = { 0x4B, 0x00, 0x21, 0x06, acc_fahr0_0, acc_fahr0_1, 0x00, 0x00 };

    // -------------------- HOCHLAUFRAMPE HEBEN LASTTEIL 2200-------------------------
    __u8 data_req_6[] = { 0x40, 0x00, 0x22, 0x02, 0x00, 0x00, 0x00, 0x00 };
    __u8 data_res_6[] = { 0x4B, 0x00, 0x22, 0x02, ramp_heben_lastteil_0, 0x00, 0x00, 0x00 };

    // -------------------- HOCHLAUFRAMPE HEBEN LASTTEIL 2200-------------------------
    __u8 data_req_6[] = { 0x40, 0x00, 0x22, 0x06, 0x00, 0x00, 0x00, 0x00 };
    __u8 data_res_6[] = { 0x4B, 0x00, 0x22, 0x06, ramp_heben_lastteil_0, 0x00, 0x00, 0x00 };

    // -------------------- LOGBUCH 2001-------------------------
    __u8 data_req_6[] = { 0x40, 0x01, 0x20, 0x01, 0x00, 0x00, 0x00, 0x00 };
    __u8 data_res_6[] = { 0x4F, 0x01, 0x20, 0x01, no_Antrag_Log, 0x00, 0x00, 0x00 };

    // -------------------- BESCHLEUNIGUNG MITFAHRER SUBINDEX 02 2101-------------------------
    __u8 data_req_6[] = { 0x40, 0x01, 0x21, 0x02, 0x00, 0x00, 0x00, 0x00 };
    __u8 data_res_6[] = { 0x4B, 0x01, 0x21, 0x02, acc_by_user_fahr0_0, 0x00, 0x00, 0x00 };

    // -------------------- BESCHLEUNIGUNG MITFAHRER SUBINDEX 06 2101-------------------------
    __u8 data_req_6[] = { 0x40, 0x01, 0x21, 0x06, 0x00, 0x00, 0x00, 0x00 };
    __u8 data_res_6[] = { 0x4B, 0x01, 0x21, 0x06, acc_by_user_fahr0_0, 0x00, 0x00, 0x00 };

    // -------------------- AUSLAUFRAMPE HEBEN LASTTEIL SUBINDEX 2 2201-------------------------
    __u8 data_req_6[] = { 0x40, 0x01, 0x22, 0x02, 0x00, 0x00, 0x00, 0x00 };
    __u8 data_res_6[] = { 0x4B, 0x01, 0x22, 0x02, auslauf_heben_lastteil_0, 0x00, 0x00, 0x00 };

    // -------------------- AUSLAUFRAMPE HEBEN LASTTEIL SUB INDEX 6 2201-------------------------
    __u8 data_req_6[] = { 0x40, 0x01, 0x22, 0x06, 0x00, 0x00, 0x00, 0x00 };
    __u8 data_res_6[] = { 0x4B, 0x01, 0x22, 0x06, auslauf_heben_lastteil_0, 0x00, 0x00, 0x00 };

    // -------------------- UNKNOWN 2103 SUBINX 06-------------------------
    __u8 data_req_6[] = { 0x40, 0x03, 0x21, 0x06, 0x00, 0x00, 0x00, 0x00 };
    __u8 data_res_6[] = { 0x4B, 0x03, 0x21, 0x06, unknown_0, 0x00, 0x00, 0x00 };

    // -------------------- UNKNOWN 2103 SUBINX 02-------------------------
    __u8 data_req_6[] = { 0x40, 0x03, 0x21, 0x02, 0x00, 0x00, 0x00, 0x00 };
    __u8 data_res_6[] = { 0x4B, 0x03, 0x21, 0x02, unknown_0, 0x00, 0x00, 0x00 };

    // -------------------- AUSROLLBREMSE MITFAHRER 2104 SUBINX 02-------------------------
    __u8 data_req_6[] = { 0x40, 0x04, 0x21, 0x02, 0x00, 0x00, 0x00, 0x00 };
    __u8 data_res_6[] = { 0x4B, 0x04, 0x21, 0x02, bremse_fahr0_0, 0x00, 0x00, 0x00 };

    // -------------------- AUSROLLBREMSE MITFAHRER 2104 SUBINX 06-------------------------
    __u8 data_req_6[] = { 0x40, 0x04, 0x21, 0x06, 0x00, 0x00, 0x00, 0x00 };
    __u8 data_res_6[] = { 0x4B, 0x04, 0x21, 0x06, bremse_fahr0_0, 0x00, 0x00, 0x00 };

    // -------------------- AUSROLLBREMSE MITGÄNGER 2105 SUBINX 02-------------------------
    __u8 data_req_6[] = { 0x40, 0x05, 0x21, 0x02, 0x00, 0x00, 0x00, 0x00 };
    __u8 data_res_6[] = { 0x4B, 0x05, 0x21, 0x02, bremse_fahr0_by_user, 0x00, 0x00, 0x00 };

    // -------------------- AUSROLLBREMSE MITGÄNGER 2105 SUBINX 06-------------------------
    __u8 data_req_6[] = { 0x40, 0x05, 0x21, 0x06, 0x00, 0x00, 0x00, 0x00 };
    __u8 data_res_6[] = { 0x4B, 0x05, 0x21, 0x06, bremse_fahr0_by_user, 0x00, 0x00, 0x00 };

    // -------------------- UMKEHRBREMSE MITGÄNGER 2106 SUBINX 02-------------------------
    __u8 data_req_6[] = { 0x40, 0x05, 0x21, 0x02, 0x00, 0x00, 0x00, 0x00 };
    __u8 data_res_6[] = { 0x4B, 0x05, 0x21, 0x02, bremse_fahr0_umkehr, 0x00, 0x00, 0x00 };

    // -------------------- UMKEHRBREMSE MITGÄNGER 2106 SUBINX 06-------------------------
    __u8 data_req_6[] = { 0x40, 0x05, 0x21, 0x06, 0x00, 0x00, 0x00, 0x00 };
    __u8 data_res_6[] = { 0x4B, 0x05, 0x21, 0x06, bremse_fahr0_umkehr, 0x00, 0x00, 0x00 };

    // -------------------- UNKNOWN 2107 SUBINX 02-------------------------
    __u8 data_req_6[] = { 0x40, 0x07, 0x21, 0x02, 0x00, 0x00, 0x00, 0x00 };
    __u8 data_res_6[] = { 0x4B, 0x07, 0x21, 0x02, unknown_1, 0x00, 0x00, 0x00 };

    // -------------------- UNKNOWN 2107 SUBINX 06-------------------------
    __u8 data_req_6[] = { 0x40, 0x07, 0x21, 0x06, 0x00, 0x00, 0x00, 0x00 };
    __u8 data_res_6[] = { 0x4B, 0x07, 0x21, 0x06, unknown_1, 0x00, 0x00, 0x00 };


    // -------------------- ANTRIEB SPEED 2108 SUBINX 06-------------------------
    __u8 data_req_6[] = { 0x40, 0x08, 0x21, 0x06, 0x00, 0x00, 0x00, 0x00 };
    __u8 data_res_6[] = { 0x4B, 0x08, 0x21, 0x06, antrieb_speed_fahr0_0, 0x00, 0x00, 0x00 };

    // -------------------- ANTRIEB SPEED 2108 SUBINX 02-------------------------
    __u8 data_req_6[] = { 0x40, 0x08, 0x21, 0x02, 0x00, 0x00, 0x00, 0x00 };
    __u8 data_res_6[] = { 0x4B, 0x08, 0x21, 0x02, antrieb_speed_fahr0_0, 0x00, 0x00, 0x00 };

    // -------------------- ANTRIEB BY USER 2109 SUBINX 02-------------------------
    __u8 data_req_6[] = { 0x40, 0x09, 0x21, 0x02, 0x00, 0x00, 0x00, 0x00 };
    __u8 data_res_6[] = { 0x4B, 0x09, 0x21, 0x02, antrieb_speed_fahr0_by_user, 0x00, 0x00, 0x00 };

    // -------------------- ANTRIEB BY USER 2109 SUBINX 06-------------------------
    __u8 data_req_6[] = { 0x40, 0x09, 0x21, 0x06, 0x00, 0x00, 0x00, 0x00 };
    __u8 data_res_6[] = { 0x4B, 0x09, 0x21, 0x06, antrieb_speed_fahr0_by_user, 0x00, 0x00, 0x00 };

    // -------------------- ANTRIEB RÜCKEN BY USER 210A SUBINX 02-------------------------
    __u8 data_req_6[] = { 0x40, 0x0A, 0x21, 0x02, 0x00, 0x00, 0x00, 0x00 };
    __u8 data_res_6[] = { 0x4B, 0x0A, 0x21, 0x02, rucken_fahr0_speed_0, 0x00, 0x00, 0x00 };

    // -------------------- GABEL SPEED 210C SUBINX 06-------------------------
    __u8 data_req_6[] = { 0x40, 0x0C, 0x21, 0x06, 0x00, 0x00, 0x00, 0x00 };
    __u8 data_res_6[] = { 0x4B, 0x0C, 0x21, 0x06, gabel_speed_fahr0_0, 0x00, 0x00, 0x00 };

    // -------------------- ANTRIEB RÜCKEN BY USER 210A SUBINX 06-------------------------
    __u8 data_req_6[] = { 0x40, 0x0A, 0x21, 0x06, 0x00, 0x00, 0x00, 0x00 };
    __u8 data_res_6[] = { 0x4B, 0x0A, 0x21, 0x06, rucken_fahr0_speed_0, 0x00, 0x00, 0x00 };

    // -------------------- GABEL SPEED 210C SUBINX 02-------------------------
    __u8 data_req_6[] = { 0x40, 0x0C, 0x21, 0x02, 0x00, 0x00, 0x00, 0x00 };
    __u8 data_res_6[] = { 0x4B, 0x0C, 0x21, 0x02, gabel_speed_fahr0_0, 0x00, 0x00, 0x00 };

    // -------------------- GABEL BY USER 210D SUBINX 02-------------------------
    __u8 data_req_6[] = { 0x40, 0x0D, 0x21, 0x02, 0x00, 0x00, 0x00, 0x00 };
    __u8 data_res_6[] = { 0x4B, 0x0D, 0x21, 0x02, gabel_speed_fahr0_by_user, 0x00, 0x00, 0x00 };

    // -------------------- GABEL BY USER 210D SUBINX 06-------------------------
    __u8 data_req_6[] = { 0x40, 0x0D, 0x21, 0x06, 0x00, 0x00, 0x00, 0x00 };
    __u8 data_res_6[] = { 0x4B, 0x0D, 0x21, 0x06, gabel_speed_fahr0_by_user, 0x00, 0x00, 0x00 };

    // -------------------- GABEL RÜCKEN 210E SUBINX 02-------------------------
    __u8 data_req_6[] = { 0x40, 0x0E, 0x21, 0x02, 0x00, 0x00, 0x00, 0x00 };
    __u8 data_res_6[] = { 0x4B, 0x0E, 0x21, 0x02, rucken_gabel_speed_fahr0_0, 0x00, 0x00, 0x00 };

    // -------------------- GABEL RÜCKEN 210E SUBINX 06-------------------------
    __u8 data_req_6[] = { 0x40, 0x0E, 0x21, 0x06, 0x00, 0x00, 0x00, 0x00 };
    __u8 data_res_6[] = { 0x4B, 0x0E, 0x21, 0x06, rucken_gabel_speed_fahr0_0, 0x00, 0x00, 0x00 };

    // -------------------- BESCHLEUNIGUNG 2110 FAHRPROGRAM 2 SUBINX 02-------------------------
    __u8 data_req_6[] = { 0x40, 0x10, 0x21, 0x02, 0x00, 0x00, 0x00, 0x00 };
    __u8 data_res_6[] = { 0x4B, 0x10, 0x21, 0x02, acc_fahr1_0, 0x00, 0x00, 0x00 };

    // -------------------- BESCHLEUNIGUNG 2110 FAHRPROGRAM 2 SUBINX 06-------------------------
    __u8 data_req_6[] = { 0x40, 0x10, 0x21, 0x06, 0x00, 0x00, 0x00, 0x00 };
    __u8 data_res_6[] = { 0x4B, 0x10, 0x21, 0x06, acc_fahr1_0, 0x00, 0x00, 0x00 };

    // -------------------- BESCHLEUNIGUNG BY USER 2111 FAHRPROGRAM 2 SUBINX 02-------------------------
    __u8 data_req_6[] = { 0x40, 0x11, 0x21, 0x02, 0x00, 0x00, 0x00, 0x00 };
    __u8 data_res_6[] = { 0x4B, 0x11, 0x21, 0x02, acc_by_user_fahr1_0, 0x00, 0x00, 0x00 };

    // -------------------- BESCHLEUNIGUNG BY USER 2111 FAHRPROGRAM 2 SUBINX 06-------------------------
    __u8 data_req_6[] = { 0x40, 0x11, 0x21, 0x06, 0x00, 0x00, 0x00, 0x00 };
    __u8 data_res_6[] = { 0x4B, 0x11, 0x21, 0x06, acc_by_user_fahr1_0, 0x00, 0x00, 0x00 };

    // -------------------- BESCHLEUNIGUNG RÜCKEN BY USER 2112 FAHRPROGRAM 2 SUBINX 02-------------------------
    __u8 data_req_6[] = { 0x40, 0x12, 0x21, 0x02, 0x00, 0x00, 0x00, 0x00 };
    __u8 data_res_6[] = { 0x4B, 0x12, 0x21, 0x02, acc_by_user_fahr1_0, 0x00, 0x00, 0x00 };

    // -------------------- BESCHLEUNIGUNG RÜCKEN BY USER 2112 FAHRPROGRAM 2 SUBINX 02-------------------------
    __u8 data_req_6[] = { 0x40, 0x12, 0x21, 0x06, 0x00, 0x00, 0x00, 0x00 };
    __u8 data_res_6[] = { 0x4B, 0x12, 0x21, 0x06, acc_by_user_fahr1_0, 0x00, 0x00, 0x00 };

    // -------------------- KENNLINIE 2411 SUBINDX 02-------------------------
    __u8 data_req_6[] = { 0x40, 0x11, 0x21, 0x02, 0x00, 0x00, 0x00, 0x00 };
    __u8 data_res_6[] = { 0x4B, 0x11, 0x21, 0x02, kenn_0, 0x00, 0x00, 0x00 };

    // -------------------- KENNLINIE 2411 SUBINDEX 06-------------------------
    __u8 data_req_6[] = { 0x40, 0x11, 0x21, 0x06, 0x00, 0x00, 0x00, 0x00 };
    __u8 data_res_6[] = { 0x4B, 0x11, 0x21, 0x06, kenn_0, 0x00, 0x00, 0x00 };

    // -------------------- KENNLINIE 2411-------------------------
    __u8 data_req_6[] = { 0x40, 0x11, 0x21, 0x06, 0x00, 0x00, 0x00, 0x00 };
    __u8 data_res_6[] = { 0x4B, 0x11, 0x21, 0x06, kenn_0, 0x00, 0x00, 0x00 };

    // -------------------- EINHEITEN UMSCHALTUNG 2612 SUBINDEX 2-------------------------
    __u8 data_req_6[] = { 0x40, 0x12, 0x26, 0x02, 0x00, 0x00, 0x00, 0x00 };
    __u8 data_res_6[] = { 0x4B, 0x12, 0x26, 0x02, umschalt_0, 0x00, 0x00, 0x00 };

    // -------------------- EINHEITEN UMSCHALTUNG 2612 SUBINDEX 6-------------------------
    __u8 data_req_6[] = { 0x40, 0x12, 0x26, 0x06, 0x00, 0x00, 0x00, 0x00 };
    __u8 data_res_6[] = { 0x4B, 0x12, 0x26, 0x06, umschalt_0, 0x00, 0x00, 0x00 };

    // -------------------- LENKKORREKTUR 2413 SUBINDEX 2-------------------------
    __u8 data_req_6[] = { 0x40, 0x13, 0x24, 0x02, 0x00, 0x00, 0x00, 0x00 };
    __u8 data_res_6[] = { 0x4B, 0x13, 0x24, 0x02, lenken_korrektur_0, 0x00, 0x00, 0x00 };

    // -------------------- LENKKORREKTUR 2413 SUBINDEX 6-------------------------
    __u8 data_req_6[] = { 0x40, 0x13, 0x24, 0x06, 0x00, 0x00, 0x00, 0x00 };
    __u8 data_res_6[] = { 0x4B, 0x13, 0x24, 0x06, lenken_korrektur_0, 0x00, 0x00, 0x00 };

    // -------------------- AUSROLLBREMSE MITFAHRER FAHRPROGRAM 2 2114 SUBINDEX 2-------------------------
    __u8 data_req_6[] = { 0x40, 0x14, 0x21, 0x02, 0x00, 0x00, 0x00, 0x00 };
    __u8 data_res_6[] = { 0x4B, 0x14, 0x21, 0x02, bremse_fahr1_0, 0x00, 0x00, 0x00 };

    // -------------------- AUSROLLBREMSE MITFAHRER FAHRPROGRAM 2 2114 SUBINDEX 2-------------------------
    __u8 data_req_6[] = { 0x40, 0x14, 0x21, 0x02, 0x00, 0x00, 0x00, 0x00 };
    __u8 data_res_6[] = { 0x4B, 0x14, 0x21, 0x02, bremse_fahr1_0, 0x00, 0x00, 0x00 };

    // -------------------- LENKENUBERSETZUNG 2414 SUBINDX 02-------------------------
    __u8 data_req_6[] = { 0x40, 0x14, 0x24, 0x02, 0x00, 0x00, 0x00, 0x00 };
    __u8 data_res_6[] = { 0x4B, 0x14, 0x24, 0x02, lenken_ubersetzung, 0x00, 0x00, 0x00 };

    // -------------------- LENKENUBERSETZUNG 2414 SUBINDX 02-------------------------
    __u8 data_req_6[] = { 0x40, 0x14, 0x24, 0x06, 0x00, 0x00, 0x00, 0x00 };
    __u8 data_res_6[] = { 0x4B, 0x14, 0x24, 0x06, lenken_ubersetzung, 0x00, 0x00, 0x00 };

    // -------------------- AUSROLLBREMSE MITGÄNGER FAHRPROGRAM 2 2115 SUBINDX 02-------------------------
    __u8 data_req_6[] = { 0x40, 0x15, 0x21, 0x02, 0x00, 0x00, 0x00, 0x00 };
    __u8 data_res_6[] = { 0x4B, 0x15, 0x21, 0x02, bremse_fahr1_by_user, 0x00, 0x00, 0x00 };

    // -------------------- AUSROLLBREMSE MITGÄNGER FAHRPROGRAM 2 2115 SUBINDX 06-------------------------
    __u8 data_req_6[] = { 0x40, 0x15, 0x21, 0x06, 0x00, 0x00, 0x00, 0x00 };
    __u8 data_res_6[] = { 0x4B, 0x15, 0x21, 0x06, bremse_fahr1_by_user, 0x00, 0x00, 0x00 };

    // -------------------- MAXIMALGEGSCHWINDIGKEIT ANTRIEBSRICHTUNG IM FP2 2118 SUBINDEX 2-------------------------
    __u8 data_req_6[] = { 0x40, 0x18, 0x21, 0x02, 0x00, 0x00, 0x00, 0x00 };
    __u8 data_res_6[] = { 0x4B, 0x18, 0x21, 0x02, antrieb_speed_fahr1_0, 0x00, 0x00, 0x00 };

    // -------------------- MAXIMALGEGSCHWINDIGKEIT ANTRIEBSRICHTUNG IM FP2 2118 SUBINDEX 6-------------------------
    __u8 data_req_6[] = { 0x40, 0x18, 0x21, 0x06, 0x00, 0x00, 0x00, 0x00 };
    __u8 data_res_6[] = { 0x4B, 0x18, 0x21, 0x06, antrieb_speed_fahr1_0, 0x00, 0x00, 0x00 };

    // -------------------- HOCHLAUFRAMPE HEBEN PLATFORM 2218 SUBINDX 2-------------------------
    __u8 data_req_6[] = { 0x40, 0x18, 0x22, 0x02, 0x00, 0x00, 0x00, 0x00 };
    __u8 data_res_6[] = { 0x4B, 0x18, 0x22, 0x02, ramp_heben_0, 0x00, 0x00, 0x00 };

    // -------------------- HOCHLAUFRAMPE HEBEN PLATFORM 2218 SUBINDX 6-------------------------
    __u8 data_req_6[] = { 0x40, 0x18, 0x22, 0x06, 0x00, 0x00, 0x00, 0x00 };
    __u8 data_res_6[] = { 0x4B, 0x18, 0x22, 0x026, ramp_heben_0, 0x00, 0x00, 0x00 };

    // -------------------- MITGÄNGERGESCHWINDIGKEIT ANTRIEB BY USER IN FP2 2119 SUBINDX 2-------------------------
    __u8 data_req_6[] = { 0x40, 0x19, 0x21, 0x02, 0x00, 0x00, 0x00, 0x00 };
    __u8 data_res_6[] = { 0x4B, 0x19, 0x21, 0x02, antrieb_speed_fahr1_by_user, 0x00, 0x00, 0x00 };

    // -------------------- MITGÄNGERGESCHWINDIGKEIT ANTRIEB BY USER IN FP2 2119 SUBINDX 6-------------------------
    __u8 data_req_6[] = { 0x40, 0x19, 0x21, 0x06, 0x00, 0x00, 0x00, 0x00 };
    __u8 data_res_6[] = { 0x4B, 0x19, 0x21, 0x06, antrieb_speed_fahr1_by_user, 0x00, 0x00, 0x00 };

    // -------------------- AUSLAUFRAMPE HEBEN 2219 SUBINDX 2-------------------------
    __u8 data_req_6[] = { 0x40, 0x19, 0x22, 0x02, 0x00, 0x00, 0x00, 0x00 };
    __u8 data_res_6[] = { 0x4B, 0x19, 0x22, 0x02, auslauf_heben_0, 0x00, 0x00, 0x00 };

    // -------------------- AUSLAUFRAMPE HEBEN 2219 SUBINDX 6-------------------------
    __u8 data_req_6[] = { 0x40, 0x19, 0x22, 0x06, 0x00, 0x00, 0x00, 0x00 };
    __u8 data_res_6[] = { 0x4B, 0x19, 0x22, 0x06, auslauf_heben_0, 0x00, 0x00, 0x00 };

    // -------------------- ANTRIEB RÜCKEN BY USER GESCHWINDIGKEIT FP2 211A SUBINDX 2-------------------------
    __u8 data_req_6[] = { 0x40, 0x1A, 0x21, 0x02, 0x00, 0x00, 0x00, 0x00 };
    __u8 data_res_6[] = { 0x4B, 0x1A, 0x21, 0x02, antrieb_speed_fahr1_by_user, 0x00, 0x00, 0x00 };

    // -------------------- ANTRIEB RÜCKEN BY USER GESCHWINDIGKEIT FP2 211A SUBINDX 6-------------------------
    __u8 data_req_6[] = { 0x40, 0x1A, 0x21, 0x06, 0x00, 0x00, 0x00, 0x00 };
    __u8 data_res_6[] = { 0x4B, 0x1A, 0x21, 0x06, antrieb_speed_fahr1_by_user, 0x00, 0x00, 0x00 };

    // -------------------- GABEL GESCHWINDIGKEIT FP2 211C SUBINDX 2-------------------------
    __u8 data_req_6[] = { 0x40, 0x1C, 0x21, 0x02, 0x00, 0x00, 0x00, 0x00 };
    __u8 data_res_6[] = { 0x4B, 0x1C, 0x21, 0x02, gabel_speed_fahr1_0, 0x00, 0x00, 0x00 };

    // -------------------- GABEL GESCHWINDIGKEIT FP2 211C SUBINDX 6-------------------------
    __u8 data_req_6[] = { 0x40, 0x1C, 0x21, 0x06, 0x00, 0x00, 0x00, 0x00 };
    __u8 data_res_6[] = { 0x4B, 0x1C, 0x21, 0x06, gabel_speed_fahr1_0, 0x00, 0x00, 0x00 };

    // -------------------- GABEL BY USER GESCHWINDIGKEIT FP2 211D SUBINDX 2-------------------------
    __u8 data_req_6[] = { 0x40, 0x1D, 0x21, 0x02, 0x00, 0x00, 0x00, 0x00 };
    __u8 data_res_6[] = { 0x4B, 0x1D, 0x21, 0x02, gabel_speed_fahr1_by_user, 0x00, 0x00, 0x00 };

    // -------------------- GABEL BY USER GESCHWINDIGKEIT FP2 211D SUBINDX 6-------------------------
    __u8 data_req_6[] = { 0x40, 0x1D, 0x21, 0x06, 0x00, 0x00, 0x00, 0x00 };
    __u8 data_res_6[] = { 0x4B, 0x1D, 0x21, 0x06, gabel_speed_fahr1_by_user, 0x00, 0x00, 0x00 };

    // -------------------- GABEL RÜCKEN BY USER GESCHWINDIGKEIT FP2 211E SUBINDX 2-------------------------
    __u8 data_req_6[] = { 0x40, 0x1E, 0x21, 0x02, 0x00, 0x00, 0x00, 0x00 };
    __u8 data_res_6[] = { 0x4B, 0x1E, 0x21, 0x02, rucken_gabel_speed_fahr1_0, 0x00, 0x00, 0x00 };

    // -------------------- GABEL RÜCKEN BY USER GESCHWINDIGKEIT FP2 211E SUBINDX 6-------------------------
    __u8 data_req_6[] = { 0x40, 0x1E, 0x21, 0x06, 0x00, 0x00, 0x00, 0x00 };
    __u8 data_res_6[] = { 0x4B, 0x1E, 0x21, 0x06, rucken_gabel_speed_fahr1_0, 0x00, 0x00, 0x00 };

    // -------------------- BESCHLEUNIGUNG FP3 SUBINDX 2-------------------------
    __u8 data_req_6[] = { 0x40, 0x21, 0x20, 0x02, 0x00, 0x00, 0x00, 0x00 };
    __u8 data_res_6[] = { 0x4B, 0x21, 0x20, 0x02, acc_fahr2_0, 0x00, 0x00, 0x00 };

    // -------------------- BESCHLEUNIGUNG FP3 SUBINDX 6-------------------------
    __u8 data_req_6[] = { 0x40, 0x21, 0x20, 0x06, 0x00, 0x00, 0x00, 0x00 };
    __u8 data_res_6[] = { 0x4B, 0x21, 0x20, 0x06, acc_fahr2_0, 0x00, 0x00, 0x00 };

    // -------------------- FAHRZEUGTYP 2029 SUBINDX 2-------------------------
    __u8 data_req_6[] = { 0x40, 0x29, 0x20, 0x02, 0x00, 0x00, 0x00, 0x00 };
    __u8 data_res_6[] = { 0x4B, 0x29, 0x20, 0x02, truck_type, 0x00, 0x00, 0x00 };

    // -------------------- FAHRZEUGTYP 2029 SUBINDX 6-------------------------
    __u8 data_req_6[] = { 0x40, 0x29, 0x20, 0x06, 0x00, 0x00, 0x00, 0x00 };
    __u8 data_res_6[] = { 0x4B, 0x29, 0x20, 0x06, truck_type, 0x00, 0x00, 0x00 };

    // -------------------- BESCHLEUNIGUNG BY USER FP3 2121 SUBINDX 2-------------------------
    __u8 data_req_6[] = { 0x40, 0x21, 0x21, 0x02, 0x00, 0x00, 0x00, 0x00 };
    __u8 data_res_6[] = { 0x4B, 0x21, 0x21, 0x02, acc_by_user_fahr2_0, 0x00, 0x00, 0x00 };

    // -------------------- BESCHLEUNIGUNG BY USER FP3 2121 SUBINDX 6-------------------------
    __u8 data_req_6[] = { 0x40, 0x21, 0x21, 0x02, 0x00, 0x00, 0x00, 0x00 };
    __u8 data_res_6[] = { 0x4B, 0x21, 0x21, 0x02, acc_by_user_fahr2_0, 0x00, 0x00, 0x00 };

    // -------------------- BESCHLEUNIGUNG RUCKEN FP3 2122 SUBINDX 2-------------------------
    __u8 data_req_6[] = { 0x40, 0x22, 0x21, 0x02, 0x00, 0x00, 0x00, 0x00 };
    __u8 data_res_6[] = { 0x4B, 0x22, 0x21, 0x02, acc_rucken_farh2_0, 0x00, 0x00, 0x00 };

    // -------------------- BESCHLEUNIGUNG RUCKEN FP3 2122 SUBINDX 6-------------------------
    __u8 data_req_6[] = { 0x40, 0x22, 0x21, 0x06, 0x00, 0x00, 0x00, 0x00 };
    __u8 data_res_6[] = { 0x4B, 0x22, 0x21, 0x06, acc_rucken_farh2_0, 0x00, 0x00, 0x00 };

    // -------------------- VERRIEGELUNG 2922 SUBINDX 2-------------------------
    __u8 data_req_6[] = { 0x40, 0x22, 0x29, 0x02, 0x00, 0x00, 0x00, 0x00 };
    __u8 data_res_6[] = { 0x4B, 0x22, 0x29, 0x02, verrie_0, 0x00, 0x00, 0x00 };

    // -------------------- VERRIEGELUNG 2922 SUBINDX 6-------------------------
    __u8 data_req_6[] = { 0x40, 0x22, 0x29, 0x06, 0x00, 0x00, 0x00, 0x00 };
    __u8 data_res_6[] = { 0x4B, 0x22, 0x29, 0x06, verrie_1, 0x00, 0x00, 0x00 };

    // -------------------- ZEIT EINFALL BREMSE UND BANDAGENENTSPANNUNG 2923 SUBINDX 2-------------------------
    __u8 data_req_6[] = { 0x40, 0x23, 0x29, 0x02, 0x00, 0x00, 0x00, 0x00 };
    __u8 data_res_6[] = { 0x4B, 0x23, 0x29, 0x02, zeit_bremse_0, 0x00, 0x00, 0x00 };

    // -------------------- ZEIT EINFALL BREMSE UND BANDAGENENTSPANNUNG 2923 SUBINDX 6-------------------------
    __u8 data_req_6[] = { 0x40, 0x23, 0x29, 0x06, 0x00, 0x00, 0x00, 0x00 };
    __u8 data_res_6[] = { 0x4B, 0x23, 0x29, 0x06, zeit_bremse_0, 0x00, 0x00, 0x00 };

    // -------------------- AUSROLLBREMSE MITFAHRER FP3 2124 SUBINDX 2-------------------------
    __u8 data_req_6[] = { 0x40, 0x24, 0x21, 0x02, 0x00, 0x00, 0x00, 0x00 };
    __u8 data_res_6[] = { 0x4B, 0x24, 0x21, 0x02, bremse_fahr2_0, 0x00, 0x00, 0x00 };

    // -------------------- AUSROLLBREMSE MITFAHRER FP3 2124 SUBINDX 6-------------------------
    __u8 data_req_6[] = { 0x40, 0x24, 0x21, 0x06, 0x00, 0x00, 0x00, 0x00 };
    __u8 data_res_6[] = { 0x4B, 0x24, 0x21, 0x06, bremse_fahr2_0, 0x00, 0x00, 0x00 };

    // -------------------- ECO VORGABE 2924 SUBINDX 2-------------------------
    __u8 data_req_6[] = { 0x40, 0x24, 0x29, 0x02, 0x00, 0x00, 0x00, 0x00 };
    __u8 data_res_6[] = { 0x4B, 0x24, 0x29, 0x02, eco_0, 0x00, 0x00, 0x00 };

    // -------------------- ECO VORGABE 2924 SUBINDX 6-------------------------
    __u8 data_req_6[] = { 0x40, 0x24, 0x29, 0x06, 0x00, 0x00, 0x00, 0x00 };
    __u8 data_res_6[] = { 0x4B, 0x24, 0x29, 0x06, eco_0, 0x00, 0x00, 0x00 };

    // -------------------- AUSROLLBREMSE BY USER FP3 2125 SUNINDX 2-------------------------
    __u8 data_req_6[] = { 0x40, 0x25, 0x21, 0x02, 0x00, 0x00, 0x00, 0x00 };
    __u8 data_res_6[] = { 0x4B, 0x25, 0x21, 0x02, bremse_fahr2_by_user, 0x00, 0x00, 0x00 };


    // -------------------- AUSROLLBREMSE BY USER FP3 2125 SUNINDX 6-------------------------
    __u8 data_req_6[] = { 0x40, 0x25, 0x21, 0x06, 0x00, 0x00, 0x00, 0x00 };
    __u8 data_res_6[] = { 0x4B, 0x25, 0x21, 0x06, bremse_fahr2_by_user, 0x00, 0x00, 0x00 };

    // -------------------- MAXIMALGESCHWINDIGKEIT ANTRIEBSRICHTUNG FP3 2128 SUNINDX 2-------------------------
    __u8 data_req_6[] = { 0x40, 0x28, 0x21, 0x02, 0x00, 0x00, 0x00, 0x00 };
    __u8 data_res_6[] = { 0x4B, 0x28, 0x21, 0x02, antrieb_speed_fahr2_0, 0x00, 0x00, 0x00 };


    // -------------------- MAXIMALGESCHWINDIGKEIT ANTRIEBSRICHTUNG FP3 2128 SUNINDX 6-------------------------
    __u8 data_req_6[] = { 0x40, 0x28, 0x21, 0x02, 0x00, 0x00, 0x00, 0x00 };
    __u8 data_res_6[] = { 0x4B, 0x28, 0x21, 0x02, antrieb_speed_fahr2_0_indx6, 0x00, 0x00, 0x00 };

    // -------------------- GESCHWINDIGKEIT BY USER ANTRIEBSRICHTUNG FP3 2129 SUNINDX 2-------------------------
    __u8 data_req_6[] = { 0x40, 0x29, 0x21, 0x02, 0x00, 0x00, 0x00, 0x00 };
    __u8 data_res_6[] = { 0x4B, 0x29, 0x21, 0x02, antrieb_speed_fahr2_by_user, 0x00, 0x00, 0x00 };

    // -------------------- GESCHWINDIGKEIT BY USER ANTRIEBSRICHTUNG FP3 2129 SUNINDX 6-------------------------
    __u8 data_req_6[] = { 0x40, 0x29, 0x21, 0x06, 0x00, 0x00, 0x00, 0x00 };
    __u8 data_res_6[] = { 0x4B, 0x29, 0x21, 0x06, antrieb_speed_fahr2_by_user, 0x00, 0x00, 0x00 };

    // -------------------- GESCHWINDIGKEIT RÜCKEN ANTRIEBSRICHTUNG FP3 212A SUNINDX 2-------------------------
    __u8 data_req_6[] = { 0x40, 0x2A, 0x21, 0x02, 0x00, 0x00, 0x00, 0x00 };
    __u8 data_res_6[] = { 0x4B, 0x2A, 0x21, 0x02, rucken_fahr2_speed_0, 0x00, 0x00, 0x00 };


    // -------------------- GESCHWINDIGKEIT RÜCKEN ANTRIEBSRICHTUNG FP3 212A SUNINDX 2-------------------------
    __u8 data_req_6[] = { 0x40, 0x2A, 0x21, 0x06, 0x00, 0x00, 0x00, 0x00 };
    __u8 data_res_6[] = { 0x4B, 0x2A, 0x21, 0x06, rucken_fahr2_speed_0, 0x00, 0x00, 0x00 };

    // -------------------- UBERWACHUNGSZEIT BEDIENERPOSITION 292B SUNINDX 2-------------------------
    __u8 data_req_6[] = { 0x40, 0x2B, 0x29, 0x02, 0x00, 0x00, 0x00, 0x00 };
    __u8 data_res_6[] = { 0x4B, 0x2B, 0x29, 0x02, wakeup_0, 0x00, 0x00, 0x00 };


    // -------------------- UBERWACHUNGSZEIT BEDIENERPOSITION 292B SUNINDX 6-------------------------
    __u8 data_req_6[] = { 0x40, 0x2B, 0x29, 0x06, 0x00, 0x00, 0x00, 0x00 };
    __u8 data_res_6[] = { 0x4B, 0x2B, 0x29, 0x06, wakeup_0, 0x00, 0x00, 0x00 };

    // -------------------- MAXIMALGESCHWINDIGKEIT GABEL FP3 212C SUNINDX 2-------------------------
    __u8 data_req_6[] = { 0x40, 0x2C, 0x21, 0x02, 0x00, 0x00, 0x00, 0x00 };
    __u8 data_res_6[] = { 0x4B, 0x2C, 0x21, 0x02, gabel_speed_fahr2_0, 0x00, 0x00, 0x00 };

    // -------------------- MAXIMALGESCHWINDIGKEIT GABEL FP3 212C SUNINDX 6-------------------------
    __u8 data_req_6[] = { 0x40, 0x2C, 0x21, 0x02, 0x00, 0x00, 0x00, 0x00 };
    __u8 data_res_6[] = { 0x4B, 0x2C, 0x21, 0x02, gabel_speed_fahr2_0_subindex6, 0x00, 0x00, 0x00 };


    // -------------------- GESCHWINDIGKEIT BY USER GABEL FP3 212D SUNINDX 2-------------------------
    __u8 data_req_6[] = { 0x40, 0x2D, 0x21, 0x02, 0x00, 0x00, 0x00, 0x00 };
    __u8 data_res_6[] = { 0x4B, 0x2D, 0x21, 0x02, gabel_speed_fahr2_by_user, 0x00, 0x00, 0x00 };

    
    // -------------------- GESCHWINDIGKEIT BY USER GABEL FP3 212D SUNINDX 6-------------------------
    __u8 data_req_6[] = { 0x40, 0x2D, 0x21, 0x06, 0x00, 0x00, 0x00, 0x00 };
    __u8 data_res_6[] = { 0x4B, 0x2D, 0x21, 0x06, gabel_speed_fahr2_by_user, 0x00, 0x00, 0x00 };


    // -------------------- GESCHWINDIGKEIT RÜCKEN GABEL FP3 212E SUNINDX 2-------------------------
    __u8 data_req_6[] = { 0x40, 0x2E, 0x21, 0x02, 0x00, 0x00, 0x00, 0x00 };
    __u8 data_res_6[] = { 0x4B, 0x2E, 0x21, 0x02,rucken_gabel_speed_fahr2_0, 0x00, 0x00, 0x00 };

    // -------------------- GESCHWINDIGKEIT RÜCKEN GABEL FP3 212E SUNINDX 6-------------------------
    __u8 data_req_6[] = { 0x40, 0x2E, 0x21, 0x06, 0x00, 0x00, 0x00, 0x00 };
    __u8 data_res_6[] = { 0x4B, 0x2E, 0x21, 0x06,rucken_gabel_speed_fahr2_0, 0x00, 0x00, 0x00 };

 

    // -------------------- BATTERIELADEZUSTAND 2030-------------------------
    __u8 data_req_6[] = { 0x40, 0x30, 0x20, 0x00, 0x00, 0x00, 0x00, 0x00 };
    __u8 data_res_6[] = { 0x4F, 0x30, 0x20, 0x00, percent0, 0x00, 0x00, 0x00 };

    

    // -------------------- HOCHLAUFRAMPE SENKEN LASTTEIL 2330 SUBINDX 02-------------------------
    __u8 data_req_6[] = { 0x40, 0x30, 0x23, 0x02, 0x00, 0x00, 0x00, 0x00 };
    __u8 data_res_6[] = { 0x4B, 0x30, 0x23, 0x02, ramp_senken_lastteil_0, 0x00, 0x00, 0x00 };

  
    // -------------------- HOCHLAUFRAMPE SENKEN LASTTEIL 2330 SUBINDX 06-------------------------
    __u8 data_req_6[] = { 0x40, 0x30, 0x23, 0x06, 0x00, 0x00, 0x00, 0x00 };
    __u8 data_res_6[] = { 0x4B, 0x30, 0x23, 0x06, ramp_senken_lastteil_0, 0x00, 0x00, 0x00 };

   

    // -------------------- AUSLAUFRAMPE SENKEN LASTTEIL 2331 SUBINDX 02-------------------------
    __u8 data_req_6[] = { 0x40, 0x31, 0x23, 0x02, 0x00, 0x00, 0x00, 0x00 };
    __u8 data_res_6[] = { 0x4B, 0x31, 0x23, 0x02, auslauf_senken_lastteil_0, 0x00, 0x00, 0x00 };

   

    // -------------------- AUSLAUFRAMPE SENKEN LASTTEIL 2331 SUBINDX 06-------------------------
    __u8 data_req_6[] = { 0x40, 0x31, 0x23, 0x06, 0x00, 0x00, 0x00, 0x00 };
    __u8 data_res_6[] = { 0x4B, 0x31, 0x23, 0x06, auslauf_senken_lastteil_0, 0x00, 0x00, 0x00 };



    // -------------------- HUB OPTION 2233 SUBINDX 02-------------------------
    __u8 data_req_6[] = { 0x40, 0x33, 0x22, 0x02, 0x00, 0x00, 0x00, 0x00 };
    __u8 data_res_6[] = { 0x4B, 0x33, 0x22, 0x02, hub_option_0, 0x00, 0x00, 0x00 };

   

    // -------------------- HUB OPTION 2233 SUBINDX 06-------------------------
    __u8 data_req_6[] = { 0x40, 0x33, 0x22, 0x06, 0x00, 0x00, 0x00, 0x00 };
    __u8 data_res_6[] = { 0x4B, 0x33, 0x22, 0x06, hub_option_0, 0x00, 0x00, 0x00 };

   

    // -------------------- HOCHLAUFRAMPE SENKEN PLATFORM 2334 SUBINDX 02-------------------------
    __u8 data_req_6[] = { 0x40, 0x34, 0x23, 0x02, 0x00, 0x00, 0x00, 0x00 };
    __u8 data_res_6[] = { 0x4B, 0x34, 0x23, 0x02, ramp_senken_platform, 0x00, 0x00, 0x00 };

 

    // -------------------- HOCHLAUFRAMPE SENKEN PLATFORM 2334 SUBINDX 06-------------------------
    __u8 data_req_6[] = { 0x40, 0x34, 0x23, 0x06, 0x00, 0x00, 0x00, 0x00 };
    __u8 data_res_6[] = { 0x4B, 0x34, 0x23, 0x06 , ramp_senken_platform, 0x00, 0x00, 0x00 };

  

    // -------------------- AUSLAUFRAMPE SENKEN PLATFORM 2335 SUBINDX 02-------------------------
    __u8 data_req_6[] = { 0x40, 0x35, 0x23, 0x02, 0x00, 0x00, 0x00, 0x00 };
    __u8 data_res_6[] = { 0x4B, 0x35, 0x23, 0x02 , auslauf_senken_platform, 0x00, 0x00, 0x00 };

   


    // -------------------- AUSLAUFRAMPE SENKEN PLATFORM 2335 SUBINDX 06-------------------------
    __u8 data_req_6[] = { 0x40, 0x35, 0x23, 0x06, 0x00, 0x00, 0x00, 0x00 };
    __u8 data_res_6[] = { 0x4B, 0x35, 0x23, 0x06 , auslauf_senken_platform, 0x00, 0x00, 0x00 };


    // -------------------- OPTION ZUGANGSMODUL 2960 SUBINDX 02-------------------------
    __u8 data_req_6[] = { 0x40, 0x60, 0x29, 0x02, 0x00, 0x00, 0x00, 0x00 };
    __u8 data_res_6[] = { 0x4B, 0x60, 0x29, 0x02, zugang_0, 0x00, 0x00, 0x00 };

   


    // -------------------- OPTION ZUGANGSMODUL 2960 SUBINDX 06-------------------------
    __u8 data_req_6[] = { 0x40, 0x60, 0x29, 0x06, 0x00, 0x00, 0x00, 0x00 };
    __u8 data_res_6[] = { 0x4B, 0x60, 0x29, 0x06, zugang_0, 0x00, 0x00, 0x00 };

   
    // -------------------- BATTERIE TYP 2561 SUBINDX 02-------------------------
    __u8 data_req_6[] = { 0x40, 0x61, 0x25, 0x02, 0x00, 0x00, 0x00, 0x00 };
    __u8 data_res_6[] = { 0x4B, 0x61, 0x25, 0x02, batterie_typ, 0x00, 0x00, 0x00 };

    


    // -------------------- BATTERIE TYP 2561 SUBINDX 06-------------------------
    __u8 data_req_6[] = { 0x40, 0x61, 0x25, 0x06, 0x00, 0x00, 0x00, 0x00 };
    __u8 data_res_6[] = { 0x4B, 0x61, 0x25, 0x06, batterie_typ, 0x00, 0x00, 0x00 };

   

    // -------------------- BATTERIE CAPACITY 2562 SUBINDX 02-------------------------
    __u8 data_req_6[] = { 0x40, 0x62, 0x25, 0x02, 0x00, 0x00, 0x00, 0x00 };
    __u8 data_res_6[] = { 0x4B, 0x62, 0x25, 0x02, batterie_kapazitat_0, batterie_kapazitat_1, 0x00, 0x00 };

    

    // -------------------- BATTERIE CAPACITY 2562 SUBINDX 06-------------------------
    __u8 data_req_6[] = { 0x40, 0x62, 0x25, 0x06, 0x00, 0x00, 0x00, 0x00 };
    __u8 data_res_6[] = { 0x4B, 0x62, 0x25, 0x06, batterie_kapazitat_0, batterie_kapazitat_1, 0x00, 0x00 };

    return 0; 
}
*/


/*
can_frame createCanFrame(uint16_t _can_id, uint8_t _can_dlc, vector<uint8_t> _data)
{
    can_frame req;
    req.can_id = _can_id;
    req.can_dlc = _can_dlc;
    //cout << _can_dlc << endl;
    req.data = _data;
    return req;
}
*/

/*
char * hex_str(unsigned char* data, int len) {
    stringstream ss;

    ss << hex ;

    for (int i = 0; i < len; i++) {
        ss << setw(2) << setfill('0') << (int)data[i];
    }

    return ss.str();
}
*/

/*
char * create_req_str(vector<uint8_t> data, uint16_t can_id, int can_dlc)
{
  /*  char * s;
    s.reserve(100);

    unsigned char temp[] = { (uint8_t)(can_id >> 8),(uint8_t)(can_id) };

    s += hex_str(temp,2) + " ";
    s += to_string(can_dlc) + " ";

    for (int i = 0; i < data.size(); i++) {

        s += hex_str(&data.at(i), 1) + " ";
    };

    return s; 
}
*/

/*
map<char *, char *> LOITRUCK::create_CAN_Dic_Segmented() {

     map<char *, char *> temp;
     /*
    // -------------------- SERIEN NUMMER 2002-------------------------
    _data1 = { 0x40, 0x02, 0x20, 0x04, 0x00, 0x00, 0x00, 0x00 };
    _data2 = { 0x41, 0x02, 0x20, 0x04, 0x20, 0x00, 0x00, 0x00 };  // 32 bytes data
    _data3 = { 0x00, truck_name_0, truck_name_1, truck_name_2, truck_name_3, truck_name_4, truck_name_5, truck_name_6 };  // ECE225_
    _data4 = { 0x10, truck_name_6, truck_name_6, truck_name_6, truck_name_6, truck_name_6, truck_name_6, truck_name_6 };  // _______
    _data5 = { 0x00, truck_name_7, truck_name_8, truck_name_9, truck_name_10, 0x00, 0x00, 0x00 };  // 2015
    _data6 = { 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };  // add empty to enough 32 bytes
    _data7 = { 0x07, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };  // end of msg  

    dic_req_str = create_req_str(_data1, 0x641,8);

    can_arr.clear();

    for (int i = 2; i < 8; i++) {
        switch (i) {
        case 2: dic_res = createCanFrame(0x581, 8, _data2); break;
        case 3: dic_res = createCanFrame(0x581, 8, _data3); break;
        case 4: dic_res = createCanFrame(0x581, 8, _data4); break;
        case 5: dic_res = createCanFrame(0x581, 8, _data5); break;
        case 6: dic_res = createCanFrame(0x581, 8, _data6); break;
        case 7: dic_res = createCanFrame(0x581, 8, _data7); break;
        }
        can_arr.push_back(dic_res);
    }

    temp.insert(make_pair(dic_req_str, &can_arr));
    

    return temp;
};
*/

//std::map<const char *,const char *> LOITRUCK::create_CAN_Dic_Expedited() {
/*
void LOITRUCK::create_CAN_Dic_Expedited() {
    std::map<const char *, const char *> temp;
    //std::map<const char *,const char *> temp;    

    /*for (int i = 0; i < req_arr.size(); i++) {
        temp.insert(create_req_str(req_arr.at(i), 0x641, 8), create_req_str(res_arr.at(i), 0x5C1, 8));
    };

    //return temp;
};
*/

// Receive a String of Request and Output a can_fram Response
/*
uint8_t LOITRUCK::getResponse(char * req, RUN_MODE mode, bool seg, std::map<char *, char *> my_map)
{   
    // Compare req string with the map key
    return 0;
    //return (my_map.find(req)->second);
}
*/


/*
void show_Dic(map<char *, char *> dic) {
   /* 
    // PRINT Expedited Map
    map<string, string>::iterator itr;
    cout << "The segmented map dic is \n";
    cout << "\tKEY\t\t\t\tELEMENT\n";

    for (itr = dic.begin(); itr != dic.end(); itr++) {
        cout << "\t" << itr->first 
             << "\t" << itr->second << endl;
    }
   
}
*/


/*int main()
{
    map<string, string> fin_dic;
    LOITRUCK loiTruck = LOITRUCK(MODE_HAPPY);
    

    loiTruck.create_req_res_arr(req_arr, res_arr);

    // CREATE DICTIONARY
    //fin_dic_seg = loiTruck.create_CAN_Dic_Segmented();
    fin_dic = loiTruck.create_CAN_Dic_Expedited();    
    
    
    // DISPLAY DICTIONARY
    show_Dic(fin_dic);


    // CHECK RESPONSE
    
    _data1 = { 0x40, 0x61, 0x25, 0x06, 0x00, 0x00, 0x00, 0x00 };
    string req = create_req_str(_data1, 0x641, 8);
    string res = loiTruck.getResponse(req, MODE_HAPPY, false, fin_dic);
    
    cout << "CAN FRAM RESQUEST" << endl;
    cout << req << endl;
    cout << "CAN FRAME RECEIVED" << endl;
    cout << res << endl;
}*/



// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file


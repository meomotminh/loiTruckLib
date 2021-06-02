// loiTruck.cpp : This file contains the 'main' function. Program execution begins and ends there.
//


#include <iterator>
#include <stdint.h>
#include <vector>
#include <map>
#include <Arduino.h>
#include <SPI.h>
#include <mcp2515.h>
#include "loiTruck.h"


// Global Vector for data
//std::vector<std::vector<__u8>> req_arr;
//std::vector<std::vector<__u8>> res_arr; 



// Global String
String dic_req_str;
String dic_res_str;


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

void display_CAN_Frame(can_frame _toDisplay)
{   
    Serial.println("ID \t DLC \t Data");
    
    Serial.print(_toDisplay.can_id,HEX);Serial.print(" ");

    Serial.print(_toDisplay.can_dlc);Serial.print(" ");

    for (int i = 0; i<8; i++)
    {
        Serial.print(_toDisplay.data[i],HEX); Serial.print(" ");
    }
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
}

void LOITRUCK::set_expecting_Segmented_Req(int count){
    __u8 data[8];
    if (count == 0){
        assign_arr(data, 0x60, this->receive_Segment_Req.data[1], this->receive_Segment_Req.data[2], this->receive_Segment_Req.data[3], this->receive_Segment_Req.data[4], this->receive_Segment_Req.data[5], this->receive_Segment_Req.data[6], this->receive_Segment_Req.data[7]);
        
    } else {
        // toggle bit 60 - 70
        __u8 toggle_bit = (this->receive_Segment_Req.data[0]) ^ (0x10); // XOR 
        assign_arr(data, toggle_bit, this->receive_Segment_Req.data[1], this->receive_Segment_Req.data[2], this->receive_Segment_Req.data[3], this->receive_Segment_Req.data[4], this->receive_Segment_Req.data[5], this->receive_Segment_Req.data[6], this->receive_Segment_Req.data[7]);
    }
    this->expecting_Segment_frame = create_CAN_frame(0x581, 8, data);
}

can_frame LOITRUCK::get_Segmented_Response()
{
        
    String tempstr;
    can_frame toReturn;

    // if first res then find in the map

    if (this->count_segmented == 0){
        String temp = create_str_from_frame(this->receive_Segment_Req);
        std::map<String,String>::iterator it = this->CAN_Segmented_Map.find(temp);
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

can_frame LOITRUCK::get_Expedited_Response(can_frame _toGet)
{
    String tempstr = create_str_from_frame(_toGet);
    can_frame temp;

    std::map<String,String>::iterator it = this->CAN_Expedited_Map.find(tempstr);
    __u8 data[8] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

    // if found in map
    if (it != this->CAN_Expedited_Map.end()){
        tempstr = it->second;
        temp = create_frame_from_str(tempstr);
    } else {
        assign_arr(data, 0x43, _toGet.data[1], _toGet.data[2], _toGet.data[3], 0x00, 0x00, 0x00, 0x00); // all bytes count
        temp = create_CAN_frame(0x5C1, 8, data);
    }

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
    
    std::map<String,String>::iterator it = this->CAN_Segmented_Map.find(temp);
    if (it != this->CAN_Segmented_Map.end()){
        // if is Segmented then initialize
        this->initial_Segmented_Transmit(_toTest);        
        return true;
    } else return false;    
}


int LOITRUCK::create_req_res_arr()
{
    std::vector<__u8> inner_req;
    std::vector<__u8> inner_res;
    std::vector<std::vector<__u8>>::const_iterator it;
    std::vector<__u8>::const_iterator it2;
    
    // SERIENNUMMER    
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

    // -------------------- BETRIEBSZEIT U16 STUNDEN 2020-------------------------
    assign_arr(data_req_1, 0x40, 0x02, 0x20, 0x02, 0x00, 0x00, 0x00, 0x00);
    assign_arr(data_res_1, 0x43, 0x02, 0x20, 0x02, zeit0, zeit1, 0x00, 0x00);
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

    inner_req.reserve(8);inner_req.assign(data_req_1,data_req_1+8); inner_res.reserve(8);inner_res.assign(data_res_1,data_res_1+8);
    this->req_arr.push_back(inner_req);this->res_arr.push_back(inner_res);
    inner_req.clear(); inner_res.clear();
    
/*
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

}

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

void LOITRUCK::create_CAN_Expedited_map()
{
    std::vector<std::vector<__u8>>::iterator outer_it, outer_it2;
    
    outer_it2 = this->res_arr.begin();
    for (outer_it = this->req_arr.begin(); outer_it != this->req_arr.end(); outer_it++){
      this->CAN_Expedited_Map.insert(std::pair<String,String>(create_str(*outer_it, 0x641, 8), create_str(*outer_it2, 0x5C1, 8)));  
      //Serial.println("Hier 1");
      //Serial.println(create_str(*outer_it, 0x641, 8));
      outer_it2++;
    }
}

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

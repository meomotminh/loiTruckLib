//#include <cstdint>
#ifndef LOITRUCK_H
#define LOITRUCK_H

#include <SPI.h>
#include <Arduino.h>

//#include <iterator>
//#include <vector>
//#include <map>
//#include <new.cpp>

//#include <bitset>
//#include <vector>
//#include <map>




//int addL(int a, int b);

/*
struct can_frame {
    uint16_t        can_id;
    uint8_t         can_dlc;
    std::vector<uint8_t> data;
};
*/

// UTILITY FUNCTION
String create_str(std::vector<__u8> data, __u16 can_id, int can_dlc); 
void assign_arr(__u8 data[8],__u8 byte1, __u8 byte2,__u8 byte3,__u8 byte4,__u8 byte5,__u8 byte6,__u8 byte7,__u8 byte8);
can_frame create_CAN_frame(__u32 can_id, __u8 can_dlc, __u8 data[8]);
String create_str_from_frame(can_frame _toConvert);
can_frame create_frame_from_str(String _toConvert);
void display_CAN_Frame(can_frame _toDisplay);
__u16 prepare_ID(__u16 ID_req);


enum RUN_MODE {
    MODE_HAPPY,
    MODE_NORMAL,
    MODE_UNHAPPY,
    MODE_IGNORE
};



class LOITRUCK
{
public:
    RUN_MODE _runMode;
    std::vector<std::vector<__u8>> req_arr;
    std::vector<std::vector<__u8>> res_arr; 
    std::map<String, String> CAN_Expedited_Map;
    std::map<String, String> CAN_Segmented_Map;

    bool Segmented;
    can_frame receive_Segment_Req;
    can_frame expecting_Segment_frame;
    int count_segmented;

    String segmented_res_Fahrzeug_Name[5];

    enum ERROR {
        ERROR_OK = 0,
        ERROR_OVR8 = 1,
        ERROR_NONEXIST = 2
    };

    // Constructor
    LOITRUCK(RUN_MODE runMode) {
        _runMode = runMode;
    };

    // FUNCTION
    //int create_req_res_arr(std::vector<std::vector<uint8_t>> & req_arr, std::vector<std::vector<uint8_t>> & res_arr);
    //uint8_t getResponse(String req, RUN_MODE mode, bool seg, std::map<char *, char *> myMap);
    //std::map<const char *,const char *> create_CAN_Dic_Expedited();
    //void create_CAN_Dic_Expedited();
    //std::map<char *, char *> create_CAN_Dic_Segmented();
    //ERROR getSegmentedResponse(can_frame* req, can_frame res[8], RUN_MODE mode);
    int create_req_res_arr();
    void create_CAN_Expedited_map();
    void create_CAN_Segmented_map();
    bool check_Segmented(can_frame _toTest);
    can_frame get_Segmented_Response();
    can_frame get_Expedited_Response(can_frame _toGet);
    void create_segmented_res_Fahrzeug_Name();
    void set_expecting_Segmented_Req(int count);
    void initial_Segmented_Transmit(can_frame receive_frame);
    void finalise_Segmented_Transmit();
    
//private:
    // serien nummer 11_111_111
    enum SERIENUMMER : uint8_t {
        Seri0_HAPPY = 0xC7,
        Seri1_HAPPY = 0x8A,
        Seri2_HAPPY = 0xA9,
        Seri3_HAPPY = 0x00,

        Seri0_UNHAPPY = 0x8E,
        Seri1_UNHAPPY = 0x15,
        Seri2_UNHAPPY = 0x53,
        Seri3_UNHAPPY = 0x01
    };


    // produktion nummer 22_222_222
    enum PRODUKTION_NUMMER : uint8_t {
        produk0 = 0x8E,
        produk1 = 0x15,
        produk2 = 0x31,
        produk3 = 0x15
    };

    // software materialnummer 0x 51 85 49 62
    enum SW_MATERIAL_NUMMER : uint8_t {
        sw0 = 0x62,
        sw1 = 0x49,
        sw2 = 0x85,
        sw3 = 0x51
    };

    // hardware materialnummer 0x 51 22 68 01
    enum HW_MATERIAL_NUMMER : uint8_t {
        hw0 = 0x01,
        hw1 = 0x68,
        hw2 = 0x22,
        hw3 = 0x51
    };

    // betriebszeit
    enum BETRIEB_ZEIT : uint8_t {
        zeit0 = 0x49,
        zeit1 = 0x01,
        hunder0 = 0x2E,
        hunder1 = 0x74
    };

    // batterie
    enum BATTERIE_ZUSTAND : uint8_t {
        percent0 = 0x32, // 50%
        batterie_typ = 0x01, // EPzS
        batterie_kapazitat_0 = 0xD1,
        batterie_kapazitat_1 = 0x01,
    };

    // logbuch
    enum LOGBUCH : uint8_t {
        no_Antrag_Log = 0x0C, // 9 gultig antrag
    };

    // beschleunigung
    enum SPEED_UP : uint8_t {
        acc_fahr0_0 = 0x64,    // 2100
        acc_fahr0_1 = 0x00,     // 2100
        acc_by_user_fahr0_0 = 0x32,    // 2101

        acc_fahr1_0 = 0xC8,
        acc_by_user_fahr1_0 = 0x64,

        acc_fahr2_0 = 0x2C,
        acc_by_user_fahr2_0 = 0xC8,
        acc_rucken_farh2_0 = 0xE6,
    };

    // hochlauframpe        
    enum HOCHLAUFRAMPE : uint8_t {
        ramp_heben_lastteil_0 = 0x0F,
        ramp_heben_0 = 0x0A,

        ramp_senken_lastteil_0 = 0x32,
        ramp_senken_0 = 0x32,
        ramp_senken_platform = 0x14,
    };

    // auslauframpe
    enum AUSLAUFRAMPE : uint8_t {
        auslauf_heben_lastteil_0 = 0x19,
        auslauf_heben_0 = 0x28,

        auslauf_senken_lastteil_0 = 0x32,
        auslauf_senken_0 = 0x28,
        auslauf_senken_platform = 0x1E,
    };

    // fahrzeugsname
    enum TRUCK_NAME : uint8_t {
        truck_name_0 = 0x45,    // E
        truck_name_1 = 0x43,    // C
        truck_name_2 = 0x45,    // E
        truck_name_3 = 0x32,    // 2
        truck_name_4 = 0x32,    // 2
        truck_name_5 = 0x35,    // 5
        truck_name_6 = 0x20,    // _
        truck_name_7 = 0x32,    // 2
        truck_name_8 = 0x30,    // 0
        truck_name_9 = 0x31,    // 1
        truck_name_10 = 0x35,   // 5

        truck_type = 0x00   // ECE 225
    };

    // unknown
    enum UNKNOWN : uint8_t {
        unknown_0 = 0x96,   // 2103
        unknown_1 = 0x82,   // 2107
    };

    // Ausrollbremse Mitfahrer
    enum BREMSE : uint8_t {
        bremse_fahr0_0 = 0x82,
        bremse_fahr0_by_user = 0x82,
        bremse_fahr0_umkehr = 0xDC,

        bremse_fahr1_0 = 0x82,
        bremse_fahr1_by_user = 0x82,
        bremse_fahr1_umkehr = 0xDC,

        bremse_fahr2_0 = 0x82,
        bremse_fahr2_by_user = 0x82,
        bremse_fahr2_umkehr = 0xDC,
    };

    // SPEED_OF_ANTRIEB
    enum ANTRIEB_SPEED : uint8_t {
        antrieb_speed_fahr0_0 = 0x69,
        antrieb_speed_fahr0_by_user = 0x19,
        rucken_fahr0_speed_0 = 0x19,

        antrieb_speed_fahr1_0 = 0x7D,
        antrieb_speed_fahr1_by_user = 0x28,
        rucken_fahr1_speed_0 = 0x19,

        antrieb_speed_fahr2_0 = 0x82,
        antrieb_speed_fahr2_0_indx6 = 0x8C,
        antrieb_speed_fahr2_by_user = 0x28,
        rucken_fahr2_speed_0 = 0x32
    };

    // SPEED_OF_GABEL
    enum GABEL_SPEED : uint8_t {
        gabel_speed_fahr0_0 = 0x69,
        gabel_speed_fahr0_by_user = 0x19,
        rucken_gabel_speed_fahr0_0 = 0x19,

        gabel_speed_fahr1_0 = 0x7D,
        gabel_speed_fahr1_by_user = 0x28,
        rucken_gabel_speed_fahr1_0 = 0x28,

        gabel_speed_fahr2_0 = 0x82,
        gabel_speed_fahr2_0_subindex6 = 0x8C,
        gabel_speed_fahr2_by_user = 0x28,
        rucken_gabel_speed_fahr2_0 = 0x28,
    };

    // KENNLINIE CHARACTERISTIC CURVE
    enum KENNLINIE : uint8_t {
        kenn_0 = 0x06,
    };

    // EINHEITEN UMSCHALTUNG
    enum UMSCHALTUNG : uint8_t {
        umschalt_0 = 0x07,
    };

    // LENKEN
    enum LENKEN : uint8_t {
        lenken_korrektur_0 = 0x80,
        lenken_ubersetzung = 0x32,

    };

    // VERRIEGELUNG 2922
    enum VERRIEGELUNG : uint8_t {
        verrie_0 = 0x00,
        verrie_1 = 0x05,
    };


    // 2923
    enum ZEIT_BREMSE : uint8_t {
        zeit_bremse_0 = 0x04,
    };

    // 2924
    enum ECO_Vorgabe : uint8_t {
        eco_0 = 0x00,
    };

    // 292B
    enum UBERWACHUNG_ZEIT : uint8_t {
        wakeup_0 = 0x3C,
    };

    // 2233
    enum HUB_OPTION : uint8_t {
        hub_option_0 = 0x01,    // Heben und Senken mit Fußtaster
    };

    // 2960
    enum OPTION_ZUGANG : uint8_t {
        zugang_0 = 0x02,
    };


};



#endif // !LOITRUCK_H


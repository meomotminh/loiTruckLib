//#include <cstdint>
#ifndef LOITRUCK_H
#define LOITRUCK_H

#include <SPI.h>
#include <Arduino.h>

#include <stdint.h>

//#include <iterator>
//#include <vector>
//#include <map>
//#include <new.cpp>

//#include <bitset>
//#include <vector>
//#include <map>

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



//------------- DECLARE RES FAHRZEUG 1 TO SAVE IN FLASH -----------------
//----------------- Read-only----------------------
const PROGMEM uint8_t res_2000_01_1[]  = {sw0, sw1, sw2, sw3};
const PROGMEM uint8_t res_2000_03_1[]  = {hw0, hw1, hw2, hw3};
const PROGMEM uint8_t res_2002_02_1[]  = {produk0, produk1, produk2, produk3};
const PROGMEM uint8_t res_2020_02_1[]  = {hunder0, hunder1, 0x00, 0x00};
const PROGMEM uint8_t res_2100_02_1[] = {acc_fahr0_0, 0x00, 0x00, 0x00}; // -------------------- BESCHLEUNIGUNG FAHRPROGRAM (SPEED UP 2100-------------------------
const PROGMEM uint8_t res_2102_02_1[]  = {acc_by_user_fahr0_0, 0x00, 0x00, 0x00};// -------------------- BESCHLEUNIGUNG MITGÄNGER FAHRPROGRAM (SPEED UP SUBINDEX 2 2102-------------------------
const PROGMEM uint8_t res_2102_06_1[]  = {acc_by_user_fahr0_0, 0x00, 0x00, 0x00};// -------------------- BESCHLEUNIGUNG MITGÄNGER FAHRPROGRAM (SPEED UP SUBINDEX 6 2102-------------------------
const PROGMEM uint8_t res_2100_06_1[]  = {acc_fahr0_0, acc_fahr0_1, 0x00, 0x00};// -------------------- BESCHLEUNIGUNG FAHRPROGRAM (SPEED UP 2 2100???-------------------------
const PROGMEM uint8_t res_2200_02_1[]  = {ramp_heben_lastteil_0, 0x00, 0x00, 0x00};// -------------------- HOCHLAUFRAMPE HEBEN LASTTEIL 2200-------------------------
const PROGMEM uint8_t res_2200_06_1[]  = {ramp_heben_lastteil_0, 0x00, 0x00, 0x00};// -------------------- HOCHLAUFRAMPE HEBEN LASTTEIL 2200-------------------------
const PROGMEM uint8_t res_2001_01_1[]  = {no_Antrag_Log, 0x00, 0x00, 0x00};    // -------------------- LOGBUCH 2001-------------------------
const PROGMEM uint8_t res_2101_02_1[]  = {acc_by_user_fahr0_0, 0x00, 0x00, 0x00};// -------------------- BESCHLEUNIGUNG MITFAHRER SUBINDEX 02 2101-------------------------
const PROGMEM uint8_t res_2101_06_1[]  = {acc_by_user_fahr0_0, 0x00, 0x00, 0x00};// -------------------- BESCHLEUNIGUNG MITFAHRER SUBINDEX 06 2101-------------------------
const PROGMEM uint8_t res_2201_02_1[]  = {auslauf_heben_lastteil_0, 0x00, 0x00, 0x00};// -------------------- AUSLAUFRAMPE HEBEN LASTTEIL SUBINDEX 2 2201-------------------------
const PROGMEM uint8_t res_2201_06_1[]  = {auslauf_heben_lastteil_0, 0x00, 0x00, 0x00};// -------------------- AUSLAUFRAMPE HEBEN LASTTEIL SUB INDEX 6 2201-------------------------
const PROGMEM uint8_t res_2103_06_1[]  = {unknown_0, 0x00, 0x00, 0x00};// -------------------- UNKNOWN 2103 SUBINX 06-------------------------
const PROGMEM uint8_t res_2103_02_1[]  = {unknown_0, 0x00, 0x00, 0x00};// -------------------- UNKNOWN 2103 SUBINX 02-------------------------
const PROGMEM uint8_t res_2104_02_1[]  = {bremse_fahr0_0, 0x00, 0x00, 0x00};// -------------------- AUSROLLBREMSE MITFAHRER 2104 SUBINX 02-------------------------
const PROGMEM uint8_t res_2106_06_1[]  = {bremse_fahr0_0, 0x00, 0x00, 0x00}; // -------------------- AUSROLLBREMSE MITFAHRER 2104 SUBINX 06-------------------------

//----------------- Read-Write----------------------
const PROGMEM uint8_t res_2002_01_1[]  = {}; //---- Seri Nummer -----
const PROGMEM uint8_t res_2020_01_1[]  = {}; //---- betriebszeit -----

const PROGMEM uint8_t res_2413_02_1[]  = {}; //---- Lenkkorrektur ----
const PROGMEM uint8_t res_2923_02_1[]  = {}; //---- Bandagenentspannung ---
const PROGMEM uint8_t res_2414_02_1[]  = {}; //---- Lenkübersetzung ---
const PROGMEM uint8_t res_2461_02_1[]  = {}; //---- Status ---
const PROGMEM uint8_t res_2405_02_1[]  = {}; //---- Sollwertgeber ----
const PROGMEM uint8_t res_2402_02_1[]  = {}; //---- Istwertgeber ----
const PROGMEM uint8_t res_2001_02_1[]  = {}; //---- Soll Indx Logbuch ---
const PROGMEM uint8_t res_2001_03_1[]  = {}; //---- Ist Indx Logbuch ---
const PROGMEM uint8_t res_2411_02_1[]  = {}; //---- Kennlinie ---
const PROGMEM uint8_t res_2405_07_1[]  = {}; //---- Sollwertgeber subindx 7 ---
const PROGMEM uint8_t res_2460_02_1[]  = {}; //---- Istwertgeber ----------

//---------------- LOOK UP TABLE-----------------
const uint8_t * const res_table_1[] PROGMEM = {
    res_2000_01_1,  // 0
    res_2000_03_1,  // 1
    
    res_2001_01_1,  // 2
    res_2002_01_1, // Seri number ** 3

    res_2002_02_1, // 4
    
    res_2020_01_1, // betriebszeit ** 5
    res_2020_02_1, // 6
    
    res_2100_02_1, // 7
    res_2100_06_1, // 8
    
    res_2101_02_1, // 9
    res_2101_06_1, // 10

    res_2102_02_1, // 11
    res_2102_06_1, // 12   
    
    res_2103_02_1, // 13
    res_2103_06_1, // 14   

    res_2104_02_1, // 15
    res_2106_06_1, // 16
 
    res_2200_02_1, // 17
    res_2200_06_1, // 18
        
    res_2201_02_1, // 19
    res_2201_06_1, // 20   

    res_2413_02_1, //---- Lenkkorrektur ---- 21
    res_2923_02_1, //---- Bandagenentspannung --- 22
    res_2414_02_1, //---- Lenkübersetzung --- 23
    res_2461_02_1, //---- Status --- 24
    res_2405_02_1, //---- Sollwertgeber ---- 25
    res_2402_02_1, //---- Istwertgeber ---- 26

    res_2001_02_1, //--- Logbuch sollindex --- 27
    res_2001_03_1, //--- Logbuch IstIndex --- 28
    res_2411_02_1, //--- Kennlinie --- 29

    res_2405_07_1, //--- Sollwertgeber 07 --- 30
    res_2460_02_1, //--- Istwertgeber 02 --- 31
};


// UTILITY FUNCTION
String create_str(std::vector<__u8> data, __u16 can_id, int can_dlc); 
void assign_arr(__u8 data[8],__u8 byte1, __u8 byte2,__u8 byte3,__u8 byte4,__u8 byte5,__u8 byte6,__u8 byte7,__u8 byte8);
can_frame create_CAN_frame(__u32 can_id, __u8 can_dlc, __u8 data[8]);
String create_str_from_frame(can_frame _toConvert);
can_frame create_frame_from_str(String _toConvert);
void display_CAN_Frame(can_frame _toDisplay);
__u16 prepare_ID(__u16 ID_req);
__u8 prepare_Command_ID(can_frame req, bool end_msg);
bool check_COB_ID_range(can_frame req);




enum RUN_MODE {
    MODE_HAPPY, // richtig anwort (0x60)
    MODE_ADT,   // negativ anwort (0x80)
    //MODE_NORMAL, // 50 50
    MODE_UNHAPPY, // immer falsch
    MODE_IGNORE // nicht 
};

enum RUN_STATE {
    STATE_WELCOME,
    STATE_SELECT,
    STATE_RUN,
};

struct answer {
    __u8 data0;
    __u8 data1;
    __u8 data2;
    __u8 data3;
};



class LOITRUCK
{
public:
    RUN_MODE _runMode;
    RUN_STATE _runState;
    int _mousePos;

    // For create map
    std::vector<std::vector<__u8>> req_arr;
    std::vector<std::vector<__u8>> res_arr; 
    //std::map<String, String> CAN_Expedited_Map;
    std::map<String, String> CAN_Segmented_Map;
    std::map<int, int> CAN_Expedited_Map;

    // For segmented
    bool Segmented;
    can_frame receive_Segment_Req;
    can_frame expecting_Segment_frame;
    __u8 count_segmented;
    String segmented_res_Fahrzeug_Name[5];

    // For initialization
    __u8 selected_Truck;

    
    // Object DATA
    __u8 loiTruck_Seri0;
    __u8 loiTruck_Seri1;
    __u8 loiTruck_Seri2;
    __u8 loiTruck_Seri3;

    __u8 loiTruck_Zeit0;
    __u8 loiTruck_Zeit1;

    __u8 loiTruck_Lenken_Korrektur;     // 2413
    __u8 loiTruck_Lenken_Zeit_Einfall;  // Bandagenentspannung
    __u8 loiTruck_Lenken_Ubersetzung;   // 
    __u8 loiTruck_Lenken_Status_0;      // 2461
    __u8 loiTruck_Lenken_Status_1;      // 2461
    __u8 loiTruck_Lenken_Sollwertgeber_0; // 2405  soll lenkwinkel link
    __u8 loiTruck_Lenken_Sollwertgeber_1; // 2405  soll lenkwinkel link
    __u8 loiTruck_Lenken_Istwertgeber_0;  // 2402  is lenkwinkel link   
    __u8 loiTruck_Lenken_Istwertgeber_1;  // 2402  is lenkwinkel link   
    __u8 loiTruck_Lenken_Kennlinie;     // 2411 

    __u8 loiTruck_Logbuch_SavedIndx;
    

    enum ERROR {
        ERROR_OK = 0,
        ERROR_OVR8 = 1,
        ERROR_NONEXIST = 2
    };

    // Constructor
    LOITRUCK(RUN_MODE runMode) {
        _runMode = runMode;
        _runState = STATE_WELCOME;
        _mousePos = 2;

        // assign Seri
        loiTruck_Seri0 = Seri0_HAPPY;
        loiTruck_Seri1 = Seri1_HAPPY;
        loiTruck_Seri2 = Seri2_HAPPY;
        loiTruck_Seri3 = Seri3_HAPPY;

        // assign Zeit
        loiTruck_Zeit0 = zeit0;
        loiTruck_Zeit1 = zeit1;

        // Lenken
        loiTruck_Lenken_Korrektur = 0xC2;   // 0 grad : 0x80   4.02 grad : 0xC2   
        loiTruck_Lenken_Zeit_Einfall = 0x01;    // maximum 9 *10 in second
        loiTruck_Lenken_Ubersetzung = 0x32;  // 50 -> 5 revolution for 180 grad
        
        loiTruck_Lenken_Status_0 = 0x22;    // Nullstellung
        loiTruck_Lenken_Status_1 = 0x22;    // Nullstellung 0x1111

        loiTruck_Lenken_Istwertgeber_0 = 0x22;  // 2402 Max
        loiTruck_Lenken_Istwertgeber_1 = 0x22;  // 2402 Max

        loiTruck_Lenken_Sollwertgeber_0 = 0x11; // 2405 Max
        loiTruck_Lenken_Sollwertgeber_1 = 0x11; // 2405 Max

        loiTruck_Lenken_Kennlinie = 0x06;   // follow Noris excel

        // Logbuch
        loiTruck_Logbuch_SavedIndx = 0x00;

        // assign selected truck (later via lcd and joystick)
        selected_Truck = 1;     // ECE2252015
    };

    // FUNCTION
   
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
    
    bool actuator(can_frame req_frame);
    answer prepare_Answer(can_frame req, int indx_subindx);
    bool create_map();

    // HARDWARE
    bool display_LCD(LiquidCrystal_I2C lcd);
    bool modify_after_joystick(int mapx, int mapy, int clicked);
    void demo_test();
};



#endif // !LOITRUCK_H


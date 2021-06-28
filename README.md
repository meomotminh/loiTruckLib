# LIBRARY DOCUMENTATION

----

## Objective

- Simulate different Trucks implementing CAN Open protocol
- Simulate different Error Scenarios by different Running Mode:
    - MODE HAPPY
    - MODE ADT
    - MODE UNHAPPY
    - MODE IGNORE
- In combination with those Modes, Range and Delay enable users to simulate errors more freely:
    - Range:
        - WRITE-REQ
        - WRITE-CHECK
        - ALL
    - Delay:
        - 0 - 2 mil (integer upper boundary, step 100ms)

----

## Requirements

![](loiTruckSchema.svg)

>Hardware
- Board : Arduino Mega 2560
- LCD 20x4 I2C [link LCD]
- Joystick [link Joystick]
- CAN module MCP2515 [link MCP]
- 1xServo Motor
- 2xLED
- 2x220 Ohm resistors
- 2xBreadboars

[link LCD]: https://www.amazon.de/SunFounder-Serial-Arduino-Mega2560-IIC2004/dp/B01GPUMP9C/ref=asc_df_B01GPUMP9C/?tag=googshopde-21&linkCode=df0&hvadid=309955879354&hvpos=&hvnetw=g&hvrand=17007324613408845966&hvpone=&hvptwo=&hvqmt=&hvdev=c&hvdvcmdl=&hvlocint=&hvlocphy=9043559&hvtargid=pla-572925702212&psc=1&th=1&psc=1&tag=&ref=&adgrpid=61302520443&hvpone=&hvptwo=&hvadid=309955879354&hvpos=&hvnetw=g&hvrand=17007324613408845966&hvqmt=&hvdev=c&hvdvcmdl=&hvlocint=&hvlocphy=9043559&hvtargid=pla-572925702212
[link Joystick]: https://www.reichelt.de/de/de/entwicklerboards-joystick-modul-debo-thumb-joy-p239254.html?PROVID=2788&gclid=Cj0KCQjw5uWGBhCTARIsAL70sLJHHHH1-nO5mFu5e82Zeq3ZaQQquua2n3wwcwOz8eTeXlqD78cR9JwaAuAOEALw_wcB&&r=1
[link MCP]: https://www.az-delivery.de/en/products/mcp2515-can-bus-modul

>Software
- Arduino IDE
- MCP2515 lib [link lib MCP2515]
- LCD Lib [link lib LCD]

[link lib MCP2515]: https://github.com/autowp/arduino-mcp2515
[link lib LCD]: https://github.com/fdebrabander/Arduino-LiquidCrystal-I2C-library/blob/master/LiquidCrystal_I2C.h

*How to import a library into Arduino IDE ?* [lib import lib]

[lib import lib]: https://www.arduino.cc/en/guide/libraries

----

## App Description

### Structure

Three important objects using in the app
- mcp2515
- lcd
- loiTruck
    - Attributes
        - _runMode : 
            -   define Run Mode of loiTruck 
            -   Type RUN_MODE : MODE_HAPPY, MODE_ADT, MODE_UNHAPPY, MODE_IGNORE
        - _runState :
            -   define State of loiTruck corresponding to LCD display
            -   Type RUN_STATE : STATE_WELCOME, STATE_SELECT_TRUCK, STATE_SELECT_MODE, STATE_RUN, STATE_DEMO, STATE_CONFIG, CONFIG_SELECT_RANGE, CONFIG_SELECT_DELAY
        - _runMode_Apply :
            -   define range of RUN_MODE underlying logic
            -   Type APPLY_RANGE : WRITE_REQ, WRITE_CHECK, ALL
        - _runMode_Delay :
            -   define delay (ms) between receiving CAN request and replying CAN response
            -   Type integer 0 - 2 mil
        - _mousePos :
            -   define pos of icon "->" in LCD display
            -   Type integer : 0,1,2,3
        - _just_Save :
            -   mark just receive an CAN Write Request (Command ID 0x2x)
            -   Type boolean : true, false
        - CAN_Segmented_Map :
            -   Map String of CAN request with corresponding CAN respond for Segmented CAN
            -   Type <String, String> : only CAN request 20021 asking for Truck IdentString is added now
        - CAN_Expedited_Map :
            -   Map coded value of CAN request with corresponding position in the LOOK UP TABLE
            -   Type <int, int>
        - Segmented :
            -   Check if the received CAN request is Segmented or Expedited
            -   Type boolean : true, false
        - receive_Segment_Req :
            -   Store CAN received request 
            -   Type can_frame object defined in mcp2515 lib
        - expecting_Segment_frame :
            -   Store expecting Segment request. During Segmented Conversation, it will be updated and replied if receive_Segment_Req equal to expecting_Segment_frame
            -   Type can_frame object defined in mcp2515 lib
        - count_segmented :
            -   Count up during Segmented Conversation, used to determine ending point
            -   Type __u8 
        - segmented_res_Fahrzeug_Name :
            -   Store string of CAN reply to the 20021 (ask IdentString) request
            -   Type array of String length 5
        - selected_Truck :
            -   remember Truck has been selected
            -   Type __u8
        - loiTruck_Seri0...3 :
            -   Seri number of the Truck
            -   Type __u8
        - loiTruck_Zeit0...1 :
            -   Betriebstunden of the Truck
            -   Type __u8
        - loiTruck_Lenken_Korrektur :
            -   Type __u8
        - loiTruck_Lenken_Zeit_Einfall :
            -   Type __u8
        - loiTruck_Lenken_Ubersetzung :
            -   Type __u8
        - loiTruck_Lenken_Ist_Status_0..1 :
            -   Reply to 2460 CAN req
            -   Type __u8
        - loiTruck_Lenken_Soll_Status_0..1 :
            -   Reply to 2461 CAN req
            -   Type __u8
        - loiTruck_Lenken_SollLenkwinkel_Recht/Link/Null_0..1 :
            -   Reply to 2405/04/03 CAN req
            -   Type __u8
        - loiTruck_Lenken_IstLenkwinkel_Recht/Link/Null_0..1 :
            -   Reply to 2402/01/00 CAN req
            -   Type __u8
        - loiTruck_Lenken_Standar/Werk/Kennlinie :
            -   Reply to Lenken standard/Werk/Kennlinie CAN req
            -   Type __u8
        - loiTruck_Logbuch_SavedIndx :
            -   Saved indx from previous Write CAN request
            -   Type __u8
        - loiTruck_lenken_load/save :
            -   Mark just receive "LOAD"/"SAVE" Command
            -   Type boolean
        - loiTruck_lenken_save_null/link/recht :
            -   Mark just save null/link/recht during Teach-In procedure (after saved, corresponding Soll/Istwert will not be updated anymore, even if you rotate the Poti)
            -   Type boolean
        - _teach_In :
            -   Mark start Teach-In procedure (trigger by receiving "LOAD" + "SAVE" command)
            -   Type boolean
        - _count_Teach_In :
            -   Count up during Teach-In procedure (might be used for timing), unused currently
            -   Type int
        - _ist_Teach_In :
            -   Mark start step 3 of Teach-In procedure (trigger after Save Null)
            -   Type boolean
        - _last_saved_time :
            -   Save last time receive "SAVE" command in step 2 of Teach-In procedure (used for timing but currently unused)
            -   Type int
        - ignore :
            -   Decide if the CAN request should be ignored or not
            -   Type boolean
        - _servo :
            -   the Servo Motor connect to pin 3
            -   Type Servo
        - last_Servo_Pos :
            -   Save last Servo Position to trigger update LCD and Joystick if needed (when difference > 10)
            -   Tyoe int
        
    - Functions:
        - Constructor(RUN_MODE runMode, Servo servo) :
            -   Need to be called before using loiTruck object
            -   assign all default object attributes as :
                -   Zeit : 0x4901
                -   Lenken_Korrektur : 0xC2
                -   Lenken_Zeit_Einfall : 0x01
                -   Lenken_Ubersetzung : 0x32
                -   Soll_Status : 0x0000
                -   Ist_Status  : 0x7777
                -   Ist_Link, Soll_Link, Ist_Recht, Soll_Recht, Ist_Null, Soll_Null : 0x1100
                -   lenken_Min = 0x0000
                -   lenken_Max = 0x7777
                -   lenken_Standard = 0xFF
                -   lenken_werk = 0x80
                -   lenken_kennlinie = 0x06
        - void create_CAN_Expedited_map() :
            -   Call to create Expedited map
        - void create_CAN_Segmented_map() :
            -   Call to create Segmented map
        - bool check_Segmented(can_frame _toTest) :
            -   check if received CAN frame is segmented or not
        - can_frame get_Segmented_Response(can_frame req, LiquidCrystal_I2C lcd) :
            -   calculate the Segmented Respone for the corresponding CAN frame request req
        - can_frame get_Expedited_Response(can_frame _toGet, LiquidCrystal_I2C lcd) :
            -   calculate the Expedited Respone for the corresponding CAN frame _toGet req
        - void create_segmented_res_Fahrzeug_Name() :
            -   call to create segmented response map for 20021 CAN req
        - void set_expecting_Segmented_Req(int count) :
            -   call to set expecting response for the upcoming Segmented CAN request
        - void initial_Segmented_Transmit(can_frame receive_frame) :
            -   call after receiving Segmented Request to set initial variables as count, expecting response...
        - void finalise_Segmented_Transmit() :
            -   call at the end of the Segmented Conversation to free those relevant variables.
        - bool **actuator(can_frame req_frame, int indx_subindx, LiquidCrystal_I2C lcd)** :
            -   always being called when receive CAN request
            -   update loiTruck attribute if needed when receive CAN "Write" Command
        - answer **prepare_Answer(can_frame req, int indx_subindx, LiquidCrystal_I2C lcd)** :
            -   always being called when receive CAN request
            -   check if the answer is known (if existed in the map) then return the answer, if not then return 0000 (unknown answer)
        - __u8 **prepare_Command_ID(can_frame req, bool end_msg)** :
            -   always being called when receive CAN request
            -   calculate the Response's Command ID base on request, runMode...
        - bool **create_map()** :
            -   function need to be called to create CAN_Expedited_Map and call create_segmented_res_Fahrzeug_Name() internally
            -   it is used to simplify the call procedure
        - void **actuate_servo(int minPot, int maxPot)** :
            -   is alway called to update Servo motor and corresponding Ist/SollWert variables
        - bool **display_LCD(LiquidCrystal_I2C lcd)** :
            -   is called if joystick is move more than 10 unit to update LCD
        - bool **modify_after_joystick(int mapx, int mapy, int clicked, LiquidCrystal_I2C lcd)** :
            -   is called if joystick is move more than 10 unit to update LCD and corresponding variables
        - void demo_test() :
            -   is called when click on Demo -> servo motor shall rotate 2 rounds
    
### Underlying Run Mode LOGIC

>MODE HAPPY

    -   Write Command (Command ID = 0x20): reply 0x60 and confirm correctly
    -   Read Command (Command ID = 0x40): reply 0x4x and value
    -   Delay is applied base on Range Selection (Write Command,Read Confirm or All)

>MODE ADT

    -   Write Command (Command ID = 0x20): reply 0x80 (Abort) + Error Code (sub index does not exist) if Range = WRITE REQ
    -   Read Confirm (Read after Write Command): reply 0x80 (Abort) + Error Code (sub index does not exist) if Range = WRITE CHECK
    -   Read Command (Command ID = 0x40): reply 0x80 (Abort) + Error Code (sub index does not exist) if Range = ALL
    -   Delay is applied base on Range Selection (Write Command,Read Confirm or All)

>MODE UNHAPPY

    -   Write Command (Command ID = 0x20): reply 0x60 + confirm wrong
    -   Read Command (Command ID = 0x40): reply 0x4x + 0000 (if Range = ALL)
    -   Delay is applied base on Range Selection (Write Command,Read Confirm or All)

>MODE IGNORE

    -   Write Command (Command ID = 0x20): not reply if Range = WRITE REQ
    -   Read Confirm (Read after Write Command): not reply if Range = WRITE CHECK
    -   Read Command (Command ID = 0x40) : not reply if Range = ALL
    -   Delay is applied base on Range Selection (Write Command,Read Confirm or All)

        

#include <SPI.h>
#include <MFRC522.h>

#include <TFT_eSPI.h>  // Hardware-specific library
#include <qrcode_gen.h>

#include <Arduino.h>

#include <WiFi.h>
#include <WiFiMulti.h>

#include <HTTPClient.h>
#include <HTTPUpdate.h>

WiFiMulti WiFiMulti;

#include "Free_Fonts.h"  // Include the header file attached to this sketch
#include "main_screen.h"
#include "BEi_logo.h"
#include "NotoSansBold15.h"
#include "NotoSansMonoSCB20.h"
#include "NotoSansBold36.h"

#define AA_FONT_SMALL NotoSansBold15
#define AA_FONT_M NotoSansMonoSCB20
#define AA_FONT_LARGE NotoSansBold36

TFT_eSPI tft = TFT_eSPI();  // Invoke custom library with default width and heigh
TFT_eSprite spr = TFT_eSprite(&tft); // Sprite class needs to be invoked
unsigned long drawTime = 0;

#define RX 25  //30 
#define TX 12  //23
#define RX2 35  //8
#define TX2 18  //22


#define EVCC_SW_MODE 2

#define EMERGENCY_PIN 33

#define TFT_GREY 0x5AEB // New colour
#define BLACK           0x0000
#define BLUE            0x001F
#define RED             0xF800
#define GREEN           0x07E0
#define CYAN            0x07FF
#define MAGENTA         0xF81F
#define YELLOW          0xFFE0
#define WHITE           0xFFFF
#define SKY             0xAEFB
//#define CYAN            0x07FF
#define PINK            0xFB38
#define ORANGE          0xFCC6  //light orange color
//#define messageBox      0x0046
#define messageBox      0x39A6
#define kwhBox          0x18C3
#define alarmBox        0xE0A6
#define symbol_circle   0xC4E0
//#define BG1             0x21FF
#define BG1             0x018F
#define GRAY            0x8410
#define LimeGreen       0x3666

bool set_stand_by_state = 0;
bool set_connect_state = 0;
bool set_charging_mode_state = 0;
bool set_leakage_fault_state = 0;
bool set_fault_e_state = 0;
bool set_emergency_warning_state = 0;
bool set_get_out_emergency_warning_first_time_state = 0;
bool set_no_power_state = 0;
bool set_stop_charging_state = 0;

bool evcc_lock = 0;
bool ev_mode = 0;
bool rfid_control = 0;
bool wrong_rfid_card = 0; //ev mode0
bool pass_rfid_card = 0; //ev mode1
bool stop_charging_pass_rfid_card = 0;
bool stop_charging_wrong_rfid_card = 0;
bool send_ocpp_plugin = 0;
bool reservation_rfid_card = 0;
bool wait_display = 1;
bool required_firmware_update = 0;

bool remote_start_transaction = 0;
bool SuspendedEV = 0;
bool reserved_status = 0;

bool tap_card_status = 0;

bool voltage_cutting = 0;
bool SW_mode = 0;
bool HW_mode = 0;

unsigned long wait_for_delay_5000;
unsigned long evcc_question_timer;
unsigned long delay_question_to_evcc;
unsigned long delay_refresh_display = 1500;
unsigned long swipe_card_delay_500;
unsigned long led_charge_delay;

//String x = "STAND_BY";
String x;
String EV_state;
float volt = 0;
float current_L1 = 0;
float current_L2 = 0;
float current_L3 = 0;
float power = 0;
float power_sum_sec = 0;
float temp = 25;

int wifistr = 0;
int maxCurrent = 1;
bool tle_4G_connected = 0;
int tle_4G_on_symbol = 2;
bool lan_connected = 0;
int lan_on_symbol = 2;
bool swipe_card_symbol = 1;

String x_pass;
String EV_state_pass;
float volt_pass;
float current_L1_pass;
float power_pass;
float power_sum_sec_pass;
float temp_pass;

String D_x_pass;
String D_volt = "0";
String D_current_L1 = "0";
String D_current_L2 = "0";
String D_current_L3 = "0";
String D_power = "0";
String D_power_sum_sec = "0";
String D_temp = "-";
String D_unit="kWh";
String D_total_power = "";
String D_time_pass;
String OCPP_receive = "";
String OCPP_send = "";
String data_memory = "";
String QRCODE_LINK = "";

String sendData;

String rfid_card_last_swip_num_1;

//time zone
unsigned long charging_mode_timer;
unsigned long stand_by_mode_timer;
unsigned long timer;
unsigned long charging_mode_time_stamp_start_point;
String D_time = "00:00:00";
int sec_passed;
int sec0 = 0;
int sec1 = 0;
int min2 = 0;
int min3 = 0;
int hour4 = 0;
int hour5 = 0;

int count_refresh = 0;

int round_command_set = 0;
int round_command_set_pass = 0;
int same_question_point = 0;

int leakage_fault_point = 0;
int leak_score=0;

int temp_alarm_count = 0;

// const char *STASSID = "TP-Link_F844";
// const char *STAPSK = "11276203";

//const char *STASSID = "4G-UFI-1193";
//const char *STAPSK = "1234567890";

String Firmware_Link_Update_Display = "http://ums.svi.co.th/evdbf.bin";
String WIFI = "WLCIOT";
String PASS = "1234567890";
const char *STASSID = WIFI.c_str();
const char *STAPSK = PASS.c_str();

String key = "";

////////////////////////////////////////////////////////////////////////
//led
#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
#include <avr/power.h>  // Required for 16 MHz Adafruit Trinket
#endif

#define PIXEL_PIN    32  // Digital IO pin connected to the NeoPixels.

#define PIXEL_COUNT 8  // Number of NeoPixels

Adafruit_NeoPixel strip(PIXEL_COUNT, PIXEL_PIN, NEO_GRB + NEO_KHZ800);

#define PIN 32  // On Trinket or Gemma, suggest changing this to 1

#define NUMPIXELS 8  // Popular NeoPixel ring size

Adafruit_NeoPixel pixels(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

#define DELAYVAL 100  // Time (in milliseconds) to pause between pixels

long firstPixelHue = 0;
int i_led_pixel_0 = 0;
int i_led_pixel_1 = 0;
int i_led_pixel_2 = 0;
int i_led_pixel_3 = 0;
int i_led_pixel_4 = 0;
int i_led_pixel_5 = 0;
int i_led_pixel_6 = 0;
int i_led_pixel_7 = 0;


int i_led=0;


///////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////
//rfid

#define RST_PIN   27  //GPIO22
#define SS_PIN    5  //GPIO21
#define MISO_PIN  19  //GPIO27//12//19
#define MOSI_PIN  26  //GPIO26//13//23
#define SCK_PIN   18  //GIPO25//14//18

#define rfid_check_pin 34
bool id_card_require_display = 0;
bool swipe_card_status = 0;

MFRC522 rfid(SS_PIN, RST_PIN);  // Instance of the class

////////////////////////////////////////////////////////////////////////////

void setup() {

  pinMode(EVCC_SW_MODE, OUTPUT);
  digitalWrite(EVCC_SW_MODE, HIGH);

  Serial1.begin(115200, SERIAL_8N1, RX, TX);
  Serial2.begin(115200, SERIAL_8N1, RX2, TX2);
  tft.begin(115200);
  SPI.begin(SCK_PIN, MISO_PIN, MOSI_PIN, SS_PIN);
  tft.setRotation(3);
  spr.setColorDepth(16); // 16 bit colour needed to show antialiased fonts

  tft.setSwapBytes(true);
  tft.pushImage(0, 0, 480, 320, BEi_logo);
  //EV_Display();

  Serial.begin(9600);
  /////////////////////////////////////////////////////////////////////////
//led
#if defined(__AVR_ATtiny85__) && (F_CPU == 16000000)
  clock_prescale_set(clock_div_1);
#endif

  pixels.begin();  // INITIALIZE NeoPixel strip object (REQUIRED)
  pixels.show();

  /////////////////////////////////////////////////////////////////////////
  
  pinMode(rfid_check_pin, INPUT_PULLDOWN);
  if(digitalRead(rfid_check_pin) == HIGH){
    rfid_control = 1;
  }

  //x = "STAND_BY";
  EV_Display_Refresh();
  led_standby();

  
  pinMode(EMERGENCY_PIN, INPUT_PULLUP);


  ///////////////////////////////////////////////////////////////////////////
  //rfid
  rfid.PCD_Init();  // Init MFRC522
  ////////////////////////////////////////////////////////////////////////////

  //maxCurrent32();
  WiFi.begin(STASSID, STAPSK);
  for(int i=0;i<5;i++){
    if(!WiFi.isConnected()){
      Serial.println("wait wifi!");
      delay(1000);
    }else{
      i=10;
      Serial.println("wifi connected!");
    }
  }
  if(String(WiFi.localIP()) == "0"){
    Serial.println("wifi disconnected!");
  }
  
  maxCurrent = 32;

  HW_CONF();
  
}

void loop() {
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//emergecy button have been pushed         yes = 1 , no = 0 ///////////////////////////////////////////////////////////////////////
  if(digitalRead(EMERGENCY_PIN)==1){  //and short the EMERGENCY pin to ground.
    x = "EMERGENCY_WARNING";
    digitalWrite(EVCC_SW_MODE, HIGH);
    set_get_out_emergency_warning_first_time_state = 0;
  }else{
    if(set_get_out_emergency_warning_first_time_state == 0) {
      x = "STAND_BY";
      D_temp!="-";
      set_stand_by_state = 0;
      set_get_out_emergency_warning_first_time_state = 1;
    }
    
    //temperature //
    if(D_temp!="-"){

      if(temp<70){
        if (temp_alarm_count >=1 ){
          EV_Display();
          // if(x == "STAND_BY"){for(int i=147; i<161; i++){tft.drawCircle(240,160,i,TFT_BLUE);}}
          // else if ((x == "CONNECTED")||(x == "FINISHED")){for(int i=147; i<161; i++){tft.drawCircle(240,160,i,CYAN);}}
          // else if (x == "CHARGING_MODE"){for(int i=147; i<161; i++){tft.drawCircle(240,160,i,TFT_GREEN);}}
          temp_alarm_count = 0;
        }
      }
      else if(temp >= 70){
        temp_alarm_count+=1;

        if(temp_alarm_count==1){EV_temp_alarm();}
        else if (temp_alarm_count==10){
          EV_Display();
          // if(x == "STAND_BY"){for(int i=147; i<161; i++){tft.drawCircle(240,160,i,TFT_BLUE);}}
          // else if ((x == "CONNECTED")||(x == "FINISHED")){for(int i=147; i<161; i++){tft.drawCircle(240,160,i,CYAN);}}
          // else if (x == "CHARGING_MODE"){for(int i=147; i<161; i++){tft.drawCircle(240,160,i,TFT_GREEN);}}
        }
        else if (temp_alarm_count>30){temp_alarm_count = 0;}
      }
      else if(temp >= 80){
        digitalWrite(EVCC_SW_MODE, HIGH);
        EV_temp_alarm();
        temp_alarm_count = 220;
      }

    }

  }
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//lock ev
  if(evcc_lock==1){digitalWrite(EVCC_SW_MODE, HIGH);}
  if(stop_charging_pass_rfid_card==1){digitalWrite(EVCC_SW_MODE, HIGH);}
  if(x != "CHARGING_MODE"){
    current_L1 = 0;
    D_current_L1 = "0";
    current_L2 = 0;
    D_current_L2 = "0";
    current_L3 = 0;
    D_current_L3 = "0";
  }
  if(SuspendedEV == 1){
      EV_time();
    }
  
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// state ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  if (x == "STAND_BY") {
    tap_card_status = 0;

    if(SuspendedEV == 1){
      EV_time();
    }

    if(send_ocpp_plugin == 1){
      send_ocpp_plugin = 0;
    }

    if (set_stand_by_state == 0) {
      led_standby();    
      OCPP_receive = "state=?";
      OCPP_sends();

      digitalWrite(EVCC_SW_MODE, HIGH);

      set_stand_by_state = 1;
      id_card_require_display = 0;
      swipe_card_status = 0;
      set_connect_state = 0;
      set_charging_mode_state=0;
      D_power="0";
      D_unit="kWh";
      D_total_power="";
      set_leakage_fault_state = 0;
      leakage_fault_point = 0;
      set_fault_e_state = 0;
      set_emergency_warning_state = 0;
      set_no_power_state = 0;
      set_stop_charging_state = 0;
      send_ocpp_plugin = 0;

      pass_rfid_card = 0;
      wrong_rfid_card = 0;

      if(SuspendedEV == 0){
              sec0 = 0;
              sec1 = 0;
              min2 = 0;
              min3 = 0;
              hour4 = 0;
              hour5 = 0;
              D_time = String(hour5) + String(hour4) + ":" + String(min3) + String(min2) + ":" + String(sec1) + String(sec0);

              power = 0;
              power_sum_sec = 0;
              D_power_sum_sec = "0";
        }
 
      EV_Display();
      //for(int i=147; i<161; i++){tft.drawCircle(240,160,i,TFT_BLUE);}
      //tft.drawArc(239, 159, 147, 161, 0, 360, TFT_BLUE, TFT_BLACK, true);
      QRCODE_LINK = "";
      
      stand_by_mode_timer = millis();
    }
  } else if (x == "SWIPE_CARD") {

    if(remote_start_transaction == 1){
      digitalWrite(EVCC_SW_MODE, LOW);
      set_stand_by_state = 1;
      id_card_require_display = 0;
      swipe_card_status = 0;
      set_connect_state = 0;
      D_power="0";
      D_unit="kWh";
      D_total_power="";
      set_leakage_fault_state = 0;
      leakage_fault_point = 0;
      set_fault_e_state = 0;
      set_emergency_warning_state = 0;
      set_no_power_state = 0;
      set_stop_charging_state = 0;
      send_ocpp_plugin = 0;
      
    }else{
    
      if(set_stop_charging_state == 0){

        if(send_ocpp_plugin==0){
        OCPP_receive = "state=?";
        OCPP_sends();
        send_ocpp_plugin = 1;
        }


        if(evcc_lock==0){
          
          if (id_card_require_display == 0) {
            led_swipe();
    
            id_card_require_display = 1;
            set_stand_by_state = 0;
            set_connect_state = 0;
            set_leakage_fault_state = 0;
            leakage_fault_point = 0;
            set_emergency_warning_state = 0;
            set_no_power_state = 0;
            set_stop_charging_state = 0;
            digitalWrite(EVCC_SW_MODE, HIGH);

            if(SuspendedEV == 0){
              sec0 = 0;
              sec1 = 0;
              min2 = 0;
              min3 = 0;
              hour4 = 0;
              hour5 = 0;
              D_time = String(hour5) + String(hour4) + ":" + String(min3) + String(min2) + ":" + String(sec1) + String(sec0);

              power = 0;
              power_sum_sec = 0;
              D_power_sum_sec = "0";
            }
      
            //for(int i=147; i<161; i++){tft.drawCircle(240,160,i,TFT_BLUE);}
            // tft.drawArc(239, 159, 147, 161, 0, 360, CYAN, TFT_BLACK, true);
            EV_rfid_require_card();
          }
        
        } else {
          digitalWrite(EVCC_SW_MODE, HIGH);
        }
      
      }
    }
    
  } else if (x == "CONNECTED") {
    
    if(power_sum_sec != 0){x = "FINISHED";D_power="0";led_charge_full();}
    else {
      led_charge();
    }
    if (set_connect_state == 0){
      led_charge();
      OCPP_receive = "state=?";
      OCPP_sends();
      EV_Display();
      //for(int i=147; i<161; i++){tft.drawCircle(240,160,i,CYAN);}
      tft.drawArc(239, 159, 147, 161, 0, 360, TFT_GREEN, TFT_BLACK, true);
      EV_Display_Refresh();
      set_connect_state = 1;
      set_stand_by_state = 0;
      id_card_require_display = 0;
      set_charging_mode_state = 0;
      set_leakage_fault_state = 0;
      leakage_fault_point = 0;
      set_no_power_state = 0;
      swipe_card_status = 0;
      charging_mode_time_stamp_start_point = millis();

      remote_start_transaction = 0;
      tap_card_status = 0;
    }
    EV_time();
    
  
  } else if (x == "FINISHED") {
    led_charge_full();
    D_power=D_power_sum_sec;
    D_unit="kW";
    D_total_power="total";
    EV_time();

  } else if (x == "CHARGING_MODE") {
    if(set_charging_mode_state==0){
      led_charge();
      OCPP_receive = "state=?";
      OCPP_sends();
      EV_Display();
      //for(int i=147; i<161; i++){tft.drawCircle(240,160,i,TFT_GREEN);}
      tft.drawArc(239, 159, 147, 161, 0, 360, TFT_GREEN, TFT_BLACK, true);
      set_stand_by_state = 0;
      set_charging_mode_state=1;
      set_connect_state = 0;
      D_unit="kWh";
      D_total_power="";
      id_card_require_display = 0;
      set_fault_e_state = 0;
      set_leakage_fault_state = 0;
      leakage_fault_point = 0;
      set_no_power_state = 0;
      tap_card_status = 0;

      remote_start_transaction = 0;

      charging_mode_timer = millis();
      charging_mode_time_stamp_start_point = millis();
    }
    else{
      if (millis() - charging_mode_timer >= 300000) {
        charging_mode_timer = millis();
      }
    }
    
    EV_time();
    second_loop();
    led_charge();

  }else if (x == "FAULT_E") {
    if(set_fault_e_state == 0){
      digitalWrite(EVCC_SW_MODE, HIGH); 
      led_error();
      OCPP_receive = "state=?";
      OCPP_sends();
      x = "FAULT_E";
      EV_Display_Refresh();
      //for(int i=147; i<161; i++){tft.drawCircle(240,160,i,TFT_RED);}
      tft.drawArc(239, 159, 147, 161, 0, 360, TFT_RED, TFT_BLACK, true);
      EV_Fault_E();
      set_stand_by_state = 0;
      set_connect_state = 0;
      set_fault_e_state = 1;
      set_no_power_state = 0;
    }
    
  
  } else if (x == "LEAKAGE_FAULT") {
    if(leakage_fault_point >= 5){
      if(set_leakage_fault_state == 0){
        digitalWrite(EVCC_SW_MODE, HIGH);
        led_error();
        OCPP_receive = "state=?";
        OCPP_sends();
        x = "LEAKAGE_FAULT";
        EV_Display_Refresh();
        //for(int i=147; i<161; i++){tft.drawCircle(240,160,i,TFT_RED);}
        tft.drawArc(239, 159, 147, 161, 0, 360, TFT_RED, TFT_BLACK, true);
        EV_Leakage_Fault();
        set_stand_by_state = 0;
        set_connect_state = 0;
        set_leakage_fault_state = 1;
        set_no_power_state = 0;
        SuspendedEV = 0;
      }
      
    }
    leakage_fault_point += 1;
  
  } else if (x == "EMERGENCY_WARNING") {
    //if ((set_leakage_fault_state == 0) && (set_fault_e_state == 0)){
      
      if(set_emergency_warning_state==0){
        digitalWrite(EVCC_SW_MODE, HIGH); 
        led_error();
        OCPP_receive = "state=?";
        OCPP_sends();
        
        if(QRCODE_LINK != ""){
          EV_Display();
        }else{
          EV_Display_Refresh();
        }
        
        //for(int i=147; i<161; i++){tft.drawCircle(240,160,i,TFT_RED);}
        tft.drawArc(239, 159, 147, 161, 0, 360, TFT_RED, TFT_BLACK, true);
        EV_emergency_warning_display();
        
        set_stand_by_state = 0;
        id_card_require_display = 0;
        set_connect_state = 0;
        set_leakage_fault_state = 0;
        leakage_fault_point = 0;
        set_fault_e_state = 0;
        set_no_power_state = 0;
        
        set_emergency_warning_state=1;   
        set_get_out_emergency_warning_first_time_state = 0;   
      }
      
    //}
  } else if (x == "NO_POWER") {
    if(set_no_power_state == 0){
      digitalWrite(EVCC_SW_MODE, HIGH);
      OCPP_receive = "state=?";
      OCPP_sends();
      set_no_power_state = 1;
    }

  }else if (x == "STOP_CHARGING"){
    if(remote_start_transaction == 1){
      digitalWrite(EVCC_SW_MODE, LOW);
      set_stand_by_state = 0;
      id_card_require_display = 0;
      swipe_card_status = 0;
      set_connect_state = 0;
      D_power="0";
      D_unit="kWh";
      D_total_power="";
      set_leakage_fault_state = 0;
      leakage_fault_point = 0;
      set_fault_e_state = 0;
      set_emergency_warning_state = 0;
      set_no_power_state = 0;
      set_stop_charging_state = 0;
      send_ocpp_plugin = 0;
     if(SuspendedEV == 0){
              sec0 = 0;
              sec1 = 0;
              min2 = 0;
              min3 = 0;
              hour4 = 0;
              hour5 = 0;
              D_time = String(hour5) + String(hour4) + ":" + String(min3) + String(min2) + ":" + String(sec1) + String(sec0);

              power = 0;
            }

    }else{
      if(set_stop_charging_state == 0){
        led_connect();
        OCPP_receive = "state=?";
        OCPP_sends();
        while(digitalRead(EVCC_SW_MODE)==LOW){}
        EV_Display();
        //for(int i=147; i<161; i++){tft.drawCircle(240,160,i,CYAN);}
        tft.drawArc(239, 159, 147, 161, 0, 360, CYAN, TFT_BLACK, true);
        EV_Display_Refresh();
        set_connect_state = 1;
        set_stand_by_state = 0;
        id_card_require_display = 0;
        set_charging_mode_state = 0;
        set_leakage_fault_state = 0;
        leakage_fault_point = 0;
        set_no_power_state = 0;
        swipe_card_status = 0;
        set_stop_charging_state = 1;
      }
    }
  }
  

  second_loop();

  EV_Read_UART();

  OCPP_recieves();

  UpdateDisplayFirmware();

  command_from_serial();

  rfid_any_state();

}

void rfid_any_state(){
  if ((digitalRead(rfid_check_pin) == HIGH)&&(rfid_control==1)) {
    
    if(millis() - swipe_card_delay_500 >= 500){
      if((x=="SWIPE_CARD")&&(QRCODE_LINK!="")){
        
      }else{
        in_waiting_card();
      }
      rfid.PCD_Init();  // Init MFRC522

      // Serial.print("D500ms_");

      if(x == "SWIPE_CARD"){
        // Serial.print("SC_");

        if(evcc_lock==0){

          if(set_stop_charging_state == 0){
            
            if(pass_rfid_card==1){
              EV_rfid_require_card_access();
              digitalWrite(EVCC_SW_MODE, LOW);
              pass_rfid_card = 0;
              tap_card_status = 0;
            }
            if(wrong_rfid_card==1){
              EV_rfid_require_card_wrong();
              digitalWrite(EVCC_SW_MODE, HIGH);
              id_card_require_display = 0;
              wrong_rfid_card = 0;
              tap_card_status = 0;
            }
            if(reservation_rfid_card == 1){
              EV_rfid_require_card_reservation();
              digitalWrite(EVCC_SW_MODE, HIGH);
              id_card_require_display = 0;
              reservation_rfid_card = 0;
              tap_card_status = 0;
            }

            if(millis() - wait_for_delay_5000 >= 5000){
              // Serial.print("D5s_");
              if(tap_card_status == 1){
                EV_rfid_disconnect_with_server();
                digitalWrite(EVCC_SW_MODE, HIGH);
                id_card_require_display = 0;
                wrong_rfid_card = 0;
                tap_card_status = 0;
              }
              // Look for new cards
              if (rfid.PICC_IsNewCardPresent()) {
                // Select one of the cards
                if (rfid.PICC_ReadCardSerial()){
                  //Show UID on serial monitor
                  Serial.print("UID tag :");
                  String tagContent= "";
                  byte letter;
                  for (byte i = 0; i < rfid.uid.size; i++) 
                  {
                    Serial.print(rfid.uid.uidByte[i], HEX);
                    tagContent.concat(String(rfid.uid.uidByte[i], HEX));
                  }
                  tagContent.toUpperCase();
                  Serial.println();
                  swipe_waiting_card();

                  EV_rfid_require_card_wait();
                  
                  OCPP_receive = "rfid-check-card=?";
                  rfid_card_last_swip_num_1 = "VerifyRFIDtag" + tagContent;
                  OCPP_sends();

                  tap_card_status = 1;

                  // Serial.print("TP_");

                  wait_for_delay_5000 = millis();
                }
              } 
            }
          }
          else{
            if (rfid.PICC_IsNewCardPresent()) {
              // Select one of the cards
              if (rfid.PICC_ReadCardSerial()){
                //Show UID on serial monitor
                Serial.print("UID tag :");
                String tagContent= "";
                byte letter;
                for (byte i = 0; i < rfid.uid.size; i++) 
                {
                  Serial.print(rfid.uid.uidByte[i], HEX);
                  tagContent.concat(String(rfid.uid.uidByte[i], HEX));
                }
                tagContent.toUpperCase();
                Serial.println();
                swipe_waiting_card();

                // Serial.print("FN.in.SC_");
              }
            } 
          }
        }
        else{
          if (rfid.PICC_IsNewCardPresent()) {
            // Select one of the cards
            if (rfid.PICC_ReadCardSerial()){
              //Show UID on serial monitor
              Serial.print("UID tag :");
              String tagContent= "";
              byte letter;
              for (byte i = 0; i < rfid.uid.size; i++) 
              {
                Serial.print(rfid.uid.uidByte[i], HEX);
                tagContent.concat(String(rfid.uid.uidByte[i], HEX));
              }
              tagContent.toUpperCase();
              Serial.println();
              swipe_waiting_card();
            }  
          }   
        }
      }
      else if(((x == "CONNECTED")||(x == "FINISHED"))||(x == "CHARGING_MODE")){
        // Serial.print("ChGi_");
        
        if(wrong_rfid_card==1){
          EV_rfid_require_card_wrong();
          digitalWrite(EVCC_SW_MODE, LOW);
          set_connect_state = 0; 
          set_charging_mode_state=0;
          wrong_rfid_card = 0;
          tap_card_status = 0;
        }

        if(millis() - wait_for_delay_5000 > 5000){
          // Serial.print("D5s_");
          if(tap_card_status == 1){
            EV_rfid_disconnect_with_server();
            digitalWrite(EVCC_SW_MODE, LOW);
            set_connect_state = 0; 
            set_charging_mode_state=0;
            wrong_rfid_card = 0;
            tap_card_status = 0;
          }
          // Look for new cards
          if (rfid.PICC_IsNewCardPresent()) {
            // Select one of the cards
            if (rfid.PICC_ReadCardSerial()){
              //Show UID on serial monitor
              Serial.print("UID tag :");
              String tagContent= "";
              byte letter;
              for (byte i = 0; i < rfid.uid.size; i++) 
              {

                Serial.print(rfid.uid.uidByte[i], HEX);

                tagContent.concat(String(rfid.uid.uidByte[i], HEX));
              }
              tagContent.toUpperCase();
              Serial.println();
              swipe_waiting_card();

              EV_rfid_require_card_wait();
              
              OCPP_receive = "rfid-check-card=?";
              rfid_card_last_swip_num_1 = "VerifyRFIDtag" + tagContent;
              OCPP_sends();

              tap_card_status = 1;

              // Serial.print("TP_");

              wait_for_delay_5000 = millis();
            }
          }
        }
      }
      else{
        //if((x == "STAND_BY"||x == "NO_POWER" )|| x == "STOP_CHARGING"){
          // Serial.print("NorS_");
              
              // Look for new cards
              if (rfid.PICC_IsNewCardPresent()) {
                // Select one of the cards
                if (rfid.PICC_ReadCardSerial()){
                  //Show UID on serial monitor
                  Serial.print("UID tag :");
                  String tagContent= "";
                  byte letter;
                  for (byte i = 0; i < rfid.uid.size; i++) 
                  {
                    Serial.print(rfid.uid.uidByte[i], HEX);
                    tagContent.concat(String(rfid.uid.uidByte[i], HEX));
                  }
                  tagContent.toUpperCase();
                  Serial.println();
                  swipe_waiting_card();

                  tap_card_status = 0;

                  // Serial.print("TP_");
                }
              }
        //}
      }
      swipe_card_delay_500 = millis();
      // Serial.println();
    }
    
  }
  
    
}

void UpdateDisplayFirmware(){
  if(required_firmware_update == 1){
    EV_update_display();
    if(String(WiFi.localIP()) != "0"){
      Serial.println("wifi connected!");
      if (WiFi.status()  == WL_CONNECTED) {
        Serial.println("wait display update!");
        Serial.println("start update!");

        WiFiClient client;

        t_httpUpdate_return ret = httpUpdate.update(client, Firmware_Link_Update_Display);
        switch (ret) {
          case HTTP_UPDATE_FAILED:
            Serial.printf("HTTP_UPDATE_FAILED Error (%d): %s\n", httpUpdate.getLastError(), httpUpdate.getLastErrorString().c_str());
            break;

          case HTTP_UPDATE_NO_UPDATES:
            Serial.println("HTTP_UPDATE_NO_UPDATES");
            break;

          case HTTP_UPDATE_OK:
            Serial.println("HTTP_UPDATE_OK");
            break;
        }
      }
    }else{
      WiFi.begin(STASSID, STAPSK);
      for(int i=0;i<5;i++){
        if(!WiFi.isConnected()){
          Serial.println("wait wifi!");
          delay(1000);
        }else{
          i=10;
          Serial.println("wifi connected!");
        }
      }
      if(String(WiFi.localIP()) == "0"){
        Serial.println("wifi disconnected!");
      }
        
    }
  }
}

void second_loop() {

  if(wait_display == 0){

    if((x=="SWIPE_CARD")&&(QRCODE_LINK!="")){
        
    }else{
      wifistrength();

      lan_connection();

      tle_4G_connection();
    }

  }

  if(round_command_set_pass == round_command_set){
    if(millis() - evcc_question_timer >= 2000){
      round_command_set += 1;
      // Serial1.begin(115200, SERIAL_8N1, RX, TX);
    }
  }

  if(round_command_set_pass != round_command_set) {

    if(maxCurrent==1){

        if (x == "CHARGING_MODE") {
          if (round_command_set == 0) {
            // Serial.println("read C1");
            EV_current_L1();
            // delay(300);
            // EV_current_L1();
          } else if (round_command_set == 1) {
            // Serial.println("read C2");
            EV_current_L2();
            // delay(300);
            // EV_current_L2();
          } else if (round_command_set == 2) {
            // Serial.println("read C3");
            EV_current_L3();
            // delay(300);
            // EV_current_L3();
          } else if (round_command_set == 3) {
            // Serial.println("readVolt");
            EV_volt();
            // delay(300);
            // EV_volt();
          } else if (round_command_set == 4) {
            // Serial.println("readTemp");
            EV_temp();
            // delay(300);
            // EV_temp();
          } else if (round_command_set == 5) {
            // Serial.println("readState");
            EV_pilot_state();
            // delay(300);
            // EV_pilot_state();
          } else {
            // Serial.println("read C1");
            EV_current_L1();
            // delay(300);
            // EV_current_L1();
            round_command_set = 0;
          }
          EV_power();

        // other state
        } else {
          if (round_command_set == 0) {
            // Serial.println("readVolt");
            EV_volt();
            // delay(300);
            // EV_volt();
          } else if (round_command_set == 1) {
            // Serial.println("readTemp");
            EV_temp();
            // delay(300);
            // EV_temp();
          } else if (round_command_set == 2) {
            // Serial.println("readState");
            EV_pilot_state();
            // delay(300);
            // EV_pilot_state();
          } else {
            // Serial.println("readVolt");
            EV_volt();
            // delay(300);
            // EV_volt();
            round_command_set = 0;
          }

          current_L1 = 0;
          current_L2 = 0;
          current_L3 = 0;

        }


      evcc_question_timer = millis();
      round_command_set_pass = round_command_set;
    
    } else {

      if(maxCurrent==6){
        maxCurrent6();
        Serial.print("666666666666");
        delay(500);
        maxCurrent6();
        maxCurrent = 1;
      }else if(maxCurrent==7){
        maxCurrent7();
        delay(500);
        maxCurrent7();
        maxCurrent = 1;
      }else if(maxCurrent==8){
        maxCurrent8();
        delay(500);
        maxCurrent8();
        maxCurrent = 1;
      }else if(maxCurrent==9){
        maxCurrent9();
        delay(500);
        maxCurrent9();
        maxCurrent = 1;
      }else if(maxCurrent==10){
        maxCurrent10();
        delay(500);
        maxCurrent10();
        maxCurrent = 1;
      }else if(maxCurrent==11){
        maxCurrent11();
        delay(500);
        maxCurrent11();
        maxCurrent = 1;
      }else if(maxCurrent==12){
        maxCurrent12();
        delay(500);
        maxCurrent12();
        maxCurrent = 1;
      }else if(maxCurrent==13){
        maxCurrent13();
        delay(500);
        maxCurrent13();
        maxCurrent = 1;
      }else if(maxCurrent==14){
        maxCurrent14();
        delay(500);
        maxCurrent14();
        maxCurrent = 1;
      }else if(maxCurrent==15){
        maxCurrent15();
        delay(500);
        maxCurrent15();
        maxCurrent = 1;
      }else if(maxCurrent==16){
        maxCurrent16();
        Serial.print("1666666666666");
        delay(500);
        maxCurrent16();
        maxCurrent = 1;
      }else if(maxCurrent==17){
        maxCurrent17();
        delay(500);
        maxCurrent17();
        maxCurrent = 1;
      }else if(maxCurrent==18){
        maxCurrent18();
        delay(500);
        maxCurrent18();
        maxCurrent = 1;
      }else if(maxCurrent==19){
        maxCurrent19();
        delay(500);
        maxCurrent19();
        maxCurrent = 1;
      }else if(maxCurrent==20){
        maxCurrent20();
        delay(500);
        maxCurrent20();
        maxCurrent = 1;
      }else if(maxCurrent==21){
        maxCurrent21();
        delay(500);
        maxCurrent21();
        maxCurrent = 1;
      }else if(maxCurrent==22){
        maxCurrent22();
        delay(500);
        maxCurrent22();
        maxCurrent = 1;
      }else if(maxCurrent==23){
        maxCurrent23();
        delay(500);
        maxCurrent23();
        maxCurrent = 1;
      }else if(maxCurrent==24){
        maxCurrent24();
        delay(500);
        maxCurrent24();
        maxCurrent = 1;
      }else if(maxCurrent==25){
        maxCurrent25();
        delay(500);
        maxCurrent25();
        maxCurrent = 1;
      }else if(maxCurrent==26){
        maxCurrent26();
        delay(500);
        maxCurrent26();
        maxCurrent = 1;
      }else if(maxCurrent==27){
        maxCurrent27();
        delay(500);
        maxCurrent27();
        maxCurrent = 1;
      }else if(maxCurrent==28){
        maxCurrent28();
        delay(500);
        maxCurrent28();
        maxCurrent = 1;
      }else if(maxCurrent==29){
        maxCurrent29();
        delay(500);
        maxCurrent29();
        maxCurrent = 1;
      }else if(maxCurrent==30){
        maxCurrent30();
        delay(500);
        maxCurrent30();
        maxCurrent = 1;
      }else if(maxCurrent==31){
        maxCurrent31();
        delay(500);
        maxCurrent31();
        maxCurrent = 1;
      }else if(maxCurrent==32){
        maxCurrent32();
        Serial.print("3222222222222");
        delay(500);
        maxCurrent32();
        maxCurrent = 1;
      }
      delay(300);
    }
  }

    // if(voltage_cutting == 1){
    //   if(SW_mode == 0){
    //     //SW_CONF();
    //     //delay(1000);
    //     //remote_off();
    //     //delay(1000);
    //     SW_mode = 1;
    //     HW_mode = 0;
    //   }
    // }else{
    //   if(HW_mode == 0){
    //     //HW_CONF();
    //     //delay(1000);
    //     SW_mode = 0;
    //     HW_mode = 1;
    //   }
    // }


    if(key != ""){
      if (key == "solfwaremode")
      {
        SW_CONF();
      }
      else if (key == "hardwaremode")
      {
        HW_CONF();
      }
      else if (key == "remoteoff")
      {
        remote_off();
      }
      else if (key == "remoteon")
      {
        remote_on();
      }
      else if (key == "remotemode")
      {
        byte message[] = { 0x31, 0x32, 0x30, 0x00 };
        Serial1.write(message, sizeof(message));
      }
      else if (key == "remotestatus")
      {
        byte message[] = { 0x31, 0x32, 0x31, 0x00 };
        Serial1.write(message, sizeof(message));
      }
      else if (key == "readevsereadyregister")
      {
        byte message[] = { 0x31, 0x33, 0x31, 0x00 };
        Serial1.write(message, sizeof(message));
      }
      else if (key == "rV")
      {
        EV_volt();
        delay(300);
        EV_volt();
      }
      else if (key == "rS")
      {
        EV_pilot_state();
        delay(300);
        EV_pilot_state();
      }
      else if (key == "rT")
      {
        EV_temp();
        delay(300);
        EV_temp();
      }
      else if (key == "rc1")
      {
        if(x == "CHARGING_MODE"){
          Serial.println("read C1");
          EV_current_L1();
          delay(300);
          EV_current_L1();
        }else{
          current_L1 = 0;
        }
      }
      else if (key == "rc2")
      {
        if(x == "CHARGING_MODE"){
          Serial.println("read C2");
          EV_current_L2();
          delay(300);
          EV_current_L2();
        }else{
          current_L2 = 0;
        }
      }
      else if (key == "rc3")
      {
        if(x == "CHARGING_MODE"){
          Serial.println("read C3");
          EV_current_L3();
          delay(300);
          EV_current_L3();
        }else{
          current_L3 = 0;
        }
      }

      // read_serial1_available();
      delay(1000);
      key = "";

      EV_power();
    }

  if ((((((set_leakage_fault_state == 0) && (set_fault_e_state == 0)) && (x != "SWIPE_CARD"))&&(x != "EMERGENCY_WARNING"))&&((temp_alarm_count==0)||((temp_alarm_count)>=6)))&&(wait_display == 0)) {      
    if(millis() - delay_refresh_display >= 1000){
      EV_Display_Refresh();
      delay_refresh_display = millis();
    }
      
  }

}

void serial_monitor_print() {
  Serial.println();
  
  Serial.print(x);
  Serial.print(": ");
  Serial.print(x_pass);

  Serial.print("  Vmains: ");
  Serial.print(volt);

  Serial.print("  I_L1: ");
  Serial.print(current_L1);

  Serial.print("  I_L2: ");
  Serial.print(current_L2);

  Serial.print("  I_L3: ");
  Serial.print(current_L3);

  Serial.print("  POWER: ");
  Serial.print(power);

  Serial.print("  SUM_P: ");
  Serial.print(power_sum_sec);

  Serial.print("  TEMP: ");
  Serial.print(temp);

  Serial.print("  TIME: ");
  Serial.print(D_time);

  Serial.print("   evcc_lock = ");
  Serial.print(evcc_lock);

  Serial.print("    rfid_control = ");
  Serial.print(rfid_control);

  Serial.println();
  Serial.print("OCPP recieves = ");
  Serial.print(OCPP_receive);

  Serial.println();
  Serial.print("OCPP send = ");
  Serial.print(OCPP_send);
  
  Serial.println();
}

void OCPP_recieves(){
  int inByte[1000];
  int i = 0;
  if(Serial2.available() > 0){
    OCPP_receive = "";
    //Serial.println("");
    //Serial.print("incomming = ");

    while (Serial2.available() > 0) {
      
      byte incomming = Serial2.read();
      //Serial.print(incomming);
      OCPP_receive = OCPP_receive + ((char)incomming);
      inByte[i] = incomming;
      i++;
    }
    //delay(100);
    //Serial.print("OCPP recieves = ");
    //Serial.println(OCPP_receive);

    data_memory = data_memory + OCPP_receive;

    String My_S = data_memory;
    
    if(My_S.indexOf('*') != -1){

      if(My_S.indexOf('#') != -1){

        if(My_S.indexOf('*') < My_S.indexOf('#')){

          if(My_S.indexOf('*') > 0){
            My_S.replace(My_S.substring(0, My_S.indexOf('*') - 1), "");
          }

          if(My_S.endsWith("#")){
            data_memory = "";

            My_S.replace(My_S.substring(My_S.indexOf('#') - 4, My_S.length()), "");
            My_S.remove(0, 4);
            OCPP_receive = My_S;
            Serial.print("OCPP recieves = ");
            Serial.println(OCPP_receive);
            OCPP_sends();
          }else{
            String My_BS = My_S;
            My_BS.replace(My_BS.substring(0, My_BS.indexOf('#') - 1), "");
            data_memory = My_BS;

            My_S.replace(My_S.substring(My_S.indexOf('#') - 4, My_S.length()), "");
            My_S.remove(0, 4);
            OCPP_receive = My_S;
            Serial.print("OCPP recieves = ");
            Serial.println(OCPP_receive);
            OCPP_sends();
          }
          
          
        }else{
          if(My_S.indexOf('*') > 0){
            My_S.replace(My_S.substring(0, My_S.indexOf('*') - 1), "");
          }
          data_memory = My_S;
        }

      }else{
        if(My_S.indexOf('*') > 0){
          My_S.replace(My_S.substring(0, My_S.indexOf('*') - 1), "");
        }
        data_memory = My_S;
      }

    }else{
      data_memory = "";
    }
  }
}

void OCPP_sends(){

      if(OCPP_receive.startsWith("EV_FLUD=")){
        OCPP_receive.remove(0, 8);
        Firmware_Link_Update_Display = OCPP_receive;
        Serial.println(OCPP_receive);
      }else if(OCPP_receive.startsWith("EV_WIFI=")){
        OCPP_receive.remove(0, 8);
        Serial.println(OCPP_receive);
        WIFI = OCPP_receive;
      }else if(OCPP_receive.startsWith("EV_PASS=")){
        OCPP_receive.remove(0, 8);
        Serial.println(OCPP_receive);
        PASS = OCPP_receive;
      }else if(OCPP_receive.startsWith("EV_QRCODE=")){
        OCPP_receive.remove(0, 10);
        Serial.println(OCPP_receive);
        QRCODE_LINK = OCPP_receive;
        if(x == "SWIPE_CARD"){
          EV_rfid_require_card();
        }
        sendData = "OK";
      }else if(OCPP_receive == "state=?"){
        sendData = "Verifystate" + x;
      }else if(OCPP_receive == "volt=?"){
        sendData = D_volt;
      }else if(OCPP_receive == "currentl1=?"){
        sendData = D_current_L1;
      }else if(OCPP_receive == "currentl2=?"){
        sendData = D_current_L2;
      }else if(OCPP_receive == "currentl3=?"){
        sendData = D_current_L3;
      }else if(OCPP_receive == "temparature=?"){
        sendData = D_temp;
      }else if(OCPP_receive == "timer=?"){
        sendData = D_time;
      }else if(OCPP_receive == "sum-power=?"){
        sendData = D_power_sum_sec;
      }else if(OCPP_receive == "power=?"){
        sendData = D_power;
      }else if(OCPP_receive == "rfid-check-pin=?"){
        String rfidxcheckxpin;
        if(digitalRead(rfid_check_pin)==1){rfidxcheckxpin="1";}
        else{rfidxcheckxpin="0";}
        sendData = rfidxcheckxpin;
      }else if(OCPP_receive == "rfid-check-card=?"){
        sendData = rfid_card_last_swip_num_1;
      }else if(OCPP_receive == "rfid-control=?"){
        sendData = rfid_control;
      }else if(OCPP_receive == "rfid-control=0"){
        rfid_control = 0;
        sendData = "OK";
        id_card_require_display = 0;
      }else if(OCPP_receive == "rfid-control=1"){
        rfid_control = 1;
        sendData = "OK";
      }else if(OCPP_receive == "remote-start-transaction=1"){
        remote_start_transaction = 1;
        digitalWrite(EVCC_SW_MODE, LOW);
        sendData = "OK";
      }else if(OCPP_receive == "remote-start-transaction=0"){
        remote_start_transaction = 0;
        digitalWrite(EVCC_SW_MODE, HIGH);
        if(x != "EMERGENCY_WARNING"){
          x = "STOP_CHARGING";
        }
        sendData = "OK";
      }else if(OCPP_receive == "reserved_to_display=0"){
        reserved_status = 0;
        EV_Display_Refresh();
        sendData = "OK";
      }else if(OCPP_receive == "reserved_to_display=1"){
        reserved_status = 1;
        EV_Display_Refresh();
        sendData = "OK";
      }else if(OCPP_receive == "evcc-lock=?"){
        sendData = evcc_lock;
      }else if(OCPP_receive == "evcc-lock=0"){
        evcc_lock = 0;
        sendData = "OK";
      }else if(OCPP_receive == "evcc-lock=1"){
        evcc_lock = 1;
        sendData = "OK";
      }else if(OCPP_receive == "ev-mode=?"){
        String evxmode;
        if(digitalRead(EVCC_SW_MODE)==1){evxmode="1";}
        else{evxmode="0";}
        sendData = evxmode;
      }else if(OCPP_receive == "ev-mode=0"){
        wrong_rfid_card = 1;
        sendData = "OK";
      }else if(OCPP_receive == "ev-mode=1"){
        pass_rfid_card = 1;
          // if(x == "SWIPE_CARD"){
          //   if(pass_rfid_card==1){
          //         EV_rfid_require_card_access();
          //         digitalWrite(EVCC_SW_MODE, LOW);
          //         pass_rfid_card = 0;
          //         tap_card_status = 0;
          //       }
          // }
          if(((x == "CONNECTED")||(x == "FINISHED"))||(x == "CHARGING_MODE")){
            if(pass_rfid_card==1){
                  digitalWrite(EVCC_SW_MODE, HIGH);
                  delay(3000);
                  EV_rfid_require_card_access();
                  delay(2000);
                  set_connect_state = 0; 
                  set_charging_mode_state=0;
                  pass_rfid_card = 0;
                  x = "STOP_CHARGING";
            }
          }
        sendData = "OK";
      }else if(OCPP_receive == "emergency-pin=?"){
        String emergencyxpin;
        if(digitalRead(EMERGENCY_PIN)==1){emergencyxpin="1";}
        else{emergencyxpin="0";}
        sendData = emergencyxpin;
      }else if(OCPP_receive == "FOUND-WRONG-COMMAND"){
        sendData = "||(>_<)||";
      }else if(OCPP_receive == "ev-stop-charging=?"){
        String ev_stop_charging;
        if(digitalRead(EVCC_SW_MODE)==1){ev_stop_charging="1";}
        else{ev_stop_charging="0";}
        sendData = ev_stop_charging;
      }else if(OCPP_receive == "ev-stop-charging=0"){
        digitalWrite(EVCC_SW_MODE, LOW);
        sendData = "OK";
      }else if(OCPP_receive == "ev-stop-charging=1"){
        digitalWrite(EVCC_SW_MODE, HIGH);
        sendData = "OK";
      }else if(OCPP_receive == "reservation-mode=1"){
        reservation_rfid_card = 1;
        sendData = "OK";
      }else if(OCPP_receive == "rssi=90"){
        wifistr = 90;
        sendData = "OK";
      }else if(OCPP_receive == "rssi=60"){
        wifistr = 60;
        sendData = "OK";
      }else if(OCPP_receive == "rssi=40"){
        wifistr = 40;
        sendData = "OK";
      }else if(OCPP_receive == "rssi=20"){
        wifistr = 20;
        sendData = "OK";
      }else if(OCPP_receive == "rssi=0"){
        wifistr = 0;
        sendData = "OK";
      }else if(OCPP_receive.startsWith("maxCurrent=")){
        if(OCPP_receive == "maxCurrent=?"){
          sendData = String(maxCurrent);
        }else{
          OCPP_receive.remove(0, 11);
          maxCurrent = OCPP_receive.toInt();
          sendData = "OK";
        }
      }else if(OCPP_receive == "lan-connected=0"){
        lan_connected=0;
        sendData = "OK";
      }else if(OCPP_receive == "lan-connected=1"){
        lan_connected=1;
        sendData = "OK";
      }else if(OCPP_receive == "tle-4G-connected=0"){
        tle_4G_connected=0;
        sendData = "OK";
      }else if(OCPP_receive == "tle-4G-connected=1"){
        tle_4G_connected=1;
        sendData = "OK";
      }else if(OCPP_receive == "ping-from-ocpp-board"){
        sendData = "PONG";
      }else if(OCPP_receive == "UpdateDisplay"){
        required_firmware_update = 1;
        sendData = "OK";
      }else if(OCPP_receive == "StopUpdateDisplay"){
        required_firmware_update = 0;
        EV_Display();
        sendData = "OK";
      }else if(OCPP_receive == "SuspendedEV=0"){
        SuspendedEV = 0;
        sendData = "OK";
        // if(x != "EMERGENCY_WARNING"){
        //   EV_Display();
        //   EV_Display_Refresh();
        // }
        
      }else if(OCPP_receive == "SuspendedEV=1"){
        SuspendedEV = 1;
        sendData = "OK";
        //EV_Display();
      }else if(OCPP_receive == "Loading_Display=0"){
        wait_display = 0;
        EV_Display();
        sendData = "OK";
      }else if(OCPP_receive == "Loading_Display=1"){
        wait_display = 1;
        Loading_Display();
        sendData = "OK";
      }else if(OCPP_receive == "Setup_Display=1"){
        wait_display = 1;
        Setup_Display();
        sendData = "OK";
      }else{
        sendData = "||(>.,<)||";
      }

      OCPP_send = "ICE*"+ sendData +"*PAT";
      writeString(OCPP_send);
    
}
// for send data
void writeString(String stringData) { // Used to serially push out a String with Serial.write()
  for (int i = 0; i < stringData.length(); i++)
  {
    Serial1.write(stringData[i]);   // Push each char 1 by 1 on each loop pass
  }
  Serial.print("OCPP send = ");
  Serial.println(OCPP_send);

}// end writeString

void EV_Read_UART() {
  
  int inByte[1000];
  int i = 100;
  byte incomming = 0;
  byte checknewincomming = inByte[i];
  while (Serial1.available() > 0) {
    incomming = Serial1.read();
    // Serial.print(incomming);
    // Serial.print(" ");
    for(int j=0; j<i; j++){
      inByte[j] = inByte[j+1];
    }
    inByte[i] = incomming;
    // i++;
  }

  // if(incomming==0){

  // } else {
  //   Serial.println();
  // }
  if(checknewincomming != inByte[i]){
    Serial.println();
    Serial.print("TestCheck: ");
    for( int checkcheck= 0; checkcheck<i; checkcheck++){
      Serial.print(inByte[checkcheck]);
      Serial.print(" ");
    }
    Serial.println();
  }
  

  for (int A_loop = 0; A_loop + 2 < i; A_loop++) {
    if (inByte[A_loop] == 160) {
      if (inByte[A_loop + 1] == 49) {
        if (inByte[A_loop + 2] == 49) {if(digitalRead(EMERGENCY_PIN)==1){} else {x = "STAND_BY";} } //A1
        if (inByte[A_loop + 2] == 50) {if(digitalRead(EMERGENCY_PIN)==1){} else {x = "FAULT_PE";} } //A2
        if (inByte[A_loop + 2] == 51) {if(digitalRead(EMERGENCY_PIN)==1){} else {x = "SWIPE_CARD";} } //B1
        if (inByte[A_loop + 2] == 52) {if(digitalRead(EMERGENCY_PIN)==1){} else {x = "CONNECTED";} } //B2
        if (inByte[A_loop + 2] == 53) {if(digitalRead(EMERGENCY_PIN)==1){} else {x = "STOP_CHARGING";} } //C1
        if (inByte[A_loop + 2] == 54) {if(digitalRead(EMERGENCY_PIN)==1){} else {x = "CHARGING_MODE";} } //C2
        if (inByte[A_loop + 2] == 55) {if(digitalRead(EMERGENCY_PIN)==1){} else {x = "NO_POWER";} } //D1
        if (inByte[A_loop + 2] == 56) {if(digitalRead(EMERGENCY_PIN)==1){} else {x = "D2";} } //D2
        if (inByte[A_loop + 2] == 57) {if(digitalRead(EMERGENCY_PIN)==1){} else {x = "FAULT_E"; } }
        if (inByte[A_loop + 2] == 58) {
          if(digitalRead(EMERGENCY_PIN)==1){} 
          else {
            if(set_no_power_state==0){
              if(leak_score>=3){
                x = "LEAKAGE_FAULT";
              }
              leak_score++;
            } 
          } 
        }
        Serial.print("state: ");
        Serial.println(x);

        inByte[A_loop] = 0;
      }
    }
    if (inByte[A_loop] == 177){
      if (inByte[A_loop + 1] == 48) {
        if (inByte[A_loop + 2] == 49) {if(digitalRead(EMERGENCY_PIN)==1){} else {x = "STAND_BY";leak_score=0;} } //A1
        if (inByte[A_loop + 2] == 50) {if(digitalRead(EMERGENCY_PIN)==1){} else {x = "FAULT_PE";leak_score=0;} } //A2
        if (inByte[A_loop + 2] == 51) {if(digitalRead(EMERGENCY_PIN)==1){} else {x = "SWIPE_CARD";leak_score=0;} } //B1
        if (inByte[A_loop + 2] == 52) {if(digitalRead(EMERGENCY_PIN)==1){} else {x = "CONNECTED";leak_score=0;} } //B2
        if (inByte[A_loop + 2] == 53) {if(digitalRead(EMERGENCY_PIN)==1){} else {x = "STOP_CHARGING";leak_score=0;} } //C1
        if (inByte[A_loop + 2] == 54) {if(digitalRead(EMERGENCY_PIN)==1){} else {x = "CHARGING_MODE";leak_score=0;} } //C2
        if (inByte[A_loop + 2] == 55) {if(digitalRead(EMERGENCY_PIN)==1){} else {x = "NO_POWER";leak_score=0;} } //D1
        if (inByte[A_loop + 2] == 56) {if(digitalRead(EMERGENCY_PIN)==1){} else {x = "D2";leak_score=0;} } //D2
        if (inByte[A_loop + 2] == 57) {if(digitalRead(EMERGENCY_PIN)==1){} else {x = "FAULT_E";leak_score=0; } }
        if (inByte[A_loop + 2] == 58) {
          if(digitalRead(EMERGENCY_PIN)==1){} 
          else {
            if(set_no_power_state==0){
              if(leak_score>=3){
                x = "LEAKAGE_FAULT";
              }
              leak_score++;
            } 
          } 
        }
        Serial.print("state: ");
        Serial.println(x);
        round_command_set += 1;

        inByte[A_loop] = 0;
      }
    }
    if (inByte[A_loop] == 188) {
      volt = ((0.274 * ((inByte[A_loop + 1] * 256) + inByte[A_loop + 2])) + 29.29);
      //if((volt>=260)||(volt<=190)){volt=0;}
      float Ex_Float = volt;                           // สร้างตัวแปรชื่อ Ex_Float ชนิด float (ทศนิยม)
      int Ex_Int = int(Ex_Float);                      // สร้างตัวแปรชื่อ Ex_Int ชนิด int (จำนวนเต็ม) จากนั้นแปลง Ex_Float เป็นจำนวนเต็ม ทำให้ทศนิยมถูกตัดออก
      long Ex_Dec = 100 * (Ex_Float - Ex_Int);         // สร้างตัวแปรชื่อ Ex_Int ชนิด int (จำนวนเต็ม)
      //D_volt = String(Ex_Int) + "." + String(Ex_Dec);  // นำสตริงทั้งหมดมาต่อกัน
      D_volt = String(Ex_Int);
      if(volt==0){D_volt="0";}
      if(x=="NO_POWER"){
        if((volt>190) && (volt<261)){
          x="STAND_BY";
        }
      }
      if((volt<=200) || (volt>=260)){
        voltage_cutting = 1;
      }else{
        voltage_cutting = 0;
      }
      Serial.print("volt: ");
      Serial.println(volt);
      round_command_set += 1;

      inByte[A_loop] = 0;
    }
    if (inByte[A_loop] == 185) {
      current_L1 = ((((inByte[A_loop + 1] * 256) + inByte[A_loop + 2]) / 2.14) - 0.434);
      if ((current_L1 < 0) || (x != "CHARGING_MODE")) { current_L1 = 0; }
      float Ex_Float = current_L1;                           // สร้างตัวแปรชื่อ Ex_Float ชนิด float (ทศนิยม)
      int Ex_Int = int(Ex_Float);                            // สร้างตัวแปรชื่อ Ex_Int ชนิด int (จำนวนเต็ม) จากนั้นแปลง Ex_Float เป็นจำนวนเต็ม ทำให้ทศนิยมถูกตัดออก
      long Ex_Dec = 100 * (Ex_Float - Ex_Int);               // สร้างตัวแปรชื่อ Ex_Int ชนิด int (จำนวนเต็ม)
      D_current_L1 = String(Ex_Int) + "." + String(Ex_Dec);  // นำสตริงทั้งหมดมาต่อกัน
      if(current_L1==0){D_current_L1="0";}
      Serial.print("C1: ");
      Serial.println(current_L1);
      round_command_set += 1;

      inByte[A_loop] = 0;
    }
    if (inByte[A_loop] == 186) {
      current_L2 = ((((inByte[A_loop + 1] * 256) + inByte[A_loop + 2]) / 2.14) - 0.434);
      if ((current_L2 < 0) || (x != "CHARGING_MODE")) { current_L2 = 0; }
      float Ex_Float = current_L2;                           // สร้างตัวแปรชื่อ Ex_Float ชนิด float (ทศนิยม)
      int Ex_Int = int(Ex_Float);                            // สร้างตัวแปรชื่อ Ex_Int ชนิด int (จำนวนเต็ม) จากนั้นแปลง Ex_Float เป็นจำนวนเต็ม ทำให้ทศนิยมถูกตัดออก
      long Ex_Dec = 100 * (Ex_Float - Ex_Int);               // สร้างตัวแปรชื่อ Ex_Int ชนิด int (จำนวนเต็ม)
      D_current_L2 = String(Ex_Int) + "." + String(Ex_Dec);  // นำสตริงทั้งหมดมาต่อกัน
      if(current_L2==0){D_current_L2="0";}
      Serial.print("C2: ");
      Serial.println(current_L2);
      round_command_set += 1;

      inByte[A_loop] = 0;
    }
    if (inByte[A_loop] == 187) {
      current_L3 = ((((inByte[A_loop + 1] * 256) + inByte[A_loop + 2]) / 2.14) - 0.434);
      if ((current_L3 < 0) || (x != "CHARGING_MODE")) { current_L3 = 0; }
      float Ex_Float = current_L3;                           // สร้างตัวแปรชื่อ Ex_Float ชนิด float (ทศนิยม)
      int Ex_Int = int(Ex_Float);                            // สร้างตัวแปรชื่อ Ex_Int ชนิด int (จำนวนเต็ม) จากนั้นแปลง Ex_Float เป็นจำนวนเต็ม ทำให้ทศนิยมถูกตัดออก
      long Ex_Dec = 100 * (Ex_Float - Ex_Int);               // สร้างตัวแปรชื่อ Ex_Int ชนิด int (จำนวนเต็ม)
      D_current_L3 = String(Ex_Int) + "." + String(Ex_Dec);  // นำสตริงทั้งหมดมาต่อกัน
      if(current_L3==0){D_current_L3="0";}
      Serial.print("C3: ");
      Serial.println(current_L3);
      round_command_set += 1;

      inByte[A_loop] = 0;
    }
    if (inByte[A_loop] == 191) {
      if (temp = ((inByte[A_loop + 1]) + (((inByte[A_loop + 2]) / 32) * 0.125)));
      float Ex_Float = temp;                           // สร้างตัวแปรชื่อ Ex_Float ชนิด float (ทศนิยม)
      int Ex_Int = int(Ex_Float);                      // สร้างตัวแปรชื่อ Ex_Int ชนิด int (จำนวนเต็ม) จากนั้นแปลง Ex_Float เป็นจำนวนเต็ม ทำให้ทศนิยมถูกตัดออก
      long Ex_Dec = 100 * (Ex_Float - Ex_Int);         // สร้างตัวแปรชื่อ Ex_Int ชนิด int (จำนวนเต็ม)
      //D_temp = String(Ex_Int) + "." + String(Ex_Dec);  // นำสตริงทั้งหมดมาต่อกัน
      D_temp = String(Ex_Int);
      if(D_volt=="0"){D_temp = "-";}
      Serial.print("temp: ");
      Serial.println(temp);
      round_command_set += 1;

      inByte[A_loop] = 0;
    }
  }
}
void maxCurrent6(){
  byte message[] = { 0x3A, 0x30, 0x06, 0x00 };  //6A
  Serial1.write(message, sizeof(message));
}
void maxCurrent7(){
  byte message[] = { 0x3A, 0x30, 0x07, 0x00 };  //7A
  Serial1.write(message, sizeof(message));
}
void maxCurrent8(){
  byte message[] = { 0x3A, 0x30, 0x08, 0x00 };  //8A
  Serial1.write(message, sizeof(message));
}
void maxCurrent9(){
  byte message[] = { 0x3A, 0x30, 0x09, 0x00 };  //9A
  Serial1.write(message, sizeof(message));
}
void maxCurrent10(){
  byte message[] = { 0x3A, 0x31, 0x31, 0x00 };  //10A
  Serial1.write(message, sizeof(message));
}
void maxCurrent11(){
  byte message[] = { 0x3A, 0x30, 0x0B, 0x00 };  //11A
  Serial1.write(message, sizeof(message));
}
void maxCurrent12(){
  byte message[] = { 0x3A, 0x30, 0x0C, 0x00 };  //12A
  Serial1.write(message, sizeof(message));
}
void maxCurrent13(){
  byte message[] = { 0x3A, 0x30, 0x0D, 0x00 };  //13A
  Serial1.write(message, sizeof(message));
}
void maxCurrent14(){
  byte message[] = { 0x3A, 0x30, 0x0E, 0x00 };  //14A
  Serial1.write(message, sizeof(message));
}
void maxCurrent15(){
  byte message[] = { 0x3A, 0x30, 0x0F, 0x00 };  //15A
  Serial1.write(message, sizeof(message));
}
void maxCurrent16(){
  byte message[] = { 0x3A, 0x31, 0x32, 0x00 };  //16A
  Serial1.write(message, sizeof(message));
}
void maxCurrent17(){
  byte message[] = { 0x3A, 0x30, 0x11, 0x00 };  //17A
  Serial1.write(message, sizeof(message));
}
void maxCurrent18(){
  byte message[] = { 0x3A, 0x30, 0x12, 0x00 };  //18A
  Serial1.write(message, sizeof(message));
}
void maxCurrent19(){
  byte message[] = { 0x3A, 0x30, 0x13, 0x00 };  //19A
  Serial1.write(message, sizeof(message));
}
void maxCurrent20(){
  byte message[] = { 0x3A, 0x30, 0x14, 0x00 };  //20A
  Serial1.write(message, sizeof(message));
}
void maxCurrent21(){
  byte message[] = { 0x3A, 0x30, 0x15, 0x00 };  //21A
  Serial1.write(message, sizeof(message));
}
void maxCurrent22(){
  byte message[] = { 0x3A, 0x30, 0x16, 0x00 };  //22A
  Serial1.write(message, sizeof(message));
}
void maxCurrent23(){
  byte message[] = { 0x3A, 0x30, 0x17, 0x00 };  //23A
  Serial1.write(message, sizeof(message));
}
void maxCurrent24(){
  byte message[] = { 0x3A, 0x30, 0x18, 0x00 };  //24A
  Serial1.write(message, sizeof(message));
}
void maxCurrent25(){
  byte message[] = { 0x3A, 0x30, 0x19, 0x00 };  //25A
  Serial1.write(message, sizeof(message));
}
void maxCurrent26(){
  byte message[] = { 0x3A, 0x30, 0x1A, 0x00 };  //26A
  Serial1.write(message, sizeof(message));
}
void maxCurrent27(){
  byte message[] = { 0x3A, 0x30, 0x1B, 0x00 };  //27A
  Serial1.write(message, sizeof(message));
}
void maxCurrent28(){
  byte message[] = { 0x3A, 0x30, 0x1C, 0x00 };  //28A
  Serial1.write(message, sizeof(message));
}
void maxCurrent29(){
  byte message[] = { 0x3A, 0x30, 0x1D, 0x00 };  //29A
  Serial1.write(message, sizeof(message));
}
void maxCurrent30(){
  byte message[] = { 0x3A, 0x30, 0x1E, 0x00 };  //30A
  Serial1.write(message, sizeof(message));
}
void maxCurrent31(){
  byte message[] = { 0x3A, 0x30, 0x1F, 0x00 };  //31A
  Serial1.write(message, sizeof(message));
}
void maxCurrent32(){
  byte message[] = { 0x3A, 0x31, 0x33, 0x00 };  //32A
  Serial1.write(message, sizeof(message));

}

void HW_CONF(){
  byte message[] = { 0x31, 0x30, 0x30, 0x00 };  //CONF Ventilation feature on
  Serial1.write(message, sizeof(message));
}
void SW_CONF(){
  byte message[] = { 0x31, 0x30, 0x31, 0x00 };  //CONF Ventilation feature on
  Serial1.write(message, sizeof(message));
}
void remote_off(){
  byte message[] = { 0x31, 0x31, 0x30, 0x00 };  //CONF Ventilation feature off
  Serial1.write(message, sizeof(message));
}
void remote_on(){
  byte message[] = { 0x31, 0x31, 0x31, 0x00 };  //CONF Ventilation feature on
  Serial1.write(message, sizeof(message));
}

void read_serial1_available(){
  int i = 0;
  byte inByte[100];
  while (Serial1.available() > 0) {
    byte incomming = Serial1.read();
    Serial.print(incomming, HEX);
    //Serial.print(incomming);
    Serial.print(" ");
    inByte[i] = incomming;
    i++;
  }
  Serial.println();
}

void EV_volt() {
  //delay(100);
  byte message[] = { 0x30, 0x32, 0x36, 0x00 };  //V1
  Serial1.write(message, sizeof(message));
}
void EV_pilot_state(){
  byte message[] = { 0x33, 0x32, 0x31, 0x00 };  //state
  Serial1.write(message, sizeof(message));
}
void EV_current_L1() {
  byte message[] = { 0x3B, 0x32, 0x30, 0x00 };  //C1
  Serial1.write(message, sizeof(message));
}
void EV_current_L2() {
  byte message[] = { 0x3B, 0x32, 0x31, 0x00 };  //C1
  Serial1.write(message, sizeof(message));
}
void EV_current_L3() {
  byte message[] = { 0x3B, 0x32, 0x32, 0x00 };  //C1
  Serial1.write(message, sizeof(message));
}
void EV_temp() {
  byte message[] = { 0x30, 0x32, 0x38, 0x00 };  //temp
  Serial1.write(message, sizeof(message));
}
void EV_power() {
  power = (((volt * current_L1)/1000)+((volt * current_L2)/1000)+((volt * current_L3)/1000));
  if(power==0){D_power="0";}
  else{
    float Ex_Float = power_sum_sec;                           // สร้างตัวแปรชื่อ Ex_Float ชนิด float (ทศนิยม)
    int Ex_Int = int(Ex_Float);                               // สร้างตัวแปรชื่อ Ex_Int ชนิด int (จำนวนเต็ม) จากนั้นแปลง Ex_Float เป็นจำนวนเต็ม ทำให้ทศนิยมถูกตัดออก
    long Ex_Dec = 100 * (Ex_Float - Ex_Int);                  // สร้างตัวแปรชื่อ Ex_Int ชนิด int (จำนวนเต็ม)
    String Ex_Dec_text = String(Ex_Dec);
    if(Ex_Dec_text == "1"){Ex_Dec_text = "01";}
    else if(Ex_Dec_text == "2"){Ex_Dec_text = "02";}
    else if(Ex_Dec_text == "3"){Ex_Dec_text = "03";}
    else if(Ex_Dec_text == "4"){Ex_Dec_text = "04";}
    else if(Ex_Dec_text == "5"){Ex_Dec_text = "05";}
    else if(Ex_Dec_text == "6"){Ex_Dec_text = "06";}
    else if(Ex_Dec_text == "7"){Ex_Dec_text = "07";}
    else if(Ex_Dec_text == "8"){Ex_Dec_text = "08";}
    else if(Ex_Dec_text == "9"){Ex_Dec_text = "09";}
    
    if(Ex_Dec_text == "0"){D_power_sum_sec = String(Ex_Int);}
    else{D_power_sum_sec = String(Ex_Int) + "." + Ex_Dec_text;}// นำสตริงทั้งหมดมาต่อกัน
    
    if(Ex_Dec_text == "0"){D_power_sum_sec = String(Ex_Int);}
    Ex_Float = power;                           // สร้างตัวแปรชื่อ Ex_Float ชนิด float (ทศนิยม)
    Ex_Int = int(Ex_Float);                               // สร้างตัวแปรชื่อ Ex_Int ชนิด int (จำนวนเต็ม) จากนั้นแปลง Ex_Float เป็นจำนวนเต็ม ทำให้ทศนิยมถูกตัดออก
    Ex_Dec = 100 * (Ex_Float - Ex_Int);
    D_power = String(Ex_Int) + "." + String(Ex_Dec);
  }
  // Serial.print("Po: ");
  // Serial.println(power);
}

void EV_time() {
  timer = (millis() - charging_mode_time_stamp_start_point);
  float Ex_Float = (timer/1000);                           // สร้างตัวแปรชื่อ Ex_Float ชนิด float (ทศนิยม)
  int Ex_Int = int(Ex_Float);                            // สร้างตัวแปรชื่อ Ex_Int ชนิด int (จำนวนเต็ม) จากนั้นแปลง Ex_Float เป็นจำนวนเต็ม ทำให้ทศนิยมถูกตัดออก

  int i = Ex_Int - sec_passed;
  int j=0;

  while(j<i){
    power_sum_sec += (power / 3600);
    sec0 += 1;
    j++;
    
    if (sec0 == 10) {
      sec1 += 1;
      sec0 = 0;
    }
    if (sec1 == 6) {
      min2 += 1;
      sec1 = 0;
    }
    if (min2 == 10) {
      min3 += 1;
      min2 = 0;
    }
    if (min3 == 6) {
      hour4 += 1;
      min3 = 0;
    }
    if (hour4 == 10) {
      hour5 += 1;
      hour4 = 0;
    }
  }
  sec_passed = Ex_Int;
  D_time = String(hour5) + String(hour4) + ":" + String(min3) + String(min2) + ":" + String(sec1) + String(sec0);
}

void EV_Display() {
  if(wait_display == 0){

  tft.setSwapBytes(true);
  tft.pushImage(0, 0, 480, 320, main_screen);

  wifistrength();
  lan_on_symbol = 2;
  lan_connection();
  tle_4G_on_symbol = 2;
  tle_4G_connection();
  swipe_card_symbol = 1;

  if((x=="SWIPE_CARD")&&(QRCODE_LINK!="")){
        
  }else{
    in_waiting_card();
  }

  if(x == "STAND_BY"){
    if(SuspendedEV == 0){
      if(reserved_status == 0){
        EV_state = "Available";
        led_standby();
        tft.drawArc(239, 159, 147, 161, 0, 360, TFT_BLUE, TFT_BLACK, true);
      }else{
        EV_state = "Reserved";
        led_reserved();
        tft.drawArc(239, 159, 147, 161, 0, 360, TFT_YELLOW, TFT_BLACK, true);
      }
    }else{
      EV_state = "SuspendedEV";
      led_greenpass();
      tft.drawArc(239, 159, 147, 161, 0, 360, TFT_GREEN, TFT_BLACK, true);
    }
  }
  else if(x == "FAULT_PE"){EV_state = "FAULT PE";}
  else if(x == "SWIPE_CARD"){EV_state = "Preparing";}
  else if(x == "CONNECTED"){EV_state = "Charging";}
  else if(x == "STOP_CHARGING"){
    EV_state = "Finishing";
    led_swipe();
  }
  else if(x == "CHARGING_MODE"){EV_state = "Charging";}
  else if(x == "NO_POWER"){EV_state = "Powerloss";}
  else if(x == "FAULT_E"){EV_state = "FAULT E";}
  else if(x == "LEAKAGE_FAULT"){EV_state = "LEAKAGE FAULT";}
  else if(x == "FINISHED"){EV_state = "Charging";}

  tft.fillRect(182, 150, 116, 19, TFT_BLACK);
  tft.loadFont(AA_FONT_SMALL);
  tft.setTextColor(LimeGreen, BLACK);
  tft.setTextDatum(MC_DATUM);
  tft.drawString(EV_state, 240, 160);
  tft.unloadFont();

  if (volt == 0) { D_volt = "0";}
  tft.loadFont(AA_FONT_SMALL);
  tft.setTextColor(WHITE,  messageBox);
  tft.setTextDatum(MC_DATUM);
  tft.drawString(D_volt, 175, 209);
  tft.unloadFont();

  if (current_L1 == 0) { D_current_L1 = "0"; }
  tft.loadFont(AA_FONT_SMALL);
  tft.setTextColor(WHITE,  messageBox);
  tft.setTextDatum(MC_DATUM);
  tft.drawString(D_current_L1, 240, 209);
  tft.unloadFont();

  tft.loadFont(AA_FONT_SMALL);
  tft.setTextColor(WHITE,  messageBox);
  tft.setTextDatum(MC_DATUM);
  tft.drawString(D_temp, 305, 209);
  tft.unloadFont();

  Power_BIG_fontSize();

  tft.fillRect(204, 265, 75, 19, BLACK);
  tft.loadFont(AA_FONT_SMALL);
  tft.setTextColor(WHITE,  BLACK);
  tft.setTextDatum(MC_DATUM);
  tft.drawString(D_time, 240, 275);
  tft.unloadFont();
  }
}
void Power_BIG_fontSize() {
  
  if((x == "FINISHED")||(x == "STOP_CHARGING")){
    tft.fillRect(180, 85, 120, 34, messageBox);

    tft.setTextColor(LimeGreen, kwhBox);
    tft.loadFont(AA_FONT_SMALL);
    tft.drawString("Total", 186, 129);
    tft.unloadFont();

    tft.setTextColor(WHITE,  messageBox);
    tft.loadFont(AA_FONT_LARGE);
    tft.setTextDatum(MC_DATUM);
    tft.drawString(D_power_sum_sec, 240, 105);
    tft.unloadFont();

    tft.fillRect(222, 120, 35, 11, kwhBox);
    tft.setTextColor(WHITE, kwhBox);
    tft.loadFont(AA_FONT_SMALL);
    tft.drawString("kWh", 240, 128);
    tft.unloadFont();
  }else{
    tft.fillRect(180, 85, 120, 34, messageBox);
    tft.setTextColor(WHITE,  messageBox); 
    tft.loadFont(AA_FONT_LARGE);
    tft.setTextDatum(MC_DATUM);
    tft.drawString(D_power, 240, 105);
    tft.unloadFont();

    tft.fillRect(222, 120, 35, 11, kwhBox);
    tft.setTextColor(TFT_WHITE, kwhBox);
    tft.loadFont(AA_FONT_SMALL);
    tft.drawString("kW", 240, 128);
    tft.unloadFont();
  }
}
void EV_Display_Refresh() {

  if(wait_display == 0){

    if((x=="SWIPE_CARD")&&(QRCODE_LINK!="")){
        
    }else{

      wifistrength();

      lan_connection();

      tle_4G_connection();

      if(x == "STAND_BY"){
        if(SuspendedEV == 0){
          if(reserved_status == 0){
            EV_state = "Available";
            led_standby();
            tft.drawArc(239, 159, 147, 161, 0, 360, TFT_BLUE, TFT_BLACK, true);
          }else{
            EV_state = "Reserved";
            led_reserved();
            tft.drawArc(239, 159, 147, 161, 0, 360, TFT_YELLOW, TFT_BLACK, true);
          }
        }else{
          EV_state = "SuspendedEV";
          led_greenpass();
          tft.drawArc(239, 159, 147, 161, 0, 360, TFT_GREEN, TFT_BLACK, true);
        }
      }
      else if(x == "FAULT_PE"){EV_state = "FAULT PE";}
      else if(x == "SWIPE_CARD"){EV_state = "Preparing";}
      else if(x == "CONNECTED"){EV_state = "Charging";}
      else if(x == "STOP_CHARGING"){
        EV_state = "Finishing";
        led_swipe();
      }
      else if(x == "CHARGING_MODE"){EV_state = "Charging";}
      else if(x == "NO_POWER"){EV_state = "Powerloss";}
      else if(x == "FAULT_E"){EV_state = "FAULT E";}
      else if(x == "LEAKAGE_FAULT"){EV_state = "LEAKAGE FAULT";}
      else if(x == "FINISHED"){EV_state = "Charging";}

      if ((x != x_pass)||(EV_state != EV_state_pass)) {
        tft.fillRect(178, 150, 124, 19, TFT_BLACK);
        tft.loadFont(AA_FONT_SMALL);
        tft.setTextColor(LimeGreen, BLACK);
        tft.setTextDatum(MC_DATUM);
        tft.drawString(EV_state, 240, 160);
        tft.unloadFont();
      }

      //count_refresh += 1;
      //if(count_refresh>2){
        if (volt != volt_pass) {
          if (volt == 0) { D_volt = "0"; }
          tft.fillRect(151, 198, 46, 19, messageBox);
          tft.loadFont(AA_FONT_SMALL);
          tft.setTextColor(WHITE,  messageBox);
          tft.setTextDatum(MC_DATUM);
          tft.drawString(D_volt, 175, 209);
          tft.unloadFont();
        }


        if (current_L1 != current_L1_pass) {
          if (current_L1 == 0) { D_current_L1 = "0"; }
          tft.fillRect(217, 198, 46, 19, messageBox);
          tft.loadFont(AA_FONT_SMALL);
          tft.setTextColor(WHITE,  messageBox);
          tft.setTextDatum(MC_DATUM);
          tft.drawString(D_current_L1, 240, 209);
          tft.unloadFont();
        }


        if (temp != temp_pass) {
          if (temp == 0) { D_temp = "0"; }
          tft.fillRect(282, 198, 46, 19, messageBox);
          tft.loadFont(AA_FONT_SMALL);
          tft.setTextColor(WHITE,  messageBox);
          tft.setTextDatum(MC_DATUM);
          tft.drawString(D_temp, 305, 209);
          tft.unloadFont();
        }

        if (power != power_pass) {
          if (power == 0) { D_power = "0"; }
          Power_BIG_fontSize();
        }

        //count_refresh = 0;
      //}

      if(D_time_pass!=D_time){
        if((x == "CHARGING_MODE")||(x == "CONNECTED")||(x == "FINISHED")||(x== "STOP_CHARGING")){    
          tft.fillRect(204, 265, 75, 19, BLACK);
          tft.loadFont(AA_FONT_SMALL);
          tft.setTextColor(WHITE,  BLACK);
          tft.setTextDatum(MC_DATUM);
          tft.drawString(D_time, 240, 275);
          tft.unloadFont();
        }
        if(x=="SWIPE_CARD"){
          if(set_stop_charging_state==1){
            tft.fillRect(204, 265, 75, 19, BLACK);
            tft.loadFont(AA_FONT_SMALL);
            tft.setTextColor(WHITE,  BLACK);
            tft.setTextDatum(MC_DATUM);
            tft.drawString(D_time, 240, 275);
            tft.unloadFont();
          }
        }
      }
      

      D_x_pass = x;
      x_pass = x;
      EV_state_pass = EV_state;
      volt_pass = volt;
      current_L1_pass = current_L1;
      temp = temp_pass;
      power_pass = power;
      D_time_pass = D_time;

    }
  }
}


void EV_Leakage_Fault() {
  tft.setFreeFont(FSI19);
  tft.fillRect(0, 100, 480, 130, RED);
  tft.setTextColor(TFT_WHITE, RED);
  tft.setTextSize(1);
  tft.setTextDatum(MC_DATUM);
  tft.drawString("Leakage Fault", 240, 160, GFXFF);
}
void EV_Fault_E() {
  tft.setFreeFont(FSI24);
  tft.fillRect(0, 100, 480, 130, RED);
  tft.setTextColor(TFT_WHITE, RED);
  tft.setTextSize(1);
  tft.setTextDatum(MC_DATUM);
  tft.drawString("Fault E", 240, 160, GFXFF);
}
void EV_rfid_require_card() {
  if(QRCODE_LINK==""){
    EV_Display_Refresh();
    tft.drawArc(239, 159, 147, 161, 0, 360, CYAN, TFT_BLACK, true);
    tft.setFreeFont(FSI24);
    tft.fillRect(0, 100, 480, 130, CYAN);
    tft.setTextColor(TFT_BLACK, CYAN);
    tft.setTextSize(1);
    tft.setTextDatum(MC_DATUM);
    tft.drawString("RFID CARD", 240, 160, GFXFF);
  }else{
    // ล้างหน้าจอ
    tft.fillScreen(TFT_BLACK);

    // วาดพื้นหลังสีฟ้าสำหรับข้อความ
    int rectHeight = 110;  // ความสูงของพื้นที่สำหรับข้อความ
    tft.fillRect(0, 0, tft.width(), rectHeight, TFT_CYAN);
    
    // แสดงข้อความบนพื้นหลังสีฟ้า
    tft.setFreeFont(FSI12);
    tft.setTextColor(TFT_BLACK, TFT_CYAN);  // ตั้งค่าสีข้อความ
    tft.setTextSize(1);  // ตั้งค่าขนาดตัวอักษร
    tft.setCursor(187, 45);  // ตั้งค่าตำแหน่งเริ่มต้นของข้อความ
    tft.println("Tap CARD");
    tft.setCursor(187, 85);
    tft.println("or scan QR");

    // สร้าง QR code
    QRCode qrcode;
    uint8_t qrcodeData[qrcode_getBufferSize(6)];
    qrcode_initText(&qrcode, qrcodeData, 6, ECC_LOW, QRCODE_LINK.c_str());

    // ขนาดของบล็อก QR code
    int blockSize = 4;  // คุณสามารถปรับขนาดบล็อกนี้ได้

    // คำนวณขนาดและตำแหน่งของ QR code เพื่อให้อยู่ตรงกลาง
    int qrSize = qrcode.size * blockSize;
    int xOffset = (tft.width() - qrSize) / 2;
    int yOffset = ((tft.height() - qrSize) / 2) + 35;

    // วาด QR code บนหน้าจอ
    for (uint8_t y = 0; y < qrcode.size; y++) {
      for (uint8_t x = 0; x < qrcode.size; x++) {
        if (qrcode_getModule(&qrcode, x, y)) {
          tft.fillRect(xOffset + x * blockSize, yOffset + y * blockSize, blockSize, blockSize, TFT_BLACK);
        } else {
          tft.fillRect(xOffset + x * blockSize, yOffset + y * blockSize, blockSize, blockSize, TFT_WHITE);
        }
      }
    }

    // วาดกรอบรอบ QR code
    int borderWidth = 2;  // ความหนาของกรอบ
    tft.drawRect(xOffset - borderWidth, yOffset - borderWidth, qrSize + 2 * borderWidth, qrSize + 2 * borderWidth, TFT_BLUE);




    // EV_Display();
    // tft.drawArc(239, 159, 147, 161, 0, 360, CYAN, TFT_BLACK, true);
    // // สร้าง QR code
    // QRCode qrcode;
    // uint8_t qrcodeData[qrcode_getBufferSize(6)];
    // qrcode_initText(&qrcode, qrcodeData, 6, ECC_LOW, QRCODE_LINK.c_str());

    // // คำนวณตำแหน่งของ QR code เพื่อให้อยู่ตรงกลาง
    // int qrSize = qrcode.size * 5;  // ปรับขนาด QR code
    // int xOffset = (tft.width() - qrSize) / 2;
    // int yOffset = (tft.height() - qrSize) / 2;

    // // วาด QR code บนหน้าจอ
    // for (uint8_t y = 0; y < qrcode.size; y++) {
    //   for (uint8_t x = 0; x < qrcode.size; x++) {
    //     if (qrcode_getModule(&qrcode, x, y)) {
    //       tft.fillRect(xOffset + x * 5, yOffset + y * 5, 5, 5, TFT_BLUE);
    //     } else {
    //       tft.fillRect(xOffset + x * 5, yOffset + y * 5, 5, 5, TFT_WHITE);
    //     }
    //   }
    // }

  }
}

void EV_rfid_require_card_wait() {
  tft.drawArc(239, 159, 147, 161, 0, 360, TFT_WHITE, TFT_BLACK, true);
  tft.setFreeFont(FSI24);
  wait_display = 1;
  tft.fillRect(0, 100, 480, 130, TFT_WHITE);
  tft.setTextColor(TFT_BLACK, TFT_WHITE);
  tft.setTextSize(1);
  tft.setTextDatum(MC_DATUM);
  tft.drawString("PLEASE WAIT", 240, 160, GFXFF);
}
void EV_rfid_require_card_access() {
  led_greenpass();
  tft.drawArc(239, 159, 147, 161, 0, 360, TFT_GREEN, TFT_BLACK, true);
  tft.setFreeFont(FSI24);
  wait_display = 0;
  tft.fillRect(0, 100, 480, 130, TFT_GREEN);
  tft.setTextColor(TFT_WHITE, TFT_GREEN);
  tft.setTextSize(1);
  tft.setTextDatum(MC_DATUM);
  tft.drawString("ACCEPTED", 240, 160, GFXFF);
  delay(450);
  tft.fillRect(0, 100, 480, 130, TFT_GREEN);
  delay(100);
  tft.drawString("ACCEPTED", 240, 160, GFXFF);
  delay(450);
  tft.fillRect(0, 100, 480, 130, TFT_GREEN);
  delay(100);
  tft.drawString("ACCEPTED", 240, 160, GFXFF);
  delay(450);
}

void EV_rfid_require_card_wrong(){
  led_error();
  tft.drawArc(239, 159, 147, 161, 0, 360, RED, TFT_BLACK, true);
  tft.setFreeFont(FSI24);
  wait_display = 0;
  tft.fillRect(0, 100, 480, 130, TFT_RED);
  tft.setTextColor(TFT_WHITE, TFT_RED);
  tft.setTextSize(1);
  tft.setTextDatum(MC_DATUM);
  tft.drawString("INVALID", 240, 160, GFXFF);
  tft.fillRect(0, 100, 480, 130, TFT_RED);
  delay(100);
  tft.drawString("INVALID", 240, 160, GFXFF);
  delay(400);
  tft.fillRect(0, 100, 480, 130, TFT_RED);
  delay(100);
  tft.drawString("INVALID", 240, 160, GFXFF);
  delay(400);
}

void EV_rfid_disconnect_with_server(){
  led_error();
  tft.drawArc(239, 159, 147, 161, 0, 360, RED, TFT_BLACK, true);
  tft.setFreeFont(FSI24);
  wait_display = 0;
  tft.fillRect(0, 100, 480, 130, TFT_RED);
  tft.setTextColor(TFT_WHITE, TFT_RED);
  tft.setTextSize(1);
  tft.setTextDatum(MC_DATUM);
  tft.drawString("Connect fail", 240, 160, GFXFF);
  tft.fillRect(0, 100, 480, 130, TFT_RED);
  delay(100);
  tft.drawString("Connect fail", 240, 160, GFXFF);
  delay(400);
  tft.fillRect(0, 100, 480, 130, TFT_RED);
  delay(100);
  tft.drawString("Connect fail", 240, 160, GFXFF);
  delay(400);
}

void EV_rfid_require_card_reservation(){
  led_reserved();
  tft.drawArc(239, 159, 147, 161, 0, 360, TFT_YELLOW, TFT_BLACK, true);
  tft.setFreeFont(FSI24);
  wait_display = 0;
  tft.fillRect(0, 100, 480, 130, TFT_YELLOW);
  tft.setTextColor(TFT_BLACK, TFT_YELLOW);
  tft.setTextSize(1);
  tft.setTextDatum(MC_DATUM);
  tft.drawString("blocked", 240, 135, GFXFF);
  tft.drawString("by reservation", 240, 185, GFXFF);
  delay(1500);
}

void EV_emergency_warning_display(){
  tft.setFreeFont(FSI24);
  tft.fillRect(0, 100, 480, 130, RED);
  tft.setTextColor(TFT_WHITE, RED);
  tft.setTextSize(1);
  tft.setTextDatum(MC_DATUM);
  tft.drawString("EMERGENCY", 240, 160, GFXFF);
}
void EV_temp_alarm(){
  tft.drawArc(239, 159, 147, 161, 0, 360, TFT_ORANGE, TFT_BLACK, true);
  tft.setFreeFont(FSI12);
  tft.fillRect(0, 100, 480, 130, TFT_ORANGE);
  tft.setTextColor(TFT_BLACK, TFT_ORANGE);
  tft.setTextSize(1);
  tft.setTextDatum(MC_DATUM);
  tft.drawString("WARNING", 240, 140, GFXFF);
  tft.drawString("HIGH TEMPERATURE", 240, 180, GFXFF);
}
void EV_reserved(){
  tft.setFreeFont(FSI24);
  tft.fillRect(0, 100, 480, 130, TFT_YELLOW);
  tft.setTextColor(TFT_BLACK, TFT_YELLOW);
  tft.setTextSize(1);
  tft.setTextDatum(MC_DATUM);
  tft.drawString("RESERVED", 240, 160, GFXFF);
}
void EV_update_display(){
  tft.setFreeFont(FSI19);
  tft.fillRect(0, 100, 480, 130, CYAN);
  tft.setTextColor(TFT_BLACK, CYAN);
  tft.setTextSize(1);
  tft.setTextDatum(MC_DATUM);
  tft.drawString("UPDATE DISPLAY", 240, 160, GFXFF);
}
void Loading_Display(){
  tft.setSwapBytes(true);
  tft.pushImage(0, 0, 480, 320, BEi_logo);
  tft.loadFont(AA_FONT_SMALL);
    tft.setTextColor(TFT_BLACK, TFT_WHITE);
    tft.setTextDatum(MC_DATUM);
    tft.drawString("UPLOADING DATA...", 240, 240);
    tft.unloadFont();
}
void Setup_Display(){
  tft.setSwapBytes(true);
  tft.pushImage(0, 0, 480, 320, BEi_logo);
  tft.loadFont(AA_FONT_SMALL);
    tft.setTextColor(TFT_BLACK, TFT_WHITE);
    tft.setTextDatum(MC_DATUM);
    tft.drawString("Please setup connection", 240, 240);
    tft.unloadFont();
}
///////////////////////////////////////////////////////////////////////////////////
//led
int led_network_i = 20;

void led_network() {
  if (led_network_i < 100) {
    led_network_i += 8;
    pixels.fill(pixels.Color(led_network_i,  0,led_network_i/2, pixels.gamma8(led_network_i)));
    pixels.show();  // Send the updated pixel colors to the hardware.
  } else {
    led_network_i = 20;
  } 
}

void led_standby() {
  pixels.fill(pixels.Color(0, 0, 255, pixels.gamma8(255)));
  pixels.show();
}

void led_reserved() {
  pixels.fill(pixels.Color(255, 255, 0, pixels.gamma8(255)));
  pixels.show();
}

void led_swipe() {
  pixels.fill(pixels.Color(0, 255, 255, pixels.gamma8(255)));
  pixels.show();
}

void led_connect() {
  pixels.fill(pixels.Color(0, 255, 255, pixels.gamma8(255)));
  pixels.show();
}

void led_greenpass() {
  pixels.fill(pixels.Color(0, 255, 0, pixels.gamma8(255)));
  pixels.show();
}

// void led_charge() {
//   if(millis()-led_charge_delay > 70){

//     if(i_led < NUMPIXELS) { 
//       pixels.setPixelColor(i_led, pixels.Color(0, 255, 0));
//       pixels.setPixelColor(i_led+1, pixels.Color(0, 75, 0));
//       pixels.setPixelColor(i_led+2, pixels.Color(0, 30, 0));
//       pixels.setPixelColor(i_led+3, pixels.Color(0, 20, 0));
//       pixels.setPixelColor(i_led+4, pixels.Color(0, 10, 0));
//       pixels.setPixelColor(i_led-1, pixels.Color(0, 75, 0));
//       pixels.setPixelColor(i_led-2, pixels.Color(0, 30, 0));
//       pixels.setPixelColor(i_led-3, pixels.Color(0, 20, 0));
//       pixels.setPixelColor(i_led-4, pixels.Color(0, 10, 0));
//       pixels.setPixelColor(i_led-5, pixels.Color(0, 0, 0));
      
//       i_led++;
//     }else{
//       i_led = 0;
//     }
//     pixels.show();
//     led_charge_delay = millis();
//   }
  
// }
void led_charge() {

  if(millis()-led_charge_delay > 10){

    strip.setPixelColor(7, strip.Color(0, i_led_pixel_0, 0)); //  pixel 0
    i_led_pixel_0 += 16;
    if(i_led_pixel_0>255){
      i_led_pixel_0 = 255;

      strip.setPixelColor(6, strip.Color(0, i_led_pixel_1, 0)); //  pixel 1
      i_led_pixel_1 += 16;
      if(i_led_pixel_1>255){
        i_led_pixel_1 = 255;

        strip.setPixelColor(5, strip.Color(0, i_led_pixel_2, 0)); //  pixel 2
        i_led_pixel_2 += 16;
        if(i_led_pixel_2>255){
          i_led_pixel_2 = 255;
          
          strip.setPixelColor(4, strip.Color(0, i_led_pixel_3, 0)); //  pixel 3
          i_led_pixel_3 += 16;
          if(i_led_pixel_3>255){
            i_led_pixel_3 = 255;
            
            strip.setPixelColor(3, strip.Color(0, i_led_pixel_4, 0)); //  pixel 4
            i_led_pixel_4 += 16;
            if(i_led_pixel_4>255){
              i_led_pixel_4 = 255;
            
              strip.setPixelColor(2, strip.Color(0, i_led_pixel_5, 0)); //  pixel 5
              i_led_pixel_5 += 16;
              if(i_led_pixel_5>255){
                i_led_pixel_5 = 255;
              
                strip.setPixelColor(1, strip.Color(0, i_led_pixel_6, 0)); //  pixel 6
                i_led_pixel_6 += 16;
                if(i_led_pixel_6>255){
                  i_led_pixel_6 = 255;
                  
                  strip.setPixelColor(0, strip.Color(0, i_led_pixel_7, 0)); //  pixel 7
                  i_led_pixel_7 += 16;
                  if(i_led_pixel_7>255){
                    i_led_pixel_0 = 0;
                    i_led_pixel_1 = 0;
                    i_led_pixel_2 = 0;
                    i_led_pixel_3 = 0;
                    i_led_pixel_4 = 0;
                    i_led_pixel_5 = 0;
                    i_led_pixel_6 = 0;
                    i_led_pixel_7 = 0;

                    strip.setPixelColor(0, strip.Color(0, i_led_pixel_0, 0)); //  pixel 0
                    strip.setPixelColor(1, strip.Color(0, i_led_pixel_1, 0)); //  pixel 1
                    strip.setPixelColor(2, strip.Color(0, i_led_pixel_2, 0)); //  pixel 2
                    strip.setPixelColor(3, strip.Color(0, i_led_pixel_3, 0)); //  pixel 3
                    strip.setPixelColor(4, strip.Color(0, i_led_pixel_4, 0)); //  pixel 4
                    strip.setPixelColor(5, strip.Color(0, i_led_pixel_5, 0)); //  pixel 5
                    strip.setPixelColor(6, strip.Color(0, i_led_pixel_6, 0)); //  pixel 6
                    strip.setPixelColor(7, strip.Color(0, i_led_pixel_7, 0)); //  pixel 7
                  }
                }
              }
            }
          }
        }
      }
    }
    strip.show(); 
    led_charge_delay = millis();
  }
}

void rainbow(){
  if(firstPixelHue < 3*65536) {
    for(int i=0; i<strip.numPixels(); i++) { // For each pixel in strip...
      // Offset pixel hue by an amount to make one full revolution of the
      // color wheel (range of 65536) along the length of the strip
      // (strip.numPixels() steps):
      int pixelHue = firstPixelHue + (i * 65536L / strip.numPixels());
      // strip.ColorHSV() can take 1 or 3 arguments: a hue (0 to 65535) or
      // optionally add saturation and value (brightness) (each 0 to 255).
      // Here we're using just the single-argument hue variant. The result
      // is passed through strip.gamma32() to provide 'truer' colors
      // before assigning to each pixel:
      strip.setPixelColor(i, strip.gamma32(strip.ColorHSV(pixelHue)));
    }
    firstPixelHue += 256;
  }
  else{
    firstPixelHue = 0;
  }
  strip.show(); // Update strip with new contents
}

void led_charge_full() {
  pixels.fill(pixels.Color(0, 255, 0, pixels.gamma8(255)));
  pixels.show();
}

void led_finished() {
  pixels.fill(pixels.Color(204, 0, 204, pixels.gamma8(255)));
  pixels.show();
}

void led_error() {
  pixels.fill(pixels.Color(255, 0, 0, pixels.gamma8(255)));
  pixels.show();
}

////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
//wifistr
void wifistrength(){
  tft.drawCircle(181, 48, 17, TFT_BLACK);
  tft.drawCircle(181, 48, 16, symbol_circle);
    if (wifistr == 0){
      // tft.fillRect(160, 41, 3, 5, TFT_GREY);
      // tft.fillRect(165, 39, 3, 7, TFT_GREY);
      // tft.fillRect(170, 37, 3, 9, TFT_GREY);
      // tft.fillRect(175, 35, 3, 11, TFT_GREY);
      tft.fillCircle(181, 57, 3, TFT_GREY);
      tft.drawCircle(181, 57, 3, BLACK);
      tft.drawArc(181, 58, 9, 6, 145, 215, TFT_GREY, TFT_BLACK, true);
      tft.drawArc(181, 58, 15, 12, 140, 220, TFT_GREY, TFT_BLACK, true);
      tft.drawArc(181, 58, 21, 18, 137, 223, TFT_GREY, TFT_BLACK, true);
    }
    else if (wifistr <= 30)
    {
      //tft.fillRect(295, 10, 20, 10, BG1);
      // tft.fillRect(160, 41, 3, 5, TFT_RED);
      // tft.fillRect(165, 39, 3, 7, TFT_GREY);
      // tft.fillRect(170, 37, 3, 9, TFT_GREY);
      // tft.fillRect(175, 35, 3, 11, TFT_GREY);
      tft.fillCircle(181, 57, 3, TFT_RED);
      tft.drawCircle(181, 57, 3, BLACK);
      tft.drawArc(181, 58, 9, 6, 145, 215, TFT_GREY, TFT_BLACK, true);
      tft.drawArc(181, 58, 15, 12, 140, 220, TFT_GREY, TFT_BLACK, true);
      tft.drawArc(181, 58, 21, 18, 137, 223, TFT_GREY, TFT_BLACK, true);
    }
    else if (wifistr > 30 && wifistr <=50)
    {
      //tft.fillRect(295, 10, 20, 10, BG1);
      // tft.fillRect(160, 41, 3, 5, TFT_GREEN);
      // tft.fillRect(165, 39, 3, 7, TFT_GREEN);
      // tft.fillRect(170, 37, 3, 9, TFT_GREY);
      // tft.fillRect(175, 35, 3, 11, TFT_GREY);
      tft.fillCircle(181, 57, 3, TFT_RED);
      tft.drawCircle(181, 57, 3, BLACK);
      tft.drawArc(181, 58, 9, 6, 145, 215, TFT_RED, TFT_BLACK, true);
      tft.drawArc(181, 58, 15, 12, 140, 220, TFT_GREY, TFT_BLACK, true);
      tft.drawArc(181, 58, 21, 18, 137, 223, TFT_GREY, TFT_BLACK, true);
    }
    else if (wifistr > 50 && wifistr <=70)
    {
      //tft.fillRect(295, 10, 20, 10, BG1);
      // tft.fillRect(160, 41, 3, 5, TFT_GREEN);
      // tft.fillRect(165, 39, 3, 7, TFT_GREEN);
      // tft.fillRect(170, 37, 3, 9, TFT_GREEN);
      // tft.fillRect(175, 35, 3, 11, TFT_GREY);
      tft.fillCircle(181, 57, 3, TFT_YELLOW);
      tft.drawCircle(181, 57, 3, BLACK);
      tft.drawArc(181, 58, 9, 6, 145, 215, TFT_YELLOW, TFT_BLACK, true);
      tft.drawArc(181, 58, 15, 12, 140, 220, TFT_YELLOW, TFT_BLACK, true);
      tft.drawArc(181, 58, 21, 18, 137, 223, TFT_GREY, TFT_BLACK, true);
    }
    else{
      // tft.fillRect(160, 41, 3, 5, TFT_GREEN);
      // tft.fillRect(165, 39, 3, 7, TFT_GREEN);
      // tft.fillRect(170, 37, 3, 9, TFT_GREEN);
      // tft.fillRect(175, 35, 3, 11, TFT_GREEN);
      tft.fillCircle(181, 57, 3, TFT_GREEN);
      tft.drawCircle(181, 57, 3, BLACK);
      tft.drawArc(181, 58, 9, 6, 145, 215, TFT_GREEN, TFT_BLACK, true);
      tft.drawArc(181, 58, 15, 12, 140, 220, TFT_GREEN, TFT_BLACK, true);
      tft.drawArc(181, 58, 21, 18, 137, 223, TFT_GREEN, TFT_BLACK, true);
    }
}


////////////////////////////////////////////////////////////////////
void tle_4G_connection(){
  tft.drawCircle(219, 38, 17, TFT_BLACK);
  tft.drawCircle(219, 38, 16, symbol_circle);
  if(tle_4G_connected==0){
    if(tle_4G_on_symbol!=1){
      //tft.fillRect(210, 28, 22, 21, TFT_RED); //พิกัด (x,y,กว้าง,สูง)

      //tft.fillRect(182, 150, 116, 19, messageBox);
      tft.loadFont(AA_FONT_SMALL);
      tft.setTextColor(TFT_GREY, messageBox);
      tft.setTextDatum(MC_DATUM);
      tft.drawString("4G", 217, 42);
      tft.unloadFont();

      tft.drawArc(224, 36, 3, 4, 160, 230, TFT_GREY, TFT_BLACK, true);
      tft.drawArc(224, 36, 6, 7, 160, 235, TFT_GREY, TFT_BLACK, true);
      

      tle_4G_on_symbol = 1;
    }
  }
  else{
    if(tle_4G_on_symbol!=0){
      //tft.fillRect(210, 28, 22, 21, TFT_RED); //พิกัด (x,y,กว้าง,สูง)

      //tft.fillRect(182, 150, 116, 19, messageBox);
      tft.loadFont(AA_FONT_SMALL);
      tft.setTextColor(TFT_GREEN, messageBox);
      tft.setTextDatum(MC_DATUM);
      tft.drawString("4G", 217, 42);
      tft.unloadFont();

      tft.drawArc(224, 36, 3, 4, 160, 230, TFT_GREEN, TFT_BLACK, true);
      tft.drawArc(224, 36, 6, 7, 160, 235, TFT_GREEN, TFT_BLACK, true);
      

      tle_4G_on_symbol = 0;
    }
  }
}

////////////////////////////////////////////////////////////////////
void lan_connection(){
  tft.drawCircle(258, 38, 17, TFT_BLACK);
  tft.drawCircle(258, 38, 16, symbol_circle);
  if(lan_connected==0){
    if(lan_on_symbol!=1){
      tft.fillRect(247, 28, 22, 21, TFT_BLACK); //พิกัด (x,y,กว้าง,สูง)
      tft.fillRect(248, 29, 20, 19, TFT_GREY);
      tft.fillRect(250, 32, 16, 10, TFT_BLACK);
      tft.fillRect(253, 42, 10, 2, TFT_BLACK);
      tft.fillRect(256, 44, 4, 2, TFT_BLACK);

      tft.fillRect(252, 32, 1, 3, TFT_GREY);
      tft.fillRect(256, 32, 1, 3, TFT_GREY);
      tft.fillRect(259, 32, 1, 3, TFT_GREY);
      tft.fillRect(262, 32, 1, 3, TFT_GREY);

      lan_on_symbol = 1;
    }
  }
  else{
    if(lan_on_symbol!=0){
      tft.fillRect(247, 28, 22, 21, TFT_BLACK); //พิกัด (x,y,กว้าง,สูง)
      tft.fillRect(248, 29, 20, 19, TFT_GREEN);
      tft.fillRect(250, 32, 16, 10, TFT_BLACK);
      tft.fillRect(253, 42, 10, 2, TFT_BLACK);
      tft.fillRect(256, 44, 4, 2, TFT_BLACK);

      tft.fillRect(253, 32, 1, 3, TFT_GREEN);
      tft.fillRect(256, 32, 1, 3, TFT_GREEN);
      tft.fillRect(259, 32, 1, 3, TFT_GREEN);
      tft.fillRect(262, 32, 1, 3, TFT_GREEN);

      lan_on_symbol = 0;
    }
  }
}

////////////////////////////////////////////////////////////////////
void in_waiting_card(){
  if(wait_display == 0){
  tft.drawCircle(298, 49, 17, TFT_BLACK);
  tft.drawCircle(298, 49, 16, symbol_circle);
  
  if(swipe_card_symbol == 1){
    tft.drawCircle(298, 49, 16, symbol_circle);
    // tft.fillRect(283, 38, 22, 20, TFT_BLACK); //พิกัด (x,y,กว้าง,สูง)
    // tft.fillRect(284, 39, 20, 18, TFT_GREY);

    // //R
    // tft.drawFastVLine(285, 40, 16, TFT_BLACK);
    // tft.drawTriangle(285, 40, 285, 48, 288, 42, TFT_BLACK);
    // tft.drawLine(285, 48, 288, 54, TFT_BLACK);
    // //F
    // tft.drawFastVLine(290, 40, 16, TFT_BLACK);
    // tft.drawLine(290, 40, 293, 40, TFT_BLACK); //top line
    // tft.drawLine(290, 47, 293, 47, TFT_BLACK); //mid line
    // //I
    // tft.drawFastVLine(296, 40, 16, TFT_BLACK);
    // tft.drawLine(295, 40, 297, 40, TFT_BLACK); //top line
    // tft.drawLine(295, 55, 297, 55, TFT_BLACK);  //botton line
    // //D
    // tft.drawFastVLine(299, 40, 16, TFT_BLACK);
    // tft.drawTriangle(299, 40, 299, 55, 302, 48, TFT_BLACK);
    swipe_card_symbol = 0;
  }
  }

}
void swipe_waiting_card(){
  if(wait_display == 0){
    if((x=="SWIPE_CARD")&&(QRCODE_LINK!="")){
        
    }else{
      
      tft.drawCircle(298, 49, 17, TFT_BLACK);

      if(swipe_card_symbol == 0){
        tft.drawCircle(298, 49, 16, TFT_GREEN);
        swipe_card_symbol = 1;
      }

    }
  
      
  }
}


void command_from_serial(){
  if (Serial.available() > 0){

    // command to EV CHARGER
    key = Serial.readStringUntil('\n');
    Serial.println(key);
  }
}
 #include <WiFi.h>

#define RXD2 16
#define TXD2 17

// Replace with your network credentials
const char* ssid     = "frankESP";
const char* password = "12345678";

// Set web server port number to 80
WiFiServer server(80);
IPAddress local_IP(192, 168, 137, 3); 
IPAddress gateway(192, 168, 1, 1);
IPAddress subnet(255, 255, 0, 0);
bool Tank = false;

void setup() {
  Serial.begin(115200);
  Serial2.begin(9600, SERIAL_8N1, RXD2, TXD2);
  
  Serial.print("Setting AP (Access Point)…");
  // Remove the password parameter, if you want the AP (Access Point) to be open
  WiFi.softAP(ssid, password);
  WiFi.softAPConfig(local_IP, gateway, subnet);
  server.begin();
}

void loop(){  
    WiFiClient client = server.available();   // Listen for incoming clients
    if (client.available()) {
        String req = client.readStringUntil('\r');

        client.println("HTTP/1.1 200 OK");
        client.println("Content-type:text/html");
        client.println("Connection: close");
        client.println();
        client.println(req.substring(req.indexOf("/")+1,req.indexOf("HTTP"))); 
        
        if((Tank && (req.indexOf("_") != -1)) || (not(Tank) && (req.indexOf("=") != -1))){Serial2.write(0xff); Tank = not(Tank); client.println("Tank =" + String(Tank));}

        if (req.indexOf("STOP") != -1){
          if(Tank){Serial2.write(0x77);}
          else {Serial2.write(byte(0));}
        }
        else if (Tank && (req.indexOf("=") != -1)){
          int bg = req.indexOf("=");
          int mm = req.indexOf("x");
          int ed = req.indexOf("HTTP");
          unsigned int LL = req.substring(bg+1,mm).toInt();
          unsigned int RR = req.substring(mm+1,ed).toInt();
          RR = LL << 4 | RR;
          client.println(String(RR,HEX));
          Serial.println(RR);
          Serial2.write(byte(RR));
        }

//       MOVE
        else if(req.indexOf("status") != -1){client.println("ACK ACK");}
        else if (req.indexOf("FWD_F") != -1){ Serial2.write(byte(1));}
        else if (req.indexOf("REV_F") != -1){ Serial2.write(byte(2));}
        else if (req.indexOf("FWD_S") != -1){ Serial2.write(byte(3));}
        else if (req.indexOf("REV_S") != -1){ Serial2.write(byte(4));}
        
        //Turning
        else if (req.indexOf("FWD_L") != -1){ Serial2.write(byte(5));}
        
        else if (req.indexOf("FWD_R") != -1){ Serial2.write(byte(6));}
        
        else if (req.indexOf("REV_L") != -1){ Serial2.write(byte(7));}
        
        else if (req.indexOf("REV_R") != -1){ Serial2.write(byte(8));}
        
        else if (req.indexOf("STP_L") != -1){ Serial2.write(byte(9));}
        else if (req.indexOf("STP_R") != -1){ Serial2.write(byte(10));}

        else if (req.indexOf("L_90") != -1){ Serial2.write(byte(11));}
        else if (req.indexOf("R_90") != -1){ Serial2.write(byte(12));}
        else if (req.indexOf("U_T") != -1){ Serial2.write(byte(13));}
      
        //Mode
        else if (req.indexOf("MAN_M") != -1){ Serial2.write(byte(14));}
        else if (req.indexOf("AI_M") != -1){ Serial2.write(byte(15));}
        //End/Start
        else if (req.indexOf("STR_CHA") != -1){ Serial2.write(byte(16));}
        else if (req.indexOf("END_CHA") != -1){ Serial2.write(byte(17));}
        else if (req.indexOf("MUTE_CHA") != -1){ Serial2.write(byte(18));}

        Serial.println(req);
//        client.flush();
//        client.stop();
  }
}

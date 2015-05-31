/*################################################################################################
Wireless Technologies Final Project - University of Detroit Mercy
Metered Wireless Charging Server using Arduino

Author: Jonathon Kreska
Version Date: March 25, 2015
Release Version: 1.3

Changelog:
 1.0: Initial Release
 1.1: Fixed Button Double Click. You need GET in front of command. IndexOf was detecting referrer
 1.2: Removed Process Request Function. Now implemented in main loop
 1.3: Boolean good_request is no longer global since it was re-initialized each loop anyways
      Fixed formatting and added more comments. Reduced "client" to "cl"

Abstract:
 This program functions as a webserver that hosts and sends a command webpage to cls that
 connect via the IP Address. The Arduino Board is connected to a relay that closes a relay switch.
 This Switch allows power to be transferred through a USB cable that is connected to a phone 
 charger and a Wireless Charging Pad.
 
Usage: 
 This program is to be uploaded to an Arduino board with an Ethernet Shield. 
 Relay Control terminals are connected to the designated pin and Ground.
 Access the webpage by entering the IP address into your browser's Address Bar

References: 
 Code is started from W.A. Smith's Arduino Web Server LED Control Tutorial released on:
 11 January 2013 which is based on the built-in Arduino Example: Ethernet -> WebServer
 As of December 12, 2014, I updated it for my Computer Networking Project for a SmartHome
 webserver. 
 Now I am using its structure for this project.
 
################################################################################################*/

#include <SPI.h>
#include <Ethernet.h>

byte mac[] = {0xDE,0xAD,0xBE,0xEF,0xFE,0xED}; // Setup MAC Address
IPAddress ip(192,168,10,15);                  // Static IP address
EthernetServer server(80);                    // Create a server on port 80
String HTTP_req = "";                         // Stores the HTTP request that will contain commands

//Create status variables and initialize them on the default state
boolean relay_on = false;       //Needs to be global to maintain state

//Assign names to pins for comprehension 
const int relay =  2;           // Define pin for the relay

void setup(){
  //Setup Digital Pin Modes
  pinMode(relay, OUTPUT);       // Relay is a Digital Out
  
  Ethernet.begin(mac, ip);      // Initialize Ethernet device
  server.begin();               // Start to listen for clients
  //Serial.begin(9600);         // Uncomment for Debugging
}

//##################################### MAIN START ###############################################
void loop(){
  EthernetClient cl = server.available();  // Try to get client

  if (cl) {                                // Once a client is present...
    boolean currentLineIsBlank = true;
    while (cl.connected()) {  
      if (cl.available()) {                // client data is available to read
        char c = cl.read();                // Read 1 byte (character) from cl
        HTTP_req += c;                         // Save the HTTP request 1 char at a time
        
        // The last line of cl request is blank and ends with \n
        // so respond to cl only after last line received:    
        if (c == '\n' && currentLineIsBlank) {
          
          boolean good_request = false;        // Prevent sending controls by default
          if (HTTP_req.indexOf("GET /") > -1)  // If request is good,  
            good_request = true;               //  Allow control page to be sent to user
                      
          if (HTTP_req.indexOf("GET /?c=Charge+Paypal") > -1){      // If button was clicked,
            relay_on = true;                                        //  Set to send proper button
            digitalWrite(relay, HIGH);                              // Turn relay on
          }
          else if (HTTP_req.indexOf("GET /?c=Stop+Charging") > -1){ // If button was clicked,
            relay_on = false;                                       //  Set to send proper button
            digitalWrite(relay, LOW);                               // Turn relay Off
          }
          // The "GET" in the indexOf command is necessary to avoid detecting the referer address
          // The above compares must be here so state is preversed if user connects again
          
          HTTP_req = "";                            // Finished processing request, empty the string
          
          if (good_request) {
            cl.println("HTTP/1.1 200 OK");          // Send a standard HTTP response header
            cl.println("Content-Type: text/html");
            cl.println("Connection: close");
            cl.println();
            
            cl.println("<!DOCTYPE html>");          // Send Charging Control Pay web page
            cl.println("<html>");
            cl.println("<head>");
            cl.println("<title>SunDollars Coffee</title>"); // Text in Browser Tab 
            cl.println("</head>");
            cl.println("<body>");
            cl.println("<h1>Welcome to SunDollars Coffee Wireless Charging!</h1>");
            cl.println("<form method=\"get\">");
                  
            if (relay_on) {                                  
              cl.println("<p>Your Phone is Charging</p>");  
              cl.println("<input type=\"submit\" name=\"c\" value=\"Stop Charging\" onclick=\"submit();\" />");
            }
            if (!relay_on){                             
              cl.println("<p>Click the Button to Start Charging</p>");
              cl.println("<input type=\"submit\" name=\"c\" value=\"Charge Paypal\" onclick=\"submit();\" />");
            }   
            
            cl.println("</form>");                // End HTML Tags
            cl.println("</body>");
            cl.println("</html>");
            //Serial.print(HTTP_req);             // Uncomment for Debugging
            break;
          }
          else {                                  // If Request is bad, Send Error message to user
            cl.println("HTTP/1.1 400 BAD REQUEST");
            cl.println("Content-Type: text/html");
            cl.println("Connection: close");
            cl.println();
            cl.println("<html>");
            cl.println("<head>");
            cl.println("<title>Bad Request</title>");
            cl.println("</head>");
            cl.println("<body>");
            break;
          }
        }
        // every line of text received from the cl ends with \r\n
        if (c == '\n')                            // Last character on line of received text         
          currentLineIsBlank = true;              // Starting new line with next character read
        else if (c != '\r')                       // A text character was received from client    
          currentLineIsBlank = false;

      } // end if (cl.available())
    } // end while (cl.connected())
    delay(1);                                     // Give the web browser time to receive the data
    cl.stop();                                // Close the connection
  } // end if (cl)
}
//########################################## MAIN END ############################################
//END OF LINE

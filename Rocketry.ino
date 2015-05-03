/*
  GND to GND
  VDD to 3.3V (WARNING: do not connect + to 5V or the sensor will be damaged!)
  SDA  SCL
  A4   A5
*/

#include <SFE_BMP180.h>
#include <Wire.h>
#include <VirtualWire.h>

// You will need to create an SFE_BMP180 object, here called "pressure":

SFE_BMP180 pressure;

double baseline; // baseline pressure
char AltCharMsg[10];
char TempCharMsg[10];
boolean debug = true;

void setup()
{

  Serial.begin(9600);
  Serial.println("SETUP");

  // initialise the 433 Mhz
  vw_set_ptt_inverted(true); // Required for DR3100
  vw_setup(2000);      // Bits per sec

  // Initialize the sensor (it is important to get calibration values stored on the device).

  if (pressure.begin())
    Serial.println("BMP180 init success");
  else
  {
    // Oops, something went wrong, this is usually a connection problem,
    // see the comments at the top of this sketch for the proper connections.

    Serial.println("BMP180 init fail (disconnected?)\n\n");
    while(1); // Pause forever.
  }

  // Get the baseline pressure:
  
  baseline = getPressure();
  
  Serial.print("baseline pressure: ");
  Serial.print(baseline);
  Serial.println(" mb");  
  
  //Send baseline pressure
  //itoa(Sensor1Data,Sensor1CharMsg,10);
      
}

void loop()
{
  double a,P, T;
  
  // Get a new pressure reading:

  P = getPressure();

  // Show the relative altitude difference between
  // the new reading and the baseline reading:

  a = pressure.altitude(P,baseline);

  dtostrf(a,5,2,AltCharMsg);
  //Serial.println(AltCharMsg);

  digitalWrite(13, true); // Turn on a light to show transmitting
  vw_send((uint8_t *)AltCharMsg, strlen(AltCharMsg));
  vw_wait_tx(); // Wait until the whole message is gone
  digitalWrite(13, false); // Turn off a light after transmission
  
  /*
  pressure.startTemperature();
  pressure.getTemperature(T);
  dtostrf(T,3,2,TempCharMsg);

  digitalWrite(13, true); // Turn on a light to show transmitting
  vw_send((uint8_t *)TempCharMsg, strlen(TempCharMsg));
  vw_wait_tx(); // Wait until the whole message is gone
  digitalWrite(13, false); // Turn off a light after transmission
*/

  if (debug){

  Serial.print("relative altitude: ");
  if (a >= 0.0) Serial.print(" "); // add a space for positive numbers
  Serial.print(a,1);
  Serial.print(" meters, ");
  if (a >= 0.0) Serial.print(" "); // add a space for positive numbers  
  delay(50);
  Serial.print("temperature: ");
  Serial.println(T,2);
  }
}


double getPressure()
{
  char status;
  double T,P,p0,a;

  // You must first get a temperature measurement to perform a pressure reading.
  
  // Start a temperature measurement:
  // If request is successful, the number of ms to wait is returned.
  // If request is unsuccessful, 0 is returned.

  status = pressure.startTemperature();
  if (status != 0)
  {
    // Wait for the measurement to complete:

    delay(status);

    // Retrieve the completed temperature measurement:
    // Note that the measurement is stored in the variable T.
    // Use '&T' to provide the address of T to the function.
    // Function returns 1 if successful, 0 if failure.

    status = pressure.getTemperature(T);
    if (status != 0)
    {
      // Start a pressure measurement:
      // The parameter is the oversampling setting, from 0 to 3 (highest res, longest wait).
      // If request is successful, the number of ms to wait is returned.
      // If request is unsuccessful, 0 is returned.

      status = pressure.startPressure(3);
      if (status != 0)
      {
        // Wait for the measurement to complete:
        delay(status);

        // Retrieve the completed pressure measurement:
        // Note that the measurement is stored in the variable P.
        // Use '&P' to provide the address of P.
        // Note also that the function requires the previous temperature measurement (T).
        // (If temperature is stable, you can do one temperature measurement for a number of pressure measurements.)
        // Function returns 1 if successful, 0 if failure.

        status = pressure.getPressure(P,T);
        if (status != 0)
        {
          return(P);
        }
        else Serial.println("error retrieving pressure measurement\n");
      }
      else Serial.println("error starting pressure measurement\n");
    }
    else Serial.println("error retrieving temperature measurement\n");
  }
  else Serial.println("error starting temperature measurement\n");
}




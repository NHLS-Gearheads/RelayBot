#include <Arduino.h>

// extern HardwareSerial Serial1; // I HAVE NO IDEA IF THIS WORKS OR NOT BUT LETS TRY

String read_data_USB();
String read_data_WIFI();
void write_data_USB(String data_to_write);
void write_data_WIFI(String data_to_write);

// uint32_t timer;

void setup() {
  Serial.begin(9600);
  Serial1.begin(9600);

  // timer = millis();
}

void loop() {
  // if(millis() - timer >= 50)// every second
  // {
    // timer = millis();

    String data = read_data_WIFI();
    if(data != "")
    {
      write_data_USB(data);
    }

    data = read_data_USB();
    if(data != "")
    {
      write_data_WIFI(data);
      // write_data_USB(data);
    }
  // }
}

String read_data_WIFI() // returns data up till '\n';    if no data returns "";
{
  String data;
  char last_data;

  data = "";

  if(Serial1.available())
  {
    last_data = char(Serial1.read());
    
    while(last_data != '\n')
    {
      if(Serial1.available())
      {
        data += char(last_data);
        last_data = Serial1.read();
      }
    }
  }

  return(data);
}

String read_data_USB() // returns data up till '\n';    if no data returns "";
{
  String data;
  char last_data;

  data = "";

  if(Serial.available())
  {
    last_data = char(Serial.read());
    
    while(last_data != '\n')
    {
      if(Serial.available())
      {
        data += char(last_data);
        last_data = Serial.read();
      }
    }
  }

  return(data);
}

void write_data_USB(String data_to_write)
{
  for(int pos = 0; data_to_write[pos] != '\0' ;pos++)
    Serial.write(data_to_write[pos]);
  Serial.write("\n");
}

void write_data_WIFI(String data_to_write)
{
  for(int pos = 0; data_to_write[pos] != '\0' ;pos++)
    Serial1.write(data_to_write[pos]);
  Serial1.write("\n");
}
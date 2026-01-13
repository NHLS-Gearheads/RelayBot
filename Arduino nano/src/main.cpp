#include <Arduino.h>

void write_data(String data_to_write);
String read_data();
String data;


void setup() {
  Serial.begin(9600);
}

void loop() {
  data = read_data();
  if(data != "") // recieved data
  {
    write_data("message recieved!"); // echo back
  }
}

String read_data() // returns data up till '\n';    if no data returns "";
{
  String data;
  char last_data;

  if(Serial.available())
  {
    data = "";
    
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

void write_data(String data_to_write) // USB
{
  for(int pos = 0; data_to_write[pos] != '\0' ;pos++)
    Serial.write(data_to_write[pos]);
  Serial.write("\n");
}
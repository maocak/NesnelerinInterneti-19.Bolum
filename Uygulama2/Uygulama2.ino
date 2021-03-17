#include <ESP8266WiFi.h>
#include "DHT.h"  // DHT11 sıcaklık ve nem sensörü kütüphanesi koda eklenir. 
#define DHTTYPE DHT11 // DHT 11 sensörü tanımlanır. 
#define DHTpin 0  //D3 nolu dijital pin(GPIO-0) DHT veri pini olarak atanır. 

int analogdeger = A0; //AO analog pin toprak nem değerini ölçmek için atanır. 
int motorpompapin = 13; // D7 nolu dijital pin(GPIO13) motor pini olarak atanır. 

const char* ssid = "TurkTelekom_TP2950_2.4GHz";
const char* password = "***";

DHT dht(DHTpin, DHTTYPE); 
WiFiServer server(80);

void setup(void)
{ 
Serial.begin(9600);
delay(10);
dht.begin();

pinMode(motorpompapin, OUTPUT); //D7 nolu dijital pin çıkış olarak atanır. 
digitalWrite(motorpompapin, LOW);  // motorpompapin program ilk çalıştırıldığında durur.
// Wi-fi ye bağlantı sağlanır. 
Serial.println();
Serial.print("Wi-fi ye bağlanıyor... ");
Serial.println(ssid);
WiFi.begin(ssid, password);  //Wi-fi bağlantısı başlatılır. 
  
while (WiFi.status() != WL_CONNECTED) {
delay(500);
Serial.print(".");
}
Serial.println("");
Serial.println("WiFi connected");
 
// Sunucu bağlantısı başlatılır. 
server.begin();
Serial.println("Sunucu başladı.");
 
// Seri monitör ekranında bağlantı için kullanılacak IP adresi yazdırılır. 
Serial.print("Bağlanmak için bu URL'yi kullanınız: ");
Serial.print("http://");    //URL IP adresi destop/labtop/cep telefonu tarayıcında açılır. 
Serial.print(WiFi.localIP());
Serial.println("/");
}

void loop() {
// İstemci bağlantısının yapıldığı kontrol edilir. 
WiFiClient client = server.available();
if (!client) {
return;
} 
// İstemci veri yollayana kadar beklenir. 
Serial.println("Yeni istemci");
while(!client.available()){
delay(1);
}
// Yapılan isteğin ilk satırı okunur. 
String request = client.readStringUntil('\r');
Serial.println(request);
client.flush();

float h;  //Nem oranı değişkeni
float t;  //Celcius sıcaklık değişkeni 
float f;  //Fahrenayt sıcaklık değişkeni
float analogokuma ; //Sensörden okunan Analog değer bu değişkene atanır. 
float topraknemoran; // Toprak nem oranının atanacağı değişken 

// Match the request
int value = LOW;
if (request.indexOf("/Up=ON") != -1)  {
h = dht.readHumidity();    //Nem değeri okunur.
t = dht.readTemperature(); //Celcius cinsinden sıcaklık değeri okunur.
f = (h * 1.8) + 32; //Sıcaklık değeri Fahrenayt değerine döndürülür.
analogokuma = analogRead(analogdeger); //Analog pin analogokuma output voltage by water moisture rain sensor
topraknemoran = map(analogokuma,1024,222,0,100); //Sensörden okunan analog değer, yüzelik değere dönüştürülür.

if (analogokuma>800){  // Eğer okunan nem değeri düşükse, motor çalıştırılır. 
digitalWrite(motorpompapin, HIGH);
value = HIGH;
}
else {
digitalWrite(motorpompapin, LOW);
value = LOW;
}
}

if (request.indexOf("/motorpompapin=ON") != -1)  { //Motor çalıştırılır.
digitalWrite(motorpompapin, HIGH);
value = HIGH;
}
if (request.indexOf("/motorpompapin=OFF") != -1)  {//Motor durdurulur
digitalWrite(motorpompapin, LOW);
value = LOW;
}
// Yapılan istek geri döndürülür.
client.println("HTTP/1.1 200 OK");
client.println("Content-Type: text/html");
client.println("");   
client.println("<!DOCTYPE HTML>");
client.println("<html>");
client.println("<h2 align=left>Akilli Bahce - Hava Durumu Uygulamasi</h2><br>");
client.print("Sicaklik (Celsius) = ");
client.println(t); client.print(" C");
client.println("<br>");
client.print("Sicaklik (Fahrenayt) = ");
client.println(f); client.print(" F");
client.println("<br>");
client.print("Nem = ");
client.print(" % ");
client.println(h);
client.println("<br>");  
client.println();
client.print("Toprak Nem Degeri = ");
client.print(" % ");
client.print(topraknemoran);
client.println("<br>");
client.println("<br>");
  
if(topraknemoran>=60){ // Toprak nem oranı yüzde 60'dan büyükse...
client.println("Yagmur/Nem/Su Tespit Edildi");
}else if(topraknemoran<=25)
{
client.println("Toprak Kuru");
} else if((topraknemoran>25)and (topraknemoran<60)){
client.println("Toprak Nem seviyesi Kritik");
}
delay(10);
  
client.println("<br><br>");
if(value == HIGH) 
client.println("Motor/Pompa Calisiyor");
else 
client.print("Motor/Pompa durdu");
client.println("<br><br>");
client.println("<a href=\"/Up=ON\"\"><button>Guncelle</button></a><br />"); 
client.println("<br>");
client.println("<a href=\"/motorpompapin=ON\"\"><button>Motor Pompa -- ON </button></a>");
client.println("<a href=\"/motorpompapin=OFF\"\"><button>Motor Pompa -- OFF </button></a><br />"); 
client.println("</html>");
delay(1);
Serial.println("İstemci baglantisi kesildi.");
Serial.println("");
}

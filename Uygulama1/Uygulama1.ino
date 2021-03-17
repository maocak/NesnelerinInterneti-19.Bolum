#include <EtherCard.h> //Gerekli olan kütüphane koda eklenir.

// Ethernet MAC adresi - kullanılan ağda benzersiz olmalıdır.
static byte mymac[] = { 0x74,0x69,0x69,0x2D,0x30,0x31 };
// Ethernet arabirimi IP adresi
#define STATIC 1 // DHCP'yi kullanım dışı bırakmak için 1 yapılır (Bu durumda aşağıda verilen statik IP adresi kullanılır)
//STATIC 0 seçilirse seri monitör ekranında okunan IP adresi  kullanılır. Aynı zamanda, html kodda da IP adreslerinin değişimi yapılmalıdır.
static byte myip[] = { 192,168,1,200 };
// Ağ geçidi IPsi
static byte gwip[] = { 192,168,0,1 };

byte Ethernet::buffer[700]; // TCP/IP arabellek gönderme ve alma

// Lamba ve Fanı kontrol edecek dijital pinler tanımlanır.
const int ledpin = 2;
const int fanpin = 4;

// İsteğe cevap vermek için kullanılan On ve Off char türünde tanımlanır. 
char* on = "ON";
char* off = "OFF";
char* statusLabel;
char* buttonLabel;

// Yapılan isteğin kontrol edildiği html sayfası
const char page[] PROGMEM =
"HTTP/1.0 503 Service Unavailable\r\n"
"Content-Type: text/html\r\n"
"Retry-After: 600\r\n"
"\r\n"
"<html>"
"<head><title>"
"Arduino Uygulaması IP: 192.168.1.200"
"</title></head>"
"<body>"    
"<h3>AKILLI EV UYGULAMASI</h3>" 
"<p>FAN DURUMU</p>"   
"<a href='http://192.168.1.200/?FAN=ON'>ON</a><br>"
"<a href='http://192.168.1.200/?FAN=OFF'>OFF</a><br>"
"<br>"
"<p>LAMBA DURUMU</p>"   
"<a href='http://192.168.1.200/?LAMBA=ON'>ON</a><br>"
"<a href='http://192.168.1.200/?LAMBA=OFF'>OFF</a><br>"           
"</body>"
"</html>"
;

void setup(){
Serial.begin(9600);
Serial.println("IP adresi almaya çalışıyor.");
Serial.print("MAC adresi: ");
for (byte i = 0; i < 6; ++i) 
{
Serial.print(mymac[i], HEX);
if (i < 5)
Serial.print(':');
}
Serial.println();
if (ether.begin(sizeof Ethernet::buffer, mymac) == 0)
{
Serial.println( "Ethernet denetleyicisine erişilemedi.");
}
else
{
Serial.println("Ethernet denetleyicisi erişimi Tamam");
}
;
#if STATIC
Serial.println( "Statik IP alınıyor.");
if (!ether.staticSetup(myip, gwip))
{
Serial.println( "Statik IP alınamadı.");
}
#else

Serial.println("DHCP protokolü düzenleniyor.");
if (!ether.dhcpSetup())
{
Serial.println( "DHCP protokol hatası");
}
#endif

ether.printIp("IP adresi: ", ether.myip);
ether.printIp("Alt ağ maskesi: ", ether.netmask);
ether.printIp("Ağ geçidi IP: ", ether.gwip);
ether.printIp("DNS IP: ", ether.dnsip);
// 2 nolu ledpin çıkış olarak tanımlanır.
pinMode(ledpin, OUTPUT);
// 4 nolu fanpin çıkış olarak tanımlanır.
pinMode(fanpin, OUTPUT);
}
void loop(){  
word len = ether.packetReceive();
word pos = ether.packetLoop(len);

// Eğer ON komutu alınırsa(pin2), FANı çalıştır.
if(strstr((char *)Ethernet::buffer + pos, "GET /?FAN=ON") != 0) {
Serial.println("ON komutu alındı.");
digitalWrite(ledpin, HIGH);
}
// Eğer OFF komutu alınırsa(pin2), FANı kapat.   
if(strstr((char *)Ethernet::buffer + pos, "GET /?FAN=OFF") != 0) {
Serial.println("OFF komutu alındı.");
digitalWrite(ledpin, LOW);
}
// Eğer ON komutu alınırsa(pin4), lambayı yak.  
if(strstr((char *)Ethernet::buffer + pos, "GET /?LAMBA=ON") != 0) {
Serial.println("ON komutu alındı.");
digitalWrite(fanpin, HIGH);
}
// Eğer OFF komutu alınırsa(pin4), lambayı söndür.  
if(strstr((char *)Ethernet::buffer + pos, "GET /?LAMBA=OFF") != 0) {
Serial.println("OFF komutu alındı.");
digitalWrite(fanpin, LOW);
}
//İsteğin tamamlanması için bir sayfa döndürülür.
memcpy_P(ether.tcpOffset(), page, sizeof page);
ether.httpServerReply(sizeof page - 1);  
}

#include <Adafruit_NeoPixel.h>
#include <ESP8266WiFi.h>
#include <NTPClient.h>
#include <WiFiUdp.h>

// Configuration Wi-Fi
const char* ssid = "miguet_automation";
const char* password = "vincentbld196300";

// Configuration des LEDs
#define PIN2           D6
#define NUMPIXELS2     52

Adafruit_NeoPixel pixels2(NUMPIXELS2, PIN2, NEO_GRB + NEO_KHZ800);

// Configuration du NTP
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", 7200, 60000); // UTC+2 pour Paris (heure d'été)

// Couleurs pour chaque segment de temps sur le ruban 2
uint32_t morningColor = pixels2.Color(255, 140, 0);  // Matin - Orange foncé
uint32_t noonColor = pixels2.Color(255, 255, 0);     // Midi - Jaune vif
uint32_t afternoonColor = pixels2.Color(173, 216, 230); // Après-midi - Bleu clair
uint32_t eveningColor = pixels2.Color(0, 0, 139);    // Soir - Bleu foncé
uint32_t nightColor = pixels2.Color(0, 0, 50);       // Nuit - Bleu profond
uint32_t secondHandColor = pixels2.Color(0, 0, 0); // Trotteuse - Rouge

void setup() {
  Serial.begin(115200);
  delay(10);

  // Connexion au Wi-Fi
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  
  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  
  Serial.println("");
  Serial.println("WiFi connected");
  
  // Initialisation des LEDs
  pixels2.begin();
  pixels2.show(); // Initialise toutes les LEDs à 'off'

  // Initialisation du client NTP
  timeClient.begin();
}

void loop() {
  timeClient.update();
  
  // Effacer toutes les LEDs
  pixels2.clear();
  
  // Obtenir l'heure actuelle
  int currentHour = timeClient.getHours();
  int currentMinute = timeClient.getMinutes();
  int currentSecond = timeClient.getSeconds();

  // Calculer la position des pixels pour l'heure actuelle sur 24 heures
  int pixelIndex = map(currentHour * 60 + currentMinute, 0, 24 * 60, 0, 52);

  // Définir la couleur en fonction de la période de la journée
  uint32_t currentColor;
  if (currentHour >= 6 && currentHour < 12) {
    currentColor = morningColor;  // Matin
  } else if (currentHour >= 12 && currentHour < 14) {
    currentColor = noonColor;  // Midi
  } else if (currentHour >= 14 && currentHour < 18) {
    currentColor = afternoonColor;  // Après-midi
  } else if (currentHour >= 18 && currentHour < 23) {
    currentColor = eveningColor;  // Soir
  } else {
    currentColor = nightColor;  // Nuit
  }

  // Allumer 2 LEDs pour représenter l'heure actuelle
  pixels2.setPixelColor(pixelIndex, currentColor);
  pixels2.setPixelColor((pixelIndex + 1) % NUMPIXELS2, currentColor);

  // Calculer la position de la trotteuse (tour complet de 52 LEDs par minute)
  int secondHandIndex = map(currentSecond, 0, 60, 0, 52);

  // Allumer la LED pour la trotteuse
  pixels2.setPixelColor(secondHandIndex, secondHandColor);

  // Afficher les changements
  pixels2.show();
  
  // Attendre avant de rafraîchir (ajuster si nécessaire)
  delay(1000);
}

#include <FastBot.h>
#include <Wifi.h>
#include <esp_camera.h>

const char* ssid = "**********";
const char* password = "**********";

const char* telegramToken = "**********";
const char* telegramChatId = "**********";

const int ledPin = 4;
bool statusLed = false;

camera_fb_t *cameraFrame = NULL;

FastBot bot(telegramToken);

void setupWiFi()
{
  Serial.println();
  Serial.print("Подключение к Wi-Fi");
  WiFi.begin(ssid,password);
  while(WiFi.status()!= WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");  
  }
  Serial.println();
  Serial.print("Подключено к сети ");
  Serial.println(ssid);
  Serial.print("IP-адрес: ");
  Serial.println(WiFi.localIP());
}

void setupCamera()
{
  camera_config_t config;
  config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer = LEDC_TIMER_0;
  config.pin_d0 = 5;
  config.pin_d1 = 18;
  config.pin_d2 = 19;
  config.pin_d3 = 21;
  config.pin_d4 = 36;
  config.pin_d5 = 39;
  config.pin_d6 = 34;
  config.pin_d7 = 35;
  config.pin_xclk = 0;
  config.pin_pclk = 22;
  config.pin_vsync = 25;
  config.pin_href = 23;
  config.pin_sscb_sda = 26;
  config.pin_sscb_scl = 27;
  config.pin_pwdn = 32;
  config.pin_reset = -1;
  config.xclk_freq_hz = 20000000;
  config.pixel_format = PIXFORMAT_JPEG;   
  config.frame_size = FRAMESIZE_UXGA;
  config.jpeg_quality = 10;
  config.fb_count = 2;

  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) 
  {
    Serial.printf("Ошибка инициализации камеры! (0x%x)\n", err);
    while (1);
  }
  
  else 
  {
    Serial.println("Камера инициализирована");
  }
}

void takePhoto()
{
  if (statusLed == true)
  {
    digitalWrite(ledPin, HIGH);
    delay(500);
  }
  
  cameraFrame = esp_camera_fb_get();
  if (statusLed == true)
  {
    digitalWrite(ledPin, LOW);
  }
        
  if (cameraFrame) 
  {
    bot.sendMessage("Фото получено и отправляется.", telegramChatId);
    bot.sendFile(cameraFrame->buf, cameraFrame->len, FB_PHOTO, telegramChatId);
    esp_camera_fb_return(cameraFrame);
  } 
    
  else 
  {
    bot.sendMessage("Ошибка при получении фото.", telegramChatId);
  }
}

void newMessage(FB_msg& msg) 
{
  Serial.println();
  Serial.println(msg.chatID);
  Serial.println(msg.username);
  Serial.println(msg.text);

  if (msg.text == "/photo")
  {
    takePhoto();  
  }
    
  else if (msg.text == "/ledon")
  {
    statusLed = true;
    bot.sendMessage("Диод включен.", msg.chatID);
  }
  
  else if (msg.text == "/ledoff")
  {
    statusLed = false;
    bot.sendMessage("Диод выключен.", msg.chatID);
  }
}

void setup() 
{
  Serial.begin(9600);

  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, LOW);

  setupWiFi();
  setupCamera();
  
  bot.setChatID(telegramChatId);
  bot.sendMessage("Я включился и жду указаний.");
  bot.attach(newMessage);
}

void loop() 
{
  bot.tick();
}

#include <M5StickCPlus.h>
#include <WiFi.h>
#include <WiFiClient.h>
#include <WiFiAP.h>
#include "time.h"
#include "page.h"

const IPAddress ap_ip(192, 168, 4, 1);
const char *connect_ssid     = "xxxxx";
const char *connect_password = "xxxxx";
const char *self_ssid     = "M5Stack_Ap";
const char *self_password = "poyu9239";
const char *ntp_server = "time.stdtime.gov.tw";
const long time_zone = 28800; // GMT+8

String schedule[5] = {"", "", "", "", ""};

RTC_TimeTypeDef rtc_time;
RTC_DateTypeDef rtc_date;
WiFiServer server(80);

void scheduler();
void button_control();
void motor_init();
void motor_on();
void motor_off();
void start_web_server();
void web_server_handle();
void send_schedule_page(WiFiClient client);
void clear_lcd();

void setup() {
    M5.begin();
    M5.lcd.setRotation(3);
    time_sync();
    motor_init();
    motor_off();
    start_web_server();
}

void loop() {
    M5.update();
    web_server_handle();
    scheduler();
    button_control();
}

void scheduler() {
    struct tm timeInfo;
    if (getLocalTime(&timeInfo)) {
        char time_str[7];
        strftime(time_str, 7, "%H:%M", &timeInfo);
        for (int i = 0; i < 5; i++) {
            if (schedule[i] == time_str) {
                motor_on();
                delay(1000);
                motor_off();
                schedule[i] = "";
            }
        }
    }
}

void start_web_server() {
    WiFi.softAP(self_ssid, self_password);
    server.begin();
    M5.Lcd.println("Server started");
    M5.Lcd.println(WiFi.softAPIP());
}

void web_server_handle() {
    WiFiClient client = server.available();
    if (client) {
        String current_line = "";
        boolean get_time = false;
        boolean delete_schedule = false;
        while (client.connected()) {
            if (client.available()) {
                char c = client.read();
                if (c == '\n') {
                    if (current_line.length() == 0) {
                        client.println("HTTP/1.1 200 OK");
                        client.println("Content-type:text/html");
                        client.println();
                        send_schedule_page(client);
                        client.println();
                        break;
                    } else {
                        current_line = "";
                    }
                } else if (c != '\r') {
                    current_line += c;
                }
                if (current_line.indexOf("GET /set_schedule?time=") != -1 && current_line.indexOf("HTTP/1.1") != -1 && !get_time) {
                    int sub_index = current_line.indexOf("time=") + 5;
                    String time = current_line.substring(sub_index, sub_index + 7);
                    for (int i = 0; i < 5; i++) {
                        if (schedule[i] == "") {
                            time.replace("%3A", ":");
                            schedule[i] = time;
                            break;
                        }
                    }
                    get_time = true;
                }
                if (current_line.indexOf("GET /delete_schedule?index=") != -1 && current_line.indexOf("HTTP/1.1") != -1 && !delete_schedule) {
                    int sub_index = current_line.indexOf("index=") + 6;
                    int index = current_line.substring(sub_index, sub_index + 1).toInt();
                    schedule[index] = "";
                    delete_schedule = true;
                }
            }
            button_control();
        }
        client.stop();
    }
}

void send_schedule_page(WiFiClient client) {
    client.println(page_part1);
    for (int i = 0; i < 5; i++) {
        if (schedule[i] != "") {
            String page_part2_copy = page_part2;
            page_part2_copy.replace("${index}", String(i));
            page_part2_copy.replace("${time}", schedule[i]);
            client.println(page_part2_copy);
        }
    }
    client.println(page_part3);
}

void button_control() {
    if (M5.BtnA.isPressed()) motor_on();
    if (M5.BtnA.wasReleased()) motor_off();
}

void motor_init() {
    ledcSetup(0, 500, 8);
    ledcAttachPin(26, 0);
}

void motor_on() {
    ledcWrite(0, 255);
}

void motor_off() {
    ledcWrite(0, 0);
}

void time_sync() {
    M5.Lcd.printf("connecting WiFi: %s ", connect_ssid);
    WiFi.begin(connect_ssid, connect_password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        M5.Lcd.print(".");
    }
    M5.Lcd.fillScreen(BLACK);
    M5.Lcd.println("Connected");
    // Set ntp time to local
    configTime(time_zone, 0, ntp_server);

    // Get local time
    struct tm timeInfo;
    if (getLocalTime(&timeInfo)) {
        // Set RTC time
        RTC_TimeTypeDef TimeStruct;
        TimeStruct.Hours   = timeInfo.tm_hour;
        TimeStruct.Minutes = timeInfo.tm_min;
        TimeStruct.Seconds = timeInfo.tm_sec;
        M5.Rtc.SetTime(&TimeStruct);

        RTC_DateTypeDef DateStruct;
        DateStruct.WeekDay = timeInfo.tm_wday;
        DateStruct.Month = timeInfo.tm_mon + 1;
        DateStruct.Date = timeInfo.tm_mday;
        DateStruct.Year = timeInfo.tm_year + 1900;
        M5.Rtc.SetDate(&DateStruct);
        M5.Lcd.println("synced");
        delay(500);
        clear_lcd();
    }
}

void clear_lcd() {
    M5.Lcd.fillScreen(BLACK);
    M5.Lcd.setCursor(0, 0);
}
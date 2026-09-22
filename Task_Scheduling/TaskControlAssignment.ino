#include <Arduino.h>
#include <Adafruit_NeoPixel.h>

// ==================================================
// Onboard RGB LED
// ESP32-S3-DevKitC-1 v1.1 -> GPIO 38
// ==================================================

#define RGB_LED_PIN 38
#define NUM_LEDS 1

Adafruit_NeoPixel rgbLed(
    NUM_LEDS,
    RGB_LED_PIN,
    NEO_GRB + NEO_KHZ800
);


// ==================================================
// Shared variable
// ==================================================

volatile uint32_t blinkInterval = 500;


// ==================================================
// Task Handles
// ==================================================

TaskHandle_t serialTaskHandle = NULL;
TaskHandle_t ledTaskHandle = NULL;


// ==================================================
// LED helper functions
// ==================================================

void ledOn()
{
    rgbLed.setPixelColor(
        0,
        rgbLed.Color(0, 150, 0)   // Green
    );

    rgbLed.show();
}


void ledOff()
{
    rgbLed.setPixelColor(
        0,
        rgbLed.Color(0, 0, 0)
    );

    rgbLed.show();
}


// ==================================================
// LED Task
// ==================================================

void ledTask(void *p)
{
    for (;;)
    {
        // LED ON
        ledOn();

        // Read latest interval
        uint32_t interval = blinkInterval;

        // Task becomes BLOCKED
        vTaskDelay(pdMS_TO_TICKS(interval));


        // LED OFF
        ledOff();

        // Read latest interval again
        interval = blinkInterval;

        // Task becomes BLOCKED
        vTaskDelay(pdMS_TO_TICKS(interval));
    }
}


// ==================================================
// Serial Task
// ==================================================

void serialTask(void *p)
{
    String command;

    for (;;)
    {
        if (Serial.available() > 0)
        {
            command = Serial.readStringUntil('\n');

            command.trim();


            // ==========================================
            // Change interval
            // ==========================================

            if (command == "250")
            {
                blinkInterval = 250;

                Serial.println(
                    "Blink interval = 250 ms"
                );
            }


            else if (command == "500")
            {
                blinkInterval = 500;

                Serial.println(
                    "Blink interval = 500 ms"
                );
            }


            else if (command == "1000")
            {
                blinkInterval = 1000;

                Serial.println(
                    "Blink interval = 1000 ms"
                );
            }


            // ==========================================
            // Suspend LED Task
            // ==========================================

            else if (command == "suspend")
            {
                Serial.println(
                    "Suspending LED Task..."
                );

                vTaskSuspend(ledTaskHandle);

                Serial.println(
                    "LED Task suspended."
                );
            }


            // ==========================================
            // Resume LED Task
            // ==========================================

            else if (command == "resume")
            {
                Serial.println(
                    "Resuming LED Task..."
                );

                vTaskResume(ledTaskHandle);

                Serial.println(
                    "LED Task resumed."
                );
            }


            // ==========================================
            // Invalid command
            // ==========================================

            else
            {
                Serial.println("Invalid command.");
                Serial.println(
                    "Use: 250, 500, 1000, suspend, resume"
                );
            }
        }


        // Serial Task sleeps for 50 ms
        vTaskDelay(pdMS_TO_TICKS(50));
    }
}


// ==================================================
// Setup
// ==================================================

void setup()
{
    Serial.begin(115200);

    // Initialize onboard RGB LED
    rgbLed.begin();

    // Start with LED OFF
    rgbLed.clear();
    rgbLed.show();


    delay(1000);


    Serial.println();
    Serial.println("==============================");
    Serial.println("FreeRTOS Task Control Lab");
    Serial.println("==============================");

    Serial.println("Commands:");
    Serial.println("250");
    Serial.println("500");
    Serial.println("1000");
    Serial.println("suspend");
    Serial.println("resume");
    Serial.println();


    // ==================================================
    // Create Serial Task
    // ==================================================

    xTaskCreatePinnedToCore(
        serialTask,
        "Serial Task",
        4096,
        NULL,
        2,
        &serialTaskHandle,
        0
    );


    // ==================================================
    // Create LED Task
    // ==================================================

    xTaskCreatePinnedToCore(
        ledTask,
        "LED Task",
        4096,
        NULL,
        1,
        &ledTaskHandle,
        0
    );
}


// ==================================================
// Arduino loop
// ==================================================

void loop()
{
    // FreeRTOS tasks do the work.

    vTaskDelay(pdMS_TO_TICKS(1000));
}
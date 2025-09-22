// Define pin assignments for the IR sensors and traffic light LEDs  
const int IR_SENSOR_PINS[4] = {A3, A0, A1, A2}; // Analog input pins for IR sensors  
const int RED_PINS[4] = {2, 5, 8, 11};           // Output pins for red lights  
const int YELLOW_PINS[4] = {3, 6, 9, 12};        // Output pins for yellow lights  
const int GREEN_PINS[4] = {4, 7, 10, 13};        // Output pins for green lights  
// Configuration Constants  
const int THRESHOLD = 500;                      // Sensor value threshold to detect vehicles  
const unsigned long BASE_GREEN_DURATION = 3000; // Default 3s for green light as base  
const unsigned long FIXED_YELLOW_DURATION = 2000;  
const unsigned long FIXED_RED_DURATION = 2000;    
const unsigned long BUFFER_TIME = 1000;        // Buffer time between phases  
const unsigned long SENSOR_READ_INTERVAL = 50;  // Sensor polling interval  
const unsigned long MAX_GREEN_DURATION = 13000;  
// Global variables to hold traffic light system state  
unsigned int vehicleCounts[4] = {0, 0, 0, 0}; // Vehicle counts for each direction  
unsigned int lastSensorValues[4] = {0, 0, 0, 0}; // Last sensor readings for each direction  
unsigned long lastSensorReadTime = 0;    // Time for the last sensor read  
unsigned long lastLightChangeTime = 0;   // Time for the last light change  
int currentDirection = 0;                // Current direction with the green light  
enum LightPhase { GREEN, YELLOW, RED, BUFFER };  
LightPhase currentPhase = GREEN; // Current phase of the traffic light  
unsigned int historicalCounts[100][4]; // Historical counts for up to 100 cycles  
unsigned int currentCycle = 0;          // Tracks the current cycle of traffic lights  


void setup() {  
    Serial.begin(9600); // Start serial communication for real-time data streaming  
    // Initialize pin modes for IR sensors and traffic light LEDs  
    for (int i = 0; i < 4; i++) {  
        pinMode(IR_SENSOR_PINS[i], INPUT);      // Set IR sensor pins as INPUT  
        pinMode(RED_PINS[i], OUTPUT);           // Set RED light pins as OUTPUT  
        pinMode(YELLOW_PINS[i], OUTPUT);        // Set YELLOW light pins as OUTPUT  
        pinMode(GREEN_PINS[i], OUTPUT);         // Set GREEN light pins as OUTPUT  
        lastSensorValues[i] = analogRead(IR_SENSOR_PINS[i]); // Initialize sensor values  
    }  
    setAllLightsToRed(); // Initially set all lights to red  
}  


void setAllLightsToRed() {  
    // Turn on the red lights and turn off the yellow and green lights for all directions  
    for (int i = 0; i < 4; i++) {  
        digitalWrite(RED_PINS[i], HIGH);     // Set red lights to HIGH, other colours to LOW  
        digitalWrite(YELLOW_PINS[i], LOW);  
        digitalWrite(GREEN_PINS[i], LOW);    
    }  
}  


void updateVehicleCount() {  
    // Algorithm to count vehicles based on the IR sensor readings  
    for (int i = 0; i < 4; i++) {  
        int sensorValue = analogRead(IR_SENSOR_PINS[i]); // Read the sensor value  
        // Detect a vehicle crossing when the sensor detects 1 then 0  
        if (sensorValue > THRESHOLD && lastSensorValues[i] <= THRESHOLD) {  
            vehicleCounts[i]++;  // Increment vehicle count for the direction  
        }  
        lastSensorValues[i] = sensorValue; // Update last sensor value  
    }  
}  


int NewDir(int startDir) {  
    // Find the next direction that has vehicles waiting  
    int attempts = 0;  
    while (vehicleCounts[startDir] == 0 && attempts < 4) {  
        startDir = (startDir + 1) % 4;  // Move to the next direction (cycling through 0 to 3)  
        attempts++;  
    }  
    return startDir; // Return the next direction with vehicles  
}  


unsigned long calculateDynamicGreenDuration() {  
    // Calculate the dynamic duration for the green light based only on vehicles  
    unsigned int totalVehicles = 0;
    totalVehicles = vehicleCounts[currentDirection];  
    // Add 1s for every vehicle detected at the current direction, with a cap of MAX_GREEN_DURATION  
    unsigned long dynamicDuration = BASE_GREEN_DURATION + totalVehicles * 1000;  
    return min(dynamicDuration, MAX_GREEN_DURATION); // Ensure green duration stays within max  
}


void controlTrafficLights() {  
    unsigned long currentTime = millis();  // Get the current time in milliseconds  
    unsigned long elapsedTime = currentTime - lastLightChangeTime; // Calculate the elapsed time since the last light change  


    switch (currentPhase) {  
        case GREEN:  
            digitalWrite(GREEN_PINS[currentDirection], HIGH);  
            digitalWrite(RED_PINS[currentDirection], LOW);  
           
            // Check if the green light duration has elapsed  
            if (elapsedTime >= calculateDynamicGreenDuration()) {  
                // Turn off the green light, turn on the yellow light  
                digitalWrite(GREEN_PINS[currentDirection], LOW);  
                digitalWrite(YELLOW_PINS[currentDirection], HIGH);  


                currentPhase = YELLOW; // Transition to YELLOW phase  
                lastLightChangeTime = currentTime;    // Update the last light change time  
            }  
            break;  


        case YELLOW:  
            if (elapsedTime >= FIXED_YELLOW_DURATION) {  
                // Turn off the yellow light, turn on the red light  
                digitalWrite(YELLOW_PINS[currentDirection], LOW);  
                digitalWrite(RED_PINS[currentDirection], HIGH);  


                currentPhase = BUFFER; // Transition to BUFFER phase  
                lastLightChangeTime = currentTime;    // Update the last light change time  
            }  
            break;  


        case BUFFER:  
            if (elapsedTime >= BUFFER_TIME) {  
                // Reset vehicle count for the current direction before moving to the next  
                saveHistoricalData();  
                vehicleCounts[currentDirection] = 0;  
                // Move to the next direction with vehicles  
                currentDirection = NewDir((currentDirection + 1) % 4);  
                currentPhase = RED; // Transition to RED phase  
                lastLightChangeTime = currentTime; // Update the last light change time  
            }  
            break;  


        case RED:  
            if (elapsedTime >= FIXED_RED_DURATION) {  
                currentPhase = GREEN; // Transition to GREEN phase  
                lastLightChangeTime = currentTime;  // Update the last light change time  
            }  
            break;  
    }  
}  


void saveHistoricalData() {  
    // Store the vehicle counts for the current cycle  
    for (int i = 0; i < 4; i++) {  
        historicalCounts[currentCycle][i] = vehicleCounts[i];  
    }    // Reset vehicle counts for the next cycle
    currentCycle++;  // Increment cycle count  
}  


void sendToExcel() {  
    // Output current timestamp and counts in a format suitable for a table  
    Serial.print("Real time count\n"); // Print timestamp in seconds with two decimal places  
    for (int i = 0; i < 4; i++) {  
        Serial.print(vehicleCounts[i]);  
        if (i < 3) Serial.print(","); // Comma-separated  
    }  
    Serial.println(); // End of current counts row  


    // Print historical data  
    Serial.println("Historical Data\n"); // Heading for historical data  
    for (int cycle = 0; cycle < currentCycle; cycle++) {  
        Serial.print("Cycle ");  
        Serial.print(cycle + 1); // Cycle number  
        Serial.print(",");  
        for (int i = 0; i < 4; i++) {  
            Serial.print(historicalCounts[cycle][i]);  
            if (i < 3) Serial.print(","); // Comma-separated  
        }  
        Serial.println(); // End of historical data row  
    }  
    Serial.println(); // Blank line for separation  
}  


void loop() {  
    // Update vehicle counts at regular intervals  
    if (millis() - lastSensorReadTime >= SENSOR_READ_INTERVAL) {  
        updateVehicleCount();  
        lastSensorReadTime = millis();  
    }  
    // Control traffic light phases  
    controlTrafficLights();  
    static unsigned long lastPrintTime = 0; // Tracks the last time data was printed  
    if (millis() - lastPrintTime >= 1000) { // Check if 1 second has passed  
        sendToExcel();  
        lastPrintTime = millis(); // Update the last print time  
    }  
}

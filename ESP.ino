#include <WiFi.h>
#include <WebServer.h>
#include <HTTPClient.h>
#include <WiFiClientSecure.h>  // Include for secure SSL connection

// Wi-Fi Credentials
const char* ssid = "Your_WiFi_SSID";
const char* password = "Your_WiFi_Password";

// Web Server on Port 80
WebServer server(80);

// Serial communication with Arduino (or any device sending the data)
#define ARDUINO_SERIAL_SPEED 9600 // Set the baud rate of Arduino

// Store the received data
float temperature = 0.0;
float humidity = 0.0;
float pressure = 0.0;
float windspeed = 0.0;
bool rainPrediction = false; // To store the rain prediction

// Web page to display data
String webPage = "<html>\
  <head>\
    <title>Weather Prediction</title>\
    <style>\
      body {\
        font-family: Arial, sans-serif;\
        background-color: #f0f8ff;\
        margin: 0;\
        padding: 0;\
      }\
      header {\
        background-color: #4CAF50;\
        color: white;\
        padding: 10px 0;\
        text-align: center;\
        font-size: 24px;\
      }\
      .container {\
        width: 80%;\
        margin: 0 auto;\
        padding: 20px;\
        background-color: white;\
        box-shadow: 0px 4px 8px rgba(0, 0, 0, 0.1);\
        border-radius: 8px;\
        margin-top: 20px;\
      }\
      h2 {\
        text-align: center;\
        color: #333;\
      }\
      .form-group {\
        margin-bottom: 15px;\
      }\
      label {\
        font-weight: bold;\
        color: #555;\
        font-size: 16px;\
      }\
      input[type='text'] {\
        width: 100%;\
        padding: 10px;\
        margin-top: 5px;\
        border: 2px solid #ddd;\
        border-radius: 5px;\
        font-size: 14px;\
      }\
      input[type='submit'] {\
        background-color: #4CAF50;\
        color: white;\
        border: none;\
        padding: 12px 20px;\
        text-align: center;\
        font-size: 16px;\
        cursor: pointer;\
        border-radius: 5px;\
        width: 100%;\
        margin-top: 20px;\
      }\
      input[type='submit']:hover {\
        background-color: #45a049;\
      }\
      .result {\
        text-align: center;\
        font-size: 18px;\
        margin-top: 30px;\
      }\
      .result span {\
        font-weight: bold;\
        color: #4CAF50;\
      }\
      footer {\
        text-align: center;\
        font-size: 14px;\
        margin-top: 30px;\
        color: #777;\
      }\
    </style>\
    <script>\
      setInterval(function() {\
        fetch('/getData')\
          .then(response => response.json())\
          .then(data => {\
            document.getElementById('temperature').value = data.temperature;\
            document.getElementById('humidity').value = data.humidity;\
            document.getElementById('pressure').value = data.pressure;\
            document.getElementById('windspeed').value = data.windspeed;\
            document.getElementById('rainPrediction').innerText = data.rainPrediction ? 'Yes' : 'No';\
          });\
      }, 2000);  // Update every 2 seconds\
    </script>\
  </head>\
  <body>\
    <header>\
      Weather Prediction\
    </header>\
    <div class='container'>\
      <h2>Weather Data</h2>\
      <form action='/submit' method='POST'>\
        <div class='form-group'>\
          <label for='temperature'>Temperature (°C):</label>\
          <input type='text' id='temperature' name='temperature' value='%TEMP%' readonly><br>\
        </div>\
        <div class='form-group'>\
          <label for='humidity'>Humidity (%):</label>\
          <input type='text' id='humidity' name='humidity' value='%HUMIDITY%' readonly><br>\
        </div>\
        <div class='form-group'>\
          <label for='pressure'>Pressure (hPa):</label>\
          <input type='text' id='pressure' name='pressure' value='%PRESSURE%' readonly><br>\
        </div>\
        <div class='form-group'>\
          <label for='windspeed'>Wind Speed (km/h):</label>\
          <input type='text' id='windspeed' name='windspeed' value='%WINDSPEED%' readonly><br><br>\
        </div>\
        <input type='submit' value='Submit Data'>\
      </form>\
      <div class='result'>\
        <h3>Will it rain tomorrow? <span id='rainPrediction'>Processing...</span></h3>\
      </div>\
    </div>\
    <footer>\
      <p>&copy; 2024 Weather Prediction System. All rights reserved.</p>\
    </footer>\
  </body>\
</html>";


void setup() {
  Serial.begin(115200); // Debugging via Serial Monitor
  Serial1.begin(ARDUINO_SERIAL_SPEED, SERIAL_8N1, 16, 17); // ESP32 uses Serial1 to communicate with Arduino (use GPIO16 and GPIO17 for RX/TX)

  // Connect to WiFi
  WiFi.begin(ssid, password);

  // Wait for WiFi connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }

  Serial.println("Connected to WiFi");
  Serial.print("Server IP Address: ");
  Serial.println(WiFi.localIP());

  // Web server root page
  server.on("/", []() {
    // Serve the web page with the current sensor data
    String page = webPage;
    page.replace("%TEMP%", String(temperature));
    page.replace("%HUMIDITY%", String(humidity));
    page.replace("%PRESSURE%", String(pressure));
    page.replace("%WINDSPEED%", String(windspeed));
    page.replace("%RAINF", rainPrediction ? "Yes" : "No");
    server.send(200, "text/html", page);
  });

  // Handle form submission
server.on("/submit", HTTP_POST, []() {
  // Ensure that the Arduino data has been updated
  if (temperature != 0.0 && humidity != 0.0 && pressure != 0.0 && windspeed != 0.0) {
    // Send the data to the model server via HTTPS POST
    String serverUrl = "https://iot-weather-prediction.onrender.com/predict";  // URL of your deployed model
    HTTPClient http;
    WiFiClientSecure client;  // Create a secure WiFiClient object

    // Disable SSL verification (for testing)
    client.setInsecure();  // Bypasses SSL verification

    http.begin(client, serverUrl);  // Use the WiFiClientSecure object for HTTPS
    http.addHeader("Content-Type", "application/json");

    // Prepare the JSON data for POST request
    String jsonData = "{\"AvgTemp\": " + String(temperature) + 
                      ", \"Humidity9am\": " + String(humidity) + 
                      ", \"Pressure9am\": " + String(pressure) + 
                      ", \"WindSpeed9am\": " + String(windspeed) + "}";

    Serial.println("Sending JSON Data: " + jsonData); // Debugging JSON payload

    // Send POST request
    int httpResponseCode = http.POST(jsonData);

    // Check the response from the server
    if (httpResponseCode > 0) {
      String response = http.getString();
      Serial.println("Prediction result from server: " + response);

      // Parse the response
      rainPrediction = response.indexOf("\"RainTomorrow\":1") > -1;

      // Display the result on the web page with improved UI
      String resultPage = "<html>\
        <head>\
          <title>Prediction Result</title>\
          <style>\
            body {\
              font-family: Arial, sans-serif;\
              background-color: #f0f8ff;\
              text-align: center;\
              margin: 0;\
              padding: 0;\
            }\
            .container {\
              width: 70%;\
              margin: 30px auto;\
              padding: 30px;\
              background-color: white;\
              box-shadow: 0px 4px 12px rgba(0, 0, 0, 0.1);\
              border-radius: 8px;\
            }\
            h1 {\
              color: #4CAF50;\
            }\
            p {\
              font-size: 18px;\
              font-weight: bold;\
              color: #333;\
            }\
            .result {\
              font-size: 20px;\
              color: #4CAF50;\
              margin-top: 20px;\
              font-weight: bold;\
            }\
            a {\
              text-decoration: none;\
              color: #fff;\
              background-color: #4CAF50;\
              padding: 10px 20px;\
              border-radius: 5px;\
              margin-top: 20px;\
              display: inline-block;\
            }\
            a:hover {\
              background-color: #45a049;\
            }\
            span {\
              color: #4CAF50;\
              font-size: 22px;\
            }\
          </style>\
        </head>\
        <body>\
          <div class='container'>\
            <h1>Prediction Result</h1>\
            <p>Temperature: " + String(temperature) + " °C</p>\
            <p>Humidity: " + String(humidity) + " %</p>\
            <p>Pressure: " + String(pressure) + " hPa</p>\
            <p>Wind Speed: " + String(windspeed) + " km/h</p>\
            <div class='result'>\
              <p>Will it rain tomorrow?\
                 <span>" + (rainPrediction ? "Yes" : "No") + "</span>\
              </p>\
           </div>\
            <a href='/'>Back to Home</a>\
          </div>\
        </body>\
      </html>";
      
      // Send the prediction result page to the client
      server.send(200, "text/html", resultPage);

      // Display on Serial Monitor
      Serial.println("Will it rain tomorrow? " + String(rainPrediction ? "Yes" : "No"));
    } else {
      // Handle error in prediction
      String errorPage = "<html>\
        <head>\
          <title>Error</title>\
          <style>\
            body {\
              font-family: Arial, sans-serif;\
              background-color: #f8d7da;\
              text-align: center;\
              margin: 0;\
              padding: 0;\
            }\
            .container {\
              width: 70%;\
              margin: 30px auto;\
              padding: 30px;\
              background-color: white;\
              box-shadow: 0px 4px 12px rgba(0, 0, 0, 0.1);\
              border-radius: 8px;\
              color: #721c24;\
            }\
            h1 {\
              color: #721c24;\
            }\
            p {\
              font-size: 18px;\
              font-weight: bold;\
            }\
            a {\
              text-decoration: none;\
              color: #fff;\
              background-color: #721c24;\
              padding: 10px 20px;\
              border-radius: 5px;\
              margin-top: 20px;\
              display: inline-block;\
            }\
            a:hover {\
              background-color: #5c1a21;\
            }\
          </style>\
        </head>\
        <body>\
          <div class='container'>\
            <h1>Error in Prediction</h1>\
            <p>There was an issue sending the data to the server. Please try again later.</p>\
            <a href='/'>Back to Home</a>\
          </div>\
        </body>\
      </html>";
      
      // Send error page
      server.send(500, "text/html", errorPage);

      Serial.println("Error sending POST request to server");
    }

    // End HTTP connection
    http.end();
  } else {
    // Handle case where no data was received from Arduino
    String noDataPage = "<html>\
      <head>\
        <title>No Data Received</title>\
        <style>\
          body {\
            font-family: Arial, sans-serif;\
            background-color: #f8d7da;\
            text-align: center;\
            margin: 0;\
            padding: 0;\
          }\
          .container {\
            width: 70%;\
            margin: 30px auto;\
            padding: 30px;\
            background-color: white;\
            box-shadow: 0px 4px 12px rgba(0, 0, 0, 0.1);\
            border-radius: 8px;\
            color: #721c24;\
          }\
          h1 {\
            color: #721c24;\
          }\
          p {\
            font-size: 18px;\
            font-weight: bold;\
          }\
          a {\
            text-decoration: none;\
            color: #fff;\
            background-color: #721c24;\
            padding: 10px 20px;\
            border-radius: 5px;\
            margin-top: 20px;\
            display: inline-block;\
          }\
          a:hover {\
            background-color: #5c1a21;\
          }\
        </style>\
      </head>\
      <body>\
        <div class='container'>\
          <h1>No Data Received from Arduino</h1>\
          <p>It seems that no valid data was received from the Arduino. Please check the connections or try again later.</p>\
          <a href='/'>Back to Home</a>\
        </div>\
      </body>\
    </html>";

    // Send the error page
    server.send(400, "text/html", noDataPage);
  }
});


  // Endpoint to provide live data to the client
  server.on("/getData", HTTP_GET, []() {
    String jsonData = "{";
    jsonData += "\"temperature\": " + String(temperature) + ",";
    jsonData += "\"humidity\": " + String(humidity) + ",";
    jsonData += "\"pressure\": " + String(pressure) + ",";
    jsonData += "\"windspeed\": " + String(windspeed) + ",";
    jsonData += "\"rainPrediction\": " + String(rainPrediction);
    jsonData += "}";
    server.send(200, "application/json", jsonData);
  });

  server.begin();
  Serial.println("HTTP server started");
}

void loop() {
  // Continuously handle the HTTP requests
  server.handleClient();
  
  // Read serial data from Arduino and update the values
  if (Serial1.available() > 0) {
    String arduinoData = Serial1.readStringUntil('\n');  // Read the complete line from Arduino

    // Parse the received Arduino data (temperature, humidity, pressure, wind speed)
    int commaIndex1 = arduinoData.indexOf(',');
    int commaIndex2 = arduinoData.indexOf(',', commaIndex1 + 1);
    int commaIndex3 = arduinoData.indexOf(',', commaIndex2 + 1);

    if (commaIndex1 != -1 && commaIndex2 != -1 && commaIndex3 != -1) {
      temperature = arduinoData.substring(0, commaIndex1).toFloat();
      humidity = arduinoData.substring(commaIndex1 + 1, commaIndex2).toFloat();
      pressure = arduinoData.substring(commaIndex2 + 1, commaIndex3).toFloat();
      windspeed = arduinoData.substring(commaIndex3 + 1).toFloat();
    }

    // Optionally print to Serial Monitor for debugging
    // Serial.println("Parsed Data:");
    // Serial.println("Temperature: " + String(temperature));
    // Serial.println("Humidity: " + String(humidity));
    // Serial.println("Pressure: " + String(pressure));
    // Serial.println("Wind Speed: " + String(windspeed));
  }

  delay(100); // Ensure the ESP32 isn't overwhelmed and checks serial buffer periodically
}

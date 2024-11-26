import os
from flask import Flask, request, jsonify
import joblib

app = Flask(__name__)

# Load the model
model = joblib.load('random_forest_model.pkl')

@app.route('/')
def home():
    return "IOT Weather Prediction API"

@app.route('/predict', methods=['POST'])
def predict():
    data = request.get_json()
    try:
        # Extract features from JSON payload
        avg_temp = data['AvgTemp']
        humidity = data['Humidity9am']
        pressure = data['Pressure9am']
        wind_speed = data['WindSpeed9am']

        # Prepare data for prediction
        features = [[avg_temp, humidity, pressure, wind_speed]]
        prediction = model.predict(features)

        # Return prediction as JSON
        return jsonify({'RainTomorrow': int(prediction[0])})
    except KeyError as e:
        return jsonify({'error': f'Missing key: {e}'}), 400

if __name__ == '__main__':
    port = int(os.environ.get('PORT', 5000))
    app.run(host='0.0.0.0', port=port)

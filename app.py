import os
from flask import Flask, request, jsonify
import joblib

app = Flask(__name__)

# Lazy load the model to improve startup performance
model = None

def get_model():
    global model
    if model is None:
        model = joblib.load('optimized_random_forest_model.pkl')
    return model

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
        model = get_model()
        prediction = model.predict(features)

        # Return prediction as JSON
        return jsonify({'RainTomorrow': int(prediction[0])})
    except KeyError as e:
        return jsonify({'error': f'Missing key: {e}'}), 400
    except Exception as e:
        return jsonify({'error': str(e)}), 500

if __name__ == '__main__':
    # Dynamically use the port assigned by Render
    port = int(os.environ.get('PORT', 5000))
    app.run(host='0.0.0.0', port=port, debug=False)

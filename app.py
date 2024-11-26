from flask import Flask, request, jsonify
import joblib
import numpy as np

# Load the trained model
model = joblib.load('random_forest_model.pkl')

# Initialize the Flask app
app = Flask(__name__)

@app.route('/predict', methods=['POST'])
def predict():
    # Get the JSON data from the request
    data = request.get_json()

    try:
        # Extract input features from JSON
        AvgTemp = data['AvgTemp']
        Humidity9am = data['Humidity9am']
        Pressure9am = data['Pressure9am']
        WindSpeed9am = data['WindSpeed9am']

        # Prepare input data for prediction
        input_data = np.array([[AvgTemp, Humidity9am, Pressure9am, WindSpeed9am]])

        # Make prediction
        prediction = model.predict(input_data)

        # Convert numeric prediction to label
        result = 'Yes' if prediction[0] == 1 else 'No'

        # Return result as JSON
        return jsonify({'RainTomorrow': result})

    except KeyError as e:
        return jsonify({'error': f'Missing key: {e}'}), 400

# Run the Flask app
if __name__ == '__main__':
    app.run(debug=True)

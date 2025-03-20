"""
Flask Documentation:     https://flask.palletsprojects.com/
Jinja2 Documentation:    https://jinja.palletsprojects.com/
Werkzeug Documentation:  https://werkzeug.palletsprojects.com/
This file creates your application.
"""

# from crypt import methods
import site
import json
from app import app, Config,  mongo, Mqtt
from flask import escape, render_template, request, jsonify, send_file, redirect, make_response, send_from_directory
from json import dumps, loads
from werkzeug.utils import secure_filename
from datetime import datetime, timedelta, timezone
from os import getcwd
from os.path import join, exists
from time import time, ctime
from math import floor


#####################################
#   Routing for your application    #
#####################################


# 1. CREATE ROUTE FOR '/api/update'
@app.route('/api/update', methods=['POST'])
def update_radar():
    '''Updates the 'weather' collection'''
    if request.method == "POST":
        try:
            jsonDoc = request.get_json()
            # Update the document in the 'code' collection with the new passcode

            timestamp = datetime.now().timestamp()
            timestamp = floor(timestamp)
            jsonDoc['timestamp'] = timestamp

            Mqtt.publish("620164974", mongo.dumps(jsonDoc))
            # Mqtt.publish("620156144_pub",mongo.dumps(jsonDoc))
            Mqtt.publish("620164974_sub", mongo.dumps(jsonDoc))

            print(f"MQTT: {jsonDoc}")

            item = mongo.insertData(jsonDoc)
            if item:
                return jsonify({"status": "complete", "data": "complete"})
        except Exception as e:
            msg = str(e)
            print(f"update Error: {msg}")
        return jsonify({"status": "failed", "data": "failed"})

# 2. CREATE ROUTE FOR '/api/sensor/<sense>'


@app.route('/api/data', methods=['GET'])
def get_sense():
    '''Returns the field/variable <sense> in the 'weather' collection'''
    if request.method == "GET":
        try:
            result = mongo.retrieve_latest()
            if result:
                return jsonify({"status": "success", "data": result})
        except Exception as e:
            print(f"get_sense() error: {e}")

        return jsonify({"status": "failed", "data": "failed"})


@app.route('/api/data/<start>/<end>', methods=['GET'])
def get_data(start, end):
    startt = float(start)
    endt = float(end)
    if start is None or end is None:
        return jsonify({'status': 'error', 'message': 'Missing start or end parameter'}), 400

    # print(collection.find({'timestamp': {'$gte': startt, '$lte': endt}}))
    data = mongo.retrieve_sense_dates(startt, endt)
    ret = []

    for x in data:
        ret.append({key: value for key, value in x.items() if key != '_id'})

    return jsonify({'status': 'success', 'data': ret}), 200


@app.route('/api/test')
def test():
    return jsonify({'status': 'success', 'data': 'test'}), 200


@app.route('/api/test')
def send():
    return jsonify({"status": "success"})

###############################################################
# The functions below should be applicable to all Flask apps. #
###############################################################


@app.after_request
def add_header(response):
    """
    Add headers to both force latest IE rendering engine or Chrome Frame,
    and also tell the browser not to cache the rendered page. If we wanted
    to we could change max-age to 600 seconds which would be 10 minutes.
    """
    response.headers['X-UA-Compatible'] = 'IE=Edge,chrome=1'
    response.headers['Cache-Control'] = 'public, max-age=0'
    return response


@app.errorhandler(405)
def page_not_found(error):
    """Custom 404 page."""
    return jsonify({"status": 404}), 404

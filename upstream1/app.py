# upstream1/app.py

from flask import Flask, request, jsonify
import os

app = Flask(__name__)

@app.route('/hoge', methods=['POST'])
def handle_hoge():
    data = request.get_json()
    hoge = data.get('hoge')
    if hoge is None:
        return "Bad Request: 'hoge' field is missing.\n", 400
    if not isinstance(hoge, int):
        return "Bad Request: 'hoge' must be an integer.\n", 400
    # get HOSTNAME from env
    hostname = os.environ.get('UPSTREAM')
    return jsonify(message=f"Response from {hostname}"), 200

@app.route('/fuga', methods=['POST'])
def handle_fuga():
    data = request.get_json()
    hoge = data.get('hoge')
    if hoge is None:
        return "Bad Request: 'hoge' field is missing.\n", 400
    if not isinstance(hoge, int):
        return "Bad Request: 'hoge' must be an integer.\n", 400
    # get HOSTNAME from env
    hostname = os.environ.get('UPSTREAM')
    return jsonify(message=f"Response from {hostname}"), 200

@app.route('/', methods=['GET'])
def index():
    return "Upstream1 is running.\n", 200

if __name__ == '__main__':
    app.run(host='0.0.0.0', port=80)

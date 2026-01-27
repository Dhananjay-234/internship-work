from flask import Flask, render_template, request, jsonify
import json

app = Flask(__name__)

@app.route('/')
def index():
    with open("data.json", "r") as f:
        data = json.load(f)
    return render_template("index.html", data=data)

@app.route('/update', methods=['POST'])
def update():
    data = request.json
    with open("data.json", "w") as f:
        json.dump(data, f)
    return jsonify({"status": "ok"})

if __name__ == '__main__':
    app.run(host='0.0.0.0', port=5000, debug=True)

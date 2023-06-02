
from flask import Flask, jsonify, redirect, render_template, request
import requests

url = "http://127.0.0.1:5000"
local_vm_url = "http://sp23-cs340-152.cs.illinois.edu:5001/"
vm_test_url = "http://sp23-cs340-adm.cs.illinois.edu:34999/addMG"
vm_course_url = "http://sp23-cs340-adm.cs.illinois.edu:34000/addMG"

mg_info = {
    "name": "ssriv5staticGEN",
    "url": local_vm_url,
    "author": "Shray Srivastava ssriv5"
}

response = requests.put(vm_course_url, json=mg_info)

app = Flask(__name__)


@app.route('/', methods=["GET"])
def starter():
    return "starter",200

@app.route('/generate', methods=["GET"])
def static_generate():
    maze = ['9ac2aac','5422064','5024406','6240260','1442244','5226064','3224226']
    dic = {"geom": maze}
    
    return jsonify(dic),200


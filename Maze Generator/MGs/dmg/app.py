from flask import Flask, jsonify, redirect, render_template, request
import requests
import random

# DYNAMIC

url = "http://127.0.0.1:5000"
local_vm_url = "http://sp23-cs340-152.cs.illinois.edu:5002/"
vm_test_url = "http://sp23-cs340-adm.cs.illinois.edu:34999/addMG"
vm_course_url = "http://sp23-cs340-adm.cs.illinois.edu:34000/addMG"

mg_info = {
    "name": "ssriv5dynaGEN",
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

    maze1 = ['98a0cac','1640246','4264404','3204600','3400026','5620404','3260626']
    maze2 = ['9aa0aac','5044044','5266264','0246020','5422644','5666664','3220226']
    maze3 = ['9ac2aac','5422064','5224406','6000440','3644244','1262464','3224226']
    maze4 = ['baa0a8c','1244464','5644206','0260440','5000444','5000444','3220626']
    maze5 = ['9ae288c','5064204','5420604','0046020','5000646','5224206','3260666']
    maze6 = ['98a0a8c','1404204','1420604','0200020','1646444','1604464','3220226']
    maze7 = ['98a2a8c','1402404','1446404','0402400','1444404','1444604','3220226']
    maze8 = ['9882c8c','1446404','1402604','0402200','1402404','1446404','3620626']
    maze9 = ['9aa2aac','5022264','5400004','4400000','5422224','5222264','3220226']
    maze10 = ['9a808ac','5620664','1464604','0066200','1664624','5600464','3220226']
    maze11 = ['fee4eee','7664666','7664666','2220222','7664666','7664666','7664666']

    maze_options = [maze1, maze2, maze3, maze4, maze5, maze6
                    ,maze7, maze8, maze9, maze10, maze11]
    maze = random.choice(maze_options)
    
    return jsonify({"geom": maze}),200

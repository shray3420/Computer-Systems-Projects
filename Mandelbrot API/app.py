import json
from flask import Flask, jsonify, send_file, render_template, request, Response
import requests
import os
import io
import boto3
from botocore.exceptions import ClientError
import base64
import dotenv
import hashlib
import redis
from PIL import Image
app = Flask(__name__)

# s3 = boto3.client('s3')
# bucket = ''
INITIAL_URL = 'http://127.0.0.1:34000/mandelbrot/inferno/0.36:-0.09:0.01:512:1024'
INITIAL_STATE = {'colormap':'inferno', 'real':0.36, 'imag':-0.09, 'height':0.01, 'dim':512, 'iter':1024}
# {"key": INITIAL_URL, "image": "og.png"}
global count
count = 0
global cache 
# cache = []

# {'colormap':'inferno', 'real':0.36, 'imag':-0.09, 'height':0.01, 'dim':512, 'iter':1024}


# REDIS JKLMNOP



username = "ssriv5"


password1 = hashlib.md5(username.encode()).hexdigest()
password2 = hashlib.md5(password1.encode()).hexdigest()
# print(password)

r = redis.Redis(
    host="sp23-cs340-adm.cs.illinois.edu"
    ,port=6379
    ,password=password1
    ,username=username
)
r2 = redis.Redis(
    host="sp23-cs340-adm.cs.illinois.edu"
    ,port=6379
    ,password=password2
    ,username=password1
)
# r.auth(password, username)

# print("done")

# key_name = f"{password1}:page2"
# value = r2.get(key_name)
# print(value)
page_num = 3
while True:
    key_name = f"{password1}:page{page_num}"
    value = r2.get(key_name)
    if b"\xf0\x9f\x8e\x89" in value:
        print(page_num)
        print(value)
        break
    page_num += 1

# v = r.keys('*')
# print(value)


# GOLD RUSH JKLMNOP
# netid = 'ssriv5'
# net_hex = hashlib.sha256(netid.encode()).hexdigest()
# net_key = net_hex + '.txt'

# course = 'cs340'
# course_hex = hashlib.sha256(course.encode()).hexdigest()
# course_key = course_hex + '.txt'

# ec = boto3.client('s3', aws_access_key_id='AKIAQUUXEXCMMQ2EYQGD', aws_secret_access_key='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')

# ec.download_file('waf-uiuc-cs340', net_key,  "text.txt")
# ec.download_file('waf-uiuc-cs340', course_key,  "picture.png")


s3 = boto3.client('s3', endpoint_url='http://127.0.0.1:9000', 
                      aws_access_key_id='ROOTNAME', 
                      aws_secret_access_key='CHANGEME123')
bucket_name = 'mybuck'
try:
    s3.create_bucket(Bucket=bucket_name)
except:
    "do nothing"

# def mandel_req (url):
#     response = requests.get(url)
#     if response.status_code != 200:
#         return "error"
    
#     file_name =  str(len(cache)) + ".png"
#     try:
#         s3.upload_fileobj(file_name, bucket_name, url)
#     except ClientError as e:
#         print(f"error uploading file {file_name} to bucket {bucket_name}: {e}")
#         return "error"

#     # return file_name
#     # file_path = "images/" + str(len(cache)) + ".png"
#     # with open(file_path, 'wb') as temp:
#     #     temp.write(response.content)
#     # return file_path
    

@app.route('/')
def index():
    # print("hello")
    return render_template('index.html')

@app.route('/all')
def all():
    # print("hello")
    return render_template('all.html')

@app.route('/moveUp', methods=["POST"])
def moveUp():
    global INITIAL_STATE
    INITIAL_STATE['imag'] += INITIAL_STATE['height'] * .25
    return "success", 200

@app.route('/moveDown', methods=["POST"])
def moveDown():
    global INITIAL_STATE
    INITIAL_STATE['imag'] -= INITIAL_STATE['height'] * .25
    return "success", 200

@app.route('/moveLeft', methods=["POST"])
def moveLeft():
    global INITIAL_STATE
    INITIAL_STATE['real'] -= INITIAL_STATE['height'] * .25
    return "success", 200

@app.route('/moveRight', methods=["POST"])
def moveRight():
    global INITIAL_STATE
    INITIAL_STATE['real'] += INITIAL_STATE['height'] * .25
    return "success", 200

@app.route('/zoomIn', methods=["POST"])
def zoomIn():
    global INITIAL_STATE
    INITIAL_STATE['height'] = INITIAL_STATE['height'] * (1/1.4)
    return "success", 200

@app.route('/zoomOut', methods=["POST"])
def zoomOut():
    global INITIAL_STATE
    INITIAL_STATE['height'] = INITIAL_STATE['height'] * 1.4
    return "success", 200

@app.route('/smallerImage', methods=["POST"])
def smallerImage():
    global INITIAL_STATE
    INITIAL_STATE['dim'] = round(INITIAL_STATE['dim'] * (1/1.25))
    return "success", 200

@app.route('/largerImage', methods=["POST"])
def largerImage():
    global INITIAL_STATE
    INITIAL_STATE['dim'] = round(INITIAL_STATE['dim'] * 1.25)
    return "success", 200

@app.route('/moreIterations', methods=["POST"])
def moreIterations():
    global INITIAL_STATE
    INITIAL_STATE['iter'] = round(INITIAL_STATE['iter'] * 2)
    return "success", 200

@app.route('/lessIterations', methods=["POST"])
def lessIterations():
    global INITIAL_STATE
    INITIAL_STATE['iter'] = round(INITIAL_STATE['iter'] * .5)
    return "success", 200

@app.route('/changeColorMap', methods=["POST"])
def changeColorMap():
    global INITIAL_STATE
    INITIAL_STATE['colormap'] = request.json['colormap']
    return "success", 200

@app.route('/mandelbrot', methods=["GET"])
def mandelbrot():
    global cache
    
    url = 'http://127.0.0.1:34000/mandelbrot/{colormap}/{real}:{imag}:{height}:{dim}:{iter}'.format(**INITIAL_STATE)
    key = '{colormap}.{real}.{imag}.{height}.{dim}.{iter}'.format(**INITIAL_STATE)
    objects = s3.list_objects(Bucket=bucket_name)
        # print(len(objects))
    if 'Contents' in objects:
        for obj in objects['Contents']:
            if obj['Key'] == url + ".png":
                image = s3.get_object(Bucket=bucket_name,Key=obj['Key'])
                image_data = image["Body"].read()
                response = Response(image_data, mimetype="image/png")
                return response
    
    res = requests.get(url)
    if res == "error":
        return "Error connecting to mandelbrot micro", 400
    image_data = res.content
        
    s3.put_object(
        Bucket=bucket_name,
        Key=key + ".png",
        Body=image_data,
        ContentType="image/png",
    )
        # dic = {'key': url, 'image' : str(len(cache)) + ".png"}
        # cache.append(dic)
        
    # print (INITIAL_STATE)
    response = Response(image_data, mimetype="image/png")
    return response, 200     


    # try:
    #     image = s3.get_object(Bucket=bucket_name,Key=str(len(cache)) + ".png")
    #     image_data = image["Body"].read()
    #     response = Response(image_data, mimetype="image/png")
    #     return response, 200
    # except s3.exceptions.NoSuchKey:
        


@app.route('/storage', methods=["GET"])
def storage():
    # print(cache)
    try:
    # image = s3.get_object(Bucket=bucket_name,Key=obj['Key'])
    #             image_data = image["Body"].read()
    #             response = Response(image_data, mimetype="image/png")
    #             return response
        images = []
        objects = s3.list_objects(Bucket=bucket_name)
        if 'Contents' in objects:
            for obj in objects['Contents']:
                image = s3.get_object(Bucket=bucket_name,Key=obj['Key'])
                image_data = image['Body'].read()
                encoded_image_data = base64.b64encode(image_data).decode('utf-8')
                entry = {'key': obj['Key'], 'image': f"data:image/png;base64,{encoded_image_data}"}
                images.append(entry)

        return jsonify(images), 200
    except Exception as e:
        print(e)
        return jsonify([]), 400

@app.route('/resetTo', methods=["POST"])
def resetTo():
    global INITIAL_STATE
    data = request.get_json()
    INITIAL_STATE = {'colormap':data['colormap'], 
                   'real':float(data['real']), 
                   'imag':float(data['imag']), 
                   'height':float(data['height']), 
                   'dim':data['dim'], 
                   'iter':data['iter']}
    
    return "success", 200

@app.route('/getState', methods=["GET"])
def getState():
    global INITIAL_STATE
    return jsonify(INITIAL_STATE), 200

@app.route('/clearCache', methods=["GET"])
def clearCache():
    global count, cache
    objects = s3.list_objects(Bucket=bucket_name)
    print(len(objects))
    if 'Contents' in objects:
        for obj in objects['Contents']:
            s3.delete_object(Bucket=bucket_name, Key=obj['Key'])
    return "cache cleared", 200
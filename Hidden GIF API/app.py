from flask import Flask, render_template, send_file, request
import os

app = Flask(__name__)
global gif_count
gif_count = 0

# Route for "/" for a web-based interface to this micro-service:
@app.route('/')
def index():
  return render_template("index.html")


# Extract a hidden "uiuc" GIF from a PNG image:
@app.route('/extract', methods=["POST"])
def extract_hidden_gif():
  # ...
  global gif_count
  if not os.path.exists("gifs"):
    os.makedirs("gifs")
  if not os.path.exists("pngs"):
    os.makedirs("pngs")

  png = request.files["png"]
  if not png: 
    return "No file was uploaded", 500
  
  if not png.filename.endswith('.png'):
    return "File was not a PNG", 422
  
  png_file_path = "pngs/" + png.filename
  png.save(png_file_path)

  gif_path = "gifs/" + str(gif_count) + ".gif"
  
  # for now output.gif, later change to the gif folder
  command = "./png-extractGIF " + png_file_path + " " + gif_path
  terminal_response = os.system(command)

  print(terminal_response, command)
  if terminal_response == 0:
    gif_count = gif_count + 1
    return send_file(gif_path), 200
    
  else:
    return "PNG did not contain a GIF", 415

# Get the nth saved "uiuc" GIF:
@app.route('/extract/<int:image_num>', methods=['GET'])
def extract_image(image_num):
  global gif_count
  if image_num < gif_count:
    file = "gifs/" + str(image_num) + ".gif"
    return send_file(file), 200
  else:
    return "GIF not found", 404

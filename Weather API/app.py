from flask import Flask, render_template, request, jsonify
import os
import requests
from datetime import datetime, timedelta, time

app = Flask(__name__)

server_url = "http://127.0.0.1:34000"
weather_url = "https://api.weather.gov/gridpoints/ILX/95,71/forecast/hourly"
global cache
cache = {}

# Route for "/" (frontend):
@app.route('/')
def index():
  return render_template("index.html")

def get_next_meeting(course_info):
  days = course_info.get("Days of Week", "")
  start_time = course_info.get("Start Time", "")
  course = course_info.get("course", "")
  school_days = {"M": 0, "T": 1, "W": 2, "R": 3, "F": 4}
  next_meeting = None
  for day in days:
    if day in school_days:
      difference_in_days = school_days[day] - datetime.now().weekday()
      if difference_in_days <= 0:
        difference_in_days += 7
      meeting_time = datetime.combine(datetime.now().date() + timedelta(days=difference_in_days), datetime.strptime(start_time, "%I:%M %p").time())
      # if course == "TEST 999":
      #   return meeting_time + timedelta(days=7)
      if meeting_time >= datetime.now() and meeting_time.weekday() < 5:
        if next_meeting is None or meeting_time < next_meeting:
          next_meeting = meeting_time
  return next_meeting



# Route for "/weather" (middleware):
@app.route('/weather', methods=["POST"])
def POST_weather():
 
  course = request.form["course"]
  
  subject = None
  number = None
  if ' ' in course:
    parts = course.split()
    if len(parts) == 2 and parts[1].isdigit():
      subject = parts[0].upper()
      number = int(parts[1])
    else:
      return {}, 400
  else:
    for i in range(len(course)):
      if course[i].isdigit():
        subject = course[:i].upper()
        
        try:
          number = int(course[i:])
        except ValueError:
          return {}, 400
        break
  if subject is None or number is None:
    return {}, 400      

  response = requests.get(f"{server_url}/{subject}/{number}")
  if response.status_code == 404:
    return {} , 400
  
  course_info = response.json()
  # print(course_info)
  next_meeting = get_next_meeting(course_info).replace(tzinfo=None)

  data = {"course":course_info.get("course", "")}
  
  if next_meeting is None:
    return {}, 400
  
  data["nextCourseMeeting"] = next_meeting.strftime("%Y-%m-%d %H:%M:%S")
  forecast_time = next_meeting.replace(minute=0,second=0)
  data["forecastTime"] =  forecast_time.strftime("%Y-%m-%d %H:%M:%S")

  if (next_meeting - datetime.now()) > timedelta(days=6):  
    data["temperature"] = "forecast unavailable"
    data["shortForecast"] = "forecast unavailable"
    # print(data)
    return data, 200
    
  # data["temperature"] = "figure out later"
  # data["shortForecast"] = "figure out later"  

  # forecast_data = get_forecast(forecast_time)
  forecast_data = {}
  # if forecast_time in cache:
  #   forecast_data = cache[forecast_time]
  
  res = requests.get(weather_url)
  weather_data = res.json()["properties"]["periods"]
  for period in weather_data:
    forecast_number = period["number"]
    if forecast_number in cache:
      forecast_data = cache[forecast_number]
      break
    period_time = datetime.fromisoformat(period['startTime']).replace(tzinfo=None)
    if (period_time == forecast_time):
      forecast_data["temperature"] = period["temperature"]
      forecast_data["shortForecast"] = period["shortForecast"]
      
      # global cache
      cache[forecast_number] = forecast_data
      break
  if forecast_data == {}:
    data["temperature"] = "forecast unavailable"
    data["shortForecast"] = "forecast unavailable"
  else:
    data["temperature"] = forecast_data["temperature"]
    data["shortForecast"] = forecast_data["shortForecast"]

  
    
  
  # print(data)
  return data, 200


# Route for "/weatherCache" (middleware/backend):
@app.route('/weatherCache')
def get_cached_weather():
  # print(cache)
  if len(cache) == 0:
    return jsonify({})
  return cache

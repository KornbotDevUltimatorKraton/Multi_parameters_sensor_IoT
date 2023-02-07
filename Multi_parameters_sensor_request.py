import requests 
from itertools import count 

for i in count(0):
     try:
         res = requests.get("http://192.168.50.201:5670/npk_sensors")
         sensor_multi_parameters = res.json()
         print(sensor_multi_parameters)
     except:
          print("Server not found")

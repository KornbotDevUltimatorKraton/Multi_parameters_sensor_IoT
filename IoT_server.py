from flask import Flask,render_template,jsonify,request,redirect,url_for
npk_sensor = {}
app = Flask(__name__)

@app.route("/postplain",methods=['GET','POST'])
def devices_requests():

             inputjson = request.get_json(force=True)
             print(inputjson)
             return jsonify(inputjson)
@app.route("/soil_npk_sensor",methods=['GET','POST'])
def npk_requests():

             inputjson = request.get_json(force=True)
             npk_sensor[list(inputjson)[0]] = inputjson.get(list(inputjson)[0])
             return jsonify(inputjson)
@app.route("/npk_sensors")
def NPK():

     return jsonify(npk_sensor)

if __name__ == "__main__":
  
               app.run(debug=True,threaded=True,host="0.0.0.0",port=5670)

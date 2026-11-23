from flask import Flask
from flask_restful import Api, Resource
from datetime import datetime

app = Flask(__name__)

api = Api(app)

class LogReadings(Resource):
    def get(self, StationID, Sensor1, Sensor2, Sensor3,Verbose):
        now = datetime.now()
        with open('IoT_Data.txt', 'a') as f:
            str1 = "%s\t %s\t %6.3f\t %6.3f\t %6.3f\n" %(StationID, now.strftime("%m/%d/%Y, %H:%M:%S"), Sensor1, Sensor2, Sensor3)
            f.write(str1)
        f.close()
        if(Verbose ==1):
            return{"StationID" : StationID, "Sensor1": Sensor1, "Sensor2": Sensor2, "Sensor3": Sensor3, "Date_Time": now.strftime("%m/%d/%Y, %H:%M:%S") }
        else:
            return{ "Logged @" : now.strftime("%m/%d/%Y, %H:%M:%S")}
        

api.add_resource(
    LogReadings, 
    "/log/<string:StationID>/<float(signed=True):Sensor1>"
    "/<float:Sensor2>/<float:Sensor3>/<int:Verbose>"
    )

@app.route("/")
def home():
    try:
        with open("IoT_Data.txt", "r") as file:
            lines = [line for line in file if line.strip()]

        if not lines:
            return '<meta http-equiv="refresh" content="2">Waiting for Pico reports.'

        station, timestamp, door, ambient, password = (
            lines[-1].strip().split("\t")
        )

        door_text = "CLOSED" if float(door) == 1 else "OPEN"
        ambient_text = "OK" if float(ambient) == 1 else "NOK"
        password_text = "OK" if float(password) == 1 else "NOK"

        return f"""
        <html>
        <head>
            <title>Security System</title>
            <meta http-equiv="refresh" content="2">
        </head>
        <body>
            <h1>Group 3 Security System</h1>
            <p>Door: {door_text}</p>
            <p>Ambient: {ambient_text}</p>
            <p>Password: {password_text}</p>
            <p>Last report: {timestamp}</p>
        </body>
        </html>
        """

    except FileNotFoundError:
        return '<meta http-equiv="refresh" content="2">Waiting for Pico reports.'
    except ValueError:
        return "The latest log entry has an unexpected format."

if __name__ == "__main__":

    app.run(
        host="0.0.0.0", 
        port="8443", 
        debug=False,
        ssl_context=('server.crt', 'server.key')
        )


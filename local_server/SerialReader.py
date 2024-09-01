import serial
import sys
from datetime import datetime
import json
import mysql.connector
from time import time


'''
Tables

koa_access_control_gate_cards
+----------+-----------------------+---------------+---------------+--------+
| id       | label                 | valid_start   | valid_end     | active |
+----------+-----------------------+---------------+---------------+--------+



koa_access_control_gate_card_logs
+-------+----------+---------------+------+---------+------------------------+
| id    | card_id  | time          | type | success | message                |
+-------+----------+---------------+------+---------+------------------------+

send: card_id, time, 
'''

# Function used to read each character available to serial. 
def readSerial(ser, command):
    if ser.in_waiting > 0:  # Check if there is data in the buffer
        data = ser.read(ser.in_waiting)  # Read as much as available
        decoded_data = data.decode('utf-8')
        command += decoded_data
    return command

# Function used to process whenever a character is added to a command. 
def processCommand(ser, command):
    # JSON values. Checks if the new character is the end of a JSON object.
    if len(command) != 0 and command[len(command) - 1] == "}":
        json_file= json.loads(command)

        # Connect to server
        mydb = mysql.connector.connect(
            host="localhost",
            user="root",
            password="$Ft9chxvxW7",
            database="tidal_events"
        )

        # Check with database
        if (json_file["type"] == "scan"):
            print(json_file["value"])

            # SQL execute
            mycursor = mydb.cursor()
            mycursor.execute("SELECT id, valid_start, valid_end FROM koa_access_control_gate_cards where id='" + json_file["value"] + "' and active='1'") # Checks if active
            myresult = mycursor.fetchall()
            now = int(time() * 1000)
            
            # Check if there was a result
            # If there is no result.
            if (len(myresult) == 0):
                ser.write(int(0).to_bytes(1, byteorder='little'))
            # If the person should have access
            elif(now > myresult[0][2] and now < myresult[0][3]):
                ser.write(int(1).to_bytes(1, byteorder='little'))
            # If it is not valid. 
            else:
                ser.write(int(0).to_bytes(1, byteorder='little'))

        # Upload log
        elif (json_file["type"] == "log"):
            print(json_file["value"])

        command = ""

    # Regular log
    elif len(command) != 0 and command[len(command) - 1] == "\n":
        print(command)
        command = "" 

    return command

# Main method.
def main():
    try:
        ser = None
        while (True):
            # Infinite loop until a CTRL^C
            try:
                # Create serial and command associated with it. 
                # In Linux computers the port should be /dev/ttyUSB#
                # In Windows computers the port should be COM#
                # Where # is a port.
                ser = serial.Serial("/dev/ttyUSB0", 9600, timeout=1)
                command = ""
                while True:
                    command = readSerial(ser=ser, command=command)
                    command = processCommand(ser=ser, command=command)  
            
            # In case there is a connection error.
            except OSError:
                print("Lost Connection!")
                if (ser != None):
                    ser.close()

            # If the serial gives an unvalid character. 
            except UnicodeDecodeError:
                print("Invalid character")
                if (ser != None):
                    ser.close()

    # Close if key interupt.                
    except KeyboardInterrupt:
        print("\nProgram interrupted by user")
        sys.exit(0)

if __name__ == "__main__":
    main()
import serial
import sys
from datetime import datetime

f = open("output.txt", "a")
try:
    # Add date and time on the first line
    now = datetime.now().strftime("%d/%m/%Y %H:%M:%S")
    f.writelines("[" + now + "] ")
    with serial.Serial('COM4', 9600, timeout=1) as ser:  # Set a timeout of 1 second
        while True:
            if ser.in_waiting > 0:  # Check if there is data in the buffer
                data = ser.read(ser.in_waiting)  # Read as much as available
                decoded_data = data.decode('utf-8')
                f.write(decoded_data)

                 # Add date and time if a new line 
                if (decoded_data == '\n'):
                    now = datetime.now().strftime("%d/%m/%Y %H:%M:%S")
                    f.writelines("[" + now + "] ")
                
except KeyboardInterrupt:
    print("\nProgram interrupted by user")
    f.close()
    sys.exit(0)
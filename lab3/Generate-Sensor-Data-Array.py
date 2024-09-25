# Description: Generate a .h C file contianing an array intialized by sensor data found in a mock sensor scan measurment file.

# Created by: Phillip Jones (06/15/2024)
# Useage: Generate-Sensor-Data-Array.py -i Input-filename -o Output-filename

# General Python Reference/Tutorials: https://www.w3schools.com/python/

# import numpy as np  #Import/Include useful math and plotting functions
import os           # import function for finding absolute path to this python script
import getopt, sys  # Process commandline arguments (https://www.geeksforgeeks.org/command-line-arguments-in-python/?ref=ml_lbp )

# A little python magic to make it more convient for you to adjust where you want the data file to live
# Link for more info: https://towardsthecloud.com/get-relative-path-python 
absolute_path = os.path.dirname(__file__) # Absoult path to this python script
relative_path = "./"   # Path to sensor data file relative to this python script (./ means data file is in the same directory as this python script
full_path = os.path.join(absolute_path, relative_path) # Full path to sensor data file

logfile_filename = 'data.txt' # Default Input-filename of sensor data logfile
c_array_output_filename = 'data.h'  # Default Output-filename of .h file, which will contain an array that is initilized with the sensor data logfile values


###########################################
## 1. Process command line arguments     ##
###########################################

print("Note: use the -h option if you want to see the help for this program.\n");

argumentList = sys.argv[1:]  # Remove the 0th argument, i.e, the name of this python script

input_flag = 0
output_flag = 0

# Allowable Command line options
options = "hi:o:"  # -h for Help, -i for Input-Filename, -i for Output-Filename

# Corresponding Long -- version for each allowable option
long_options = ["Help", "Input_filename=", "Output_filename="]

# Catch erros during commandline processing
try:
    # Parsing argument
    arguments, values = getopt.getopt(argumentList, options, long_options)
    
    # checking each argument
    for currentArgument, currentValue in arguments:
        
        if currentArgument in ("-h", "--Help"):
            print ("This program takes as input a logfile of sensor data, and outputs a .h file")
            print ("containing a C array initalized with the logfile sensor data. \n")
            print ("Useage:" + os.path.basename(sys.argv[0]) + " -i Input-filename -o Output-filename \n")
	    
        elif currentArgument in ("-i", "--Input_filename"):
            logfile_filename = currentValue
            print ("Input Filename: ", logfile_filename)
            input_flag = 1
            
        elif currentArgument in ("-o", "--Output_filename"):
            c_array_output_filename = currentValue
            print ("Output Filename ", c_array_output_filename)
            output_flag = 1
			
except getopt.error as err:
    # output error, and return with an error code
    print (str(err))

# If a command line argument is not give, then use the default value
if input_flag == 0 :
    print ("Using defalut Input Filename: ", logfile_filename, ". Use the -i option if you want to specify an input filename")   
    
if output_flag == 0 :
    print ("Using default Output Filename: ", c_array_output_filename, ". Use the -o option if you want to specify an output filename \n")


######################################################################
## 2. Extract sensor data from logfile, and place in data structure ##
##    that will be use to create sensor array in a C .h file        ##
######################################################################

# Open file containing CyBot sensor scan from 0 - 180 degrees (Assumes first line of logfile is the header
file_object_logfile_filename = open(full_path + logfile_filename,'r') # Open the file: file_object is just a variable for the file "handler" returned by open()
file_header = file_object_logfile_filename.readline() # Read and store the header row (i.e., 1st row) of the file into file_header
file_data = file_object_logfile_filename.readlines()  # Read the rest of the lines of the file into file_data
file_object_logfile_filename.close() # Important to close file one you are done with it!!

# For each line of the file split into columns, and assign each column to a variable
angle_degrees_list = []
ping_distance_list = []

for line in file_data: 
    data = line.split()    # Split line into columns (by default delineates columns by whitespace)
    angle_degrees_list.append(data[0])  # Column 0 holds the angle in degrees at which distance was measured
    ping_distance_list.append(data[1])  # Column 1 holds the ping distance that was measured at a given angle       

print(angle_degrees_list)
print(ping_distance_list)


###################################################################
## 3. Create a .h header file that contains an array             ##
##    populated with sensor data form the input logfile          ##
###################################################################

# Open the file: file_object is just a variable for the file "handler" returned by open()           
file_object_c_array_file = open(full_path + c_array_output_filename,'w') 

# Link to how to grab parts of a file name: https://www.geeksforgeeks.org/python-program-to-get-the-file-name-from-the-file-path/
c_array_output_filename_base_ext = os.path.splitext(c_array_output_filename) # Grab basename and extension of filename
c_array_output_filename_basename = c_array_output_filename_base_ext[0]  # index 0 is the basename, index 1 is the extension

print(c_array_output_filename_basename)
# Replace "-" (dashes) with "_" (underscores) in basename.  This formate will be used in the .h file           
c_array_output_filename_basename = c_array_output_filename_basename.replace("-", "_")
print(c_array_output_filename_basename) 

# Write the first part of the .h file to gard that this .h file will only be included once
file_object_c_array_file.write("#ifndef " + c_array_output_filename_basename.upper() + "_H \n") 
file_object_c_array_file.write("#define " + c_array_output_filename_basename.upper() + "_H \n\n")

# Decalre one demensional array of floats
file_object_c_array_file.write("float sensor_data_array[" + str(len(angle_degrees_list)) + "] = { \n")

# Intialize array with sensor data from logfile 
for index, angle_degrees in enumerate(angle_degrees_list):
    if index > 0:  # Approach to have a "," after every element of the array initialization except the last. A little awkward. :-( Let me know if you know a cleaner approach. :-)
        file_object_c_array_file.write(", \n")
    file_object_c_array_file.write(ping_distance_list[index])

# End array initialization
file_object_c_array_file.write("\n}; \n\n")

# Write the last part of a .h file gard so that this .h file will only be included once
file_object_c_array_file.write("#endif \n") 

file_object_c_array_file.close() # Important to close file once you are done with it!!

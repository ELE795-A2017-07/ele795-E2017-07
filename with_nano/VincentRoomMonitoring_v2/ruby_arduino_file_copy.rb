#Ruby_arduino_file_copy.rb by Vincent Gosselin, copyright 2016.

#This simple script is made to get ride of the anoying Copy-Paste
#from sublime text into arduino IDE and texts editors to program the arduino.

#This is a script that copies sublime text files
#Arduino file (.ino), .cpp file and .h file into the respective 
#files.


#Source
source1 = File.readlines(Dir.pwd+"/VincentRoomMonitoring_v2.cpp")



#Destination files
destionation1 = "/Users/vincentgosselin/Documents/Arduino/VincentRoomMonitoring_v2/VincentRoomMonitoring_v2.ino"


#COPY-PASTE
File.open(destionation1, "w") do |f|
	f.puts source1
end


puts "COPY-PASTE DONE"




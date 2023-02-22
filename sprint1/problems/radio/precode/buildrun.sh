pkill radio
cd build;
cmake ..;
cmake --build .;
cd bin;
./radio "server" "127.0.0.1" 3400 &
./radio "client" "127.0.0.1" 3400 
echo  "End"


pkill game_server
cd build;
cmake .. ;
cmake --build . ;
cd bin
./game_server ../../data/config.json

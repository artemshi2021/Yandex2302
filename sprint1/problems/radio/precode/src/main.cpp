#include "audio.h"
#include <iostream>
#include <boost/asio.hpp>


namespace net = boost::asio;
using namespace std::literals;


typedef  boost::asio::ip::basic_endpoint<boost::asio::ip::udp>  udpadress;

void DoServerCode(const udpadress& adr);
void DoClientCode(const udpadress& adr);
const int Max_buffer_size = 65000;

class MainOperation
{
	int _port ;
	ProgType _tp ;
	udpadress _adr;
	std::chrono::duration<float> _sec = 1.05s ;
	public:
	bool Inited = false ;
	private:
	void Log (const std::string& msg)
	{
		auto now  = boost::posix_time::second_clock::local_time();
		std::cout << (  _tp == Server ? "Server:: " : "Client:: " ) << msg << "\n" ;
	}
	public:
	MainOperation(int argc, char** argv)
	{
		if ( argc < 2 )	{
			std::cerr << "No type of programm. Mast be client or server" ;
			return ;
		}
		std::string tpVal (argv[1]) ;
		_tp = tpVal == "client" ? Client : ( tpVal == "server" ? Server : Unknown ) ;
		if ( _tp == Unknown ){
			std::cerr << "No type of programm. Mast be client or server" ;
			return ;	
		}
		
		if ( argc < 3 )	{
			std::cerr << "No ip adress\n" ;
			return ;
		}
		
		if ( argc < 4 )	{
			std::cerr << "No port\n" ;
			return ;

		}
		
		try {
			_port = atoi ( argv[3] ) ;
		}
		catch (std::exception& e) {
			std::cerr << e.what() << std::endl;
			return ;
		}
		
		boost::system::error_code ec;
		using net::ip::udp;
		_adr = udp::endpoint(net::ip::make_address(argv[2], ec), _port);
		
		if (ec) {
			std::cout << "Wrong IP format"sv << std::endl;
			return ;
		}
		Inited = true ;
		Log ( "The begining. " ) ;
	
	}
	void Run()
	{
		if ( _tp == Client )//the begin
			DoClientCode(  ) ;
		else if ( _tp==Server )
			DoServerCode(  ) ;

		Log ( "The end" ) ;
	}
	void DoClientCode()
	{
		Recorder recorder( ma_format_u8 , 1 ) ;
		Player player(ma_format_u8, 1);
		boost::system::error_code ec;
		using boost::asio::ip::udp;
	     
		try {
			std::string str;

			std::cout << "Press Enter to record message..." << std::endl;
			std::getline(std::cin, str);

			Recorder::RecordingResult rec_result = recorder.Record(65000, _sec );
			Log ( "Recording done" );
					
			net::io_context io_context;
			udp::socket socket(io_context, udp::v4());
			
//			std::cout << "Size:: " << rec_result.frames  ;
//			player.PlayBuffer(rec_result.data.data(), rec_result.frames, 1.5s);
			
			socket.send_to( net::buffer(rec_result.data ) , _adr );	
		}
		catch (std::exception& e) {
			std::cerr << e.what() << std::endl;
	    }
	}
	void DoServerCode()
	{
		using net::ip::udp;
		Recorder recorder(ma_format_u8, 1);
		Player player(ma_format_u8, 1);
		boost::system::error_code ec;
		
		boost::asio::io_context io_context;
		udp::socket socket(io_context, _adr);
	     
	     	for ( ;; )
	     	{
			try {
				std::string str;
				std::array<char, Max_buffer_size> recv_buf;
				Log ("Listening" ) ;
				auto size = socket.receive_from(boost::asio::buffer(recv_buf), _adr );
				Log ("Message from client. size= " + std::to_string (size ) )  ;
				player.PlayBuffer( (char*)recv_buf.data(), size /*rec_result.frames*/, _sec);
				std::cout << "Playing done" << std::endl;
			}
			catch (std::exception& e) {
				std::cerr << e.what() << std::endl;
			}
		}
	}
} ;


int main(int argc, char** argv) {

	using net::ip::udp;
	MainOperation mn ( argc, argv ) ;

	if ( mn.Inited )
		mn.Run() ;

	
	return 0;
}



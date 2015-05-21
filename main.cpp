#include <websocketpp/config/asio_no_tls.hpp>
#include <websocketpp/server.hpp>
#include <websocketpp/common/thread.hpp>

#include <stdio.h>

typedef websocketpp::server<websocketpp::config::asio> server;
using websocketpp::connection_hdl;
using websocketpp::lib::placeholders::_1;
using websocketpp::lib::placeholders::_2;
using websocketpp::lib::bind;

using websocketpp::lib::thread;
using websocketpp::lib::mutex;
using websocketpp::lib::unique_lock;
using websocketpp::lib::condition_variable;


enum action_type {
    SUBSCRIBE,
    UNSUBSCRIBE,
    MESSAGE
};

struct action {
    action(action_type t, connection_hdl h) : type(t), hdl(h) {}
    action(action_type t, connection_hdl h, server::message_ptr m)
      : type(t), hdl(h), msg(m) {}

    action_type type;
    websocketpp::connection_hdl hdl;
    server::message_ptr msg;
};

int main()
{
    printf("Starting Server...\n");
	uint16_t port = 8888;
	server m_server;
	
	m_server.init_asio();
	m_server.listen(port);
	m_server.start_accept();
	
	try {
		m_server.run();
	} catch (const std::exception & e) {
		std::cout << e.what() << std::endl;
	}
    return 0;
}